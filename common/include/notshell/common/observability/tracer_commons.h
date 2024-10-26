#pragma once

#include <grpcpp/server_context.h>
#include <grpcpp/client_context.h>

#include "opentelemetry/trace/provider.h"
#include "opentelemetry/trace/semantic_conventions.h" 
#include "opentelemetry/trace/propagation/http_trace_context.h"
#include "opentelemetry/context/propagation/global_propagator.h"

#include "project_config.h"

namespace trace_api = opentelemetry::trace;

#define START_SPAN_BEGIN(span_var_name, startSpanOptions_var_name) \
    opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span> span_var_name; \
    { \
        opentelemetry::trace::StartSpanOptions startSpanOptions_var_name;

#define INJECT_GRPC_CTX(context) \
    {\
        dochkas::notshell::common::observability::GrpcClientCarrier carrier(&context); \
        auto current_ctx = opentelemetry::context::RuntimeContext::GetCurrent(); \
        opentelemetry::context::propagation::GlobalTextMapPropagator::GetGlobalPropagator() \
            ->Inject(carrier, current_ctx); \
    }

#define EXTRACT_GRPC_CTX(context, startSpanOptions_var_name) \
        startSpanOptions_var_name.kind = opentelemetry::trace::SpanKind::kServer; \
        auto current_ctx = opentelemetry::context::RuntimeContext::GetCurrent(); \
        auto new_context = opentelemetry::context::propagation::GlobalTextMapPropagator::GetGlobalPropagator() \
            ->Extract(dochkas::notshell::common::observability::GrpcServerCarrier(context), current_ctx); \
        startSpanOptions_var_name.parent   = opentelemetry::trace::GetSpan(new_context)->GetContext();

#define START_SPAN_END(span_name, span_var_name, rpc_service, rpc_method, startSpanOptions_var_name) \
        span_var_name = dochkas::notshell::common::observability::getTracer()->StartSpan(span_name, \
                                                    {{opentelemetry::trace::SemanticConventions::kRpcSystem, "grpc"}, \
                                                    {opentelemetry::trace::SemanticConventions::kRpcService, rpc_service}, \
                                                    {opentelemetry::trace::SemanticConventions::kRpcMethod, rpc_method}, \
                                                    {opentelemetry::trace::SemanticConventions::kRpcGrpcStatusCode, 0}}, \
                                                    startSpanOptions_var_name); \
    } \
    auto span_var_name##_scope = opentelemetry::trace::Scope(span_var_name);

#define INJECT_GRPC_CTX_AND_START_SPAN(span_name, span_var_name, rpc_service, rpc_method, context) \
    START_SPAN_BEGIN(span_var_name, startSpanOptions) \
    startSpanOptions.kind = opentelemetry::trace::SpanKind::kClient; \
    START_SPAN_END(span_name, span_var_name, rpc_service, rpc_method, startSpanOptions) \
    INJECT_GRPC_CTX(context)

#define EXTRACT_GRPC_CTX_AND_START_SPAN(span_name, span_var_name, rpc_service, rpc_method, context) \
    START_SPAN_BEGIN(span_var_name, startSpanOptions) \
    EXTRACT_GRPC_CTX(context, startSpanOptions) \
    START_SPAN_END(span_name, span_var_name, rpc_service, rpc_method, startSpanOptions)


NOTSHELL_NAMESPACE_BEGIN

namespace common {
namespace observability {

    class GrpcClientCarrier : public opentelemetry::context::propagation::TextMapCarrier
    {
    public:
        GrpcClientCarrier(grpc::ClientContext *context);
        GrpcClientCarrier() = default;

        virtual opentelemetry::nostd::string_view Get(opentelemetry::nostd::string_view /* key */) const noexcept override;

        virtual void Set(opentelemetry::nostd::string_view key, opentelemetry::nostd::string_view value) noexcept override;

        grpc::ClientContext *context_;
    };

    class GrpcServerCarrier : public opentelemetry::context::propagation::TextMapCarrier
    {
    public:
        GrpcServerCarrier(grpc::ServerContext *context);
        GrpcServerCarrier() = default;

        virtual opentelemetry::nostd::string_view Get(opentelemetry::nostd::string_view key) const noexcept override;

        virtual void Set(opentelemetry::nostd::string_view /* key */, opentelemetry::nostd::string_view /* value */) noexcept override;

        grpc::ServerContext *context_;
    };
    
    void initTracer(const char* tracerName);

    const std::string& getTracerName();

    inline opentelemetry::nostd::shared_ptr<trace_api::Tracer> getTracer()
    {
        return trace_api::Provider::GetTracerProvider()->GetTracer(getTracerName());
    }
}
}
NOTSHELL_NAMESPACE_END
