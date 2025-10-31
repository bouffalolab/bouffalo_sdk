# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += inc

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src
COMPONENT_SRCS += src/rnm_utils.c \
                  src/rnm_common.c

ifeq ($(CONFIG_CPU_E907),1)
RNM_SERVER := 1
endif
ifeq ($(CONFIG_SDIOWIFI),1)
RNM_SERVER := 1
endif
ifeq ($(CONFIG_CHIP_NAME),BL616)
RNM_SERVER := 1
endif

ifeq ($(CONFIG_CPU_C906),1)
RNM_CLIENT := 1
endif

ifeq ($(RNM_SERVER),1)
COMPONENT_SRCS += src/rnm_server.c \
                  src/rnm_server_cmd_handler.c \
                  src/rnm_ota.c \
                  src/rnm_user_ext.c
endif

ifeq ($(RNM_CLIENT),1)
COMPONENT_SRCS += src/rnm_client.c \
                  src/rnm_client_cmds.c
endif

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src

ifeq ($(CONFIG_DHCP_IN_EMB),1)
CPPFLAGS += -D DHCP_IN_EMB
endif 
CFLAGS += -DBL_IOT_SDK

