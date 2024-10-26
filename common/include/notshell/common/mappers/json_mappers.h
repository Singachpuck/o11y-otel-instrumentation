#pragma once

#include <nlohmann/json.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

// Overload adl_serializer serializer
namespace nlohmann {
    template <>
    struct adl_serializer<boost::posix_time::ptime> {
        static void to_json(nlohmann::json& j, const boost::posix_time::ptime& opt);

        static void from_json(const nlohmann::json& j, boost::posix_time::ptime& opt);
    };
}