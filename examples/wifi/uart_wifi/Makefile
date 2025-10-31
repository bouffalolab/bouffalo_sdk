SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= ./../../../

export BL_SDK_BASE

CROSS_COMPILE ?= riscv64-unknown-elf-
MACSW_CONFIG_SELECT = default

# add custom cmake definition
#cmake_definition+=-Dxxx=sss

CONFIG_RCP_ENABLE ?= 0
cmake_definition+=-DCONFIG_RCP_ENABLE=$(CONFIG_RCP_ENABLE)

CONFIG_USE_LPAPP ?= 0
cmake_definition+=-DCONFIG_USE_LPAPP=$(CONFIG_USE_LPAPP)

cmake_definition+=-DCONFIG_PSRAM_COPY_CODE=$(CONFIG_PSRAM_COPY_CODE)

-include $(SDK_DEMO_PATH)/.config
include $(BL_SDK_BASE)/project.build 
