#pragma once

#include <forward_list>
#include <cxxopts.hpp>
#include <yaml-cpp/yaml.h>

#include "project_config.h"

NOTSHELL_NAMESPACE_BEGIN

namespace common {
namespace config {
    typedef YAML::Node ObservabilityConfig;
    // Needs to be defined in each main.cpp
    extern const char* CURRENT_SERVICE_NAME;

    struct ConfigHolder {
    private:
        ObservabilityConfig obsConfig;
        std::unordered_map<std::string, std::string> environment;

    public:
        ConfigHolder();
        const ObservabilityConfig& getO11yConf() const;
        const std::string& getEnvOrDefault(const std::string& env, const std::string& def) const;
        friend void parse_observability_config(const cxxopts::ParseResult& cl_arguments);
        friend void parse_environment(const std::forward_list<std::string>& env_names);
    };

    const ConfigHolder& get();

    cxxopts::ParseResult parse_command_line_args(int argc, char* argv[], std::string appName, std::string description);

    void parse_observability_config(const cxxopts::ParseResult& cl_arguments);

    void parse_environment(const std::forward_list<std::string>& env_names);
}
}

NOTSHELL_NAMESPACE_END