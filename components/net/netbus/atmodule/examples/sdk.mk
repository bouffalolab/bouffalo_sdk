ifeq ($(strip $(ATMODULE_PATH)),)
$(error ATMODULE_PATH must be set before including examples/sdk.mk)
endif
ifeq ($(wildcard $(ATMODULE_PATH)/BOUFFALO_SDK_VERSION),)
$(error SDK version file not found: $(ATMODULE_PATH)/BOUFFALO_SDK_VERSION)
endif
ATMODULE_SDK_VERSION := $(strip $(shell cat "$(ATMODULE_PATH)/BOUFFALO_SDK_VERSION"))
ifeq ($(ATMODULE_SDK_VERSION),)
$(error SDK version file is empty: $(ATMODULE_PATH)/BOUFFALO_SDK_VERSION)
endif

ifneq ($(wildcard $(ATMODULE_PATH)/bouffalo_sdk/project.build),)
BL_SDK_BASE ?= $(ATMODULE_PATH)/bouffalo_sdk
else ifneq ($(wildcard $(ATMODULE_PATH)/../../../../project.build),)
BL_SDK_BASE ?= $(abspath $(ATMODULE_PATH)/../../../..)
else
BL_SDK_BASE ?= $(ATMODULE_PATH)/bouffalo_sdk
endif
export BL_SDK_BASE ATMODULE_PATH

SDK_REPOSITORY ?= https://github.com/bouffalolab/bouffalo_sdk.git

ATMODULE_SDK_GOALS := $(if $(MAKECMDGOALS),$(MAKECMDGOALS),all)
ATMODULE_SDK_PREPARE := $(if $(ATMODULE_SDK_READY)$(findstring n,$(firstword -$(MAKEFLAGS))),,$(filter-out clean clean-build pack_clean help config-info config-show,$(ATMODULE_SDK_GOALS)))

ifneq ($(ATMODULE_SDK_PREPARE),)
.DEFAULT_GOAL := all
.PHONY: $(ATMODULE_SDK_GOALS) atmodule-sdk-prepare
$(ATMODULE_SDK_GOALS): atmodule-sdk-prepare ;

atmodule-sdk-prepare:
	@if [ ! -f "$(BL_SDK_BASE)/project.build" ]; then \
		git clone --branch "$(ATMODULE_SDK_VERSION)" --depth 1 "$(SDK_REPOSITORY)" "$(BL_SDK_BASE)" && \
		git -C "$(BL_SDK_BASE)" submodule update --init --recursive; \
	fi
	@if [ "$(abspath $(BL_SDK_BASE))" = "$(abspath $(ATMODULE_PATH)/bouffalo_sdk)" ]; then \
		cd "$(BL_SDK_BASE)" && current=$$(git rev-parse HEAD) && \
		if [ "$$current" != "$$(git rev-parse -q --verify 'refs/tags/$(ATMODULE_SDK_VERSION)^{commit}')" ]; then \
			git fetch --depth 1 "$(SDK_REPOSITORY)" "refs/tags/$(ATMODULE_SDK_VERSION):refs/tags/$(ATMODULE_SDK_VERSION)" && \
			git checkout --detach "refs/tags/$(ATMODULE_SDK_VERSION)" && \
			git submodule update --init --recursive; \
		fi; \
	fi
	$(MAKE) ATMODULE_SDK_READY=1 $(MAKECMDGOALS)
else ifneq ($(wildcard $(BL_SDK_BASE)/project.build),)
include $(BL_SDK_BASE)/project.build
endif
