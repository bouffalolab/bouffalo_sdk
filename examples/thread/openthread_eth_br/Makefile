SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= $(abspath  ./../../..)

export BL_SDK_BASE

BOARD ?= ${CHIP}dk
OTBR_REST ?= 1
CONFIG_LMAC154_CMD ?= 0
CONFIG_LMAC154_LOG ?= 0

CROSS_COMPILE ?= riscv64-unknown-elf-

# cmake_definition+=-DCONFIG_LMAC154_LOG=1

cmake_definition+=-DOT_FTD=1
cmake_definition+=-DOT_CLI=1
cmake_definition+=-DOT_SERIAL=SYS_CLI

cmake_definition+=-DCONFIG_SHELL_CMD_SIZE=256
cmake_definition+=-DCONFIG_SHELL=1

cmake_definition+=-DCONFIG_LMAC154_CMD=${CONFIG_LMAC154_CMD}
cmake_definition+=-DCONFIG_LMAC154_LOG=${CONFIG_LMAC154_LOG}

cmake_definition+=-DCONFIG_OTBR_REST=${OTBR_REST}
cmake_definition+=-DOPENTHREAD_PROJECT_CORE_CONFIG_FILE=\"openthread-core-proj-config.h\"
cmake_definition+=-DOPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE=\"openthread-core-platform-config-check.h\"

include $(BL_SDK_BASE)/project.build
