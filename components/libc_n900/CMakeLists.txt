sdk_generate_library()

sdk_add_include_directories(.)

# ==============================================================================
# 1. Common syscall and port files (shared by all implementations)
# ==============================================================================
sdk_library_add_sources(picolibc/syscalls.c)
sdk_library_add_sources(picolibc/port_memory.c)

message(STATUS "Using picolibc from Zephyr SDK toolchain")

# Picolibc needs stdio_streams.c to provide stdin/stdout/stderr pointers
sdk_library_add_sources(picolibc/stdio_streams.c)
sdk_library_add_sources(picolibc/port_tty.c)
sdk_library_add_sources(picolibc/port_time.c)
sdk_library_add_sources(picolibc/port_init_fini.c)
sdk_library_add_sources(picolibc/port_errno.c)
sdk_library_add_sources(picolibc/port_libcpp.c)
sdk_add_compile_definitions(-D__PICOLIBC_ERRNO_FUNCTION=__errno)

# Use custom printf family implementation instead of picolibc's
sdk_library_add_sources(printf.c)
sdk_library_add_sources(snprintf.c)
sdk_library_add_sources(sprintf.c)
sdk_library_add_sources(vsprintf.c)
if(CONFIG_VSNPRINTF_NANO)
    sdk_library_add_sources(vsnprintf_nano.c)
else()
    sdk_library_add_sources(vsnprintf.c)
endif()
sdk_library_add_sources(vprintf.c)

# Compile definitions for custom vsnprintf
if(CONFIG_VSNPRINTF_FLOAT)
    sdk_add_compile_definitions(-D CONFIG_LIBC_FLOAT=1)
else()
    sdk_add_compile_definitions(-D CONFIG_LIBC_FLOAT=0)
endif()
if(CONFIG_VSNPRINTF_FLOAT_EX)
    sdk_add_compile_definitions(-D CONFIG_LIBC_FLOAT_EX=1)
endif()
if(CONFIG_VSNPRINTF_LONG_LONG)
    sdk_add_compile_definitions(-D CONFIG_LIBC_LONG_LONG=1)
endif()
sdk_add_compile_definitions(-DCONFIG_NEWLIB)

# Linker wrappers for custom implementations
sdk_add_link_options(
    -Wl,-wrap,snprintf
    -Wl,-wrap,sprintf
    -Wl,-u,__wrap_snprintf
    -Wl,-u,__wrap_sprintf
)

# Force linking of float support
if(CONFIG_VSNPRINTF_FLOAT OR CONFIG_VSNPRINTF_FLOAT_EX)
    sdk_add_link_options(-u_printf_float)
endif()

# Filesystem support
if(CONFIG_NEWLIB_FATFS)
    sdk_library_add_sources(picolibc/port_file_fatfs.c)
    message(STATUS "Picolibc with FATFS filesystem support")
else()
    sdk_library_add_sources(picolibc/port_file_nosys.c)
    message(STATUS "Picolibc without filesystem support")
endif()

# Picolibc provides all standard library functions:
# - stdio: printf, scanf, fprintf, fscanf, sprintf, sscanf, snprintf, vsnprintf, etc.
# - stdlib: malloc, free, atoi, atol, strtod, strtol, strtoll, qsort, bsearch, abs, etc.
# - string: strcpy, strncpy, strcat, strncat, strcmp, strncmp, strchr, strstr, etc.
# - math: sin, cos, tan, sqrt, pow, exp, log, etc. (from libm)
# - assert: assert macro and function
# No custom implementations needed - all from picolibc.a

# ==============================================================================
# 3. Compiler builtins control
# ==============================================================================
# Disable builtins only for custom implementations
# Picolibc and picolibc-standard work with compiler builtins
if((NOT CONFIG_NEWLIB) AND (NOT CONFIG_LIBC_PICOLIBC) AND (NOT CONFIG_NEWLIB_STANDARD))
    sdk_add_compile_options(-fno-builtin)
    message(STATUS "Disabling compiler builtins for custom implementations")
endif()

# ==============================================================================
# 4. Optional features
# ==============================================================================
if(CONFIG_STDATOMIC)
    # C11 atomic operations support
    sdk_library_add_sources(stdatomic.c)
    message(STATUS "C11 stdatomic support enabled")
endif()
