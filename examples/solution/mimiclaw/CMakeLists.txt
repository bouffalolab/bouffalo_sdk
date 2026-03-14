cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)
sdk_add_compile_definitions(-DWEBSOCKET_SSL_CHECK_HOSTNAME)
sdk_add_compile_definitions(-DSHELL_CMD_SIZE=512)

# Keep upstream code and local adaptation decoupled.
target_sources(app PRIVATE
    app/src/mimiclaw_app.c
    port/src/mimiclaw_port.c
    port/src/mimiclaw_transport_ws.c
    port/src/mimiclaw_storage.c
    port/src/mimiclaw_ext_rtc.c
    port/src/mimiclaw_tool_get_time.c
    port/src/mimiclaw_tool_files.c
    port/src/mimiclaw_tool_web_search.c
    port/src/mimiclaw_audio_player.c
    port/src/mimiclaw_tool_audio_playback.c
    port/src/mimiclaw_llm_proxy.c
    port/src/mimiclaw_agent_loop.c
    port/src/mimiclaw_telegram_bot.c
    upstream/main/bus/message_bus.c
    upstream/main/cron/cron_service.c
    upstream/main/tools/tool_cron.c
    upstream/main/tools/tool_registry.c
)

sdk_add_include_directories(.)
sdk_add_include_directories(app/include)
sdk_add_include_directories(port/include)
sdk_add_include_directories(compat)
sdk_add_include_directories(upstream/main)

sdk_set_main_file(main.c)

project(mimiclaw)
