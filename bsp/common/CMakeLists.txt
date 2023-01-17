if(CONFIG_ETHERNET)
target_sources(app PRIVATE ethernet/ethernet_phy.c)
target_sources(app PRIVATE ethernet/phy_8720.c)
if(CONFIG_LWIP)
target_sources(app PRIVATE ethernet/ethernetif.c)
endif()
sdk_add_include_directories(ethernet)
endif()

# sdh sdcard driver
if(CONFIG_BSP_SDH_SDCARD)
sdk_add_compile_definitions(-DCONFIG_BSP_SDH_SDCARD)
target_sources(app PRIVATE sdcard/sdh_sdcard.c)
sdk_add_include_directories(sdcard)
endif()

# spi sdcard driver
if(CONFIG_BSP_SPI_SDCARD)

endif()

# lcd driver
if(CONFIG_BSP_LCD)
target_sources(app PRIVATE lcd/lcd.c)
target_sources(app PRIVATE lcd/font.c)

target_sources(app PRIVATE lcd/spi/ili9341_spi.c)
target_sources(app PRIVATE lcd/spi/ili9488_spi.c)
target_sources(app PRIVATE lcd/spi/st7796_spi.c)
target_sources(app PRIVATE lcd/spi/st7789v_spi.c)
sdk_add_include_directories(lcd)
endif()

# touch sensor driver
if(CONFIG_BSP_TOUCH)
target_sources(app PRIVATE touch/touch.c)
target_sources(app PRIVATE touch/ft6x36_i2c.c)
target_sources(app PRIVATE touch/gt911_i2c.c)
sdk_add_include_directories(touch)
endif()

# image sensor driver
if(CONFIG_BSP_IMAGE_SENSOR)
target_sources(app PRIVATE image_sensor/image_sensor.c)
sdk_add_include_directories(image_sensor)
if(CONFIG_BSP_CSI)
sdk_add_compile_definitions(-DCONFIG_BSP_CSI)
endif()
endif()