cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_compile_definitions(-DCONFIG_BOOT_NP_ASYNC)

sdk_add_include_directories(.)

if (CONFIG_IPC)
    #sdk_add_compile_definitions(-DCONFIG_IPC_ENABLE)
    if (CONFIG_RPMSG_SERVICE)
        sdk_add_compile_definitions(-DCONFIG_RPMSG_SERVICE)
        sdk_add_compile_definitions(-DCONFIG_RPMSG_SERVICE_MODE_MASTER)
        target_sources(app PRIVATE rpmsg_service_master.c)
    else()
        target_sources(app PRIVATE demo_openamp_master.c)
    endif()
endif()

sdk_set_main_file(main.c)

project(openamp)
