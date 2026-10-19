sdk_generate_library()

sdk_add_private_compile_options(-Wno-unused-function)
sdk_add_private_compile_options(-Wno-unused-variable)

# !! MANDATORY REQUIREMENT !!
sdk_add_private_compile_options(-Werror)
# !! MANDATORY REQUIREMENT !!

## These include paths would be exported to project level
sdk_add_include_directories(include)
sdk_add_include_directories(include/bl_supplicant)

sdk_add_private_include_directories(include)
sdk_add_private_include_directories(include/bl_supplicant)
sdk_add_private_include_directories(port/include)
sdk_add_private_include_directories(src)

sdk_library_add_sources(port/os_bl.c)
sdk_library_add_sources(src/ap/ap_config.c)
sdk_library_add_sources(src/ap/wpa_auth_ie.c)
sdk_library_add_sources(src/ap/wpa_auth_rsn_ccmp_only.c)
sdk_library_add_sources(src/bl_supplicant/bl_hostap.c)
sdk_library_add_sources(src/bl_supplicant/bl_wpa3.c)
sdk_library_add_sources(src/bl_supplicant/bl_wpa_main.c)
sdk_library_add_sources(src/bl_supplicant/bl_wpas_glue.c)
#sdk_library_add_sources(src/bl_supplicant/bl_wps.c)
sdk_library_add_sources(src/common/sae.c)
sdk_library_add_sources(src/common/wpa_common.c)
#sdk_library_add_sources(src/crypto/aes-cbc.c)
#sdk_library_add_sources(src/crypto/aes-internal-bl.c)
sdk_library_add_sources(src/crypto/aes-omac1.c)
sdk_library_add_sources(src/crypto/aes-unwrap.c)
sdk_library_add_sources(src/crypto/aes-wrap.c)
#sdk_library_add_sources(src/crypto/crypto_internal-modexp.c)
sdk_library_add_sources(src/crypto/dh_group5.c)
sdk_library_add_sources(src/crypto/dh_groups.c)
#sdk_library_add_sources(src/crypto/md5-internal.c)
#sdk_library_add_sources(src/crypto/md5.c)
sdk_library_add_sources(src/crypto/rc4.c)
#sdk_library_add_sources(src/crypto/sha1-internal.c)
sdk_library_add_sources(src/crypto/sha1-pbkdf2-bl.c)
#sdk_library_add_sources(src/crypto/sha1.c)
#sdk_library_add_sources(src/crypto/sha256-internal.c)
sdk_library_add_sources(src/crypto/sha256-prf.c)
#sdk_library_add_sources(src/crypto/sha256.c)
sdk_library_add_sources(src/crypto/crypto_mbedtls-ec.c)
sdk_library_add_sources(src/crypto/crypto_mbedtls-bignum.c)
sdk_library_add_sources(src/crypto/crypto_mbedtls_misc.c)
#sdk_library_add_sources(src/eap_peer/eap_common.c)
#sdk_library_add_sources(src/rsn_supp/pmksa_cache.c)
sdk_library_add_sources(src/rsn_supp/wpa.c)
sdk_library_add_sources(src/rsn_supp/wpa_ie.c)
sdk_library_add_sources(src/utils/common.c)
sdk_library_add_sources(src/utils/wpa_debug.c)
sdk_library_add_sources(src/utils/wpabuf.c)
#sdk_library_add_sources(src/wps/wps.c)
#sdk_library_add_sources(src/wps/wps_attr_build.c)
#sdk_library_add_sources(src/wps/wps_attr_parse.c)
#sdk_library_add_sources(src/wps/wps_attr_process.c)
#sdk_library_add_sources(src/wps/wps_common.c)
#sdk_library_add_sources(src/wps/wps_dev_attr.c)
#sdk_library_add_sources(src/wps/wps_enrollee.c)
#sdk_library_add_sources(src/wps/wps_registrar.c)
#sdk_library_add_sources(src/wps/wps_validate.c)
#sdk_library_add_sources(test/test_crypto-bl.c)

sdk_add_compile_definitions(-DCONFIG_NEWLIB_NANO_FORMAT=0)
sdk_add_compile_definitions(-DCONFIG_WPA_WAPI_PSK=0)
sdk_add_compile_definitions(-DPSK_SUPPLICANT=1)
sdk_add_compile_definitions(-DWPA3_SUPPORTED=1)
sdk_add_compile_definitions(-DCONFIG_ECC)
sdk_add_compile_definitions(-DUSE_MBEDTLS_CRYPTO)

if(CONFIG_BL_SUPPLICANT_WPS OR CONFIG_BL_SUPPLICANT_P2P)
    sdk_library_add_sources(src/bl_supplicant/bl_wps.c)
    sdk_library_add_sources(src/bl_supplicant/bl_wps_ap.c)
    sdk_library_add_sources(src/eap_peer/eap_common.c)
    sdk_library_add_sources(src/rsn_supp/pmksa_cache.c)
    sdk_library_add_sources(src/utils/base64.c)
    sdk_library_add_sources(src/utils/uuid.c)
    sdk_library_add_sources(src/wps/wps.c)
    sdk_library_add_sources(src/wps/wps_attr_build.c)
    sdk_library_add_sources(src/wps/wps_attr_parse.c)
    sdk_library_add_sources(src/wps/wps_attr_process.c)
    sdk_library_add_sources(src/wps/wps_common.c)
    sdk_library_add_sources(src/wps/wps_dev_attr.c)
    sdk_library_add_sources(src/wps/wps_enrollee.c)
    sdk_library_add_sources(src/wps/wps_registrar.c)
    sdk_library_add_sources(src/wps/wps_validate.c)
endif()

if(CONFIG_BL_SUPPLICANT_P2P)
    set(bl_p2p_sources
        src/p2p/p2p.c
        src/p2p/p2p_build.c
        src/p2p/p2p_dev_disc.c
        src/p2p/p2p_go_neg.c
        src/p2p/p2p_group.c
        src/p2p/p2p_invitation.c
        src/p2p/p2p_parse.c
        src/p2p/p2p_pd.c
        src/p2p/p2p_sd.c
        src/p2p/p2p_utils.c
        src/common/gas.c
    )

    set_source_files_properties(${bl_p2p_sources} PROPERTIES
        COMPILE_OPTIONS "-iquote${CMAKE_CURRENT_SOURCE_DIR}/src/p2p_compat;-iquote${CMAKE_CURRENT_SOURCE_DIR}/src/utils"
    )

    sdk_add_private_include_directories(src/p2p)
    sdk_add_private_include_directories(src/wpa_crypto)
    sdk_library_add_sources(src/bl_supplicant/bl_p2p_glue.c)
    sdk_library_add_sources(src/bl_supplicant/bl_p2p_runtime.c)
    sdk_library_add_sources(src/p2p_compat/ieee802_11_common_compat.c)
    sdk_library_add_sources(${bl_p2p_sources})
    sdk_add_compile_definitions(-DCONFIG_P2P)
endif()
