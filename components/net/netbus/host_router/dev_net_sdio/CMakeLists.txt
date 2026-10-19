sdk_generate_library()

sdk_library_add_sources(hostrouter_init.c
                        hostrouter_log.c
                        net_wifi_transceiver.c
						sdio_msg_ext_frame.c
						sdiowifi_mgmr.c
						sdiowifi_tty.c
						sdiowifi_tx_buf.c
						trcver_sdio.c 
						sdio_port.c
						sdio_pkt_hooks.c
						adapter/sdiowifi_freertos.c)

sdk_add_include_directories(.)
sdk_add_include_directories(adapter)

if(CONFIG_DHCP_IN_EMB)
sdk_add_compile_definitions(-DDHCP_IN_EMB)
endif()
