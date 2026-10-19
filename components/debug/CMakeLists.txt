# DWARF CFI backtrace (unwind_6byte based)
if(CONFIG_BACKTRACE)
    sdk_generate_library()
    sdk_library_add_sources(src/unwind_6byte.c src/backtrace.c)
    if (CONFIG_MM_ENABLE_TRACKING AND NOT CONFIG_MEM_HEAP5_EN)
        sdk_library_add_sources(src/mm_track.c) #src/mm_track_test.c
    endif()
    sdk_add_include_directories(include)
endif()
