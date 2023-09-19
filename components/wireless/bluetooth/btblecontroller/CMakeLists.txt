sdk_add_include_directories(btble_inc)
sdk_add_include_directories(btblecontroller_port)

# use default port built by lib
#target_sources(app PRIVATE btblecontroller_port/btblecontroller_port_freertos_os.c)
#target_sources(app PRIVATE btblecontroller_port/btblecontroller_port.c)
#target_sources(app PRIVATE btblecontroller_port/btblecontroller_port_uart.c)

sdk_add_static_library(lib/libbtblecontroller_${CHIP}_${PRIV_CONFIG_GEN_BLE}.a)