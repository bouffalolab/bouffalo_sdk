cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# User
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)

target_sources(app PRIVATE
                   app/app_main.c
                   app/player/app_player.c
                   app/player/cli_player.c
                   multimedia_port.c
                   )

sdk_add_include_directories(. app app/player)

sdk_set_main_file(main.c)

project(player)
