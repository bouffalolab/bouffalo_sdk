
sdk_generate_library()

sdk_library_add_sources(rpmsg_backend.c)
sdk_library_add_sources(rpmsg_service.c)

sdk_add_include_directories(.)
