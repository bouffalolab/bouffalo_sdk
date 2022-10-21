if(CONFIG_BSP_COMMON)
sdk_generate_library()

if(CONFIG_ETHERNET)
sdk_library_add_sources(ethernet/ethernet_phy.c)
sdk_library_add_sources(ethernet/phy_8720.c)
if(CONFIG_LWIP)
sdk_library_add_sources(ethernet/ethernetif.c)
endif()
sdk_add_include_directories(ethernet/)
endif()

if(CONFIG_LUA)
sdk_library_add_sources(lua/lua_start.c)
endif()

# fatfs
if(CONFIG_FATFS)

# fatfs use sdcard over sdio
if(CONFIG_BSP_FATFS_SDH_SDCARD)
sdk_add_compile_definitions(-DCONFIG_BSP_FATFS_SDH_SDCARD)
sdk_library_add_sources(fatfs/fatfs_sdh_sdcard.c)
endif()

# fatfs use sdcard over spi
if(CONFIG_BSP_FATFS_SPI_SDCARD)

endif()

# fatfs use psram
if(CONFIG_BSP_FATFS_PSRAM)

endif()

# fatfs use flash
if(CONFIG_BSP_FATFS_FLASH)

endif()

# fatfs use usb host msc
if(CONFIG_BSP_FATFS_USBH)

endif()

endif()

# sdh sdcard driver
if(CONFIG_BSP_SDH_SDCARD)
sdk_add_compile_definitions(-DCONFIG_BSP_SDH_SDCARD)
sdk_library_add_sources(sdcard/sdh_sdcard.c)
sdk_add_include_directories(sdcard/)
endif()

# spi sdcard driver
if(CONFIG_BSP_SPI_SDCARD)

endif()

# lcd driver
if(CONFIG_BSP_LCD)
sdk_library_add_sources(lcd/lcd.c)
sdk_library_add_sources(lcd/font.c)
sdk_library_add_sources(lcd/spi/ili9341_spi.c)
sdk_library_add_sources(lcd/spi/ili9488_spi.c)
sdk_library_add_sources(lcd/spi/st7796_spi.c)
sdk_add_include_directories(lcd)
endif()

# lvgl port
if(CONFIG_LVGL)
sdk_library_add_sources(lvgl/lv_port_disp.c)
# sdk_library_add_sources(lvgl/lv_port_fs.c)
# sdk_library_add_sources(lvgl/lv_port_indev.c)
sdk_add_include_directories(lvgl)
endif()

endif()