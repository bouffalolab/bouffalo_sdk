sdk_generate_library()

sdk_add_include_directories(
    include
    )

# sdk_add_private_include_directories(
#     xcodec/src
#     )

sdk_library_add_sources(
    src/debug.c
    src/device.c
    src/list.c
    src/mringbuffer.c
    src/xringbuffer.c
    #src/time_patch.c
    )

sdk_add_compile_options(--save-temps)

sdk_add_compile_definitions(
  -DCONFIG_MSP_DEBUG=1
)
