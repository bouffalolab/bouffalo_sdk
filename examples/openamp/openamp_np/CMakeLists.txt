cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# User
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)


if (CONFIG_IPC)
    #sdk_add_compile_definitions(-DCONFIG_IPC_ENABLE)
    if (CONFIG_RPMSG_SERVICE)
        sdk_add_compile_definitions(-DCONFIG_RPMSG_SERVICE)
        target_sources(app PRIVATE rpmsg_service_remote.c)
    else()
        target_sources(app PRIVATE demo_openamp_remote.c)
    endif()
endif()

target_sources(app PRIVATE
                app/src/sample/app_sample.c
                app/src/app_init.c)

sdk_add_include_directories(.)
sdk_add_include_directories(app/include)

sdk_set_main_file(main.c)

project(openamp)
