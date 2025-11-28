# CMakeLists.txt for Bouffalo SDK Memory Management Framework
# Copyright (c) 2025 Bouffalo Lab

sdk_generate_library()

sdk_add_include_directories(.)
sdk_library_add_sources(mm.c)

sdk_library_add_sources(mm_debug.c)

if(NOT DEFINED CONFIG_MEM_TLSF_EN)
    set(CONFIG_MEM_TLSF_EN y)
endif()

if(CONFIG_MEM_TLSF_EN)
    sdk_add_include_directories(tlsf)
    sdk_library_add_sources(tlsf/tlsf.c)
    sdk_library_add_sources(tlsf/tlsf_allocator.c)
endif()

if(CONFIG_MEM_HEAP5_EN)

endif()
