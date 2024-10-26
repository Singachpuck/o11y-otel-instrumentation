#include "notshell/common/observability/logging_commons.h"
#include "notshell/common/observability/tracer_commons.h"
#include "notshell/common/observability/metrics_commons.h"

#include <boost/core/null_deleter.hpp>
#include <boost/log/core.hpp>
#include <boost/log/utility/formatting_ostream.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/expressions.hpp>
#include <boost/type_traits/is_arithmetic.hpp>
#include <boost/algorithm/string.hpp>

#include "opentelemetry/common/attribute_value.h"

#include "opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_factory.h"
#include "opentelemetry/logs/provider.h"
#include "opentelemetry/sdk/logs/simple_log_record_processor.h"
#include "opentelemetry/sdk/logs/simple_log_record_processor_factory.h"
#include "opentelemetry/sdk/logs/logger_provider_factory.h"

#include "opentelemetry/exporters/ostream/span_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_exporter_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/simple_processor_factory.h"
#include "opentelemetry/sdk/trace/batch_span_processor_options.h"
#include "opentelemetry/sdk/trace/batch_span_processor_factory.h"
#include "opentelemetry/sdk/trace/tracer_provider_factory.h"

#include "opentelemetry/exporters/ostream/metric_exporter_factory.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_factory.h"
#include "opentelemetry/sdk/metrics/aggregation/aggregation_config.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_factory.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_options.h"
#include "opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/meter_context.h"
#include "opentelemetry/sdk/metrics/meter_context_factory.h"
#include "opentelemetry/sdk/metrics/metric_reader.h"
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"
#include "opentelemetry/sdk/metrics/view/view_registry.h"
#include "opentelemetry/sdk/metrics/view/view_registry_factory.h"

#include "notshell/common/config_parser.h"

namespace sinks = boost::log::sinks;

namespace nostd = opentelemetry::nostd;
namespace otlp = opentelemetry::exporter::otlp;
namespace trace_api = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace trace_exporter = opentelemetry::exporter::trace;
namespace otel_context = opentelemetry::context;
namespace metrics_api = opentelemetry::metrics;
namespace metrics_sdk = opentelemetry::sdk::metrics;
namespace common = opentelemetry::common;
namespace exportermetrics = opentelemetry::exporter::metrics;
namespace logs_sdk = opentelemetry::sdk::logs;
namespace logs_api = opentelemetry::logs;

namespace config = dochkas::notshell::common::config;


NOTSHELL_NAMESPACE_BEGIN

namespace common::observability {

    static const auto OTEL_RESOURCE =
        opentelemetry::sdk::resource::Resource::Create({{ "service.name", config::CURRENT_SERVICE_NAME }});

    TraceIdAttrImpl::TraceIdAttrImpl(std::string tracerName) : tracerName(tracerName) {}

    logging::attribute_value TraceIdAttrImpl::get_value()
    {
        auto&& tracer = trace_api::Provider::GetTracerProvider()->GetTracer(this->tracerName);

        char trace_id[OTEL_TRACE_LENGTH] = {0};
        tracer->GetCurrentSpan()->GetContext().trace_id().ToLowerBase16(trace_id);

        return attrs::make_attribute_value(std::string(trace_id, OTEL_TRACE_LENGTH));
    }

    TraceIdAttr::TraceIdAttr(std::string tracerName) : logging::attribute(new TraceIdAttrImpl(tracerName)) {}

    TraceIdAttr::TraceIdAttr(attrs::cast_source const& source) : logging::attribute(source.as< TraceIdAttrImpl >()) {}

    SpanIdAttrImpl::SpanIdAttrImpl(std::string tracerName) : tracerName(tracerName) {}

    logging::attribute_value SpanIdAttrImpl::get_value()
    {
        auto&& tracer = trace_api::Provider::GetTracerProvider()->GetTracer(this->tracerName);

        char span_id[OTEL_SPAN_LENGTH] = {0};
        tracer->GetCurrentSpan()->GetContext().span_id().ToLowerBase16(span_id);

        return attrs::make_attribute_value(std::string(span_id, OTEL_SPAN_LENGTH));
    }

    SpanIdAttr::SpanIdAttr(std::string tracerName) : logging::attribute(new SpanIdAttrImpl(tracerName)) {}

    SpanIdAttr::SpanIdAttr(attrs::cast_source const& source) : logging::attribute(source.as< SpanIdAttrImpl >()) {}

    void SpanFeeder::consume(logging::record_view const& rec, string_type const& command_line) {
        getTracer()->GetCurrentSpan()->AddEvent(rec["Message"].extract<std::string>().get());
    }

    std::unordered_map<
            std::string,
            std::function< void(std::ostream&, logging::attribute_value const&) >
        > formatters {};

    #define JSON_INDENT_OR_NO(mode, indent_symbol) (mode == JsonFormat::FORMATTED ? indent_symbol : "")

    /**
     * Note: Array and nested object are not supported.
     */
    template< typename T >
    void to_json_node(std::ostream& strm, logging::attribute_value const& attr)
    {
        auto val = logging::extract< T >(attr);
        if (val) {
            if (boost::is_arithmetic<T>::value) {
                strm << val.get();
            } else {
                std::stringstream ss;
                ss << val.get();
                strm << std::quoted(ss.str());
            }
        }
    }

    void json_attrs_formatter(const logging::record_view &view, logging::formatting_ostream &os, JsonFormat mode) {
        os << "{" << JSON_INDENT_OR_NO(mode, "\n");
        bool first = true;
        for (auto& attribute : view.attribute_values()) {
            auto it = formatters.find(attribute.first.string());
            if (it != formatters.end()) {
                if (!first) {
                    os << "," << JSON_INDENT_OR_NO(mode, "\n");
                }
                os << JSON_INDENT_OR_NO(mode, "\t") << std::quoted(attribute.first.string()) << ":" << JSON_INDENT_OR_NO(mode, " ");
                std::stringstream ss;
                it->second(ss, attribute.second);
                os << ss.str();
                first = false;
            }
        }

        os << JSON_INDENT_OR_NO(mode, "\n") << "}\n";
    }

    BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", logging::trivial::severity_level)
    BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
    BOOST_LOG_ATTRIBUTE_KEYWORD(lineId, "LineID", std::uint32_t)
    BOOST_LOG_ATTRIBUTE_KEYWORD(traceId, "TraceId", std::string)
    BOOST_LOG_ATTRIBUTE_KEYWORD(spanId, "SpanId", std::string)

    BOOST_LOG_GLOBAL_LOGGER_INIT(default_logger, src::severity_logger<logging::trivial::severity_level>) {
        src::severity_logger<logging::trivial::severity_level> lg;

        lg.add_attribute(traceId.get_name(), TraceIdAttr(getTracerName()));
        lg.add_attribute(spanId.get_name(), SpanIdAttr(getTracerName()));

        formatters[timestamp.get_name().string()] = &to_json_node<boost::posix_time::ptime>;
        formatters[severity.get_name().string()] = &to_json_node<logging::trivial::severity_level>;
        formatters[lineId.get_name().string()] = &to_json_node<std::uint32_t>;
        formatters[traceId.get_name().string()] = &to_json_node<std::string>;
        formatters[spanId.get_name().string()] = &to_json_node<std::string>;
        formatters[logging::expressions::smessage.get_name().string()] = &to_json_node<std::string>;

        return lg;
    }

    void initLogging() {
        logging::add_common_attributes();

        auto text_formatter = logging::expressions::stream
                    << "[" << timestamp << "] "
                    << "[Trace: " << traceId << "] "
                    << "[Span: " << spanId << "] "
                    << "|" << severity << "| "
                    << logging::expressions::smessage;
        

        auto json_formatted = std::bind(&json_attrs_formatter, std::placeholders::_1, std::placeholders::_2, JsonFormat::FORMATTED);
        auto json_no_indent = std::bind(&json_attrs_formatter, std::placeholders::_1, std::placeholders::_2, JsonFormat::NO_INDENT);

        typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
        typedef sinks::synchronous_sink< sinks::text_file_backend  > file_sink;
        typedef sinks::synchronous_sink< SpanFeeder  > span_sink;

        auto obsConfig =  config::get().getO11yConf();

        YAML::Node localNode = obsConfig[config::CURRENT_SERVICE_NAME]["logs"]["local"];
        YAML::Node fileNode = obsConfig[config::CURRENT_SERVICE_NAME]["logs"]["file"];
        YAML::Node spanNode = obsConfig[config::CURRENT_SERVICE_NAME]["logs"]["span"];
        if (localNode["active"] && localNode["active"].as<bool>()) {
            boost::shared_ptr< text_sink > clog_sink = boost::make_shared< text_sink >();

            if (localNode["format"] && boost::iequals("json", localNode["format"].as<std::string>())) {
                clog_sink->set_formatter(json_no_indent);
            } else if (localNode["format"] && boost::iequals("json-formatted", localNode["format"].as<std::string>())) {
                clog_sink->set_formatter(json_formatted);
            } else {
                clog_sink->set_formatter(text_formatter);
            }

            boost::shared_ptr<std::ostream> stream{&std::clog, boost::null_deleter()};
            clog_sink->locked_backend()->add_stream(stream);
            logging::core::get()->add_sink(clog_sink);
        }
        
        if (fileNode["active"] && fileNode["active"].as<bool>()) {
            boost::shared_ptr< file_sink > f_sink = boost::make_shared< file_sink >(
                logging::keywords::file_name = fileNode["path"].as<std::string>(),
                logging::keywords::rotation_size = 1048576
            );

            if (fileNode["format"] && boost::iequals("json", fileNode["format"].as<std::string>())) {
                f_sink->set_formatter(json_no_indent);
            } else if (fileNode["format"] && boost::iequals("json-formatted", fileNode["format"].as<std::string>())) {
                f_sink->set_formatter(json_formatted);
            } else {
                f_sink->set_formatter(text_formatter);
            }

            // For demo purposes
            f_sink->locked_backend()->auto_flush(true);

            logging::core::get()->add_sink(f_sink);
        }

        if (spanNode["active"] && spanNode["active"].as<bool>()) {
            boost::shared_ptr<span_sink> spanFeeder = boost::make_shared<span_sink>();
            logging::core::get()->add_sink(spanFeeder);
        }

        // Required configured OTLP Log Exporter so that app doesn't crush
        otlp::OtlpGrpcLogRecordExporterOptions opts;
        auto exporter  = otlp::OtlpGrpcLogRecordExporterFactory::Create(opts);
        // auto processor = logs_sdk::SimpleLogRecordProcessorFactory::Create(std::move(exporter));
        // nostd::shared_ptr<logs_api::LoggerProvider> provider(
        // logs_sdk::LoggerProviderFactory::Create(std::move(processor)));
        // logs_api::Provider::SetLoggerProvider(provider);
    }

    ///////////////////////////////////// TRACING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

    GrpcClientCarrier::GrpcClientCarrier(grpc::ClientContext *context) : context_(context) {}

    opentelemetry::nostd::string_view GrpcClientCarrier::Get(opentelemetry::nostd::string_view /* key */) const noexcept
    {
        return "";
    }

    void GrpcClientCarrier::Set(opentelemetry::nostd::string_view key, opentelemetry::nostd::string_view value) noexcept
    {   
        context_->AddMetadata(std::string(key), std::string(value));
    }


    GrpcServerCarrier::GrpcServerCarrier(grpc::ServerContext *context) : context_(context) {}

    opentelemetry::nostd::string_view GrpcServerCarrier::Get(opentelemetry::nostd::string_view key) const noexcept
    {
        auto it = context_->client_metadata().find({key.data(), key.size()});
        if (it != context_->client_metadata().end())
        {
            return it->second.data();
        }
        return "";
    }

    void GrpcServerCarrier::Set(opentelemetry::nostd::string_view /* key */, opentelemetry::nostd::string_view /* value */) noexcept
    {
        // Not required for server
    }
    

    static std::string TRACER_NAME;

    void initTracer(const char* tracerName)
    {
        TRACER_NAME = tracerName;

        auto processors = std::vector<std::unique_ptr<trace_sdk::SpanProcessor>> {};

        auto& obsConfig = config::get().getO11yConf();

        YAML::Node localNode = obsConfig[config::CURRENT_SERVICE_NAME]["traces"]["local"];
        YAML::Node exporterNode = obsConfig[config::CURRENT_SERVICE_NAME]["traces"]["export"];
        if (localNode["active"] && localNode["active"].as<bool>()) {
            auto exporter = trace_exporter::OStreamSpanExporterFactory::Create();

            // SimpleProcessor for demo purposes, use BatchProcessor instead
            auto processor = trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));

            processors.push_back(std::move(processor));
        }

        if (exporterNode["active"] && exporterNode["active"].as<bool>()) { 
            // TODO: exporter configuration
            if (exporterNode["mode"] && !boost::iequals("grpc", exporterNode["mode"].as<std::string>())) {
                throw std::runtime_error("Unsupported exporting mode: " + exporterNode["mode"].as<std::string>());
            }

            // trace_sdk::BatchSpanProcessorOptions bspOpts{};
            opentelemetry::exporter::otlp::OtlpGrpcExporterOptions opts;
            opts.endpoint = exporterNode["endpoint"].as<std::string>();
            opts.use_ssl_credentials = false;
            // opts.ssl_credentials_cacert_as_string = "ssl-certificate";
            auto exporter  = opentelemetry::exporter::otlp::OtlpGrpcExporterFactory::Create(opts);
            // Consider changing to BatchProcessor
            auto processor = trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));
            // auto processor = trace_sdk::BatchSpanProcessorFactory::Create(std::move(exporter), bspOpts);

            processors.push_back(std::move(processor));
        }

        std::shared_ptr<opentelemetry::trace::TracerProvider> provider =
            trace_sdk::TracerProviderFactory::Create(std::move(processors), OTEL_RESOURCE);

        // set the global trace provider
        trace_api::Provider::SetTracerProvider(provider);

        otel_context::propagation::GlobalTextMapPropagator::SetGlobalPropagator(
            std::shared_ptr<otel_context::propagation::TextMapPropagator>(
                new trace_api::propagation::HttpTraceContext())
        );
    }

    const std::string& getTracerName() {
        return TRACER_NAME;
    }


    ///////////////////////////////// METRICS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

    void initMetrics() {
        auto views = metrics_sdk::ViewRegistryFactory::Create();
        auto context = metrics_sdk::MeterContextFactory::Create(std::move(views), OTEL_RESOURCE);
        auto provider = std::make_shared<metrics_sdk::MeterProvider>(std::move(context));

        auto obsConfig = config::get().getO11yConf();

        YAML::Node localNode = obsConfig[config::CURRENT_SERVICE_NAME]["metrics"]["local"];
        YAML::Node exporterNode = obsConfig[config::CURRENT_SERVICE_NAME]["metrics"]["export"];
        if (localNode["active"] && localNode["active"].as<bool>()) {
            auto exporter = exportermetrics::OStreamMetricExporterFactory::Create();

            // Initialize and set the global MeterProvider
            metrics_sdk::PeriodicExportingMetricReaderOptions options;
            options.export_interval_millis = std::chrono::milliseconds(localNode["interval"].as<uint32_t>(5000));
            options.export_timeout_millis = std::chrono::milliseconds(500);

            auto reader =
                metrics_sdk::PeriodicExportingMetricReaderFactory::Create(std::move(exporter), options);
            provider->AddMetricReader(std::move(reader));
        }

        if (exporterNode["active"] && exporterNode["active"].as<bool>()) {
            // // TODO: exporter configuration
            if (exporterNode["mode"] && !boost::iequals("grpc", exporterNode["mode"].as<std::string>())) {
                throw std::runtime_error("Unsupported exporting mode: " + exporterNode["mode"].as<std::string>());
            }

            otlp::OtlpGrpcMetricExporterOptions opts;
            opts.endpoint = exporterNode["endpoint"].as<std::string>();
            opts.use_ssl_credentials = false;
            // opts.ssl_credentials_cacert_as_string = "ssl-certificate";
            auto exporter = otlp::OtlpGrpcMetricExporterFactory::Create(opts);
            metrics_sdk::PeriodicExportingMetricReaderOptions reader_options;
            reader_options.export_interval_millis = std::chrono::milliseconds(exporterNode["interval"].as<uint32_t>(5000));
            reader_options.export_timeout_millis  = std::chrono::milliseconds(500);
            auto reader = metrics_sdk::PeriodicExportingMetricReaderFactory::Create(std::move(exporter), reader_options);
            provider->AddMetricReader(std::move(reader));
        }

        std::shared_ptr<opentelemetry::metrics::MeterProvider> api_provider(std::move(provider));
        metrics_api::Provider::SetMeterProvider(api_provider);
    }

}
NOTSHELL_NAMESPACE_END
