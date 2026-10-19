cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

if(CONFIG_SOCKET_OFFLOAD)
    target_compile_options(librpmsg_service PRIVATE -UVRING_SIZE -DVRING_SIZE=4
                          -DIPM_TASK_STACK_SIZE=1024)
endif()

sdk_add_compile_definitions(-DCONFIG_BOOT_NP_ASYNC)

sdk_add_include_directories(.)

sdk_set_main_file(main.c)
sdk_set_flash_erase(2)

project(wifi)


