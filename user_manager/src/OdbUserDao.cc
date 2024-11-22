#include "notshell/user_manager/dao/OdbUserDao.h"

#include <odb/transaction.hxx>
#include <utility>
#include "User-odb.hxx"

#include "notshell/common/services.h"


namespace commons = dochkas::notshell::common;

NOTSHELL_NAMESPACE_BEGIN

namespace user_manager {

    OdbUserDao::OdbUserDao(std::shared_ptr<odb::database> db) : db(std::move(db)) {}

    std::shared_ptr<Users> OdbUserDao::getUsers() {
        auto userCollection = std::make_shared<Users>();

        odb::transaction t(this->db->begin());
        odb::result<User> r(this->db->query<User>());

        for (auto i (r.begin()); i != r.end(); ++i) {
            userCollection->push_back(std::make_shared<User>(*i));
        }

        t.commit();

        return userCollection;
    }

    std::shared_ptr<User> OdbUserDao::getUserById(std::uint64_t id) {
        auto userCollection = std::make_shared<Users>();

        odb::transaction t(this->db->begin());
        auto user = this->db->query_one<User>(odb::query<User>::id == id);

        t.commit();

        return std::make_shared<User>(*user);
    }

    std::shared_ptr<User> OdbUserDao::createUser(std::unique_ptr<User> user) {
        try {
            odb::transaction t (this->db->begin());

            std::uint16_t id = this->db->persist(*user);

            t.commit();

            return this->getUserById(id);
        }
        catch (const odb::exception& e)
        {
            BOOST_LOG_SEV(logger, logging::trivial::error) << e.what();
            return nullptr;
        }
    }

}

NOTSHELL_NAMESPACE_END