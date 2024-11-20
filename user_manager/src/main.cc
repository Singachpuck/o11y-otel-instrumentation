#include <iostream>
#include <fmt/core.h>
#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include "project_config.h"
#include "notshell/common/config_parser.h"
#include "notshell/common/observability/tracer_commons.h"
#include "notshell/common/observability/logging_commons.h"
#include "notshell/common/observability/metrics_commons.h"
#include "notshell/common/services.h"
#include "notshell/user_manager/dao/InMemoryUserDao.h"
#include "notshell/user_manager/dao/OdbUserDao.h"
#include "notshell/user_manager/service/UserService.h"

#include <odb/pgsql/database.hxx>

namespace commons = dochkas::notshell::common;
namespace obs = dochkas::notshell::common::observability;
namespace config = dochkas::notshell::common::config;

using namespace dochkas::notshell::user_manager;

// Must have in each main.cpp
const char* config::CURRENT_SERVICE_NAME = USER_SERVICE_NAME;


int main(int argc, char* argv[])
{
    {
        auto cl_result = config::parse_command_line_args(argc, argv,
            fmt::format("{}/{}", PROJECT_NAME, USER_SERVICE_NAME),
            "User manager service.");

        config::parse_observability_config(cl_result);
        config::parse_pgsql_config(cl_result);
    }

    obs::initTracer(config::CURRENT_SERVICE_NAME);
    obs::initLogging();
    obs::initMetrics();

    auto& logger = obs::default_logger::get();
    BOOST_LOG_SEV(logger, logging::trivial::info) << "User Manager works!";

    // std::shared_ptr<odb::database> db = commons::initDatabase();
    // std::auto_ptr<odb::database> db (new odb::pgsql::database (argc, argv));
    commons::initDatabase();
    auto userDao = std::make_shared<InMemoryUserDao>();
    // auto userDao = std::make_shared<OdbUserDao>(db);
    UserServiceImpl service(userDao);

    std::string server_address("0.0.0.0:50051");
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    BOOST_LOG_SEV(logger, logging::trivial::info) << "User Service is registered. Followig rpc's are available: ReadUsers, ReadUser, CreateUser";
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    BOOST_LOG_SEV(logger, logging::trivial::info) << "gRPC Server is listening on " << server_address;
    server->Wait();
    
    return EXIT_SUCCESS;
}