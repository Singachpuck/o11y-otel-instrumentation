#include "notshell/gateway/controller/UserController.h"


NOTSHELL_NAMESPACE_BEGIN

namespace gateway {

    UserController::UserController(std::shared_ptr<restbed::Service> service, std::shared_ptr<IUserService> userService, std::shared_ptr<trace::Tracer> tracer)
        : RestController(service, tracer), userService(userService) {};

    void UserController::init() {
        this->stat.requestCounter = obs::getMeter()->CreateUInt64Counter(
            // name
            "RequestCounter",
            // description
            "Counts total number of request to Gateway Service",
            //unit
            "requests"
        );

        // Multiple entities handling
        auto usersResource = std::make_shared<restbed::Resource>();

        BUILD_GENERIC_HANDLER(getUsersHandler, &UserController::readUsers, model::UsersDto)
        BUILD_ENTITY_CONSUMER_HANDLER(createUserHandler, &UserController::createUser, model::UserDto);

        usersResource->set_path("/users");
        usersResource->set_method_handler("GET", getUsersHandler);
        usersResource->set_method_handler("POST", createUserHandler);

        // Single Entity handling
        auto userResource = std::make_shared<restbed::Resource>();

        BUILD_GENERIC_HANDLER(getUserHandler, &UserController::readUser, model::UserDto)

        userResource->set_path("/users/{user_id: [0-9]+}");
        userResource->set_method_handler("GET", getUserHandler);

        this->getService()->publish(usersResource);
        this->getService()->publish(userResource);
    }

    std::shared_ptr<model::UsersDto> UserController::readUsers(const std::shared_ptr<const restbed::Request> request) {
        auto scope = trace_api::Scope(tracer->StartSpan("RestUserService/readUsers"));
        this->stat.requestCounter->Add(1);

        return this->userService->readUsers();
    }

    std::shared_ptr<model::UserDto> UserController::readUser(const std::shared_ptr<const restbed::Request> request) {
        auto scope = trace_api::Scope(tracer->StartSpan("RestUserService/readUser"));
        this->stat.requestCounter->Add(1);

        std::uint64_t id = std::strtoull(request->get_path_parameter("user_id").c_str(), nullptr, 10);

        auto user = this->userService->readUser(id);

        if (user == nullptr) {
            throw std::runtime_error("User not found!");
        }

        return user;
    }

    std::shared_ptr<model::UserDto> UserController::createUser(std::unique_ptr<model::UserDto> userDto) {
        auto scope = trace_api::Scope(tracer->StartSpan("RestUserService/createUser"));
        this->stat.requestCounter->Add(1);

        auto user = this->userService->createUser(std::move(userDto));

        if (user == nullptr) {
            throw std::runtime_error("User is invalid!");
        }

        return user;
    }
}
NOTSHELL_NAMESPACE_END