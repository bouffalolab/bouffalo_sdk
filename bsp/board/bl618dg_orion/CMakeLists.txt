if(NOT CHIP STREQUAL "bl618dg" OR NOT CPU_ID STREQUAL "ap")
    message(FATAL_ERROR "bl618dg_orion requires CHIP=bl618dg CPU_ID=ap")
endif()

# Reuse DK startup, memory and RF setup with Orion hardware wiring.
set(BOARD_BASE_PATH ${BL_SDK_BASE}/bsp/board/bl618dgdk)
# Board hardware configuration must precede component default headers.
target_include_directories(sdk_intf_lib BEFORE INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board_gpio.c)
if(CONFIG_XCODEC_IO AND CONFIG_BSP_AUDIO_CODEC)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board_audio.c)
endif()
if(CONFIG_BSP_AUDIO_CODEC)
    sdk_add_compile_definitions(-DCONFIG_AUDIO_PA_PIN=BOARD_AUDIO_PA_PIN)
    sdk_add_compile_options("SHELL:-include ${CMAKE_CURRENT_SOURCE_DIR}/board_config.h")
endif()
add_subdirectory(${BOARD_BASE_PATH} bl618dgdk)
# GPIO belongs to Orion; only reuse the DK startup, storage and RF sources.
get_target_property(board_sources app SOURCES)
list(REMOVE_ITEM board_sources "${BOARD_BASE_PATH}/board_gpio.c")
set_property(TARGET app PROPERTY SOURCES "${board_sources}")
