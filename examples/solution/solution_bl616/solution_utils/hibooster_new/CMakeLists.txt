# HiBooster New Implementation
sdk_add_include_directories(inc)

# porting
target_sources(app PRIVATE port/hb_port.c)

# HiBooster Test (only add if testing)
if(CONFIG_HB_SEND_TEST)
    sdk_add_include_directories(test)
    target_sources(app PRIVATE test/hb_sender_test.c)
endif()
if(CONFIG_HB_RECV_TEST)
    target_sources(app PRIVATE test/hb_receiver_test.c)
endif()

if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)
    sdk_generate_library()
    sdk_add_include_directories(src)
    sdk_library_add_sources(src/hb_sender_raw.c)
    sdk_library_add_sources(src/hb_receiver_raw.c)
else()
    sdk_add_static_library(lib/libhibooster_e907.a)
endif()
