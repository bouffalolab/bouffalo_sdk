sdk_add_include_directories(inc)

sdk_add_static_library(lib/libphyrf.a)

sdk_add_compile_definitions(-DWL_WB03=0)
sdk_add_compile_definitions(-DWL_BL616=1)
sdk_add_compile_definitions(-DWL_BL618M=0)