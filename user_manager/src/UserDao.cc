#include "notshell/user_manager/dao/InMemoryUserDao.h"

#include "notshell/common/services.h"

namespace commons = dochkas::notshell::common;

NOTSHELL_NAMESPACE_BEGIN

std::shared_ptr<user_manager::Users> user_manager::InMemoryUserDao::getUsers() {
    auto userCollection = std::make_shared<Users>();
    for (auto& user : this->users) {
        userCollection->push_back(std::make_shared<User>(user.second));
    }
    return userCollection;
}

std::shared_ptr<user_manager::User> user_manager::InMemoryUserDao::getUserById(std::uint64_t id) {
    return std::make_shared<User>(this->users.at(id));
}

std::shared_ptr<user_manager::User> user_manager::InMemoryUserDao::createUser(std::unique_ptr<User> user) {
    user->id = commons::getNextId("UserEntity");
    auto result = this->users.insert({user->id, *user});
    return std::make_shared<User>(result.first->second);
}

NOTSHELL_NAMESPACE_END