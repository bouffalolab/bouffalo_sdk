# Bus core, endpoints, and the bus backends that are switched on in bus/Kconfig
target_sources(app PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/display_bus.c
    ${CMAKE_CURRENT_LIST_DIR}/display_io.c)

if(CONFIG_DISPLAY_BUS_SOFT_DBI_C)
    target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/soft_dbi_c/display_soft_dbi_c.c)
endif()

if(CONFIG_DISPLAY_BUS_SPI)
    target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/spi/display_spi.c)
endif()

if(CONFIG_DISPLAY_BUS_DBI)
    target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/dbi/display_dbi.c)
endif()

if(CONFIG_DISPLAY_BUS_DSI)
    if(NOT CHIP STREQUAL "bl618dg")
        message(FATAL_ERROR "The MIPI DSI host requires BL618DG")
    endif()
    target_sources(app PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/dsi/display_dsi.c
        ${CMAKE_CURRENT_LIST_DIR}/dsi/display_dsi_bl618dg.c
        ${CMAKE_CURRENT_LIST_DIR}/../scanout/bl618dg_dpu/display_bl618dg_dpu_clock.c)
endif()
