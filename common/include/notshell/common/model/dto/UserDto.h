#pragma once

#include <cstdint>
#include <string>
#include <nlohmann/json.hpp>

#include "project_config.h"
#include "notshell/common/http_helper.h"
#include "notshell/common/mappers/json_mappers.h"
#include "notshell/common/model/IJsonConvertible.h"

NOTSHELL_NAMESPACE_BEGIN

namespace common {
namespace model {
class UserDto : public IJsonConvertible<UserDto> {
    public:
        std::uint64_t id;
        std::string name;
        std::string password;
        boost::posix_time::ptime created;

        inline void to_json(nlohmann::json& nlohmann_json_j, const UserDto& nlohmann_json_t) {
            nlohmann_json_j["id"] = nlohmann_json_t.id;
            nlohmann_json_j["name"] = nlohmann_json_t.name;
            // nlohmann_json_j["password"] = nlohmann_json_t.password;
            nlohmann_json_j["created"] = nlohmann_json_t.created;
        }
            
        inline void from_json(const nlohmann::json& nlohmann_json_j, UserDto& nlohmann_json_t) {
            // nlohmann_json_j.at("id").get_to(nlohmann_json_t.id);
            nlohmann_json_j.at("name").get_to(nlohmann_json_t.name);
            nlohmann_json_j.at("password").get_to(nlohmann_json_t.password);
            // nlohmann_json_j.at("created").get_to(nlohmann_json_t.created);
        }
};

class UsersDto : public IJsonConvertible<UsersDto> {
    public:
        std::vector<std::shared_ptr<UserDto>> users;

        JSON_DEFINE_ARRAY_METHODS(UsersDto, users, UserDto)
};

}
}

NOTSHELL_NAMESPACE_END
