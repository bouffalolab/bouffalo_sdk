cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

target_sources(app PRIVATE
                    app/audio_file.c
                    app/codec_mic.c
                    app/codec_speaker.c
                    app/codec_speaker_sin.c
                    app/codec_template.c
                    app/codec_loop.c
                    app/codec_eq.c
                    multimedia_port.c
                   )

sdk_add_include_directories(. app)

sdk_set_main_file(main.c)

project(codec)
