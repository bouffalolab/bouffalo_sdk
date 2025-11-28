cmake_minimum_required(VERSION 3.15)

if(DEFINED PROJECT_SDK_VERSION)
    sdk_generate_library(wl80211_plat)
    sdk_add_private_compile_options(-Wno-unused-function)
    sdk_add_private_compile_options(-Wno-unused-variable)
    sdk_add_private_compile_options(-Wstrict-prototypes)

    # !! MANDATORY REQUIREMENT !!
    sdk_add_private_compile_options(-Werror)
    # !! MANDATORY REQUIREMENT !!

    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)
        # If the src directory exists, the header files
        # and library files are entirely from src.
        sdk_add_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/include)
        sdk_add_private_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/macsw)

        sdk_add_static_library(${SDK_DEMO_PATH}/${BUILD_DIR}/build_wl80211/src/libwl80211_${CHIP}.a)
    else ()
        sdk_add_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
        sdk_add_private_include_directories(${CMAKE_CURRENT_SOURCE_DIR}/macsw)

        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lib/libwl80211_${CHIP}.a)
            sdk_add_static_library(lib/libwl80211_${CHIP}.a)
        else()
            message(FATAL_ERROR "what happened?")
        endif()
    endif()

    sdk_library_add_sources(wifi_mgmr.c)
    sdk_library_add_sources(invariants.c)
    sdk_library_add_sources(rtos_al.c)
    sdk_library_add_sources(lwip.c)
else()
    message(STATUS "WL80211 Standalone build ${BOARD}")
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src)
        project(wl80211)
        add_subdirectory(src)
    endif()
endif()
