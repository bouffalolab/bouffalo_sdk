SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= $(abspath  ./../../..)

export BL_SDK_BASE

# Optional matching LPFW image, without replacing the SDK default.
ifneq ($(strip $(LPFW_BIN)),)
cmake_definition += -DLPFW_BIN=$(abspath $(LPFW_BIN))
endif

include $(BL_SDK_BASE)/project.build

# This defconfig-only option has no Kconfig symbol; pass it directly to CMake.
CONFIG_INPUT_VARS := $(filter-out CONFIG_PSRAM_RETENTION,$(CONFIG_INPUT_VARS))
cmake_definition += -DCONFIG_PSRAM_RETENTION=$(if $(filter y,$(CONFIG_PSRAM_RETENTION)),ON,OFF)
