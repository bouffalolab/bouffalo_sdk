sdk_generate_library()

sdk_library_add_sources(src/rnm_utils.c)
sdk_library_add_sources(src/rnm_common.c)

if(CONFIG_CPU_E907)
set(RNM_SERVER 1)
endif()

if(CONFIG_SDIOWIFI)
set(RNM_SERVER 1)
endif()

if((${CHIP} STREQUAL "bl616"))
sdk_add_compile_definitions(-DCFG_BL616)
set(RNM_SERVER 1)
endif()

if(CONFIG_CPU_C906)
set(RNM_SERVER 1)
endif()

if(RNM_SERVER)
sdk_library_add_sources(src/rnm_server.c)
sdk_library_add_sources(src/rnm_server_cmd_handler.c)
sdk_library_add_sources(src/rnm_ota.c)
sdk_library_add_sources(src/rnm_user_ext.c)
endif()

if(RNM_CLIENT)
sdk_library_add_sources(src/rnm_client.c)
sdk_library_add_sources(src/rnm_client_cmds.c)
endif()

if(CONFIG_DHCP_IN_EMB)
sdk_add_compile_definitions(-DDHCP_IN_EMB)
endif()

sdk_add_include_directories(inc)
