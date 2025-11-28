cmake_minimum_required(VERSION 3.15)

#include("$ENV{BL_SDK_BASE}/cmake/auto_config.cmake")

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# User
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)

if(CONFIG_WIFI_GATEWAY)
sdk_add_compile_definitions(-DFHOST_BUFFER_COPY=1)
sdk_add_compile_definitions(-DFHOST_RX_BUF_CNT=48)
sdk_add_compile_definitions(-DFHOST_RX_BUF_SECTION=\".bss\")
sdk_add_compile_definitions(-DCONFIG_WIFI_GATEWAY)
sdk_add_compile_definitions(-DCFG_ENABLE_AP_PS)
#sdk_add_compile_definitions(-DLWIP_HEAP_SIZE=\(32*1024\))
sdk_add_compile_definitions(-DCFG_IP_NAPT_TIMEOUT_MS_TCP=\(30*60*1000\))
sdk_add_compile_definitions(-DCFG_IP_NAPT_TIMEOUT_MS_TCP_DISCON=\(2*20*1000\))
sdk_add_compile_definitions(-DCFG_IP_NAPT_TIMEOUT_MS_UDP=\(620*1000\))
sdk_add_compile_definitions(-DCFG_IP_NAPT_TIMEOUT_MS_ICMP=\(5*2*1000\))
endif()

sdk_add_compile_definitions(-DCONFIG_ATMODULE=1)
sdk_add_compile_definitions(-DCONFIG_ATMODULE_TTY=1)
sdk_add_compile_definitions(-DCONFIG_ATMODULE_WIFI=1)

sdk_add_compile_definitions(-DCONFIG_NETHUB=1)

target_sources(app PRIVATE
                   app/src/atmodule/app_atmodule.c
                   app/src/app_user.c
                   )

sdk_add_include_directories(.)
sdk_add_include_directories(./app/include)

sdk_set_main_file(main.c)

project(nethub)
