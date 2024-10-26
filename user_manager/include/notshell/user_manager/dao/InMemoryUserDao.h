#pragma once

#include <unordered_map>

#include "project_config.h"

#include "notshell/user_manager/dao/IUserDao.h"

NOTSHELL_NAMESPACE_BEGIN

namespace user_manager {
    class InMemoryUserDao : public IUserDao {
    private:
        std::unordered_map<std::uint64_t, User> users{};
    public:
        std::shared_ptr<Users> getUsers();
        std::shared_ptr<User> getUserById(std::uint64_t);
        std::shared_ptr<User> createUser(std::unique_ptr<User>);
    };
}

NOTSHELL_NAMESPACE_END