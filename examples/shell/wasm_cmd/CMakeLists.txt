cmake_minimum_required(VERSION 3.15)

# The wifi6 submodule's lwipopts.h names these values MACSW_TXDESC_CNT*, while
# the selected macsw_default_config.h exposes them as CFG_TXDESC*.  Keep the
# compatibility mapping in the example so the vendor submodule stays untouched.
add_compile_definitions(
    MACSW_TXDESC_CNT0=CFG_TXDESC0
    MACSW_TXDESC_CNT1=CFG_TXDESC1
    MACSW_TXDESC_CNT2=CFG_TXDESC2
    MACSW_TXDESC_CNT3=CFG_TXDESC3
    MACSW_AMPDU_RX_BUF_SIZE=CFG_REORD_BUF
    MACSW_MAX_BA_RX=CFG_BARX
)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_include_directories(.)
sdk_add_include_directories(${CMAKE_CURRENT_LIST_DIR}/app)
sdk_set_main_file(main.c)

set(COREMARK_DIR ${CMAKE_CURRENT_LIST_DIR}/../../coremark_v1.01)
set(COREMARK_SOURCES
    ${COREMARK_DIR}/core_list_join.c
    ${COREMARK_DIR}/core_matrix.c
    ${COREMARK_DIR}/core_state.c
    ${COREMARK_DIR}/core_util.c
)

if(CONFIG_WAMR_INTERP OR CONFIG_WAMR_AOT)
    target_sources(app PRIVATE
        app/wasm_cmd.c
        app/coremark_bench.c
        ${COREMARK_SOURCES}
    )
    if(CONFIG_WL80211)
        target_sources(app PRIVATE app/wl80211_connect.c)
    endif()
    target_include_directories(app PRIVATE ${COREMARK_DIR})
    set_source_files_properties(
        app/coremark_bench.c
        ${COREMARK_SOURCES}
        PROPERTIES COMPILE_OPTIONS
        "-O2;-include;${CMAKE_CURRENT_LIST_DIR}/app/coremark_portable.h"
    )
    if(CONFIG_WAMR_INTERP)
        target_compile_definitions(app PRIVATE CONFIG_WAMR_INTERP=1)
    endif()
    if(CONFIG_WAMR_AOT)
        target_sources(app PRIVATE app/aot_store.c app/aot_xz.c)
        target_compile_definitions(app PRIVATE CONFIG_WAMR_AOT=1)
    endif()
endif()

project(wasm_cmd)
