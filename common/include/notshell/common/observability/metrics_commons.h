#pragma once

#include "opentelemetry/metrics/meter_provider.h"
#include "opentelemetry/metrics/provider.h"

#include "project_config.h"
#include "notshell/common/config_parser.h"

namespace metrics_api = opentelemetry::metrics;

namespace config = dochkas::notshell::common::config;


NOTSHELL_NAMESPACE_BEGIN

namespace common::observability {

void initMetrics();

inline opentelemetry::nostd::shared_ptr<metrics_api::Meter> getMeter() {
    return metrics_api::Provider::GetMeterProvider()->GetMeter(config::CURRENT_SERVICE_NAME,
        PROJECT_VERSION,
        "https://opentelemetry.io/schemas/1.2.0");
}

}

NOTSHELL_NAMESPACE_END