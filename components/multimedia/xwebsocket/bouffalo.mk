# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += include

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src
COMPONENT_SRCS :=        src/librws.c \
                         src/rws_common.c \
                         src/rws_error.c \
                         src/rws_frame.c \
                         src/rws_list.c \
                         src/rws_memory.c \
                         src/rws_network.c \
                         src/rws_socketpriv.c \
                         src/rws_socketpub.c \
                         src/rws_string.c \
                         src/port/rws_msp.c \

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src src/port
