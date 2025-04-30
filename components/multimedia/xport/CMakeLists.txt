sdk_generate_library()

sdk_add_include_directories(
    include
    )

# sdk_add_private_include_directories(
#     src
#     )

if (CONFIG_MSP_USE_CODEC_ONLY)
sdk_library_add_sources(
    src/msp_freertos.c
    src/msp_port.c
    src/xav_port.c
    )
else()
sdk_library_add_sources(
    src/msp_freertos.c
    # src/msp_dma.c
    src/msp_port.c
    src/web_lower.c
    src/xav_port.c
    src/xav_avcodec_all.c
    src/xav_resample_all.c
    src/xav_stream_all.c
    src/xav_atempo_all.c
    src/xav_aef_all.c
    src/xav_format_all.c
    src/xav_parser_all.c
    src/msp_fatfs.c
    src/msp_romfs.c
    src/msp_fs.c
    src/msp_ltfs.c
    #src/codec_es8388.c
    )
endif()

if (CONFIG_CODEC_USE_I2S_RX OR CONFIG_CODEC_USE_I2S_TX OR CONFIG_CODEC_USE_ES8388)
    sdk_library_add_sources(src/codec_es8388.c
    )
endif()

if (CONFIG_ROMFS)
    sdk_add_compile_definitions(-DCONFIG_ROMFS=1)
endif()

if (CONFIG_LTFS)
    sdk_add_compile_definitions(-DCONFIG_LTFS=1)
endif()

if (CONFIG_FATFS)
    sdk_add_compile_definitions(-DCONFIG_FATFS=1)
endif()

sdk_add_private_compile_options(--save-temps)
