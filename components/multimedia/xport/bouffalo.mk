# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += include

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src
ifeq ($(CONFIG_MSP_ENABLE),1)
COMPONENT_SRCS :=   src/msp_port.c              \
                    src/msp_freertos.c          \
                    src/msp_posixfs.c           \
                    src/web_lower.c             \
                    src/xav_port.c              \
                    src/xav_avcodec_all.c       \
                    src/xav_resample_all.c      \
                    src/xav_stream_all.c        \
                    src/xav_atempo_all.c        \
                    src/xav_aef_all.c           \
                    src/xav_format_all.c        \
                    src/xav_parser_all.c        
else
ifeq ($(CONFIG_MSP_CODEC),1)
COMPONENT_SRCS :=   src/msp_port.c              \
                    src/msp_freertos.c          \
                    src/xav_port.c              
endif
endif

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src
