sdk_generate_library()

sdk_library_add_sources(transport_sdio.c)
sdk_library_add_sources(msg_ctrl_sdio.c)
sdk_library_add_sources(netdev_wifi.c)
sdk_library_add_sources(tty_atcmd.c)
sdk_library_add_sources(virtualchan.c)

sdk_add_private_include_directories(${CMAKE_CURRENT_LIST_DIR}/../../../include)
sdk_add_private_include_directories(${CMAKE_CURRENT_LIST_DIR}/../../../core)
sdk_add_private_include_directories(${CMAKE_CURRENT_LIST_DIR}/../../../profile)
sdk_add_private_include_directories(${CMAKE_CURRENT_LIST_DIR})
