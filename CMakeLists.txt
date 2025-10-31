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

# handle __FILE__ macro issue
set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
sdk_add_compile_options("-fmacro-prefix-map=${SOURCE_PATH}=.")
sdk_add_compile_options("-fdebug-prefix-map=${SOURCE_PATH}=.")

if(("${CHIP}" STREQUAL "bl616d") AND ("${CPU_MODEL}" STREQUAL "a0"))
    sdk_add_compile_definitions(-DCPU_MODEL_A0)
endif()

sdk_add_compile_definitions(-DBL616D_VERSION_A0)
sdk_add_compile_definitions(-DBL616L_VERSION_A0)

add_subdirectory(bsp)
add_subdirectory(components)
add_subdirectory(drivers/lhal)
add_subdirectory(drivers/soc/${CHIP}/std)

set(SYS_DRV_EXCLUDED_CHIPS bl602 bl628 bl606p bl808)
if(NOT "${CHIP}" IN_LIST SYS_DRV_EXCLUDED_CHIPS)
    add_subdirectory(drivers/sys)
endif()

set(RFPARAM_SUPPORT_CHIPS bl616 bl616d bl616l bl602)
if("${CHIP}" IN_LIST RFPARAM_SUPPORT_CHIPS)
    sdk_add_subdirectory_ifdef(CONFIG_RF drivers/rfparam)
endif()

sdk_add_subdirectory_ifdef(CONFIG_RF drivers/soc/${CHIP}/phyrf)
sdk_add_subdirectory_ifdef(CONFIG_PHY_LPFW drivers/soc/${CHIP}/phyrf)
sdk_add_include_directories(.)
