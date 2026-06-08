sdk_generate_library()

sdk_add_include_directories(.)

if(CONFIG_POSIX)
sdk_add_include_directories(./posix)
endif()

if("${CHIP}" STREQUAL "bl618dg" AND "${CPU_MODEL}" STREQUAL "b0")
    sdk_add_include_directories(picolibc)
    sdk_library_add_sources(picolibc/port_errno.c)
    sdk_library_add_sources(picolibc/port_libcpp.c)
    sdk_library_add_sources(picolibc/port_random.c)
    sdk_library_add_sources(picolibc/stdio_streams.c)
    sdk_add_compile_options(
    -Wno-error=nonnull-compare
    )
    sdk_add_compile_definitions(-D__PICOLIBC_ERRNO_FUNCTION=__errno)
endif()

# ##################################################################################################
# 1.syscall select
sdk_library_add_sources(newlib/syscalls_simple_io.c)

if(CONFIG_NEWLIB_LITTLEFS)
    sdk_library_add_sources(newlib/syscalls.c)
    sdk_library_add_sources(newlib/port_tty.c)
    sdk_library_add_sources(newlib/port_file_littlefs.c)
elseif(CONFIG_NEWLIB_FATFS)
    sdk_library_add_sources(newlib/syscalls.c)
    sdk_library_add_sources(newlib/port_tty.c)
    sdk_library_add_sources(newlib/port_file_fatfs.c)
else()
    sdk_library_add_sources(newlib/port_file_nosys.c)
    sdk_library_add_sources(newlib/syscalls_nosys.c)
endif()

sdk_library_add_sources(snprintf.c)
sdk_library_add_sources(sprintf.c)
sdk_library_add_sources(vsprintf.c)
sdk_library_add_sources(printf.c)
sdk_library_add_sources(vprintf.c)
sdk_library_add_sources(assert.c)

# Select the vsnprintf method
if(CONFIG_VSNPRINTF_NANO)
    sdk_library_add_sources(vsnprintf_nano.c)
else()
    sdk_library_add_sources(vsnprintf.c)
endif()

sdk_add_link_options(
    -Wl,-wrap,snprintf
    -Wl,-wrap,sprintf
)

if(CONFIG_GCC_COMPILE_LTO)
sdk_add_link_options(
    -Wl,-u,__wrap_snprintf
    -Wl,-u,__wrap_sprintf
)
endif()

if(CONFIG_STACK_SWITCH_CALL_SSCANF)
    sdk_library_add_sources(sscanf.c)
    sdk_add_link_options(-Wl,-wrap,sscanf)
endif()

# vsnprintf %f %F
if(CONFIG_VSNPRINTF_FLOAT)
    sdk_add_compile_definitions(-D CONFIG_LIBC_FLOAT=1)
else()
    sdk_add_compile_definitions(-D CONFIG_LIBC_FLOAT=0)
endif()
# vsnprintf %g %G %e %E
if(CONFIG_VSNPRINTF_FLOAT_EX)
    sdk_add_compile_definitions(-D CONFIG_LIBC_FLOAT_EX=1)
endif()
# vsnprintf %lld %lli %llu %llx %llX %llo
if(CONFIG_VSNPRINTF_LONG_LONG)
    sdk_add_compile_definitions(-D CONFIG_LIBC_LONG_LONG=1)
endif()

sdk_library_add_sources(newlib/port_memory.c)
sdk_library_add_sources(newlib/port_time.c)
sdk_library_add_sources(newlib/port_init_fini.c)

sdk_library_add_sources(snprintf.c)
sdk_library_add_sources(sprintf.c)
sdk_library_add_sources(vsprintf.c)
sdk_library_add_sources(printf.c)
sdk_library_add_sources(vprintf.c)
sdk_library_add_sources(assert.c)

# Select the vsnprintf method
if(CONFIG_VSNPRINTF_NANO)
    sdk_library_add_sources(vsnprintf_nano.c)
else()
    sdk_library_add_sources(vsnprintf.c)
endif()

if(CONFIG_STACK_SWITCH_CALL_SSCANF)
    sdk_library_add_sources(sscanf.c)
    sdk_add_link_options(-Wl,-wrap,sscanf)
endif()

# vsnprintf %f %F
if(CONFIG_VSNPRINTF_FLOAT)
    sdk_add_compile_definitions(-D CONFIG_LIBC_FLOAT=1)
else()
    sdk_add_compile_definitions(-D CONFIG_LIBC_FLOAT=0)
endif()
# vsnprintf %g %G %e %E
if(CONFIG_VSNPRINTF_FLOAT_EX)
    sdk_add_compile_definitions(-D CONFIG_LIBC_FLOAT_EX=1)
endif()


# ##################################################################################################
# 2.stdlib and string select
# use nuttx stdlib and string implementations
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
    # nuttx/libc/stdlib/lib_strtof.c nuttx/libc/stdlib/lib_strtol.c
    # nuttx/libc/stdlib/lib_strtold.c nuttx/libc/stdlib/lib_strtoll.c
    # nuttx/libc/stdlib/lib_strtoull.c
)

# use nuttx string
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
    # nuttx/libc/string/lib_memcmp.c
    # nuttx/libc/string/lib_memcpy.c  # use lib_vikmemcpy.c
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
    # nuttx/libc/string/lib_isbasedigit.c nuttx/libc/string/lib_skipspace.c
    # nuttx/libc/string/lib_vikmemcpy.c
)

if(NOT CONFIG_BOOT2)
    sdk_library_add_sources(
        nuttx/libc/string/lib_memcmp.c
        nuttx/libc/string/lib_vikmemcpy.c
    )
endif()

# use custom apis first, if not exist, then use builtin apis
sdk_add_compile_options(-fno-builtin)

if(CONFIG_STDATOMIC)
    # add atomic
    sdk_library_add_sources(stdatomic.c)
endif()
