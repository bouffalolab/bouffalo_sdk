SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= $(abspath  ./../../../)

export BL_SDK_BASE

CROSS_COMPILE ?= riscv64-unknown-elf-


CONFIG_RCP_ENABLE ?= 0
cmake_definition+=-DCONFIG_RCP_ENABLE=$(CONFIG_RCP_ENABLE)

CONFIG_USE_LPAPP ?= 0
cmake_definition+=-DCONFIG_USE_LPAPP=$(CONFIG_USE_LPAPP)

cmake_definition+=-DCONFIG_PSRAM_COPY_CODE=$(CONFIG_PSRAM_COPY_CODE)

include $(BL_SDK_BASE)/project.build 
