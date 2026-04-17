sdk_generate_library()

sdk_library_add_sources(ota/ota.c)
sdk_add_include_directories(.)

sdk_library_add_sources(compat/bflb_ota.c)
sdk_add_include_directories(compat)

if(CONFIG_HTTP)
    sdk_library_add_sources(https/https_fota.c https/https_fota_shell.c)
    sdk_add_include_directories(https)
endif()

if(CONFIG_LWIP)
    sdk_library_add_sources(tcp/tcp_fota.c tcp/tcp_fota_shell.c)
    sdk_add_include_directories(tcp)
endif()
