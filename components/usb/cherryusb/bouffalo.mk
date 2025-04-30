# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += core common osal class/cdc class/hid class/msc class/video class/audio class/hub
COMPONENT_ADD_INCLUDEDIRS += ./
## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=
## This component's src
COMPONENT_SRCS := core/usbd_core.c \
                  class/cdc/usbd_cdc.c \
                  class/hid/usbd_hid.c \
                  class/msc/usbd_msc.c \
                  class/video/usbd_video.c \
                  class/audio/usbd_audio.c \

COMPONENT_SRCDIRS := core class/cdc class/hid class/msc class/video class/audio

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))
