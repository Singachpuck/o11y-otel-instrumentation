CONAN ?= conan
CONAN_PROFILES ?= -pr:a config/conan_profiles/release \
				-pr:a config/conan_profiles/conan2/windows_x86_64_msvc194
# -pr config/conan_profiles/windows_x86_64_vs2022
# CMAKE_HOME ?= $(CONAN_USER_HOME)/.conan/data/cmake/3.30.0/_/_/package/ca33edce272a279b24f87dc0d4cf5bbdcffbc187/bin
# CMAKE := $(CMAKE_HOME)/cmake
CMAKE := cmake
CMAKE_GENERATOR ?= Visual Studio 17 2022

# Left to show example of configuration
# PROTOBUF_HOME ?= D:/.conan/5c10e5/1
# PROTOC ?= $(PROTOBUF_HOME)/bin/protoc.exe
# GRPC_HOME ?= D:/.conan/c24159/1
# GRPC_PLUGIN ?= $(GRPC_HOME)/bin/grpc_cpp_plugin.exe

BUILD_CONFIG ?= --config Release

BUILD_DIR ?= build

define rm_directory
  powershell -Command "Remove-Item -Path $(1) -Recurse -Force"
endef

define mk_directory
  powershell -Command "New-Item -ItemType Directory -Path $(1) -Force"
endef

run-user-manager: $(BUILD_DIR)/conanrun.ps1 $(BUILD_DIR)/Release/user-manager-service.exe
	powershell -noexit -Command \
	"& \"$(word 1,$^)\"; \
	$(word 2,$^) -o ./config/o11y/obs-config-user.yml --db-password=postgres --db-name=o11y --db-host=localhost --db-port=5433"

run-gateway: $(BUILD_DIR)/conanrun.ps1 $(BUILD_DIR)/Release/gateway-service.exe
	$(BUILD_DIR)/Release/gateway-service -o ./config/o11y/obs-config-gateway.yml