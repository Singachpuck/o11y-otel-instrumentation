#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include "project_config.h"

NOTSHELL_NAMESPACE_BEGIN

namespace user_manager {
    class User {
        public:
            std::uint64_t id;
            std::string name;
            std::string password;
            boost::posix_time::ptime created;
    };

    typedef std::vector<std::shared_ptr<User>> Users;
}

NOTSHELL_NAMESPACE_END