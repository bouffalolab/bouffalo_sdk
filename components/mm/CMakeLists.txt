# CMakeLists.txt for Bouffalo SDK Memory Management Framework
# Copyright (c) 2025 Bouffalo Lab

sdk_generate_library()

sdk_add_include_directories(.)
sdk_library_add_sources(mm.c)

sdk_library_add_sources(mm_debug.c)

if(CONFIG_MEM_TLSF_EN)
    sdk_add_include_directories(tlsf)
    sdk_library_add_sources(tlsf/tlsf.c)
    sdk_library_add_sources(tlsf/tlsf_allocator.c)
endif()

if(CONFIG_MEM_HEAP5_EN)
    sdk_add_include_directories(heap5)
    sdk_library_add_sources(heap5/heap5.c)
    sdk_library_add_sources(heap5/heap5_allocator.c)
endif()
