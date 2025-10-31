string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$" MATCHED_VERSION "${PROJECT_SDK_VERSION}")

if(MATCHED_VERSION)
    string(REGEX MATCHALL "[0-9]+" VERSION_LIST "${MATCHED_VERSION}")
    list(GET VERSION_LIST 0 X)
    list(GET VERSION_LIST 1 Y)
    list(GET VERSION_LIST 2 Z)

    if(X LESS 255 AND Y LESS 255 AND Z LESS 255)
        sdk_add_compile_definitions(-DAPP_VER_X=${X})
        sdk_add_compile_definitions(-DAPP_VER_Y=${Y})
        sdk_add_compile_definitions(-DAPP_VER_Z=${Z})
    else()
        message(FATAL_ERROR "Invalid VERSION: number not less than 255")
    endif()
else()
    message(FATAL_ERROR "Invalid VERSION: Must be x.y.z")
endif()

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/sdk_version.h.in"
  "${CMAKE_BINARY_DIR}/generated/sdk_version.h"
)
target_include_directories(app PRIVATE "${CMAKE_BINARY_DIR}/generated")
target_sources(app PRIVATE sdk_version.c)

add_subdirectory(board)
add_subdirectory(common)
