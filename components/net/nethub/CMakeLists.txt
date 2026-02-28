sdk_generate_library()

sdk_library_add_sources(nethub/forwardrules/hostless_rules.c)
sdk_library_add_sources(nethub/forwardrules/sdiowifi_rules.c)
# sdk_library_add_sources(channel/sdio.c)
sdk_library_add_sources_ifdef(CONFIG_FHOST channel/wifi.c)
sdk_library_add_sources_ifdef(CONFIG_WL80211 channel/wifi_wl80211.c)
sdk_library_add_sources(channel/tcpip.c)
sdk_library_add_sources(port/port_sys.c)
sdk_library_add_sources(port/nhsdiowifi.c)
sdk_library_add_sources(port/nhsdiowifi_cli.c)
sdk_library_add_sources(nethub/src/nethub.c)
sdk_library_add_sources(nethub/src/nh_forward.c)
sdk_library_add_sources(nethub/src/nhif_manager.c)

sdk_add_include_directories(nethub/include port)

sdk_add_compile_definitions(-DCONFIG_NETHUB_DEBUG)

# add_subdirectory(transportsdio_new)
add_subdirectory(transport)

