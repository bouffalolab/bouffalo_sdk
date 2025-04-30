
include $(COMPONENT_PATH)/../openthread_common.mk

# src/ncp 
otlib := ../openthread/src
otlib_module_srcdir := ncp
otlib_module_src := \
    changed_props_set.cpp                           \
    changed_props_set.hpp                           \
    ncp_base.cpp                                    \
    ncp_base.hpp                                    \
    ncp_base_ftd.cpp                                \
    ncp_base_mtd.cpp                                \
    ncp_base_radio.cpp                              \
    ncp_base_dispatcher.cpp                         \
    ncp_config.h                                    \
    ncp_spi.cpp                                     \
    ncp_spi.hpp                                     \
    ncp_hdlc.cpp                                    \
    ncp_hdlc.hpp                                    \

otlib_module_srcdir := $(addprefix $(otlib)/,${otlib_module_srcdir})
otlib_module_src := $(addprefix $(otlib_module_srcdir)/,${otlib_module_src}) 

otlib_module_inc := $(otlib_module_srcdir)

COMPONENT_SRCS := $(otlib_module_src)
COMPONENT_SRCDIRS := $(otlib_module_srcdir) 
COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_ADD_INCLUDEDIRS) $(otlib_module_inc) 
COMPONENT_PRIV_INCLUDEDIRS :=

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
COMPONENT_PRIV_INCLUDEDIRS :=

# src/lib/spinel
otlib := ../openthread/src/lib
otlib_module_srcdir := spinel
otlib_module_src := \
    spinel.c                                        \
    spinel_buffer.cpp                               \
    spinel_decoder.cpp                              \
    spinel_encoder.cpp                              \

otlib_module_srcdir := $(addprefix $(otlib)/,${otlib_module_srcdir})
otlib_module_src := $(addprefix $(otlib_module_srcdir)/,${otlib_module_src}) 

otlib_module_inc := $(otlib_module_srcdir)

COMPONENT_SRCS += $(otlib_module_src)
COMPONENT_SRCDIRS += $(otlib_module_srcdir) 
COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_ADD_INCLUDEDIRS) $(otlib_module_inc) 
COMPONENT_PRIV_INCLUDEDIRS :=

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
COMPONENT_PRIV_INCLUDEDIRS :=

COMPONENT_OBJS := $(patsubst %.cpp,%.o, $(filter %.cpp,$(COMPONENT_SRCS))) $(patsubst %.c,%.o, $(filter %.c,$(COMPONENT_SRCS))) $(patsubst %.S,%.o, $(filter %.S,$(COMPONENT_SRCS)))

CPPFLAGS += -Wimplicit-fallthrough=0
CPPFLAGS += -Wno-switch-default
CPPFLAGS += -Wno-format
