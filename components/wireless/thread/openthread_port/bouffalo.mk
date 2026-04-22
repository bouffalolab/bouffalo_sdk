# Component Makefile
#

include $(COMPONENT_PATH)/../openthread_common.mk

COMPONENT_PRIV_INCLUDEDIRS := .

COMPONENT_SRCS := ot_alarm.c ot_freertos.c ot_link_metric.c ot_radio.c ot_sys_iot.c ot_uart_iot.c 

ifndef OT_NCP
	COMPONENT_SRCS := ${COMPONENT_SRCS} ot_logging.c
endif

ifeq (${CONFIG_EASYFLASH_ENABLE}, 1)
	COMPONENT_SRCS := ${COMPONENT_SRCS} ot_settings_easyflash.c
else ifeq (${CONFIG_LITTLEFS}, 1)
	COMPONENT_SRCS := ${COMPONENT_SRCS} ot_settings_littlefs.c
	CPPFLAGS += -DLFS_THREADSAFE -DCONFIG_FREERTOS
else 
	$(error "Need storage setting: CONFIG_EASYFLASH_ENABLE or CONFIG_LITTLEFS")
endif

ifeq (${CONFIG_OTBR}, 1)
	COMPONENT_SRCS := ${COMPONENT_SRCS} otbr_rtos_lwip.c
    CPPFLAGS += -DOPENTHREAD_BORDER_ROUTER
endif

ifeq ($(CONFIG_USB_CDC),1)
    CPPFLAGS += -DCFG_USB_CDC_ENABLE
endif

ifeq ($(CONFIG_SYS_AOS_LOOP_ENABLE),1)
    CPPFLAGS += -DSYS_AOS_LOOP_ENABLE
endif

ifeq ($(CONFIG_SYS_AOS_CLI_ENABLE),1)
    ifdef CONFIG_PREFIX
        CPPFLAGS += -DCFG_PREFIX=\"${CONFIG_PREFIX}\"
    endif
endif

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

COMPONENT_OBJS := $(patsubst %.cpp,%.o, $(filter %.cpp,$(COMPONENT_SRCS))) $(patsubst %.c,%.o, $(filter %.c,$(COMPONENT_SRCS))) $(patsubst %.S,%.o, $(filter %.S,$(COMPONENT_SRCS)))

CPPFLAGS += -D$(CONFIG_CHIP_NAME)
