# Component Makefile
#

## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS    += include

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := ./

## This component's src 
COMPONENT_SRCS := bl_mfd.c 

COMPONENT_OBJS := $(patsubst %.cpp,%.o, $(filter %.cpp,$(COMPONENT_SRCS))) $(patsubst %.c,%.o, $(filter %.c,$(COMPONENT_SRCS))) $(patsubst %.S,%.o, $(filter %.S,$(COMPONENT_SRCS)))
COMPONENT_SRCDIRS := .

ifeq ($(CONFIG_CHIP_NAME),BL602)
CFLAGS += -DBL_MFD_PLAT_H=\"bl_mfd_bl602.h\"
COMPONENT_SRCS += bl_mfd_decrypt.c 
else ifeq ($(CONFIG_CHIP_NAME),BL702)
CFLAGS += -DBL_MFD_PLAT_H=\"bl_mfd_bl702.h\"
COMPONENT_SRCS += bl_mfd_decrypt.c 
else ifeq ($(CONFIG_CHIP_NAME),BL702L)
CFLAGS += -DBL_MFD_PLAT_H=\"bl_mfd_bl702l.h\"
COMPONENT_SRCS += bl_mfd_decrypt.c 
else
$(error "No chip specified.")
endif