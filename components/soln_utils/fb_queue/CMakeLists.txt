# Frame Buffer Queue core

# Public FBQ headers and application-configured solution instances remain in
# this directory and are built into the parent soln_utils library.
sdk_add_include_directories(.)
sdk_library_add_sources(soln_fbq.c)

# The core implementation is compiled from soln_private when available,
# otherwise use the chip-specific prebuilt archive.

if(SOLN_PRIVATE_SOURCE_ENABLED)
    if(NOT SOLN_PRIVATE_FB_QUEUE_SOURCES)
        message(FATAL_ERROR "soln_utils/soln_private/sources.cmake does not define FBQ sources")
    endif()

    sdk_generate_library(fb_queue)
    sdk_library_add_sources(${SOLN_PRIVATE_FB_QUEUE_SOURCES})
    sdk_add_private_include_directories(${SOLN_PRIVATE_FB_QUEUE_INCLUDES})
else()
    set(SOLN_FB_QUEUE_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/lib/libfb_queue_${CHIP}.a)
    if(NOT EXISTS ${SOLN_FB_QUEUE_LIBRARY})
        message(FATAL_ERROR "Missing FBQ prebuilt library for CHIP=${CHIP}: ${SOLN_FB_QUEUE_LIBRARY}")
    endif()
    sdk_add_static_library(${SOLN_FB_QUEUE_LIBRARY})
endif()