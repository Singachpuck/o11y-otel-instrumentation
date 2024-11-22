#include "notshell/user_manager/dao/InMemoryUserDao.h"

#include "notshell/common/services.h"

namespace commons = dochkas::notshell::common;

NOTSHELL_NAMESPACE_BEGIN

user_manager::InMemoryUserDao::InMemoryUserDao() {
    User u1;
    u1.name = "user1";
    u1.password = "password1";
    u1.created = boost::posix_time::second_clock::universal_time();

    User u2;
    u2.name = "user2";
    u2.password = "password2";
    u2.created = boost::posix_time::second_clock::universal_time();

    User u3;
    u3.name = "user3";
    u3.password = "password3";
    u3.created = boost::posix_time::second_clock::universal_time();

    u1.id = commons::getNextId("UserEntity");
    this->users.insert({u1.id, u1});
    u2.id = commons::getNextId("UserEntity");
    this->users.insert({u2.id, u2});
    u3.id = commons::getNextId("UserEntity");
    this->users.insert({u3.id, u3});
}

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