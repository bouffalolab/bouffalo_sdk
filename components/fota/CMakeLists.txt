sdk_generate_library()

sdk_library_add_sources(ota/ota.c)
sdk_add_include_directories(.)

sdk_library_add_sources(compat/bflb_ota.c)
sdk_add_include_directories(compat)

if(CONFIG_HTTP)
    sdk_add_include_directories(https)
    sdk_library_add_sources(https/https_fota.c)

    if(CONFIG_SHELL AND (NOT CONFIG_FOTA_NO_SHELL))
        sdk_library_add_sources(https/https_fota_shell.c)
    endif()
endif()

if(CONFIG_LWIP)
    sdk_add_include_directories(tcp)
    sdk_library_add_sources(tcp/tcp_fota.c)

    if(CONFIG_SHELL AND (NOT CONFIG_FOTA_NO_SHELL))
        sdk_library_add_sources(tcp/tcp_fota_shell.c)
    endif()
endif()
