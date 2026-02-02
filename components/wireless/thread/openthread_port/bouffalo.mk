# Component Makefile
#

include $(COMPONENT_PATH)/../openthread_common.mk

## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS := ../openthread_utils/include

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := ../openthread_utils

COMPONENT_SRCS := ../openthread_utils/ot_frame.c ../openthread_utils/ot_linkmetric.c ../openthread_utils/ot_utils.c

ifeq ($(CONFIG_CHIP_NAME), BL702)
    CPPFLAGS += -DOT_UTILS_H=\"ot_utils_bl702.h\"
else ifeq ($(CONFIG_CHIP_NAME), BL702L)
    CPPFLAGS += -DOT_UTILS_H=\"ot_utils_bl702l.h\"
endif

COMPONENT_OBJS := $(patsubst %.cpp,%.o, $(filter %.cpp,$(COMPONENT_SRCS))) $(patsubst %.c,%.o, $(filter %.c,$(COMPONENT_SRCS))) $(patsubst %.S,%.o, $(filter %.S,$(COMPONENT_SRCS)))
COMPONENT_SRCDIRS := ../openthread_utils

# CPPFLAGS += -DOT_CFG_HWACK_CFG=1
CPPFLAGS += -D$(CONFIG_CHIP_NAME)
