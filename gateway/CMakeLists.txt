
add_executable(${GATEWAY_TARGET_NAME})
target_link_libraries(${GATEWAY_TARGET_NAME} PRIVATE ${PROTO_COLLECTION_NAME})

add_subdirectory(src)
add_subdirectory(include)
