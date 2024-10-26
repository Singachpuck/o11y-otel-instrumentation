cmake_minimum_required(VERSION 3.17)
project(notshell VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

option(WITH_USER_MANAGER "Build user manager service?" ON)
option(WITH_GATEWAY "Build gateway service?" ON)

if ((NOT WITH_GATEWAY) AND (NOT WITH_USER_MANAGER))
    message(SEND_ERROR "No targets to build. Aborting...")
endif ()

find_package(cxxopts REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(restbed REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(fmt REQUIRED)
find_package(Boost REQUIRED COMPONENTS log)
find_package(opentelemetry-cpp REQUIRED)

# Generates protobuf files
set(PROTO_COLLECTION_NAME "proto-objects")
add_subdirectory(protos)

set(COMMONS_LIBRARY_NAME "notshell_commons")
add_subdirectory(common)

if (WITH_USER_MANAGER)
    set(USER_MANAGER_TARGET_NAME "user-manager-service")

    add_subdirectory(user_manager)
endif ()

if (WITH_GATEWAY)
    set(GATEWAY_TARGET_NAME "gateway-service")

    add_subdirectory(gateway)
endif ()

add_subdirectory(config)
