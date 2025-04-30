SDK_DEMO_PATH ?= .
BL_SDK_BASE ?= $(SDK_DEMO_PATH)/../../../..

export BL_SDK_BASE

CHIP ?= bl616
BOARD ?= bl616dk
CROSS_COMPILE ?= riscv64-unknown-elf-

# add custom cmake definition
#cmake_definition+=-Dxxx=sss
CONFIG_LINK_IN_PSRAM ?= 1
cmake_definition+=-DCONFIG_LINK_IN_PSRAM=$(CONFIG_LINK_IN_PSRAM)

include $(BL_SDK_BASE)/project.build
