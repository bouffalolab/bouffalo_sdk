cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

string(TOUPPER "${CHIP}" upper_case_chip)
string(TOLOWER "${CHIP}" lower_case_chip)
sdk_add_compile_definitions(-DOPENTHREAD_CONFIG_PLATFORM_INFO=\"${upper_case_chip}\")

sdk_add_include_directories(.)

target_sources(app PRIVATE board_${lower_case_chip}.c)

sdk_set_main_file(main.c)

project(openthread_ncp)
