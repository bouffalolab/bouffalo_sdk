SDK_DEMO_PATH ?= .
BL_SDK_BASE ?= $(abspath  $(SDK_DEMO_PATH)/../../../../..)

export BL_SDK_BASE

BL_SDK_BASE_PATH = $(realpath $(BL_SDK_BASE))

CPU_ID ?= np
BOARD ?= bl616ddk
CROSS_COMPILE ?= riscv64-unknown-elf-


include $(BL_SDK_BASE)/project.build
