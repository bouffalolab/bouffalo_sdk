cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# User

target_sources(app PRIVATE
                   app/app_main.c
                   app/player/app_player.c
                   app/player/cli_player.c
                   ../common/multimedia_port_common.c
                   )

sdk_add_include_directories(. app app/player)

sdk_set_main_file(main.c)

sdk_add_flash_partition_image(
    PARTITION media
    FILE ${CMAKE_BINARY_DIR}/build_out/romfs.bin)

project(player)
