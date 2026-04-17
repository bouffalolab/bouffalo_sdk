
include $(COMPONENT_PATH)/../openthread_common.mk

COMPONENT_PRIV_INCLUDEDIRS := ${COMPONENT_PRIV_INCLUDEDIRS} ../openthread/src/include ../openthread/src

# src/ncp 
otlib := ../openthread/src
otlib_module_srcdir := ncp
otlib_module_src := \
    changed_props_set.cpp \
    multipan_platform.cpp \
    ncp_base.cpp \
    ncp_base_dispatcher.cpp \
    ncp_base_radio.cpp \
    ncp_spi.cpp \
    ncp_hdlc.cpp \
    platform/dnssd.cpp \
    platform/infra_if.cpp

ifeq (${OT_NCP}, 1)
    otlib_module_src := ${otlib_module_src} ncp_base_ftd.cpp ncp_base_mtd.cpp
    CPPFLAGS += -DOPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE=1
else
    CPPFLAGS += -DOPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE=0
endif

otlib_module_srcdir := $(addprefix $(otlib)/,${otlib_module_srcdir})
otlib_module_src := $(addprefix $(otlib_module_srcdir)/,${otlib_module_src}) 

otlib_module_srcdir := $(otlib_module_srcdir) $(otlib_module_srcdir)/platform

otlib_module_inc := $(otlib_module_srcdir)

COMPONENT_SRCS := $(otlib_module_src)
COMPONENT_SRCDIRS := $(otlib_module_srcdir) 
COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_ADD_INCLUDEDIRS) $(otlib_module_inc) 
COMPONENT_PRIV_INCLUDEDIRS := ${COMPONENT_PRIV_INCLUDEDIRS}

# src/lib/hdlc
otlib := ../openthread/src/lib
otlib_module_srcdir := hdlc
otlib_module_src := hdlc.cpp
otlib_module_srcdir := $(addprefix $(otlib)/,${otlib_module_srcdir})
otlib_module_src := $(addprefix $(otlib_module_srcdir)/,${otlib_module_src}) 

otlib_module_inc := $(otlib_module_srcdir)

COMPONENT_SRCS += $(otlib_module_src)
COMPONENT_SRCDIRS += $(otlib_module_srcdir) 
COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_ADD_INCLUDEDIRS) $(otlib_module_inc) 
COMPONENT_PRIV_INCLUDEDIRS := ${COMPONENT_PRIV_INCLUDEDIRS}

# src/lib/spinel
otlib := ../openthread/src/lib
otlib_module_srcdir := spinel
otlib_module_src := \
    spinel.c \
    spinel_buffer.cpp \
    spinel_decoder.cpp \
    spinel_encoder.cpp \
    spinel_helper.cpp \
    spinel_prop_codec.cpp

otlib_module_srcdir := $(addprefix $(otlib)/,${otlib_module_srcdir})
otlib_module_src := $(addprefix $(otlib_module_srcdir)/,${otlib_module_src}) 

otlib_module_inc := $(otlib_module_srcdir)

COMPONENT_SRCS += $(otlib_module_src)
COMPONENT_SRCDIRS += $(otlib_module_srcdir) 
COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_ADD_INCLUDEDIRS) $(otlib_module_inc) 
COMPONENT_PRIV_INCLUDEDIRS := ${COMPONENT_PRIV_INCLUDEDIRS}

# examples/apps/ncp
otlib := ../openthread/examples/apps
otlib_module_srcdir := ncp
otlib_module_src := \
    ncp.c

otlib_module_srcdir := $(otlib)/${otlib_module_srcdir}
otlib_module_src := $(addprefix $(otlib_module_srcdir)/,${otlib_module_src}) 

otlib_module_inc := $(otlib_module_srcdir)
COMPONENT_SRCS += $(otlib_module_src)
COMPONENT_SRCDIRS += $(otlib_module_srcdir)
COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_ADD_INCLUDEDIRS) $(otlib_module_inc) $(otlib_module_srcdir)/platforms/utils
COMPONENT_PRIV_INCLUDEDIRS := ${COMPONENT_PRIV_INCLUDEDIRS}

COMPONENT_OBJS := $(patsubst %.cpp,%.o, $(filter %.cpp,$(COMPONENT_SRCS))) $(patsubst %.c,%.o, $(filter %.c,$(COMPONENT_SRCS))) $(patsubst %.S,%.o, $(filter %.S,$(COMPONENT_SRCS)))

CPPFLAGS += -Wimplicit-fallthrough=0
CPPFLAGS += -Wno-switch-default
CPPFLAGS += -Wno-format
