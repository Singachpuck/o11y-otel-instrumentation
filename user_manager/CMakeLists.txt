add_executable(${USER_MANAGER_TARGET_NAME})
target_link_libraries(${USER_MANAGER_TARGET_NAME} PRIVATE ${PROTO_COLLECTION_NAME})

set(ODB_COLLECTION_NAME "odb-objects")
add_subdirectory(odb)
target_link_libraries(${USER_MANAGER_TARGET_NAME} PRIVATE ${ODB_COLLECTION_NAME})

add_subdirectory(src)
add_subdirectory(include)
