cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# User
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)

target_sources(app PRIVATE
                    ../common/multimedia_port_common.c
                    app/app_main.c
                    app/player/app_minialsa.c
                    app/player/audio_file.c
                    # app/player/codec_eq.c
                    app/player/codec_speaker
                   )

sdk_add_include_directories(. app app/player)

sdk_set_main_file(main.c)

project(minialsa)
