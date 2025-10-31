sdk_generate_library()

sdk_add_include_directories(include)
sdk_add_include_directories(src)
sdk_add_include_directories(src/port)

sdk_library_add_sources(src/port/spisync_port.c)
sdk_library_add_sources(src/port/desc_buf.c)
sdk_library_add_sources(src/ramsync.c)
sdk_library_add_sources(src/spisync.c)
sdk_library_add_sources(src/spisync_lp.c)
sdk_library_add_sources(src/spisync_checksum.c)
sdk_library_add_sources(src/spisync_syscmd.c)
#sdk_library_add_sources(src/spisync_loop_cli.c)

