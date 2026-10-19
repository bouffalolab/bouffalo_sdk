sdk_generate_library()

sdk_add_include_directories(.)

sdk_library_add_sources(
    mquickjs.c
    cutils.c
    dtoa.c
    libm.c
    bouffalo_mquickjs.c
)
