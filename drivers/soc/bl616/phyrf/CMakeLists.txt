sdk_add_include_directories(include)

sdk_add_compile_definitions(-DWL_WB03=0)
sdk_add_compile_definitions(-DWL_BL616=1)
sdk_add_compile_definitions(-DWL_BL618M=0)

sdk_add_static_library(lib-gcc_10.2.0-toolchain_V2.6.1/libbl616_phyrf.a)