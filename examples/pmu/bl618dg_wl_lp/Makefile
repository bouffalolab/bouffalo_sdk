SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= ./../../..

export BL_SDK_BASE

# MACSW_CONFIG_SELECT = default


CHIP=bl618dg
BOARD ?= bl618dgdk
CPU_ID ?= ap

# add custom cmake definition
# cmake_definition+=-Dxxx=sss

-include $(SDK_DEMO_PATH)/.config
include $(BL_SDK_BASE)/project.build
