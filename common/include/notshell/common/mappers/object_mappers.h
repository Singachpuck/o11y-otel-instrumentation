#pragma once

// #include <google/protobuf/timestamp.pb.h>
// #include <boost/date_time/posix_time/posix_time_types.hpp>

#include "project_config.h"

NOTSHELL_NAMESPACE_BEGIN

namespace common {

template<class T1, class T2>
void convert(const T1& from, T2& to);

}
NOTSHELL_NAMESPACE_END
