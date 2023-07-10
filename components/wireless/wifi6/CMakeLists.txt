sdk_add_include_directories(inc)

sdk_add_static_library(lib/libwifi6.a)

add_subdirectory(bl6_os_adapter)

# Enable common section for wifi6
sdk_add_compile_options(-fcommon)

# disable undef warning
sdk_add_compile_options(-Wno-undef)

sdk_ifndef(CONFIG_VIF_MAX 2)
sdk_ifndef(CONFIG_STA_MAX 4)
sdk_ifndef(CONFIG_MAC_TXQ_DEPTH 32)
sdk_ifndef(CONFIG_MAC_RXQ_DEPTH 12)

sdk_add_compile_definitions(-DCONFIG_VIF_MAX=${CONFIG_VIF_MAX})
sdk_add_compile_definitions(-DCONFIG_STA_MAX=${CONFIG_STA_MAX})
sdk_add_compile_definitions(-DCONFIG_MAC_TXQ_DEPTH=${CONFIG_MAC_TXQ_DEPTH})
sdk_add_compile_definitions(-DCONFIG_MAC_RXQ_DEPTH=${CONFIG_MAC_RXQ_DEPTH})