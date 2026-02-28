SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= $(abspath  ./../../../..)

export BL_SDK_BASE

BL_SDK_BASE_PATH = $(realpath $(BL_SDK_BASE))

CHIP=bl618dg
CPU_ID ?= ap
BOARD ?= bl618dgdk
# BOARD ?= bl618dgdk
# BOARD_DIR ?= $(BL_SDK_BASE_PATH)/chiptest/bl618dg
CROSS_COMPILE ?= riscv64-unknown-elf-



include $(BL_SDK_BASE)/project.build
