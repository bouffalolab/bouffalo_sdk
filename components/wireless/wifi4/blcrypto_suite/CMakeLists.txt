sdk_generate_library()

## These include paths would be exported to project level
sdk_add_include_directories(inc)

sdk_add_private_include_directories(priv_inc)

if(("${CHIP}" STREQUAL "bl602"))
    sdk_add_compile_definitions(-DCFG_CHIP_BL602)
endif()

## This component's src
sdk_library_add_sources(src/blcrypto_suite_bignum.c             )
sdk_library_add_sources(src/blcrypto_suite_ecp.c                )
sdk_library_add_sources(src/blcrypto_suite_ecp_curves.c         )
sdk_library_add_sources(src/blcrypto_suite_platform_util.c      )
sdk_library_add_sources(src/blcrypto_suite_porting.c            )
sdk_library_add_sources(src/blcrypto_suite_hacc.c               )
sdk_library_add_sources(src/blcrypto_suite_aes.c                )
sdk_library_add_sources(src/blcrypto_suite_hacc_glue.c          )
sdk_library_add_sources(src/blcrypto_suite_hacc_secp256r1_mul.c )
sdk_library_add_sources(src/blcrypto_suite_supplicant_api.c     )
sdk_library_add_sources(src/blcrypto_suite_export_fw.c          )

