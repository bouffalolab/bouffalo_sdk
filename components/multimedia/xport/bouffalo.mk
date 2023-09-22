# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += include

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=

## This component's src
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
                    src/xav_parser_all.c        \

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src
