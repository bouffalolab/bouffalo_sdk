cmake_minimum_required(VERSION 3.10)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/tools/cmake/riscv64-unknown-elf-gcc.cmake)
include(${CMAKE_SOURCE_DIR}/tools/cmake/compiler_flags.cmake)
include(${CMAKE_SOURCE_DIR}/tools/cmake/tools.cmake)

PROJECT(${BOARD} C CXX ASM)

include_directories(${CMAKE_SOURCE_DIR}/common/misc)
include_directories(${CMAKE_SOURCE_DIR}/bsp/bsp_common/platform)

if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/bsp/board/${BOARD})
include_directories(${CMAKE_SOURCE_DIR}/bsp/board/${BOARD})
else()  
message(FATAL_ERROR "${CMAKE_SOURCE_DIR}/bsp/board/${BOARD} is not exist")
endif()

add_subdirectory(common)
add_subdirectory(components/fatfs)
add_subdirectory(components/usb_stack)

if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/drivers/${mcu}_driver)
add_subdirectory(drivers/${mcu}_driver)
else()  
message(FATAL_ERROR "${CMAKE_SOURCE_DIR}/drivers/${mcu}_driver is not exist")
endif()

if(${SUPPORT_SHELL} STREQUAL "y")
add_subdirectory(components/shell)
endif()

if(${SUPPORT_FREERTOS} STREQUAL "y")
add_subdirectory(components/freertos)
endif()

if(${SUPPORT_LVGL} STREQUAL "y")
add_subdirectory(components/lvgl)
endif()

search_application(${CMAKE_SOURCE_DIR}/examples)

