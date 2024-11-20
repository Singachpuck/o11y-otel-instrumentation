add_library(${ODB_COLLECTION_NAME} OBJECT)

target_link_libraries(${ODB_COLLECTION_NAME} PUBLIC odb::odb Boost::date_time)

set(ODB_BINARY_DIR "${CMAKE_BINARY_DIR}/odb_output")
set(ODB_IMPORT_DIRS "${CMAKE_BINARY_DIR}/configured_files/include" "${Boost_INCLUDE_DIRS}")
set(ENTITIES_DIR "${CMAKE_CURRENT_LIST_DIR}/../include/notshell/user_manager/model")

target_include_directories(${ODB_COLLECTION_NAME} PUBLIC
    "${ENTITIES_DIR}"
    "${CMAKE_SOURCE_DIR}/common/include"
    "${CMAKE_BINARY_DIR}/configured_files/include"
)

generate_odb(TARGET ${ODB_COLLECTION_NAME}
    OUT_DIR ${ODB_BINARY_DIR}
    DATABASE pgsql
    WITH_SCHEMA
    ENTITY_SRC "${ENTITIES_DIR}/User.h"
    IMPORTS ${ODB_IMPORT_DIRS}
    PRAGMAS "orm/user_orm.h"
    ODBC_OPTIONS "--hxx-prologue" "#include \"notshell/common/mappers/odb/odb_pgsql_mappers.h\"" "")