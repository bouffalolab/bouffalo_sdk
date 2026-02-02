SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= $(abspath  ./../../..)

export BL_SDK_BASE

BOARD ?= ${CHIP}dk
ifeq ($(CHIP), bl616d)
	CPU_ID ?= ap
endif

CROSS_COMPILE ?= riscv64-unknown-elf-


include $(BL_SDK_BASE)/project.build
