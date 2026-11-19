sdk_generate_library()

sdk_add_compile_definitions(-D LV_KCONFIG_IGNORE)
sdk_add_compile_definitions(-D LV_CONF_INCLUDE_SIMPLE)

file(GLOB_RECURSE SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/core/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/draw/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/font/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/*.c
)
sdk_library_add_sources(${SOURCES})

if(CONFIG_LVGL8_PORT_DISP AND CONFIG_BSP_LCD)
    sdk_library_add_sources(port/lcd_old/lv_port_disp_mcu.c)
    sdk_library_add_sources(port/lcd_old/lv_port_disp_rgb.c)
    sdk_add_include_directories(port/lcd_old)
elseif(CONFIG_LVGL8_PORT_DISP AND CONFIG_DISPLAY_DEFAULT)
    # The default display of the display framework (bsp/common/display)
    if(CONFIG_DISPLAY_DEFAULT_TYPE_VIDEO)
        sdk_library_add_sources(port/lv_port_disp_default_video.c)
    elseif(CONFIG_DISPLAY_DEFAULT_TYPE_GRAM)
        sdk_library_add_sources(port/lv_port_disp_default_gram.c)
    endif()
endif()

if(CONFIG_LVGL8_PORT_FS)
    sdk_library_add_sources(port/lv_port_fs.c)
endif()

if(CONFIG_LVGL8_PORT_INDEV)
    sdk_library_add_sources(port/lv_port_indev.c)
endif()

sdk_add_include_directories(.)
sdk_add_include_directories(port/)
