SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= $(abspath  ./../../../..)

export BL_SDK_BASE

BL_SDK_BASE_PATH = $(realpath $(BL_SDK_BASE))

CPU_ID ?= m0
BOARD ?= bl616ddk
CROSS_COMPILE ?= riscv64-unknown-elf-


include $(BL_SDK_BASE)/project.build
