# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += include        \
                             include/bt     \

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src
COMPONENT_SRCS :=   src/smart_audio.c                    \
                    src/smart_audio_cli.c                \
                    src/aud_ctrl_case/local_play.c       \
                    src/aud_ctrl_case/online_music.c     \

ifeq ($(CONFIG_BT_A2DP),1)
COMPONENT_SRCS :=   src/aud_ctrl_case/bt_a2dp.c          \
                    src/aud_ctrl_case/bt_vol.c
endif

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src src/aud_ctrl_case

CPPFLAGS += -DCONFIG_SMART_AUDIO_STACK_SIZE=1024
CPPFLAGS += -DCONFIG_SMART_AUDIO=1

ifeq ($(CONFIG_BT_A2DP),1)
CPPFLAGS += -DCONFIG_BT_A2DP=1
endif
