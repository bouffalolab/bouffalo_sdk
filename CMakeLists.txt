cmake_minimum_required(VERSION 3.15)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/tools/cmake/riscv64-unknown-elf-gcc.cmake)
include(${CMAKE_SOURCE_DIR}/tools/cmake/compiler_flags.cmake)
include(${CMAKE_SOURCE_DIR}/tools/cmake/tools.cmake)

PROJECT(${BOARD} C CXX ASM)

# set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories(${CMAKE_SOURCE_DIR}/common/misc)
include_directories(${CMAKE_SOURCE_DIR}/bsp/bsp_common/platform)

if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/bsp/board/${BOARD})
include_directories(${CMAKE_SOURCE_DIR}/bsp/board/${BOARD})
else()  
message(FATAL_ERROR "${CMAKE_SOURCE_DIR}/bsp/board/${BOARD} is not exist")
endif()

if(${SUPPORT_FLOAT} STREQUAL "y")
add_definitions(-DBFLB_PRINT_FLOAT_SUPPORT)
else()
endif()

if(${SUPPORT_SHELL} STREQUAL "y")
add_definitions(-DSHELL_SUPPORT)
else()
endif()

add_subdirectory(common)
add_subdirectory(components/fatfs)
add_subdirectory(components/usb_stack)

if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/drivers/${CHIP}_driver)
add_subdirectory(drivers/${CHIP}_driver)
else()  
message(FATAL_ERROR "${CMAKE_SOURCE_DIR}/drivers/${CHIP}_driver is not exist")
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

if(${SUPPORT_XZ} STREQUAL "y")
add_subdirectory(components/xz)
endif()

if(${SUPPORT_BLE} STREQUAL "y")
if(${SUPPORT_FREERTOS} STREQUAL "n")
message(FATAL_ERROR "ble need freertos,so you should set SUPPORT_FREERTOS=y")
endif()
add_subdirectory(components/ble)
endif()

search_application(${CMAKE_SOURCE_DIR}/${APP_DIR})

