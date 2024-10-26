
target_include_directories(${COMMONS_LIBRARY_NAME} PUBLIC
    "./"
    "${CMAKE_BINARY_DIR}/configured_files/include"
    "${CMAKE_BINARY_DIR}/proto_output/")
