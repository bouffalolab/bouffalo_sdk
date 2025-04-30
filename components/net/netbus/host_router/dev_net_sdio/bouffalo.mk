# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += .
COMPONENT_ADD_INCLUDEDIRS += adapter

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src
COMPONENT_SRCS := $(wildcard *.c) 

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_OBJS := $(patsubst %.cpp,%.o, $(COMPONENT_OBJS))

COMPONENT_SRCDIRS := .
COMPONENT_SRCDIRS += adapter

ifeq ($(CONFIG_DHCP_IN_EMB),1)
CPPFLAGS += -D DHCP_IN_EMB
endif
CFLAGS += -DBL_IOT_SDK 

