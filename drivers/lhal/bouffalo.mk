# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += include
COMPONENT_ADD_INCLUDEDIRS += include/arch
COMPONENT_ADD_INCLUDEDIRS += include/arch/risc-v/t-head
COMPONENT_ADD_INCLUDEDIRS += include/arch/risc-v/t-head/Core/Include

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src
COMPONENT_SRCS := src/bflb_uart.c  \
				  src/bflb_gpio.c  \

ifeq ($(CONFIG_CHIP_NAME),BL602)
CFLAGS   += -DBL602
endif

ifeq ($(CONFIG_CHIP_NAME),BL616)
CFLAGS   += -DBL616
COMPONENT_SRCS += include/arch/risc-v/t-head/rv_hart.c
COMPONENT_SRCS += include/arch/risc-v/t-head/rv_pmp.c
COMPONENT_SRCS += config/bl616/device_table.c
endif

ifeq ($(CONFIG_CHIP_NAME),BL702)
CFLAGS   += -DBL702
COMPONENT_SRCS += config/bl702/device_table.c
endif

ifeq ($(CONFIG_CHIP_NAME),BL702L)
CFLAGS   += -DBL702L
endif

ifeq ($(CONFIG_CHIP_NAME),BL808)
CFLAGS   += -DBL808
endif

ifeq ($(CONFIG_CHIP_NAME),BL606P)
CFLAGS   += -DBL606P
endif

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src include/arch/risc-v/t-head config/bl616
