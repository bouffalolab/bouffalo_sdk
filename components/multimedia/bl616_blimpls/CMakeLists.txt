sdk_generate_library()

sdk_add_include_directories(include
                            # include/ble
                            # include/classic
                           )


sdk_add_compile_options(-Wno-nonnull-compare -Wno-unused-function -Wno-unused-but-set-variable)

# sdk_add_compile_definitions()

sdk_library_add_sources(
   src/ble_hal.c
   src/bt_hal.c
)

