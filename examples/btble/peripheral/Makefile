SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= $(abspath  ./../../..)

export BL_SDK_BASE


ifeq ($(CONFIG_BTBLE_USE_STANDALONE_PATH),1)
    cmake_definition+=-DCONFIG_BTBLE_USE_STANDALONE_PATH=1
endif

include $(BL_SDK_BASE)/project.build
