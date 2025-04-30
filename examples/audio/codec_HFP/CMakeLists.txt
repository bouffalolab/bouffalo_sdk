cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

target_sources(app PRIVATE
                    multimedia_port.c
                   )

sdk_add_include_directories(. app)

sdk_set_main_file(main.c)

project(codec_HFP)
