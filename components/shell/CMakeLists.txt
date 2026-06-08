sdk_generate_library()
sdk_library_add_sources(shell.c)
sdk_library_add_sources(shell_auto.c)
sdk_add_include_directories(.)

if(CONFIG_FREERTOS)
sdk_library_add_sources(shell_freertos.c)
endif()

if(CONFIG_SHELL_CMD_SIZE)
sdk_add_compile_definitions(-DSHELL_CMD_SIZE=${CONFIG_SHELL_CMD_SIZE})
endif()

if(CONFIG_SHELL_THREAD_STACK_SIZE)
sdk_add_compile_definitions(-DSHELL_THREAD_STACK_SIZE=${CONFIG_SHELL_THREAD_STACK_SIZE})
endif()
