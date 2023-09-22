# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += include

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src
COMPONENT_SRCS :=   src/device.c              \
                    src/debug.c               \
                    src/list.c                \
                    src/mringbuffer.c         \
                    src/xringbuffer.c         \
                    src/select.c              \

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src

CPPFLAGS += -DCONFIG_MSP_DEBUG=1
