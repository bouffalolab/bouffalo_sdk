SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= $(abspath  $(SDK_DEMO_PATH)/../../../../..)

export BL_SDK_BASE

BL_SDK_BASE_PATH = $(realpath $(BL_SDK_BASE))

CHIP ?= bl618dg
BOARD ?= bl618dgdk
CPU_ID ?= ap


include $(BL_SDK_BASE)/project.build
