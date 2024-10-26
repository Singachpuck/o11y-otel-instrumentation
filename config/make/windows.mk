CONAN := conan
CONAN_PROFILES ?= -pr config/conan_profiles/release \
				-pr config/conan_profiles/windows_x86_64_vs2022
CMAKE_HOME ?= $(CONAN_USER_HOME)/.conan/data/cmake/3.30.0/_/_/package/ca33edce272a279b24f87dc0d4cf5bbdcffbc187/bin
CMAKE := $(CMAKE_HOME)/cmake
CMAKE_GENERATOR ?= Visual Studio 17 2022
PROTOBUF_HOME ?= D:/.conan/5c10e5/1
PROTOC ?= $(PROTOBUF_HOME)/bin/protoc.exe
GRPC_HOME ?= D:/.conan/c24159/1
GRPC_PLUGIN ?= $(GRPC_HOME)/bin/grpc_cpp_plugin.exe

BUILD_CONFIG = --config Release

BUILD_DIR ?= build

define rm_directory
  powershell -Command "Remove-Item -Path $(1) -Recurse -Force"
endef

define mk_directory
  powershell -Command "New-Item -ItemType Directory -Path $(1) -Force"
endef

run-user-manager:
	./build/Release/user-manager-service -o ./config/o11y/obs-config-user.yml

run-gateway:
	./build/Release/gateway-service -o ./config/o11y/obs-config-gateway.yml