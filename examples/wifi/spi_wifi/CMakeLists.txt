cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# User
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)
#sdk_add_compile_definitions(-DCONFIG_MEMSET_OPTSPEED)


if (CONFIG_LPAPP)
    target_sources(app PRIVATE
                       app/src/app_spiwifi.c
                       app/src/atmodule/app_atmodule.c
                       app/src/pm/app_pm.c
                       )
else ()

sdk_add_compile_definitions(-DHIGH_PERFORMANCE_PBUF=1)

target_sources(app PRIVATE
                   app/src/app_spiwifi.c
                   app/src/atmodule/app_atmodule.c
                   # app/src/easyflash4/app_easyflash4.c
                   # app/src/atcmd/app_atcmd.c
                   # app/src/sample/app_sample.c
                   # app/src/spisync/app_spisync.c
                   # app/src/system/app_msgbuffer.c
                   # app/src/useruart/app_useruart.c
                   )
endif()

sdk_add_compile_definitions(-DAT_NET_TX_MAX_LEN=NXBD_MTU)
sdk_add_compile_definitions(-DAT_TRANS_ZEROCOPY=1)

sdk_add_include_directories(.)
sdk_add_include_directories(./app/include)

sdk_set_main_file(main.c)

if (CONFIG_RCP_ENABLE)
sdk_add_compile_definitions(-DLWIP_HEAP_SIZE=\(1*1024\))
sdk_add_compile_definitions(-DCONFIG_RCP_ENABLE=$(CONFIG_RCP_ENABLE))
endif()

project(spi_wifi)
