sdk_generate_library()

sdk_add_include_directories(
    include
    src/bl616/drv
    )

sdk_add_private_include_directories(
    src/bl616/codec
    src/bl616/drv
    src/bl616/port
    )

if (CONFIG_CODEC_USE_I2S_TX)
sdk_add_compile_definitions(
    -DCONFIG_CODEC_USE_I2S_TX=1
    )
endif()

if (CONFIG_CODEC_USE_I2S_RX)
sdk_add_compile_definitions(
    -DCONFIG_CODEC_USE_I2S_RX=1
    )
endif()

if (CONFIG_CODEC_USE_ES8388)
sdk_add_compile_definitions(
    -DCONFIG_CODEC_USE_ES8388=1
    )
sdk_library_add_sources(
    src/bl616/codec/codec_es8388_input.c
    src/bl616/codec/codec_es8388_output.c
    )
elseif(CONFIG_CODEC_EBABLE_HFP)
sdk_library_add_sources(
    src/bl616/codec/codec_hfp.c
    )
else()
sdk_library_add_sources(
    src/bl616/codec/codec_input.c
    src/bl616/codec/codec_output.c
    )
endif()

sdk_library_add_sources(
    src/bl616/codec/codec_debug.c
    src/bl616/codec/codec_eqdrc.c
    src/bl616/codec/codec_volume.c
    src/bl616/codec/xcodec.c
    src/bl616/drv/msp_bl616_auadc.c
    src/bl616/drv/msp_bl616_aupwm.c
    src/bl616/drv/msp_bl616_dma.c
    src/bl616/drv/msp_bl616_glb.c
    src/bl616/drv/msp_bl616_glb_gpio.c
    src/bl616/port/msp_sys.c
    src/bl616/codec/test/test_output.c
    )

sdk_add_private_compile_options(--save-temps)
