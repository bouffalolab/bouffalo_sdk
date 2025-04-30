SDK_DEMO_PATH ?= .
BL_SDK_BASE ?= $(SDK_DEMO_PATH)/../../../..

export BL_SDK_BASE

NOW_REAL_PATH:= $(realpath ./)

CHIP ?= bl616
BOARD ?= bl616dk_solution
BOARD_DIR = ${NOW_REAL_PATH}
CROSS_COMPILE ?= riscv64-unknown-elf-

# add custom cmake definition
#cmake_definition+=-Dxxx=sss

include $(BL_SDK_BASE)/project.build
