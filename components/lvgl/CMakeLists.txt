sdk_generate_library()

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

sdk_library_add_sources(${sources})
sdk_add_include_directories(.)