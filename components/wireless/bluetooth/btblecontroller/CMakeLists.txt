sdk_add_include_directories(btble_inc)
sdk_add_include_directories(btblecontroller_port)
sdk_add_static_library(lib/libbtblecontroller_${CHIP}_${CONFIG_BTBLECONTROLLER_LIB}.a)

