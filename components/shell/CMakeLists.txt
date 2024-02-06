sdk_generate_library()
sdk_library_add_sources(shell.c)
sdk_add_include_directories(.)

if(CONFIG_FREERTOS)
sdk_library_add_sources(shell_freertos.c)
endif()

sdk_add_compile_definitions(-DCONFIG_SHELL)
if(CONFIG_SHELL_EXEC_THREAD)
sdk_add_compile_definitions(-DCONFIG_SHELL_EXEC_THREAD)
endif()

if(CONFIG_SHELL_CMD_SIZE)
sdk_add_compile_definitions(-DSHELL_CMD_SIZE=${CONFIG_SHELL_CMD_SIZE})
endif()
