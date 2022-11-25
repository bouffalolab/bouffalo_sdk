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
COMPONENT_SRCS := src/bflb_gpio.c    \
				  src/bflb_irq.c     \
				  src/bflb_l1c.c     \
				  src/bflb_mtimer.c  \
				  src/bflb_sec_aes.c \
				  src/bflb_uart.c    \

CHIP_NAME = $(shell echo $(CONFIG_CHIP_NAME) | tr A-Z a-z)

COMPONENT_SRCS += config/$(CHIP_NAME)/device_table.c
COMPONENT_ADD_INCLUDEDIRS += config/$(CHIP_NAME)

CFLAGS += -D$(CONFIG_CHIP_NAME)
CFLAGS += -DCONFIG_IRQ_NUM=80
CFLAGS += -DBL_IOT_SDK

ifeq ($(CONFIG_CHIP_NAME),BL616)
COMPONENT_SRCS += include/arch/risc-v/t-head/rv_hart.c
COMPONENT_SRCS += include/arch/risc-v/t-head/rv_pmp.c

ifeq ($(CONFIG_CHERRYUSB),1)
COMPONENT_SRCS += src/bflb_usb_v2.c
endif

endif

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src include/arch/risc-v/t-head config/$(CHIP_NAME)