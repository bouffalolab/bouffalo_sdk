SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= ./../../../..

export BL_SDK_BASE

BL_SDK_BASE_PATH = $(realpath $(BL_SDK_BASE))

CHIP=bl616d
CPU_ID ?= ap
BOARD ?= bl616ddk
# BOARD ?= bl616ddk
# BOARD_DIR ?= $(BL_SDK_BASE_PATH)/chiptest/bl616d
CROSS_COMPILE ?= riscv64-unknown-elf-


# add custom cmake definition
#cmake_definition+=-Dxxx=sss

-include $(SDK_DEMO_PATH)/.config
include $(BL_SDK_BASE)/project.build
