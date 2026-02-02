SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= $(abspath  ./../../..)

export BL_SDK_BASE

BOARD ?= ${CHIP}dk
CROSS_COMPILE ?= riscv64-unknown-elf-
CONFIG_LMAC154_DBG ?= n

OT_FTD ?= 1 
OT_SERIAL ?= SYS_CLI

# add custom cmake definition
cmake_definition+=-DOT_FTD=${OT_FTD}
cmake_definition+=-DOT_CLI=1

ifeq ("$(OT_SERIAL)", "SYS_CLI")
  cmake_definition+=-DCONFIG_SHELL_CMD_SIZE=256
  cmake_definition+=-DCONFIG_SHELL=1
endif
cmake_definition+=-DOT_SERIAL=${OT_SERIAL}

cmake_definition+=-DOPENTHREAD_PROJECT_CORE_CONFIG_FILE=\"openthread-core-proj-config.h\"
cmake_definition+=-DOPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE=\"openthread-core-platform-config-check.h\"

include $(BL_SDK_BASE)/project.build
