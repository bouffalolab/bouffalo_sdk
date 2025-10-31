SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= ./../..

export BL_SDK_BASE

CROSS_COMPILE ?= riscv64-unknown-elf-
CONFIG_COREDUMP := n
CONFIG_BOOT2_VER ?= 1.0.0
CONFIG_BOOT2_ANTI_ROLLBACK_VER ?= 0

# add custom cmake definition
cmake_definition+=-DCONFIG_BOOT2_VER=${CONFIG_BOOT2_VER}
cmake_definition+=-DCONFIG_BOOT2_ANTI_ROLLBACK_VER=$(CONFIG_BOOT2_ANTI_ROLLBACK_VER)

-include $(SDK_DEMO_PATH)/.config
include $(BL_SDK_BASE)/project.build
