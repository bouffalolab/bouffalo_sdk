cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_include_directories(.)

target_sources(app PRIVATE audio_v1_mic_speaker_multichan_template.c)
sdk_set_main_file(main.c)

project(usbd_audio_v1_mic_speaker_multichan)
