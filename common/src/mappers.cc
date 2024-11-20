#include "notshell/common/mappers/json_mappers.h"
#include "notshell/common/mappers/object_mappers.h"

#include <sstream>
#include <google/protobuf/timestamp.pb.h>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

#include "schema/User.pb.h"
#include "notshell/common/model/dto/UserDto.h"

namespace model = dochkas::notshell::common::model;

// JSON
void nlohmann::adl_serializer<boost::posix_time::ptime>::to_json(nlohmann::json& j, const boost::posix_time::ptime& opt) {
    std::stringstream ss;
    ss << opt;
    j = ss.str();
}

void nlohmann::adl_serializer<boost::posix_time::ptime>::from_json(const nlohmann::json& j, boost::posix_time::ptime& opt) {
    // Nothing to do yet
}
// ~JSON 

template<>
void dochkas::notshell::common::convert<boost::posix_time::ptime, long long>(const boost::posix_time::ptime& from, long long& to) {
    boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
    boost::posix_time::time_duration diff = from - epoch;
    
    to = diff.total_milliseconds();
}

template<>
void dochkas::notshell::common::convert<long long, boost::posix_time::ptime>(const long long& from, boost::posix_time::ptime& to) {
    const boost::posix_time::ptime epoch(boost::gregorian::date(1970, 1, 1));
    const boost::posix_time::milliseconds ms(from);
    
    to = epoch + ms;
}

template<>
void dochkas::notshell::common::convert<boost::posix_time::ptime, google::protobuf::Timestamp>(const boost::posix_time::ptime& from, google::protobuf::Timestamp& to) {
    const boost::posix_time::ptime time_t_epoch(boost::gregorian::date(1970,1,1));
    const boost::posix_time::time_duration diff = from - time_t_epoch;

    to.set_seconds(diff.total_seconds());
    to.set_nanos(diff.total_nanoseconds());
}

template<>
void dochkas::notshell::common::convert<google::protobuf::Timestamp, boost::posix_time::ptime>(const google::protobuf::Timestamp& from, boost::posix_time::ptime& to) {
    const boost::posix_time::ptime time_t_epoch(boost::gregorian::date(1970,1,1));
    to = time_t_epoch + boost::posix_time::seconds(from.seconds()) + boost::posix_time::microseconds(from.nanos() / 1000);
}

template<>
void dochkas::notshell::common::convert<foo::UserResponse, model::UserDto>(const foo::UserResponse& from, model::UserDto& to) {
    to.id = from.id();
    to.name = from.name();
    convert(from.created(), to.created);
}

template<>
void dochkas::notshell::common::convert<model::UserDto, foo::UserRequest>(const model::UserDto& from, foo::UserRequest& to) {
    to.set_name(from.name);
    to.set_password(from.password);
}
