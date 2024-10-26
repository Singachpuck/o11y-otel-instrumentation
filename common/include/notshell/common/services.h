#pragma once

#include <cstdint>

#include "project_config.h"
#include "notshell/common/observability/metrics_commons.h"

namespace obs = dochkas::notshell::common::observability;
namespace config = dochkas::notshell::common::config;

NOTSHELL_NAMESPACE_BEGIN

namespace common {
struct ServiceStatistics {
    opentelemetry::nostd::shared_ptr<metrics_api::Counter<uint64_t>> requestCounter;
};

std::uint64_t getNextId(const std::string& key);
}
NOTSHELL_NAMESPACE_END