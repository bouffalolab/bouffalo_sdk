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
sdk_library_add_sources(lcd/spi/st7789v_spi.c)
sdk_add_include_directories(lcd/)
endif()

# image sensor driver
if(CONFIG_BSP_IMAGE_SENSOR)
sdk_library_add_sources(image_sensor/image_sensor.c)
sdk_add_include_directories(image_sensor/)
endif()

endif()