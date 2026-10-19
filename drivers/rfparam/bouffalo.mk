COMPONENT_ADD_INCLUDEDIRS += Inc 
COMPONENT_SRCS := Src/rfparam_adapter.c \
				  Src/rfparam_rftlv.c

ifeq ($(CONFIG_CHIP_NAME),BL616CL)
COMPONENT_SRCS += Src/rfparam_bl616cl_flash_otp.c
endif

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_OBJS := $(patsubst %.S,%.o, $(COMPONENT_OBJS))                  


COMPONENT_SRCDIRS := Src

ifeq ($(CONFIG_BLE_MFG),1)
CPPFLAGS += -DCONFIG_BLE_MFG
endif
ifeq ($(CONFIG_CHIP_NAME),BL616)
CPPFLAGS += -DRFPARAM_BL616
endif
