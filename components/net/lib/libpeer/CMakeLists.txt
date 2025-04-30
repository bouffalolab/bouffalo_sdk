sdk_generate_library()

sdk_library_add_sources(
    src/address.c
    src/agent.c
    src/base64.c
    src/buffer.c
    src/dtls_srtp.c
    src/ice.c
    src/mdns.c
    src/peer.c
    src/peer_connection.c
    src/peer_signaling.c
    src/ports.c
    src/rtcp.c
    src/rtp.c
    src/sctp.c
    src/sdp.c
    src/socket.c
    src/ssl_transport.c
    src/stun.c
    src/utils.c
    #third_party/coreHTTP/source/core_http_client.c
    #third_party/coreHTTP/source/dependency/3rdparty/llhttp/src/api.c
    #third_party/coreHTTP/source/dependency/3rdparty/llhttp/src/llhttp.c
    #third_party/coreHTTP/source/dependency/3rdparty/llhttp/src/http.c
    #third_party/coreMQTT/source/core_mqtt.c
    #third_party/coreMQTT/source/core_mqtt_state.c
    #third_party/coreMQTT/source/core_mqtt_serializer.c
)

sdk_add_include_directories(src)
#sdk_add_include_directories(third_party/coreHTTP/source/include)
#sdk_add_include_directories(third_party/coreHTTP/source/dependency/3rdparty/llhttp/include)
#sdk_add_include_directories(third_party/coreMQTT/source/include)
#sdk_add_include_directories(third_party/coreMQTT/source/interface)

sdk_add_compile_definitions(-D__BYTE_ORDER=__LITTLE_ENDIAN)
sdk_add_compile_definitions(-DCONFIG_USE_LWIP=1)
sdk_add_compile_definitions(-DCONFIG_USE_USRSCTP=0)
sdk_add_compile_definitions(-DCONFIG_AUDIO_BUFFER_SIZE=8096)
sdk_add_compile_definitions(-DCONFIG_DATA_BUFFER_SIZE=80960)
sdk_add_compile_definitions(-DHTTP_DO_NOT_USE_CUSTOM_CONFIG=1)
sdk_add_compile_definitions(-DMQTT_DO_NOT_USE_CUSTOM_CONFIG=1)
sdk_add_compile_definitions(-DDISABLE_PEER_SIGNALING=0)


