SDK_DEMO_PATH ?= .
BL_SDK_BASE = $(SDK_DEMO_PATH)/../../..

export BL_SDK_BASE

CHIP ?= bl808
BOARD ?= bl808dk
CPU_ID ?= m0
CROSS_COMPILE ?= riscv64-unknown-elf-

# add custom cmake definition
#cmake_definition+=-Dxxx=sss

include $(BL_SDK_BASE)/project.build
