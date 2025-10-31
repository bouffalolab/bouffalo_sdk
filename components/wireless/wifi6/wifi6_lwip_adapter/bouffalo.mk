# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += include

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := .

COMPONENT_SRCS := \
    net_al.c \
    net_al_ext.c \
    net_tg_al.c \
    net_iperf_al.c \
    net_tg_al_priv.c \
    net_ping_al_priv.c \
    wifi_pkt_hooks.c \
    cli_al.c \
    tx_buffer_copy.c \

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := \
    . \

CFLAGS += -DIOT_SDK_ADAPTER

ifeq ($(CONFIG_IPV6),1)
CFLAGS += -D CFG_IPV6
endif

ifeq ($(CONFIG_FHOST_BUFFER_COPY),1)
CPPFLAGS += -D FHOST_BUFFER_COPY
endif

##
#CPPFLAGS +=
#CFLAGS += -DCONFIG_BLE_AT_CMD
