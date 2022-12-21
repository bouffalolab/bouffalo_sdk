SDK_DEMO_PATH ?= .
BL_SDK_BASE ?= $(SDK_DEMO_PATH)/../..

export BL_SDK_BASE

CHIP ?= bl616
BOARD ?= bl616dk
CROSS_COMPILE ?= riscv64-unknown-elf-
CONFIG_BOOT2_VER ?= 1.0.0
# add custom cmake definition
cmake_definition+=-DCONFIG_BOOT2_VER=${CONFIG_BOOT2_VER}

include $(BL_SDK_BASE)/project.build
