sdk_generate_library()

add_subdirectory(newlib)

# use newlib stdlib or nuttx stdlib
if(NOT CONFIG_NEWLIB_STANDARD)
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
endif()

# use newlib string or nuttx string
if(NOT CONFIG_NEWLIB_STANDARD)
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
endif()

# is boot2
if(NOT CONFIG_BOOT2)
# use newlib memcpy or nuttx memcpy
if(NOT CONFIG_NEWLIB_STANDARD)
sdk_library_add_sources(
nuttx/libc/string/lib_vikmemcpy.c
)
endif()
else()
sdk_library_add_sources(
nuttx/libc/string/lib_memcpy.c
)
endif()

sdk_add_include_directories(.)

if(NOT CONFIG_NEWLIB_STANDARD)
if(CONFIG_VSNPRINTF_NANO)
sdk_add_compile_definitions(-DCONFIG_VSNPRINTF_NANO=1)
sdk_library_add_sources(vsnprintf_nano.c)
else()
sdk_library_add_sources(vsnprintf.c)
endif()
endif()

if(NOT CONFIG_NEWLIB)
sdk_library_add_sources(snprintf.c)
sdk_library_add_sources(sprintf.c)
sdk_library_add_sources(vsprintf.c)
sdk_library_add_sources(printf.c)

if(CONFIG_STACK_SWITCH_CALL_SSCANF)
sdk_library_add_sources(sscanf.c)
sdk_add_link_options(-Wl,-wrap,sscanf)
endif()

sdk_add_link_options(-Wl,-wrap,snprintf -Wl,-wrap,sprintf)
endif()

# vsnprintf %f %F
if(CONFIG_VSNPRINTF_FLOAT)
    sdk_add_compile_definitions(-DCONFIG_LIBC_FLOAT=1)
elseif((CONFIG_VSNPRINTF_FLOAT EQUAL 0) )
    sdk_add_compile_definitions(-DCONFIG_LIBC_FLOAT=0)
endif()
# vsnprintf %g %G %e %E
if(CONFIG_VSNPRINTF_FLOAT_EX)
    sdk_add_compile_definitions(-DCONFIG_LIBC_FLOAT_EX=${CONFIG_VSNPRINTF_FLOAT_EX})
endif()
# vsnprintf %lld %lli %llu %llx %llX %llo
if(CONFIG_VSNPRINTF_LONG_LONG)
    sdk_add_compile_definitions(-DCONFIG_LIBC_LONG_LONG=${CONFIG_VSNPRINTF_LONG_LONG})
endif()

# use custom apis first, if not exist, then use builtin apis
sdk_add_compile_options(-fno-builtin)
