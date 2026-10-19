# HiBooster New Implementation

sdk_add_include_directories(inc)

# porting
target_sources(app PRIVATE port/hb_port.c)

# HiBooster Test (only add if testing)
if(CONFIG_SOLN_HB_TX_TEST_EN)
    sdk_add_include_directories(test)
    target_sources(app PRIVATE test/hb_sender_test.c)
endif()
if(CONFIG_SOLN_HB_RX_TEST_EN)
    target_sources(app PRIVATE test/hb_receiver_test.c)
endif()

# Private source when available, otherwise lib/libhibooster_${CHIP}.a
if(SOLN_PRIVATE_SOURCE_ENABLED)
    if(NOT SOLN_PRIVATE_HIBOOSTER_SOURCES)
        message(FATAL_ERROR "soln_utils/soln_private/sources.cmake does not define HiBooster sources")
    endif()

    sdk_generate_library(hibooster)
    sdk_library_add_sources(${SOLN_PRIVATE_HIBOOSTER_SOURCES})
    sdk_add_private_include_directories(${SOLN_PRIVATE_HIBOOSTER_INCLUDES})
else()
    set(SOLN_HIBOOSTER_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/lib/libhibooster_${CHIP}.a)
    if(NOT EXISTS ${SOLN_HIBOOSTER_LIBRARY})
        message(FATAL_ERROR "Missing HiBooster prebuilt library for CHIP=${CHIP}: ${SOLN_HIBOOSTER_LIBRARY}")
    endif()
    sdk_add_static_library(${SOLN_HIBOOSTER_LIBRARY})
endif()
