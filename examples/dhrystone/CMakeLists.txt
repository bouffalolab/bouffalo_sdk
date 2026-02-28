cmake_minimum_required(VERSION 3.15)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

target_sources(app PRIVATE
    dhry_1.c
    dhry_2.c
)

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
     # -funroll-all-loops
 )

project(dhrystone)
