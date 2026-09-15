sdk_generate_library()
sdk_library_add_sources(shell.c)
sdk_library_add_sources(shell_auto.c)
sdk_add_include_directories(.)

if(CONFIG_FREERTOS)
sdk_library_add_sources(shell_freertos.c)
endif()
