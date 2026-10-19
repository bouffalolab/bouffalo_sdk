SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= $(abspath  ./../../..)

export BL_SDK_BASE

BOARD ?= ${CHIP}dk

CONFIG_OT_NCP ?= n
ifeq ($(CONFIG_OT_NCP),y)
CONFIG_OT_RCP ?= n
else
CONFIG_OT_RCP ?= y
endif
CONFIG_OT_FTD ?= n
CONFIG_OT_MTD ?= n

include $(BL_SDK_BASE)/project.build
