#pragma once

#include <odb/database.hxx>

#include "project_config.h"
#include "notshell/common/observability/logging_commons.h"
#include "notshell/user_manager/dao/IUserDao.h"

NOTSHELL_NAMESPACE_BEGIN

namespace user_manager {
    class OdbUserDao : public IUserDao {
    private:
        src::severity_logger<logging::trivial::severity_level> logger;
        std::shared_ptr<odb::database> db;
    public:
        explicit OdbUserDao(std::shared_ptr<odb::database> db);
        std::shared_ptr<Users> getUsers() override;
        std::shared_ptr<User> getUserById(std::uint64_t) override;
        std::shared_ptr<User> createUser(std::unique_ptr<User>) override;
    };
}

NOTSHELL_NAMESPACE_END