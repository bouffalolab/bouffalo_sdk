# Component Makefile
#

## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS    += include

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src 
COMPONENT_SRCS := efuse_slot.c \
                  matter_factory_data.c

COMPONENT_OBJS := $(patsubst %.cpp,%.o, $(filter %.cpp,$(COMPONENT_SRCS))) $(patsubst %.c,%.o, $(filter %.c,$(COMPONENT_SRCS))) $(patsubst %.S,%.o, $(filter %.S,$(COMPONENT_SRCS)))
COMPONENT_SRCDIRS := .