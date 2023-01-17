sdk_generate_library()

sdk_library_add_sources(
nuttx/libc/stdlib/lib_abs.c
nuttx/libc/stdlib/lib_atof.c
nuttx/libc/stdlib/lib_atoi.c
nuttx/libc/stdlib/lib_atol.c
nuttx/libc/stdlib/lib_atoll.c
nuttx/libc/stdlib/lib_bsearch.c
nuttx/libc/stdlib/lib_checkbase.c
nuttx/libc/stdlib/lib_itoa.c
nuttx/libc/stdlib/lib_llabs.c
nuttx/libc/stdlib/lib_lldiv.c
nuttx/libc/stdlib/lib_qsort.c
nuttx/libc/stdlib/lib_strtod.c
# nuttx/libc/stdlib/lib_strtof.c
# nuttx/libc/stdlib/lib_strtol.c
# nuttx/libc/stdlib/lib_strtold.c
# nuttx/libc/stdlib/lib_strtoll.c
# nuttx/libc/stdlib/lib_strtoull.c
)

sdk_library_add_sources(
nuttx/libc/string/lib_ffs.c
nuttx/libc/string/lib_ffsl.c
nuttx/libc/string/lib_ffsll.c
nuttx/libc/string/lib_fls.c
nuttx/libc/string/lib_flsl.c
nuttx/libc/string/lib_flsll.c
nuttx/libc/string/lib_index.c
nuttx/libc/string/lib_memccpy.c
nuttx/libc/string/lib_memchr.c
nuttx/libc/string/lib_memcmp.c
nuttx/libc/string/lib_memmove.c
nuttx/libc/string/lib_memrchr.c
nuttx/libc/string/lib_memset.c
nuttx/libc/string/lib_stpcpy.c
nuttx/libc/string/lib_stpncpy.c
nuttx/libc/string/lib_strcasecmp.c
nuttx/libc/string/lib_strcasestr.c
nuttx/libc/string/lib_strcat.c
nuttx/libc/string/lib_strchr.c
nuttx/libc/string/lib_strcmp.c
nuttx/libc/string/lib_strcspn.c
nuttx/libc/string/lib_strdup.c
nuttx/libc/string/lib_strnlen.c
nuttx/libc/string/lib_strpbrk.c
nuttx/libc/string/lib_strrchr.c
nuttx/libc/string/lib_strsep.c
nuttx/libc/string/lib_strspn.c
nuttx/libc/string/lib_strstr.c
nuttx/libc/string/lib_strtok.c
nuttx/libc/string/lib_strtokr.c
# nuttx/libc/string/lib_isbasedigit.c
# nuttx/libc/string/lib_skipspace.c
)

sdk_library_add_sources(
nuttx/libc/string/lib_vikmemcpy.c
)

sdk_add_include_directories(.)

# libc or vlibc select
if(CONFIG_VLIBC)
    # vsnprintf config
    if(CONFIG_VSNPRINTF_FLOAT)
        sdk_add_compile_definitions(-DCONFIG_VLIBC_FLOAT=${CONFIG_VSNPRINTF_FLOAT})
    endif()

    if(CONFIG_VSNPRINTF_FLOAT_EX)
        sdk_add_compile_definitions(-DCONFIG_VLIBC_FLOAT_EX=${CONFIG_VSNPRINTF_FLOAT_EX})
    endif()

    if(CONFIG_VSNPRINTF_LONG_LONG)
        sdk_add_compile_definitions(-DCONFIG_VLIBC_LONG_LONG=${CONFIG_VSNPRINTF_LONG_LONG})
    endif()

    if(CONFIG_VSNPRINTF_WRITEBACK)
        sdk_add_compile_definitions(-DCONFIG_VLIBC_WRITEBACK=${CONFIG_VSNPRINTF_WRITEBACK})
    endif()

    sdk_add_compile_definitions(-DCONFIG_VLIBC)

    # vlibc debug enable
    if(CONFIG_VLIBC_DEBUG)
        sdk_add_compile_definitions(-DCONFIG_VLIBC_DEBUG)
    endif()

    # vlibc fatfs port enable
    if(CONFIG_VLIBC_FATFS)
        sdk_add_compile_definitions(-DCONFIG_VLIBC_FATFS)
    endif()

    sdk_library_add_sources(vlibc/printf.c)
    sdk_library_add_sources(vlibc/vlibc_stdio.c)
    sdk_library_add_sources(vlibc/vlibc_vsnprintf.c)
    sdk_add_include_directories(vlibc)
else()
    # vsnprintf config
    if(CONFIG_VSNPRINTF_FLOAT)
        sdk_add_compile_definitions(-DCONFIG_LIBC_FLOAT=${CONFIG_VSNPRINTF_FLOAT})
    endif()

    if(CONFIG_VSNPRINTF_FLOAT_EX)
        sdk_add_compile_definitions(-DCONFIG_LIBC_FLOAT_EX=${CONFIG_VSNPRINTF_FLOAT_EX})
    endif()

    if(CONFIG_VSNPRINTF_LONG_LONG)
        sdk_add_compile_definitions(-DCONFIG_LIBC_LONG_LONG=${CONFIG_VSNPRINTF_LONG_LONG})
    endif()

    if(CONFIG_VSNPRINTF_NANO)
        sdk_library_add_sources(vsnprintf_nano.c)
    else()
        sdk_library_add_sources(vsnprintf.c)
    endif()

    sdk_library_add_sources(printf.c)
endif()

sdk_library_add_sources(snprintf.c)

# use custom apis first, if not exist, then use builtin apis
sdk_add_compile_options(-fno-builtin)
