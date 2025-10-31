SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= ./../../../..

export BL_SDK_BASE

NOW_REAL_PATH:= $(realpath ./)

BOARD_DIR = ${NOW_REAL_PATH}
CROSS_COMPILE ?= riscv64-unknown-elf-
MACSW_CONFIG_SELECT = default

# add custom cmake definition
#cmake_definition+=-Dxxx=sss

-include $(SDK_DEMO_PATH)/.config
include $(BL_SDK_BASE)/project.build
