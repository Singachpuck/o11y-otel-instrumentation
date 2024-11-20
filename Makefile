all: clean c-install generate b

-include config.mk

c-install:
	$(CONAN) install $(CONAN_PROFILES) --output-folder=$(BUILD_DIR) . --build=missing

generate:
	$(CMAKE) . -G "$(CMAKE_GENERATOR)" -B $(BUILD_DIR) -DCMAKE_TOOLCHAIN_FILE=$(BUILD_DIR)/conan_toolchain.cmake \
		-DCMAKE_BUILD_TYPE=Release

b:
	$(CMAKE) --build ./$(BUILD_DIR) --verbose -j 10 $(BUILD_CONFIG)


update-obs-config:
	kubectl create configmap o11y-config --from-file=./config/o11y/obs-config-gateway.yml --from-file=./config/o11y/obs-config-user.yml -o yaml --dry-run=client | kubectl apply -f -

clean:
	$(call rm_directory,$(BUILD_DIR))
	$(call mk_directory,$(BUILD_DIR))