#include "notshell/gateway/service/GrpcUserService.h"

#include <memory>

#include <grpc/grpc.h>
#include <grpcpp/client_context.h>

#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"

#include "notshell/common/mappers/object_mappers.h"
#include "notshell/common/model/dto/UserDto.h"

#include "notshell/common/observability/tracer_commons.h"

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace nostd = opentelemetry::nostd;

namespace commons = dochkas::notshell::common;
namespace model = dochkas::notshell::common::model;

NOTSHELL_NAMESPACE_BEGIN

namespace gateway {
    GrpcUserService::GrpcUserService(std::shared_ptr<grpc::Channel> channel) : stub_(foo::UserService::NewStub(channel)) {}

    std::shared_ptr<model::UsersDto> GrpcUserService::readUsers() {
        grpc::ClientContext context;
        INJECT_GRPC_CTX_AND_START_SPAN("GrpcUserService/readUser", span, "UserService", "ReadUser", context)

        auto users = std::make_shared<model::UsersDto>();
        foo::Empty empty;
        foo::UserResponse userResponse;
        std::unique_ptr<grpc::ClientReader<foo::UserResponse>> reader(
            stub_->ReadUsers(&context, empty));
        
        while (reader->Read(&userResponse)) {
            auto userDto = std::make_shared<model::UserDto>();

            commons::convert(userResponse, *userDto);

            users->users.push_back(userDto);
        }
        grpc::Status status = reader->Finish();
        if (status.ok()) {
            span->SetStatus(trace_api::StatusCode::kOk);
            BOOST_LOG_SEV(logger, logging::trivial::info) << "ReadUsers rpc succeeded.";
        } else {
            span->SetStatus(trace_api::StatusCode::kError);
            BOOST_LOG_SEV(logger, logging::trivial::info) << "ReadUsers rpc failed.";
        }

        return users;
    }

    std::shared_ptr<model::UserDto> GrpcUserService::readUser(std::uint64_t id) {
        grpc::ClientContext context;
        INJECT_GRPC_CTX_AND_START_SPAN("GrpcUserService/readUser", span, "UserService", "ReadUser", context)

        foo::Id idRequest;
        foo::UserResponse userResponse;
        idRequest.set_id(id);
        grpc::Status status = stub_->ReadUser(&context, idRequest, &userResponse);
        if (!status.ok()) {
            span->SetStatus(trace_api::StatusCode::kError);
            BOOST_LOG_SEV(logger, logging::trivial::info) << "ReadUser rpc failed.";
            return nullptr;
        }

        BOOST_LOG_SEV(logger, logging::trivial::info) << "ReadUser rpc succeeded.";
        
        auto user = std::make_shared<model::UserDto>();

        commons::convert(userResponse, *user);

        span->SetStatus(trace_api::StatusCode::kOk);
        return user;
    }

    std::shared_ptr<model::UserDto> GrpcUserService::createUser(std::unique_ptr<model::UserDto> user) {
        grpc::ClientContext context;
        INJECT_GRPC_CTX_AND_START_SPAN("GrpcUserService/createUser", span, "UserService", "CreateUser", context)

        foo::UserRequest userRequest;
        commons::convert(*user, userRequest);

        foo::UserResponse userResponse;
        
        grpc::Status status = stub_->CreateUser(&context, userRequest, &userResponse);
        if (!status.ok()) {
            span->SetStatus(trace_api::StatusCode::kError);
            BOOST_LOG_SEV(logger, logging::trivial::info) << "CreateUser rpc failed.";
            return nullptr;
        }

        BOOST_LOG_SEV(logger, logging::trivial::info) << "CreateUser rpc succeeded.";
        
        auto userDto = std::make_shared<model::UserDto>();

        commons::convert(userResponse, *userDto);

        span->SetStatus(trace_api::StatusCode::kOk);
        return userDto;
    }
}
NOTSHELL_NAMESPACE_END