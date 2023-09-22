sdk_generate_library()

sdk_add_include_directories(
    include
    include/bt
    )

# sdk_add_private_include_directories(
#     include/bt
#     )

sdk_library_add_sources(
    src/aud_ctrl_case/local_play.c
    src/aud_ctrl_case/online_music.c
    src/smart_audio.c
    )

if(CONFIG_BT_A2DP)
sdk_library_add_sources(
    src/aud_ctrl_case/bt_a2dp.c
    src/aud_ctrl_case/bt_vol.c
    )
endif()

sdk_add_compile_definitions(
  -DCONFIG_SMART_AUDIO_STACK_SIZE=1024
  -DCONFIG_SMART_AUDIO=1
)

if(CONFIG_BT_A2DP)
sdk_add_compile_definitions(
  -DCONFIG_BT_A2DP=1
)
endif()

sdk_add_compile_options(--save-temps)
