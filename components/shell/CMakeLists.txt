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

# Get the commit hash and dirty status
execute_process(
    COMMAND git describe --always --abbrev=8 --dirty
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Add definition for the version string and commit hash
sdk_add_compile_definitions(-DBOUFFALO_SDK_VER="${GIT_COMMIT_HASH}")
