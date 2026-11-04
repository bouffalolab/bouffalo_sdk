cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# project sources (no dsi_common dependency; DSI bring-up lives in the LCD framework)
sdk_add_include_directories(.)
target_sources(app PRIVATE dpi_manager.c)

if(CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO OR CONFIG_LVGL_V9_WITH_OSD_OTA)
    target_sources(app PRIVATE app_wifi.c)
endif()

if(CONFIG_LVGL_V9_WITH_OSD_OTA)
    target_sources(app PRIVATE app_ota.c)
    sdk_add_compile_definitions(-DCONFIG_LVGL_V9_WITH_OSD_OTA=1)
    # This example accepts HTTP only. Keep unused TLS client code out of the
    # image to reduce OTA package size and RAM use.
    target_compile_definitions(libhttp PRIVATE CONFIG_HTTP_CLIENT_NO_HTTPS=1)
endif()

if(CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO)
    # USB mode: the example owns the composite USB descriptor/enumeration, while
    # NetHub provides ECM/CMD ACM service hooks and a generic stream for video.
    target_sources(app PRIVATE usb_reader.c app_usb_composite.c)
    sdk_add_compile_definitions(-DCONFIG_LVGL_V9_WITH_OSD_USB_VIDEO=1)
else()
    # Default mode: keep the original SD-card JPEG producer.
    target_sources(app PRIVATE filesystem_reader.c)
endif()

# One OSD UI and one set of assets for every display resolution.
set(UI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/demos/ui)
file(GLOB_RECURSE ui_sources "${UI_DIR}/*.c")
sdk_add_include_directories(${UI_DIR})

file(READ ${CMAKE_CURRENT_SOURCE_DIR}/lcd_conf_user.h _lcd_conf)
# DPI panels mux SDH DAT2/DAT3 (IO43/44) onto the panel's R7/R6, so the SD card
# must run 1-line. Force it in the SD core (skip the ACMD6 4-bit negotiation);
# filesystem_reader.c already drops IO43/44 from the SDH GPIO init to match.
if(NOT CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO AND _lcd_conf MATCHES "\n[ \t]*#define[ \t]+LCD_DPI_(GC9503V|ST7701P|ST7701S|JD9165BA|ILI9488|STANDARD)([ \t\r\n]|$)")
    sdk_add_compile_definitions(-DCONFIG_SDH_BUS_WIDTH_1LINE)
    message(STATUS "SDH bus width: 1-line (DPI panel shares IO43/44)")
endif()
target_sources(app PRIVATE ${ui_sources})

sdk_set_main_file(main.c)

project(lvgl_v9_with_osd)
