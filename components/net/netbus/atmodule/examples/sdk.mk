ifneq ($(wildcard $(ATMODULE_PATH)/bouffalo_sdk/project.build),)
BL_SDK_BASE ?= $(ATMODULE_PATH)/bouffalo_sdk
else ifneq ($(wildcard $(ATMODULE_PATH)/../../../../project.build),)
BL_SDK_BASE ?= $(abspath $(ATMODULE_PATH)/../../../..)
else
BL_SDK_BASE ?= $(ATMODULE_PATH)/bouffalo_sdk
endif
export BL_SDK_BASE ATMODULE_PATH

SDK_REPOSITORY ?= https://github.com/bouffalolab/bouffalo_sdk.git

$(BL_SDK_BASE)/project.build:
	git clone --branch "$$(cat "$(ATMODULE_PATH)/BOUFFALO_SDK_VERSION")" --depth 1 "$(SDK_REPOSITORY)" "$(BL_SDK_BASE)"
	git -C "$(BL_SDK_BASE)" submodule update --init --recursive

ifeq ($(shell grep -c ATMODULE_PATH "$(BL_SDK_BASE)/components/net/netbus/CMakeLists.txt" 2>/dev/null),0)
SDK_COMPAT_ERROR := $(shell git -C "$(BL_SDK_BASE)" apply "$(ATMODULE_PATH)/examples/sdk.patch" 2>&1)
ifneq ($(.SHELLSTATUS),0)
$(error Cannot enable external ATMODULE_PATH in $(BL_SDK_BASE): $(SDK_COMPAT_ERROR))
endif
endif
include $(BL_SDK_BASE)/project.build
