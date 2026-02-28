sdk_generate_library()

# msg_ctrl
sdk_add_include_directories(sdio_msg_ctrl)
sdk_library_add_sources(sdio_msg_ctrl/msg_ctrl_sdio.c)

# netdev
sdk_add_include_directories(sdio_netdev)
sdk_library_add_sources(sdio_netdev/netdev_wifi.c)

# virtualchan
sdk_add_include_directories(sdio_virtualchan)
sdk_library_add_sources(sdio_virtualchan/virtualchan.c)

# tty
sdk_add_include_directories(sdio_tty_atcmd)
sdk_library_add_sources(sdio_tty_atcmd/tty_atcmd.c)


# transport_sdio
sdk_add_include_directories(./)
sdk_library_add_sources(./transport_sdio.c)
