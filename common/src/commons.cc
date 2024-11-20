#include "notshell/common/controller/RestController.h"
#include "notshell/common/config_parser.h"

#include <cxxopts.hpp>
#include <yaml-cpp/yaml.h>

#include "notshell/common/http_helper.h"

NOTSHELL_NAMESPACE_BEGIN

namespace common::config {
    static ConfigHolder holder;
    
    const ConfigHolder& get() {
        return holder;
    }

    void ConfigHolder::withPostgresConfig() {
        this->postgresConfig = std::make_unique<PostgresConfig>();
    }

    /*
    user-manager:
        logs:
            local:
                active: true
                format: text
            file:
                active: true
                path: log-output.log
                format: json
            span:
                active: true
                format: text
        traces:
            local: # For debugging purposes
                active: true
            export: # Not supported yet
                active: false
    */
    ConfigHolder::ConfigHolder() {
        ObservabilityConfig defaultConfig;

        defaultConfig[CURRENT_SERVICE_NAME]["logs"]["local"]["active"] = true;
        defaultConfig[CURRENT_SERVICE_NAME]["logs"]["local"]["format"] = "text";
        defaultConfig[CURRENT_SERVICE_NAME]["logs"]["file"]["active"] = false;
        defaultConfig[CURRENT_SERVICE_NAME]["logs"]["span"]["active"] = false;
        defaultConfig[CURRENT_SERVICE_NAME]["traces"]["local"]["active"] = false;
        defaultConfig[CURRENT_SERVICE_NAME]["traces"]["export"]["active"] = false;
        defaultConfig[CURRENT_SERVICE_NAME]["metrics"]["local"]["active"] = false;
        defaultConfig[CURRENT_SERVICE_NAME]["metrics"]["export"]["active"] = false;

        this->obsConfig = defaultConfig;
    }

    const ObservabilityConfig& ConfigHolder::getO11yConf() const {
        return this->obsConfig;
    }

    const PostgresConfig& ConfigHolder::getPostgresConfig() const {
        return *this->postgresConfig;
    }

    const std::string& ConfigHolder::getEnvOrDefault(const std::string& env, const std::string& def) const {
        if (this->environment.count(env)) {
            return this->environment.at(env);
        }

        return def;
    }

    cxxopts::ParseResult parse_command_line_args(int argc, char* argv[], std::string appName, std::string description) {
        cxxopts::Options options(appName, description);

        options.add_options()
            ("o,observability-config", "Observability Configuration File", cxxopts::value<std::string>())
            ("db-user", "Database User", cxxopts::value<std::string>())
            ("db-password", "Database Password", cxxopts::value<std::string>())
            ("db-name", "Database Name", cxxopts::value<std::string>())
            ("db-host", "Database Host", cxxopts::value<std::string>())
            ("db-port", "Database Port", cxxopts::value<std::uint16_t>());

        return options.parse(argc, argv);
    }

    void parse_observability_config(const cxxopts::ParseResult& cl_arguments) {
        if (!cl_arguments.count("observability-config")) {
            return;
        }

        std::string config_file = cl_arguments["observability-config"].as<std::string>();

        YAML::Node configured = YAML::LoadFile(config_file);

        holder.obsConfig = configured;
    }

    // For generic Environmental Variables, none for now
    void parse_environment_internal(std::unordered_map<std::string, std::string>& environment) {}

    void parse_environment(const std::forward_list<std::string>& env_names) {
        holder.environment.clear();
        parse_environment_internal(holder.environment);
        for (auto& env : env_names) {
            auto e = std::getenv(env.c_str());
            if (e != NULL) {
                holder.environment[env] = e;
            }
        }
    }

    /**
     * --db-user
     * --db-password
     * --db-name
     * --db-host
     * --db-port
     */
    void parse_pgsql_config(const cxxopts::ParseResult& cl_arguments) {
        holder.withPostgresConfig();

        if (auto db_user = cl_arguments["db-user"]; cl_arguments.count("db_user")) {
            holder.postgresConfig->user = db_user.as<std::string>();
        }

        if (auto db_password = cl_arguments["db-password"]; cl_arguments.count("db-password")) {
            holder.postgresConfig->password = db_password.as<std::string>();
        }

        if (auto db_name = cl_arguments["db-name"]; cl_arguments.count("db-name")) {
            holder.postgresConfig->db = db_name.as<std::string>();
        }

        if (auto db_host = cl_arguments["db-host"]; cl_arguments.count("db-host")) {
            holder.postgresConfig->host = db_host.as<std::string>();
        }

        if (auto db_port = cl_arguments["db-port"]; cl_arguments.count("db-port")) {
            holder.postgresConfig->port = db_port.as<std::uint16_t>();
        }
    }
}

NOTSHELL_NAMESPACE_END