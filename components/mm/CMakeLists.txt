sdk_generate_library()

sdk_add_include_directories(.)

sdk_library_add_sources(mem.c)

# memheap
if (CONFIG_TLSF)
    add_subdirectory(tlsf)
else()
    add_subdirectory(mmheap)
endif()

if(CONFIG_FREERTOS)
sdk_add_compile_definitions(-DconfigSTACK_ALLOCATION_FROM_SEPARATE_HEAP=1)
endif()