#include "notshell/common/services.h"

//#include <odb/database.hxx>
#include <odb/pgsql/database.hxx>
//#include <odb/exceptions.hxx>
// #include <odb/pgsql/error.hxx>
//#include <odb/pgsql/exceptions.hxx>

#include "notshell/common/observability/logging_commons.h"

static std::unordered_map<std::string, std::uint64_t> id_sequences{};

std::uint64_t dochkas::notshell::common::getNextId(const std::string& key) {
    return id_sequences[key]++;
}

std::unique_ptr<odb::database> dochkas::notshell::common::initDatabase() {
    auto& logger = obs::default_logger::get();
    auto config = config::get().getPostgresConfig();
    
//    std::ostringstream info;
//    odb::pgsql::database::print_usage(info);
//    BOOST_LOG_SEV(logger, logging::trivial::info) << info.str();
//    try {
//        BOOST_LOG_SEV(logger, logging::trivial::info) << config.user;
//        BOOST_LOG_SEV(logger, logging::trivial::info) << config.password;
//        BOOST_LOG_SEV(logger, logging::trivial::info) << config.db;
//        BOOST_LOG_SEV(logger, logging::trivial::info) << config.host;
//        BOOST_LOG_SEV(logger, logging::trivial::info) << config.port;
        // odb::pgsql::database qwdqd("user", "pass", "mydb");
        // odb::pgsql::cli_exception("");
        // odb::pgsql::transaction();
        // std::exception();
//        throw std::exception();
//
//    } catch (...)
//    {
//        BOOST_LOG_SEV(logger, logging::trivial::error) << "Hi";
//        // return nullptr;
//    }

     return std::make_unique<odb::pgsql::database>(config.user, config.password, config.db, config.host, config.port);
}