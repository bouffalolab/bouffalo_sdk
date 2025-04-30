# Component Makefile
#

include $(COMPONENT_PATH)/../openthread_common.mk


## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS :=
COMPONENT_ADD_INCLUDEDIRS += include
COMPONENT_ADD_INCLUDEDIRS += ../openthread/include ../openthread/src/core ../openthread/src ../openthread/examples/platforms

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := .

ifeq ($(CONFIG_PDS_ENABLE),1)
    CONFIG_PDS_LEVEL ?= 31
    CPPFLAGS += -DCFG_PDS_LEVEL=$(CONFIG_PDS_LEVEL)
    CPPFLAGS += -DCFG_PDS_ENABLE
    ifeq ($(origin CONFIG_DATA_POLL_CSMA), undefined)
        CPPFLAGS += -DCFG_DATA_POLL_CSMA=1
    else
        CPPFLAGS += -DCFG_DATA_POLL_CSMA=${CONFIG_DATA_POLL_CSMA}
    endif
endif

## This component's src 
COMPONENT_SRCS := ot_freertos.c \
                  ot_radio.c \
                  ot_sys.c \

ifeq ($(CONFIG_CHIP_NAME), BL702)
    COMPONENT_SRCS += ot_alarm_bl702.c
    COMPONENT_SRCS += ot_radio_bl70xx.c
else ifeq ($(CONFIG_CHIP_NAME), BL702L)
    COMPONENT_SRCS += ot_radio_bl70xx.c
    COMPONENT_SRCS += ot_alarm_bl702l.c
    ifneq ($(CONFIG_BUILD_ROM_CODE),1)
        CPPFLAGS += -DCFG_OT_USE_ROM_CODE
    endif
endif

ifeq ($(CONFIG_LITTLEFS), 1)
    COMPONENT_SRCS += ot_settings_littlefs.c
    CPPFLAGS += -DLFS_THREADSAFE -DCONFIG_FREERTOS
else ifeq ($(CONFIG_EASYFLASH_ENABLE), 1)
    COMPONENT_SRCS += ot_settings_easyflash.c
endif

ifndef OT_NCP
COMPONENT_SRCS += ot_logging.c
endif
COMPONENT_SRCS += ot_uart.c

ifeq ($(CONFIG_OTBR),1)
    COMPONENT_SRCS += otbr_lwip.c
    CPPFLAGS += -DOPENTHREAD_BORDER_ROUTER
endif

COMPONENT_OBJS := $(patsubst %.cpp,%.o, $(filter %.cpp,$(COMPONENT_SRCS))) $(patsubst %.c,%.o, $(filter %.c,$(COMPONENT_SRCS))) $(patsubst %.S,%.o, $(filter %.S,$(COMPONENT_SRCS)))
COMPONENT_SRCDIRS := .

ifeq ($(CONFIG_SYS_AOS_LOOP_ENABLE),1)
    CPPFLAGS += -DSYS_AOS_LOOP_ENABLE
endif

ifeq ($(CONFIG_SYS_AOS_CLI_ENABLE),1)
    CPPFLAGS += -DSYS_AOS_CLI_ENABLE
    ifdef CONFIG_PREFIX
        CPPFLAGS += -DCFG_PREFIX=\"${CONFIG_PREFIX}\"
    endif
endif

ifeq ($(CONFIG_USB_CDC),1)
    CPPFLAGS += -DCFG_USB_CDC_ENABLE
endif

ifeq ($(OPENTHREAD_TESTS_UNIT),1)
    CPPFLAGS += -DCFG_OPENTHREAD_TESTS_UNIT
endif

ifeq ($(CONFIG_USE_PSRAM), 1)
    CPPFLAGS += -DCFG_USE_PSRAM=1
endif

CPPFLAGS += -D$(CONFIG_CHIP_NAME)
