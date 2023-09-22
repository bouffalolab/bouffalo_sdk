# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += include         \
                             src/bl616/drv   \

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS :=   src/bl616/codec         \
                                src/bl616/drv           \
                                src/bl616/port          \

## This component's src
COMPONENT_SRCS :=   src/bl616/codec/codec_debug.c       \
                    src/bl616/codec/codec_eqdrc.c       \
                    src/bl616/codec/codec_input.c       \
                    src/bl616/codec/codec_output.c      \
                    src/bl616/codec/codec_volume.c      \
                    src/bl616/codec/xcodec.c            \
                    src/bl616/drv/msp_bl616_auadc.c     \
                    src/bl616/drv/msp_bl616_aupwm.c     \
                    src/bl616/drv/msp_bl616_dma.c       \
                    src/bl616/drv/msp_bl616_glb.c       \
                    src/bl616/drv/msp_bl616_glb_gpio.c  \
                    src/bl616/port/msp_sys.c            \
                    src/bl616/codec/test/test_output.c  \

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := src/bl616/codec src/bl616/drv src/bl616/port src/bl616/codec/test/
