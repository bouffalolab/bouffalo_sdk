cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

target_sources(app PRIVATE core_list_join.c
               core_main.c
               core_matrix.c
               core_portme.c
               core_state.c
               core_util.c)

sdk_set_main_file(main.c)

sdk_add_compile_definitions(
    -DITERATIONS=0
    -DCORE_DEBUG=0
    -DCOMPILER_REQUIRES_SORT_RETURN=0
    -DVALIDATION_RUN=0
    -DPROFILE_RUN=0
    -DPERFORMANCE_RUN=1
)

sdk_add_compile_options(
    -O3
    -falign-functions=2
    -fno-code-hoisting
    -finline-limit=500
    -funroll-all-loops
)

project(coremark)
