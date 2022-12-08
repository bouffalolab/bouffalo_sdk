sdk_generate_library()

if(CONFIG_LOG_DISABLE)
    sdk_add_compile_definitions(-DCONFIG_LOG_DISABLE)
endif()

if(CONFIG_ASSERT_DISABLE)
    sdk_add_compile_definitions(-DCONFIG_ASSERT_DISABLE)
endif()

if(DEFINED CONFIG_LOG_LEVEL)
    sdk_add_compile_definitions(-DCONFIG_LOG_LEVEL=${CONFIG_LOG_LEVEL})
else()
    sdk_add_compile_definitions(-DCONFIG_LOG_LEVEL=3)
endif()

if(CONFIG_BFLOG_POOL_SIZE)
    sdk_add_compile_definitions(-DCONFIG_BFLOG_POOL_SIZE=${CONFIG_BFLOG_POOL_SIZE})
endif()


# log
sdk_library_add_sources(log/log.c)
sdk_add_include_directories(log)

# vsnprintf config
if(CONFIG_VSNPRINTF_FLOAT)
    sdk_add_compile_definitions(-DCONFIG_VLIBC_FLOAT=${CONFIG_VSNPRINTF_FLOAT})
    sdk_add_compile_definitions(-DCONFIG_LIBC_FLOAT=${CONFIG_VSNPRINTF_FLOAT})
endif()
if(CONFIG_VSNPRINTF_FLOAT_EX)
    sdk_add_compile_definitions(-DCONFIG_VLIBC_FLOAT_EX=${CONFIG_VSNPRINTF_FLOAT_EX})
    sdk_add_compile_definitions(-DCONFIG_LIBC_FLOAT_EX=${CONFIG_VSNPRINTF_FLOAT_EX})
endif()
if(CONFIG_VSNPRINTF_LONG_LONG)
    sdk_add_compile_definitions(-DCONFIG_VLIBC_LONG_LONG=${CONFIG_VSNPRINTF_LONG_LONG})
    sdk_add_compile_definitions(-DCONFIG_LIBC_LONG_LONG=${CONFIG_VSNPRINTF_LONG_LONG})
endif()
if(CONFIG_VSNPRINTF_WRITEBACK)
    sdk_add_compile_definitions(-DCONFIG_VLIBC_WRITEBACK=${CONFIG_VSNPRINTF_WRITEBACK})
endif()

# libc or vlibc select
if(CONFIG_VLIBC)
    sdk_add_compile_definitions(-DCONFIG_VLIBC)
    # vlibc debug enable
    if (CONFIG_VLIBC_DEBUG)
        sdk_add_compile_definitions(-DCONFIG_VLIBC_DEBUG)
    endif()

    # vlibc fatfs port enable
    if(CONFIG_VLIBC_FATFS)
        sdk_add_compile_definitions(-DCONFIG_VLIBC_FATFS)
    endif()

    sdk_library_add_sources(vlibc/printf.c)
    sdk_library_add_sources(vlibc/syscalls.c)
    sdk_library_add_sources(vlibc/vlibc_stdio.c)
    sdk_library_add_sources(vlibc/vlibc_vsnprintf.c)
    sdk_add_include_directories(vlibc)
else()
    if(CONFIG_VSNPRINTF_NANO)
    sdk_library_add_sources(libc/vsnprintf_nano.c)
    else()
    sdk_library_add_sources(libc/vsnprintf.c)
    endif()

    sdk_library_add_sources(libc/syscalls.c)
    sdk_library_add_sources(libc/printf.c)
endif()

# memheap
sdk_library_add_sources(mmheap/bflb_mmheap.c)
sdk_add_include_directories(mmheap)

# memheap lock user config
if (CONFIG_MMHEAP_USER)
    sdk_add_compile_definitions(-DCONFIG_MMHEAP_USER)
endif()

# ring_buffer
sdk_library_add_sources(ring_buffer/ring_buffer.c)
sdk_add_include_directories(ring_buffer)

# bflb block pool debug enable
if (CONFIG_BFLB_BLOCK_POOL_DEBUG)
    sdk_add_compile_definitions(-DCONFIG_BFLB_BLOCK_POOL_DEBUG)
endif()

# bflb block pool mutex
if (CONFIG_BFLB_BLOCK_POOL_MTX_TIMEOUT)
    sdk_add_compile_definitions(-DCONFIG_BFLB_BLOCK_POOL_MTX_TIMEOUT=${CONFIG_BFLB_BLOCK_POOL_MTX_TIMEOUT})
endif()

# bflb block pool
sdk_library_add_sources(bflb_block_pool/bflb_block_pool.c)
sdk_add_include_directories(bflb_block_pool)

# soft crc
sdk_library_add_sources(soft_crc/soft_crc.c)
sdk_add_include_directories(soft_crc)

if(DEFINED CONFIG_TIMEZONE)
    sdk_add_compile_definitions(-DBFLB_TIMESTAMP_TIMEZONE=${CONFIG_TIMEZONE})
else()
    sdk_add_compile_definitions(-DBFLB_TIMESTAMP_TIMEZONE=8)
endif()

# timestamp
sdk_library_add_sources(bflb_timestamp/bflb_timestamp.c)
sdk_add_include_directories(bflb_timestamp)

sdk_add_compile_options(-fno-builtin-printf)
sdk_add_link_options(-u_malloc_r)