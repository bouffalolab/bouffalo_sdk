cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_include_directories(.)
sdk_set_main_file(main.c)

if(CONFIG_WAMR_INTERP OR CONFIG_WAMR_AOT)
    target_sources(app PRIVATE app/wasm_cmd.c)
    if(CONFIG_WAMR_INTERP)
        target_compile_definitions(app PRIVATE CONFIG_WAMR_INTERP=1)
    endif()
    if(CONFIG_WAMR_AOT)
        target_compile_definitions(app PRIVATE CONFIG_WAMR_AOT=1)
    endif()
endif()

project(wasm_cmd)
