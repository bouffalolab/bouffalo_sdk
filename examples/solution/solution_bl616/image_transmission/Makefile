SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= $(abspath  ./../../../..)

export BL_SDK_BASE

NOW_REAL_PATH:= $(realpath ./)

BOARD_DIR = ${NOW_REAL_PATH}
CROSS_COMPILE ?= riscv64-unknown-elf-


include $(BL_SDK_BASE)/project.build
