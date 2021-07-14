cmake_minimum_required(VERSION 3.15)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/tools/cmake/riscv64-unknown-elf-gcc.cmake)
include(${CMAKE_SOURCE_DIR}/tools/cmake/compiler_flags.cmake)
include(${CMAKE_SOURCE_DIR}/tools/cmake/tools.cmake)

PROJECT(${BOARD} C CXX ASM)

# set(CMAKE_VERBOSE_MAKEFILE ON)
ENABLE_LANGUAGE(ASM)

include_directories(${CMAKE_SOURCE_DIR}/common/misc)
include_directories(${CMAKE_SOURCE_DIR}/bsp/bsp_common/platform)

if(${CHIP} STREQUAL "bl702")
    if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/bsp/board/bl70x)
    include_directories(${CMAKE_SOURCE_DIR}/bsp/board/bl70x)
    else()
    message(FATAL_ERROR "${CMAKE_SOURCE_DIR}/bsp/board/bl70x is not exist")
    endif()
else()
    if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/bsp/board/${CHIP})
    include_directories(${CMAKE_SOURCE_DIR}/bsp/board/${CHIP})
    else()
    message(FATAL_ERROR "${CMAKE_SOURCE_DIR}/bsp/board/${CHIP} is not exist")
    endif()
endif()


search_application(${CMAKE_SOURCE_DIR}/${APP_DIR})

