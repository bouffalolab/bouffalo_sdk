sdk_generate_library()

sdk_add_include_directories(
    include
    )

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
