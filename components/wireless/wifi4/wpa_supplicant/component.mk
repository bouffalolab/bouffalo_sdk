# supplicant make file

COMPONENT_PRIV_INCLUDEDIRS := src
COMPONENT_SRCDIRS := port src/ap src/common src/crypto src/eap_peer src/rsn_supp src/tls src/utils src/esp_supplicant src/wps
COMPONENT_ADD_INCLUDEDIRS := include port/include include/esp_supplicant src/utils

ifeq ($(CONFIG_WPA_MBEDTLS_CRYPTO), y)
    COMPONENT_OBJEXCLUDE += src/tls/asn1.o \
    src/tls/bignum.o \
    src/tls/pkcs1.o \
    src/tls/pkcs5.o \
    src/tls/pkcs8.o \
    src/tls/rsa.o \
    src/tls/tls_internal.o \
    src/tls/tlsv1_client.o \
    src/tls/tlsv1_client_read.o \
    src/tls/tlsv1_client_write.o \
    src/tls/tlsv1_common.o \
    src/tls/tlsv1_cred.o \
    src/tls/tlsv1_record.o \
    src/tls/tlsv1_server.o \
    src/tls/tlsv1_server_read.o \
    src/tls/tlsv1_server_write.o \
    src/tls/x509v3.o
else
    COMPONENT_OBJEXCLUDE := src/crypto/tls_mbedtls.o
endif
ifneq ($(CONFIG_WPA_11KV_SUPPORT), y)
    COMPONENT_OBJEXCLUDE += src/common/rrm.o \
    src/common/wnm_sta.o \
    src/common/bss.o \
    src/common/scan.o \
    src/common/ieee802_11_common.o \
    src/esp_supplicant/esp_common.o \
    src/esp_supplicant/esp_scan.o
endif

CFLAGS += -DCONFIG_DPP -DCONFIG_IEEE80211W -DBL_SUPPLICANT -DIEEE8021X_EAPOL -DEAP_PEER_METHOD -DEAP_TLS -DEAP_TTLS -DEAP_PEAP -DEAP_MSCHAPv2 -DUSE_WPA2_TASK -DCONFIG_WPS2 -DCONFIG_WPS_PIN -DUSE_WPS_TASK -DESPRESSIF_USE -DESP32_WORKAROUND -DCONFIG_ECC -DCONFIG_WNM -D__ets__ -Wno-strict-aliasing

ifdef CONFIG_ESP32_WIFI_ENABLE_WPA3_SAE
	CFLAGS += -DCONFIG_WPA3_SAE
endif
