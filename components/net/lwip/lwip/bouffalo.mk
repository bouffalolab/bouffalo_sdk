# Component Makefile

ifeq ($(CONFIG_ETHERNET), 1)
CFLAGS   += -DCFG_ETHERNET_ENABLE
CPPFLAGS += -DCFG_ETHERNET_ENABLE
endif

ifeq ($(CONFIG_NETBUS_WIFI_ENABLE),1)
CFLAGS += -DCFG_NETBUS_WIFI_ENABLE
endif
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += src/include src/include/compat/posix lwip-port lwip-port/FreeRTOS lwip-port/arch

ifneq ($(CONFIG_CHIP_NAME),BL616)
# bl616 lwipopt.h define in wifi6_lwip_adapter
COMPONENT_ADD_INCLUDEDIRS += lwip-port/config
else
# bl616 need lwip errno.h
COMPONENT_ADD_INCLUDEDIRS += src/include/compat/stdc
endif

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src

COMPONENT_SRCDIRS := src/api src/core src/core/ipv4 src/netif lwip-port/FreeRTOS lwip-port src/apps/altcp_tls
COMPONENT_SRCDIRS += src/apps/sntp

ifeq ($(CONFIG_LWIP_NETCONN_DUPLEX),1)
CFLAGS += -DLWIP_NETCONN_DUPLEX_SWITCH
endif
ifeq ($(CONFIG_ENABLE_OS_TLS),1)
CFLAGS += -Dconfig_ENABLE_OS_TLS_SWITCH
endif

ifeq ($(CONFIG_IPV6), 1)
COMPONENT_SRCDIRS += src/core/ipv6
CPPFLAGS += -D CFG_IPV6
endif

ifeq ($(CONFIG_OTBR),1)
## Openthread Border Router has its own src/core/ipv6/mld6.c
COMPONENT_SRCS := $(foreach compsrcdir,$(COMPONENT_SRCDIRS),$(wildcard $(COMPONENT_PATH)/$(compsrcdir)/*.c))
COMPONENT_SRCS := $(subst $(COMPONENT_PATH)/,,$(COMPONENT_SRCS))
COMPONENT_SRCS := $(filter-out src/core/ipv6/mld6.c,$(COMPONENT_SRCS))
COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
CFLAGS += -DOPENTHREAD_BORDER_ROUTER
endif

##
#CPPFLAGS +=

ifeq ($(CONFIG_LWIP_PE_OPT),1)
CFLAGS += -msave-restore
endif

ifeq ($(CONFIG_MATTER_SUPPORT),1)
CPPFLAGS += -DMATTER_SUPPORT
endif
