#pragma once

#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>

#include "schema/User.pb.h"
#include "schema/User.grpc.pb.h"

#include "notshell/common/services.h"
#include "notshell/common/observability/logging_commons.h"
#include "notshell/user_manager/model/User.h"
#include "notshell/user_manager/dao/InMemoryUserDao.h"

namespace logging = boost::log;
namespace src = boost::log::sources;

namespace obs = dochkas::notshell::common::observability;
namespace commons = dochkas::notshell::common;

NOTSHELL_NAMESPACE_BEGIN

// TODO: Add DAO layer
namespace user_manager {
    class UserServiceImpl : public foo::UserService::Service {
    private:
        src::severity_logger<logging::trivial::severity_level> logger;
        commons::ServiceStatistics stat;
        std::shared_ptr<IUserDao> userDao;
        Users users;
    
    public:
        UserServiceImpl(std::shared_ptr<IUserDao> userDao);

        grpc::Status ReadUsers(::grpc::ServerContext* context, const ::foo::Empty* request, ::grpc::ServerWriter< ::foo::UserResponse>* writer) override;

        grpc::Status ReadUser(::grpc::ServerContext* context, const ::foo::Id* request, ::foo::UserResponse* response) override;

        grpc::Status CreateUser(::grpc::ServerContext* context, const ::foo::UserRequest* request, ::foo::UserResponse* response) override;
    };
}
NOTSHELL_NAMESPACE_END