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

    const std::string& ConfigHolder::getEnvOrDefault(const std::string& env, const std::string& def) const {
        if (this->environment.count(env)) {
            return this->environment.at(env);
        }

        return def;
    }

    cxxopts::ParseResult parse_command_line_args(int argc, char* argv[], std::string appName, std::string description) {
        cxxopts::Options options(appName, description);

        options.add_options()
            ("o,observability-config", "Observability Configuration File", cxxopts::value<std::string>());

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
}

NOTSHELL_NAMESPACE_END