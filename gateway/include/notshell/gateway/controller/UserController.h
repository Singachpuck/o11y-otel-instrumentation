#pragma once

#include "project_config.h"
#include "notshell/common/services.h"
#include "notshell/common/controller/RestController.h"
#include "notshell/common/model/dto/UserDto.h"
#include "notshell/gateway/service/GrpcUserService.h"

namespace trace_api = opentelemetry::trace;

namespace commons = dochkas::notshell::common;
namespace model = dochkas::notshell::common::model;

NOTSHELL_NAMESPACE_BEGIN

// TODO: Create mappers +
// TODO: Add Error handling
// TODO: Separate .h .cc files +
// TODO: Add opentelemetry on top +
// TODO: Add log level filter
// TODO: Add configurable logging options to OTEL +
namespace gateway {
    class UserController : public commons::RestController {
    private:
        std::shared_ptr<IUserService> userService;
        commons::ServiceStatistics stat;

    public:
        UserController(std::shared_ptr<restbed::Service> service, std::shared_ptr<IUserService> userService, std::shared_ptr<trace::Tracer> tracer);

        void init() override;

        std::shared_ptr<model::UsersDto> readUsers(const std::shared_ptr<const restbed::Request> request);

        std::shared_ptr<model::UserDto> readUser(const std::shared_ptr<const restbed::Request> request);
    
        std::shared_ptr<model::UserDto> createUser(const std::unique_ptr<model::UserDto> userDto);
    };
}

NOTSHELL_NAMESPACE_END