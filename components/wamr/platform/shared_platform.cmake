set(PLATFORM_SHARED_DIR ${CMAKE_CURRENT_LIST_DIR})
include_directories(${PLATFORM_SHARED_DIR} ${SHARED_DIR}/platform/include)
add_definitions(-DBH_PLATFORM_BOUFFALO)
set(PLATFORM_SHARED_SOURCE ${PLATFORM_SHARED_DIR}/bouffalo_platform.c)
