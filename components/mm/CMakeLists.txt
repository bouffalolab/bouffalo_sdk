sdk_generate_library()

sdk_add_include_directories(.)

sdk_library_add_sources(mem.c)

if(CONFIG_TLSF) 
sdk_library_add_sources_ifdef(CONFIG_TLSF_DEBUG tlsf/tlsf_debug.c)
sdk_library_add_sources(tlsf/tlsf.c tlsf/bflb_tlsf.c)
sdk_add_include_directories(tlsf)
else()
message(FATAL_ERROR "Please implement your own memheap and then disable this error")
endif()

if(CONFIG_FREERTOS)
sdk_add_compile_definitions(-DconfigSTACK_ALLOCATION_FROM_SEPARATE_HEAP=1)
endif()
