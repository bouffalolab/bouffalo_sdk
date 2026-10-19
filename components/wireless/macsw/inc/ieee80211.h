#ifndef __IEEE80211_H__
#define __IEEE80211_H__
#include <stdint.h>
#include <stdbool.h>


/**
 ****************************************************************************************
 * @addtogroup CO_INT
 * @ingroup COMMON
 * @brief Common integer standard types (removes use of stdint)
 *
 * @{
 ****************************************************************************************
 */


/*
 * DEFINES
 ****************************************************************************************
 */


typedef uint8_t u8_l;
typedef int8_t s8_l;
typedef bool bool_l;
typedef uint16_t u16_l;
typedef int16_t s16_l;
typedef uint32_t u32_l;
typedef int32_t s32_l;
typedef uint64_t u64_l;

//typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int32_t s32;
typedef int8_t s8;

typedef uint64_t __le64;
typedef uint32_t __le32;
typedef uint16_t __le16;

typedef uint32_t __be32;
typedef uint16_t __be16;

/**
 ****************************************************************************************
 *
 * @file ieee80211.h
 *
 ****************************************************************************************
 */


#ifndef cpu_to_le16
#define cpu_to_le16(v16) (v16)
#endif

#ifndef cpu_to_le32
#define cpu_to_le32(v32) (v32)
#endif

#define WLAN_SA_QUERY_TR_ID_LEN 2
#define WLAN_MEMBERSHIP_LEN 8
#define WLAN_USER_POSITION_LEN 16

#define WIRELESS_FCS_LEN 4

#define WIRELESS_80211_FCTL_FTYPE		0x000c
#define WIRELESS_80211_FCTL_STYPE		0x00f0

#define WIRELESS_80211_SCTL_SEQ		    0xFFF0

#define WIRELESS_80211_FTYPE_MGMT		0x0000
#define WIRELESS_80211_FTYPE_DATA		0x0008

/* management */
#define WIRELESS_80211_STYPE_PROBE_REQ	0x0040
#define WIRELESS_80211_STYPE_PROBE_RESP	0x0050
#define WIRELESS_80211_STYPE_BEACON		0x0080
#define WIRELESS_80211_STYPE_DISASSOC	0x00A0
#define WIRELESS_80211_STYPE_AUTH		0x00B0
#define WIRELESS_80211_STYPE_DEAUTH		0x00C0
#define WIRELESS_80211_STYPE_ACTION		0x00D0

/* data */
#define WIRELESS_80211_STYPE_QOS_DATA		0x0080

#define WIRELESS_80211_SN_MASK		((WIRELESS_80211_SCTL_SEQ) >> 4)
#define WIRELESS_80211_MAX_SN		WIRELESS_80211_SN_MASK
#define WIRELESS_80211_SN_MODULO		(WIRELESS_80211_MAX_SN + 1)

#define WIRELESS_80211_HT_MCS_MASK_LEN       10

/* Reason codes */
enum wireless_80211_reasoncode {
	WIRELESS_REASONCODE_UNSPECIFIED = 1,
	WIRELESS_REASONCODE_PREV_AUTH_NOT_VALID = 2,
	WIRELESS_REASONCODE_DEAUTH_LEAVING = 3,
	WIRELESS_REASONCODE_DISASSOC_DUE_TO_INACTIVITY = 4,
	WIRELESS_REASONCODE_DISASSOC_AP_BUSY = 5,
	WIRELESS_REASONCODE_CLASS2_FRAME_FROM_NONAUTH_STA = 6,
	WIRELESS_REASONCODE_CLASS3_FRAME_FROM_NONASSOC_STA = 7,
	WIRELESS_REASONCODE_DISASSOC_STA_HAS_LEFT = 8,
	WIRELESS_REASONCODE_STA_REQ_ASSOC_WITHOUT_AUTH = 9,
};

/**
 * wireless_80211_is_beacon - check if WIRELESS_80211_FTYPE_MGMT && WIRELESS_80211_STYPE_BEACON
 * @fc: frame control bytes in little-endian byteorder
 */
static inline bool wireless_80211_is_beacon(__le16 fc)
{
	return (fc & cpu_to_le16(WIRELESS_80211_FCTL_FTYPE | WIRELESS_80211_FCTL_STYPE)) ==
	       cpu_to_le16(WIRELESS_80211_FTYPE_MGMT | WIRELESS_80211_STYPE_BEACON);
}

/**
 * wireless_80211_is_deauth - check if WIRELESS_80211_FTYPE_MGMT && WIRELESS_80211_STYPE_DEAUTH
 * @fc: frame control bytes in little-endian byteorder
 */
static inline bool wireless_80211_is_deauth(__le16 fc)
{
	return (fc & cpu_to_le16(WIRELESS_80211_FCTL_FTYPE | WIRELESS_80211_FCTL_STYPE)) ==
	       cpu_to_le16(WIRELESS_80211_FTYPE_MGMT | WIRELESS_80211_STYPE_DEAUTH);
}

/**
 * wireless_80211_is_disassoc - check if WIRELESS_80211_FTYPE_MGMT && WIRELESS_80211_STYPE_DISASSOC
 * @fc: frame control bytes in little-endian byteorder
 */
static inline bool wireless_80211_is_disassoc(__le16 fc)
{
	return (fc & cpu_to_le16(WIRELESS_80211_FCTL_FTYPE | WIRELESS_80211_FCTL_STYPE)) ==
	       cpu_to_le16(WIRELESS_80211_FTYPE_MGMT | WIRELESS_80211_STYPE_DISASSOC);
}

/**
 * wireless_80211_is_action - check if WIRELESS_80211_FTYPE_MGMT && WIRELESS_80211_STYPE_ACTION
 * @fc: frame control bytes in little-endian byteorder
 */
static inline bool wireless_80211_is_action(__le16 fc)
{
	return (fc & cpu_to_le16(WIRELESS_80211_FCTL_FTYPE | WIRELESS_80211_FCTL_STYPE)) ==
	       cpu_to_le16(WIRELESS_80211_FTYPE_MGMT | WIRELESS_80211_STYPE_ACTION);
}

/**
 *  * wireless_80211_is_probe_resp - check if WIRELESS_80211_FTYPE_MGMT && WIRELESS_80211_STYPE_PROBE_RESP
 *   * @fc: frame control bytes in little-endian byteorder
 *    */
static inline int wireless_80211_is_probe_resp(__le16 fc)
{
        return (fc & cpu_to_le16(WIRELESS_80211_FCTL_FTYPE | WIRELESS_80211_FCTL_STYPE)) ==
                       cpu_to_le16(WIRELESS_80211_FTYPE_MGMT | WIRELESS_80211_STYPE_PROBE_RESP);
}

/**
 * wireless_80211_is_probe_req - check if WIRELESS_80211_FTYPE_MGMT && WIRELESS_80211_STYPE_PROBE_REQ
 * @fc: frame control bytes in little-endian byteorder
 */
static inline bool wireless_80211_is_probe_req(__le16 fc)
{
	return (fc & cpu_to_le16(WIRELESS_80211_FCTL_FTYPE | WIRELESS_80211_FCTL_STYPE)) ==
	       cpu_to_le16(WIRELESS_80211_FTYPE_MGMT | WIRELESS_80211_STYPE_PROBE_REQ);
}

/**
 * wireless_80211_is_data - check if type is WIRELESS_80211_FTYPE_DATA
 * @fc: frame control bytes in little-endian byteorder
 */
static inline bool wireless_80211_is_data(__le16 fc)
{
	return (fc & cpu_to_le16(WIRELESS_80211_FCTL_FTYPE)) ==
	       cpu_to_le16(WIRELESS_80211_FTYPE_DATA);
}

/**
 * wireless_80211_is_data_qos - check if type is WIRELESS_80211_FTYPE_DATA and WIRELESS_80211_STYPE_QOS_DATA is set
 * @fc: frame control bytes in little-endian byteorder
 */
static inline bool wireless_80211_is_data_qos(__le16 fc)
{
	/*
	 * mask with QOS_DATA rather than WIRELESS_80211_FCTL_STYPE as we just need
	 * to check the one bit
	 */
	return (fc & cpu_to_le16(WIRELESS_80211_FCTL_FTYPE | WIRELESS_80211_STYPE_QOS_DATA)) ==
	       cpu_to_le16(WIRELESS_80211_FTYPE_DATA | WIRELESS_80211_STYPE_QOS_DATA);
}

#endif
