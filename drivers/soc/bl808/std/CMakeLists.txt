sdk_generate_library()

sdk_library_add_sources(startup/${CPU_ID}/start.S)
sdk_library_add_sources(startup/${CPU_ID}/vector.S)
sdk_library_add_sources(startup/${CPU_ID}/riscv_fpu.S)
sdk_library_add_sources(startup/${CPU_ID}/start_load.c)
sdk_library_add_sources(startup/${CPU_ID}/system_bl808.c)
sdk_library_add_sources(startup/${CPU_ID}/interrupt.c)

sdk_library_add_sources(src/bl808_aon.c)
sdk_library_add_sources(src/bl808_clock.c)
sdk_library_add_sources(src/bl808_common.c)
sdk_library_add_sources(src/bl808_ef_cfg.c)
sdk_library_add_sources(src/bl808_glb.c)
sdk_library_add_sources(src/bl808_glb_gpio.c)
sdk_library_add_sources(src/bl808_hbn.c)
sdk_library_add_sources(src/bl808_l1c.c)
sdk_library_add_sources(src/bl808_pds.c)
sdk_library_add_sources(src/bl808_sdh.c)

sdk_library_add_sources(src/bl808_tzc_sec.c)
sdk_library_add_sources(src/bl808_psram_uhs.c)
sdk_library_add_sources(src/bl808_uhs_phy.c)

sdk_library_add_sources(port/bl808_clock.c)

sdk_add_include_directories(
include
include/hardware
)

if("${CPU_ID}" STREQUAL "m0")
SET(MCPU "e907")
SET(MARCH "rv32imafcpzpsfoperand_xtheade")
SET(MABI "ilp32f")
elseif("${CPU_ID}" STREQUAL "d0")
SET(MCPU "c906")
SET(MARCH "rv64imafdcv0p7_zfh_xtheadc")
SET(MABI "lp64d")
elseif("${CPU_ID}" STREQUAL "lp")
SET(MCPU "e902")
SET(MARCH "rv32emcxtheadse")
SET(MABI "ilp32e")
endif()

sdk_add_compile_definitions(-DARCH_RISCV -DBFLB_USE_HAL_DRIVER)
sdk_add_compile_options(-march=${MARCH} -mabi=${MABI} -mtune=${MCPU})
sdk_add_link_options(-march=${MARCH} -mabi=${MABI} -mtune=${MCPU})

sdk_add_compile_definitions(-DCONFIG_IRQ_NUM=80)