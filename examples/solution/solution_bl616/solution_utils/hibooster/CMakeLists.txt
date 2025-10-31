
if(CONFIG_HIBOOSTER_SIMU_DATA_TEST)
    sdk_add_compile_definitions(-D SIMU_DATA_TEST)
endif()

target_sources(app PRIVATE bl_cam_frame.c)

if(CONFIG_SOLUTION_BUILD_FROM_LIB_MODE)
    sdk_add_static_library(libhibooster.a)
    sdk_add_include_directories(include)
else()
    sdk_generate_library()
    sdk_library_add_sources(
        src/sender.c
        src/mjpeg_sw_buffer.c
        src/sender_cli.c
        src/hibooster_config.c
    )
    sdk_add_include_directories(src include)
    sdk_add_compile_definitions(-D CPU_M0)
endif()
