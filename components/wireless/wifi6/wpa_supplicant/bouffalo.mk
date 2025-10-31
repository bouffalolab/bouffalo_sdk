# Component Makefile
#
## These include paths would be exported to project level
COMPONENT_ADD_INCLUDEDIRS += \
    wpa_supplicant \
    src \
    src/utils \
    src/crypto \
    src/l2_packet \
    src/rsn_supp \
    src/drivers \

## not be exported to project level
COMPONENT_PRIV_INCLUDEDIRS := \

COMPONENT_SRCS := \
    wpa_supplicant/wpa_supplicant.c \
    wpa_supplicant/events.c \
    wpa_supplicant/scan.c \
    wpa_supplicant/notify.c \
    wpa_supplicant/bss.c \
    wpa_supplicant/wmm_ac.c \
    wpa_supplicant/wpas_glue.c \
    wpa_supplicant/eap_register.c \
    wpa_supplicant/config.c \
    wpa_supplicant/config_none.c \
    wpa_supplicant/ctrl_iface.c \
    wpa_supplicant/ctrl_iface_macsw.c \
    wpa_supplicant/main_macsw.c \
    wpa_supplicant/op_classes.c \
    wpa_supplicant/rrm.c \
    wpa_supplicant/bssid_ignore.c \
    wpa_supplicant/robust_av.c \
    src/utils/eloop_rtos.c \
    src/utils/common.c \
    src/utils/wpabuf.c \
    src/utils/os_macsw.c \
    src/utils/wpa_debug.c \
    src/utils/bitfield.c \
    src/crypto/rc4.c \
    src/crypto/sha1-prf.c \
    src/crypto/sha1-pbkdf2-bl.c \
    src/crypto/sha256-prf.c \
    src/crypto/sha256-kdf.c \
    src/crypto/aes-wrap.c \
    src/crypto/aes-unwrap.c \
    src/crypto/aes-omac1.c \
    src/crypto/random.c \
    src/crypto/tls_none.c \
    wpa_supplicant/sme.c \
    src/common/sae.c \
    src/common/dragonfly.c \
    src/crypto/crypto_mbedtls.c \
    src/crypto/crypto_mbedtls_misc.c \
    src/crypto/dh_groups.c \
    src/l2_packet/l2_packet_macsw.c \
    src/rsn_supp/wpa.c \
    src/rsn_supp/wpa_ie.c \
    src/rsn_supp/preauth.c \
    src/drivers/drivers.c \
    src/drivers/driver_common.c \
    src/drivers/driver_macsw.c \
    src/common/wpa_common.c \
    src/common/ieee802_11_common.c \
    src/common/hw_features_common.c \
    wpa_supplicant/ap.c \
    src/ap/hostapd.c \
    src/ap/wpa_auth_glue.c \
    src/ap/utils.c \
    src/ap/authsrv.c \
    src/ap/ap_config.c \
    src/utils/ip_addr.c \
    src/ap/sta_info.c \
    src/ap/tkip_countermeasures.c \
    src/ap/ap_mlme.c \
    src/ap/ieee802_1x.c \
    src/eapol_auth/eapol_auth_sm.c \
    src/ap/ieee802_11_auth.c \
    src/ap/ieee802_11_shared.c \
    src/ap/drv_callbacks.c \
    src/ap/ap_drv_ops.c \
    src/ap/beacon.c \
    src/ap/bss_load.c \
    src/ap/eap_user_db.c \
    src/ap/neighbor_db.c \
    src/ap/rrm.c \
    src/utils/crc32.c \
    src/ap/wmm.c \
    src/ap/ap_list.c \
    src/ap/ieee802_11.c \
    src/ap/hw_features.c \
    src/ap/dfs.c \
    src/ap/wpa_auth.c \
    src/ap/wpa_auth_ie.c \
    src/ap/pmksa_cache_auth.c \
    src/ap/ieee802_11_ht.c \
    src/ap/ieee802_11_vht.c \
    src/ap/ieee802_11_he.c \
    src/ap/wnm_ap.c \
    src/eap_server/eap_server.c \
    src/eap_server/eap_server_identity.c \
    src/eap_server/eap_server_methods.c \
    src/eap_common/eap_common.c \
    src/ap/ctrl_iface_ap.c \

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := \
    . \
    wpa_supplicant \
    src \
    src/ap \
    src/common \
    src/eapol_auth \
    src/eapol_supp \
    src/utils \
    src/crypto \
    src/l2_packet \
    src/rsn_supp \
    src/drivers \
    src/eap_server \
    src/eap_common \
    src/eap_peer \


##
#CPPFLAGS +=
#CFLAGS += -DCONFIG_BLE_AT_CMD

CPPFLAGS += -D USE_UTILS_CRC
CFLAGS += -msave-restore
