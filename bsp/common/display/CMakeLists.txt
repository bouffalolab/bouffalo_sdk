# Display framework
#
# Added by bsp/common/CMakeLists.txt when CONFIG_BSP_DISPLAY is on.
# Headers are included by their layer path relative to this directory, e.g. "bus/dbi/display_dbi.h".
# One CMakeLists.txt per layer/tool: bus/, panel/, scanout/, default/, simple_font/ (each with its Kconfig).
if(NOT CONFIG_BSP_DISPLAY)
    return()
endif()

sdk_add_include_directories(.)

# Display and the platform adaptation
target_sources(app PRIVATE
    display.c
    display_os.c)

add_subdirectory(bus)
add_subdirectory(panel)
add_subdirectory(scanout)
add_subdirectory(default)
add_subdirectory(simple_font)
