/*
 * wpa_supplicant/hostapd - Build time configuration defines
 * Copyright (c) 2005-2006, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 *
 * This header file can be used to define configuration defines that were
 * originally defined in Makefile. This is mainly meant for IDE use or for
 * systems that do not have suitable 'make' tool. In these cases, it may be
 * easier to have a single place for defining all the needed C pre-processor
 * defines.
 */

#ifndef BUILD_CONFIG_H
#define BUILD_CONFIG_H

/* Insert configuration defines, e.g., #define EAP_MD5, here, if needed. */

#define CONFIG_PMK_CACHE_IN_MGMR
#define CONFIG_EXTRA_WPA_SSID_FLAG

#define CONFIG_ONLY_STA_AP_MODE
//#define CONFIG_NO_HIGHER_40M_OR_NON_24G
//#define CONFIG_SAE_BASIC_ONLY
#define CONFIG_EXTRA_SIZE_OPT
#define CONFIG_RANDOM_OPT
#define CONFIG_EAP_SERVER
#define CONFIG_WNM
#define CONFIG_MBO
#define CONFIG_RRM
/* #define CONFIG_DFS */

#define IEEE8021X_EAPOL

#ifdef CONFIG_WIN32_DEFAULTS
#define CONFIG_NATIVE_WINDOWS
#define CONFIG_ANSI_C_EXTRA
#define CONFIG_WINPCAP
#define IEEE8021X_EAPOL
#define PKCS12_FUNCS
#define PCSC_FUNCS
#define CONFIG_CTRL_IFACE
#define CONFIG_CTRL_IFACE_NAMED_PIPE
#define CONFIG_DRIVER_NDIS
#define CONFIG_NDIS_EVENTS_INTEGRATED
#define CONFIG_DEBUG_FILE
#define EAP_MD5
#define EAP_TLS
#define EAP_MSCHAPv2
#define EAP_PEAP
#define EAP_TTLS
#define EAP_GTC
#define EAP_OTP
#define EAP_LEAP
#define EAP_TNC
#define _CRT_SECURE_NO_DEPRECATE
#define CONFIG_USE_MBEDTLS

#ifdef USE_INTERNAL_CRYPTO
#define CONFIG_TLS_INTERNAL_CLIENT
#define CONFIG_INTERNAL_LIBTOMMATH
#define CONFIG_CRYPTO_INTERNAL
#endif /* USE_INTERNAL_CRYPTO */
#endif /* CONFIG_WIN32_DEFAULTS */

#include "macsw.h"

#define CONFIG_MACSW
#ifndef CONFIG_LWIP
#define CONFIG_LWIP
#endif
#define CONFIG_DRIVER_MACSW
#define OS_NO_C_LIB_DEFINES
#if !MACSW_DEBUG
#define CONFIG_NO_WPA_MSG
#endif
#define CONFIG_NO_CONFIG_WRITE
#define CONFIG_NO_HOSTAPD_LOGGER
#define CONFIG_CTRL_IFACE
#define CONFIG_CTRL_IFACE_UDP

#define CONFIG_TLS_INTERNAL_CLIENT
#define CONFIG_INTERNAL_LIBTOMMATH
#define CONFIG_CRYPTO_INTERNAL

#define CONFIG_WEP

// for now only used for SAE
#ifdef CONFIG_USE_MBEDTLS
#define CONFIG_SME
#define CONFIG_SAE
#define CONFIG_ECC
#endif

// AP support
#define CONFIG_AP
#define NEED_AP_MLME
#define CONFIG_NO_RADIUS
#define CONFIG_NO_ACCOUNTING
#define CONFIG_NO_VLAN
#define EAP_SERVER
#define EAP_SERVER_IDENTITY
#define CONFIG_WNM_AP
#define CONFIG_WPS
#define IEEE8021X_EAPOL
#define EAP_WSC
#if MACSW_P2P
#define CONFIG_P2P
#define CONFIG_WPS
#define CONFIG_GAS
#define CONFIG_IEEE8021X_EAPOL
#define CONFIG_SHA256
#define LTM_FAST
#define EAP_WSC
#define EAP_SERVER_WSC
#define CONFIG_OFFCHANNEL
#define IEEE8021X_EAPOL
#endif // MACSW_P2P
#if MACSW_VHT
#define CONFIG_IEEE80211AC
#endif
#if MACSW_HE
#define CONFIG_IEEE80211AX
#endif
#if MACSW_MESH_EN
#define CONFIG_MESH
#define CONFIG_SHA256
#endif // MACSW_MESH_EN

// Must undef, some msg needed by fhost depend on wpa_supplicant_ctrl_iface_msg_cb
#undef CONFIG_NO_WPA_MSG
#endif /* BUILD_CONFIG_H */
