# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += include

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := src

## This component's src
COMPONENT_SRCS :=   src/event_svr.c      \
                    src/event.c      	 \
                    src/rpc.c            \
                    src/uservice.c       \
                    src/utask.c          \

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src

CPPFLAGS += -DCONFIG_USERVICE_EVENT_TASK_STACK_SIZE=2048
