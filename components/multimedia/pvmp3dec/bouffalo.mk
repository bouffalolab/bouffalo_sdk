COMPONENT_LIB_ONLY := 1
COMPONENT_ADD_INCLUDEDIRS += include
ifeq ($(CONFIG_MP3_DECODER_MONO),1)
LIBS ?= pvmp3dec_mono_prebuild
else
LIBS ?= pvmp3dec_prebuild
endif
COMPONENT_ADD_LDFLAGS += -L$(COMPONENT_PATH)/libs/e907fp $(addprefix -l,$(LIBS))
ALL_LIB_FILES := $(patsubst %,$(COMPONENT_PATH)/libs/e907fp/lib%.a,$(LIBS))
COMPONENT_ADD_LINKER_DEPS := $(ALL_LIB_FILES)