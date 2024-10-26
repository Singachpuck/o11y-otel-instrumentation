CONAN := conan
# CMAKE_HOME := $(CONAN_USER_HOME)/.conan/data/cmake/3.17.3/thirdparty/stable/package/4db1be536558d833e52e862fd84d64d75c2b3656/cmake-3.17.3-Linux-x86_64/bin
CMAKE := cmake
CMAKE_GENERATOR := Unix Makefiles
CXX_HOME := $(CONAN_USER_HOME)/.conan/data/gcc/8.2.0/thirdparty/stable/package/4db1be536558d833e52e862fd84d64d75c2b3656/bin
PROTOBUF_HOME := ${CONAN_USER_HOME}/.conan/data/protobuf/3.21.12/_/_/package/c5d8eb4028ec0f08de6595582857476d7b554344
PROTOC := $(PROTOBUF_HOME)/bin/protoc
PROTOBUF_INCLUDE := $(PROTOBUF_HOME)/include

GRPC_PLUGIN := $(CONAN_USER_HOME)/.conan/data/grpc/1.54.3/_/_/package/ea3ff86c0ee1b7f22019344b237530a00d38a503/bin/grpc_cpp_plugin
BUILD_DIR := build

CMAKE_GENERATE_COMMAND := $(CMAKE) . -G "$(CMAKE_GENERATOR)" -B $(BUILD_DIR) -DCMAKE_TOOLCHAIN_FILE=$(BUILD_DIR)/conan_toolchain.cmake \
	-DCMAKE_BUILD_TYPE=Release

export CC := gcc
export CXX := g++
# export PATH := $(PATH):$(CMAKE_HOME):$(CXX_HOME)

all: clean c-install generate-protobuf generate b

c-install:
	# $(CONAN) install -pr release -pr linux_x86_64_gcc_8.2.0_CXX03_ABI -if=$(BUILD_DIR) . --build=missing
	$(CONAN) install -pr default -if=$(BUILD_DIR) . --build=missing

generate:
	$(CMAKE_GENERATE_COMMAND)

generate-protobuf:
	mkdir -p $(BUILD_DIR)/proto_output/schema/
	$(PROTOC) -I protos --grpc_out=$(BUILD_DIR)/proto_output/schema/ --plugin=protoc-gen-grpc="$(GRPC_PLUGIN)" --proto_path=$(PROTOBUF_INCLUDE) protos/User.proto
	$(PROTOC) -I protos --cpp_out=$(BUILD_DIR)/proto_output/schema/ --proto_path=$(PROTOBUF_INCLUDE) protos/User.proto

b:
	$(CMAKE) --build ./$(BUILD_DIR) # --verbose

clean:
	rm -rf build
	mkdir build

run-user-manager:
	./user-manager-service -o ./config/o11y/obs-config-user.yml

run-gateway:
	./gateway-service -o ./config/o11y/obs-config-gateway.yml