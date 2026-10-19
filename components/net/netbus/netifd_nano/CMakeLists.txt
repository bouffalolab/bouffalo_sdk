sdk_generate_library()

sdk_library_add_sources(netifd_nano.c
                        netifd_cli.c)

sdk_add_include_directories(include)
