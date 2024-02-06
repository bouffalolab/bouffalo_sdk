configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/sdk_version.h.in"
  "${CMAKE_BINARY_DIR}/generated/sdk_version.h"
)
target_include_directories(app PRIVATE "${CMAKE_BINARY_DIR}/generated")
target_sources(app PRIVATE sdk_version.c)

add_subdirectory(board)
add_subdirectory(common)
