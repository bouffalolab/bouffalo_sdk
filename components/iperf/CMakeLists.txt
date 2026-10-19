sdk_generate_library(iperf)

sdk_add_include_directories(include)
sdk_add_private_include_directories(iperf)

sdk_library_add_sources(
	iperf/bflb_iperf.c
	iperf/iperf_common.c
	iperf/iperf_tcp_socket.c
	iperf/iperf_udp_socket.c
)
if(NOT CONFIG_SOCKET_OFFLOAD_CLIENT)
    sdk_library_add_sources(iperf/iperf_tcp_raw.c iperf/iperf_udp_raw.c)
endif()
sdk_library_add_sources_ifdef(CONFIG_SHELL iperf_cli.c)
