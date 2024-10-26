#pragma once

#include "project_config.h"
#include "notshell/user_manager/model/User.h"

NOTSHELL_NAMESPACE_BEGIN

namespace user_manager {
    class IUserDao {
    public:
        virtual std::shared_ptr<Users> getUsers() = 0;
        virtual std::shared_ptr<User> getUserById(std::uint64_t) = 0;
        virtual std::shared_ptr<User> createUser(std::unique_ptr<User>) = 0;
    };
}

NOTSHELL_NAMESPACE_END