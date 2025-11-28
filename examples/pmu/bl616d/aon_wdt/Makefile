SDK_DEMO_PATH ?= .
BL_SDK_BASE ?= $(abspath  $(SDK_DEMO_PATH)/../../../..)

export BL_SDK_BASE

BL_SDK_BASE_PATH = $(realpath $(BL_SDK_BASE))

CHIP ?= bl616d
CPU_ID ?= ap
BOARD ?= bl616ddk_dv
CROSS_COMPILE ?= riscv64-unknown-elf-


LDO_KEEP_NURMAL_MODE_EN ?= n

cmake_definition+= -DLDO_KEEP_NURMAL_MODE_EN=$(LDO_KEEP_NURMAL_MODE_EN)

include $(BL_SDK_BASE)/project.build
