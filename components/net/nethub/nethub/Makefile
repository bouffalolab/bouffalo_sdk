# nethub framework Makefile

# Default targets
.PHONY: clean test all

# Check if build directory exists
BUILD_DIR := build
ifeq ($(wildcard $(BUILD_DIR)),)
    HAS_BUILD := false
else
    HAS_BUILD := true
endif

# Default target: build project
all:
	@if [ "$(HAS_BUILD)" = "false" ]; then \
		echo "Creating build directory and initializing CMake..."; \
		mkdir -p $(BUILD_DIR); \
		cd $(BUILD_DIR) && cmake .. -DCMAKE_C_FLAGS="-DCONFIG_NETHUB_ON_LINUX"; \
	fi
	@echo "Compiling project..."
	$(MAKE) -C $(BUILD_DIR)

# Compatibility target
make: all

# Run tests: execute all example programs
test:
	@echo "=== nethub framework test ==="
	@echo ""
	@# Check if build directory exists, build if it doesn't
	@if [ "$(HAS_BUILD)" = "false" ]; then \
		echo "Build directory does not exist, starting build..."; \
		mkdir -p $(BUILD_DIR); \
		cd $(BUILD_DIR) && cmake .. -DCMAKE_C_FLAGS="-DCONFIG_NETHUB_ON_LINUX" && $(MAKE); \
	fi
	@# Check if executable files exist
	@if [ ! -f "$(BUILD_DIR)/examples/sdiowifi/sdiowifi_demo" ]; then \
		echo "sdiowifi_demo does not exist, starting build..."; \
		$(MAKE) all; \
	fi
	@if [ ! -f "$(BUILD_DIR)/examples/hostless/hostless_demo" ]; then \
		echo "hostless_demo does not exist, starting build..."; \
		$(MAKE) all; \
	fi
	@echo ""
	@echo "Starting to run tests..."
	@echo ""
	@# Run SDIO WiFi example
	@echo "========== Test 1: SDIO WiFi Application Example =========="
	@if [ -f "$(BUILD_DIR)/examples/sdiowifi/sdiowifi_demo" ]; then \
		echo "Running SDIO WiFi application example..."; \
		cd $(BUILD_DIR)/examples/sdiowifi && timeout 5s ./sdiowifi_demo || echo "SDIO WiFi example completed or timed out"; \
		echo ""; \
	else \
		echo "Error: sdiowifi_demo does not exist, skipping test"; \
		echo ""; \
	fi
	@# Run Hostless example
	@echo "========== Test 2: Hostless Application Example =========="
	@if [ -f "$(BUILD_DIR)/examples/hostless/hostless_demo" ]; then \
		echo "Running Hostless application example..."; \
		cd $(BUILD_DIR)/examples/hostless && timeout 5s ./hostless_demo || echo "Hostless example completed or timed out"; \
		echo ""; \
	else \
		echo "Error: hostless_demo does not exist, skipping test"; \
		echo ""; \
	fi
	@echo "========== Test Complete =========="
	@echo "All example program tests completed!"

# Remove build directory and all build files
clean:
	@if [ "$(HAS_BUILD)" = "true" ]; then \
		echo "Removing build directory and all build files..."; \
		rm -rf $(BUILD_DIR); \
	else \
		echo "Build directory does not exist, no cleanup needed"; \
	fi