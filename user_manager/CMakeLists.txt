
add_executable(${USER_MANAGER_TARGET_NAME})
target_link_libraries(${USER_MANAGER_TARGET_NAME} PRIVATE ${PROTO_COLLECTION_NAME})

add_subdirectory(src)
add_subdirectory(include)
