sdk_generate_library()

## These include paths would be exported to project level
sdk_add_include_directories(. bflb_os_adapter/include
                            bflb_os_adapter/include/bflb_os_adapter
                            bflb_os_adapter
                            tlv
                            blcrypto_sec_pka)

sdk_library_add_sources(bflb_os_adapter/bflb_os_hal.c
                        plat_bouffalo_sdk.c
                        cli_al.c
                        net_iperf_al.c
                        rtos_al.c
                        tlv/utils_tlv_bl.c
                        blcrypto_sec_pka/blcrypto_sec_pka.c)


sdk_add_compile_definitions(-DCFG_FREERTOS -DBOUFFALO_SDK)

