cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_include_directories(. ../../i2s/i2s_codec)

target_sources(app PRIVATE
    ../../i2s/i2s_codec/recorder.c
)

sdk_set_main_file(main.c)

project(audio_dac_i2s_rec)
