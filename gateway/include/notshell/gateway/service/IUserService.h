#pragma once

#include <memory>

#include "project_config.h"
#include "notshell/common/model/dto/UserDto.h"

namespace model = dochkas::notshell::common::model;

NOTSHELL_NAMESPACE_BEGIN

namespace gateway{
    class IUserService {
    public:
        virtual std::shared_ptr<model::UsersDto> readUsers() = 0;
        virtual std::shared_ptr<model::UserDto> readUser(std::uint64_t) = 0;
        virtual std::shared_ptr<model::UserDto> createUser(std::unique_ptr<model::UserDto>) = 0;
    };
}
NOTSHELL_NAMESPACE_END
