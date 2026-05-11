SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= $(abspath  ./../../..)

export BL_SDK_BASE

BOARD ?= ${CHIP}dk
ifeq ($(CHIP), bl618dg)
	CPU_ID ?= ap
endif



include $(BL_SDK_BASE)/project.build
