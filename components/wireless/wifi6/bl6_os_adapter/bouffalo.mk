# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += include
							 
## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=		 

## This component's src 
COMPONENT_SRCS := src/rtos_al.c
				  
COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src

# CFLAGS   += -DCFG_FREERTOS 
##
#CPPFLAGS += 
