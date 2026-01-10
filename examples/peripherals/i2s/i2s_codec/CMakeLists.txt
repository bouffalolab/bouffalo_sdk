cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

target_sources(app PRIVATE
				   mp3_player.c
				   wave_player.c
				   recorder.c
				   )

sdk_set_main_file(main.c)

project(i2s_codec)
