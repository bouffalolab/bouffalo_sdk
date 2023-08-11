sdk_generate_library()

sdk_library_add_sources(bflb_romfs.c)
sdk_add_include_directories(.)

sdk_add_compile_definitions(-DCONFIG_ROMFS)