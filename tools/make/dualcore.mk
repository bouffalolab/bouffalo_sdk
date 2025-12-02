all:
	make -C $(APP_NAME)_np BUILD_DIR=../$(BUILD_DIR)/np;				      \
	np_size=$$(wc -c <$(BUILD_DIR)/np/build_out/$(APP_NAME)_$(CHIP)_np.bin);	      \
	make -C $(APP_NAME)_ap BUILD_DIR=../$(BUILD_DIR)/ap CONFIG_NP_IMAGE_SIZE=$$np_size;   \
	cat $(BUILD_DIR)/ap/build_out/$(APP_NAME)_$(CHIP)_ap.bin $(BUILD_DIR)/np/build_out/$(APP_NAME)_$(CHIP)_np.bin >$(BUILD_DIR)/$(APP_NAME)_$(CHIP).bin; \

clean:
	make -C $(APP_NAME)_np clean
	make -C $(APP_NAME)_ap clean

cmake_cache:
	make -C $(APP_NAME)_np $@
	make -C $(APP_NAME)_ap $@

whole:
	cmake -E copy flash_prog_cfg.ini $(BUILD_DIR)/flash_prog_cfg.ini
	sed -i 's/\.\/build/\./g' $(BUILD_DIR)/flash_prog_cfg.ini
	cmake -E chdir $(BUILD_DIR) $(BL_SDK_BASE)/tools/bflb_tools/bouffalo_flash_cube/BLFlashCommand-ubuntu --chipname=$(CHIP) --config=flash_prog_cfg.ini --build
	cmake -E copy $(BL_SDK_BASE)/tools/bflb_tools/bouffalo_flash_cube/chips/$(CHIP)/img_create/whole_flash_data.bin $(BUILD_DIR)/whole_flash_data.bin

flash: whole
	$(BL_SDK_BASE)/tools/bflb_tools/bouffalo_flash_cube/BLFlashCommand-ubuntu --chip=$(CHIP) --port $(COMX) --whole_chip --firmware $(BUILD_DIR)/whole_flash_data.bin

.PHONY: clean
