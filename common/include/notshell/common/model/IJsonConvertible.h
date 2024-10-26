#pragma once

#include <nlohmann/json.hpp>

#include "project_config.h"

using json = nlohmann::json;

NOTSHELL_NAMESPACE_BEGIN

namespace common {
namespace model {
    template<class T>
    class IJsonConvertible {
    public:
        virtual void to_json(json&, const T&) = 0;

        virtual void from_json(const json&, T&) = 0;

        inline std::string serialize_json() {
            json j;
            T& obj = static_cast<T&>(*this);
            this->to_json(j, obj);
            return j.dump();
        }

        inline T& deserialize_json(const std::string& json_string) {
            json j = json::parse(json_string);
            T& obj = static_cast<T&>(*this);
            this->from_json(j, obj);
            return obj;
        }
    };
}
}

NOTSHELL_NAMESPACE_END