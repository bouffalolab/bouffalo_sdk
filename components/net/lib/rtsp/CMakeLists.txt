sdk_generate_library()

sdk_library_add_sources(src/delay_task.c)
sdk_library_add_sources(src/jpeg_parser.c)

sdk_library_add_sources(src/librtspsrv.c)
sdk_library_add_sources(src/log.c)
sdk_library_add_sources(src/rtcp.c)
sdk_library_add_sources(src/rtp.c)
sdk_library_add_sources(src/rtsp_method.c)
sdk_library_add_sources(src/rtsp_parser.c)
sdk_library_add_sources(src/rtsp_sess.c)
sdk_library_add_sources(src/rtsp_srv.c)

sdk_library_add_sources(src/sd_handler.c)
sdk_library_add_sources(src/util.c)

sdk_add_include_directories(inc)
