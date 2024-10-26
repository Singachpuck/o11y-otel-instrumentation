#pragma once

#include <grpcpp/channel.h>

#include "project_config.h"

#include "IUserService.h"
#include "schema/User.pb.h"
#include "schema/User.grpc.pb.h"
#include "notshell/common/observability/logging_commons.h"


namespace obs = dochkas::notshell::common::observability;


NOTSHELL_NAMESPACE_BEGIN

namespace gateway {
    class GrpcUserService : public IUserService {
    private:
        src::severity_logger<logging::trivial::severity_level> logger = obs::default_logger::get();
        std::unique_ptr<foo::UserService::Stub> stub_;

    public:
        GrpcUserService(std::shared_ptr<grpc::Channel> channel);

        std::shared_ptr<model::UsersDto> readUsers() override;

        std::shared_ptr<model::UserDto> readUser(std::uint64_t id) override;

        std::shared_ptr<model::UserDto> createUser(std::unique_ptr<model::UserDto>) override;
    };
}
NOTSHELL_NAMESPACE_END
