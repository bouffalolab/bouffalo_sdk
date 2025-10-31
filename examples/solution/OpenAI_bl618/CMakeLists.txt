cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# User
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)
#sdk_add_compile_definitions(-DBL_VERIFY)
sdk_add_compile_definitions(-DCONFIG_USING_TLS)
sdk_add_compile_definitions(-DHAVE_CONFIG_H)
sdk_add_compile_definitions(-DCONFIG_XCODEC_OUTPUT_CACHE)
sdk_add_compile_definitions(-DSHELL_CMD_SIZE=256)

target_sources(app PRIVATE
                   app/src/oai_http.c
                   app/src/oai_media.c
                   app/src/oai_webrtc.c
                   https/src/https.c
                   https/src/bl_https.c
                   multimedia_port.c
                   )

sdk_add_include_directories(.)
sdk_add_include_directories(./app/include)
sdk_add_include_directories(./https/include)

sdk_set_main_file(main.c)

project(OpenAI_bl618)
