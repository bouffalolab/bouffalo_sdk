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
    execute_process(
        COMMAND git describe --abbrev=8 --tags --dirty --always --match "sdk-*"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_TAG
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    if(GIT_TAG)
        set(PROJECT_SDK_VERSION ${GIT_TAG})
    else()
        message(WARNING "No Valid version info found for SDK!")
        set(PROJECT_SDK_VERSION "version-unknown-panic")
    endif()
endif()
message(STATUS "Project SDK Version: ${PROJECT_SDK_VERSION}")

# handle __FILE__ macro issue
set(SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
sdk_add_compile_options("-fmacro-prefix-map=${SOURCE_PATH}=.")
sdk_add_compile_options("-fdebug-prefix-map=${SOURCE_PATH}=.")

if(CONFIG_ANTI_ROLLBACK)
    sdk_add_compile_definitions(-DCONFIG_ANTI_ROLLBACK)
endif()
sdk_add_compile_definitions(-DCONFIG_APP_ANTI_ROLLBACK_VER=${CONFIG_APP_ANTI_ROLLBACK_VER})

add_subdirectory(bsp)
add_subdirectory(components)
add_subdirectory(drivers/lhal)
add_subdirectory(drivers/soc/${CHIP}/std)
if("${CHIP}" STREQUAL "bl702")
add_subdirectory(drivers/sys)
endif()
if("${CHIP}" STREQUAL "bl702l")
add_subdirectory(drivers/sys)
endif()
if("${CHIP}" STREQUAL "bl616")
add_subdirectory(drivers/sys)
endif()
sdk_add_subdirectory_ifdef(CONFIG_RF drivers/soc/${CHIP}/phyrf)
if("${CHIP}" STREQUAL "bl616")
sdk_add_subdirectory_ifdef(CONFIG_RF drivers/rfparam)
sdk_add_subdirectory_ifdef(CONFIG_PHY_LPFW drivers/soc/bl616/phyrf)
endif()
