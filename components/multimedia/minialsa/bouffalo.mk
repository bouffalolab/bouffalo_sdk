# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += include

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src
COMPONENT_SRCS :=   src/core.c         \
                    src/mixer.c        \
                    src/pcm.c          \
                    src/snd.c          \

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src

CPPFLAGS += -DCONFIG_USE_MINIALSA=1
