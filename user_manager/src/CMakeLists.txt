set(SOURCES
    "main.cc"
    "mappers.cc"
    "UserService.cc"
    "UserDao.cc")

target_sources(${USER_MANAGER_TARGET_NAME} PRIVATE ${SOURCES})

target_link_libraries(${USER_MANAGER_TARGET_NAME} PRIVATE ${COMMONS_LIBRARY_NAME})