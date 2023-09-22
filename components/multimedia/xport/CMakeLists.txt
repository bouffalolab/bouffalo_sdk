sdk_generate_library()

sdk_add_include_directories(
    include
    )

# sdk_add_private_include_directories(
#     src
#     )

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
    src/msp_romfs.c
    #src/codec_es8388.c
    )

if (CONFIG_CODEC_USE_I2S_TX)
    sdk_add_compile_definitions(-DCONFIG_CODEC_USE_I2S_TX=1)
    sdk_library_add_sources(src/codec_es8388.c)
endif()

if (CONFIG_CODEC_USE_I2S_RX)
    sdk_add_compile_definitions(-DCONFIG_CODEC_USE_I2S_RX=1)
    sdk_library_add_sources(src/codec_es8388.c)
endif()


sdk_add_compile_options(--save-temps)
