build_dirs += wifi_ap
build_dirs += wifi_np

CHIP ?= bl616d

all: $(build_dirs)

$(build_dirs):
	make -C $@

clean_all:
	@for dir in $(build_dirs); do \
		$(MAKE) -C $$dir clean; \
	done

.PHONY: $(build_dirs) clean_all

BL_SDK_BASE = ./../../..

export BL_SDK_BASE

-include $(SDK_DEMO_PATH)/.config
include $(BL_SDK_BASE)/project.build
