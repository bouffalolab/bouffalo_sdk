if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${BOARD}/CMakeLists.txt")
    message(STATUS "Select board: ${CMAKE_CURRENT_SOURCE_DIR}/${BOARD}")
    add_subdirectory("${BOARD}")
elseif(EXISTS "${SDK_DEMO_PATH}/${BOARD}/CMakeLists.txt")
    message(STATUS "Select board: ${SDK_DEMO_PATH}/${BOARD}")
    add_subdirectory("${SDK_DEMO_PATH}/${BOARD}" "${BOARD}")
else()
    message(FATAL_ERROR
        "BOARD '${BOARD}' was not found in '${CMAKE_CURRENT_SOURCE_DIR}/${BOARD}' or "
        "'${SDK_DEMO_PATH}/${BOARD}'")
endif()
