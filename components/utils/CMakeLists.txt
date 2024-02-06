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

if(CONFIG_LOG_POOL_SIZE)
    sdk_add_compile_definitions(-DCONFIG_LOG_POOL_SIZE=${CONFIG_LOG_POOL_SIZE})
endif()


# log
if(CONFIG_LOG_NCOLOR)
    sdk_add_compile_definitions(-DCONFIG_LOG_NCOLOR=1)
endif()

sdk_library_add_sources(log/log.c)
sdk_add_include_directories(log)

if(CONFIG_BFLOG)
    sdk_add_compile_definitions(-DCONFIG_BFLOG)
    sdk_library_add_sources(log/bflog/bflog.c)
    sdk_add_include_directories(log/bflog)
    if(CONFIG_FREERTOS)
        sdk_library_add_sources(log/log_freertos.c)
    endif()
    if(CONFIG_BFLOG_DEBUG)
    sdk_add_private_compile_definitions(-DCONFIG_BFLOG_DEBUG)
    endif()
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

if(DEFINED CONFIG_TIMEZONE)
    sdk_add_compile_definitions(-DBFLB_TIMESTAMP_TIMEZONE=${CONFIG_TIMEZONE})
else()
    sdk_add_compile_definitions(-DBFLB_TIMESTAMP_TIMEZONE=8)
endif()

# timestamp
sdk_library_add_sources(bflb_timestamp/bflb_timestamp.c)
sdk_add_include_directories(bflb_timestamp)

# getopt
sdk_library_add_sources(getopt/utils_getopt.c)
sdk_add_include_directories(getopt)

if(CONFIG_COREDUMP)
sdk_add_compile_definitions(-DCONFIG_COREDUMP)
sdk_library_add_sources(coredump/bflb_coredump.c)
sdk_add_include_directories(coredump)
endif()

# partition
if(CONFIG_PARTITION)
sdk_library_add_sources(partition/partition.c)
sdk_add_include_directories(partition)
endif()

# bflb_mtd
if(CONFIG_BFLB_MTD)
sdk_library_add_sources(bflb_mtd/bflb_mtd.c bflb_mtd/bflb_boot2.c)
sdk_add_include_directories(bflb_mtd/include)
endif()

#cjson
sdk_library_add_sources(cjson/cJSON.c)
sdk_add_include_directories(cjson)

# math
sdk_library_add_sources(math/src/utils_base64.c math/src/utils_crc.c math/src/utils_hex.c)
sdk_add_include_directories(math/include)

# ota
if(CONFIG_BFLB_OTA)
sdk_library_add_sources(bflb_ota/bflb_ota.c bflb_ota/utils_sha256.c)
sdk_add_include_directories(bflb_ota)
endif()

# list
sdk_add_include_directories(list)