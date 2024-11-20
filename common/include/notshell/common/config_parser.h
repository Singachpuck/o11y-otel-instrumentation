#pragma once

#include <forward_list>
#include <cxxopts.hpp>
#include <yaml-cpp/yaml.h>

#include "project_config.h"

NOTSHELL_NAMESPACE_BEGIN

namespace common {
namespace config {
    struct PostgresConfig {
        std::string user = "postgres";
        std::string password = "postgres";
        std::string db = "postgres";
        std::string host = "localhost";
        std::uint16_t port = 5432;
    };

    typedef YAML::Node ObservabilityConfig;
    // Needs to be defined in each main.cpp
    extern const char* CURRENT_SERVICE_NAME;

    struct ConfigHolder {
    private:
        std::unique_ptr<PostgresConfig> postgresConfig;
        ObservabilityConfig obsConfig;
        std::unordered_map<std::string, std::string> environment;

    public:
        ConfigHolder();
        void withPostgresConfig();
        const ObservabilityConfig& getO11yConf() const;
        const PostgresConfig& getPostgresConfig() const;
        const std::string& getEnvOrDefault(const std::string& env, const std::string& def) const;
        friend void parse_pgsql_config(const cxxopts::ParseResult& cl_arguments);
        friend void parse_observability_config(const cxxopts::ParseResult& cl_arguments);
        friend void parse_environment(const std::forward_list<std::string>& env_names);
    };

    const ConfigHolder& get();

    cxxopts::ParseResult parse_command_line_args(int argc, char* argv[], std::string appName, std::string description);

    void parse_observability_config(const cxxopts::ParseResult& cl_arguments);

    void parse_environment(const std::forward_list<std::string>& env_names);

    void parse_pgsql_config(const cxxopts::ParseResult& cl_arguments);
}
}

NOTSHELL_NAMESPACE_END