#include "notshell/common/mappers/object_mappers.h"

#include "schema/User.pb.h"
#include "notshell/common/services.h"
#include "notshell/user_manager/model/User.h"

using namespace dochkas::notshell::user_manager;

template<>
void dochkas::notshell::common::convert<User, foo::UserResponse>(const User& from, foo::UserResponse& to) {
    to.set_id(from.id);
    to.set_name(from.name);
    convert(from.created, *to.mutable_created());
}

template<>
void dochkas::notshell::common::convert<foo::UserRequest, User>(const foo::UserRequest& from, User& to) {
    // to.id = getNextId("UserEntity");
    to.name = from.name();
    to.password = from.password();
    to.created = boost::posix_time::second_clock::universal_time();
}