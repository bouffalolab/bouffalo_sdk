cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# project sources (no dsi_common dependency; DSI bring-up lives in the LCD framework)
sdk_add_include_directories(.)
target_sources(app PRIVATE dpi_manager.c filesystem_reader.c)

# LVGL demos (use the copy kept inside this project's demos/ directory)
set(LVGL_DEMO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/demos)
file(GLOB_RECURSE lvgl_demo_sources "${LVGL_DEMO_DIR}/*.c")
sdk_add_include_directories(${LVGL_DEMO_DIR})

# SquareLine UI: one folder per panel resolution (same symbol names in each, so
# only ONE may be compiled or the linker sees duplicate ui_init/ui_Screen1/...).
# Pick the folder that matches the panel selected in lcd_conf_user.h, drop both
# from the GLOB above, and add only the chosen one. main.c includes "ui.h"
# (no folder prefix), resolved via the include dir added below, so it never
# needs to know which resolution is active.
#
# DPI standard (800x480) has no matching SquareLine UI, so it builds NO UICODE
# folder and main.c runs a built-in LVGL demo instead (guarded by LCD_INTERFACE_TYPE).
set(UI_DIR_480 ${LVGL_DEMO_DIR}/UICODE480)
set(UI_DIR_720 ${LVGL_DEMO_DIR}/UICODE720)
list(FILTER lvgl_demo_sources EXCLUDE REGEX "/UICODE480/|/UICODE720/")

file(READ ${CMAKE_CURRENT_SOURCE_DIR}/lcd_conf_user.h _lcd_conf)
# DPI panels mux SDH DAT2/DAT3 (IO43/44) onto the panel's R7/R6, so the SD card
# must run 1-line. Force it in the SD core (skip the ACMD6 4-bit negotiation);
# filesystem_reader.c already drops IO43/44 from the SDH GPIO init to match.
if(_lcd_conf MATCHES "\n[ \t]*#define[ \t]+LCD_DPI_")
    sdk_add_compile_definitions(-DCONFIG_SDH_BUS_WIDTH_1LINE)
    message(STATUS "SDH bus width: 1-line (DPI panel shares IO43/44)")
endif()
# match an ACTIVE (non-commented) panel define
if(_lcd_conf MATCHES "\n[ \t]*#define[ \t]+LCD_DPI_STANDARD")
    set(UI_DIR "") # DPI: built-in demo (lv_demo_benchmark), no SquareLine UI
elseif(_lcd_conf MATCHES "\n[ \t]*#define[ \t]+LCD_DPI_JD9165BA")
    set(UI_DIR "") # 1024x600 DPI: no matching SquareLine UI, run built-in benchmark
elseif(_lcd_conf MATCHES "\n[ \t]*#define[ \t]+LCD_DSI_AXS15231B_HS035")
    set(UI_DIR "") # 172x640 DSI: no matching SquareLine UI, run built-in benchmark
elseif(_lcd_conf MATCHES "\n[ \t]*#define[ \t]+LCD_DSI_ILI9881C_")
    set(UI_DIR ${UI_DIR_720}) # KD050HDFIA020 (4-lane) / KD050HDFIA023_W4 (2-lane), both 720x1280
elseif(_lcd_conf MATCHES "\n[ \t]*#define[ \t]+LCD_DSI_ST7102_YH494")
    set(UI_DIR ${UI_DIR_480}) # YH-494BSAC002N1 480x960
else()
    message(FATAL_ERROR "lcd_conf_user.h: no UI mapping for the selected panel; add a mapping in CMakeLists.txt")
endif()

if(UI_DIR)
    message(STATUS "SquareLine UI folder: ${UI_DIR}")
    file(GLOB_RECURSE ui_sources "${UI_DIR}/*.c")
    sdk_add_include_directories(${UI_DIR})
else()
    message(STATUS "No SquareLine UI (DPI built-in demo)")
    set(ui_sources "")
endif()

target_sources(app PRIVATE ${lvgl_demo_sources} ${ui_sources})

sdk_set_main_file(main.c)

project(lvgl_v9_with_osd)
