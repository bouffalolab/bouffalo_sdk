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

file(GLOB_RECURSE lv_sources "${CMAKE_CURRENT_SOURCE_DIR}/app/src/lv_demos/*.c")
file(GLOB_RECURSE coremark_sources "${CMAKE_CURRENT_SOURCE_DIR}/app/src/coremark/*.c")

set(BOARD_BASE_PATH  ${BL_SDK_BASE}/bsp/board/bl616ddk)

target_sources(app PRIVATE
                app/src/sample/app_sample.c
                app/src/app_init.c
                app/src/wifi/app_wifi.c
                ${CMAKE_CURRENT_SOURCE_DIR}/board_gpio_overlay.c
                ${lv_sources}
                ${coremark_sources})

add_subdirectory(${BOARD_BASE_PATH} bl616ddk)

#sdk_add_compile_options(
#    -O2
#    -falign-functions=2
#    -fno-code-hoisting
#    -finline-limit=500
#    -funroll-all-loops
#)

sdk_add_compile_definitions(
    -DITERATIONS=10000
    -DCORE_DEBUG=0
    -DCOMPILER_REQUIRES_SORT_RETURN=0
    -DVALIDATION_RUN=0
    -DPROFILE_RUN=0
    -DPERFORMANCE_RUN=1
)

sdk_add_include_directories(.)
sdk_add_include_directories(app/include)

sdk_set_main_file(main.c)

project(wifi)
