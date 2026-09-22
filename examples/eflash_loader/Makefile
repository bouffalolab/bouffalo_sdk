SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= $(abspath ../..)

export BL_SDK_BASE

CHIP ?= bl618dg

-include $(SDK_DEMO_PATH)/port/$(CHIP)/flashloader.mk

FLASHLOADER_IMAGE = $(BUILD_DIR)/build_out/eflash_loader_$(CHIP)$(if $(strip $(CPU_ID)),_$(CPU_ID)).bin
FLASH_CUBE_IMAGE = $(FLASH_CUBE_ROOT)/chips/$(CHIP)/eflash_loader/$(notdir $(FLASHLOADER_IMAGE))

include $(BL_SDK_BASE)/project.build

# The loader is a RAM-resident image that a debugger downloads over JTAG and
# that is never programmed into flash, so there is no whole-flash image to
# build. Override the SDK's flash-only `whole` rule and publish the linked RAM
# image under the file name the CI artifact collector expects, which lets the
# build matrix run its standard `whole` step on this example too.
whole:
	$(CMAKE) -E make_directory "$(BUILD_DIR)"
	$(CMAKE) -E copy "$(FLASHLOADER_IMAGE)" "$(BUILD_DIR)/whole_flash_data.bin"

test:
	$(MAKE) -C tests test CHIP=$(CHIP) BL_SDK_BASE=$(BL_SDK_BASE) CMAKE=$(CMAKE)

package: $(BUILD_DIR)
	@test -n "$(FLASH_CUBE_ROOT)" || \
		(echo "Set FLASH_CUBE_ROOT to the bouffalo_flash_cube source root"; exit 1)
	@test -d "$(FLASH_CUBE_ROOT)/chips/$(CHIP)/eflash_loader" || \
		(echo "Missing FlashCube loader directory for $(CHIP)"; exit 1)
	$(CMAKE) -E copy_if_different "$(FLASHLOADER_IMAGE)" "$(FLASH_CUBE_IMAGE)"
	@cmp "$(FLASHLOADER_IMAGE)" "$(FLASH_CUBE_IMAGE)"
	@echo "Packaged and verified $(FLASH_CUBE_IMAGE)"

.PHONY: test package whole
