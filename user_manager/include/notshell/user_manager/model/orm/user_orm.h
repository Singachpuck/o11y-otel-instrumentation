#pragma once

NOTSHELL_NAMESPACE_BEGIN

namespace user_manager {

#pragma db object(User)
#pragma db member(User::id) id auto
#pragma db value(boost::posix_time::ptime) type("TIMESTAMP")

}

NOTSHELL_NAMESPACE_END