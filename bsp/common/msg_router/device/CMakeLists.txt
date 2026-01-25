sdk_generate_library()

sdk_add_include_directories(frame_buff_ctrl)
sdk_library_add_sources(frame_buff_ctrl/mr_frame_buff_ctrl.c)

sdk_add_include_directories(msg_ctrl)
sdk_library_add_sources(msg_ctrl/mr_msg_ctrl.c)

if(CONFIG_MR_SDIO_DRIVER)
    set(SDIO2_LIMITED_CHIPS "bl616" "bl616l")
    set(SDIO3_LIMITED_CHIPS "bl616d")

    sdk_add_include_directories(sdio_driver)
    if("${CHIP}" IN_LIST SDIO2_LIMITED_CHIPS)
        sdk_library_add_sources(sdio_driver/mr_sdio2_drv.c)
    elseif("${CHIP}" IN_LIST SDIO3_LIMITED_CHIPS)
        sdk_library_add_sources(sdio_driver/mr_sdio3_drv.c)
    endif()

    if(CONFIG_MR_SDIO_QUEUE_DEPTH)
        sdk_add_compile_definitions(-D SDIO2_MAX_PORT_NUM=${CONFIG_MR_SDIO_QUEUE_DEPTH})
        sdk_add_compile_definitions(-D SDIO3_FUNC_QUEUE_NUM_MAX=${CONFIG_MR_SDIO_QUEUE_DEPTH})
    else()
        sdk_add_compile_definitions(-D SDIO2_MAX_PORT_NUM=4)
        sdk_add_compile_definitions(-D SDIO3_FUNC_QUEUE_NUM_MAX=4)
    endif()

    if(CONFIG_MR_MSG_CTRL_SDIO)
        sdk_library_add_sources(msg_ctrl/mr_msg_ctrl_sdio.c)
    endif()

endif()

if(CONFIG_MR_NETDEV)
    sdk_add_include_directories(netdev)
    sdk_library_add_sources(netdev/mr_netdev.c)

    if(CONFIG_MR_NETDEV_EMAC)
        sdk_library_add_sources(netdev/mr_netdev_emac.c)
    endif()
endif()

if(CONFIG_MR_NETLINK)
    sdk_add_include_directories(netlink)
    sdk_library_add_sources(netlink/mr_netlink.c)

    if(CONFIG_MR_NETLINK_CLI)
        sdk_library_add_sources(netlink/mr_netlink_cli.c)
    endif()
endif()

if(CONFIG_MR_SPEED_TEST)
    sdk_add_include_directories(speed_test)
    sdk_library_add_sources(speed_test/mr_speed_test.c)

    if(CONFIG_MR_SPEED_TEST_START)
        sdk_library_add_sources(speed_test/mr_speed_test_start.c)
    endif()
endif()

if(CONFIG_MR_TTY)
    sdk_add_include_directories(tty)
    sdk_library_add_sources(tty/mr_tty.c)

    if(CONFIG_MR_TTY_CLI)
        sdk_library_add_sources(tty/mr_tty_cli.c)
    endif()
endif()
