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

sdk_add_compile_definitions(-DCONFIG_USE_LWIP=1)
sdk_add_compile_definitions(-DCONFIG_USE_USRSCTP=0)
if(CONFIG_MBEDTLS_V2)
    sdk_add_compile_definitions(-DCONFIG_MBEDTLS_2_X=1)
else()
    sdk_add_compile_definitions(-DCONFIG_MBEDTLS_2_X=0)
    # mbedTLS 3 loads this standard hook after the SDK/application profile.
    # Applications may set a different hook before find_package(bouffalo_sdk).
    if(NOT DEFINED MBEDTLS_USER_CONFIG_FILE)
        set(MBEDTLS_USER_CONFIG_FILE "libpeer_mbedtls_config.h")
    endif()
    if(MBEDTLS_USER_CONFIG_FILE)
        sdk_add_compile_definitions(-DMBEDTLS_USER_CONFIG_FILE=\"${MBEDTLS_USER_CONFIG_FILE}\")
    endif()
endif()
if(NOT DEFINED CONFIG_AUDIO_BUFFER_SIZE)
    set(CONFIG_AUDIO_BUFFER_SIZE 8096)
endif()
if(NOT DEFINED CONFIG_DATA_BUFFER_SIZE)
    set(CONFIG_DATA_BUFFER_SIZE 80960)
endif()
sdk_add_compile_definitions(-DCONFIG_AUDIO_BUFFER_SIZE=${CONFIG_AUDIO_BUFFER_SIZE})
sdk_add_compile_definitions(-DCONFIG_DATA_BUFFER_SIZE=${CONFIG_DATA_BUFFER_SIZE})
foreach(option SCTP_MESSAGE_SIZE SCTP_TX_WINDOW SCTP_RX_WINDOW
               SCTP_DATA_RETRY_MS SCTP_DATA_RETRY_LIMIT DTLS_HANDSHAKE_TIMEOUT_MS)
    if(DEFINED CONFIG_${option})
        sdk_add_compile_definitions(-DCONFIG_${option}=${CONFIG_${option}})
    endif()
endforeach()
sdk_add_compile_definitions(-DHTTP_DO_NOT_USE_CUSTOM_CONFIG=1)
sdk_add_compile_definitions(-DMQTT_DO_NOT_USE_CUSTOM_CONFIG=1)
sdk_add_compile_definitions(-DDISABLE_PEER_SIGNALING=1)
