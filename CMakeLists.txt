cmake_minimum_required(VERSION 3.15)

sdk_add_compile_definitions(-D CONFIG_BOUFFALO_SDK)

# check VERSION file
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")
    # read info from VERSION file
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" VERSION_FILE_CONTENT)

    string(REGEX MATCH "PROJECT_SDK_VERSION[ ]+\"([^\"]+)\"" _ ${VERSION_FILE_CONTENT})
    set(PROJECT_SDK_VERSION "${CMAKE_MATCH_1}")
else()
    # get git tag
    # execute_process(
    #     COMMAND git describe --abbrev=8 --tags --dirty --always --match "sdk-*"
    #     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #     OUTPUT_VARIABLE GIT_TAG
    #     OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET
    # )

    # if(GIT_TAG)
    #     set(PROJECT_SDK_VERSION ${GIT_TAG})
    # else()
    #     message(WARNING "No Valid version info found for SDK!")
    #     set(PROJECT_SDK_VERSION "version-unknown-panic")
    # endif()
    set(PROJECT_SDK_VERSION "2.0.0")
endif()
message(STATUS "Project SDK Version: ${PROJECT_SDK_VERSION}")

find_package(Git QUIET)
set(BFLB_GIT_DIRTY 1)
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short=8 HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE BFLB_GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} diff-index --quiet HEAD --
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE BFLB_GIT_DIRTY
    )
endif()

if(BFLB_GIT_HASH)
    if(BFLB_GIT_DIRTY EQUAL 0)
        set(BFLB_GIT_SUFFIX " ${BFLB_GIT_HASH}")
    else()
        set(BFLB_GIT_SUFFIX " ${BFLB_GIT_HASH}+")
    endif()
else()
    set(BFLB_GIT_SUFFIX "")
endif()
sdk_add_compile_definitions(-DBFLB_GIT_SUFFIX="${BFLB_GIT_SUFFIX}")

# handle __FILE__ macro issue
set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
sdk_add_compile_options("-fmacro-prefix-map=${SOURCE_PATH}=.")
sdk_add_compile_options("-fdebug-prefix-map=${SOURCE_PATH}=.")

if(("${CHIP}" STREQUAL "bl618dg") AND ("${CPU_MODEL}" STREQUAL "a0"))
    sdk_add_compile_definitions(-DCPU_MODEL_A0)
endif()

sdk_add_compile_definitions(-DBL618DG_VERSION_A0)
sdk_add_compile_definitions(-DBL616CL_VERSION_A0)

add_subdirectory(bsp)
add_subdirectory(components)
add_subdirectory(drivers/lhal)
add_subdirectory(drivers/soc/${CHIP}/std)

set(SYS_DRV_EXCLUDED_CHIPS)
if(NOT "${CHIP}" IN_LIST SYS_DRV_EXCLUDED_CHIPS)
    add_subdirectory(drivers/sys)
endif()

set(RFPARAM_SUPPORT_CHIPS bl616 bl618dg bl616cl bl602)
if("${CHIP}" IN_LIST RFPARAM_SUPPORT_CHIPS)
    sdk_add_subdirectory_ifdef(CONFIG_RF drivers/rfparam)
endif()

sdk_add_subdirectory_ifdef(CONFIG_RF drivers/soc/${CHIP}/phyrf)
sdk_add_subdirectory_ifdef(CONFIG_PHY_LPFW drivers/soc/${CHIP}/phyrf)
sdk_add_include_directories(.)
