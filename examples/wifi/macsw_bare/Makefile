SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= $(abspath  ./../../../)

export BL_SDK_BASE



include $(BL_SDK_BASE)/project.build

ifeq ($(CONFIG_GCOV_DUMP),y)
CFLAGS := $(filter-out -fno-inline -fno-lto,$(CFLAGS)) -fno-inline -fno-lto
CXXFLAGS := $(filter-out -fno-inline -fno-lto,$(CXXFLAGS)) -fno-inline -fno-lto
export CFLAGS CXXFLAGS
endif
