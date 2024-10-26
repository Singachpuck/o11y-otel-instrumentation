
set(SOURCES
    "main.cc"
    "controllers.cc"
    "services.cc")

target_sources(${GATEWAY_TARGET_NAME} PRIVATE ${SOURCES})

target_link_libraries(${GATEWAY_TARGET_NAME} PRIVATE ${COMMONS_LIBRARY_NAME})