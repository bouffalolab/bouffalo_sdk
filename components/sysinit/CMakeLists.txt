sdk_generate_library()
sdk_library_add_sources(sysinit.c)
if(CONFIG_FREERTOS)
sdk_library_add_sources(sysinit_freertos.c)
if(CONFIG_SYSINIT AND CONFIG_COMPAT_V2)
# Intercept legacy main() calls to vTaskStartScheduler(); the wrapper in
# sysinit_freertos.c prints a deprecation warning and longjmps back to APP.
sdk_add_link_options(-Wl,--wrap,vTaskStartScheduler)
endif()
endif()
sdk_add_include_directories(.)
