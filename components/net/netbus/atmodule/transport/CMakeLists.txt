# sdk_generate_library()

sdk_add_subdirectory_ifdef(CONFIG_ATMODULE_UART  uart)
sdk_add_subdirectory_ifdef(CONFIG_ATMODULE_NXSPI nxspi)
sdk_add_subdirectory_ifdef(CONFIG_ATMODULE_USB   usb)
sdk_add_subdirectory_ifdef(CONFIG_ATMODULE_TTY   tty)
