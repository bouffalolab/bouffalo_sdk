sdk_generate_library()
							
sdk_add_include_directories(.)
sdk_add_include_directories(inc)

sdk_add_compile_definitions(-DCONFIG_BLSYNC)

sdk_library_add_sources(
src/blsync_ble.c
src/encrypt_layer.c
src/payload.c
src/transfer.c
blsync_ble_app.c
)