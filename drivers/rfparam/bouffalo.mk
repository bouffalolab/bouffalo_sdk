COMPONENT_ADD_INCLUDEDIRS += Inc 
COMPONENT_SRCS := Src/rfparam_adapter.c \
				  Src/rfparam_rftlv.c

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
COMPONENT_OBJS := $(patsubst %.S,%.o, $(COMPONENT_OBJS))                  


COMPONENT_SRCDIRS := Src

ifeq ($(CONFIG_BLE_MFG),1)
CPPFLAGS += -DCONFIG_BLE_MFG
endif
ifeq ($(CONFIG_CHIP_NAME),BL616)
CPPFLAGS += -DRFPARAM_BL616
endif

ifeq ($(CONFIG_CHIP_NAME),WB03)
CPPFLAGS += -DRFPARAM_WB03
endif

