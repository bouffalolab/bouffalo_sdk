cmake_minimum_required(VERSION 3.15)


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

target_sources(app PRIVATE
                app/src/sample/app_sample.c
                app/src/app_init.c)

sdk_add_include_directories(.)
sdk_add_include_directories(app/include)

sdk_add_include_directories(${CMAKE_SOURCE_DIR}/../../../../../components/utils/wifi_ota)

# Add flash_rpc_remote.c for dual-core OTA (NP core - Remote)
if(CONFIG_WIFI_OTA_DUAL_CORE)
    sdk_add_include_directories(${CMAKE_SOURCE_DIR}/../../../../../components/utils/wifi_ota/rpc)
    sdk_add_include_directories(${CMAKE_SOURCE_DIR}/../../../../../drivers/soc/bl616d/std/include)
    target_sources(app PRIVATE
        ${CMAKE_SOURCE_DIR}/../../../../../components/utils/wifi_ota/rpc/flash_rpc_remote.c
    )
endif()

sdk_set_main_file(main.c)

project(wifi)

