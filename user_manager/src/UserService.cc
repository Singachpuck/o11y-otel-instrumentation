#include "notshell/user_manager/service/UserService.h"

#include <fmt/core.h>

#include "notshell/common/services.h"
#include "notshell/common/mappers/object_mappers.h"
#include "notshell/common/observability/metrics_commons.h"
#include "notshell/common/observability/tracer_commons.h"

namespace trace_api = opentelemetry::trace;
namespace otel_context = opentelemetry::context;

namespace commons = dochkas::notshell::common;

using namespace dochkas::notshell::user_manager;

UserServiceImpl::UserServiceImpl(std::shared_ptr<IUserDao> userDao) : userDao(userDao) {
    this->logger = obs::default_logger::get();

    {
        User u1;
        u1.name = "user1";
        u1.password = "password1";
        u1.created = boost::posix_time::second_clock::universal_time();

        User u2;
        u2.name = "user2";
        u2.password = "password2";
        u2.created = boost::posix_time::second_clock::universal_time();


        User u3;
        u3.name = "user3";
        u3.password = "password3";
        u3.created = boost::posix_time::second_clock::universal_time();

        userDao->createUser(std::make_unique<User>(u1));
        userDao->createUser(std::make_unique<User>(u2));
        userDao->createUser(std::make_unique<User>(u3));
    }

    this->stat.requestCounter = obs::getMeter()->CreateUInt64Counter(
        // name
        "RequestCounter",
        // description
        "Counts total number of request to User Manager Service",
        //unit
        "requests"
    );
}

grpc::Status UserServiceImpl::ReadUsers(::grpc::ServerContext* context, const ::foo::Empty* request, ::grpc::ServerWriter< ::foo::UserResponse>* writer) {
    EXTRACT_GRPC_CTX_AND_START_SPAN("GrpcUserService/ReadUsers", span, "UserService", "ReadUsers", context)
    this->stat.requestCounter->Add(1);

    BOOST_LOG_SEV(logger, logging::trivial::info) << "Client " << context->peer() << " requested all users.";

    std::uint32_t i = 0;
    auto users = this->userDao->getUsers();
    for (auto& u : *users) {
        foo::UserResponse userResponse;
        commons::convert(*u, userResponse);

        writer->Write(userResponse);
        i++;
    }

    BOOST_LOG_SEV(logger, logging::trivial::info) << "Retrieved " << i << " users.";

    span->SetStatus(trace_api::StatusCode::kOk);
    span->End();
    return grpc::Status::OK;
}

grpc::Status UserServiceImpl::ReadUser(::grpc::ServerContext* context, const ::foo::Id* request, ::foo::UserResponse* response) {
    EXTRACT_GRPC_CTX_AND_START_SPAN("GrpcUserService/ReadUser", span, "UserService", "ReadUser", context)
    this->stat.requestCounter->Add(1);

    BOOST_LOG_SEV(logger, logging::trivial::info) << "Client " << context->peer() << " requested User with id " << request->id() << ".";

    auto user = this->userDao->getUserById(request->id());

    commons::convert(*user, *response);

    BOOST_LOG_SEV(logger, logging::trivial::info) << "User " << request->id() << " found. Sending back response...";

    span->SetStatus(trace_api::StatusCode::kOk);
    span->End();
    return grpc::Status::OK;

    // for (auto& u : *this->userDao->getUsers()) {
    //     if (u->id == request->id()) {

    //     }
    // }

    // BOOST_LOG_SEV(logger, logging::trivial::info) << "User with id " << request->id() << " not found.";

    // span->SetStatus(trace_api::StatusCode::kError);
    // span->End();

    // return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, fmt::format("User with id {} not found.", request->id()));
}

grpc::Status UserServiceImpl::CreateUser(::grpc::ServerContext* context, const ::foo::UserRequest* request, ::foo::UserResponse* response) {
    EXTRACT_GRPC_CTX_AND_START_SPAN("GrpcUserService/CreateUser", span, "UserService", "CreateUser", context)
    this->stat.requestCounter->Add(1);

    auto user = std::make_unique<User>();
    commons::convert(*request, *user);
    
    auto newUser = this->userDao->createUser(std::move(user));

    commons::convert(*newUser, *response);

    BOOST_LOG_SEV(logger, logging::trivial::info) << "User " << response->id() << " created. Sending back response...";

    span->SetStatus(trace_api::StatusCode::kOk);
    span->End();
    return grpc::Status::OK;
}