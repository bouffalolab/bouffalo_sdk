# Reuse bl616cldk/board.c; AVE-specific mux wiring and H1 guidance live in
# board_gpio_overlay.c and README.md.
set(BOARD_BASE_PATH ${BL_SDK_BASE}/bsp/board/bl616cldk)

# The AVE microSD socket connects DAT0 only. Keep the SD host in 1-bit mode.
sdk_add_compile_definitions(-DCONFIG_SDH_BUS_WIDTH_1LINE=1)

sdk_add_include_directories(.)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board_gpio_overlay.c)

add_subdirectory(${BOARD_BASE_PATH} bl616cldk)
