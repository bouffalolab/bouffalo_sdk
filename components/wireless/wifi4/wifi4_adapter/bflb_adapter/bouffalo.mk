# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += bflb_os_adapter/include bflb_os_adapter/include/bflb_os_adapter bflb_os_adapter
							 
## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=		 

## This component's src 
COMPONENT_SRCS := bflb_os_adapter/bflb_os_hal.c
				  
COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := bflb_os_adapter

CFLAGS   += -DCFG_FREERTOS 
##
#CPPFLAGS += 
