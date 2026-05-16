SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= $(abspath  ./../../..)

export BL_SDK_BASE

BOARD ?= ${CHIP}dk

CONFIG_OT_FTD ?= y
CONFIG_OT_MTD ?= n
CONFIG_OT_CLI ?= shell
# CONFIG_OT_CLI ?= uart
CONFIG_LMAC154_DBG ?= 0

ifeq ("$(CONFIG_OT_CLI)", "shell")
  CONFIG_SHELL =y
  CONFIG_SHELL_CMD_SIZE =256
endif

include $(BL_SDK_BASE)/project.build
