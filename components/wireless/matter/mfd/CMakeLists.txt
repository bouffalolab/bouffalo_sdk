sdk_generate_library()

sdk_add_include_directories(./include)
sdk_add_private_include_directories(./)

sdk_library_add_sources(bflb_mfd.c bflb_mfd_decrypt.c)
