SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= ./../../../..

export BL_SDK_BASE

CROSS_COMPILE ?= riscv64-unknown-elf-
MACSW_CONFIG_SELECT = bridge

# add custom cmake definition
#cmake_definition+=-Dxxx=sss
CONFIG_LINK_IN_PSRAM ?= 1
cmake_definition+=-DCONFIG_LINK_IN_PSRAM=$(CONFIG_LINK_IN_PSRAM)

-include $(SDK_DEMO_PATH)/.config
include $(BL_SDK_BASE)/project.build
