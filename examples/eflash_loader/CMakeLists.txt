cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

set(FLASHLOADER_PORT_DIR ${CMAKE_SOURCE_DIR}/port/${CHIP})
if(NOT IS_DIRECTORY ${FLASHLOADER_PORT_DIR})
    message(FATAL_ERROR "eflash_loader port for ${CHIP} does not exist")
endif()

sdk_add_include_directories(include)
sdk_add_include_directories(${FLASHLOADER_PORT_DIR})
sdk_add_include_directories(${BL_SDK_BASE}/drivers/lhal/src/flash)

target_sources(app PRIVATE
    protocol/flashloader_codec.c
    protocol/flashloader_protocol.c
    transport/flashloader_transport_mem.c
    storage/flashloader_flash.c
    storage/flashloader_commands.c
)

if(EXISTS ${FLASHLOADER_PORT_DIR}/CMakeLists.txt)
    add_subdirectory(${FLASHLOADER_PORT_DIR})
else()
    target_sources(app PRIVATE ${FLASHLOADER_PORT_DIR}/flashloader_port.c)
endif()

sdk_set_main_file(main.c)

project(eflash_loader)
