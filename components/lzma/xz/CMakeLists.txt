sdk_generate_library()
sdk_library_add_sources(xz_crc32.c xz_dec_lzma2.c xz_dec_stream.c xz_decompress.c xz_port.c)
sdk_add_include_directories(.)