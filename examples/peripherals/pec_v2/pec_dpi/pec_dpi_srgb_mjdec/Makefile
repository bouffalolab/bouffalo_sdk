SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= ./../../../../..

export BL_SDK_BASE

CROSS_COMPILE ?= riscv64-unknown-elf-

# add custom cmake definition
#cmake_definition+=-Dxxx=sss

CONFIG_PSRAM ?= y

-include $(SDK_DEMO_PATH)/.config
include $(BL_SDK_BASE)/project.build
