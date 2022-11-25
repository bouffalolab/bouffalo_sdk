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
				rv32i_xtheade_lz4.S
				${CMAKE_SOURCE_DIR}/port/${CHIP}/bflb_port_boot2.c
				)
				

if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/port/${CHIP})
include_directories(${CMAKE_SOURCE_DIR}/port/${CHIP})
else()
message(FATAL_ERROR "${CMAKE_SOURCE_DIR}/port/${CHIP} is not exist")
endif()	
include_directories(${CMAKE_SOURCE_DIR})			
sdk_set_main_file(main.c)
sdk_set_linker_script(port/${CHIP}/blsp_boot2_iap_flash.ld)

if(CONFIG_DEBUG)
sdk_add_compile_definitions(-DCONFIG_DEBUG)
endif()

project(boot2_isp)
