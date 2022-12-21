cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

target_sources(app PRIVATE 
				bflb_eflash_loader_interface.c 
				bflb_eflash_loader_cmds.c 
				bflb_eflash_loader_uart.c 
				blsp_boot_decompress.c 
				blsp_boot_parser.c 
				blsp_common.c 
				blsp_media_boot.c 
				blsp_port.c 
				blsp_ram_image_boot.c 
				partition.c
				softcrc.c
				${CMAKE_SOURCE_DIR}/port/${CHIP}/bflb_port_boot2.c
				)
				

if("${CHIP}" STREQUAL "bl616")
target_sources(app PRIVATE rv32i_xtheade_lz4.S)
endif()

if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/port/${CHIP})
include_directories(${CMAKE_SOURCE_DIR}/port/${CHIP})
else()
message(FATAL_ERROR "${CMAKE_SOURCE_DIR}/port/${CHIP} is not exist")
endif()	
include_directories(${CMAKE_SOURCE_DIR})			
sdk_set_main_file(main.c)
sdk_set_linker_script(port/${CHIP}/blsp_boot2_iap_flash.ld)

if("${CONFIG_DEBUG}" STREQUAL "y")
sdk_add_compile_definitions(-DCONFIG_DEBUG)
endif()
sdk_add_compile_definitions(-DBFLB_BOOT2_VER="${CONFIG_BOOT2_VER}")
message(${CONFIG_BOOT2_VER})

if((${CHIP} STREQUAL "bl602")) 
    sdk_add_compile_definitions(-DCHIP_BL602)
endif()

if((${CHIP} STREQUAL "bl702")) 
    sdk_add_compile_definitions(-DCHIP_BL702)
endif()

if((${CHIP} STREQUAL "bl808")) 
    sdk_add_compile_definitions(-DCHIP_BL808)
endif()

if((${CHIP} STREQUAL "bl606p")) 
    sdk_add_compile_definitions(-DCHIP_BL606P)
endif()

if((${CHIP} STREQUAL "bl616")) 
    sdk_add_compile_definitions(-DCHIP_BL616)
endif()

    
project(boot2_isp)
