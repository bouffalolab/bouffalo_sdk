cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# User
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)
sdk_add_compile_definitions(-DAT_NET_TX_MAX_LEN=6144)

if (CONFIG_LPAPP)
    target_sources(app PRIVATE
                       app/src/app_uartwifi.c
                       app/src/atmodule/app_atmodule.c
                       app/src/pm/app_pm.c
                       )
else ()
    if(NOT CONFIG_PSRAM_COPY_CODE)
        sdk_add_compile_definitions(-DCFG_LINK_ALL_IN_FLASH=1)
        sdk_add_compile_definitions(-DCONFIG_HIGH_PERFORMANCE=1)
    endif()


target_sources(app PRIVATE
                   app/src/app_uartwifi.c
                   app/src/atmodule/app_atmodule.c
                   )
endif()

sdk_add_include_directories(.)
sdk_add_include_directories(./app/include)

sdk_set_main_file(main.c)

project(uart_wifi)
