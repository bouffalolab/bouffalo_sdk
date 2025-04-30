sdk_generate_library()

add_subdirectory(openthread_port)
add_subdirectory(openthread_utils)
if (DEFINED OT_NCP)
  add_subdirectory(openthread_ncp)
endif()
sdk_add_subdirectory_ifdef(CONFIG_OTBR openthread_br)
sdk_add_subdirectory_ifdef(CONFIG_OTBR_REST openthread_rest)

get_target_property(SDK_INTF_LIB_LINK_OPTIONS sdk_intf_lib INTERFACE_LINK_OPTIONS)
list(REMOVE_ITEM SDK_INTF_LIB_LINK_OPTIONS -nostartfiles)
if(CONFIG_NEWLIB_STANDARD)
    list(REMOVE_ITEM SDK_INTF_LIB_LINK_OPTIONS --specs=nano.specs)
endif()

set_target_properties(sdk_intf_lib PROPERTIES INTERFACE_LINK_OPTIONS "")
foreach(item ${SDK_INTF_LIB_LINK_OPTIONS})
    sdk_add_link_options(${item})
endforeach()

if (CONFIG_MATTER)
  sdk_add_high_prior_compile_options(-fno-short-enums)
endif()

if (DEFINED OT_NCP AND OT_CLI)
  message(FATAL_ERROR "OT_NCP can't be defined with OT_CLI set true.")
endif()

if (DEFINED OT_NCP AND NOT OT_NCP)
  sdk_add_compile_definitions(-DOPENTHREAD_FTD=0)
  sdk_add_compile_definitions(-DOPENTHREAD_MTD=0)
  sdk_add_compile_definitions(-DOPENTHREAD_RADIO=1)
else()
  sdk_add_compile_definitions(-DOPENTHREAD_RADIO=0)
  if (OT_FTD) 
    sdk_add_compile_definitions(-DOPENTHREAD_FTD=1)
    sdk_add_compile_definitions(-DOPENTHREAD_MTD=0)
  else()
    sdk_add_compile_definitions(-DOPENTHREAD_FTD=0)
    sdk_add_compile_definitions(-DOPENTHREAD_MTD=1)
  endif()
endif()

sdk_add_compile_definitions(-DOPENTHREAD_PROJECT_CORE_CONFIG_FILE=${OPENTHREAD_PROJECT_CORE_CONFIG_FILE})
sdk_add_compile_definitions(-DOPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE=${OPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE})

if (DEFINED OT_SERIAL_BUFFER_SIZE)
  sdk_add_compile_definitions(-DOT_SERIAL_BUFFER_SIZE=${OT_SERIAL_BUFFER_SIZE})
endif()
if (DEFINED OT_SERIAL_LINE_SIZE)
  sdk_add_compile_definitions(-DOT_SERIAL_LINE_SIZE=${OT_SERIAL_LINE_SIZE})
endif()

set(PROJECT_SOURCE_DIR openthread)

set(CLI_SOURCES
    cli.cpp
    cli_bbr.cpp
    cli_br.cpp
    cli_coap.cpp
    cli_coap_secure.cpp
    cli_commissioner.cpp
    cli_dataset.cpp
    cli_dns.cpp
    cli_history.cpp
    cli_joiner.cpp
    cli_mac_filter.cpp
    cli_network_data.cpp
    cli_output.cpp
    cli_srp_client.cpp
    cli_srp_server.cpp
    cli_tcp.cpp
    cli_udp.cpp
)

set(CORE_INCLUDES
    ${PROJECT_SOURCE_DIR}/src/core
)

set(CORE_SOURCES
    api/backbone_router_api.cpp
    api/backbone_router_ftd_api.cpp
    api/border_agent_api.cpp
    api/border_router_api.cpp
    api/border_routing_api.cpp
    api/channel_manager_api.cpp
    api/channel_monitor_api.cpp
    api/child_supervision_api.cpp
    api/coap_api.cpp
    api/coap_secure_api.cpp
    api/commissioner_api.cpp
    api/crypto_api.cpp
    api/dataset_api.cpp
    api/dataset_ftd_api.cpp
    api/dataset_updater_api.cpp
    api/diags_api.cpp
    api/dns_api.cpp
    api/dns_server_api.cpp
    api/error_api.cpp
    api/heap_api.cpp
    api/history_tracker_api.cpp
    api/icmp6_api.cpp
    api/instance_api.cpp
    api/ip6_api.cpp
    api/jam_detection_api.cpp
    api/joiner_api.cpp
    api/link_api.cpp
    api/link_metrics_api.cpp
    api/link_raw_api.cpp
    api/logging_api.cpp
    api/mesh_diag_api.cpp
    api/message_api.cpp
    api/multi_radio_api.cpp
    api/nat64_api.cpp
    api/netdata_api.cpp
    api/netdata_publisher_api.cpp
    api/netdiag_api.cpp
    api/network_time_api.cpp
    api/ping_sender_api.cpp
    api/radio_stats_api.cpp
    api/random_crypto_api.cpp
    api/random_noncrypto_api.cpp
    api/server_api.cpp
    api/sntp_api.cpp
    api/srp_client_api.cpp
    api/srp_client_buffers_api.cpp
    api/srp_server_api.cpp
    api/tasklet_api.cpp
    api/tcp_api.cpp
    api/tcp_ext_api.cpp
    api/thread_api.cpp
    api/thread_ftd_api.cpp
    api/trel_api.cpp
    api/udp_api.cpp
    backbone_router/backbone_tmf.cpp
    backbone_router/bbr_leader.cpp
    backbone_router/bbr_local.cpp
    backbone_router/bbr_manager.cpp
    backbone_router/multicast_listeners_table.cpp
    backbone_router/ndproxy_table.cpp
    border_router/infra_if.cpp
    border_router/routing_manager.cpp
    coap/coap.cpp
    coap/coap_message.cpp
    coap/coap_secure.cpp
    common/appender.cpp
    common/binary_search.cpp
    common/crc16.cpp
    common/data.cpp
    common/error.cpp
    common/frame_builder.cpp
    common/frame_data.cpp
    common/heap.cpp
    common/heap_data.cpp
    common/heap_string.cpp
    common/instance.cpp
    common/log.cpp
    common/message.cpp
    common/notifier.cpp
    common/preference.cpp
    common/random.cpp
    common/settings.cpp
    common/string.cpp
    common/tasklet.cpp
    common/time_ticker.cpp
    common/timer.cpp
    common/tlvs.cpp
    common/trickle_timer.cpp
    common/uptime.cpp
    crypto/aes_ccm.cpp
    crypto/aes_ecb.cpp
    crypto/crypto_platform.cpp
    crypto/hkdf_sha256.cpp
    crypto/hmac_sha256.cpp
    crypto/mbedtls.cpp
    crypto/sha256.cpp
    crypto/storage.cpp
    diags/factory_diags.cpp
    mac/channel_mask.cpp
    mac/data_poll_handler.cpp
    mac/data_poll_sender.cpp
    mac/link_raw.cpp
    mac/mac.cpp
    mac/mac_filter.cpp
    mac/mac_frame.cpp
    mac/mac_links.cpp
    mac/mac_types.cpp
    mac/sub_mac.cpp
    mac/sub_mac_callbacks.cpp
    meshcop/announce_begin_client.cpp
    meshcop/border_agent.cpp
    meshcop/commissioner.cpp
    meshcop/dataset.cpp
    meshcop/dataset_local.cpp
    meshcop/dataset_manager.cpp
    meshcop/dataset_manager_ftd.cpp
    meshcop/dataset_updater.cpp
    meshcop/dtls.cpp
    meshcop/energy_scan_client.cpp
    meshcop/extended_panid.cpp
    meshcop/joiner.cpp
    meshcop/joiner_router.cpp
    meshcop/meshcop.cpp
    meshcop/meshcop_leader.cpp
    meshcop/meshcop_tlvs.cpp
    meshcop/network_name.cpp
    meshcop/panid_query_client.cpp
    meshcop/timestamp.cpp
    net/checksum.cpp
    net/dhcp6_client.cpp
    net/dhcp6_server.cpp
    net/dns_client.cpp
    net/dns_dso.cpp
    net/dns_platform.cpp
    net/dns_types.cpp
    net/dnssd_server.cpp
    net/icmp6.cpp
    net/ip4_types.cpp
    net/ip6.cpp
    net/ip6_address.cpp
    net/ip6_filter.cpp
    net/ip6_headers.cpp
    net/ip6_mpl.cpp
    net/nat64_translator.cpp
    net/nd6.cpp
    net/nd_agent.cpp
    net/netif.cpp
    net/sntp_client.cpp
    net/socket.cpp
    net/srp_client.cpp
    net/srp_server.cpp
    net/tcp6.cpp
    net/tcp6_ext.cpp
    net/udp6.cpp
    radio/radio.cpp
    radio/radio_callbacks.cpp
    radio/radio_platform.cpp
    radio/trel_interface.cpp
    radio/trel_link.cpp
    radio/trel_packet.cpp
    thread/address_resolver.cpp
    thread/announce_begin_server.cpp
    thread/announce_sender.cpp
    thread/anycast_locator.cpp
    thread/child_supervision.cpp
    thread/child_table.cpp
    thread/csl_tx_scheduler.cpp
    thread/discover_scanner.cpp
    thread/dua_manager.cpp
    thread/energy_scan_server.cpp
    thread/indirect_sender.cpp
    thread/key_manager.cpp
    thread/link_metrics.cpp
    thread/link_metrics_types.cpp
    thread/link_quality.cpp
    thread/lowpan.cpp
    thread/mesh_forwarder.cpp
    thread/mesh_forwarder_ftd.cpp
    thread/mesh_forwarder_mtd.cpp
    thread/mle.cpp
    thread/mle_router.cpp
    thread/mle_tlvs.cpp
    thread/mle_types.cpp
    thread/mlr_manager.cpp
    thread/neighbor_table.cpp
    thread/network_data.cpp
    thread/network_data_leader.cpp
    thread/network_data_leader_ftd.cpp
    thread/network_data_local.cpp
    thread/network_data_notifier.cpp
    thread/network_data_publisher.cpp
    thread/network_data_service.cpp
    thread/network_data_tlvs.cpp
    thread/network_data_types.cpp
    thread/network_diagnostic.cpp
    thread/network_diagnostic_tlvs.cpp
    thread/panid_query_server.cpp
    thread/radio_selector.cpp
    thread/router_table.cpp
    thread/src_match_controller.cpp
    thread/thread_netif.cpp
    thread/time_sync_service.cpp
    thread/tmf.cpp
    thread/topology.cpp
    thread/uri_paths.cpp
    utils/channel_manager.cpp
    utils/channel_monitor.cpp
    utils/flash.cpp
    utils/heap.cpp
    utils/history_tracker.cpp
    utils/jam_detector.cpp
    utils/mesh_diag.cpp
    utils/otns.cpp
    utils/parse_cmdline.cpp
    utils/ping_sender.cpp
    utils/power_calibration.cpp
    utils/slaac_address.cpp
    utils/srp_client_buffers.cpp
)

set(RADIO_CORE_SOURCES
    api/diags_api.cpp
    api/error_api.cpp
    api/instance_api.cpp
    api/link_raw_api.cpp
    api/logging_api.cpp
    api/random_noncrypto_api.cpp
    api/tasklet_api.cpp
    common/binary_search.cpp
    common/error.cpp
    common/frame_builder.cpp
    common/instance.cpp
    common/log.cpp
    common/random.cpp
    common/string.cpp
    common/tasklet.cpp
    common/timer.cpp
    common/uptime.cpp
    crypto/aes_ccm.cpp
    crypto/aes_ecb.cpp
    crypto/crypto_platform.cpp
    crypto/storage.cpp
    diags/factory_diags.cpp
    mac/link_raw.cpp
    mac/mac_frame.cpp
    mac/mac_types.cpp
    mac/sub_mac.cpp
    mac/sub_mac_callbacks.cpp
    radio/radio.cpp
    radio/radio_callbacks.cpp
    radio/radio_platform.cpp
    thread/link_quality.cpp
    utils/parse_cmdline.cpp
    utils/power_calibration.cpp
)

set(PLATFORM_SOURCES
  exit_code.c
)

set(TCPLP_SOURCES
    bsdtcp/cc/cc_newreno.c
    bsdtcp/tcp_input.c
    bsdtcp/tcp_output.c
    bsdtcp/tcp_reass.c
    bsdtcp/tcp_sack.c
    bsdtcp/tcp_subr.c
    bsdtcp/tcp_timer.c
    bsdtcp/tcp_timewait.c
    bsdtcp/tcp_usrreq.c
    lib/bitmap.c
    lib/cbuf.c
    lib/lbuf.c
)

set(EXAMPLE_CLI_SOURCES
    cli_uart.cpp
)

sdk_add_include_directories(${PROJECT_SOURCE_DIR}/include)
sdk_add_include_directories(${PROJECT_SOURCE_DIR}/src)
sdk_add_include_directories(${PROJECT_SOURCE_DIR}/src/core)

if (OT_CLI)
  foreach(source_file ${CLI_SOURCES})
    sdk_library_add_sources(${PROJECT_SOURCE_DIR}/src/cli/${source_file})
  endforeach()
endif()

if (DEFINED OT_NCP AND NOT OT_NCP)
  foreach(source_file ${RADIO_CORE_SOURCES})
    sdk_library_add_sources(${PROJECT_SOURCE_DIR}/src/core/${source_file})
  endforeach()
else()
  foreach(source_file ${CORE_SOURCES})
    sdk_library_add_sources(${PROJECT_SOURCE_DIR}/src/core/${source_file})
  endforeach()

  sdk_add_private_compile_definitions(-Dtcp_input=tcp_input_ot)
  sdk_add_private_compile_definitions(-Dtcp_output=tcp_output_ot)
  sdk_add_private_compile_definitions(-Dtcp_close=tcp_close_ot)

  sdk_add_include_directories(${PROJECT_SOURCE_DIR}/third_party/tcplp)
  foreach(source_file ${TCPLP_SOURCES})
    sdk_library_add_sources(${PROJECT_SOURCE_DIR}/third_party/tcplp/${source_file})
  endforeach()
endif()

sdk_add_include_directories(${PROJECT_SOURCE_DIR}/src/lib/platform)
foreach(source_file ${PLATFORM_SOURCES})
  sdk_library_add_sources(${PROJECT_SOURCE_DIR}/src/lib/platform/${source_file})
endforeach()

if (OT_CLI)
  sdk_add_include_directories(${PROJECT_SOURCE_DIR}/examples/platforms)
  foreach(source_file ${EXAMPLE_CLI_SOURCES})
    sdk_library_add_sources(${PROJECT_SOURCE_DIR}/examples/apps/cli/${source_file})
  endforeach()
endif()

