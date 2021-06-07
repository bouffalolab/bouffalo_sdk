include(${CMAKE_SOURCE_DIR}/drivers/${CHIP}_driver/cpu_flags.cmake)

list(APPEND GLOBAL_C_FLAGS -Os -g3)
list(APPEND GLOBAL_C_FLAGS -fshort-enums -fno-common -fms-extensions -ffunction-sections -fdata-sections -fstrict-volatile-bitfields)
list(APPEND GLOBAL_C_FLAGS -Wall -Wshift-negative-value -Wchar-subscripts -Wformat -Wuninitialized -Winit-self -Wignored-qualifiers -Wunused -Wundef)
list(APPEND GLOBAL_C_FLAGS -msmall-data-limit=4)

list(APPEND GLOBAL_LD_FLAGS -Wl,--cref -Wl,--gc-sections -nostartfiles -g3)
list(APPEND GLOBAL_LD_FLAGS -fms-extensions -ffunction-sections -fdata-sections)
list(APPEND GLOBAL_LD_FLAGS -Wall -Wchar-subscripts -std=c99)

if(${SUPPORT_FLOAT} STREQUAL "y")
list(APPEND GLOBAL_C_FLAGS -DBFLB_PRINT_FLOAT_SUPPORT)
endif()

if(${SUPPORT_SHELL} STREQUAL "y")
list(APPEND GLOBAL_C_FLAGS -DSHELL_SUPPORT)
endif()
