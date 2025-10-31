# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += \
    include

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := . macsw

COMPONENT_SRCS := \
    wl80211.c \
    timeout.c \
    rtos_al.c \
    cntrl.c \
    lwip.c \
    wifi_mgmr.c \
    invariants.c \
    macsw/macif_wl80211.c \
    macsw/macsw.c \
    macsw/rx.c \
    macsw/tx.c \
    macsw/scan.c \
    macsw/connect.c \
    macsw/disconnect.c \
    macsw/supplicant_api.c \
    macsw/ps.c

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_OBJS := $(patsubst %.cpp,%.o, $(COMPONENT_OBJS))

COMPONENT_SRCDIRS := \
    macsw \
    . \
