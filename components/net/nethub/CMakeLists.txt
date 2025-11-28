sdk_generate_library()

sdk_library_add_sources(nethub/forwardrules/hostless_rules.c
                        nethub/forwardrules/sdiowifi_rules.c
                        port/port_sdio.c
                        port/port_sta.c
                        port/port_tcpipsta.c
                        port/port_ap.c
                        port/port_tcpipap.c
                        port/port_sys.c
                        port/nhsdiowifi.c
                        port/nhsdiowifi_cli.c
                        port/transportsdio.c
                        nethub/src/nethub.c
                        nethub/src/nh_forward.c
                        nethub/src/nhif_manager.c)

sdk_add_include_directories(nethub/include port)

sdk_add_compile_definitions(-DCONFIG_NETHUB_DEBUG)
sdk_add_compile_definitions(-DCONFIG_SDIO3_SOFT_RST_INT_USER)

add_subdirectory(transportsdio)
