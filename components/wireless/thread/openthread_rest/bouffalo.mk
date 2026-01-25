# Component Makefile
#

include $(COMPONENT_PATH)/../openthread_common.mk


## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS :=
COMPONENT_ADD_INCLUDEDIRS += include

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := .

## This component's src 
COMPONENT_SRCS := openthread_httpd.c \
                  openthread_rest.c

COMPONENT_OBJS := $(patsubst %.cpp,%.o, $(filter %.cpp,$(COMPONENT_SRCS))) $(patsubst %.c,%.o, $(filter %.c,$(COMPONENT_SRCS))) $(patsubst %.S,%.o, $(filter %.S,$(COMPONENT_SRCS)))
COMPONENT_SRCDIRS := .

CPPFLAGS += -D$(CONFIG_CHIP_NAME)
