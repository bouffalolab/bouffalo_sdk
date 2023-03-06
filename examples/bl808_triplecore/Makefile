build_dirs += helloworld_m0
build_dirs += helloworld_d0
build_dirs += helloworld_lp

CHIP ?= bl808

all: $(build_dirs)

$(build_dirs):
	make -C $@

.PHONY: $(build_dirs)

BL_SDK_BASE = ./../..

export BL_SDK_BASE

include $(BL_SDK_BASE)/project.build