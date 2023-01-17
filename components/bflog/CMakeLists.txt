sdk_generate_library()
sdk_library_add_sources(bflog.c)
sdk_add_include_directories(.)
sdk_add_compile_definitions(-DCONFIG_BFLOG)

if(CONFIG_BFLOG_USER)
sdk_add_private_compile_definitions(-DCONFIG_BFLOG_USER)
endif()

if(CONFIG_BFLOG_DEBUG)
sdk_add_private_compile_definitions(-DCONFIG_BFLOG_DEBUG)
endif()