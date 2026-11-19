cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

if(NOT CHIP STREQUAL "bl618dg" OR NOT CPU_ID STREQUAL "ap")
    message(FATAL_ERROR "lvgl_v9_with_osd requires BL618DG AP")
endif()
if(NOT CONFIG_BSP_DISPLAY OR NOT CONFIG_DISPLAY_DEFAULT OR NOT CONFIG_DISPLAY_DEFAULT_TYPE_VIDEO
   OR NOT CONFIG_LVGL9_PORT_DISP OR NOT CONFIG_DISPLAY_DEFAULT_VIDEO_FORMAT_ARGB8888)
    message(FATAL_ERROR "lvgl_v9_with_osd requires the default video LVGL port with ARGB8888 OSD")
endif()

# project sources (display bring-up lives in the default video port + app_display)
sdk_add_include_directories(./config_file)
sdk_add_include_directories(.)
target_sources(app PRIVATE app_display.c dpi_manager.c)

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
    # NetHub provides ECM/CMD ACM hooks; DATA callbacks feed usb_reader directly.
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

# DPI panels mux SDH DAT2/DAT3 (IO43/44) onto the panel's R7/R6, so the SD card
# must run 1-line. Force it in the SD core (skip the ACMD6 4-bit negotiation);
# filesystem_reader.c already drops IO43/44 from the SDH GPIO init to match.
if(CONFIG_DISPLAY_DEFAULT_TYPE_VIDEO AND NOT CONFIG_DISPLAY_DEFAULT_DSI)
    if(NOT CONFIG_DPI_DISPLAY_BOARD_MODE)
        message(FATAL_ERROR "RGB video requires CONFIG_DPI_DISPLAY_BOARD_MODE=y (control pins GPIO0..3)")
    endif()
    if(CONFIG_DISPLAY_DEFAULT_RESET_USED AND CONFIG_DISPLAY_DEFAULT_RESET_PIN GREATER_EQUAL 0
       AND CONFIG_DISPLAY_DEFAULT_RESET_PIN LESS_EQUAL 3)
        message(FATAL_ERROR "RGB board mode uses GPIO0..3 for display control; reset GPIO${CONFIG_DISPLAY_DEFAULT_RESET_PIN} conflicts. Set CONFIG_DISPLAY_DEFAULT_RESET_USED=n, or wire reset to a free GPIO and set CONFIG_DISPLAY_DEFAULT_RESET_PIN accordingly.")
    endif()
    if(NOT CONFIG_LVGL_V9_WITH_OSD_USB_VIDEO)
        sdk_add_compile_definitions(-DCONFIG_SDH_BUS_WIDTH_1LINE)
        message(STATUS "SDH bus width: 1-line (DPI panel shares IO43/44)")
    endif()
endif()
target_sources(app PRIVATE ${ui_sources})

sdk_set_main_file(main.c)

project(display_lvgl_v9_with_osd)