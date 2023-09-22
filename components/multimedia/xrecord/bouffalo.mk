# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += include         \
                             src/recio \
                             src/record   \
                             src/utils  \

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := 

## This component's src
COMPONENT_SRCS =    utils/uri_parse.c  \
                    recio/recio.c \
                    recio/ws.c \
                    recio/ringb.c \
                    recio/null.c \
                    recio/mic.c \
                    record/record.c \
                    cli/cli_record.c \

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := utils recio record cli

##
#CPPFLAGS +=
