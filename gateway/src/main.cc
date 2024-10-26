#include <memory>
#include <thread>
#include <restbed>

#include <fmt/core.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "notshell/common/observability/logging_commons.h"
#include "notshell/common/observability/tracer_commons.h"
#include "notshell/common/observability/metrics_commons.h"
#include "notshell/gateway/controller/UserController.h"
#include "notshell/common/model/dto/UserDto.h"
#include "notshell/common/config_parser.h"

#define USER_MANAGER_HOST_ENV_NAME "USER_MANAGER_HOST"

namespace gateway = dochkas::notshell::gateway;
namespace model = dochkas::notshell::common::model;
namespace obs = dochkas::notshell::common::observability;
namespace config = dochkas::notshell::common::config;

const char* config::CURRENT_SERVICE_NAME = GATEWAY_SERVICE_NAME;
const std::string USER_MANAGER_DEFAULT_HOST("localhost:50051");

int main(int argc, char* argv[])
{
    auto cl_result = config::parse_command_line_args(argc, argv,
        fmt::format("{}/{}", PROJECT_NAME, config::CURRENT_SERVICE_NAME),
        "Gateway service.");

    config::parse_observability_config(cl_result);
    config::parse_environment({ USER_MANAGER_HOST_ENV_NAME });

    obs::initTracer(config::CURRENT_SERVICE_NAME);
    obs::initLogging();
    obs::initMetrics();

    auto& logger = obs::default_logger::get();
    auto tracer = std::shared_ptr<opentelemetry::trace::Tracer>(obs::getTracer().get());

    auto& userManagerHost = config::get().getEnvOrDefault(USER_MANAGER_HOST_ENV_NAME, USER_MANAGER_DEFAULT_HOST);

    std::shared_ptr<gateway::IUserService> userService = std::make_shared<gateway::GrpcUserService>(
        grpc::CreateChannel(userManagerHost, grpc::InsecureChannelCredentials()));
    BOOST_LOG_SEV(logger, logging::trivial::info) << "Created grpc channel on " << userManagerHost;
    
    std::shared_ptr<restbed::Service> service = std::make_shared<restbed::Service>();
    gateway::UserController controller(service, userService, tracer);

    BOOST_LOG_SEV(logger, logging::trivial::info) << "Registering /users service...";
    controller.init();
    
    const std::uint16_t port = 8081;
    const std::string root = "/api/v1";

    auto settings = std::make_shared< restbed::Settings >();
    settings->set_root(root);
    settings->set_port(port);
    settings->set_worker_limit( std::thread::hardware_concurrency( ) );
    
    BOOST_LOG_SEV(logger, logging::trivial::info) << "Http Server is listening on port " << port;
    BOOST_LOG_SEV(logger, logging::trivial::info) << "REST Api root is at " << root;

    service->start( settings );
    
    return EXIT_SUCCESS;
}