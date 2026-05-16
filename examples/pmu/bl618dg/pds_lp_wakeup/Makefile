SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= $(abspath  ./../../../..)

CHIP ?= bl618dg
BUILD_DIR ?= build
APP_NAME = pds_lp_wakeup
PROJ_NAME = pds_lp_wakeup

all:
	make -C lp SDK_DEMO_PATH=$(abspath lp) BUILD_DIR=../$(BUILD_DIR)/lp CPU_ID=lp
	make -C ap SDK_DEMO_PATH=$(abspath ap) BUILD_DIR=../$(BUILD_DIR)/ap CPU_ID=ap CONFIG_THIRDCORE_LP_IMAGE=../lp/build_out/$(PROJ_NAME)_$(CHIP)_lp.bin
	cp $(BUILD_DIR)/ap/build_out/$(PROJ_NAME)_$(CHIP)_ap.bin $(BUILD_DIR)/$(APP_NAME)_$(CHIP).bin

clean:
	make -C lp SDK_DEMO_PATH=$(abspath lp) BUILD_DIR=../$(BUILD_DIR)/lp clean
	make -C ap SDK_DEMO_PATH=$(abspath ap) BUILD_DIR=../$(BUILD_DIR)/ap clean
	cmake -E remove_directory $(BUILD_DIR)

cmake_cache:
	make -C lp SDK_DEMO_PATH=$(abspath lp) BUILD_DIR=../$(BUILD_DIR)/lp CPU_ID=lp $@
	make -C ap SDK_DEMO_PATH=$(abspath ap) BUILD_DIR=../$(BUILD_DIR)/ap CPU_ID=ap $@

IS_WSL2 = $(findstring WSL2,$(shell uname -r))
ifneq ($(IS_WSL2),)
	FLASH_CMD = $(BL_SDK_BASE)/tools/bflb_tools/bouffalo_flash_cube/BLFlashCommand.exe
else
	UNAME_S = $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		FLASH_CMD = $(BL_SDK_BASE)/tools/bflb_tools/bouffalo_flash_cube/BLFlashCommand-ubuntu
	else
		FLASH_CMD = $(BL_SDK_BASE)/tools/bflb_tools/bouffalo_flash_cube/BLFlashCommand.exe
	endif
endif

whole:
	cmake -E copy flash_prog_cfg.ini $(BUILD_DIR)/flash_prog_cfg.ini
	sed -i 's/\.\/build/\./g' $(BUILD_DIR)/flash_prog_cfg.ini
	sed -i 's/$$(CHIPNAME)/$(CHIP)/g' $(BUILD_DIR)/flash_prog_cfg.ini
	cmake -E chdir $(BUILD_DIR) $(FLASH_CMD) --chipname=$(CHIP) --config=flash_prog_cfg.ini --build
	cmake -E copy $(BL_SDK_BASE)/tools/bflb_tools/bouffalo_flash_cube/chips/$(CHIP)/img_create/whole_flash_data.bin $(BUILD_DIR)/whole_flash_data.bin

flash: whole
	$(FLASH_CMD) --chip=$(CHIP) --port $(COMX) --whole_chip --firmware $(BUILD_DIR)/whole_flash_data.bin

ota: whole
	$(BL_SDK_BASE)/tools/bflb_tools/bflb_fw_post_proc/bflb_fw_post_proc-ubuntu --chipname=$(CHIP) --imgfile=$(BUILD_DIR)/$(APP_NAME)_$(CHIP).bin --appkeys=shared --brdcfgdir=$(BL_SDK_BASE)/bsp/board/$(BOARD)/config

.PHONY: all clean cmake_cache whole flash ota
