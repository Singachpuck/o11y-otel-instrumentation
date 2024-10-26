set(GENERATED_PROTO_DIR ${CMAKE_BINARY_DIR}/proto_output/schema)
set(GRPC_PROTO_EXTENSIONS ".grpc.pb.h" ".grpc.pb.cc")

add_library(${PROTO_COLLECTION_NAME} OBJECT "${CMAKE_CURRENT_LIST_DIR}/User.proto")

target_link_libraries(${PROTO_COLLECTION_NAME} PUBLIC protobuf::libprotobuf gRPC::grpc++)

set(PROTO_BINARY_DIR "${CMAKE_BINARY_DIR}/proto_output/schema")
set(PROTO_IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}")

target_include_directories(${PROTO_COLLECTION_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/proto_output>")

#file(MAKE_DIRECTORY ${PROTO_BINARY_DIR})

add_custom_target(GENERATE_PROTO_DIR ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${GENERATED_PROTO_DIR})

add_dependencies(${PROTO_COLLECTION_NAME} GENERATE_PROTO_DIR)

protobuf_generate(
        TARGET ${PROTO_COLLECTION_NAME}
        PROTOC_OUT_DIR "${PROTO_BINARY_DIR}"
        IMPORT_DIRS ${PROTO_IMPORT_DIRS}
)

protobuf_generate(
        TARGET ${PROTO_COLLECTION_NAME}
        LANGUAGE grpc
        GENERATE_EXTENSIONS ${GRPC_PROTO_EXTENSIONS}
        PLUGIN "protoc-gen-grpc=\$<TARGET_FILE:gRPC::grpc_cpp_plugin>"
        IMPORT_DIRS ${PROTO_IMPORT_DIRS}
        PROTOC_OUT_DIR "${PROTO_BINARY_DIR}")