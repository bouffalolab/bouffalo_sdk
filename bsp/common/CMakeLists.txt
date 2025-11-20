if(CONFIG_BSP_ETH_PHY)
    target_sources(app PRIVATE eth_phy/eth_phy.c)
    target_sources(app PRIVATE eth_phy/eth_phy_port.c)
    target_sources(app PRIVATE eth_phy/ephy_general.c)
    target_sources(app PRIVATE eth_phy/ephy_lan8720.c)
    sdk_add_include_directories(eth_phy)

    if (CONFIG_BSP_LWIP_NETIF_EMAC)
        sdk_add_include_directories(eth_phy/lwip_netif_emac)  
        target_sources(app PRIVATE eth_phy/lwip_netif_emac/lwip_netif_emac.c)
    endif()
endif()

# sdh sdcard driver
if(CONFIG_BSP_SDH_SD)
    sdk_add_compile_definitions(-D CONFIG_BSP_SDH_SD)
    if(CONFIG_BSP_SDH_OSAL_POLLING_MODE)
        sdk_add_compile_definitions(-D CONFIG_BSP_SDH_OSAL_POLLING_MODE=${CONFIG_BSP_SDH_OSAL_POLLING_MODE})
    endif()
    if(CONFIG_BSP_SDH_LOG_LEVEL)
        sdk_add_compile_definitions(-D CONFIG_BSP_SDH_LOG_LEVEL=${CONFIG_BSP_SDH_LOG_LEVEL})
    endif()
    target_sources(app PRIVATE sdh/osal/sdh_osal.c)
    target_sources(app PRIVATE sdh/host/sdh_host.c)
    target_sources(app PRIVATE sdh/sd/sdh_sd.c)
    sdk_add_include_directories(sdh/osal)
    sdk_add_include_directories(sdh/host)
    sdk_add_include_directories(sdh/sd)

    string(TOLOWER ${CHIP} CHIP_NAME_STR)
    sdk_add_compile_definitions(-D CHIP_GLB_HEADER="${CHIP_NAME_STR}_glb.h")
endif()

# spi sdcard driver
if(CONFIG_BSP_SPI_SDCARD)

endif()

# lcd driver
if(CONFIG_BSP_LCD)
    target_sources(app PRIVATE lcd/lcd.c)
    target_sources(app PRIVATE lcd/font.c)

    target_sources(app PRIVATE lcd/mipi_dbi/bl_mipi_dbi.c)
    target_sources(app PRIVATE lcd/mipi_dbi/gc9307_dbi.c)
    target_sources(app PRIVATE lcd/mipi_dbi/ili9488_dbi.c)
    target_sources(app PRIVATE lcd/mipi_dbi/ili9486_dbi.c)
    target_sources(app PRIVATE lcd/mipi_dbi/ili9341_dbi.c)
    target_sources(app PRIVATE lcd/mipi_dbi/jd9853_dbi.c)
    target_sources(app PRIVATE lcd/mipi_dbi/nt35510_dbi.c)
    target_sources(app PRIVATE lcd/mipi_dbi/st7796_dbi.c)
    target_sources(app PRIVATE lcd/mipi_dbi/st77926_dbi_qspi.c)
    target_sources(app PRIVATE lcd/mipi_dbi/spd2010_dbi_qspi.c)

    target_sources(app PRIVATE lcd/spi/bl_spi_hard_4.c)
    target_sources(app PRIVATE lcd/spi/gc9307_spi.c)
    target_sources(app PRIVATE lcd/spi/ili9341_spi.c)
    target_sources(app PRIVATE lcd/spi/ili9488_spi.c)
    target_sources(app PRIVATE lcd/spi/st7796_spi.c)
    target_sources(app PRIVATE lcd/spi/st7789v_spi.c)
    target_sources(app PRIVATE lcd/spi/st7735_spi.c)

    target_sources(app PRIVATE lcd/mipi_dpi/bl_dpi_init_spi_soft_3.c)
    target_sources(app PRIVATE lcd/mipi_dpi/bl_dpi_init_spi_soft_4.c)
    target_sources(app PRIVATE lcd/mipi_dpi/bl_mipi_dpi_sim.c)
    target_sources(app PRIVATE lcd/mipi_dpi/bl_mipi_dpi_v2.c)
    target_sources(app PRIVATE lcd/mipi_dpi/standard_dpi.c)
    target_sources(app PRIVATE lcd/mipi_dpi/ili9488_dpi.c)
    target_sources(app PRIVATE lcd/mipi_dpi/gc9503v_dpi.c)
    target_sources(app PRIVATE lcd/mipi_dpi/st7701s_dpi.c)

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

