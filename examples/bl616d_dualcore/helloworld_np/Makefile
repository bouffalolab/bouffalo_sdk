SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE = $(SDK_DEMO_PATH)/../../..

export BL_SDK_BASE

CHIP ?= bl616d
BOARD ?= bl616ddk
CPU_ID ?= np
CROSS_COMPILE ?= riscv64-unknown-elf-

# add custom cmake definition
#cmake_definition+=-Dxxx=sss

include $(BL_SDK_BASE)/project.build
-include $(SDK_DEMO_PATH)/.config

