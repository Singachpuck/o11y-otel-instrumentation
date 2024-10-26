ifeq ($(OS), Windows_NT)

include windows.mk 

else
ifeq ($(shell uname), Linux)

include linux.mk

else

$(error OS is not recognized)

endif
endif

# In case manual proto build is needed
generate-protobuf:
	$(call mk_directory,$(BUILD_DIR)/proto_output/schema/)
	$(PROTOC) -I protos --grpc_out=$(BUILD_DIR)/proto_output/schema/ --plugin=protoc-gen-grpc="$(GRPC_PLUGIN)" --proto_path=$(PROTOBUF_HOME)/include protos/User.proto
	$(PROTOC) -I protos --cpp_out=$(BUILD_DIR)/proto_output/schema/ --proto_path=$(PROTOBUF_HOME)/include protos/User.proto