sdk_library_add_sources(mmheap.c)
sdk_library_add_sources(bflb_mmheap.c)

sdk_add_include_directories(.)

# memheap lock user config
if(CONFIG_MMHEAP_USER)
    sdk_add_compile_definitions(-DCONFIG_MMHEAP_USER)
endif()
