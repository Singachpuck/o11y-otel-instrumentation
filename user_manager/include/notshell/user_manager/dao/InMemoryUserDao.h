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
        InMemoryUserDao();
        std::shared_ptr<Users> getUsers() override;
        std::shared_ptr<User> getUserById(std::uint64_t) override;
        std::shared_ptr<User> createUser(std::unique_ptr<User>) override;
    };
}

NOTSHELL_NAMESPACE_END