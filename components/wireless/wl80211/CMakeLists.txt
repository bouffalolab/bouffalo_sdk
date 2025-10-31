sdk_generate_library()

sdk_add_private_compile_options(-Wno-unused-function)
sdk_add_private_compile_options(-Wno-unused-variable)

# !! MANDATORY REQUIREMENT !!
sdk_add_private_compile_options(-Werror)
# !! MANDATORY REQUIREMENT !!

## These include paths would be exported to project level
sdk_add_include_directories(include)

sdk_add_private_include_directories(.)

sdk_library_add_sources(wl80211.c)
sdk_library_add_sources(timeout.c)
sdk_library_add_sources(rtos_al.c)
sdk_library_add_sources(cntrl.c)
sdk_library_add_sources(lwip.c)

sdk_library_add_sources(invariants.c)
sdk_library_add_sources(wifi_mgmr.c)

sdk_add_private_include_directories(macsw)
sdk_library_add_sources(macsw/macif_wl80211.c)
sdk_library_add_sources(macsw/macsw.c)
sdk_library_add_sources(macsw/rx.c)
sdk_library_add_sources(macsw/tx.c)
sdk_library_add_sources(macsw/scan.c)
sdk_library_add_sources(macsw/connect.c)
sdk_library_add_sources(macsw/disconnect.c)
sdk_library_add_sources(macsw/ps.c)

sdk_library_add_sources(macsw/supplicant_api.c)
