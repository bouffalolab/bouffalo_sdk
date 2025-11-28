cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

if(CONFIG_AUDIO_MIC_PDM_MODE)
    sdk_add_compile_definitions(-D CONFIG_AUDIO_MIC_PDM_MODE)
endif()

sdk_add_include_directories(.)

sdk_set_main_file(main.c)

project(auadc_record_to_mem)
