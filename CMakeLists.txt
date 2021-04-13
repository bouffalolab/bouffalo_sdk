cmake_minimum_required(VERSION 3.10)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/tools/cmake/riscv64-unknown-elf-gcc.cmake)
include(${CMAKE_SOURCE_DIR}/tools/cmake/compiler_flags.cmake)
include(${CMAKE_SOURCE_DIR}/tools/cmake/tools.cmake)

PROJECT(${BOARD} C CXX ASM)

include_directories(${CMAKE_SOURCE_DIR}/common/misc)
include_directories(${CMAKE_SOURCE_DIR}/bsp/bsp_common/platform)
include_directories(${CMAKE_SOURCE_DIR}/bsp/board/${BOARD})

add_subdirectory(common)
add_subdirectory(components/fatfs)
add_subdirectory(components/usb_stack)
add_subdirectory(drivers/${mcu}_driver)

if(${SUPPORT_SHELL} STREQUAL "ENABLE")
add_subdirectory(components/shell)
else()
endif()

search_application(${CMAKE_SOURCE_DIR}/examples)

