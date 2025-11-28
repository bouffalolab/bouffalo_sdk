sdk_generate_library()

if(NOT DEFINED CONFIG_LOG_LEVEL)
    sdk_add_compile_definitions(-DCONFIG_LOG_LEVEL=3)
endif()

sdk_library_add_sources(log/log.c)
sdk_add_include_directories(log)

if(CONFIG_BFLB_LOG)
    sdk_library_add_sources(log/bflb_log/bflb_log.c)
    sdk_add_include_directories(log/bflb_log)
    if(CONFIG_FREERTOS)
        sdk_library_add_sources(log/log_freertos.c)
    endif()
endif()

# ring_buffer
sdk_library_add_sources(ring_buffer/ring_buffer.c)
sdk_add_include_directories(ring_buffer)

# bflb block pool
sdk_library_add_sources(bflb_block_pool/bflb_block_pool.c)
sdk_add_include_directories(bflb_block_pool)

if(NOT DEFINED CONFIG_TIMEZONE)
    sdk_add_compile_definitions(-DBFLB_TIMESTAMP_TIMEZONE=8)
endif()

# timestamp
sdk_library_add_sources(bflb_timestamp/bflb_timestamp.c)
sdk_add_include_directories(bflb_timestamp)

# getopt
sdk_library_add_sources(getopt/utils_getopt.c)
sdk_add_include_directories(getopt)

if(CONFIG_COREDUMP_V2)
sdk_add_compile_definitions(-DCONFIG_COREDUMP)
sdk_library_add_sources(coredump/coredump.c)
sdk_library_add_sources(coredump/encoder_v2.c)
sdk_library_add_sources(coredump/bin_fmt_v2.c)
sdk_add_include_directories(coredump)
elseif(CONFIG_COREDUMP)
sdk_library_add_sources(coredump/coredump.c)
sdk_library_add_sources(coredump/encoder_v1.c)
sdk_library_add_sources(coredump/bin_fmt_v1.c)
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
sdk_library_add_sources(cjson/cJSON.c cjson/cJSON_Utils.c)
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
sdk_library_add_sources(list/utils_list.c)

# async events (yloop)
if(CONFIG_FREERTOS)
sdk_library_add_sources(async_event/async_event.c async_event/rtos_al.c)
sdk_add_include_directories(async_event)
endif()
