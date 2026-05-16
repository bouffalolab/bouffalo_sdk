SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= $(abspath  ./../../..)

export BL_SDK_BASE

BOARD ?= ${CHIP}dk

CONFIG_OT_FTD = y
CONFIG_OT_CLI = shell
CONFIG_OTBR_REST ?= y

include $(BL_SDK_BASE)/project.build
