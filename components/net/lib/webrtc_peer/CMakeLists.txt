sdk_generate_library()

sdk_library_add_sources(
    src/webrtc_peer.c
    src/webrtc_peer_signaling.c
)

if(CONFIG_WEBRTC_PEER_MJPEG)
    sdk_library_add_sources(src/webrtc_peer_mjpeg.c)
endif()

if(CONFIG_WEBRTC_PEER_SIGNALING_APPRTC)
    sdk_library_add_sources(src/apprtc_signaling.c)
endif()

sdk_add_include_directories(include)
sdk_add_private_include_directories(src ../libpeer/src)
