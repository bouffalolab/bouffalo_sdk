sdk_generate_library()
							
sdk_add_include_directories(.)
sdk_add_include_directories(inc)

sdk_add_compile_definitions(-DCONFIG_BFLB_BLESYNC)

sdk_library_add_sources(
src/bflb_blesync.c
src/encrypt_layer.c
src/payload.c
src/transfer.c
bflb_blesync_app.c
)
if(CONFIG_WIFI6)
sdk_library_add_sources(wifi_adapter/wifiprov_bl616.c)
else()
sdk_library_add_sources(wifi_adapter/wifiprov_bl602.c)
endif()