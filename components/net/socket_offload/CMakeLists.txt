sdk_generate_library()
sdk_add_include_directories(include)

if(CONFIG_SOCKET_OFFLOAD_CLIENT)
    sdk_library_add_sources(src/socket_offload.c src/ipc_socket.c)
elseif(CONFIG_SOCKET_OFFLOAD_SERVICE)
    sdk_library_add_sources(src/socket_rpc_service.c)
endif()
