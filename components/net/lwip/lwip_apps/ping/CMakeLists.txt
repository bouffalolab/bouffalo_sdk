sdk_generate_library()

sdk_library_add_sources(ping.c)

sdk_add_link_options(-ucmd_ping)
