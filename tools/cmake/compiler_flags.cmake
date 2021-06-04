include(${CMAKE_SOURCE_DIR}/drivers/${CHIP}_driver/cpu_flags.cmake)

list(APPEND GLOBAL_C_FLAGS -Os -g3)
list(APPEND GLOBAL_C_FLAGS -fshort-enums -fno-common -fms-extensions -ffunction-sections -fdata-sections -fstrict-volatile-bitfields)
list(APPEND GLOBAL_C_FLAGS -Wall -Wshift-negative-value -Wchar-subscripts -Wformat -Wuninitialized -Winit-self -Wignored-qualifiers -Wunused -Wundef)
list(APPEND GLOBAL_C_FLAGS -msmall-data-limit=4)

list(APPEND GLOBAL_LD_FLAGS -Wl,--cref -Wl,--gc-sections -nostartfiles -g3)
list(APPEND GLOBAL_LD_FLAGS -fms-extensions -ffunction-sections -fdata-sections)
list(APPEND GLOBAL_LD_FLAGS -Wall -Wchar-subscripts -std=c99)

# Add common options
add_compile_options(${GLOBAL_C_FLAGS})
add_compile_options($<$<COMPILE_LANGUAGE:C>:-std=c99>)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-std=c++11>)
add_link_options(${GLOBAL_LD_FLAGS})


