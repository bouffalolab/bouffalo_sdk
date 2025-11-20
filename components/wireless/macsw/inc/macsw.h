/*
 * Bouffalolab MACSW Library
 */
#ifndef __MACSW_H__
#define __MACSW_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/queue.h>

#include "macsw_config.h"

#define MACSW_VERSION_STR      "v6.10.0.0"
// Version has the form Major.minor.release.patch
// The version string is "vMM.mm.rr.pp"
// Each number should fit in a 8 bits unsigned (0 to 255)
/// Major version number
#define MACSW_VERSION_MAJ      6
/// Minor version number
#define MACSW_VERSION_MIN      10
/// Release number
#define MACSW_VERSION_REL      0
/// Patch number
#define MACSW_VERSION_PAT      0

/// Version word
#define MACSW_VERSION        ((MACSW_VERSION_MAJ << 24) | (MACSW_VERSION_MIN << 16) |             \
                           (MACSW_VERSION_REL << 8) | MACSW_VERSION_PAT)

#define VERSION_MACSW_NUMBER "1.7.10"
#define VERSION_MACSW_MAJOR 1
#define VERSION_MACSW_MINOR 7
#define VERSION_MACSW_PATCH 10

/// define the force inlining attribute for this compiler
#ifndef __INLINE
#define __INLINE static __attribute__((__always_inline__)) inline
#endif

/// define the IRQ handler attribute for this compiler
#ifndef __IRQ
#define __IRQ __attribute__((interrupt))
#endif

/// function has no side effect and return depends only on arguments
#ifndef __PURE
#define __PURE __attribute__((const))
#endif

/// Align instantiated lvalue or struct member on 4 bytes
#ifndef __ALIGN4
#define __ALIGN4 __attribute__((aligned(4)))
#endif

/// Pack a structure field
#ifndef __PACKED16
#define __PACKED16 __attribute__ ((__packed__))
#endif
/// Pack a structure field
#ifndef __PACKED
#define __PACKED __attribute__ ((__packed__))
#endif
/// Weak a symbol
#ifndef __WEAK
#define __WEAK __attribute__((weak))
#endif

/// __MODULE__ comes from the RVDS compiler that supports it
#ifndef __MODULE__
#define __MODULE__ __func__
#endif

/// define a variable as maybe unused, to avoid compiler warnings on it
#ifndef __MAYBE_UNUSED
#define __MAYBE_UNUSED __attribute__((unused))
#endif

#ifndef __FALLTHROUGH
#define __FALLTHROUGH __attribute__((fallthrough))
#endif

// Mapping of these different elements is already handled in the map.txt file, so no need
// to define anything here
/// SHARED RAM for IPC structure
#ifndef __SHAREDRAMIPC
#define __SHAREDRAMIPC __attribute__ ((section("SHAREDRAMIPC")))
#endif
/// SHARED RAM
#ifndef __SHAREDRAM
#define __SHAREDRAM __attribute__ ((section("SHAREDRAM")))
#endif
/// LA RAM
#ifndef __LARAMMAC
#define __LARAMMAC __attribute__ ((section("LARAM")))
#endif
/// MIB memory
#ifndef __MIB
#define __MIB __attribute__ ((section("MACHWMIB")))
#endif

#define _ZERO_WITH_COMMA_1 0,
#define _second_arg(__ignored, val, ...) val
#define _is_enabled(x)                __is_enabled(x)
#define __is_enabled(val)             ___is_enabled(_ZERO_WITH_COMMA_##val)
#define ___is_enabled(junk_or_comma)  _second_arg(junk_or_comma 1, 0)
#define IS_ENABLED(config)            _is_enabled(config)

#define MACSW_HOOK(x, ...) do { \
  extern int __WEAK macsw_hook_##x (const char *f, ...); \
  if( &macsw_hook_##x ) {macsw_hook_##x(__func__, ##__VA_ARGS__);} \
} while(0)

////////////////////////////////////////////////////
/// Build for fully hosted partitioning
#ifdef CFG_FHOST
  #define MACSW_FULLY_HOSTED  1
  // No A-MSDU TX in fully hosted
  #undef CFG_AMSDU
  // A-MSDU de-aggregation enabled
  #define CFG_DEAGG
#else  // !CFG_FHOST
  #define MACSW_FULLY_HOSTED  0
#endif // CFG_FHOST

// Features that must be enabled for MESH support
#ifdef CFG_MESH
  #undef CFG_BCN
  #define CFG_BCN
  #undef CFG_MFP
  #define CFG_MFP
  #undef CFG_PS
  #define CFG_PS
#endif //defined CFG_MESH

/// Whether UMAC is enabled or not
#ifdef CFG_UMAC
  #define MACSW_UMAC_PRESENT 1
#else
  /// Whether UMAC is enable or not
  #define MACSW_UMAC_PRESENT 0
#endif

/**
 *******************************************************************************
 * @name MAC HW version
 *******************************************************************************
 */
/// MAC version
#ifdef CFG_MAC_VER_V10
  #define MACSW_MAC_VER 10
#elif defined CFG_MAC_VER_V20
  #define MACSW_MAC_VER 20
#elif defined CFG_MAC_VER_V21
  #define MACSW_MAC_VER 21
#endif
#if (MACSW_MAC_VER == 20)
#error "MAC version 20 is deprecated"
#endif
/// Flag indicating if the FW is compiled for a MAC HW supporting HE
#define MACSW_MAC_HE (MACSW_MAC_VER >= 20)
/// Flag indicating if the FW is compiled for a MAC HW implementing a RX ring buffer
#define MACSW_RX_RING (MACSW_MAC_VER >= 21)

/** @} MAC */

/**
 *******************************************************************************
 * @name Modem version
 *******************************************************************************
 */
/// Modem version
#ifdef CFG_MDM_VER_V10
  #define MACSW_MDM_VER 10
#elif defined CFG_MDM_VER_V11
  #define MACSW_MDM_VER 11
#elif defined CFG_MDM_VER_V20
  #define MACSW_MDM_VER 20
#elif defined CFG_MDM_VER_V21
  #define MACSW_MDM_VER 21
#elif defined CFG_MDM_VER_V22
  #define MACSW_MDM_VER 22
#elif defined CFG_MDM_VER_V30
  #define MACSW_MDM_VER 30
#elif defined CFG_MDM_VER_V31
  #define MACSW_MDM_VER 31
#elif defined CFG_MDM_VER_V32
  #define MACSW_MDM_VER 32
#endif
/** @} Modem */

/**
 *******************************************************************************
 * @name Platform version
 *******************************************************************************
 */
/// Platform version
#ifdef CFG_PLF_VER_V10
  #define MACSW_PLF_VER 10
#elif defined CFG_PLF_VER_V20
  #define MACSW_PLF_VER 20
#elif defined CFG_PLF_VER_V30
  #define MACSW_PLF_VER 30
#endif
/** @} Platform */

/**
 *******************************************************************************
 *   @name General Configuration
 *******************************************************************************
 */
/// Max number of virtual interfaces managed. MAC HW is limited to 6, but the LMAC assumes
/// that this number is a power of 2, thus only 1, 2 or 4 are allowed
#define MACSW_VIRT_DEV_MAX CFG_VIF_MAX
#if (MACSW_VIRT_DEV_MAX & (MACSW_VIRT_DEV_MAX - 1))
  #error "Not a power of 2"
#endif
#if (MACSW_VIRT_DEV_MAX > 4)
  #error "Max number of VIF shall be 4"
#endif

/// Max number of peer devices managed
#define MACSW_REMOTE_STA_MAX CFG_STA_MAX

/// Heap size
#define MACSW_HEAP_SIZE (2048 + 2048 * MACSW_UMAC_PRESENT + 256 * MACSW_VIRT_DEV_MAX + 64 * MACSW_REMOTE_STA_MAX)
/** @} General */

/**
 *******************************************************************************
 *   @name Beacon Configuration
 *******************************************************************************
 */
/// Beaconing support
#ifdef CFG_BCN
  #define MACSW_BEACONING 1
#ifndef CFG_BCN_AC_MAX
  // XXX:default use VI to send beacon for AP mode
#ifdef CONFIG_WIFI_GATEWAY
  #define MACSW_BEACON_AC_VI 0
  #define MACSW_AC_BCN_USED  1
#else
  #define MACSW_BEACON_AC_VI 1
  #define MACSW_AC_BCN_USED  0
#endif
#else
  #define MACSW_BEACON_AC_VI 0
  #define MACSW_AC_BCN_USED  1
#endif
#else
  #define MACSW_BEACONING 0
  #define MACSW_AC_BCN_USED 0
#endif

/// Maximum size of a beacon frame
#define MACSW_BCNFRAME_LEN 512
/** @} Beacon */

/**
 *******************************************************************************
 * @name HE Configuration
 *******************************************************************************
*/
// Disable HE support if FW is compiled for a 11n/ac only HW
#if MACSW_MDM_VER<30 || MACSW_MAC_VER<20
  #undef CFG_HE
#endif

// HE not yet supported for softmac
#if !MACSW_UMAC_PRESENT
  #undef CFG_HE
#endif

/// HE Support
#ifdef CFG_HE
  #define MACSW_HE 1
  // Force aggregation if HE is enabled
  #undef CFG_AGG
  #define CFG_AGG
  // Force VHT support if HE is enabled
  #undef CFG_VHT
  #define CFG_VHT
  // Force MFP support if HE is enabled, as it is mandatory for WiFi certification
  #undef CFG_MFP
  #define CFG_MFP
  #ifdef CFG_RM
    #define MACSW_RM 1
  #else
    #define MACSW_RM 0
  #endif
#else // !CFG_HE
  #define MACSW_HE 0
  #define MACSW_RM 0
#endif // CFG_HE

/** @} HE */

/**
 *******************************************************************************
 * @name TWT Configuration
 *******************************************************************************
 */
#if MACSW_UMAC_PRESENT && MACSW_HE && CFG_TWT
    /// TWT support
    #define MACSW_TWT              1
    /// Maximum Number of Flows
    #define MACSW_TWT_FLOW_NB      CFG_TWT
#else
    #define MACSW_TWT              0
    #define MACSW_TWT_FLOW_NB      0
#endif //MACSW_UMAC_PRESENT && MACSW_HE && defined CFG_TWT

/** @} TWT */

/**
 *******************************************************************************
 *   @name Power Save Configuration
 *******************************************************************************
 */
/// UAPSD support
#ifdef CFG_UAPSD
  #define MACSW_UAPSD 1
  // If UAPSD is enabled, we force the legacy PS mode to be enabled
  #undef CFG_PS
  #define CFG_PS
#else
  #define MACSW_UAPSD 0
#endif

#if MACSW_TWT
  // If TWT is enabled, we force the legacy PS mode to be enabled
  #undef CFG_PS
  #define CFG_PS
#endif //MACSW_TWT

/// Legacy power save support
#ifdef CFG_PS
  #define MACSW_POWERSAVE 1
#else
  #define MACSW_POWERSAVE 0
#endif
/** @} Power Save */

/**
 *******************************************************************************
 * @name VHT Configuration
 *******************************************************************************
 */
#if MACSW_UMAC_PRESENT
  /// VHT Support
  #ifdef CFG_VHT
    #define MACSW_VHT 1
    // Force MFP support if VHT is enabled, as it is mandatory for 802.11ac WiFi certification
    #undef CFG_MFP
    #define CFG_MFP
  #else // !CFG_VHT
    #define MACSW_VHT 0
  #endif // CFG_VHT
#endif // MACSW_UMAC_PRESENT
/** @} VHT */

/**
 *******************************************************************************
 * @name A-MSDU Configuration
 *******************************************************************************
 */
/// Define the A-MSDU option for TX
#ifdef CFG_AMSDU
  #define MACSW_AMSDU_TX 1
  // Force the aggregation to be supported
  #undef CFG_AGG
  #define CFG_AGG
  /// Number of payloads per TX descriptor
  #define MACSW_TX_PAYLOAD_MAX 6
#else
  #define MACSW_AMSDU_TX 0
  /// Number of payloads per TX descriptor
  #define MACSW_TX_PAYLOAD_MAX 1
#endif

/// Maximum size of A-MSDU supported in reception
#ifdef CFG_AMSDU_4K
  #define MACSW_MAX_AMSDU_RX    4096
#elif defined CFG_AMSDU_8K
  #define MACSW_MAX_AMSDU_RX    8192
#elif defined CFG_AMSDU_12K
  #define MACSW_MAX_AMSDU_RX    12288
#endif
/** @} A-MSDU */

/**
 *******************************************************************************
 * @name A-MPDU Configuration
 *******************************************************************************
 */
#ifdef CFG_AGG
  /// A-MPDU TX support
  #define MACSW_AMPDU_TX 1
  #if MACSW_UMAC_PRESENT
    /// Maximum number of TX Block Ack
    #define MACSW_MAX_BA_TX CFG_BATX
    #if (MACSW_MAX_BA_TX == 0)
      #error "At least one BA TX agreement shall be allowed"
    #endif
  #else  // !MACSW_UMAC_PRESENT
    #define MACSW_MAX_BA_TX 0
  #endif // MACSW_UMAC_PRESENT
#else  // !CFG_AGG
  #define MACSW_AMPDU_TX  0
  #define MACSW_MAX_BA_TX 0
  #undef CFG_BWLEN
  #undef CFG_MU_CNT
  #define CFG_MU_CNT          1
#endif // CFG_AGG

#if MACSW_UMAC_PRESENT
  /// Maximum number of RX Block Ack
  #define MACSW_MAX_BA_RX CFG_BARX
  #define MACSW_AMPDU_RX CFG_BARX
  /// RX Packet Reordering Buffer Size
  #define MACSW_AMPDU_RX_BUF_SIZE CFG_REORD_BUF
  #if (MACSW_AMPDU_RX && ((MACSW_AMPDU_RX_BUF_SIZE < 4) || (MACSW_AMPDU_RX_BUF_SIZE > 64)))
     #error "Incorrect reordering buffer size"
  #endif
  /// A-MSDU de-aggregation support
  #ifdef CFG_DEAGG
    #define MACSW_AMSDU_DEAGG 1
  #else
    #define MACSW_AMSDU_DEAGG 0
  #endif
#else  // !MACSW_UMAC_PRESENT
  /// RX Packet Reordering support
  #define MACSW_AMPDU_RX     0
  /// A-MSDU de-aggregation support
  #define MACSW_AMSDU_DEAGG 0
#endif // MACSW_UMAC_PRESENT
/** @} A-MPDU */

/**
 *******************************************************************************
 * @name BeamForming Configuration
 *******************************************************************************
 */
// We support only the VHT and HE calibration, so no need to compile the BFMER code if
// neither VHT nor HE is supported by the fullMAC.
// In softMAC we don't know if the host will use VHT/HE or not, so we keep this code
#if MACSW_UMAC_PRESENT && !MACSW_HE && !MACSW_VHT
  #undef CFG_BFMER
#endif  // MACSW_UMAC_PRESENT && !MACSW_HE && !MACSW_VHT

#ifdef CFG_BFMER
  /// Beamformer support
  #define MACSW_BFMER_EN         1
  /// Maximum number of frames reserved for calibration
  #define MACSW_BFR_TXFRAME_CNT  CFG_MU_CNT
#else // !CFG_BFMER
  #define MACSW_BFMER_EN         0
  #define MACSW_BFR_TXFRAME_CNT  0
  // Disable MU-MIMO TX if Beamformer is not supported
  #undef CFG_MU_CNT
  #define CFG_MU_CNT          1
#endif // CFG_BFMER

/// Number of users supported
#define MACSW_USER_MAX            CFG_MU_CNT
/// MU-MIMO TX support
#define MACSW_MUMIMO_TX_EN        (MACSW_USER_MAX > 1)
/// Support for up to one secondary user
#define MACSW_MUMIMO_SEC_USER1_EN (MACSW_USER_MAX > 1)
/// Support for up to two secondary users
#define MACSW_MUMIMO_SEC_USER2_EN (MACSW_USER_MAX > 2)
/// Support for up to three secondary users
#define MACSW_MUMIMO_SEC_USER3_EN (MACSW_USER_MAX > 3)

/** @} BeamForming */

/**
 *******************************************************************************
 * @name P2P Configuration
 *******************************************************************************
 */
#if CFG_P2P
  /// P2P support
  #define MACSW_P2P             1
  /// Maximum number of simultaneous P2P connections
  #define MACSW_P2P_VIF_MAX     CFG_P2P

  /// P2P GO Support
  #ifdef CFG_P2P_GO
    // Beaconing modes shall be supported
    #if !MACSW_BEACONING
      #error 'Beaconing (BCN) must be enabled'
    #endif
    #define MACSW_P2P_GO 1
  #else
    #define MACSW_P2P_GO 0
  #endif //(GFG_P2P_GO)
#else
  #define MACSW_P2P         0
  #define MACSW_P2P_VIF_MAX 0
  #define MACSW_P2P_GO      0
#endif //(CFG_P2P)
/** @} P2P */

/**
 *******************************************************************************
 * @name MESH Configuration
 *******************************************************************************
 */
#ifdef CFG_MESH
  /// Wireless Mesh Networking support
  #define MACSW_MESH_EN       (1)
  /// UMAC support for MESH
  #define MACSW_UMESH_EN      (MACSW_UMAC_PRESENT)
  /// Maximum Number of
  #define MACSW_MESH_VIF_NB   (CFG_MESH_VIF)
  /// Maximum number of MESH link
  #define MACSW_MESH_LINK_NB  (CFG_MESH_LINK)
  /// Maximum number of MESH path
  #define MACSW_MESH_PATH_NB  (CFG_MESH_PATH)
  /// Maximum number of MESH proxy
  #define MACSW_MESH_PROXY_NB (CFG_MESH_PROXY)
#else
  #define MACSW_MESH_EN       (0)
  #define MACSW_UMESH_EN      (0)
  #define MACSW_MESH_VIF_NB   (0)
  #define MACSW_MESH_LINK_NB  (0)
  #define MACSW_MESH_PATH_NB  (0)
  #define MACSW_MESH_PROXY_NB (0)
#endif //defined CFG_MESH
/** @} MESH */

/**
 *******************************************************************************
 *   @name TX Configuration
 *******************************************************************************
 */
/// Minimal MPDU spacing we support in TX
#define MACSW_TX_MPDU_SPACING   CFG_SPC

/// Number of TX queues in the LMAC
#if MACSW_AC_BCN_USED
#define MACSW_TXQ_CNT          (AC_MAX + MACSW_BEACONING)
#else
#define MACSW_TXQ_CNT          (AC_MAX)
#endif

/// Number of TX descriptors available in the system (BK)
#define MACSW_TXDESC_CNT0       CFG_TXDESC0
/// Number of TX descriptors available in the system (BE)
#define MACSW_TXDESC_CNT1       CFG_TXDESC1
/// Number of TX descriptors available in the system (VI)
#define MACSW_TXDESC_CNT2       CFG_TXDESC2
/// Number of TX descriptors available in the system (VO)
#define MACSW_TXDESC_CNT3       CFG_TXDESC3
#if MACSW_AC_BCN_USED
  /// Number of TX descriptors available in the system (BCN)
  #define MACSW_TXDESC_CNT4       CFG_TXDESC4
#else
  #define MACSW_TXDESC_CNT4 0
#endif

/// Total number of TX descriptors
#define MACSW_TXDESC_CNT ((MACSW_USER_MAX * (MACSW_TXDESC_CNT0 + MACSW_TXDESC_CNT1 + \
                                       MACSW_TXDESC_CNT2 + MACSW_TXDESC_CNT3)) \
                       + MACSW_TXDESC_CNT4)

/// Number of TX frame descriptors and buffers available for frames generated internally
#define MACSW_TXFRAME_CNT (MACSW_VIRT_DEV_MAX + MACSW_BFR_TXFRAME_CNT)
#if MACSW_TXFRAME_CNT < 4
#undef MACSW_TXFRAME_CNT
#define MACSW_TXFRAME_CNT  4
#endif


/// Maximum size of a TX frame generated internally
#if MACSW_UMAC_PRESENT
  #if (MACSW_P2P)
    #define MACSW_TXFRAME_LEN 384
  #elif MACSW_RM
    #define MACSW_TXFRAME_LEN 318
  #else
    #define MACSW_TXFRAME_LEN 256
  #endif //(MACSW_P2P)
#else
  #define MACSW_TXFRAME_LEN   128
#endif

/// Number of TX flow control credits allocated by default per RA/TID (UMAC only)
#define MACSW_DEFAULT_TX_CREDIT_CNT 4
/** @} TX */

/**
 *******************************************************************************
 *   @name RX Configuration
 *******************************************************************************
 */
/// RX Payload buffer size
#define MACSW_RX_PAYLOAD_LEN    512

/// Maximum number of the longest A-MSDUs that can be stored at the same time
#if MACSW_UMAC_PRESENT && MACSW_VHT
  #define MACSW_RX_LONG_MPDU_CNT  3
#else
  #define MACSW_RX_LONG_MPDU_CNT  2
#endif

/// Number of RX payload descriptors - defined to be n times the maximum A-MSDU size
/// plus one extra one used for HW flow control
#define MACSW_RX_PAYLOAD_DESC_CNT ((MACSW_MAX_AMSDU_RX / MACSW_RX_PAYLOAD_LEN) * MACSW_RX_LONG_MPDU_CNT + 1)

/// Number of RX descriptors (SW and Header descriptors)
#define MACSW_RXDESC_CNT MACSW_RX_PAYLOAD_DESC_CNT

#if MACSW_AMSDU_DEAGG
/// Maximum number MSDUs supported in one received A-MSDU
#define MACSW_MAX_MSDU_PER_RX_AMSDU 8
#endif

/// Number of platform DMA descriptors in each RX payload descriptor
#if MACSW_AMSDU_DEAGG
#define MACSW_DMADESC_PER_RX_PDB_CNT MACSW_MAX_MSDU_PER_RX_AMSDU
#else
#define MACSW_DMADESC_PER_RX_PDB_CNT 1
#endif

/** @} RX */

/**
 *******************************************************************************
 * @name Radar Configuration
 *******************************************************************************
 */
#ifdef CFG_RADAR
  /// Radar enable software define
  #define MACSW_RADAR_DETECT 1
  /// Radar enable hardware define (i.e. used in registers file)
  #define MACSW_RADAR_EN     1
#else
  #define MACSW_RADAR_DETECT 0
  #define MACSW_RADAR_EN     0
#endif
/** @} Radar */

/**
 *******************************************************************************
 * @name Unsupported HT Frame Logging Configuration
 *******************************************************************************
 */
#ifdef CFG_UF
  #if (MACSW_MDM_VER >= 20) && (MACSW_MDM_VER < 30)
    /// Unsupported HT Frame Logging enable
    #define MACSW_UF_EN 1
  #else
    #define MACSW_UF_EN 0
  #endif
#else
  #define MACSW_UF_EN 0
#endif
/** @} Unsupported HT Frame Logging */

/**
 *******************************************************************************
 * @name Monitor + Data interface Support
 *******************************************************************************
 */
#if MACSW_UMAC_PRESENT && defined CFG_MON_DATA
  #define MACSW_MON_DATA 1
#else
  #define MACSW_MON_DATA 0
#endif
/** @} Monitor + Data interface Support */

/**
 *******************************************************************************
 * @name FTM Configuration
 *******************************************************************************
 */
#if MACSW_UMAC_PRESENT
  /// FTM intiator Support
  #ifdef CFG_FTM_INIT
    #define MACSW_FTM_INITIATOR 1
  #else // !CFG_FTM_INIT
    #define MACSW_FTM_INITIATOR 0
    #undef CFG_FTM_RSP
  #endif // CFG_FTM_INIT

   /// Fake FTM responder Support
  #ifdef CFG_FTM_RSP
    #define MACSW_FAKE_FTM_RSP 1
  #else // !CFG_FTM_RSP
    #define MACSW_FAKE_FTM_RSP 0
  #endif // CFG_FTM_RSP
#else
#undef CFG_FTM_INIT
#undef CFG_FTM_RSP
#define MACSW_FTM_INITIATOR 0
#define MACSW_FAKE_FTM_RSP 0
#endif // MACSW_UMAC_PRESENT
/** @} VHT */

/**
 *******************************************************************************
 * @name Debug Configuration
 *******************************************************************************
 */
/// Debug dump forwarding
#ifdef CFG_DBGDUMP
  #define MACSW_DEBUG_DUMP 1
#else
  #define MACSW_DEBUG_DUMP 0
#endif

/// Debug key RAM forwarding
#ifdef CFG_DBGDUMPKEY
  #define MACSW_DEBUG_DUMP_KEY 1
#else
  #define MACSW_DEBUG_DUMP_KEY 0
#endif

/// Debug support
#define MACSW_DEBUG     CFG_WIFI_DBG
#define MACSW_DEBUG_RC  0

/// Debug print output
#if MACSW_DEBUG
  #define MACSW_PRINT MACSW_PRINT_IPC
#else
  #define MACSW_PRINT MACSW_PRINT_NONE
#endif

/// Trace Buffer Support
#ifdef CFG_TRACE
  #define MACSW_TRACE 1
#else
  #define MACSW_TRACE 0
#endif

/// Profiling support
#ifdef CFG_PROF
  #define MACSW_PROFILING_ON 1
#else
  #define MACSW_PROFILING_ON 0
#endif

/// System statistics support
#ifdef CFG_STATS
  #define MACSW_SYS_STAT 1
#else
  #define MACSW_SYS_STAT 0
#endif
/** @} Debug */

/**
 *******************************************************************************
 * @name Extra Configuration
 *******************************************************************************
 */
/// Recovery support
#ifdef CFG_REC
  #define MACSW_RECOVERY 1
#else
  #define MACSW_RECOVERY 0
#endif

/// MFP support (for UMAC only)
#if MACSW_UMAC_PRESENT && defined CFG_MFP
  #define MACSW_MFP 1
#else
  #define MACSW_MFP 0
#endif

/// WAPI support
#ifdef CFG_WAPI
  #define MACSW_WAPI_EN 1
#else
  #define MACSW_WAPI_EN 0
#endif

/// WLAN coexistence support
#ifdef CFG_COEX
  #define MACSW_WLAN_COEX_EN 1
#else
  #define MACSW_WLAN_COEX_EN 0
#endif

/// Compilation for a HW supporting Key RAM configuration
#ifdef CFG_KEYCFG
  #define MACSW_KEY_RAM_CONFIG 1
#else
  #define MACSW_KEY_RAM_CONFIG 0
#endif

/// TDLS support
#ifdef CFG_TDLS
  #define MACSW_TDLS 1
#else
  #define MACSW_TDLS 0
#endif

#ifdef CFG_BWLEN
  /// per-BW length adaptation support
  #define MACSW_BW_LEN_ADAPT 1
  /// Number of steps for BW length adaptation
  #define MACSW_BW_LEN_STEPS 4
#else
  #define MACSW_BW_LEN_ADAPT 0
  #define MACSW_BW_LEN_STEPS 1
#endif

/// HSU support. Possible values are:
/// - 0: Don't use HSU, use software implementation.
/// - 1: Use HSU and fallback to software implementation if not available.
/// - 2: Only use HSU. (runtime error is generated if HSU is not available)
#ifdef CFG_HSU
  #define MACSW_HSU (CFG_HSU)
  #define MACSW_HSU_SHA_EN 1
  #define MACSW_HSU_IP_CHK_EN 1
  #define MACSW_HSU_RSA_EN 1
  #define MACSW_HSU_EPM_EN 1
#else
  #define MACSW_HSU 0
#endif

/// Antenna Diversity support
#if MACSW_UMAC_PRESENT && defined CFG_ANT_DIV && (MACSW_MDM_VER < 30)
  #define MACSW_ANT_DIV 1
#else
  #define MACSW_ANT_DIV 0
#endif




/// Traffic generator support
#ifdef CFG_TG
  #define MACSW_TG 1
  #define CFG_BCN_AC_MAX
#else
  #define MACSW_TG 0
#endif

/// Iperf support
#ifdef CFG_IPERF
  #define MACSW_IPERF 1
#else
  #define MACSW_IPERF 0
#endif

/// Ping support
#ifdef CFG_PING
  #define MACSW_PING 1
#else
  #define MACSW_PING 0
#endif

/// Monitor mode support
#ifdef CFG_FHOST_MONITOR
  #define MACSW_FHOST_MONITOR 1
#else
  #define MACSW_FHOST_MONITOR 0
#endif

/// Smartconfig support
#ifdef CFG_SMARTCONFIG
  #undef MACSW_FHOST_MONITOR
  #define MACSW_FHOST_MONITOR 1
  #define MACSW_SMARTCONFIG 1
#else
  #define MACSW_SMARTCONFIG 0
#endif




/// External crypto lib compiled
#ifdef CFG_CRYPTO
  #define MACSW_CRYPTOLIB 1
#else
  #define MACSW_CRYPTOLIB 0
#endif

/// RX statistics support
#ifdef CFG_FHOST_RX_STATS
  #define MACSW_FHOST_RX_STATS 1
#else
  #define MACSW_FHOST_RX_STATS 0
#endif
/** @} Extra */


/**
 *******************************************************************************
 * @name Fw Features Configuration
 * Features automatically enabled if required by the selected configuration
 *******************************************************************************
 */
/// Maximum number of operating channel contexts
#define MACSW_CHAN_CTXT_CNT 3

/// Traffic Detection per STA support
#define MACSW_TD_STA (MACSW_BFMER_EN || MACSW_TDLS || MACSW_HE)

/** @} Features */

/**
 *******************************************************************************
 * @name Custom Configuration
 *******************************************************************************
 */
#ifdef CFG_LPM
#if !defined RC_EZ23Q4_BCN
#define RC_EZ23Q4_BCN 0
#endif

#if !defined RC_EZ23Q4_EDCA
#define RC_EZ23Q4_EDCA 0
#endif

#if !defined BL_RA_LOG_LEVEL
#define BL_RA_LOG_LEVEL 0
#endif

#endif // CFG_LPM

#if !defined CFG_HE_TB
#define MACSW_HE_TB 0
#else
#define MACSW_HE_TB 1
#endif

#if !defined CFG_WFA
#define MACSW_WFA 0
#else
#define MACSW_WFA 1
#endif

/** @} Custom */

/**
 *******************************************************************************
 * @name Misc
 *******************************************************************************
 */
/// Implement IEEE802.11ax-D2.0 (instead of D3.2)
#define MACSW_11AX_DRAFT_2_0               0

/// This macro appears in some generated header files, define it to avoid warning
#define MACSW_MACSW_LDPC_DEC                  1

/// This macro appears in some generated header files, define it to avoid warning
#define MACSW_MACSW_AGC_SNR_EN                1

/// This macro appears in some generated header files, define it to avoid warning
#define MACSW_MACSW_IQ_COMP_EN                1

/// This macro appears in some generated header files, define it to avoid warning
#define MACSW_MACSW_FIQ_COMP_EN               1

/// This macro appears in some generated header files, define it to avoid warning
#define MACSW_MACSW_DERIV_80211B              1

/// This macro appears in some generated header files, define it to avoid warning
#define MACSW_KEY_EXTENDED                 1

/// This macro appears in some generated header files, define it to avoid warning
#define MACSW_BFMEE_EN                     1

/// This macro appears in some generated header files, define it to avoid warning
#define MACSW_MUMIMO_RX_EN                 1

#define MACSW_GP_DMA 0
#define MACSW_TRACE 0
/// @} misc

////////////////////////////////////////////////////

#define CO_BIT(pos) (1UL<<(pos))
#define CO_ALIGN4_HI(val) (((val)+3)&~3)
#define CO_GET_INDEX(__element_ptr, __array_ptr) ((__element_ptr) - (__array_ptr))
#define CO_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#define MAC_ADDR_CPY(addr1_ptr, addr2_ptr)                                              \
    *(((uint16_t*)(addr1_ptr)) + 0) = *(((uint16_t*)(addr2_ptr)) + 0);                  \
    *(((uint16_t*)(addr1_ptr)) + 1) = *(((uint16_t*)(addr2_ptr)) + 1);                  \
    *(((uint16_t*)(addr1_ptr)) + 2) = *(((uint16_t*)(addr2_ptr)) + 2)

#define MAC_ADDR_GROUP(mac_addr_ptr) ((*((uint8_t *)(mac_addr_ptr))) & 1)

#define MAC_ADDR_CMP(addr1_ptr, addr2_ptr)                                              \
    ((*(((uint16_t*)(addr1_ptr)) + 0) == *(((uint16_t*)(addr2_ptr)) + 0)) &&            \
     (*(((uint16_t*)(addr1_ptr)) + 1) == *(((uint16_t*)(addr2_ptr)) + 1)) &&            \
     (*(((uint16_t*)(addr1_ptr)) + 2) == *(((uint16_t*)(addr2_ptr)) + 2)))

#define MAC_ADDR_CMP_PACKED(__a1, __a2)                                                 \
    (memcmp(__a1, __a2, MAC_ADDR_LEN) == 0)

__INLINE uint32_t co_min(uint32_t a, uint32_t b)
{
    return a < b ? a : b;
}

__INLINE uint32_t co_max(uint32_t a, uint32_t b)
{
    return a > b ? a : b;
}

/// Status/error codes used in the MAC software.
enum
{
    /// Successful
    CO_OK,
    /// Unuccessful
    CO_FAIL,
    /// Empty
    CO_EMPTY,
    /// Full
    CO_FULL,
    /// Bad parameters supplied
    CO_BAD_PARAM,
    /// Element not found
    CO_NOT_FOUND,
    /// No more element available
    CO_NO_MORE_ELT_AVAILABLE,
    /// No element currently in use
    CO_NO_ELT_IN_USE,
    /// Busy
    CO_BUSY,
    /// Operation already in progress
    CO_OP_IN_PROGRESS,
};

/// structure of a list element header
struct co_list_hdr
{
    /// Pointer to the next element in the list
    struct co_list_hdr *next;
};

/// structure of a list
struct co_list
{
    /// pointer to first element of the list
    struct co_list_hdr *first;
    /// pointer to the last element
    struct co_list_hdr *last;
};

void co_list_init(struct co_list *list);
void co_list_pool_init(struct co_list *list,
                       void *pool,
                       size_t elmt_size,
                       uint32_t elmt_cnt,
                       void *default_value);

void co_list_push_back(struct co_list *list,
                       struct co_list_hdr *list_hdr);

void co_list_push_front(struct co_list *list,
                        struct co_list_hdr *list_hdr);
struct co_list_hdr *co_list_pop_front(struct co_list *list);

void co_list_extract(struct co_list *list,
                     struct co_list_hdr *list_hdr);

bool co_list_find(struct co_list *list,
                  struct co_list_hdr *list_hdr);

void co_list_insert(struct co_list * const list,
                    struct co_list_hdr * const element,
                    bool (*cmp)(struct co_list_hdr const *elementA,
                                struct co_list_hdr const *elementB));

void co_list_insert_after(struct co_list * const list,
                          struct co_list_hdr * const prev_element,
                          struct co_list_hdr * const element);

void co_list_insert_after_fast(struct co_list * const list,
                               struct co_list_hdr * const prev_element,
                               struct co_list_hdr * const element);

void co_list_insert_before(struct co_list * const list,
                           struct co_list_hdr * const next_element,
                           struct co_list_hdr * const element);

void co_list_concat(struct co_list *list1, struct co_list *list2);

void co_list_remove(struct co_list *list,
                    struct co_list_hdr *prev_element,
                    struct co_list_hdr *element);

__INLINE bool co_list_is_empty(const struct co_list *const list)
{
    bool listempty;
    listempty = (list->first == NULL);
    return (listempty);
}

uint32_t co_list_cnt(const struct co_list *const list);

__INLINE struct co_list_hdr *co_list_pick(const struct co_list *const list)
{
    return (list->first);
}

__INLINE struct co_list_hdr *co_list_pick_last(const struct co_list *const list)
{
    return(list->last);
}

__INLINE struct co_list_hdr *co_list_next(const struct co_list_hdr *const list_hdr)
{
    return(list_hdr->next);
}

#include "macsw_plat.h"
#include "mac_frame.h"
#include "mac_types.h"
#include "ieee80211.h"

#include "phy.h"
#include "wl_api.h"

#if defined(CFG_CSIDMA_CLI) && CFG_CSIDMA_CLI
#include "csidma_cmd.h"
#endif

////////////////////////////////////////////
///

/** @name 802.2 Ethernet definitions */
/// Ethernet MTU
#define LLC_ETHER_MTU                1500
/// Size of the Ethernet header
#define LLC_ETHER_HDR_LEN            14
/// Offset of the Ethernet type within the header
#define LLC_ETHERTYPE_LEN_OFT        12

// Size of OUI
#define LLC_OUI_LEN                  3
/// Size of the Ethernet type
#define LLC_ETHERTYPE_LEN            2

/// Size of LLC field len
#define LLC_LLC_LEN                  3

/** @name SNAP field definitions
 * SNAP = OUI + ETHER_TYPE
 */
#define LLC_SNAP_LEN                 (LLC_OUI_LEN + LLC_ETHERTYPE_LEN)

/** @name 802.2 Header definitions
 * The 802.2 Header is the LLC + the SNAP
 */
#define LLC_802_2_HDR_LEN            (LLC_LLC_LEN + LLC_SNAP_LEN)

/** @name Useful EtherType values */
#define LLC_ETHERTYPE_APPLETALK      0x809B
#define LLC_ETHERTYPE_IPX            0x8137
#define LLC_ETHERTYPE_AARP           0x80F3
#define LLC_ETHERTYPE_EAP_T          0x888E
#define LLC_ETHERTYPE_802_1Q         0x8100
#define LLC_ETHERTYPE_IP             0x0800
#define LLC_ETHERTYPE_ARP            0x0806

#ifdef CFG_ADHOC_ENABLE
#define WLAN_802_11_MTU                   2304
#define RX_MAX_AMSDU_SUBFRAME_LEN (WLAN_802_11_MTU + LLC_ETHER_HDR_LEN + LLC_802_2_HDR_LEN)
#else
/// Maximum A-MSDU subframe length we support (Ethernet Header + LLC/SNAP + Ethernet MTU)
#define RX_MAX_AMSDU_SUBFRAME_LEN (LLC_ETHER_MTU + LLC_ETHER_HDR_LEN + LLC_802_2_HDR_LEN)
#endif

/// LLC/SNAP structure
struct llc_snap_short
{
    /// DSAP + SSAP fieldsr
    uint16_t dsap_ssap;
    /// LLC control + OUI byte 0
    uint16_t control_oui0;
    /// OUI bytes 1 and 2
    uint16_t oui1_2;
};

/// LLC/SNAP structure
struct llc_snap
{
    /// DSAP + SSAP fieldsr
    uint16_t dsap_ssap;
    /// LLC control + OUI byte 0
    uint16_t control_oui0;
    /// OUI bytes 1 and 2
    uint16_t oui1_2;
    /// Protocol
    uint16_t proto_id;
};

/// RFC1042 LLC/SNAP Header
extern const struct llc_snap_short llc_rfc1042_hdr;

/// Bridge-Tunnel LLC/SNAP Header
extern const struct llc_snap_short llc_bridge_tunnel_hdr;

/// Maximum number of CSA counter in a BEACON (1 in CSA IE and 1 in ECSA IE)
#define BCN_MAX_CSA_CPT 2

/// duration of a Time Unit in microseconds
#define TU_DURATION                     1024

/// Maximum number of SSIDs in a scan request
#define SCAN_SSID_MAX   2

/// Maximum number of channels in a scan request
#define SCAN_CHANNEL_MAX (MAC_DOMAINCHANNEL_24G_MAX + MAC_DOMAINCHANNEL_5G_MAX)


/// Array converting a TID to its associated AC
extern const uint8_t mac_tid2ac[];
/// Array converting an AC to its corresponding bit in the QoS Information field
extern const uint8_t mac_ac2uapsd[];


/// Pointer to callback function
typedef void (*cb_timer_func_ptr)(void *);

/// Structure defining the properties of a timer
struct mm_timer_tag
{
    /// List element for chaining
    struct co_list_hdr list_hdr;
    /// Function to be called upon timer expiry
    cb_timer_func_ptr cb;
    /// Pointer to be passed to the callback
    void *env;
    /// Absolute expiry time
    uint32_t time;
};

/// Structure containing the information about a key
struct key_info_tag
{
    /// Replay counters for RX
    uint64_t rx_pn[TID_MAX];
    /// Replay counter for TX
    uint64_t tx_pn;
    /// Union of MIC and MFP keys
    union
    {
        struct
        {
            /// TX MIC key
            uint32_t tx_key[2];
            /// RX MIC key
            uint32_t rx_key[2];
        } mic;
        struct
        {
            uint32_t key[4];
        } mfp;
    }u;
    /// Type of security currently used
    uint8_t cipher;
    /// Key index as specified by the authenticator/supplicant
    uint8_t key_idx;
    /// Index of the key in the HW memory
    uint8_t hw_key_idx;
    /// Flag indicating if the key is valid
    bool valid;
    /// Key cookie
    uint8_t cookie;
};

/////////////////////////////

/// max number of channels in the 2.4 GHZ band
#define MAC_DOMAINCHANNEL_24G_MAX 14

/// max number of channels in the 5 GHZ band
#define MAC_DOMAINCHANNEL_5G_MAX 28


/// Structure containing the parameters of the @ref ME_CHAN_CONFIG_REQ message
struct me_chan_config_req
{
    /// List of 2.4GHz supported channels
    struct mac_chan_def chan2G4[MAC_DOMAINCHANNEL_24G_MAX];
    /// List of 5GHz supported channels
    struct mac_chan_def chan5G[MAC_DOMAINCHANNEL_5G_MAX];
    /// Number of 2.4GHz channels in the list
    uint8_t chan2G4_cnt;
    /// Number of 5GHz channels in the list
    uint8_t chan5G_cnt;
};

enum ME_PARAM_ID_E {
    ME_PARAM_ID_TX_SINGLE_RETRY_CNT_LIMIT,
    ME_PARAM_ID_TX_AMPDU_RETRY_CNT_LIMIT,
    ME_PARAM_ID_TX_AMPDU_PROTECT_ENABLE,
    ME_PARAM_ID_TX_AMPDU_DROP_TO_SINGLETON_RETRYCNT_THRESHOLD,
    ME_PARAM_ID_TX_POWER,
    ME_PARAM_ID_MAX,
};

enum ME_PARAM_CMD_E {
    ME_PARAM_CMD_SET,
    ME_PARAM_CMD_GET,
};

/// Structure containing the parameters of the @ref ME_PARAM_REQ message
struct me_param_req
{
    /// Indicates the param ID
    enum ME_PARAM_ID_E id;
    enum ME_PARAM_CMD_E cmd;//GET or SET.
    /// payload of the param. Max is 32 Bytes
    uint8_t value[50];//GET or SET through value
};

/// Pointer to callback function
typedef void (*cb_raw_send_wait_end_ptr)(void* env);

/// Pointer to callback function for adhoc tx
typedef void (*cb_adhoc_tx_cfm_ptr)(void* env, uint32_t status);

/// Structure containing the parameters of the @ref MM_RAW_SEND_STRAT_REQ message.
struct mm_raw_send_start_req
{
    /// List of waiting raw send req
    struct co_list_hdr list_hdr;
    /// Raw send wait RX response timer
    struct mm_timer_tag wait_rx;
    /// Channel information
    struct mac_chan_op chan;
    /// Index of the VIF
    int vif_idx;
    /// Pakcet buffer
    void *pkt;
    /// Packet len
    uint32_t len;
    /// Duration in ms, default 20ms
    uint32_t duration;
    /// Raw Send wait end callback
    cb_raw_send_wait_end_ptr cb;
    /// event env
    void *env;
    /// whether need wait resp
    int need_rx;
    /// Ad-hoc mode
    bool adhoc;
    /// Receiver address
    struct mac_addr *ra;
    /// Transmitter address
    struct mac_addr *ta;
    /// Tx rate
    uint8_t rate;
    /// RTS thrshold
    uint8_t rts_thrshold;
    /// Tx retry limit
    uint8_t retry_limit;
    /// Tx power
    int8_t tx_power;
    /// cb func for adhoc tx
    cb_adhoc_tx_cfm_ptr cb_cfm;

};

/// Structure containing the parameters of the @ref MM_RAW_SEND_STRAT_CFM message.
struct mm_raw_send_start_cfm
{
    /// Status of operation (different from 0 if unsuccessful)
    uint8_t status;
};

//THD STATINFO fields
//----------------------------------------------------------------------------------------
/// Number of RTS frame retries offset
#define NUM_RTS_RETRIES_OFT                0
/// Number of RTS frame retries mask
#define NUM_RTS_RETRIES_MSK               (0xFF << NUM_RTS_RETRIES_OFT)
/// Number of MPDU frame retries offset
#define NUM_MPDU_RETRIES_OFT               8
/// Number of MPDU frame retries mask
#define NUM_MPDU_RETRIES_MSK              (0xFF << NUM_MPDU_RETRIES_OFT)
/// Retry limit reached: frame unsuccessful
#define RETRY_LIMIT_REACHED_BIT            CO_BIT(16)
/// Frame lifetime expired: frame unsuccessful
#define LIFETIME_EXPIRED_BIT               CO_BIT(17)
/// BA frame not received - valid only for MPDUs part of AMPDU
#define BA_FRAME_RECEIVED_BIT              CO_BIT(18)
/// Frame was transmitted in a HE TB PPDU - Set by SW
#define HE_TB_TX_BIT                       CO_BIT(22)
/// Frame successful by TX DMA: Ack received successfully
#define FRAME_SUCCESSFUL_TX_BIT            CO_BIT(23)
/// Last MPDU of an A-MPDU
#define A_MPDU_LAST                        (0x0F << 26)
/// Transmission bandwidth offset
#define BW_TX_OFT                          24
/// Transmission bandwidth mask
#define BW_TX_MSK                          (0x3 << BW_TX_OFT)
/// Transmission bandwidth - 20MHz
#define BW_20MHZ_TX                        (0x0 << BW_TX_OFT)
/// Transmission bandwidth - 40MHz
#define BW_40MHZ_TX                        (0x1 << BW_TX_OFT)
/// Transmission bandwidth - 80MHz
#define BW_80MHZ_TX                        (0x2 << BW_TX_OFT)
/// Transmission bandwidth - 160MHz
#define BW_160MHZ_TX                       (0x3 << BW_TX_OFT)
/// Descriptor done bit: Set by HW for TX DMA
#define DESC_DONE_TX_BIT                   CO_BIT(31)
/// Descriptor done bit: Set by SW for TX DMA
#define DESC_DONE_SW_TX_BIT                CO_BIT(30)

/// AMSDU TX values
enum amsdu_tx
{
    /// AMSDU configured as recommended by peer
    AMSDU_TX_ADV,
    /// AMSDU Enabled
    AMSDU_TX_EN,
    /// AMSDU Disabled
    AMSDU_TX_DIS,
};

/// Structure containing the parameters of the @ref ME_TX_CREDITS_UPDATE_IND message.
struct me_tx_credits_update_ind
{
    /// Index of the station for which the credits are updated
    uint8_t sta_idx;
    /// TID for which the credits are updated
    uint8_t tid;
    /// Offset to be applied on the credit count
    int8_t credits;
};

/// Rx frame legacy information
struct rx_leg_info
{
    /// Format Modulation
    uint32_t    format_mod     : 4;
    /// Channel Bandwidth
    uint32_t    ch_bw          : 3;
    /// Preamble Type
    uint32_t    pre_type       : 1;
    /// Legacy Length
    uint32_t    leg_length     :12;
    /// Legacy rate
    uint32_t    leg_rate       : 4;
}__PACKED;

/// Parameters of the @ref RXU_MGT_IND message
struct rxu_mgt_ind
{
    /// Length of the frame
    uint16_t length;
    /// Frame control field of the frame.
    uint16_t framectrl;
    /// Center frequency on which we received the packet
    uint16_t center_freq;
    /// PHY band
    uint8_t band;
    /// Index of the station that sent the frame. 0xFF if unknown.
    uint8_t sta_idx;
    /// Index of the VIF that received the frame. 0xFF if unknown.
    uint8_t inst_nbr;
    /// RSSI of the received frame.
    int8_t rssi;
    /// Rx frame legacy information
    struct rx_leg_info rx_leg_inf;
    /// Frame payload.
    uint32_t payload[];
};

/// Structure containing the parameters of the @ref SCANU_START_CFM and
/// @ref SCANU_JOIN_CFM messages
struct scanu_start_cfm
{
    /// Index of the VIF that was scanning
    uint8_t vif_idx;
    /// Status of the request
    uint8_t status;
    /// Number of scan results available
    uint8_t result_cnt;
};

/// Structure containing the parameters of the @ref SM_CONNECT_IND message.
struct sm_connect_ind
{
    /// Status code of the connection procedure
    uint16_t status_code;
    uint16_t ieeetypes_code;
    uint8_t ssid[MAC_SSID_LEN + 1];
    /// BSSID
    struct mac_addr bssid;
    /// Flag indicating if the indication refers to an internal roaming or from a host request
    bool roamed;
    /// Index of the VIF for which the association process is complete
    uint8_t vif_idx;
    /// Index of the STA entry allocated for the AP
    uint8_t ap_idx;
    /// Index of the LMAC channel context the connection is attached to
    uint8_t ch_idx;
    /// Flag indicating if the AP is supporting QoS
    bool qos;
    /// ACM bits set in the AP WMM parameter element
    uint8_t acm;
    /// Length of the AssocReq IEs
    uint16_t assoc_req_ie_len;
    /// Length of the AssocRsp IEs
    uint16_t assoc_rsp_ie_len;
    /// Association Id allocated by the AP for this connection
    uint16_t aid;
    /// AP operating channel
    struct mac_chan_op chan;
    uint8_t security;
    /// bss mode
    uint8_t bss_mode;
    /// EDCA parameters
    uint32_t ac_param[AC_MAX];
    /// IE buffer
    uint32_t assoc_ie_buf[0];
};

/// Structure containing the parameters of the @ref SM_DISCONNECT_IND message.
struct sm_disconnect_ind
{
    /// Status of the disconnection.
    uint16_t status_code;
    /// Reason of the disconnection.
    uint16_t reason_code;
    /// Index of the VIF.
    uint8_t vif_idx;
    /// Disconnection happen before a re-association
    bool reassoc;
};

/// Structure containing the parameters of the @ref ME_TKIP_MIC_FAILURE_IND message
struct me_tkip_mic_failure_ind
{
    /// Address of the sending STA
    struct mac_addr addr;
    /// TSC value
    uint64_t tsc;
    /// Boolean indicating if the packet was a group or unicast one (true if group)
    bool ga;
    /// Key Id
    uint8_t keyid;
    /// VIF index
    uint8_t vif_idx;
};

/// Structure containing the parameters of the @ref MM_PS_CHANGE_IND message
struct mm_ps_change_ind
{
    /// Index of the peer device that is switching its PS state
    uint8_t sta_idx;
    /// New PS state of the peer device (0: active, 1: sleeping)
    uint8_t ps_state;
};

/// Structure containing the parameters of the @ref MM_TRAFFIC_REQ_IND message
struct mm_traffic_req_ind
{
    /// Index of the peer device that needs traffic
    uint8_t sta_idx;
    /// Number of packets that need to be sent (if 0, all buffered traffic shall be sent and
    /// if set to @ref PS_SP_INTERRUPTED, it means that current service period has been interrupted)
    uint8_t pkt_cnt;
    /// Flag indicating if the traffic request concerns U-APSD queues or not
    bool uapsd;
};

/// Structure containing the parameters of the @ref APM_PROBE_CLIENT_CFM message.
struct apm_probe_client_ind
{
    /// Index of the VIF
    uint8_t vif_idx;
    /// Index of the Station to probe
    uint8_t sta_idx;
    /// Whether client is still present or not
    bool client_present;
    /// Unique ID as returned in @ref APM_PROBE_CLIENT_CFM
    uint32_t probe_id;
};

/// Structure containing the parameters of the @ref MM_REMAIN_ON_CHANNEL_EXP_IND message
struct mm_remain_on_channel_exp_ind
{
    /// VIF Index
    uint8_t vif_index;
    /// Channel Context Index
    uint8_t chan_ctxt_index;
    /// Frequency of the channel
    uint16_t freq;
};

/// Structure containing the parameters of the @ref MM_P2P_VIF_PS_CHANGE_IND message
struct mm_p2p_vif_ps_change_ind
{
    /// Index of the P2P VIF that is switching its PS state
    uint8_t vif_index;
    /// New PS state of the P2P VIF interface (0: active, 1: sleeping)
    uint8_t ps_state;
};


#if (MACSW_MESH_EN)
/// Structure containing the parameters of the @ref MESH_PATH_UPDATE_IND message.
struct mesh_path_update_ind
{
    /// Indicate if mesh path is deleted or not
    bool delete;
    /// Indicate if path is towards an external STA (not part of MBSS)
    bool ext_sta;
    /// VIF Index
    uint8_t vif_idx;
    /// Path Index
    uint8_t path_idx;
    /// Target MAC Address
    struct mac_addr tgt_mac_addr;
    /// External STA MAC Address (only if ext_sta is true)
    struct mac_addr ext_sta_mac_addr;
    /// Next Hop STA Index
    uint8_t nhop_sta_idx;
};

/// Structure containing the parameters of the @ref MESH_START_REQ message.
struct mesh_start_req
{
    /// Basic rate set
    struct mac_rateset basic_rates;
    /// Operating channel on which we have to enable the MBSS
    struct mac_chan_op chan;
    /// DTIM period
    uint8_t dtim_period;
    /// Beacon interval
    uint16_t bcn_int;
    /// Index of the VIF for which the MP is started
    uint8_t vif_idx;
    /// Length of the Mesh ID
    uint8_t mesh_id_len;
    /// Mesh ID
    uint8_t mesh_id[MAC_MESHID_ELMT_MAX_LEN];
    /// Address of the IEs to download
    uint32_t ie_addr;
    /// Length of the provided IEs
    uint8_t ie_len;
    /// Indicate if Mesh Peering Management (MPM) protocol is handled in userspace
    bool user_mpm;
    /// Indicate if Mesh Point is using authentication
    bool is_auth;
    /// Indicate which authentication method is used
    uint8_t auth_id;
};

/// Structure containing the parameters of the @ref MESH_START_CFM message.
struct mesh_start_cfm
{
    /// Status of the MP starting procedure
    uint8_t status;
    /// Index of the VIF for which the MP is started
    uint8_t vif_idx;
    /// Index of the channel context attached to the VIF
    uint8_t ch_idx;
    /// Index of the STA used for BC/MC traffic
    uint8_t bcmc_idx;
};

/// Structure containing the parameters of the @ref MESH_STOP_REQ message.
struct mesh_stop_req
{
    /// Index of the VIF for which the MP has to be stopped
    uint8_t vif_idx;
};

/// Structure containing the parameters of the @ref MESH_STOP_CFM message.
struct mesh_stop_cfm
{
    /// Index of the VIF for which the MP has been stopped
    uint8_t vif_idx;
    /// Status
    uint8_t status;
};

/// Structure containing the parameters of the @ref MESH_PEER_UPDATE_NTF message.
struct mesh_peer_update_ntf
{
    /// VIF Index
    uint8_t vif_idx;
    /// STA Index
    uint8_t sta_idx;
    /// Mesh Link State
    uint8_t state;
};
#endif

/// Structure containing the parameters of the @ref FTM_DONE_IND message.
struct ftm_done_ind
{
    /// Index of the VIF for which the FTM is started
    uint8_t vif_idx;
    /// Results
    struct mac_ftm_results results;
};

/// Structure containing the parameters of the @ref SM_EXTERNAL_AUTH_REQUIRED_IND
struct sm_status_ind
{
    /// Index of the VIF.
    uint8_t vif_idx;
    struct
    {
        uint32_t id;
        uint32_t val;
    } info [8];
};

/// Structure containing the parameters of the @ref MM_CSA_FINISH_IND message
struct mm_csa_finish_ind
{
    /// Index of the VIF
    uint8_t vif_index;
    /// Status of the operation
    uint8_t status;
    /// New channel ctx index
    uint8_t chan_idx;
};

/// Structure containing the parameters of the @ref MM_ADD_IF_REQ message.
struct mm_add_if_req
{
    /// Type of the interface (AP, STA, ADHOC, ...)
    uint8_t type;
    /// MAC ADDR of the interface to start
    struct mac_addr addr;
    /// P2P Interface
    bool p2p;
};

/// Structure containing the parameters of the @ref MM_ADD_IF_CFM message.
struct mm_add_if_cfm
{
    /// Status of operation (different from 0 if unsuccessful)
    uint8_t status;
    /// Interface index assigned by the LMAC
    uint8_t inst_nbr;
};

/// Structure containing the parameters of the @ref MM_REMOVE_IF_REQ message.
struct mm_remove_if_req
{
    /// Interface index assigned by the LMAC
    uint8_t inst_nbr;
};

/// Structure containing the parameters of the @ref MM_KEY_ADD_REQ message.
struct mm_key_add_req
{
    /// Key index (valid only for default keys)
    uint8_t key_idx;
    /// STA index (valid only for pairwise keys or mesh group keys)
    uint8_t sta_idx;
    /// Key material
    struct mac_sec_key  key;
    /// Cipher suite (WEP64, WEP128, TKIP, CCMP)
    uint8_t cipher_suite;
    /// Index of the interface for which the key is set (valid only for default keys or mesh group keys)
    uint8_t inst_nbr;
    /// A-MSDU SPP parameter
    uint8_t spp;
    /// Indicate if provided key is a pairwise key or not
    bool pairwise;
};

/// Structure containing the parameters of the @ref MM_KEY_ADD_CFM message.
struct mm_key_add_cfm
{
    /// Status of the operation (different from 0 if unsuccessful)
    uint8_t status;
    /// HW index of the key just added
    uint8_t hw_key_idx;
};

/// Structure containing the parameters of the @ref MM_KEY_DEL_REQ message.
struct mm_key_del_req
{
    /// HW index of the key to be deleted
    uint8_t hw_key_idx;
};

/// Structure containing the parameters of the @ref SCANU_START_REQ and
/// @ref SCANU_JOIN_REQ messages
struct scanu_start_req
{
    /// List of channel to be scanned
    struct mac_chan_def chan[SCAN_CHANNEL_MAX];
    /// List of SSIDs to be scanned
    struct mac_ssid ssid[SCAN_SSID_MAX];
    /// BSSID to be scanned (or WILDCARD BSSID if no BSSID is searched in particular)
    struct mac_addr bssid;
    /// Address (in host memory) of the additional IEs that need to be added to the ProbeReq
    /// (following the SSID element)
    uint32_t add_ies;
    /// Length of the additional IEs
    uint16_t add_ie_len;
    /// Index of the VIF that is scanning
    uint8_t vif_idx;
    /// Number of channels to scan
    uint8_t chan_cnt;
    /// Number of SSIDs to scan for
    uint8_t ssid_cnt;
    /// no CCK - For P2P frames not being sent at CCK rate in 2GHz band.
    bool no_cck;
    /// MISC flags
    uint32_t flags;
    /// Scan duration, in us. If 0 use default values
    uint32_t duration;
    /// Probe request tx count
    uint32_t probe_cnt;
    /// If true, it means will not to connnect after scan done
    bool scan_only;
};

/// Structure containing the parameters of @ref SM_CONNECT_REQ and SM_FT_AUTH_RSP message.
struct sm_connect_req
{
    /// SSID to connect to
    struct mac_ssid ssid;
    /// BSSID to connect to (if not specified, set this field to WILDCARD BSSID)
    struct mac_addr bssid;
    /// Channel on which we have to connect (if not specified, set -1 in the chan.freq field)
    struct mac_chan_def chan;
    /// WiFi Mode
    uint32_t mode;
    /// Connection flags (@ref mac_connection_flags)
    uint32_t flags;
    /// Control port Ethertype
    uint16_t ctrl_port_ethertype;
    /// Listen interval to be used for this connection
    uint16_t listen_interval;
    /// Flag indicating if the we have to wait for the BC/MC traffic after beacon or not
    bool dont_wait_bcmc;
    /// Authentication type
    uint8_t auth_type;
    /// Flag indicating if WEP alternative auth method tried
    bool wep_alt_auth_tried;
    /// WPA/WPA2/WPA3
    uint8_t wpa;
    /// UAPSD queues (bit0: VO, bit1: VI, bit2: BK, bit3: BE)
    uint8_t uapsd_queues;
    /// VIF index
    uint8_t vif_idx;
    /// Length of the association request IEs
    uint16_t ie_len;
    /// Buffer containing the additional information elements to be put in the
    /// association request
    uint32_t ie_buf[0];
};

/// Structure containing the parameters of the @ref SM_CONNECT_CFM message.
struct sm_connect_cfm
{
    /// Status. If 0, it means that the connection procedure will be performed and that
    /// a subsequent @ref SM_CONNECT_IND message will be forwarded once the procedure is
    /// completed.
    uint8_t status;
};

/// Structure containing the parameters of the @ref SM_DISCONNECT_REQ message.
struct sm_disconnect_req
{
    /// Reason of the deauthentication.
    uint16_t reason_code;
    /// Index of the VIF.
    uint8_t vif_idx;
    /// Status code
    uint16_t status_code;
};

/// Structure containing the parameters of the @ref ME_SET_CONTROL_PORT_REQ message
struct me_set_control_port_req
{
    /// Index of the station for which the control port is opened
    uint8_t sta_idx;
    /// Control port state
    bool control_port_open;
};

struct me_set_control_port_cfm
{
    /// Control port state
    bool control_port_open;
};

#if MACSW_SYS_STAT
/// Structure containing the parameters of the @ref DBG_GET_SYS_STAT_CFM message.
struct dbg_get_sys_stat_cfm
{
    /// Time spent in CPU sleep since last reset of the system statistics
    uint32_t cpu_sleep_time;
    /// Time spent in DOZE since last reset of the system statistics
    uint32_t doze_time;
    /// Total time spent since last reset of the system statistics
    uint32_t stats_time;
};
#endif

/// Structure containing the parameters of the @ref SCANU_GET_SCAN_RESULT_REQ message
struct scanu_get_scan_result_req
{
    /// index of the scan element
    uint8_t idx;
};

/// Structure containing the parameters of the @ref SCANU_GET_SCAN_RESULT_CFM message
struct scanu_get_scan_result_cfm
{
    /// Structure for scan result element
    struct mac_scan_result scan_result;
};

/// Structure containing the parameters of the @ref MM_VERSION_CFM message.
struct mm_version_cfm
{
    /// Version of the LMAC FW
    uint32_t version_lmac;
    /// Version1 of the MAC HW (as encoded in version1Reg MAC HW register)
    uint32_t version_machw_1;
    /// Version2 of the MAC HW (as encoded in version2Reg MAC HW register)
    uint32_t version_machw_2;
    /// Version1 of the PHY (depends on actual PHY)
    uint32_t version_phy_1;
    /// Version2 of the PHY (depends on actual PHY)
    uint32_t version_phy_2;
    /// Supported Features
    uint32_t features;
    /// Maximum number of supported stations
    uint16_t max_sta_nb;
    /// Maximum number of supported virtual interfaces
    uint8_t max_vif_nb;
};

/// Structure containing the parameters of the @ref ME_CONFIG_MONITOR_REQ message.
struct me_config_monitor_req
{
    /// Channel to configure
    struct mac_chan_op chan;
    /// Is channel data valid
    bool chan_set;
    /// Enable report of unsupported HT frames
    bool uf;
};

/// Structure containing the parameters of the @ref ME_CONFIG_MONITOR_CFM message.
struct me_config_monitor_cfm
{
    /// Channel context index
    uint8_t chan_index;
    /// Channel parameters (valid only if chan_index is not CHAN_CTXT_UNUSED)
    struct mac_chan_op chan;
};

/// Structure containing the parameters of the @ref SM_EXTERNAL_AUTH_REQUIRED_RSP
struct sm_external_auth_required_rsp
{
    /// Index of the VIF.
    uint8_t vif_idx;
    /// Authentication status
    uint16_t status;
};

/// Structure containing the parameters of the @ref APM_START_REQ message.
struct apm_start_req
{
    /// Basic rate set
    struct mac_rateset basic_rates;
    /// Operating channel on which we have to enable the AP
    struct mac_chan_op chan;
    /// Address, in host memory, to the beacon template
    uint32_t bcn_addr;
    /// Length of the beacon template
    uint16_t bcn_len;
    /// Offset of the TIM IE in the beacon
    uint16_t tim_oft;
    /// Beacon interval
    uint16_t bcn_int;
    /// Flags (@ref mac_connection_flags)
    uint32_t flags;
    /// Control port Ethertype
    uint16_t ctrl_port_ethertype;
    /// Length of the TIM IE
    uint8_t tim_len;
    /// Index of the VIF for which the AP is started
    uint8_t vif_idx;
};

/// Structure containing the parameters of the @ref APM_START_CFM message.
struct apm_start_cfm
{
    /// Status of the AP starting procedure
    uint8_t status;
    /// Index of the VIF for which the AP is started
    uint8_t vif_idx;
    /// Index of the channel context attached to the VIF
    uint8_t ch_idx;
    /// Index of the STA used for BC/MC traffic
    uint8_t bcmc_idx;
};

/// Structure containing the parameters of the @ref APM_STOP_REQ message.
struct apm_stop_req
{
    /// Index of the VIF for which the AP has to be stopped
    uint8_t vif_idx;
};

/// Structure containing the parameters of the @ref MM_SET_EDCA_REQ message
struct mm_set_edca_req
{
    /// EDCA parameters of the queue (as expected by edcaACxReg HW register)
    uint32_t ac_param;
    /// Flag indicating if UAPSD can be used on this queue
    bool uapsd;
    /// HW queue for which the parameters are configured
    uint8_t hw_queue;
    /// Index of the interface for which the parameters are configured
    uint8_t inst_nbr;
};

/// Structure containing the parameters of the @ref MM_BCN_CHANGE_REQ message.
struct mm_bcn_change_req
{
    /// Pointer, in host memory, to the new beacon template
    uint32_t bcn_ptr;
    /// Length of the beacon template
    uint16_t bcn_len;
    /// Offset of the TIM IE in the beacon
    uint16_t tim_oft;
    /// Length of the TIM IE
    uint8_t tim_len;
    /// Index of the VIF for which the beacon is updated
    uint8_t inst_nbr;
    /// Offset of CSA (channel switch announcement) counters (0 means no counter)
    uint8_t csa_oft[BCN_MAX_CSA_CPT];
};

/// Structure containing the parameters of the @ref ME_STA_ADD_REQ message
struct me_sta_add_req
{
    /// MAC address of the station to be added
    struct mac_addr mac_addr;
    /// Supported legacy rates
    struct mac_rateset rate_set;
    /// HT Capabilities
    struct mac_htcapability ht_cap;
    /// VHT Capabilities
    struct mac_vhtcapability vht_cap;
    /// HE capabilities
    struct mac_hecapability he_cap;
    /// Flags giving additional information about the station (@ref mac_sta_flags)
    uint32_t flags;
    /// Association ID of the station
    uint16_t aid;
    /// Bit field indicating which queues have U-APSD enabled
    uint8_t uapsd_queues;
    /// Maximum size, in frames, of a APSD service period
    uint8_t max_sp_len;
    /// Operation mode information (valid if bit @ref STA_OPMOD_NOTIF is
    /// set in the flags)
    uint8_t opmode;
    /// Index of the VIF the station is attached to
    uint8_t vif_idx;
    /// Whether the station is a TDLS station
    bool tdls_sta;
    /// Whether the TDLS station is the TDLS link initiator
    bool tdls_initiator;
    /// Whether the TDLS Channel Switch is allowed
    bool tdls_chsw_allowed;
};

/// Structure containing the parameters of the @ref ME_STA_ADD_CFM message
struct me_sta_add_cfm
{
    /// Station index
    uint8_t sta_idx;
    /// Status of the station addition
    uint8_t status;
    /// PM state of the station
    uint8_t pm_state;
};

/// Structure containing the parameters of the @ref ME_STA_DEL_REQ message.
struct me_sta_del_req
{
    /// Index of the station to be deleted
    uint8_t sta_idx;
    /// Whether the station is a TDLS station
    bool tdls_sta;
};

/// Structure containing the parameters of the @ref ME_TRAFFIC_IND_REQ message.
struct me_traffic_ind_req
{
    /// Index of the station for which UAPSD traffic is available on host
    uint8_t sta_idx;
    /// Flag indicating the availability of UAPSD packets for the given STA
    uint8_t tx_avail;
    /// Indicate if traffic is on uapsd-enabled queues
    bool uapsd;
};

/// Structure containing the parameters of the @ref APM_PROBE_CLIENT_REQ message.
struct apm_probe_client_req
{
    /// Index of the VIF
    uint8_t vif_idx;
    /// Index of the Station to probe
    uint8_t sta_idx;
};

/// Structure containing the parameters of the @ref APM_PROBE_CLIENT_CFM message.
struct apm_probe_client_cfm
{
    /// Status of the probe request
    uint8_t status;
    /// Unique ID to distinguish @ref APM_PROBE_CLIENT_IND message
    uint32_t probe_id;
};

/// Structure containing the parameters of the @ref ME_RC_SET_RATE_REQ message.
struct me_rc_set_rate_req
{
    /// Index of the station for which the fixed rate is requested
    uint8_t sta_idx;
    /// Fixed rate configuration
    uint16_t fixed_rate_cfg;
};

struct me_get_edca_req
{
    /// HW queue for which the parameters are getted
    uint8_t hw_queue;
};

struct me_get_edca_cfm
{
    /// EDCA parameters of the queue (as expected by edcaACxReg HW register)
    uint32_t ac_param;
};

struct me_get_remaining_tx_cfm
{
    /// The number of frames remaining in each Tx queue.
    uint8_t tx0_cnt;
    uint8_t tx1_cnt;
    uint8_t tx2_cnt;
    uint8_t tx3_cnt;
};

struct me_get_stats_cfm
{
    struct ieee80211_stats sta_stats;
    struct ieee80211_stats ap_stats;
    struct ieee80211_stats adhoc_stats;
};

#if MACSW_POWERSAVE
/// Structure containing the parameters of the @ref ME_SET_PS_MODE_REQ message.
struct me_set_ps_mode_req
{
    /// Power Save is activated or deactivated
    uint8_t ps_state;
    /// PS mode
    uint8_t ps_mode;
};
#endif

/// Structure containing the parameters of the @ref MM_REMAIN_ON_CHANNEL_REQ message.
struct mm_remain_on_channel_req
{
    /// Operation Code
    uint8_t op_code;
    /// VIF Index
    uint8_t vif_index;
    /// Channel parameter
    struct mac_chan_op chan;
    /// Duration
    uint32_t duration_ms;
};

/// Structure containing the parameters of the @ref MM_REMAIN_ON_CHANNEL_CFM message
struct mm_remain_on_channel_cfm
{
    /// Operation Code
    uint8_t op_code;
    /// Status of the operation
    uint8_t status;
    /// Channel Context index
    uint8_t chan_ctxt_index;
};

/// Structure containing the parameters of the @ref MM_SET_P2P_NOA_REQ message.
struct mm_set_p2p_noa_req
{
    /// VIF Index
    uint8_t vif_index;
    /// Allocated NOA Instance Number - Valid only if count = 0
    uint8_t noa_inst_nb;
    /// Count
    uint8_t count;
    /// Indicate if NoA can be paused for traffic reason
    bool dyn_noa;
    /// Duration (in us)
    uint32_t duration_us;
    /// Interval (in us)
    uint32_t interval_us;
    /// Start Time offset from next TBTT (in us)
    uint32_t start_offset;
};

/// Structure containing the parameters of the @ref MM_SET_P2P_NOA_CFM message.
struct mm_set_p2p_noa_cfm
{
    /// Request status
    uint8_t status;
};

#if MACSW_FTM_INITIATOR
/// Structure containing the parameters of the @ref FTM_START_REQ message.
struct ftm_start_req
{
    /// Index of the VIF for which the FTM is started
    uint8_t vif_idx;
    /// Number of FTMs per Burst
    uint8_t ftm_per_burst;
    /// Number of FTM responders on which we do the measurements
    uint8_t nb_ftm_rsp;
};

/// Structure containing the parameters of the @ref FTM_START_CFM message.
struct ftm_start_cfm
{
    /// Status of the FTM starting procedure
    uint8_t status;
    /// Index of the VIF for which the FTM is started
    uint8_t vif_idx;
};
#endif

/// Structure containing the parameters of the @ref ME_CONFIG_REQ message
struct me_config_req
{
    /// HT Capabilities
    struct mac_htcapability ht_cap;
    /// VHT Capabilities
    struct mac_vhtcapability vht_cap;
    /// HE capabilities
    struct mac_hecapability he_cap;
    /// Lifetime of packets sent under a BlockAck agreement (expressed in TUs)
    uint16_t tx_lft;
    /// Maximum supported BW
    uint8_t phy_bw_max;
    /// Boolean indicating if HT is supported or not
    bool ht_supp;
    /// Boolean indicating if VHT is supported or not
    bool vht_supp;
    /// Boolean indicating if HE is supported or not
    bool he_supp;
    /// Boolean indicating if HE OFDMA UL is enabled or not
    bool he_ul_on;
    /// Boolean indicating if PS mode shall be enabled or not
    bool ps_on;
    /// Boolean indicating if Antenna Diversity shall be enabled or not
    bool ant_div_on;
    /// Boolean indicating if Dynamic PS mode shall be used or not
    bool dpsm;
    /// Indicates whether AMSDU shall be forced or not
    enum amsdu_tx amsdu_tx;
    #ifdef CFG_LPM
    /// Chip version
    uint8_t chip_version;
    #endif
};

/// Structure containing the parameters of the @ref MM_START_REQ message
struct mm_start_req
{
    /// PHY configuration
    struct phy_cfg_tag phy_cfg;
    /// UAPSD timeout
    uint32_t uapsd_timeout;
    /// Local LP clock accuracy (in ppm)
    uint16_t lp_clk_accuracy;
    /// Array of TX timeout values (in ms, one per TX queue) - 0 sets default value
    uint16_t tx_timeout[AC_MAX];
    /// coex_mode for MAC_SW, 0 is default and no coex
    uint8_t coex_mode;
};

struct mm_bcn_control_req
{
    uint8_t bcn_tx_mode;
    int bcn_tx_timer;
    bool bcn_tx_stop;
};

///TWT Flow configuration
struct twt_conf_tag
{
    /// Flow Type (0: Announced, 1: Unannounced)
    uint8_t flow_type;
    /// Wake interval Exponent
    uint8_t wake_int_exp;
    /// Unit of measurement of TWT Minimum Wake Duration (0:256us, 1:tu)
    bool wake_dur_unit;
    /// Nominal Minimum TWT Wake Duration
    uint8_t min_twt_wake_dur;
    /// TWT Wake Interval Mantissa
    uint16_t wake_int_mantissa;
};

///TWT Setup request message structure
struct twt_setup_req
{
    /// VIF Index
    uint8_t vif_idx;
    /// Setup request type
    uint8_t setup_type;
    /// TWT Setup configuration
    struct twt_conf_tag conf;
};

///TWT Setup confirmation message structure
struct twt_setup_cfm
{
    /// Status (0 = TWT Setup Request has been transmitted to peer)
    uint8_t status;
};

/// TWT Teardown request message structure
struct twt_teardown_req
{
    /// TWT Negotiation type
    uint8_t neg_type;
    /// All TWT
    uint8_t all_twt;
    /// TWT flow ID
    uint8_t id;
    /// VIF Index
    uint8_t vif_idx;
};

///TWT Teardown confirmation message structure
struct twt_teardown_cfm
{
    /// Status (0 = TWT Teardown Request has been transmitted to peer)
    uint8_t status;
};

///TWT Setup indication message structure
struct twt_setup_ind
{
    /// Response type
    uint8_t resp_type;
    /// STA Index
    uint8_t sta_idx;
    /// TWT Setup configuration
    struct twt_conf_tag conf;
};

///////////////////////////////////////////////////////

/**
 ****************************************************************************************
 * @brief This function returns the value of bit field inside an array of bits,
 * represented as an array of bytes.
 * @param[in] array Array of bits
 * @param[in] lsb Position of the LSB of the field inside the array of bits
 * @param[in] width Width of the field
 * @return true if the specified bit is set, false otherwise
 ****************************************************************************************
 */
__INLINE uint8_t co_val_get(uint8_t const array[], int lsb, int width)
{
    int msb = lsb + width - 1;
    int l_byte_idx = lsb/8;
    int m_byte_idx = msb/8;
    uint8_t val;

    if (m_byte_idx == l_byte_idx)
    {
        uint8_t mask = CO_BIT(width) - 1;
        int shift = lsb % 8;
        val = (array[l_byte_idx] >> shift) & mask;
    }
    else
    {
        uint8_t l_bits_cnt = m_byte_idx * 8 - lsb;
        uint8_t l_mask = CO_BIT(l_bits_cnt) - 1;
        uint8_t m_mask = CO_BIT(width - l_bits_cnt) - 1;
        int l_shift = lsb % 8;
        val = (array[l_byte_idx] >> l_shift) & l_mask;
        val |= (array[m_byte_idx] & m_mask) << l_bits_cnt;
    }
    return (val);
}

/**
 ****************************************************************************************
 * @brief This function sets a value of a bit field inside an array of bits,
 * represented as an array of bytes.
 * @param[in] array Array of bits
 * @param[in] lsb Position of the LSB of the field inside the array of bits
 * @param[in] width Width of the field
 * @param[in] val Value to be set
 ****************************************************************************************
 */
__INLINE void co_val_set(uint8_t array[], int lsb, int width, uint8_t val)
{
    int msb = lsb + width - 1;
    int l_byte_idx = lsb/8;
    int m_byte_idx = msb/8;

    if (m_byte_idx == l_byte_idx)
    {
        uint8_t mask = CO_BIT(width) - 1;
        int shift = lsb % 8;
        array[l_byte_idx] &= ~(mask << shift);
        array[l_byte_idx] |= (val & mask) << shift;
    }
    else
    {
        uint8_t l_bits_cnt = m_byte_idx * 8 - lsb;
        uint8_t l_mask = CO_BIT(l_bits_cnt) - 1;
        uint8_t m_mask = CO_BIT(width - l_bits_cnt) - 1;
        int l_shift = lsb % 8;
        array[l_byte_idx] &= ~(l_mask << l_shift);
        array[m_byte_idx] &= ~m_mask;
        array[l_byte_idx] |= (val & l_mask) << l_shift;
        array[m_byte_idx] |= (val >> l_bits_cnt) & m_mask;
    }
}

/**
 ****************************************************************************************
 * @brief This function returns the status of a specific bit position inside an array of
 *        bits, represented as an array of bytes.
 * @param[in] array Array of bits to be checked
 * @param[in] pos Bit position to be checked
 * @return true if the specified bit is set, false otherwise
 ****************************************************************************************
 */
__INLINE bool co_bit_is_set(uint8_t const array[], int pos)
{
    return ((array[pos / 8] & CO_BIT(pos % 8)) != 0);
}

/**
 ****************************************************************************************
 * @brief This function returns the status of a specific bit position inside an array of
 *        bits, represented as an array of bytes.
 * @param[in] array Array of bits to be checked
 * @param[in] pos Bit position to be checked
 * @param[in] length Length of the array
 * @return true if the specified bit is set, false otherwise and false is the bit is
 * outside the range of the array
 ****************************************************************************************
 */
__INLINE bool co_bit_is_set_var(uint8_t* array, int pos, int length)
{
    if ((pos / 8) > (length - 1))
        return false;
    return co_bit_is_set(array, pos);
}

/**
 ****************************************************************************************
 * @brief This function sets a specific bit position inside an array of bits, represented
 * as an array of bytes.
 * @param[in] array Array of bits
 * @param[in] pos Bit position to be set
 ****************************************************************************************
 */
__INLINE void co_bit_set(uint8_t array[], uint8_t pos)
{
    array[pos / 8] |= CO_BIT(pos % 8);
}

/**
 ****************************************************************************************
 * @brief This function clears a specific bit position inside an array of bits,
 * represented as an array of bytes.
 * @param[in] array Array of bits
 * @param[in] pos Bit position to be cleared
 ****************************************************************************************
 */
__INLINE void co_bit_clr(uint8_t array[], uint8_t pos)
{
    array[pos / 8] &= ~CO_BIT(pos % 8);
}

#define HE_MAC_CAPA_VAL_GET(he_cap, field)    co_val_get((he_cap)->mac_cap_info,         \
                                                         HE_MAC_CAPA_##field##_OFT,      \
                                                         HE_MAC_CAPA_##field##_WIDTH)

/**
 ****************************************************************************************
 * Set the value of a bit field in the HE MAC capability element.
 * @param[in] he_cap Pointer to the HE capability structure
 * @param[in] field Bit field to be written
 * @param[in] val The value of the field
 ****************************************************************************************
 */
#define HE_MAC_CAPA_VAL_SET(he_cap, field, val) co_val_set((he_cap)->mac_cap_info,       \
                                                           HE_MAC_CAPA_##field##_OFT,    \
                                                           HE_MAC_CAPA_##field##_WIDTH,  \
                                                           HE_MAC_CAPA_##field##_##val)

/**
 ****************************************************************************************
 * Test if a bit in the HE MAC capability element is set.
 * @param[in] he_cap Pointer to the HE capability structure
 * @param[in] bit    Bit to be set
 * @return true if set, false otherwise
 ****************************************************************************************
 */
#define HE_MAC_CAPA_BIT_IS_SET(he_cap, bit)   co_bit_is_set((he_cap)->mac_cap_info,      \
                                                            HE_MAC_CAPA_##bit##_POS)

/**
 ****************************************************************************************
 * Set a bit in the HE MAC capability element.
 * @param[in] he_cap Pointer to the HE capability structure
 * @param[in] bit    Bit to be set
 ****************************************************************************************
 */
#define HE_MAC_CAPA_BIT_SET(he_cap, bit)      co_bit_set((he_cap)->mac_cap_info,         \
                                                          HE_MAC_CAPA_##bit##_POS)

/**
 ****************************************************************************************
 * Clear a bit in the HE MAC capability element.
 * @param[in] he_cap Pointer to the HE capability structure
 * @param[in] bit    Bit to be cleared
 ****************************************************************************************
 */
#define HE_MAC_CAPA_BIT_CLR(he_cap, bit)      co_bit_clr((he_cap)->mac_cap_info,         \
                                                         HE_MAC_CAPA_##bit##_POS)

/**
 ****************************************************************************************
 * Get the value of a bit field in the HE PHY capability element.
 * @param[in] he_cap Pointer to the HE capability structure
 * @param[in] field  Bit field to be read
 * @return The value of the field
 ****************************************************************************************
 */
#define HE_PHY_CAPA_VAL_GET(he_cap, field)    co_val_get((he_cap)->phy_cap_info,         \
                                                         HE_PHY_CAPA_##field##_OFT,      \
                                                         HE_PHY_CAPA_##field##_WIDTH)

/**
 ****************************************************************************************
 * Set the value of a bit field in the HE PHY capability element.
 * @param[in] he_cap Pointer to the HE capability structure
 * @param[in] field Bit field to be written
 * @param[in] val The value of the field
 ****************************************************************************************
 */
#define HE_PHY_CAPA_VAL_SET(he_cap, field, val) co_val_set((he_cap)->phy_cap_info,       \
                                                           HE_PHY_CAPA_##field##_OFT,    \
                                                           HE_PHY_CAPA_##field##_WIDTH,  \
                                                           HE_PHY_CAPA_##field##_##val)

/**
 ****************************************************************************************
 * Test if a bit in the HE PHY capability element is set.
 * @param[in] he_cap Pointer to the HE capability structure
 * @param[in] bit    Bit to be set
 * @return true if set, false otherwise
 ****************************************************************************************
 */
#define HE_PHY_CAPA_BIT_IS_SET(he_cap, bit)   co_bit_is_set((he_cap)->phy_cap_info,      \
                                                            HE_PHY_CAPA_##bit##_POS)

/**
 ****************************************************************************************
 * Set a bit in the HE PHY capability element.
 * @param[in] he_cap Pointer to the HE capability structure
 * @param[in] bit    Bit to be set
 ****************************************************************************************
 */
#define HE_PHY_CAPA_BIT_SET(he_cap, bit)      co_bit_set((he_cap)->phy_cap_info,         \
                                                         HE_PHY_CAPA_##bit##_POS)

/**
 ****************************************************************************************
 * Clear a bit in the HE PHY capability element.
 * @param[in] he_cap Pointer to the HE capability structure
 * @param[in] bit    Bit to be cleared
 ****************************************************************************************
 */
#define HE_PHY_CAPA_BIT_CLR(he_cap, bit)      co_bit_clr((he_cap)->phy_cap_info,         \
                                                         HE_PHY_CAPA_##bit##_POS)

/**
 ****************************************************************************************
 * Test if a bit in the extended capabilities element is set.
 * @param[in] ext_cap Pointer to the HE extended capabilities structure
 * @param[in] bit     Bit to test
 * @param[in] length  Length of the elements
 * @return true if set, false otherwise
 ****************************************************************************************
 */
#define EXT_CAPA_BIT_IS_SET(ext_cap, bit, length)   co_bit_is_set_var(ext_cap,      \
                                                            MAC_EXT_CAPA_##bit##_POS, length)

/**
 ****************************************************************************************
 * Set a bit in the extended capabilities element.
 * @param[in] ext_cap Pointer to the HE extended capabilities structure
 * @param[in] bit     Bit to test
 * @return true if set, false otherwise
 ****************************************************************************************
 */
#define EXT_CAPA_BIT_SET(ext_cap, bit)   co_bit_set(ext_cap, MAC_EXT_CAPA_##bit##_POS)

/**
 ****************************************************************************************
 * Set the value of a bit field in the FTM Parameters element.
 * @param[in] params Pointer to the FTM Parameters structure
 * @param[in] field Bit field to be written
 * @param[in] val The value of the field
 ****************************************************************************************
 */
#define FTM_PARAMS_VAL_SET(params, field, val) co_val_set((uint8_t *) params,   \
                                                          FTM_##field##_OFT,    \
                                                          FTM_##field##_WIDTH,  \
                                                          FTM_##field##_##val)

/**
 ****************************************************************************************
 * Set the numeric value of a bit field in the FTM Parameters element.
 * @param[in] params Pointer to the FTM Parameters structure
 * @param[in] field Bit field to be written
 * @param[in] val The numeric value
 ****************************************************************************************
 */
#define FTM_PARAMS_VAL_SET_NUM(params, field, val) co_val_set((uint8_t *) params,  \
                                                              FTM_##field##_OFT,   \
                                                              FTM_##field##_WIDTH, \
                                                              val)

/**
 ****************************************************************************************
 * Get the value of a bit field in the FTM Parameters element.
 * @param[in] params Pointer to the FTM Parameters structure
 * @param[in] field  Bit field to be read
 * @return The value of the field
 ****************************************************************************************
 */
#define FTM_PARAMS_VAL_GET(params, field) co_val_get((uint8_t *) params,      \
                                                     FTM_##field##_OFT,       \
                                                     FTM_##field##_WIDTH)

__INLINE uint8_t co_read8p(uint32_t addr)
{
    #ifdef CFG_RWTL
    int shift = (addr & 0x1) * 8;
    uint16_t *ptr = (uint16_t *)(addr / 2);
    return ((uint8_t)((*ptr >> shift) & 0xFF));
    #else
    return (*(uint8_t *)(intptr_t)addr);
    #endif
}

__INLINE void co_write8p(uint32_t addr, uint8_t value)
{
    #ifdef CFG_RWTL
    int shift = (addr & 0x1) * 8;
    uint16_t *ptr = (uint16_t *)(addr / 2);
    *ptr =  (*ptr & ~(0xFF << shift)) | ((value & 0xFF) << shift);
    #else
    *(uint8_t *)(intptr_t)addr = value;
    #endif
}

/**
 ****************************************************************************************
 * @brief Read a packed 16 bits word.
 * @param[in] addr The address of the first byte of the 16 bits word.
 * @return The 16 bits value.
 ****************************************************************************************
 */
__INLINE uint16_t co_read16p(uint32_t addr)
{
    #ifdef CFG_RWTL
    return ((((uint16_t)co_read8p(addr + 1)) << 8) | co_read8p(addr));
    #else
    struct co_read16_struct
    {
        /* uint16_t val __PACKED16 */
        uint16_t val __attribute__ ((__packed__));
    } *ptr = (struct co_read16_struct*)addr;
    return ptr->val;
    #endif
}

/**
 ****************************************************************************************
 * @brief Write a packed 16 bits word.
 * @param[in] addr The address of the first byte of the 16 bits word.
 * @param[in] value The value to write.
 ****************************************************************************************
 */
__INLINE void co_write16p(uint32_t addr, uint32_t value)
{
    #ifdef CFG_RWTL
    co_write8p(addr, value & 0xFF);
    co_write8p(addr + 1, ((value >> 8) & 0xFF));
    #else
    struct co_read16_struct
    {
        /* uint16_t val __PACKED16 */
        uint16_t val __attribute__ ((__packed__));
    } *ptr = (struct co_read16_struct*) addr;

    ptr->val = value;
    #endif
}

/**
 ****************************************************************************************
 * @brief Swap bytes of a 32 bits value.
 * The swap is done in every case. Should not be called directly.
 * @param[in] val32 The 32 bits value to swap.
 * @return The 32 bit swapped value.
 ****************************************************************************************
 */
__INLINE uint32_t co_bswap32(uint32_t val32)
{
    // GCC 4 define __builtin_bswap32(val32);
    return (val32<<24) | ((val32<<8)&0xFF0000) | ((val32>>8)&0xFF00) | ((val32>>24)&0xFF);
}

/**
 ****************************************************************************************
 * @brief Swap bytes of a 16 bits value.
 * The swap is done in every case. Should not be called directly.
 * @param[in] val16 The 16 bit value to swap.
 * @return The 16 bit swapped value.
 ****************************************************************************************
 */
__INLINE uint16_t co_bswap16(uint16_t val16)
{
    return ((val16<<8)&0xFF00) | ((val16>>8)&0xFF);
}


/**
 ****************************************************************************************
 * @brief Convert host to network long word.
 *
 * @param[in] hostlong Long word value to convert.
 * @return The converted long word.
 ****************************************************************************************
 */
__INLINE uint32_t co_htonl(uint32_t hostlong)
{
    return co_bswap32(hostlong);
}

/**
 ****************************************************************************************
 * @brief Convert host to network short word.
 *
 * @param[in] hostshort Short word value to convert.
 * @return The converted short word.
 ****************************************************************************************
 */
__INLINE uint16_t co_htons(uint16_t hostshort)
{
    return co_bswap16(hostshort);
}

/**
 ****************************************************************************************
 * @brief Convert network to host long word.
 *
 * @param[in] netlong Long word value to convert.
 * @return The converted long word.
 ****************************************************************************************
 */
__INLINE uint32_t co_ntohl(uint32_t netlong)
{
    return co_htonl(netlong);
}

/**
 ****************************************************************************************
 * @brief Convert network to host short word.
 *
 * @param[in] netshort Short word value to convert.
 * @return The converted short word.
 ****************************************************************************************
 */
__INLINE uint16_t co_ntohs(uint16_t netshort)
{
    return co_htons(netshort);
}

/**
 ****************************************************************************************
 * @brief Convert host to wlan long word.
 *
 * @param[in] hostlong Long word value to convert.
 * @return The converted long word.
 ****************************************************************************************
 */
__INLINE uint32_t co_htowl(uint32_t hostlong)
{
    return hostlong;
}

/**
 ****************************************************************************************
 * @brief Convert host to wlan short word.
 *
 * @param[in] hostshort Short word value to convert.
 * @return The converted short word.
 ****************************************************************************************
 */
__INLINE uint16_t co_htows(uint16_t hostshort)
{
    return hostshort;
}

/**
 ****************************************************************************************
 * @brief Convert wlan to host long word.
 *
 * @param[in] wlanlong Long word value to convert.
 * @return The converted long word.
 ****************************************************************************************
 */
__INLINE uint32_t co_wtohl(uint32_t wlanlong)
{
    return co_htowl(wlanlong);
}


/**
 ****************************************************************************************
 * @brief Convert wlan to host short word.
 *
 * @param[in] wlanshort Short word value to convert.
 * @return The converted short word.
 ****************************************************************************************
 */
__INLINE uint16_t co_wtohs(uint16_t wlanshort)
{
    return co_htows(wlanshort);
}

// count leading zeros
uint32_t co_clz(uint32_t val);

////////////////////////////
/// Task Identifier. Composed by the task type and the task index.
typedef uint16_t ke_task_id_t;

/// Message Identifier. The number of messages is limited to 0xFFFF.
/// The message ID is divided in two parts:
/// - bits[15..10] : task index (no more than 64 tasks supported).
/// - bits[9..0] : message index (no more that 1024 messages per task).
typedef uint16_t ke_msg_id_t;

/// Format of a task message handler function
typedef int (*ke_msg_func_t)(ke_msg_id_t const msgid, void const *param,
                             ke_task_id_t const dest_id, ke_task_id_t const src_id);

/// Element of a message handler table.
struct ke_msg_handler
{
    /// Id of the handled message.
    ke_msg_id_t id;
    /// Pointer to the handler function for the msgid above.
    ke_msg_func_t func;
};

/// Message structure.
struct ke_msg
{
    struct co_list_hdr hdr;     ///< List header for chaining
    ke_msg_id_t     id;         ///< Message id.
    ke_task_id_t    dest_id;    ///< Destination kernel identifier.
    ke_task_id_t    src_id;     ///< Source kernel identifier.
    uint16_t        param_len;  ///< Parameter embedded struct length.
    uint32_t        param[1];   ///< Parameter embedded struct. Must be word-aligned.
};

/// Tasks types.
enum
{
    TASK_NONE = (uint8_t) -1,

    /// MAC Management task.
    TASK_MM = 0,
    /// DEBUG task
    TASK_DBG,
    /// SCAN task
    TASK_SCAN,
    /// TDLS task
    TASK_TDLS,

    #if (MACSW_UMAC_PRESENT)
    /// SCAN task
    TASK_SCANU,
    /// ME task
    TASK_ME,
    /// SM task
    TASK_SM,
    /// APM task
    TASK_APM,
    /// BAM task
    TASK_BAM,
    /// MESH task
    TASK_MESH,
    /// RXU task
    TASK_RXU,
    /// RM task
    TASK_RM,
    /// TWT task
    TASK_TWT,
    /// FTM task
    TASK_FTM,

    // This is used to define the last task that is running on the EMB processor
    TASK_LAST_EMB = TASK_FTM,
    #else
    // This is used to define the last task that is running on the EMB processor
    TASK_LAST_EMB = TASK_TDLS,
    #endif //(MACSW_UMAC_PRESENT)

    // nX API task
    TASK_API,
    TASK_MAX,
};

#define KE_FIRST_MSG(task) ((ke_msg_id_t)((task) << 10))

enum sm_msg_tag
{
    /// Request to connect to an AP
    SM_CONNECT_REQ = KE_FIRST_MSG(TASK_SM),
    /// Confirmation of connection
    SM_CONNECT_CFM,
    /// Indicates that the SM associated to the AP
    SM_CONNECT_IND,
    /// Request to disconnect
    SM_DISCONNECT_REQ,
    /// Confirmation of disconnection
    SM_DISCONNECT_CFM,
    /// Indicates that the SM disassociated the AP
    SM_DISCONNECT_IND,
    /// Request to start external authentication
    SM_EXTERNAL_AUTH_REQUIRED_IND,
    /// Response to external authentication request
    SM_EXTERNAL_AUTH_REQUIRED_RSP,
    /// Request to update assoc elements after FT over the air authentication
    SM_FT_AUTH_IND,
    /// Response to FT authentication with updated assoc elements
    SM_FT_AUTH_RSP,

    // Section of internal SM messages. No SM API messages should be defined below this point
    /// Timeout message for procedures requiring a response from peer
    SM_RSP_TIMEOUT_IND,
    SM_DEAUTH_TIMEOUT_IND,
    SM_RETRY_CONNECT_START_IND,
    SM_FIX_RATE_CANCLE_TIMEOUT_REQ,
    SM_KEEPALIVE_REQ,
    SM_STATUS_IND,
};

enum {
    SM_STATUS_IND_ID_ASSOC_REFUSHED_TEMP = 1,
};

enum
{
    /// Scan request from host.
    SCANU_START_REQ = KE_FIRST_MSG(TASK_SCANU),
    /// Scanning start Confirmation.
    SCANU_START_CFM,
    /// Join request
    SCANU_JOIN_REQ,
    /// Join confirmation.
    SCANU_JOIN_CFM,
    /// Scan result indication.
    SCANU_RESULT_IND,
    /// Scan result update when connecting.
    SCANU_RESULT_UPDATE,
    /// Get Scan result request.
    SCANU_GET_SCAN_RESULT_REQ,
    /// Scan result confirmation.
    SCANU_GET_SCAN_RESULT_CFM,
};

enum mm_msg_tag
{
    /// RESET Request.
    MM_RESET_REQ = KE_FIRST_MSG(TASK_MM),
    /// RESET Confirmation.
    MM_RESET_CFM,
    /// START Request.
    MM_START_REQ,
    /// START Confirmation.
    MM_START_CFM,
    /// Read Version Request.
    MM_VERSION_REQ,
    /// Read Version Confirmation.
    MM_VERSION_CFM,
    /// ADD INTERFACE Request.
    MM_ADD_IF_REQ,
    /// ADD INTERFACE Confirmation.
    MM_ADD_IF_CFM,
    /// REMOVE INTERFACE Request.
    MM_REMOVE_IF_REQ,
    /// REMOVE INTERFACE Confirmation.
    MM_REMOVE_IF_CFM,
    /// STA ADD Request.
    MM_STA_ADD_REQ,
    /// STA ADD Confirm.
    MM_STA_ADD_CFM,
    /// STA DEL Request.
    MM_STA_DEL_REQ,
    /// STA DEL Confirm.
    MM_STA_DEL_CFM,
    /// RX FILTER Configuration Request.
    MM_SET_FILTER_REQ,
    /// RX FILTER Configuration Confirmation.
    MM_SET_FILTER_CFM,
    /// CHANNEL Configuration Request.
    MM_SET_CHANNEL_REQ,
    /// CHANNEL Configuration Confirmation.
    MM_SET_CHANNEL_CFM,
    /// DTIM PERIOD Configuration Request.
    MM_SET_DTIM_REQ,
    /// DTIM PERIOD Configuration Confirmation.
    MM_SET_DTIM_CFM,
    /// BEACON INTERVAL Configuration Request.
    MM_SET_BEACON_INT_REQ,
    /// BEACON INTERVAL Configuration Confirmation.
    MM_SET_BEACON_INT_CFM,
    /// BASIC RATES Configuration Request.
    MM_SET_BASIC_RATES_REQ,
    /// BASIC RATES Configuration Confirmation.
    MM_SET_BASIC_RATES_CFM,
    /// BSSID Configuration Request.
    MM_SET_BSSID_REQ,
    /// BSSID Configuration Confirmation.
    MM_SET_BSSID_CFM,
    /// EDCA PARAMETERS Configuration Request.
    MM_SET_EDCA_REQ,
    /// EDCA PARAMETERS Configuration Confirmation.
    MM_SET_EDCA_CFM,
    /// ABGN MODE Configuration Request.
    MM_SET_MODE_REQ,
    /// ABGN MODE Configuration Confirmation.
    MM_SET_MODE_CFM,
    /// Request setting the VIF active state (i.e associated or AP started)
    MM_SET_VIF_STATE_REQ,
    /// Confirmation of the @ref MM_SET_VIF_STATE_REQ message.
    MM_SET_VIF_STATE_CFM,
    /// SLOT TIME PARAMETERS Configuration Request.
    MM_SET_SLOTTIME_REQ,
    /// SLOT TIME PARAMETERS Configuration Confirmation.
    MM_SET_SLOTTIME_CFM,
    /// IDLE mode change Request.
    MM_SET_IDLE_REQ,
    /// IDLE mode change Confirm.
    MM_SET_IDLE_CFM,
    /// KEY ADD Request.
    MM_KEY_ADD_REQ,
    /// KEY ADD Confirm.
    MM_KEY_ADD_CFM,
    /// KEY DEL Request.
    MM_KEY_DEL_REQ,
    /// KEY DEL Confirm.
    MM_KEY_DEL_CFM,
    /// Block Ack agreement info addition
    MM_BA_ADD_REQ,
    /// Block Ack agreement info addition confirmation
    MM_BA_ADD_CFM,
    /// Block Ack agreement info deletion
    MM_BA_DEL_REQ,
    /// Block Ack agreement info deletion confirmation
    MM_BA_DEL_CFM,
    /// Indication of the primary TBTT to the upper MAC. Upon the reception of this
    /// message the upper MAC has to push the beacon(s) to the beacon transmission queue.
    MM_PRIMARY_TBTT_IND,
    /// Indication of the secondary TBTT to the upper MAC. Upon the reception of this
    /// message the upper MAC has to push the beacon(s) to the beacon transmission queue.
    MM_SECONDARY_TBTT_IND,
    /// Request for changing the TX power
    MM_SET_POWER_REQ,
    /// Confirmation of the TX power change
    MM_SET_POWER_CFM,
    /// Request to the LMAC to trigger the embedded logic analyzer and forward the debug
    /// dump.
    MM_DBG_TRIGGER_REQ,
    /// Set Power Save mode
    MM_SET_PS_MODE_REQ,
    /// Set Power Save mode confirmation
    MM_SET_PS_MODE_CFM,
    /// Request to add a channel context
    MM_CHAN_CTXT_ADD_REQ,
    /// Confirmation of the channel context addition
    MM_CHAN_CTXT_ADD_CFM,
    /// Request to delete a channel context
    MM_CHAN_CTXT_DEL_REQ,
    /// Confirmation of the channel context deletion
    MM_CHAN_CTXT_DEL_CFM,
    /// Request to link a channel context to a VIF
    MM_CHAN_CTXT_LINK_REQ,
    /// Confirmation of the channel context link
    MM_CHAN_CTXT_LINK_CFM,
    /// Request to unlink a channel context from a VIF
    MM_CHAN_CTXT_UNLINK_REQ,
    /// Confirmation of the channel context unlink
    MM_CHAN_CTXT_UNLINK_CFM,
    /// Request to update a channel context
    MM_CHAN_CTXT_UPDATE_REQ,
    /// Confirmation of the channel context update
    MM_CHAN_CTXT_UPDATE_CFM,
    /// Request to schedule a channel context
    MM_CHAN_CTXT_SCHED_REQ,
    /// Confirmation of the channel context scheduling
    MM_CHAN_CTXT_SCHED_CFM,
    /// Request to change the beacon template in LMAC
    MM_BCN_CHANGE_REQ,
    /// Confirmation of the beacon change
    MM_BCN_CHANGE_CFM,
    ///Beacon Transmission Control
    MM_BCN_CONTROL_REQ,
    /// CFM of Beacon Transmission Control
    MM_BCN_CONTROL_CFM,
    /// Request to update the TIM in the beacon (i.e to indicate traffic bufferized at AP)
    MM_TIM_UPDATE_REQ,
    /// Confirmation of the TIM update
    MM_TIM_UPDATE_CFM,
    /// Connection loss indication
    MM_CONNECTION_LOSS_IND,
    /// Channel context switch indication to the upper layers
    MM_CHANNEL_SWITCH_IND,
    /// Channel context pre-switch indication to the upper layers
    MM_CHANNEL_PRE_SWITCH_IND,
    /// Request to remain on channel or cancel remain on channel
    MM_REMAIN_ON_CHANNEL_REQ,
    /// Confirmation of the (cancel) remain on channel request
    MM_REMAIN_ON_CHANNEL_CFM,
    /// Remain on channel expired indication
    MM_REMAIN_ON_CHANNEL_EXP_IND,
    /// Indication of a PS state change of a peer device
    MM_PS_CHANGE_IND,
    /// Indication that some buffered traffic should be sent to the peer device
    MM_TRAFFIC_REQ_IND,
    /// Request to modify the STA Power-save mode options
    MM_SET_PS_OPTIONS_REQ,
    /// Confirmation of the PS options setting
    MM_SET_PS_OPTIONS_CFM,
    /// Indication of PS state change for a P2P VIF
    MM_P2P_VIF_PS_CHANGE_IND,
    /// Indication that CSA counter has been updated
    MM_CSA_COUNTER_IND,
    /// Message containing channel information
    MM_CHANNEL_SURVEY_IND,
    /// Message containing Beamformer information
    MM_BFMER_ENABLE_REQ,
    /// Request to Start/Stop NOA - GO Only
    MM_SET_P2P_NOA_REQ,
    /// Request to Start/Stop Opportunistic PS - GO Only
    MM_SET_P2P_OPPPS_REQ,
    /// Start/Stop NOA Confirmation
    MM_SET_P2P_NOA_CFM,
    /// Start/Stop Opportunistic PS Confirmation
    MM_SET_P2P_OPPPS_CFM,
    /// P2P NoA Update Indication - GO Only
    MM_P2P_NOA_UPD_IND,
    /// Request to set RSSI threshold and RSSI hysteresis
    MM_CFG_RSSI_REQ,
    /// Indication that RSSI is below or above the threshold
    MM_RSSI_STATUS_IND,
    /// Indication that CSA is done
    MM_CSA_FINISH_IND,
    /// Indication that CSA is in prorgess (resp. done) and traffic must be stopped (resp. restarted)
    MM_CSA_TRAFFIC_IND,
    /// Request to update the group information of a station
    MM_MU_GROUP_UPDATE_REQ,
    /// Confirmation of the @ref MM_MU_GROUP_UPDATE_REQ message
    MM_MU_GROUP_UPDATE_CFM,
    /// Request to initialize the antenna diversity algorithm
    MM_ANT_DIV_INIT_REQ,
    /// Request to stop the antenna diversity algorithm
    MM_ANT_DIV_STOP_REQ,
    /// Request to update the antenna switch status
    MM_ANT_DIV_UPDATE_REQ,
    /// Request to switch the antenna connected to path_0
    MM_SWITCH_ANTENNA_REQ,
    /// Indication that a packet loss has occurred
    MM_PKTLOSS_IND,
    /// MU EDCA PARAMETERS Configuration Request.
    MM_SET_MU_EDCA_REQ,
    /// MU EDCA PARAMETERS Configuration Confirmation.
    MM_SET_MU_EDCA_CFM,
    /// UORA PARAMETERS Configuration Request.
    MM_SET_UORA_REQ,
    /// UORA PARAMETERS Configuration Confirmation.
    MM_SET_UORA_CFM,
    /// TXOP RTS THRESHOLD Configuration Request.
    MM_SET_TXOP_RTS_THRES_REQ,
    /// TXOP RTS THRESHOLD Configuration Confirmation.
    MM_SET_TXOP_RTS_THRES_CFM,
    /// HE BSS Color Configuration Request.
    MM_SET_BSS_COLOR_REQ,
    /// HE BSS Color Configuration Confirmation.
    MM_SET_BSS_COLOR_CFM,
    /// RAW send start request.
    MM_RAW_SEND_STRAT_REQ,
    /// RAW send start confirmation.
    MM_RAW_SEND_STRAT_CFM,

    /*
     * Section of internal MM messages. No MM API messages should be defined below this point
     */
    /// Internal request to force the HW going to IDLE
    MM_FORCE_IDLE_REQ,
    /// Message indicating that the switch to the scan channel is done
    MM_SCAN_CHANNEL_START_IND,
    /// Message indicating that the scan can end early
    MM_SCAN_CHANNEL_END_EARLY,
    /// Message indicating that the scan on the channel is finished
    MM_SCAN_CHANNEL_END_IND,
    /// Internal request to move the AP TBTT by an offset
    MM_TBTT_MOVE_REQ,
    /// Internal request to send raw packet.
    MM_RAW_SEND_REQ,

    /// Timer for link statistics monitor
    MM_LINK_TIMER_IND,
    MM_BBP_START_REQ,
    MM_BBP_STOP_REQ,

    /// MAX number of messages
    MM_MAX,
};

enum mm_remain_on_channel_op
{
    MM_ROC_OP_START = 0,
    MM_ROC_OP_CANCEL,

    MM_ROC_OP_MAX,
};

enum apm_msg_tag
{
    /// Request to start the AP.
    APM_START_REQ = KE_FIRST_MSG(TASK_APM),
    /// Confirmation of the AP start.
    APM_START_CFM,
    /// Request to stop the AP.
    APM_STOP_REQ,
    /// Confirmation of the AP stop.
    APM_STOP_CFM,
    /// Request to start CAC
    APM_START_CAC_REQ,
    /// Confirmation of the CAC start
    APM_START_CAC_CFM,
    /// Request to stop CAC
    APM_STOP_CAC_REQ,
    /// Confirmation of the CAC stop
    APM_STOP_CAC_CFM,
    /// Request to Probe Client
    APM_PROBE_CLIENT_REQ,
    /// Confirmation of Probe Client
    APM_PROBE_CLIENT_CFM,
    /// Indication of Probe Client status
    APM_PROBE_CLIENT_IND,
    /// Indication of Deauth Client
    APM_DEAUTH_CLIENT_IND,
};

enum
{
    /// Configuration request from host.
    ME_CONFIG_REQ = KE_FIRST_MSG(TASK_ME),
    /// Configuration confirmation.
    ME_CONFIG_CFM,
    /// Configuration request param from host.
    ME_PARAM_REQ,
    /// Configuration param confirmation.
    ME_PARAM_CFM,
    /// Configuration request from host.
    ME_CHAN_CONFIG_REQ,
    /// Configuration confirmation.
    ME_CHAN_CONFIG_CFM,
    /// Set control port state for a station.
    ME_SET_CONTROL_PORT_REQ,
    /// Control port setting confirmation.
    ME_SET_CONTROL_PORT_CFM,
    /// TKIP MIC failure indication.
    ME_TKIP_MIC_FAILURE_IND,
    /// Add a station to the FW (AP mode)
    ME_STA_ADD_REQ,
    /// Confirmation of the STA addition
    ME_STA_ADD_CFM,
    /// Delete a station from the FW (AP mode)
    ME_STA_DEL_REQ,
    /// Confirmation of the STA deletion
    ME_STA_DEL_CFM,
    /// Indication of a TX RA/TID queue credit update
    ME_TX_CREDITS_UPDATE_IND,
    /// Request indicating to the FW that there is traffic buffered on host
    ME_TRAFFIC_IND_REQ,
    /// Confirmation that the @ref ME_TRAFFIC_IND_REQ has been executed
    ME_TRAFFIC_IND_CFM,
    /// Request RC statistics to a station
    ME_RC_STATS_REQ,
    /// RC statistics confirmation
    ME_RC_STATS_CFM,
    /// Request RC fixed rate
    ME_RC_SET_RATE_REQ,
    /// Configure monitor interface
    ME_CONFIG_MONITOR_REQ,
    /// Configure monitor interface response
    ME_CONFIG_MONITOR_CFM,
    /// Setting Power Save mode request from host
    ME_SET_PS_MODE_REQ,
    /// Set Power Save mode confirmation
    ME_SET_PS_MODE_CFM,
    // Get EDCA request
    ME_GET_EDCA_REQ,
    // Get EDCA confirmation
    ME_GET_EDCA_CFM,
    // Get the number of frames remaining in each Tx queue request
    ME_GET_REMAINING_TX_REQ,
    // Get the number of frames remaining in each Tx queue confirmation
    ME_GET_REMAINING_TX_CFM,

    // Get wifi stats request
    ME_GET_STATS_REQ,
    // Get wifi stats confirmation
    ME_GET_STATS_CFM,

    // Clear wifi stats
    ME_CLR_STATS_REQ,
    /*
     * Section of internal ME messages. No ME API messages should be defined below this point
     */
    /// Internal request to indicate that a VIF needs to get the HW going to ACTIVE or IDLE
    ME_SET_ACTIVE_REQ,
    /// Confirmation that the switch to ACTIVE or IDLE has been executed
    ME_SET_ACTIVE_CFM,
    /// Internal request to indicate that a VIF desires to de-activate/activate the Power-Save mode
    ME_SET_PS_DISABLE_REQ,
    /// Confirmation that the PS state de-activate/activate has been executed
    ME_SET_PS_DISABLE_CFM,
    /// Indication that data path is flushed for a given station
    ME_DATA_PATH_FLUSHED_IND,
};

// TODO Remove
enum dbg_msg_tag
{
    /// Memory read request
    DBG_MEM_READ_REQ = KE_FIRST_MSG(TASK_DBG),
    /// Memory read confirm
    DBG_MEM_READ_CFM,
    /// Memory write request
    DBG_MEM_WRITE_REQ,
    /// Memory write confirm
    DBG_MEM_WRITE_CFM,
    /// Module filter request
    DBG_SET_MOD_FILTER_REQ,
    /// Module filter confirm
    DBG_SET_MOD_FILTER_CFM,
    /// Severity filter request
    DBG_SET_SEV_FILTER_REQ,
    /// Severity filter confirm
    DBG_SET_SEV_FILTER_CFM,
    /// Fatal error indication
    DBG_ERROR_IND,
    /// Request to get system statistics
    DBG_GET_SYS_STAT_REQ,
    /// COnfirmation of system statistics
    DBG_GET_SYS_STAT_CFM,


    /*
     * Section of internal DBG messages. No DBG API messages should be defined below this point
     */
    /// Timer allowing resetting the system statistics periodically to avoid
    /// wrap around of timer
    DBG_SYS_STAT_TIMER,
};

enum
{
    #if MACSW_UMAC_PRESENT
    /// BROADCAST/GROUP DATA TX STA Index for first virtual AP.
    BROADCAST_STA_IDX_MIN = CFG_STA_MAX,
    /// BROADCAST/GROUP DATA TX STA Index for last virtual AP.
    BROADCAST_STA_IDX_MAX = CFG_STA_MAX + CFG_VIF_MAX - 1,
    /// Maximum number of supported STA
    STA_MAX,

    #define VIF_TO_BCMC_IDX(idx) (BROADCAST_STA_IDX_MIN + (idx))

    #else
    STA_MAX = CFG_STA_MAX,
    #endif
    /// Invalid STA Index used for error checking.
    INVALID_STA_IDX = 0xFF
};


/// Messages that are logically related to the task.
enum ftm_msg_tag
{
    /// Request to start the FTM Measurements
    FTM_START_REQ = KE_FIRST_MSG(TASK_FTM),
    /// Confirmation of the FTM start.
    FTM_START_CFM,
    /// Message sent once the requested measurements have been done
    FTM_DONE_IND,
    /// Request Measurement to the peer STA
    FTM_MEASUREMENT_REQ,
    /// Timeout message for procedures requiring a response from the peer STA
    FTM_PEER_RSP_TIMEOUT_IND,
    /// Request for closing the session after last measurement
    FTM_CLOSE_SESSION_REQ,
    #if MACSW_FAKE_FTM_RSP
    FTM_AP_MEASUREMENT,
    #endif
};

enum scan_msg_tag
{
    /// Scanning start Request.
    SCAN_START_REQ = KE_FIRST_MSG(TASK_SCAN),
    /// Scanning start Confirmation.
    SCAN_START_CFM,
    /// End of scanning indication.
    SCAN_DONE_IND,
    /// Cancel scan request
    SCAN_CANCEL_REQ,
    /// Cancel scan confirmation
    SCAN_CANCEL_CFM,

    /*
     * Section of internal SCAN messages. No SCAN API messages should be defined below this point
     */
    SCAN_PROBE_TIMER,

    /// MAX number of messages
    SCAN_MAX,
};

/// Message API of the TWT task
enum twt_msg_tag
{
    /// Request Individual TWT Establishment
    TWT_SETUP_REQ = KE_FIRST_MSG(TASK_TWT),
    /// Confirm Individual TWT Establishment
    TWT_SETUP_CFM,
    /// Indicate TWT Setup response from peer
    TWT_SETUP_IND,
    /// Request to destroy a TWT Establishment or all of them
    TWT_TEARDOWN_REQ,
    /// Confirm to destroy a TWT Establishment or all of them
    TWT_TEARDOWN_CFM,

    /// MAX number of messages
    TWT_MAX,
};

/// Structure containing the parameters of the @ref SM_EXTERNAL_AUTH_REQUIRED_IND
struct sm_external_auth_required_ind
{
    /// Index of the VIF.
    uint8_t vif_idx;
    /// SSID to authenticate to
    struct mac_ssid ssid;
    /// BSSID to authenticate to
    struct mac_addr bssid;
    /// AKM suite of the respective authentication
    uint32_t akm;
};

enum rx_status_bits
{
    /// The buffer can be forwarded to the networking stack
    RX_STAT_FORWARD = CO_BIT(0),
    /// A new buffer has to be allocated
    RX_STAT_ALLOC = CO_BIT(1),
    /// The buffer has to be deleted
    RX_STAT_DELETE = CO_BIT(2),
    /// The length of the buffer has to be updated
    RX_STAT_LEN_UPDATE = CO_BIT(3),
    /// The length in the Ethernet header has to be updated
    RX_STAT_ETH_LEN_UPDATE = CO_BIT(4),
    /// Simple copy
    RX_STAT_COPY = CO_BIT(5),
    /// Spurious frame (inform upper layer and discard)
    RX_STAT_SPURIOUS = CO_BIT(6),
    /// Frame for monitor interface
    RX_STAT_MONITOR = CO_BIT(7),
    /// unsupported frame
    RX_STAT_UF = CO_BIT(8),
};

/// Packet contains an A-MSDU
#define RX_FLAGS_IS_AMSDU_BIT         CO_BIT(0)
/// Packet contains a 802.11 MPDU
#define RX_FLAGS_IS_MPDU_BIT          CO_BIT(1)
/// Packet contains 4 addresses
#define RX_FLAGS_4_ADDR_BIT           CO_BIT(2)
/// Packet is a Mesh Beacon received from an unknown Mesh STA
#define RX_FLAGS_NEW_MESH_PEER_BIT    CO_BIT(3)
/// Bitmask indicating that a received packet is not a MSDU
#define RX_FLAGS_NON_MSDU_MSK        (RX_FLAGS_IS_MPDU_BIT)
/// Offset of the User priority index field
#define RX_FLAGS_USER_PRIO_INDEX_OFT 4
/// Mask of the User priority index field
#define RX_FLAGS_USER_PRIO_INDEX_MSK (0x7 << RX_FLAGS_USER_PRIO_INDEX_OFT)
/// Offset of the VIF index field
#define RX_FLAGS_VIF_INDEX_OFT  8
/// Mask of the VIF index field
#define RX_FLAGS_VIF_INDEX_MSK  (0xFF << RX_FLAGS_VIF_INDEX_OFT)
/// Offset of the STA index field
#define RX_FLAGS_STA_INDEX_OFT  16
/// Mask of the STA index field
#define RX_FLAGS_STA_INDEX_MSK  (0xFF << RX_FLAGS_STA_INDEX_OFT)
/// Offset of the destination STA index field
#define RX_FLAGS_DST_INDEX_OFT  24
/// Mask of the destination STA index field
#define RX_FLAGS_DST_INDEX_MSK  (0xFF << RX_FLAGS_DST_INDEX_OFT)

/// RX status value structure (as expected by Upper Layers)
struct rxu_stat_val
{
    /// Host Buffer Address
    uint32_t host_id;
    /// Length
    uint32_t frame_len;
    /// Status (@ref rx_status_bits)
    uint16_t status;

    STAILQ_ENTRY(rxu_stat_val) entries;
};

/// RX status descriptor structure (contains value plus other internal data)
struct rxdesc;
struct rxu_stat_desc
{
    #if MACSW_FULLY_HOSTED
    /// Pointer to the next element in the queue
    struct co_list_hdr list_hdr;
    /// Pointer to the associated SW Descriptor (used to know if a payload transfer is
    /// is attached to this descriptor)
    struct rxdesc *rxdesc;
    #else
    /// Pointer to the next element in the queue (Must be first field)
    struct rx_upload_cntrl_tag upload_cntrl;
    /// DMA control structure for status upload
    struct dma_desc dma_desc;
    #endif
    /// Status value to be uploaded to upper layers
    struct rxu_stat_val val;
};

struct rx_vector_1_pad {
  uint32_t pad[4];
};
struct rx_vector_2_pad {
  uint32_t pad[2];
};

/// Structure containing information about the received frame (length,
/// timestamp, rate, etc.)
struct rx_vector {
  /// Total length of the received MPDU
  uint16_t frmlen;
  /// AMPDU status information
  uint16_t ampdu_stat_info;
  /// TSF Low
  uint32_t tsflo;
  /// TSF High
  uint32_t tsfhi;
  /// Receive Vector 1
  struct rx_vector_1_pad rx_vec_1;
  /// Receive Vector 2
  struct rx_vector_2_pad rx_vec_2;
  /// MPDU status information
  uint32_t statinfo;
};

/// Structure containing the information about the received payload
struct rx_info {
  /// Rx header descriptor (this element MUST be the first of the structure)
  struct rx_vector vect;
  /// Structure containing the information about the PHY channel that was used
  /// for this RX
  struct phy_channel_info phy_info;
#if MACSW_UMAC_PRESENT
  /// Word containing some SW flags about the RX packet
  uint32_t flags;
#if MACSW_AMSDU_DEAGG
  /// Array of host buffer identifiers for the other A-MSDU subframes
  uint32_t amsdu_hostids[MACSW_MAX_MSDU_PER_RX_AMSDU - 1];
#endif
#if MACSW_MON_DATA
#error Not Support MACSW_MON_DATA yet
#endif
#endif
  /// Spare room for LMAC FW to write a pattern when last DMA is sent
  uint32_t pattern;
};

/// @name TX Flags passed by the host or UMAC

/// The frame is a retry
#define TXU_CNTRL_RETRY         CO_BIT(0)
/// The frame is sent under a BlockAck agreement
#define TXU_CNTRL_UNDER_BA      CO_BIT(1)
/// More data are buffered on host side for this STA after this packet
#define TXU_CNTRL_MORE_DATA     CO_BIT(2)
/**
 * TX Frame is a management frame:
 *      - WLAN header is already provided, no need to transform an ethernet header
 *      - Frame shall be sent as a singleton
 */
#define TXU_CNTRL_MGMT          CO_BIT(3)
/// No CCK rate can be used (valid only if TXU_CNTRL_MGMT is set)
#define TXU_CNTRL_MGMT_NO_CCK   CO_BIT(4)
/// Internal flag indicating that the PM monitoring has been started for this frame
#define TXU_CNTRL_MGMT_PM_MON   CO_BIT(5)
/// The frame is an A-MSDU
#define TXU_CNTRL_AMSDU         CO_BIT(6)
/// The frame is a robust management frame
#define TXU_CNTRL_MGMT_ROBUST   CO_BIT(7)
/// The frame shall be transmitted using a 4 address MAC Header
#define TXU_CNTRL_USE_4ADDR     CO_BIT(8)
/// The frame is the last of the UAPSD service period
#define TXU_CNTRL_EOSP          CO_BIT(9)
/// This frame is forwarded
#define TXU_CNTRL_MESH_FWD      CO_BIT(10)
/// This frame is sent directly to a TDLS peer
#define TXU_CNTRL_TDLS          CO_BIT(11)
/// This frame is postponed internally because of PS. (only for AP)
#define TXU_CNTRL_POSTPONE_PS   CO_BIT(12)
/// Internal flag indicating that this packet should use the trial rate as first or second rate
#define TXU_CNTRL_RC_TRIAL      CO_BIT(13)
/// Internal flag indicating that this is a UAPSD trigger frame
#define TXU_CNTRL_UAPSD_TRIGGER CO_BIT(14)
/// Re-use SN passed by host instead of computing a new one
#define TXU_CNTRL_REUSE_SN      CO_BIT(15)
/// @}

/** Number of Payload Buffer Descriptors attached to a packet.
    A packet passed by the TCP/IP stack may be split across TX_PBD_CNT buffers.         */
#define TX_PBD_CNT            CFG_TX_PBD_CNT

/// Mask to test if it's a basic rate - BIT(7)
#define MAC_BASIC_RATE                  0x80

/// Maximum Number of CCK rates
#define N_CCK       8
/// Maximum Number of OFDM rates
#define N_OFDM      8
/// First OFDM rate index
#define FIRST_OFDM  N_CCK
/// First HT rate index
#define FIRST_HT    FIRST_OFDM + N_OFDM
/// First VHT rate index
#define FIRST_VHT   FIRST_HT + (8 * 2 * 2 * 4)
/// First HE SU rate index
#define FIRST_HE_SU FIRST_VHT + (10 * 4 * 2 * 8)
/// First HE MU rate index
#define FIRST_HE_MU FIRST_HE_SU + (12 * 4 * 3 * 8)
/// First HE ER rate index
#define FIRST_HE_ER FIRST_HE_MU + (12 * 6 * 3 * 8)
/// Total Number of rates
#define TOT_RATES   FIRST_HE_ER + (2 * 3 * 3 * 3 + 3) // change to support ER+DCM

// Values of the FORMAT_MOD fields
/// Non-HT format
#define FORMATMOD_NON_HT          0
/// Non-HT duplicate OFDM format
#define FORMATMOD_NON_HT_DUP_OFDM 1
/// HT mixed mode format
#define FORMATMOD_HT_MF           2
/// HT greenfield format
#define FORMATMOD_HT_GF           3
/// VHT format
#define FORMATMOD_VHT             4
/// HE-SU format
#define FORMATMOD_HE_SU           5
/// HE-MU format
/// @note This format cannot be used in the policy table but is used when reporting HE
/// TB transmissions/MU receptions to upper layers
#define FORMATMOD_HE_MU           6
/// HE-ER format
#define FORMATMOD_HE_ER           7

/// Format of the DCM modulation offset
#define RC_DCM_MOD_OFT           14
/// Type of DCM modulation mask
#define RC_DCM_MOD_MASK          CO_BIT(RC_DCM_MOD_OFT)


enum mm_features
{
    /// Beaconing
    MM_FEAT_BCN_BIT         = 0,
    /// Radar Detection
    MM_FEAT_RADAR_BIT,
    /// Power Save
    MM_FEAT_PS_BIT,
    /// UAPSD
    MM_FEAT_UAPSD_BIT,
    /// A-MPDU
    MM_FEAT_AMPDU_BIT,
    /// A-MSDU
    MM_FEAT_AMSDU_BIT,
    /// P2P
    MM_FEAT_P2P_BIT,
    /// P2P Go
    MM_FEAT_P2P_GO_BIT,
    /// UMAC Present
    MM_FEAT_UMAC_BIT,
    /// VHT support
    MM_FEAT_VHT_BIT,
    /// Beamformee
    MM_FEAT_BFMEE_BIT,
    /// Beamformer
    MM_FEAT_BFMER_BIT,
    /// WAPI
    MM_FEAT_WAPI_BIT,
    /// MFP
    MM_FEAT_MFP_BIT,
    /// Mu-MIMO RX support
    MM_FEAT_MU_MIMO_RX_BIT,
    /// Mu-MIMO TX support
    MM_FEAT_MU_MIMO_TX_BIT,
    /// Wireless Mesh Networking
    MM_FEAT_MESH_BIT,
    /// TDLS support
    MM_FEAT_TDLS_BIT,
    /// Antenna Diversity support
    MM_FEAT_ANT_DIV_BIT,
    /// UF support
    MM_FEAT_UF_BIT,
    /// A-MSDU maximum size (bit0)
    MM_AMSDU_MAX_SIZE_BIT0,
    /// A-MSDU maximum size (bit1)
    MM_AMSDU_MAX_SIZE_BIT1,
    /// MON_DATA support
    MM_FEAT_MON_DATA_BIT,
    /// HE (802.11ax) support
    MM_FEAT_HE_BIT,
    /// TWT support
    MM_FEAT_TWT_BIT,
    /// FTM initiator support
    MM_FEAT_FTM_INIT_BIT,
    /// Fake FTM responder support
    MM_FEAT_FAKE_FTM_RSP_BIT
};

#if MACSW_POWERSAVE
enum
{
    /// Power-save off
    PS_MODE_OFF,
    /// Power-save on - Normal mode
    PS_MODE_ON,
    /// Power-save on - Dynamic mode
    PS_MODE_ON_DYN,
};
#endif

/// @name TX Status bits passed to the host upon frame confirmation
/// Frame transmission done
#define TX_STATUS_DONE              CO_BIT(0)
/// Frame retry required
#define TX_STATUS_RETRY_REQUIRED    CO_BIT(1)
/// Frame sw retry required
#define TX_STATUS_SW_RETRY_REQUIRED CO_BIT(2)
/// Frame acknoledged
#define TX_STATUS_ACKNOWLEDGED      CO_BIT(3)
/// Frame sequence number (offset)
#define TX_STATUS_SN_OFT            4
/// Frame sequence number (mask)
#define TX_STATUS_SN_MSK            (0xFFF << (TX_STATUS_SN_OFT))

/// Structure indicating the status and other information about the transmission
struct tx_cfm_tag
{
    #if MACSW_UMAC_PRESENT
    /// Number of flow control credits allocated
    int8_t credits;
    /// Size (in mdpu) of the A-MPDU in which this frame has been transmitted.
    /// Valid only for singleton (=1) or last frame of the A-MPDU
    uint8_t ampdu_size;
    #if MACSW_AMSDU_TX
    /// Size allowed for AMSDU
    uint16_t amsdu_size;
    #endif
    #endif // MACSW_UMAC_PRESENT

    /// TX status
    uint32_t status;

    #if !MACSW_FULLY_HOSTED
    /// Host identifier to retrieve TX buffer (set to txdesc.host.hostid)
    /// Keep it last
    uint32_t hostid;
    #endif
};

#if MACSW_FULLY_HOSTED
/// Pointer to the network stack buffer structure
typedef void * hostid_t;
#else
/// Unique host identifier sent back in TX confirmation when txdesc has been processed
typedef uint32_t hostid_t;
#endif

/// Descriptor filled by the Host
struct hostdesc
{
    /// Unique host identifier sent back in TX confirmation when txdesc has been processed
    hostid_t hostid;
    /// Pointers to packet payloads
    uint32_t packet_addr[MACSW_TX_PAYLOAD_MAX];
    /// Sizes of the MPDU/MSDU payloads
    uint16_t packet_len[MACSW_TX_PAYLOAD_MAX];
    #if MACSW_AMSDU_TX
    /// Number of payloads forming the MPDU
    uint8_t packet_cnt;
    #endif //(MACSW_AMSDU_TX)

    #if MACSW_UMAC_PRESENT
    /// Destination Address
    struct mac_addr eth_dest_addr;
    /// Source Address
    struct mac_addr eth_src_addr;
    /// Ethernet Type
    uint16_t ethertype;
    /// TX flags
    uint16_t flags;
    /// SN to use for the transmission (only valid if flag TXU_CNTRL_REUSE_SN is set)
    uint16_t sn_for_retry;
    #else
    /// Padding between the buffer control structure and the MPDU in host memory
    uint8_t padding;
    #endif
    /// Packet TID
    uint8_t tid;
    /// VIF index
    uint8_t vif_idx;
    /// Station Id (0xFF if station is unknown)
    uint8_t staid;
    #if MACSW_MUMIMO_TX_EN
    /// MU-MIMO information (GroupId and User Position in the group) - The GroupId
    /// is located on bits 0-5 and the User Position on bits 6-7. The GroupId value is set
    /// to 63 if MU-MIMO shall not be used
    uint8_t mumimo_info;
    #endif
    #if MACSW_FULLY_HOSTED
    /// TX confirmation info
    struct tx_cfm_tag cfm;
    #endif
};

/* size of internal structs */
#if RC_EZ23Q4 /* TODO remove this ugly define */
#define STRUCT_SIZE_UMACDESC 48
#define STRUCT_SIZE_LMACDESC 24
#else
#define STRUCT_SIZE_UMACDESC 36
#define STRUCT_SIZE_LMACDESC 16
#endif
#define STRUCT_SIZE_TX_HW_DESC 68
#define STRUCT_SIZE_TXL_BUFFER_TAG 240

struct txdesc;
/// LMAC Tx Descriptor
struct txdesc_public
{
    /// List element. A txdesc can be in three list:
    /// - the available list (except txdesc of internal frames and in Fully Hosted)
    /// - the transmitting list
    /// - the confirmation list
    struct co_list_hdr list_hdr;
    /// Information provided by Host
    struct hostdesc host;

    /// Information provided by UMAC
    /* struct umacdesc umac; */ // 36 bytes
    uint32_t umac[STRUCT_SIZE_UMACDESC / 4];
    /// Information provided by LMAC
    /* struct lmacdesc lmac; */ // 16 bytes
    uint32_t lmac[STRUCT_SIZE_LMACDESC / 4];
};

typedef struct bcn_param {
    uint32_t mac_local_tsf_l;
    uint32_t mac_local_tsf_h;
    uint16_t beacon_len;
    uint8_t beacon_rate;
    int8_t beacon_rssi;
} bcn_param_t;


uint32_t mac_ie_multi_bssid_find(uint32_t buffer, uint16_t buflen);
uint32_t mac_ie_sub_non_txed_bssid_find(uint32_t buffer, uint16_t buflen);
uint32_t mac_ie_non_txed_bssid_capa_find(uint32_t buffer, uint16_t buflen);
uint32_t mac_ie_multi_bssid_index_find(uint32_t buffer, uint16_t buflen);
uint32_t mac_ie_ssid_find(uint32_t buffer, uint16_t buflen, uint8_t *ie_len);
uint32_t mac_ext_ie_find(uint32_t addr, uint16_t buflen, uint8_t ext_ie_id, uint16_t *len);
uint32_t mac_vsie_find(uint32_t addr, uint16_t buflen, uint8_t const *oui, uint8_t ouilen, uint16_t *len);
uint32_t mac_ie_find(uint32_t addr, uint16_t buflen, uint8_t ie_id, uint16_t *len);

__INLINE uint16_t mac_ie_len(uint32_t addr)
{
    return ((uint16_t)co_read8p(addr + MAC_INFOELT_LEN_OFT) + MAC_INFOELT_INFO_OFT);
}

/*--------------------------------------------------------------------*/
/* Status Codes - these codes are used in bouffalolab fw actions      */
/* when an error action is taking place the status code can indicate  */
/* what the status code.                                              */
/* It is defined by bouffaloalb                                       */
/*--------------------------------------------------------------------*/
#define WLAN_FW_SUCCESSFUL                                        0
#define WLAN_FW_TX_AUTH_FRAME_ALLOCATE_FAIILURE                   1
#define WLAN_FW_AUTHENTICATION_FAIILURE                           2
#define WLAN_FW_AUTH_ALGO_FAIILURE                                3
#define WLAN_FW_TX_ASSOC_FRAME_ALLOCATE_FAIILURE                  4
#define WLAN_FW_ASSOCIATE_FAIILURE                                5
#define WLAN_FW_DEAUTH_BY_AP_WHEN_NOT_CONNECTION                  6
#define WLAN_FW_DEAUTH_BY_AP_WHEN_CONNECTION                      7
#define WLAN_FW_4WAY_HANDSHAKE_ERROR_PSK_TIMEOUT_FAILURE          8
#define WLAN_FW_4WAY_HANDSHAKE_TX_DEAUTH_FRAME_TRANSMIT_FAILURE   9
#define WLAN_FW_4WAY_HANDSHAKE_TX_DEAUTH_FRAME_ALLOCATE_FAIILURE 10
#define WLAN_FW_AUTH_OR_ASSOC_RESPONSE_TIMEOUT_FAILURE           11
#define WLAN_FW_SCAN_NO_BSSID_AND_CHANNEL                        12
#define WLAN_FW_CREATE_CHANNEL_CTX_FAILURE_WHEN_JOIN_NETWORK     13
#define WLAN_FW_JOIN_NETWORK_FAILURE                             14
#define WLAN_FW_ADD_STA_FAILURE                                  15
#define WLAN_FW_BEACON_LOSS                                      16
#define WLAN_FW_NETWORK_SECURITY_NOMATCH                         17
#define WLAN_FW_NETWORK_WEPLEN_ERROR                             18
#define WLAN_FW_DISCONNECT_BY_USER_WITH_DEAUTH                   19
#define WLAN_FW_DISCONNECT_BY_USER_NO_DEAUTH                     20
#define WLAN_FW_DISCONNECT_BY_FW_PS_TX_NULLFRAME_FAILURE         21
#define WLAN_FW_TRAFFIC_LOSS                                     22
#define WLAN_FW_SWITCH_CHANNEL_FAILURE                           23
#define WLAN_FW_AUTH_OR_ASSOC_RESPONSE_CFM_FAILURE               24
#define WLAN_FW_REASSOCIATE_STARING                              25
#define WLAN_FW_CONNECT_PARAMS_ERROR                             26

/*--------------------------------------------------------------------*/
/* AP Mode Status Codes - these codes are used in bouffalolab fw actions      */
/* when an error action is taking place the status code can indicate  */
/* what the status code.                                              */
/* It is defined by bouffaloalb                                       */
/*--------------------------------------------------------------------*/
#define WLAN_FW_APM_SUCCESSFUL                                        0
#define WLAN_FW_APM_DELETESTA_BY_USER                                 1
#define WLAN_FW_APM_DEATUH_BY_STA                                     2
#define WLAN_FW_APM_DISASSOCIATE_BY_STA                               3
#define WLAN_FW_APM_DELETECONNECTION_TIMEOUT                          4
#define WLAN_FW_APM_DELETESTA_FOR_NEW_CONNECTION                      5
#define WLAN_FW_APM_DEAUTH_BY_AUTHENTICATOR                           6

#define WLAN_CODE_NOTUSED 0xFFFF

void bl_tpc_update_power_table(int8_t *power_table);
void bl_tpc_power_table_get(int8_t *power_table);
/**
 ****************************************************************************************
 * @brief Set keep alive time for sta mode
 *
 * @param[in] the keep alive time for sta mode(unit: seconds)
 ****************************************************************************************
 */
void bl_sta_set_keepalive_period(uint8_t time_seconds);
int bl_wifi_sta_ps_active_ms(uint16_t active_ms);

void *sta_idx_getp_mac_addr(uint8_t sta_idx);
uint8_t sta_idx_get_mlink_idx(uint8_t sta_idx);
uint8_t sta_idx_get_key_idx(uint8_t sta_idx);
void *sta_idx_getp_key_info(uint8_t sta_idx);
uint32_t sta_idx_get_last_active(uint8_t sta_idx);
bool sta_idx_get_valid(uint8_t sta_idx);
uint8_t sta_idx_get_inst_nbr(uint8_t sta_idx);
uint8_t sta_idx_get_uapsd_queues(uint8_t sta_idx);
uint8_t sta_get_staid(void *p);
void *sta_get_list_next(void *p);
void *sta_getp_mac_addr(void *p);

void *vif_info_get_vif(int index);
uint8_t vif_mgmt_get_staid(const void *mac_vif, const struct mac_addr *sta_addr);

enum mac_vif_type mac_vif_get_type(void *macif);
uint8_t mac_vif_get_index(void *macif);
uint8_t mac_vif_get_sta_ap_id(void *macif);
uint16_t mac_vif_get_bcn_int(void *macif);
int mac_vif_get_key_info_len(void *macif);
struct key_info_tag *mac_vif_get_key(void *macif, int key_idx);
bool mac_vif_get_active(void *macif);
struct co_list *mac_vif_get_sta_list(void *macif);
void mac_vif_get_channel(void *macif, struct mac_chan_op* chan);
void mac_vif_get_sta_status(void *macif, struct mac_addr *bssid, uint16_t *aid, int8_t *rssi);
void mac_vif_get_mac_addr(void *macif, struct mac_addr *mac_addr);

void macif_rx_buf_ind(void);
void macif_tx_data_ind(int queue_idx);
void macif_kmsg_call(int cfm_msg_id, void *rsp, int rsp_size,
                     int msg_id, int task_id, void *param, int param_len);
void macif_kmsg_call_t(int cfm_msg_id, void *rsp, int rsp_size,
                       int msg_id, int task_id, int nb_msg, ...);


uint8_t mm_sta_to_key(uint8_t sta_idx);

#if (MACSW_UMESH_EN)
uint8_t mm_mlink_to_mfp_key(uint8_t key_idx, uint8_t mlink_idx);
uint8_t mm_mlink_to_key(uint8_t key_idx, uint8_t mlink_idx);
#endif
uint8_t mm_vif_to_key(uint8_t key_idx, uint8_t vif_idx);
uint8_t mm_vif_to_mfp_key(uint8_t key_idx, uint8_t vif_idx);

void mm_timer_set(struct mm_timer_tag *timer, uint32_t value);
void mm_timer_clear(struct mm_timer_tag *timer);
uint32_t mm_env_get(void);

void me_init_chan(struct mac_chan_op *chan);

bool mfp_is_robust_frame(uint16_t frame_cntl, uint8_t action);

void tx_txdesc_release(struct txdesc_public *txdesc);

void *ke_msg_alloc(ke_msg_id_t const id,
                   ke_task_id_t const dest_id,
                   ke_task_id_t const src_id,
                   uint16_t const param_len);

void ke_msg_free(struct ke_msg const *msg);
void ke_msg_send(void const *param_ptr);

// macro to function M2H_
uint32_t inline_hal_machw_time(void);
bool inline_hal_machw_time_past(uint32_t time);
uint8_t inline_macsw_mac_tkip_getf(void);
uint8_t inline_macsw_mac_ccmp_getf(void);
uint8_t inline_macsw_mac_gcmp_getf(void);
bool inline_hal_machw_he_support(void);

// wifi statistics functions
uint8_t export_stats_get_tx_mcs(void);
uint8_t export_stats_get_rx_mcs(void);
char* export_stats_get_rx_format(void);
char* export_stats_get_tx_format(void);


int8_t export_hal_desc_get_rssi(void *rx_vec_1, int8_t *rx_rssi);
uint16_t export_hal_desc_get_ht_length(void *rx_vec_1);

uint8_t export_rc_get_format_mod(uint32_t rate_config);
uint8_t export_rc_get_mcs_index(uint32_t rate_config);
uint8_t export_rc_get_nss(uint32_t rate_config);
uint8_t export_rc_get_bw(uint32_t rate_config);
uint8_t export_rc_get_pre_type(uint32_t rate_config);
uint8_t export_rc_get_sgi(uint32_t rate_config);

int get_mac_txdesc_cnt(int i);

void tx_desc_init_for_fhost(void *buf, void *desc, int seg_cnt,
                            uint32_t *seg_addr, uint16_t *seg_len);

void txdesc_set_lmac_hwdesc(struct txdesc_public *txdesc, void *hw_desc);
void txdesc_set_lmac_buffer(struct txdesc_public *txdesc, int idx, void *buffer);

#endif
