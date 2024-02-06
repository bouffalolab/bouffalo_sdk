sdk_generate_library()

sdk_add_compile_definitions(-DLV_KCONFIG_IGNORE)

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

sdk_library_add_sources(${sources})

sdk_add_include_directories(port)
sdk_add_include_directories(.)