if(NOT CHIP STREQUAL "bl618dg")
    message(FATAL_ERROR "bl618dg_mstar requires CHIP=bl618dg")
endif()

# Reuse the DK startup, memory and RF setup with MStar-specific GPIO wiring.
set(BOARD_BASE_PATH ${BL_SDK_BASE}/bsp/board/bl618dgdk)

sdk_add_include_directories(.)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board_gpio_overlay.c)
add_subdirectory(${BOARD_BASE_PATH} bl618dgdk)
