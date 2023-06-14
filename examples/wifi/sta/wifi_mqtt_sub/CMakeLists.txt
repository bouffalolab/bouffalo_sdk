cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# User
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)
# sdk_add_compile_options(-DMQTT_USE_CUSTOM_SOCKET_HANDLE)
# sdk_add_compile_definitions(-DMBEDTLS_PK_PARSE_C)
#sdk_add_compile_definitions(-DBL616_DHCP_DEBUG)

target_sources(app PRIVATE
                   wifi_mqtt_sub.c)

sdk_add_include_directories(.)

sdk_set_main_file(main.c)

project(wifi_mqtt_sub)
