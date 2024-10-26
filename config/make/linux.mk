CONAN ?= conan
CONAN_USER_HOME ?= ~
CONAN_PROFILES ?= -pr config/conan_profiles/release \
				-pr config/conan_profiles/linux_x86_64_gcc_12_3_CXX03_ABI
CMAKE_HOME ?= $(CONAN_USER_HOME)/.conan/data/cmake/3.30.0/_/_/package/4db1be536558d833e52e862fd84d64d75c2b3656/bin
CMAKE ?= $(CMAKE_HOME)/cmake
CMAKE_GENERATOR ?= Unix Makefiles

# Left to show example of configuration
#PROTOBUF_HOME ?= $(CONAN_USER_HOME)/.conan/data/protobuf/3.21.12/_/_/package/bc959546f27807a8f3eace965ef21774d1f4c8e6
#PROTOC ?= $(PROTOBUF_HOME)/bin/protoc
#GRPC_HOME ?= $(CONAN_USER_HOME)/.conan/data/grpc/1.54.3/_/_/package/5673b6f6b85dd59b860758ee90b167db030d5e6b
#GRPC_PLUGIN ?= $(GRPC_HOME)/bin/grpc_cpp_plugin
BUILD_DIR ?= build

define rm_directory
  rm -rf $(1)
endef

define mk_directory
  mkdir -p $(1)
endef

run-user-manager:
	./build/user-manager-service -o ./config/o11y/obs-config-user.yml

run-gateway:
	./build/gateway-service -o ./config/o11y/obs-config-gateway.yml