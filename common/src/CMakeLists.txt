
target_sources(${COMMONS_LIBRARY_NAME} PUBLIC
    "commons.cc"
    "observability.cc"
    "mappers.cc"
    "services.cc")

target_link_libraries(${COMMONS_LIBRARY_NAME}
    Boost::log
    fmt::fmt
    nlohmann_json::nlohmann_json
    opentelemetry-cpp::opentelemetry-cpp
    cxxopts::cxxopts
    yaml-cpp::yaml-cpp
    restbed::restbed)

if(WIN32)
    target_link_libraries(${COMMONS_LIBRARY_NAME} ${opentelemetry-cpp_LIB_DIRS_RELEASE}/opentelemetry_proto_grpc.lib)
elseif(UNIX)
    target_link_libraries(${COMMONS_LIBRARY_NAME} ${opentelemetry-cpp_LIB_DIRS_RELEASE}/libopentelemetry_proto_grpc.a)
endif()