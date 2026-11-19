sdk_generate_library()

sdk_add_include_directories(
    include
    )

if(CONFIG_XCODEC_IO)
    sdk_add_include_directories(interface device/include)
    sdk_add_private_include_directories(platform ../xport/include ../xutils/include)
    sdk_library_add_sources(
        xcodec_dev.c
        xcodec_dev_if.c
        platform/codec_data.c
        platform/xcodec_data_i2s.c
        platform/xcodec_data_audac.c
        platform/xcodec_data_auadc.c
        platform/xcodec_ctrl_i2c.c
        device/audio_codec/xcodec_audio_codec.c
        device/builtin/xcodec_builtin_codec.c
        )
    if(NOT CONFIG_XPORT)
        sdk_library_add_sources(../xport/src/msp_freertos.c ../xport/src/xav_port.c)
    endif()
    if(NOT CONFIG_XUTILS)
        sdk_library_add_sources(../xutils/src/mringbuffer.c ../xutils/src/debug.c)
    endif()
endif()

if(CONFIG_XCODEC)
sdk_add_private_include_directories(
    src/bl/codec
    )

if (CONFIG_CODEC_USE_I2S)
sdk_library_add_sources(
    src/bl/codec/codec_i2s_input.c
    src/bl/codec/codec_i2s_output.c
    )
elseif(CONFIG_CODEC_ENABLE_HFP)
sdk_library_add_sources(
    src/bl/codec/codec_hfp.c
    )
else()
sdk_library_add_sources(
    src/bl/codec/codec_auadc_input.c
    src/bl/codec/codec_audac_output.c
    )
endif()

sdk_library_add_sources(
    src/bl/codec/codec_debug.c
    src/bl/codec/codec_eqdrc.c
    src/bl/codec/xcodec.c
    )

sdk_add_private_compile_options(--save-temps)
endif()
