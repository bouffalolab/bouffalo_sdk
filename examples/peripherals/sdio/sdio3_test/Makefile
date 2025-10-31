SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= ./../../../..

export BL_SDK_BASE

CHIP ?= bl628
BOARD ?= bl628dk
CROSS_COMPILE ?= riscv64-unknown-elf-

# add custom cmake definition
#cmake_definition+=-Dxxx=sss

-include $(SDK_DEMO_PATH)/.config
include $(BL_SDK_BASE)/project.build
