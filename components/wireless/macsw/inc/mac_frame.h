/**
 ****************************************************************************************
 *
 * @file mac_frame.h
 *
 * @brief Main Frame Formation and Reception definitions.
 *
 *
 ****************************************************************************************
 */

#ifndef _MAC_FRAME_H_
#define _MAC_FRAME_H_

/**
 ****************************************************************************************
 * @addtogroup MAC
 * @details
 * - Definitions for various fields found in 802.11 frames
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include <stdint.h>

#include "macsw_config.h"

#include "mac_types.h"

/**
 * @name Various Field Length definitions
 * @{
 ****************************************************************************************
 */
/// WEP security header size (IV + key IDX)
#define WEP_HDR_LEN (4)
/// WEP security tail size (ICV)
#define WEP_TAIL_LEN (4)

/// TKIP security header size (TKIP Sequence Counter + Key IDX)
#define TKIP_HDR_LEN (8)
/// TKIP MIC len
#define TKIP_MIC_LEN (8)
/// TKIP security tail size (MIC + ICV)
#define TKIP_TAIL_LEN (12)

/// CCMP security header size (Packet Number + Key IDX)
#define CCMP_HDR_LEN (8)
/// CCMP security tail size (MIC)
#define CCMP_TAIL_LEN (8)

/// CCMP-256 security header size (Packet Number + Key IDX)
#define CCMP_256_HDR_LEN (8)
/// CCMP-256 security tail size (MIC)
#define CCMP_256_TAIL_LEN (16)

/// GCMP (128/256) security header size (Packet Number + Key IDX)
#define GCMP_HDR_LEN (8)
/// GCMP (128/256) security tail size (MIC)
#define GCMP_TAIL_LEN (16)

/// WPI security header size (Packet Number + Key IDX)
#define WPI_HDR_LEN (18)
/// security tail size (MIC)
#define WPI_TAIL_LEN (16)

#if MACSW_WAPI_EN
#define SEC_MAX_HDR_LEN   WPI_HDR_LEN
#define SEC_MAX_TAIL_LEN  WPI_TAIL_LEN
#elif MACSW_MAC_HE
#define SEC_MAX_HDR_LEN   GCMP_HDR_LEN
#define SEC_MAX_TAIL_LEN  GCMP_TAIL_LEN
#else
#define SEC_MAX_HDR_LEN   TKIP_HDR_LEN
#define SEC_MAX_TAIL_LEN  TKIP_TAIL_LEN
#endif

/// Length of the FCS field
#define MAC_FCS_LEN     4
/** @} */

/**
 * @name MAC Header Length definitions
 * @{
 ****************************************************************************************
 */
/// Long MAC Header length (with QoS control field and HT control field)
#define MAC_LONG_QOS_HTC_MAC_HDR_LEN   36
/// Long MAC Header length (with QoS control field)
#define MAC_LONG_QOS_MAC_HDR_LEN       32
/// Long MAC Header length (without QoS control field)
#define MAC_LONG_MAC_HDR_LEN           30
/// Short MAC Header length (with QoS control field and HT control field)
#define MAC_SHORT_QOS_HTC_MAC_HDR_LEN  30
/// Short MAC Header length (with QoS control field)
#define MAC_SHORT_QOS_MAC_HDR_LEN      26
/// Short MAC Header length (without QoS control field)
#define MAC_SHORT_MAC_HDR_LEN          24

/// QoS Control Field Length
#define MAC_HDR_QOS_CTRL_LEN           2

/// Longest possible MAC Header
#define MAC_HDR_LEN_MAX                MAC_LONG_QOS_HTC_MAC_HDR_LEN

/// Long control frame header length
#define MAC_LONG_CTRLFRAME_LEN         16
/// Short control frame header length (ACK/CTS)
#define MAC_SHORT_CTRLFRAME_LEN        10

/** @} */

/**
 * @name MAC Header offset definitions
 * @{
 ****************************************************************************************
 */
/// 802.11 MAC header definition
#define MAC_HEAD_FCTRL_OFT              0
#define MAC_HEAD_DURATION_OFT           2
#define MAC_HEAD_DURATION_CFP      0x8000
#define MAC_HEAD_ADDR1_OFT              4
#define MAC_HEAD_ADDR2_OFT             10
#define MAC_HEAD_ADDR3_OFT             16
#define MAC_HEAD_CTRL_OFT              22
#define MAC_HEAD_ADDR4_OFT             24
#define MAC_HEAD_SHORT_QOS_OFT         24
#define MAC_HEAD_LONG_QOS_OFT          30
#define MAC_ORIGINAL_ETHTYPE_OFT       36
/** @} */

/**
 * @name Frame Control bit field definitions
 * @{
 ****************************************************************************************
 */
/* The type information in the header of a frame consists of the Type and Subtype fields
 * When using the term "frame type" in the code, we refer to the type field and not to
 * the combined type+subtype information.
 */
/// 802.11 frame control definition
#define MAC_FCTRL_LEN                   2

#define MAC_FCTRL_PROTOCOLVERSION_MASK  0x0003
#define MAC_FCTRL_TYPE_MASK             0x000C
#define MAC_FCTRL_SUBT_MASK             0x00F0
#define MAC_FCTRL_TODS                  0x0100
#define MAC_FCTRL_FROMDS                0x0200
#define MAC_FCTRL_MOREFRAG              0x0400
#define MAC_FCTRL_RETRY                 0x0800
#define MAC_FCTRL_PWRMGT                0x1000
#define MAC_FCTRL_MOREDATA              0x2000
#define MAC_FCTRL_PROTECTEDFRAME        0x4000
#define MAC_FCTRL_ORDER                 0x8000

#define MAC_FCTRL_TODS_FROMDS          (MAC_FCTRL_TODS | MAC_FCTRL_FROMDS)
/** @} */

/**
 * @name Frame Control Type definitions
 * @{
 ****************************************************************************************
 */
/// 802.11 type definition
#define MAC_FCTRL_MGT_T                 0x0000
#define MAC_FCTRL_CTRL_T                0x0004
#define MAC_FCTRL_DATA_T                0x0008
/** @} */

/**
 * @name Frame Control Subtype definitions
 * @{
 ****************************************************************************************
 */
/// 802.11 subtype definition
// Management SubType
#define MAC_FCTRL_ASSOCREQ_ST           0x0000
#define MAC_FCTRL_ASSOCRSP_ST           0x0010
#define MAC_FCTRL_REASSOCREQ_ST         0x0020
#define MAC_FCTRL_REASSOCRSP_ST         0x0030
#define MAC_FCTRL_PROBEREQ_ST           0x0040
#define MAC_FCTRL_PROBERSP_ST           0x0050
#define MAC_FCTRL_BEACON_ST             0x0080
#define MAC_FCTRL_ATIM_ST               0x0090
#define MAC_FCTRL_DISASSOC_ST           0x00A0
#define MAC_FCTRL_AUTHENT_ST            0x00B0
#define MAC_FCTRL_DEAUTHENT_ST          0x00C0
#define MAC_FCTRL_ACTION_ST             0x00D0
#define MAC_FCTRL_ACTION_NO_ACK_ST      0x00E0
// Control SubTypes
#define MAC_FCTRL_HE_TRIGGER_ST         0x0020
#define MAC_FCTRL_BFM_REPORT_POLL_ST    0x0040
#define MAC_FCTRL_VHT_NDPA_ST           0x0050
#define MAC_FCTRL_CTRL_WRAPPER_ST       0x0070
#define MAC_FCTRL_BAR_ST                0x0080
#define MAC_FCTRL_BA_ST                 0x0090
#define MAC_FCTRL_PSPOLL_ST             0x00A0
#define MAC_FCTRL_RTS_ST                0x00B0
#define MAC_FCTRL_CTS_ST                0x00C0
#define MAC_FCTRL_ACK_ST                0x00D0
#define MAC_FCTRL_CFEND_ST              0x00E0
#define MAC_FCTRL_CFEND_CFACK_ST        0x00F0

// Data SubTypes
/* Decoding the subtypes of data type frames can take advantage of the fact that
 * each subtype field bit position is used to indicate a specific modification of
 * the basic data frame (subtype 0). Frame control bit 4 is set to 1 in data
 * subtypes which include +CF-Ack, bit 5 is set to 1 in data subtypes which include
 * +CF-Poll, bit 6 is set to 1 in data subtypes that contain no Frame Body,
 * and bit 7 is set to 1 in the QoS data subtypes, which have QoS Control fields in
 * their MAC headers.
 *
 * Usage: check FrameT and FrameSubT individually. If the FrameT is MAC_FCTRL_DATA_T,
 * check the following bits of the FrameSubT
 */
#define MAC_CFACK_ST_BIT                CO_BIT(4)
#define MAC_CFPOLL_ST_BIT               CO_BIT(5)
#define MAC_NODATA_ST_BIT               CO_BIT(6)
#define MAC_QOS_ST_BIT                  CO_BIT(7)
#define MAC_FCTRL_DATACFACKPOLL_ST      (MAC_CFACK_ST_BIT | MAC_CFPOLL_ST_BIT)
/** @} */

/**
 * @name Frame Control various frame type/subtype definitions
 * @{
 ****************************************************************************************
 */
/// 802.11 type/subtype definition
#define MAC_FCTRL_TYPESUBTYPE_MASK      (MAC_FCTRL_TYPE_MASK | MAC_FCTRL_SUBT_MASK)
#define MAC_FCTRL_ASSOCREQ              (MAC_FCTRL_MGT_T     | MAC_FCTRL_ASSOCREQ_ST)
#define MAC_FCTRL_ASSOCRSP              (MAC_FCTRL_MGT_T     | MAC_FCTRL_ASSOCRSP_ST)
#define MAC_FCTRL_REASSOCREQ            (MAC_FCTRL_MGT_T     | MAC_FCTRL_REASSOCREQ_ST)
#define MAC_FCTRL_REASSOCRSP            (MAC_FCTRL_MGT_T     | MAC_FCTRL_REASSOCRSP_ST)
#define MAC_FCTRL_PROBEREQ              (MAC_FCTRL_MGT_T     | MAC_FCTRL_PROBEREQ_ST)
#define MAC_FCTRL_PROBERSP              (MAC_FCTRL_MGT_T     | MAC_FCTRL_PROBERSP_ST)
#define MAC_FCTRL_BEACON                (MAC_FCTRL_MGT_T     | MAC_FCTRL_BEACON_ST)
#define MAC_FCTRL_ATIM                  (MAC_FCTRL_MGT_T     | MAC_FCTRL_ATIM_ST)
#define MAC_FCTRL_DISASSOC              (MAC_FCTRL_MGT_T     | MAC_FCTRL_DISASSOC_ST)
#define MAC_FCTRL_AUTHENT               (MAC_FCTRL_MGT_T     | MAC_FCTRL_AUTHENT_ST)
#define MAC_FCTRL_DEAUTHENT             (MAC_FCTRL_MGT_T     | MAC_FCTRL_DEAUTHENT_ST)
#define MAC_FCTRL_ACTION                (MAC_FCTRL_MGT_T     | MAC_FCTRL_ACTION_ST)
#define MAC_FCTRL_ACTION_NO_ACK         (MAC_FCTRL_MGT_T     | MAC_FCTRL_ACTION_NO_ACK_ST)
#define MAC_FCTRL_BFM_REPORT_POLL       (MAC_FCTRL_CTRL_T    | MAC_FCTRL_BFM_REPORT_POLL_ST)
#define MAC_FCTRL_HE_TRIGGER            (MAC_FCTRL_CTRL_T    | MAC_FCTRL_HE_TRIGGER_ST)
#define MAC_FCTRL_VHT_NDPA              (MAC_FCTRL_CTRL_T    | MAC_FCTRL_VHT_NDPA_ST)
#define MAC_FCTRL_BA                    (MAC_FCTRL_CTRL_T    | MAC_FCTRL_BA_ST)
#define MAC_FCTRL_BAR                   (MAC_FCTRL_CTRL_T    | MAC_FCTRL_BAR_ST)
#define MAC_FCTRL_PSPOLL                (MAC_FCTRL_CTRL_T    | MAC_FCTRL_PSPOLL_ST)
#define MAC_FCTRL_RTS                   (MAC_FCTRL_CTRL_T    | MAC_FCTRL_RTS_ST)
#define MAC_FCTRL_CTS                   (MAC_FCTRL_CTRL_T    | MAC_FCTRL_CTS_ST)
#define MAC_FCTRL_ACK                   (MAC_FCTRL_CTRL_T    | MAC_FCTRL_ACK_ST)
#define MAC_FCTRL_CFEND                 (MAC_FCTRL_CTRL_T    | MAC_FCTRL_CFEND_ST)
#define MAC_FCTRL_CFEND_CFACK           (MAC_FCTRL_CFEND     | MAC_CFACK_ST_BIT)
#define MAC_FCTRL_DATA_CFACK            (MAC_FCTRL_DATA_T    | MAC_CFACK_ST_BIT)
#define MAC_FCTRL_DATA_CFPOLL           (MAC_FCTRL_DATA_T    | MAC_CFPOLL_ST_BIT)
#define MAC_FCTRL_DATA_CFACKPOLL        (MAC_FCTRL_DATA_T    | MAC_FCTRL_DATACFACKPOLL_ST)
#define MAC_FCTRL_NULL_FUNCTION         (MAC_FCTRL_DATA_T    | MAC_NODATA_ST_BIT)
#define MAC_FCTRL_NULL_CFACK            (MAC_FCTRL_NULL_FUNCTION  | MAC_CFACK_ST_BIT)
#define MAC_FCTRL_NULL_CFPOLL           (MAC_FCTRL_NULL_FUNCTION  | MAC_CFPOLL_ST_BIT)
#define MAC_FCTRL_NULL_CFACKPOLL        (MAC_FCTRL_NULL_FUNCTION  | MAC_FCTRL_DATACFACKPOLL_ST)
#define MAC_FCTRL_QOS_DATA              (MAC_FCTRL_DATA_T    | MAC_QOS_ST_BIT)
#define MAC_FCTRL_QOS_DATA_CFACK        (MAC_FCTRL_QOS_DATA  | MAC_CFACK_ST_BIT)
#define MAC_FCTRL_QOS_DATA_CFPOLL       (MAC_FCTRL_QOS_DATA  | MAC_CFPOLL_ST_BIT)
#define MAC_FCTRL_QOS_DATA_CFACKPOLL    (MAC_FCTRL_QOS_DATA  | MAC_FCTRL_DATACFACKPOLL_ST)
#define MAC_FCTRL_QOS_NULL              (MAC_FCTRL_QOS_DATA  | MAC_NODATA_ST_BIT)
#define MAC_FCTRL_QOS_NULL_CFACK        (MAC_FCTRL_QOS_DATA  | MAC_FCTRL_NULL_CFACK)
#define MAC_FCTRL_QOS_NULL_CFPOLL       (MAC_FCTRL_QOS_DATA  | MAC_FCTRL_NULL_CFPOLL)
#define MAC_FCTRL_QOS_NULL_CFACKPOLL    (MAC_FCTRL_QOS_DATA  | MAC_FCTRL_NULL_CFACKPOLL)

#define MAC_FCTRL_IS(fc, type) (((fc) & MAC_FCTRL_TYPESUBTYPE_MASK) == MAC_FCTRL_##type)
#define MAC_FCTRL_SUBTYPE(fc) (((fc) & MAC_FCTRL_SUBT_MASK) >> 4)

/** @} */

/*
 * FIELD DESCRIPTION
 ****************************************************************************************
 */
/**
 * @name Duration/AID field definitions
 * @{
 ****************************************************************************************
 */
/// DURATION FIELD Length
#define MAC_DURATION_LEN                2

/// ASSOCIATION ID Field Mask
#define MAC_AID_MSK                     0x3FFF
/** @} */

/**
 * @name Sequence Control definitions
 * @{
 ****************************************************************************************
 */
/// 802.11 Sequence Control definition
#define MAC_SEQCTRL_LEN                 2
#define MAC_SEQCTRL_NUM_OFT             4
#define MAC_SEQCTRL_NUM_MSK             0xFFF0
#define MAC_SEQCTRL_NUM_MAX             (MAC_SEQCTRL_NUM_MSK >> MAC_SEQCTRL_NUM_OFT)
#define MAC_SEQCTRL_FRAG_OFT            0
#define MAC_SEQCTRL_FRAG_MSK            0x000F
/** @} */

/**
 * @name Security header definitions
 * @{
 ****************************************************************************************
 */
/// 802.11 Security header definition
#define MAC_HDR_TSC_LEN          6
#define MAC_TKIP_TSC1_OFT        0
#define MAC_TKIP_TSC0_OFT        2
#define MAC_TKIP_KEYID_OFT       3
#define MAC_CCMP_PN0_OFT         0
#define MAC_CCMP_PN1_OFT         1
#define MAC_CCMP_RSV_OFT         2
#define MAC_CCMP_PN2BIT_OFT      8
#define MAC_IV_EXTIV             0x20000000
/** @} */

/**
 * @name QoS Control definitions
 * @{
 ****************************************************************************************
 */
/// 802.11 QoS control definition
#define MAC_QOSCTRL_LEN                 2
#define MAC_QOSCTRL_UP_OFT              0
#define MAC_QOSCTRL_UP_MSK              0x0007
#define MAC_QOSCTRL_EOSP_OFT            4
#define MAC_QOSCTRL_EOSP                0x10
#define MAC_QOSCTRL_QUEUE_SIZE_PRESENT  0x10
#define MAC_QOSCTRL_QUEUE_SIZE_OFT      8
#define MAC_QOSCTRL_QUEUE_SIZE_UNKNOWN  (0xFF << MAC_QOSCTRL_QUEUE_SIZE_OFT)
#if MACSW_WFA
#define MAC_QOSCTRL_QUEUE_SIZE_ZERO     (0x0 << MAC_QOSCTRL_QUEUE_SIZE_OFT)
#define MAC_QOSCTRL_HE_QUEUE_SIZE       (MAC_QOSCTRL_QUEUE_SIZE_PRESENT |                \
                                         MAC_QOSCTRL_QUEUE_SIZE_ZERO)
#else
#define MAC_QOSCTRL_HE_QUEUE_SIZE       (MAC_QOSCTRL_QUEUE_SIZE_PRESENT |                \
                                         MAC_QOSCTRL_QUEUE_SIZE_UNKNOWN)
#endif
#define MAC_QOSCTRL_ACK_OFT             5
#define MAC_QOSCTRL_ACK_NORMAL          0x0000
#define MAC_QOSCTRL_ACK_NOACK           0x0020
#define MAC_QOSCTRL_ACK_NOEXPL          0x0040
#define MAC_QOSCTRL_ACK_BLOCK           0x0060
#define MAC_QOSCTRL_AMSDU_OFT           7
#define MAC_QOSCTRL_AMSDU_PRESENT       CO_BIT(7)
#define MAC_QOSCTRL_MESH_CTRL_PRESENT   CO_BIT(8)
#define MAC_QOSCTRL_MESH_POWER_SAVE_LVL CO_BIT(9)
#define MAC_QOSCTRL_MESH_RSPI           CO_BIT(10)
/** @} */


/**
 * @name HT Control definitions
 * @{
 ****************************************************************************************
 */
/// HT CONTROL FIELD
#define MAC_HTC_LEN                     4
#define MAC_HTC_TYPE_OFT                0
#define MAC_HTC_TYPE_HT_MSK             (0x01 << MAC_HTC_TYPE_OFT)
#define MAC_HTC_TYPE_HT                 (0x00 << MAC_HTC_TYPE_OFT)
#define MAC_HTC_TYPE_VHT_HE_MSK         (0x03 << MAC_HTC_TYPE_OFT)
#define MAC_HTC_TYPE_VHT                (0x01 << MAC_HTC_TYPE_OFT)
#define MAC_HTC_TYPE_HE                 (0x03 << MAC_HTC_TYPE_OFT)
#define MAC_HTC_HE_A_CONTROL_OFT        2
#define MAC_HTC_HE_A_CONTROL_MSK        (0x3FFFFFFF << MAC_HTC_HE_A_CONTROL_OFT)

/** @} */


/**
 * @name +HTC HE Variant definitions
 * @{
 ****************************************************************************************
 */
/// +HTC HE VARIANT FIELD
#define MAC_HTC_HE_CTRL_ID_OFT          0
#define MAC_HTC_HE_CTRL_ID_MSK          (0x0F << MAC_HTC_HE_CTRL_ID_OFT)
#define MAC_HTC_HE_CTRL_ID_TRS          (0x00 << MAC_HTC_HE_CTRL_ID_OFT)
#define MAC_HTC_HE_CTRL_ID_OM           (0x01 << MAC_HTC_HE_CTRL_ID_OFT)
#define MAC_HTC_HE_CTRL_ID_HLA          (0x02 << MAC_HTC_HE_CTRL_ID_OFT)
#define MAC_HTC_HE_CTRL_ID_BSR          (0x03 << MAC_HTC_HE_CTRL_ID_OFT)
#define MAC_HTC_HE_CTRL_ID_UPH          (0x04 << MAC_HTC_HE_CTRL_ID_OFT)
#define MAC_HTC_HE_CTRL_ID_BQR          (0x05 << MAC_HTC_HE_CTRL_ID_OFT)
#define MAC_HTC_HE_CTRL_ID_CAS          (0x06 << MAC_HTC_HE_CTRL_ID_OFT)
#define MAC_HTC_HE_CTRL_ID_ONES         (0x0F << MAC_HTC_HE_CTRL_ID_OFT)
#define MAC_HTC_HE_CTRL_INFO_OFT        4
#define MAC_HTC_HE_CTRL_INFO_MSK        (0x0F << MAC_HTC_HE_CTRL_ID_OFT)
#define MAC_HTC_HE_BSR_ACI_BMP_OFT      MAC_HTC_HE_CTRL_INFO_OFT
#define MAC_HTC_HE_BSR_ACI_BMP_MSK      (0x0F << MAC_HTC_HE_BSR_ACI_BMP_OFT)
#define MAC_HTC_HE_BSR_DELTA_TID_OFT    (MAC_HTC_HE_CTRL_INFO_OFT + 4)
#define MAC_HTC_HE_BSR_DELTA_TID_MSK    (0x03 << MAC_HTC_HE_BSR_DELTA_TID_OFT)
#define MAC_HTC_HE_BSR_ACI_HIGH_OFT     (MAC_HTC_HE_CTRL_INFO_OFT + 6)
#define MAC_HTC_HE_BSR_ACI_HIGH_MSK     (0x03 << MAC_HTC_HE_BSR_ACI_HIGH_OFT)
#define MAC_HTC_HE_BSR_SCALING_FAC_OFT  (MAC_HTC_HE_CTRL_INFO_OFT + 8)
#define MAC_HTC_HE_BSR_SCALING_FAC_MSK  (0x03 << MAC_HTC_HE_BSR_SCALING_FAC_OFT)
#define MAC_HTC_HE_BSR_QSIZE_HIGH_OFT   (MAC_HTC_HE_CTRL_INFO_OFT + 10)
#define MAC_HTC_HE_BSR_QSIZE_HIGH_MSK   (0xFF << MAC_HTC_HE_BSR_QSIZE_HIGH_OFT)
#define MAC_HTC_HE_BSR_QSIZE_ALL_OFT    (MAC_HTC_HE_CTRL_INFO_OFT + 18)
#define MAC_HTC_HE_BSR_QSIZE_ALL_MSK    (0xFF << MAC_HTC_HE_BSR_QSIZE_ALL_OFT)

/** @} */

/**
 * @name +HTC HE Variant building macros
 * @{
 ****************************************************************************************
 */
/**
 ****************************************************************************************
 * Format a control subfield for a +HTC HE
 *
 * The macro gets the control id and control information as parameter and group them to
 * form the corresponding control subfield.
 *
 * @param[in] id Control ID (e.g. UPH, BSR, OM, etc.)
 * @param[in] info Control information
 *
 * @return The formatted control subfield
 ****************************************************************************************
 */
#define MAC_HTC_HE_CTRL(id, info) (MAC_HTC_HE_CTRL_ID_##id | ((info) << MAC_HTC_HE_CTRL_INFO_OFT))

/**
 ****************************************************************************************
 * Format a +HTC HE Variant field
 *
 * The macro gets the A-Control subfield and format the corresponding +HTC HE field.
 *
 * @param[in] a_control A-Control subfield
 *
 * @return The formatted +HTC HE Variant field
 ****************************************************************************************
 */
#define MAC_HTC_HE(a_control) (MAC_HTC_TYPE_HE | ((a_control) << MAC_HTC_HE_A_CONTROL_OFT))

/** @} */

/**
 * @name Capability Information definitions
 * @{
 ****************************************************************************************
 */
/// 802.11 Capability definition
// CAPABILITY INFORMATION FIELD
#define MAC_CAPA_ESS                    CO_BIT(0)
#define MAC_CAPA_IBSS                   CO_BIT(1)
#define MAC_CAPA_CFPOLL                 CO_BIT(2)
#define MAC_CAPA_CFREQ                  CO_BIT(3)
#define MAC_CAPA_PRIVA                  CO_BIT(4)
#define MAC_CAPA_SHORT_PREAMBLE         CO_BIT(5)
#define MAC_CAPA_PBCB                   CO_BIT(6)
#define MAC_CAPA_AGILITY                CO_BIT(7)
#define MAC_CAPA_SPECTRUM               CO_BIT(8)
#define MAC_CAPA_QOS                    CO_BIT(9)
#define MAC_CAPA_SHORT_SLOT             CO_BIT(10)
//#define MAC_CAPA_ROBUST_SECURITY       CO_BIT(11)
#define MAC_CAPA_APSD                   CO_BIT(11)
#define MAC_CAPA_RADIO_MEASUREMENT      CO_BIT(12)
#define MAC_CAPA_DSSS_OFDM              CO_BIT(13)
#define MAC_CAPA_DELAYED_BA             CO_BIT(14)
#define MAC_CAPA_IMMEDIATE_BA           CO_BIT(15)
#define MAC_CAPA_BA_POLICY              (MAC_CAPA_DELAYED_BA | MAC_CAPA_IMMEDIATE_BA)
/** @} */

/**
 * @name HT Capability Information Element definitions
 * @{
 ****************************************************************************************
 */
/// 802.11 HT capability definition
#define MAC_HTCAPA_LDPC                 CO_BIT(0)
#define MAC_HTCAPA_40_MHZ               CO_BIT(1)
#define MAC_HTCAPA_SMPS_OFT             2
#define MAC_HTCAPA_SMPS_MSK             (0x03 << MAC_HTCAPA_SMPS_OFT)
#define MAC_HTCAPA_SMPS_STATIC          (0x00 << MAC_HTCAPA_SMPS_OFT)
#define MAC_HTCAPA_SMPS_DYNAMIC         (0x01 << MAC_HTCAPA_SMPS_OFT)
#define MAC_HTCAPA_SMPS_DISABLE         (0x03 << MAC_HTCAPA_SMPS_OFT)
#define MAC_HTCAPA_GREEN_FIELD          CO_BIT(4)
#define MAC_HTCAPA_SHORTGI_20           CO_BIT(5)
#define MAC_HTCAPA_SHORTGI_40           CO_BIT(6)
#define MAC_HTCAPA_TX_STBC              CO_BIT(7)
#define MAC_HTCAPA_RX_STBC_OFT          8
#define MAC_HTCAPA_RX_STBC_MSK          (0x03 << MAC_HTCAPA_RX_STBC_OFT)
#define MAC_HTCAPA_AMSDU                CO_BIT(11)
#define MAC_HTCAPA_DSSS_CCK_40          CO_BIT(12)
#define MAC_HTCAPA_40_INTOLERANT        CO_BIT(14)
#define MAC_HTCAPA_LSIG                 CO_BIT(15)

#define MAC_AMPDU_LEN_EXP_OFT           0
#define MAC_AMPDU_LEN_EXP_MSK           (0x03 << MAC_AMPDU_LEN_EXP_OFT)
#define MAC_AMPDU_MIN_SPACING_OFT       2
#define MAC_AMPDU_MIN_SPACING_MSK       (0x07 << MAC_AMPDU_MIN_SPACING_OFT)

#define MAC_HT_MAX_AMPDU_FACTOR         13

#define MAC_HTXCAPA_MFB_UNSOLICIT       CO_BIT(9)
/** @} */

/**
 * @name VHT Capability Information Element definitions
 * @{
 ****************************************************************************************
 */
/// 802.11 VHT capability definition
#define MAC_VHTCAPA_MAX_MPDU_LENGTH_OFT        0
#define MAC_VHTCAPA_MAX_MPDU_LENGTH_MSK        (0x03 << MAC_VHTCAPA_MAX_MPDU_LENGTH_OFT)
#define MAC_VHTCAPA_MAX_MPDU_LENGTH_3895       (0x00 << MAC_VHTCAPA_MAX_MPDU_LENGTH_OFT)
#define MAC_VHTCAPA_MAX_MPDU_LENGTH_7991       (0x01 << MAC_VHTCAPA_MAX_MPDU_LENGTH_OFT)
#define MAC_VHTCAPA_MAX_MPDU_LENGTH_11454      (0x02 << MAC_VHTCAPA_MAX_MPDU_LENGTH_OFT)
#define MAC_VHTCAPA_SUPP_CHAN_WIDTH_OFT        2
#define MAC_VHTCAPA_SUPP_CHAN_WIDTH_MSK        (0x03 << MAC_VHTCAPA_SUPP_CHAN_WIDTH_OFT)
#define MAC_VHTCAPA_SUPP_CHAN_WIDTH_80         (0x00 << MAC_VHTCAPA_SUPP_CHAN_WIDTH_OFT)
#define MAC_VHTCAPA_SUPP_CHAN_WIDTH_160        (0x01 << MAC_VHTCAPA_SUPP_CHAN_WIDTH_OFT)
#define MAC_VHTCAPA_SUPP_CHAN_WIDTH_160_80P80  (0x02 << MAC_VHTCAPA_SUPP_CHAN_WIDTH_OFT)
#define MAC_VHTCAPA_RXLDPC                     CO_BIT(4)
#define MAC_VHTCAPA_SHORT_GI_80                CO_BIT(5)
#define MAC_VHTCAPA_SHORT_GI_160               CO_BIT(6)
#define MAC_VHTCAPA_TXSTBC                     CO_BIT(7)
#define MAC_VHTCAPA_RXSTBC_OFT                 8
#define MAC_VHTCAPA_RXSTBC_MSK                 (0x07 << MAC_VHTCAPA_RXSTBC_OFT)
#define MAC_VHTCAPA_RXSTBC_1                   (0x01 << MAC_VHTCAPA_RXSTBC_OFT)
#define MAC_VHTCAPA_RXSTBC_2                   (0x02 << MAC_VHTCAPA_RXSTBC_OFT)
#define MAC_VHTCAPA_RXSTBC_3                   (0x03 << MAC_VHTCAPA_RXSTBC_OFT)
#define MAC_VHTCAPA_RXSTBC_4                   (0x04 << MAC_VHTCAPA_RXSTBC_OFT)
#define MAC_VHTCAPA_SU_BEAMFORMER_CAPABLE      CO_BIT(11)
#define MAC_VHTCAPA_SU_BEAMFORMEE_CAPABLE      CO_BIT(12)
#define MAC_VHTCAPA_BEAMFORMEE_STS_OFT         13
#define MAC_VHTCAPA_BEAMFORMEE_STS_MSK         (0x07 << MAC_VHTCAPA_BEAMFORMEE_STS_OFT)
#define MAC_VHTCAPA_SOUNDING_DIMENSIONS_OFT    16
#define MAC_VHTCAPA_SOUNDING_DIMENSIONS_MSK    (0x07 << MAC_VHTCAPA_SOUNDING_DIMENSIONS_OFT)
#define MAC_VHTCAPA_MU_BEAMFORMER_CAPABLE      CO_BIT(19)
#define MAC_VHTCAPA_MU_BEAMFORMEE_CAPABLE      CO_BIT(20)
#define MAC_VHTCAPA_VHT_TXOP_PS                CO_BIT(21)
#define MAC_VHTCAPA_HTC_VHT                    CO_BIT(22)
#define MAC_VHTCAPA_MAX_A_MPDU_LENGTH_EXP_OFT  23
#define MAC_VHTCAPA_MAX_A_MPDU_LENGTH_EXP_MSK  (0x07 << MAC_VHTCAPA_MAX_A_MPDU_LENGTH_EXP_OFT)
#define MAC_VHTCAPA_VHT_LA_VHT_UNSOL_MFB       0x08000000
#define MAC_VHTCAPA_VHT_LA_VHT_MRQ_MFB         0x0c000000
#define MAC_VHTCAPA_RX_ANTENNA_PATTERN         CO_BIT(28)
#define MAC_VHTCAPA_TX_ANTENNA_PATTERN         CO_BIT(29)

#define MAC_VHT_MCS_MAP_MSK                    0x03
#define MAC_VHT_MCS_MAP_0_7                    0x00
#define MAC_VHT_MCS_MAP_0_8                    0x01
#define MAC_VHT_MCS_MAP_0_9                    0x02
#define MAC_VHT_MCS_MAP_NONE                   0x03
/** @} */

/**
 * @name HE MAC Capability Information Element definitions
 * @{
 ****************************************************************************************
 */
/// 802.11 HE MAC capability definition
#define HE_MAC_CAPA_HTC_HE_POS                             0
#define HE_MAC_CAPA_TWT_REQ_POS                            1
#define HE_MAC_CAPA_TWT_RES_POS                            2

#define HE_MAC_CAPA_DYNAMIC_FRAG_OFT                       3
#define HE_MAC_CAPA_DYNAMIC_FRAG_WIDTH                     2
#define HE_MAC_CAPA_DYNAMIC_FRAG_NOT_SUPP                         0x00
#define HE_MAC_CAPA_DYNAMIC_FRAG_LEVEL_1                          0x01
#define HE_MAC_CAPA_DYNAMIC_FRAG_LEVEL_2                          0x02
#define HE_MAC_CAPA_DYNAMIC_FRAG_LEVEL_3                          0x03

#define HE_MAC_CAPA_MAX_NUM_FRAG_MSDU_OFT                  5
#define HE_MAC_CAPA_MAX_NUM_FRAG_MSDU_WIDTH                3
#define HE_MAC_CAPA_MAX_NUM_FRAG_MSDU_1                           0x00
#define HE_MAC_CAPA_MAX_NUM_FRAG_MSDU_2                           0x01
#define HE_MAC_CAPA_MAX_NUM_FRAG_MSDU_4                           0x02
#define HE_MAC_CAPA_MAX_NUM_FRAG_MSDU_8                           0x03
#define HE_MAC_CAPA_MAX_NUM_FRAG_MSDU_16                          0x04
#define HE_MAC_CAPA_MAX_NUM_FRAG_MSDU_32                          0x05
#define HE_MAC_CAPA_MAX_NUM_FRAG_MSDU_64                          0x06
#define HE_MAC_CAPA_MAX_NUM_FRAG_MSDU_UNLIMITED                   0x07

#define HE_MAC_CAPA_MIN_FRAG_SIZE_OFT                      8
#define HE_MAC_CAPA_MIN_FRAG_SIZE_WIDTH                    2
#define HE_MAC_CAPA_MIN_FRAG_SIZE_UNLIMITED                       0x00
#define HE_MAC_CAPA_MIN_FRAG_SIZE_128                             0x01
#define HE_MAC_CAPA_MIN_FRAG_SIZE_256                             0x02
#define HE_MAC_CAPA_MIN_FRAG_SIZE_512                             0x03

#define HE_MAC_CAPA_TF_MAC_PAD_DUR_OFT                     10
#define HE_MAC_CAPA_TF_MAC_PAD_DUR_WIDTH                   2
#define HE_MAC_CAPA_TF_MAC_PAD_DUR_0US                            0x00
#define HE_MAC_CAPA_TF_MAC_PAD_DUR_8US                            0x01
#define HE_MAC_CAPA_TF_MAC_PAD_DUR_16US                           0x02

#define HE_MAC_CAPA_MULTI_TID_AGG_RX_OFT                   12
#define HE_MAC_CAPA_MULTI_TID_AGG_RX_WIDTH                 3
#define HE_MAC_CAPA_MULTI_TID_AGG_RX_1                            0x00
#define HE_MAC_CAPA_MULTI_TID_AGG_RX_2                            0x01
#define HE_MAC_CAPA_MULTI_TID_AGG_RX_3                            0x02
#define HE_MAC_CAPA_MULTI_TID_AGG_RX_4                            0x03
#define HE_MAC_CAPA_MULTI_TID_AGG_RX_5                            0x04
#define HE_MAC_CAPA_MULTI_TID_AGG_RX_6                            0x05
#define HE_MAC_CAPA_MULTI_TID_AGG_RX_7                            0x06
#define HE_MAC_CAPA_MULTI_TID_AGG_RX_8                            0x07

#define HE_MAC_CAPA_LINK_ADAPTATION_OFT                    15
#define HE_MAC_CAPA_LINK_ADAPTATION_WIDTH                  2
#define HE_MAC_CAPA_LINK_ADAPTATION_NONE                          0x00
#define HE_MAC_CAPA_LINK_ADAPTATION_RSVD                          0x01
#define HE_MAC_CAPA_LINK_ADAPTATION_UNSOL                         0x02
#define HE_MAC_CAPA_LINK_ADAPTATION_BOTH                          0x03

#define HE_MAC_CAPA_ALL_ACK_POS                            17
#define HE_MAC_CAPA_TSR_POS                                18
#define HE_MAC_CAPA_BSR_POS                                19
#define HE_MAC_CAPA_BCAST_TWT_POS                          20
#define HE_MAC_CAPA_32BIT_BA_BITMAP_POS                    21
#define HE_MAC_CAPA_MU_CASCADING_POS                       22
#define HE_MAC_CAPA_ACK_EN_POS                             23
#define HE_MAC_CAPA_OM_CONTROL_POS                         25
#define HE_MAC_CAPA_OFDMA_RA_POS                           26

#define HE_MAC_CAPA_MAX_A_AMPDU_LEN_EXP_OFT                27
#define HE_MAC_CAPA_MAX_A_AMPDU_LEN_EXP_WIDTH              2
#define HE_MAC_CAPA_MAX_A_AMPDU_LEN_EXP_USE_VHT                   0x00
#define HE_MAC_CAPA_MAX_A_AMPDU_LEN_EXP_VHT_1                     0x01
#define HE_MAC_CAPA_MAX_A_AMPDU_LEN_EXP_VHT_2                     0x02
#define HE_MAC_CAPA_MAX_A_AMPDU_LEN_EXP_RSVD                      0x03

#define HE_MAC_CAPA_A_AMSDU_FRAG_POS                       29
#define HE_MAC_CAPA_FLEX_TWT_SCHED_POS                     30
#define HE_MAC_CAPA_RX_CTRL_FRAME_TO_MULTIBSS_POS          31
#define HE_MAC_CAPA_BSRP_BQRP_A_MPDU_AGG_POS               32
#define HE_MAC_CAPA_QTP_POS                                33
#define HE_MAC_CAPA_BQR_POS                                34
#define HE_MAC_CAPA_SRP_RESP_POS                           35
#define HE_MAC_CAPA_NDP_FB_REP_POS                         36
#define HE_MAC_CAPA_OPS_POS                                37
#define HE_MAC_CAPA_AMDSU_IN_AMPDU_POS                     38

#define HE_MAC_CAPA_MULTI_TID_AGG_TX_OFT                   39
#define HE_MAC_CAPA_MULTI_TID_AGG_TX_WIDTH                 3
#define HE_MAC_CAPA_MULTI_TID_AGG_TX_1                            0x00
#define HE_MAC_CAPA_MULTI_TID_AGG_TX_2                            0x01
#define HE_MAC_CAPA_MULTI_TID_AGG_TX_3                            0x02
#define HE_MAC_CAPA_MULTI_TID_AGG_TX_4                            0x03
#define HE_MAC_CAPA_MULTI_TID_AGG_TX_5                            0x04
#define HE_MAC_CAPA_MULTI_TID_AGG_TX_6                            0x05
#define HE_MAC_CAPA_MULTI_TID_AGG_TX_7                            0x06
#define HE_MAC_CAPA_MULTI_TID_AGG_TX_8                            0x07

#define HE_MAC_CAPA_SUB_CHNL_SEL_TX_POS                    42
#define HE_MAC_CAPA_UL_2_996_TONE_RU_POS                   43
#define HE_MAC_CAPA_OM_CONTROL_UL_MU_DIS_RX_POS            44
#define HE_MAC_CAPA_DYN_SMPS_POS                           45

/** @} */

/**
 * @name HE PHY Capability Information Element definitions
 * @{
 ****************************************************************************************
 */
/// 802.11 HE PHY capability definition
#define HE_PHY_CAPA_CHAN_WIDTH_SET_OFT                     1
#define HE_PHY_CAPA_CHAN_WIDTH_SET_WIDTH                   7
#define HE_PHY_CAPA_CHAN_WIDTH_SET_40MHZ_IN_2G                    0x01
#define HE_PHY_CAPA_CHAN_WIDTH_SET_40MHZ_80MHZ_IN_5G              0x02
#define HE_PHY_CAPA_CHAN_WIDTH_SET_160MHZ_IN_5G                   0x04
#define HE_PHY_CAPA_CHAN_WIDTH_SET_80PLUS80_MHZ_IN_5G             0x08
#define HE_PHY_CAPA_CHAN_WIDTH_SET_RU_MAPPING_IN_2G               0x10
#define HE_PHY_CAPA_CHAN_WIDTH_SET_RU_MAPPING_IN_5G               0x20

#define HE_PHY_CAPA_CHAN_WIDTH_SET_40MHZ_IN_2G_POS                1
#define HE_PHY_CAPA_CHAN_WIDTH_SET_40MHZ_80MHZ_IN_5G_POS          2
#define HE_PHY_CAPA_CHAN_WIDTH_SET_160MHZ_IN_5G_POS               3
#define HE_PHY_CAPA_CHAN_WIDTH_SET_80PLUS80_MHZ_IN_5G_POS         4
#define HE_PHY_CAPA_CHAN_WIDTH_SET_RU_MAPPING_IN_2G_POS           5
#define HE_PHY_CAPA_CHAN_WIDTH_SET_RU_MAPPING_IN_5G_POS           6

#define HE_PHY_CAPA_PREAMBLE_PUNC_RX_OFT                   8
#define HE_PHY_CAPA_PREAMBLE_PUNC_RX_WIDTH                 4
#define HE_PHY_CAPA_PREAMBLE_PUNC_RX_80M_ONLY_SCND_20M            0x01
#define HE_PHY_CAPA_PREAMBLE_PUNC_RX_80M_ONLY_SCND_40M            0x02
#define HE_PHY_CAPA_PREAMBLE_PUNC_RX_160M_ONLY_SCND_20M           0x04
#define HE_PHY_CAPA_PREAMBLE_PUNC_RX_160M_ONLY_SCND_40M           0x08

#define HE_PHY_CAPA_DEVICE_CLASS_A_POS                     12
#define HE_PHY_CAPA_LDPC_CODING_IN_PAYLOAD_POS             13
#define HE_PHY_CAPA_HE_SU_PPDU_1x_LTF_AND_GI_0_8US_POS     14

#define HE_PHY_CAPA_MIDAMBLE_RX_MAX_NSTS_OFT               15
#define HE_PHY_CAPA_MIDAMBLE_RX_MAX_NSTS_WIDTH             2
#define HE_PHY_CAPA_MIDAMBLE_RX_MAX_NSTS_1_STS                    0x00
#define HE_PHY_CAPA_MIDAMBLE_RX_MAX_NSTS_2_STS                    0x01
#define HE_PHY_CAPA_MIDAMBLE_RX_MAX_NSTS_3_STS                    0x02
#define HE_PHY_CAPA_MIDAMBLE_RX_MAX_NSTS_4_STS                    0x03

#define HE_PHY_CAPA_NDP_4x_LTF_AND_3_2US_POS               17
#define HE_PHY_CAPA_STBC_TX_UNDER_80MHZ_POS                18
#define HE_PHY_CAPA_STBC_RX_UNDER_80MHZ_POS                19
#define HE_PHY_CAPA_DOPPLER_TX_POS                         20
#define HE_PHY_CAPA_DOPPLER_RX_POS                         21
#define HE_PHY_CAPA_FULL_BW_UL_MU_MIMO_POS                 22
#define HE_PHY_CAPA_PARTIAL_BW_UL_MU_MIMO_POS              23

#define HE_PHY_CAPA_DCM_MAX_CONST_TX_OFT                   24
#define HE_PHY_CAPA_DCM_MAX_CONST_TX_WIDTH                 2
#define HE_PHY_CAPA_DCM_MAX_CONST_TX_NO_DCM                       0x00
#define HE_PHY_CAPA_DCM_MAX_CONST_TX_BPSK                         0x01
#define HE_PHY_CAPA_DCM_MAX_CONST_TX_QPSK                         0x02
#define HE_PHY_CAPA_DCM_MAX_CONST_TX_16_QAM                       0x03

#define HE_PHY_CAPA_DCM_MAX_NSS_TX_POS                     26

#define HE_PHY_CAPA_DCM_MAX_CONST_RX_OFT                   27
#define HE_PHY_CAPA_DCM_MAX_CONST_RX_WIDTH                 2
#define HE_PHY_CAPA_DCM_MAX_CONST_RX_NO_DCM                       0x00
#define HE_PHY_CAPA_DCM_MAX_CONST_RX_BPSK                         0x01
#define HE_PHY_CAPA_DCM_MAX_CONST_RX_QPSK                         0x02
#define HE_PHY_CAPA_DCM_MAX_CONST_RX_16_QAM                       0x03

#define HE_PHY_CAPA_DCM_MAX_NSS_RX_POS                     29
#define HE_PHY_CAPA_RX_HE_MU_PPDU_FRM_NON_AP_POS           30
#define HE_PHY_CAPA_SU_BEAMFORMER_POS                      31
#define HE_PHY_CAPA_SU_BEAMFORMEE_POS                      32
#define HE_PHY_CAPA_MU_BEAMFORMER_POS                      33

#define HE_PHY_CAPA_BFMEE_MAX_STS_UNDER_80MHZ_OFT          34
#define HE_PHY_CAPA_BFMEE_MAX_STS_UNDER_80MHZ_WIDTH         3
#define HE_PHY_CAPA_BFMEE_MAX_STS_UNDER_80MHZ_4                   0x03
#define HE_PHY_CAPA_BFMEE_MAX_STS_UNDER_80MHZ_5                   0x04
#define HE_PHY_CAPA_BFMEE_MAX_STS_UNDER_80MHZ_6                   0x05
#define HE_PHY_CAPA_BFMEE_MAX_STS_UNDER_80MHZ_7                   0x06
#define HE_PHY_CAPA_BFMEE_MAX_STS_UNDER_80MHZ_8                   0x07

#define HE_PHY_CAPA_BFMEE_MAX_STS_ABOVE_80MHZ_OFT          37
#define HE_PHY_CAPA_BFMEE_MAX_STS_ABOVE_80MHZ_WIDTH         3
#define HE_PHY_CAPA_BFMEE_MAX_STS_ABOVE_80MHZ_4                   0x03
#define HE_PHY_CAPA_BFMEE_MAX_STS_ABOVE_80MHZ_5                   0x04
#define HE_PHY_CAPA_BFMEE_MAX_STS_ABOVE_80MHZ_6                   0x05
#define HE_PHY_CAPA_BFMEE_MAX_STS_ABOVE_80MHZ_7                   0x06
#define HE_PHY_CAPA_BFMEE_MAX_STS_ABOVE_80MHZ_8                   0x07

#define HE_PHY_CAPA_BFMEE_NUM_SND_DIM_UNDER_80MHZ_OFT      40
#define HE_PHY_CAPA_BFMEE_NUM_SND_DIM_UNDER_80MHZ_WIDTH     3
#define HE_PHY_CAPA_BFMEE_NUM_SND_DIM_UNDER_80MHZ_1               0x00
#define HE_PHY_CAPA_BFMEE_NUM_SND_DIM_UNDER_80MHZ_2               0x01
#define HE_PHY_CAPA_BFMEE_NUM_SND_DIM_UNDER_80MHZ_3               0x02
#define HE_PHY_CAPA_BFMEE_NUM_SND_DIM_UNDER_80MHZ_4               0x03
#define HE_PHY_CAPA_BFMEE_NUM_SND_DIM_UNDER_80MHZ_5               0x04
#define HE_PHY_CAPA_BFMEE_NUM_SND_DIM_UNDER_80MHZ_6               0x05
#define HE_PHY_CAPA_BFMEE_NUM_SND_DIM_UNDER_80MHZ_7               0x06
#define HE_PHY_CAPA_BFMEE_NUM_SND_DIM_UNDER_80MHZ_8               0x07

#define HE_PHY_CAPA_BFMEE_NUM_SND_DIM_ABOVE_80MHZ_OFT      43
#define HE_PHY_CAPA_BFMEE_NUM_SND_DIM_ABOVE_80MHZ_WIDTH     3
#define HE_PHY_CAPA_BFMEE_NUM_SND_DIM_ABOVE_80MHZ_1               0x00
#define HE_PHY_CAPA_BFMEE_NUM_SND_DIM_ABOVE_80MHZ_2               0x01
#define HE_PHY_CAPA_BFMEE_NUM_SND_DIM_ABOVE_80MHZ_3               0x02
#define HE_PHY_CAPA_BFMEE_NUM_SND_DIM_ABOVE_80MHZ_4               0x03
#define HE_PHY_CAPA_BFMEE_NUM_SND_DIM_ABOVE_80MHZ_5               0x04
#define HE_PHY_CAPA_BFMEE_NUM_SND_DIM_ABOVE_80MHZ_6               0x05
#define HE_PHY_CAPA_BFMEE_NUM_SND_DIM_ABOVE_80MHZ_7               0x06
#define HE_PHY_CAPA_BFMEE_NUM_SND_DIM_ABOVE_80MHZ_8               0x07

#define HE_PHY_CAPA_NG16_SU_FEEDBACK_POS                   46
#define HE_PHY_CAPA_NG16_MU_FEEDBACK_POS                   47
#define HE_PHY_CAPA_CODEBOOK_SIZE_42_SU_POS                48
#define HE_PHY_CAPA_CODEBOOK_SIZE_75_MU_POS                49
#define HE_PHY_CAPA_TRIG_SU_BEAMFORMER_FB_POS              50
#define HE_PHY_CAPA_TRIG_MU_BEAMFORMER_FB_POS              51
#define HE_PHY_CAPA_TRIG_CQI_FB_POS                        52
#define HE_PHY_CAPA_PARTIAL_BW_EXT_RANGE_POS               53
#define HE_PHY_CAPA_PARTIAL_BW_DL_MUMIMO_POS               54
#define HE_PHY_CAPA_PPE_THRESHOLD_PRESENT_POS              55
#define HE_PHY_CAPA_SRP_BASED_SR_POS                       56
#define HE_PHY_CAPA_POWER_BOOST_FACTOR_AR_POS              57
#define HE_PHY_CAPA_HE_SU_MU_PPDU_4x_LTF_AND_08_US_GI_POS  58

#define HE_PHY_CAPA_MAX_NC_OFT                             59
#define HE_PHY_CAPA_MAX_NC_WIDTH                           3
#define HE_PHY_CAPA_MAX_NC_1                                      0x01
#define HE_PHY_CAPA_MAX_NC_2                                      0x02
#define HE_PHY_CAPA_MAX_NC_3                                      0x03
#define HE_PHY_CAPA_MAX_NC_4                                      0x04
#define HE_PHY_CAPA_MAX_NC_5                                      0x05
#define HE_PHY_CAPA_MAX_NC_6                                      0x06
#define HE_PHY_CAPA_MAX_NC_7                                      0x07

#define HE_PHY_CAPA_STBC_TX_ABOVE_80MHZ_POS                62
#define HE_PHY_CAPA_STBC_RX_ABOVE_80MHZ_POS                63
#define HE_PHY_CAPA_HE_ER_SU_PPDU_4x_LTF_AND_08_US_GI_POS  64
#define HE_PHY_CAPA_20MHZ_IN_40MHZ_HE_PPDU_IN_2G_POS       65
#define HE_PHY_CAPA_20MHZ_IN_160MHZ_HE_PPDU_POS            66
#define HE_PHY_CAPA_80MHZ_IN_160MHZ_HE_PPDU_POS            67
#define HE_PHY_CAPA_HE_ER_SU_1x_LTF_AND_08_US_GI_POS       68
#define HE_PHY_CAPA_MIDAMBLE_RX_2x_AND_1x_LTF_POS          69

#define HE_PHY_CAPA_DCM_MAX_BW_OFT                         70
#define HE_PHY_CAPA_DCM_MAX_BW_WIDTH                        3
#define HE_PHY_CAPA_DCM_MAX_BW_20MHZ                              0x00
#define HE_PHY_CAPA_DCM_MAX_BW_40MHZ                              0x01
#define HE_PHY_CAPA_DCM_MAX_BW_80MHZ                              0x02
#define HE_PHY_CAPA_DCM_MAX_BW_160MHZ_80P80MHZ                    0x03

#define HE_PHY_CAPA_16PLUS_HE_SIGB_OFDM_SYM_POS            72
#define HE_PHY_CAPA_NON_TRIG_CQI_FEEDBACK_POS              73
#define HE_PHY_CAPA_TX_1024QAM_LESS_242_RU_POS             74
#define HE_PHY_CAPA_RX_1024QAM_LESS_242_RU_POS             75
#define HE_PHY_CAPA_RX_FULL_BW_SU_COMP_SIGB_POS            76
#define HE_PHY_CAPA_RX_FULL_BW_SU_NON_COMP_SIGB_POS        77

#define HE_PHY_CAPA_NOMINAL_PACKET_PADDING_OFT             78
#define HE_PHY_CAPA_NOMINAL_PACKET_PADDING_WIDTH           2
#define HE_PHY_CAPA_NOMINAL_PACKET_PADDING_0US                    0x00
#define HE_PHY_CAPA_NOMINAL_PACKET_PADDING_8US                    0x01
#define HE_PHY_CAPA_NOMINAL_PACKET_PADDING_16US                   0x02

/** @} */

/**
 * @name HE MAC Packet And Padding Extension definitions
 * @{
 ****************************************************************************************
 */
#define HE_PPE_CAPA_NSTS_OFT            0
#define HE_PPE_CAPA_NSTS_WIDTH          3

#define HE_PPE_CAPA_RU_INDEX_BITMAP_OFT   3
#define HE_PPE_CAPA_RU_INDEX_BITMAP_WIDTH 4

#define HE_PPE_CAPA_PPE_THRES_INFO_OFT   7

#define HE_PPE_CAPA_RU_242   CO_BIT(0)
#define HE_PPE_CAPA_RU_484   CO_BIT(1)
#define HE_PPE_CAPA_RU_996   CO_BIT(2)
#define HE_PPE_CAPA_RU_2x996 CO_BIT(3)

#define HE_PPE_CAPA_BPSK    0
#define HE_PPE_CAPA_QPSK    1
#define HE_PPE_CAPA_16QAM   2
#define HE_PPE_CAPA_64QAM   3
#define HE_PPE_CAPA_256QAM  4
#define HE_PPE_CAPA_1024QAM 5
#define HE_PPE_CAPA_NONE    7

/** @} */

/**
 * @name HE MCS MAP definitions
 * @{
 ****************************************************************************************
 */
/// 802.11 HE MCS map definition
#define MAC_HE_MCS_MAP_MSK                                 0x03
#define MAC_HE_MCS_MAP_0_7                                 0x00
#define MAC_HE_MCS_MAP_0_9                                 0x01
#define MAC_HE_MCS_MAP_0_11                                0x02
#define MAC_HE_MCS_MAP_NONE                                0x03
/** @} */

/**
 * @name Information Element offsets, lengths and other definitions
 * @{
 ****************************************************************************************
 */
/// 802.11 information element definition

// SSID FIELD
#define MAC_SSID_ID_OFT                         0
#define MAC_SSID_LEN_OFT                        1
#define MAC_SSID_SSID_OFT                       2
#define MAC_SSID_MAX_LEN                       (MAC_SSID_LEN + 2)

// SUPPORTED RATES FIELD
#define MAC_RATES_ID_OFT                        0
#define MAC_RATES_LEN_OFT                       1
#define MAC_RATES_RATES_OFT                     2
#define MAC_RATES_ELMT_MAX_LEN                  8
#define MAC_RATES_MAX_LEN                       10
#define MAC_RATES_MIN_LEN                       3

// FH PARAM SET FIELD
#define MAC_FH_ID_OFT                           0
#define MAC_FH_LEN_OFT                          1
#define MAC_FH_DWELL_OFT                        2
#define MAC_FH_HOP_SET_OFT                      4
#define MAC_FH_HOP_PAT_OFT                      5
#define MAC_FH_HOP_IDX_OFT                      6
#define MAC_FH_PARAM_LEN                        7

// DS PARAM SET
#define MAC_DS_ID_OFT                           0
#define MAC_DS_LEN_OFT                          1
#define MAC_DS_CHANNEL_OFT                      2
#define MAC_DS_PARAM_LEN                        3

// CF PARAM SET
#define MAC_CF_ID_OFT                           0
#define MAC_CF_LEN_OFT                          1
#define MAC_CF_COUNT_OFT                        2
#define MAC_CF_PERIOD_OFT                       3
#define MAC_CF_MAX_DUR_OFT                      4
#define MAC_CF_REM_DUR_OFT                      6
#define MAC_CF_PARAM_LEN                        8

// TIM
#define MAC_TIM_ID_OFT                          0
#define MAC_TIM_LEN_OFT                         1
#define MAC_TIM_CNT_OFT                         2
#define MAC_TIM_PERIOD_OFT                      3
#define MAC_TIM_BMPC_OFT                        4
#define MAC_TIM_BMP_OFT                         5
#define MAC_TIM_MIN_LEN                         6
#define MAC_TIM_MAX_LEN                        (5 + MAC_TIM_SIZE)
#define MAC_TIM_BCMC_PRESENT                    CO_BIT(0)

// IBSS PARAM SET
#define MAC_IBSS_ID_OFT                         0
#define MAC_IBSS_LEN_OFT                        1
#define MAC_IBSSATIM_OFT                        2
#define MAC_IBSS_PARAM_LEN                      4

// Country
#define MAC_COUNTRY_ID_OFT                      0
#define MAC_COUNTRY_LEN_OFT                     1
#define MAC_COUNTRY_MIN_LEN                     8
#define MAC_COUNTRY_STRING_OFT                  2
#define MAC_COUNTRY_STRING_LEN                  3
#define MAC_COUNTRY_FIRST_CHAN_OFT              0
#define MAC_COUNTRY_NB_CHAN_OFT                 1
#define MAC_COUNTRY_PWR_LEVEL_OFT               2
#define MAC_COUNTRY_TRIPLET_LEN                 3

// REQUEST
#define MAC_REQUEST_ID_OFT        0
#define MAC_REQUEST_LEN_OFT       1
#define MAC_REQUEST_ELTID_REQ_OFT 2

// EXTENDED REQUEST
#define MAC_EXT_REQUEST_ID_OFT         0
#define MAC_EXT_REQUEST_LEN_OFT        1
#define MAC_EXT_REQUEST_EXT_ID_OFT     2
#define MAC_EXT_REQUEST_ELT_ID_OFT     3
#define MAC_EXT_REQUEST_ELT_ID_EXT_OFT 4

// CHALLENGE PARAM SET
#define MAC_CHALLENGE_ID_OFT                    0
#define MAC_CHALLENGE_LEN_OFT                   1
#define MAC_CHALLENGE_TEXT_OFT                  2

// MEASUREMENT REQUEST
#define MAC_MEAS_REQ_ID_OFT                      0
#define MAC_MEAS_REQ_LEN_OFT                     1
#define MAC_MEAS_REQ_TOKEN_OFT                   2
#define MAC_MEAS_REQ_MODE_OFT                    3
#define MAC_MEAS_REQ_MODE_PARALLEL_BIT           CO_BIT(0)
#define MAC_MEAS_REQ_MODE_ENABLE_BIT             CO_BIT(1)
#define MAC_MEAS_REQ_MODE_REQUEST_BIT            CO_BIT(2)
#define MAC_MEAS_REQ_MODE_REPORT_BIT             CO_BIT(3)
#define MAC_MEAS_REQ_MODE_DURATION_MANDATORY_BIT CO_BIT(4)
#define MAC_MEAS_REQ_TYPE_OFT                    4
enum mac_meas_req_type {
    MAC_MEAS_REQ_TYPE_BASIC = 0,
    MAC_MEAS_REQ_TYPE_CCA = 1,
    MAC_MEAS_REQ_TYPE_RPI = 2,
    MAC_MEAS_REQ_TYPE_CHAN_LOAD = 3,
    MAC_MEAS_REQ_TYPE_NOISE_HISTO = 4,
    MAC_MEAS_REQ_TYPE_BEACON = 5,
    MAC_MEAS_REQ_TYPE_FRAME = 6,
    MAC_MEAS_REQ_TYPE_STA_STAT = 7,
    MAC_MEAS_REQ_TYPE_LCI = 8,
    MAC_MEAS_REQ_TYPE_TRANSMIT_STREAM = 9,
    MAC_MEAS_REQ_TYPE_MULTICAST_DIAG = 10,
    MAC_MEAS_REQ_TYPE_LOCATION_CIVIC = 11,
    MAC_MEAS_REQ_TYPE_LOCATION_IDENTIFIER = 12,
    MAC_MEAS_REQ_TYPE_DIRECTIONAL_CHAN_QUALITY = 13,
    MAC_MEAS_REQ_TYPE_DIRECTIONAL_MEASUREMENT = 14,
    MAC_MEAS_REQ_TYPE_DIRECTIONAL_STATS = 15,
    MAC_MEAS_REQ_TYPE_FINE_TIMING = 16,
    MAC_MEAS_REQ_TYPE_PAUSE = 255,
};
#define MAC_MEAS_REQ_REQ_OFT                     5

// MEASUREMENT REQUEST - Beacon request (offset from MAC_MEAS_REQ_REQ_OFT)
#define MAC_MEAS_REQ_BCN_OP_CLASS_OFT    0
#define MAC_MEAS_REQ_BCN_CHAN_NUMBER_OFT 1
#define MAC_MEAS_REQ_BCN_RANDOM_INTV_OFT 2
#define MAC_MEAS_REQ_BCN_DURATION_OFT    4
#define MAC_MEAS_REQ_BCN_MODE_OFT        6
enum mac_measure_req_bcn_mode {
    MAC_MEAS_REQ_BCN_PASSIVE = 0,
    MAC_MEAS_REQ_BCN_ACTIVE  = 1,
    MAC_MEAS_REQ_BCN_TABLE   = 2,
};
#define MAC_MEAS_REQ_BCN_BSSID_OFT       7
#define MAC_MEAS_REQ_BCN_SUB_ELEM_OFT    13

// MEASUREMENT REPORT
#define MAC_MEAS_REP_ID_OFT             0
#define MAC_MEAS_REP_LEN_OFT            1
#define MAC_MEAS_REP_TOKEN_OFT          2
#define MAC_MEAS_REP_MODE_OFT           3
#define MAC_MEAS_REP_MODE_LATE_BIT      CO_BIT(0)
#define MAC_MEAS_REP_MODE_INCAPABLE_BIT CO_BIT(1)
#define MAC_MEAS_REP_MODE_REFUSE_BIT    CO_BIT(2)
#define MAC_MEAS_REP_TYPE_OFT           4
#define MAC_MEAS_REP_REP_OFT            5
#define MAC_MEAS_REP_MIN_LEN            5
#define MAC_MEAS_REP_ELMT_MIN_LEN       3

// MEASUREMENT REPORT - Beacon report (offset from MAC_MEAS_REP_REP_OFT )
#define MAC_MEAS_REP_BCN_OP_CLASS_OFT    0
#define MAC_MEAS_REP_BCN_CHAN_NUMBER_OFT 1
#define MAC_MEAS_REP_BCN_START_TIME_OFT  2
#define MAC_MEAS_REP_BCN_DURATION_OFT    10
#define MAC_MEAS_REP_BCN_INFO_OFT        12
#define MAC_MEAS_REP_BCN_RCPI_OFT        13
#define MAC_MEAS_REP_BCN_RSNI_OFT        14
#define MAC_MEAS_REP_BCN_BSSID_OFT       15
#define MAC_MEAS_REP_BCN_ANTENNA_OFT     21
#define MAC_MEAS_REP_BCN_TSF_OFT         22
#define MAC_MEAS_REP_BCN_SUB_ELEM_OFT    26

// ERP
#define MAC_ERP_ID_OFT                          0
#define MAC_ERP_LEN_OFT                         1
#define MAC_ERP_PARAM_OFT                       2
#define MAC_ERP_LEN                             3
// ERP BIT FIELD
#define MAC_ERP_NON_ERP_PRESENT                 CO_BIT(0)
#define MAC_ERP_USE_PROTECTION                  CO_BIT(1)
#define MAC_ERP_BARKER_PREAMBLE_MODE            CO_BIT(2)
#define MAC_ERP_IE_PRESENT                      CO_BIT(7)   // not standard, used internally

// RSN INFORMATION FIELD (WPA Extension)
#define MAC_RSNIE_ID_OFT                         0
#define MAC_RSNIE_LEN_OFT                        1
#define MAC_RSNIE_INFO_OFT                       2
#define MAC_RSNIE_VERSION_OFT                    2
#define MAC_RSNIE_GROUP_CIPHER_OFT               4
#define MAC_RSNIE_PAIRWISE_CIPHER_SUITE_CNT_OFT  8
#define MAC_RSNIE_PAIRWISE_CIPHER_SUITE_LIST_OFT 10
#define MAC_RSNIE_PAIRWISE_CIPHER_SIZE           4
#define MAC_RSNIE_KEY_MANAGEMENT_SUITE_CNT_OFT   14
#define MAC_RSNIE_KEY_MANAGEMENT_SUITE_LIST_OFT  16
#define MAC_RSNIE_KEY_MANAGEMENT_SIZE            4
#define MAC_RSNIE_RSN_CAPABILITIES_OFT           20
#define MAC_RSNIE_RSN_PMKID_COUNT_OFT            22
#define MAC_RSNIE_RSN_PMKID_COUNT_LIST_OFT       24
#define MAC_RSNIE_RSN_PMKID_SIZE                 16
#define MAC_RSNIE_MIN_LEN                        20

// WPA INFORMATION FIELD (pre RSN)
#define MAC_WPA_ID_OFT                           0
#define MAC_WPA_LEN_OFT                          1
#define MAC_WPA_INFO_OFT                         2
#define MAC_WPA_VERSION_OFT                      6
#define MAC_WPA_GROUP_CIPHER_OFT                 8
#define MAC_WPA_PAIRWISE_CIPHER_SUITE_CNT_OFT    12
#define MAC_WPA_PAIRWISE_CIPHER_SUITE_LIST_OFT   14
#define MAC_WPA_CIPHER_SIZE                      4
#define MAC_WPA_KEY_MANAGEMENT_SUITE_CNT_OFT     18
#define MAC_WPA_KEY_MANAGEMENT_SUITE_LIST_OFT    20
#define MAC_WPA_KEY_MANAGEMENT_SIZE              4
#define MAC_WPA_CAPABILITIES_OFT                 24
#define MAC_WPA_MIN_LEN                          24

// AP CHANNEL REPORT
#define MAC_AP_CHAN_REPORT_ID_OFT       0
#define MAC_AP_CHAN_REPORT_LEN_OFT      1
#define MAC_AP_CHAN_REPORT_OP_CLASS_OFT 2
#define MAC_AP_CHAN_REPORT_OP_CHAN_OFT  3
#define MAC_AP_CHAN_REPORT_MIN_LEN      3

// WAPI INFORMATION FIELD
#define MAC_WAPI_ID_OFT                          0
#define MAC_WAPI_LEN_OFT                         1
#define MAC_WAPI_INFO_OFT                        2
#define MAC_WAPI_VERSION_OFT                     2
#define MAC_WAPI_AKM_SUITE_CNT_OFT               4
#define MAC_WAPI_AKM_SUITE_LIST_OFT              6
#define MAC_WAPI_AKM_SUITE_SIZE                  4
#define MAC_WAPI_PAIRWISE_CIPHER_SUITE_CNT_OFT   10
#define MAC_WAPI_PAIRWISE_CIPHER_SUITE_LIST_OFT  12
#define MAC_WAPI_CIPHER_SUITE_SIZE               4
#define MAC_WAPI_GROUP_CIPHER_SUITE_OFT          16
#define MAC_WAPI_CAPABILITY_OFT                  20
#define MAC_WAPI_BKID_CNT_OFT                    22
#define MAC_WAPI_BKID_LIST_OFT                   24
#define MAC_WAPI_MIN_LEN                         22

// BSS Load IE
#define MAC_BSS_LOAD_ID_OFT                     0
#define MAC_BSS_LOAD_LEN_OFT                    1
#define MAC_BSS_LOAD_STA_CNT_OFT                2
#define MAC_BSS_LOAD_CH_UTILIZATION_OFT         4
#define MAC_BSS_LOAD_AVAIL_ADM_CAPA             5
#define MAC_BSS_LOAD_LEN                        7

// EDCA Parameter Set IE
#define MAC_EDCA_PARAM_ID_OFT                   0
#define MAC_EDCA_PARAM_LEN_OFT                  1
#define MAC_EDCA_PARAM_QOS_INFO_OFT             2
#define MAC_EDCA_PARAM_RESERVED_OFT             3
#define MAC_EDCA_PARAM_BE_PARAM_OFT             4
#define MAC_EDCA_PARAM_BK_PARAM_OFT             8
#define MAC_EDCA_PARAM_VI_PARAM_OFT             12
#define MAC_EDCA_PARAM_VO_PARAM_OFT             16
#define MAC_EDCA_PARAM_LEN                      20

// QoS Capability IE
#define MAC_QOS_CAPA_ID_OFT                     0
#define MAC_QOS_CAPA_LEN_OFT                    1
#define MAC_QOS_CAPA_INFO_OFT                   2
#define MAC_QOS_CAPA_LEN                        3

// RM Enabled Capabilities
#define MAC_RM_EN_CAPA_OFT                          2
#define MAC_RM_CAPA_BYTE_0_LINK                     CO_BIT(0)
#define MAC_RM_CAPA_BYTE_0_NEIGHBOR_REPORT          CO_BIT(1)
#define MAC_RM_CAPA_BYTE_0_PARALLEL                 CO_BIT(2)
#define MAC_RM_CAPA_BYTE_0_REPEATED                 CO_BIT(3)
#define MAC_RM_CAPA_BYTE_0_BEACON_PASSIVE           CO_BIT(4)
#define MAC_RM_CAPA_BYTE_0_BEACON_ACTIVE            CO_BIT(5)
#define MAC_RM_CAPA_BYTE_0_BEACON_TABLE             CO_BIT(6)
#define MAC_RM_CAPA_BYTE_0_BEACON_REPORT_COND       CO_BIT(7)
#define MAC_RM_CAPA_BYTE_1_FRAME                    CO_BIT(0)
#define MAC_RM_CAPA_BYTE_1_CHANNEL_LOAD             CO_BIT(1)
#define MAC_RM_CAPA_BYTE_1_NOISE_HISTO              CO_BIT(2)
#define MAC_RM_CAPA_BYTE_1_STATISTICS               CO_BIT(3)
#define MAC_RM_CAPA_BYTE_1_LCI                      CO_BIT(4)
#define MAC_RM_CAPA_BYTE_1_LCI_AZIMUTH              CO_BIT(5)
#define MAC_RM_CAPA_BYTE_1_TRANSMIT_STREAM_CAT      CO_BIT(6)
#define MAC_RM_CAPA_BYTE_1_TRIG_TRANSMIT_STREAM_CAT CO_BIT(7)
#define MAC_RM_CAPA_BYTE_2_AP_CHANNEL               CO_BIT(0)
#define MAC_RM_CAPA_BYTE_2_RM_MIB                   CO_BIT(1)
#define MAC_RM_CAPA_BYTE_2_OPER_CHAN_OFT            2
#define MAC_RM_CAPA_BYTE_2_OPER_CHAN_MSK            (0x7 << MAC_RM_CAPA_BYTE_2_OPER_CHAN_OFT)
#define MAC_RM_CAPA_BYTE_2_NON_OPER_CHAN_OFT        5
#define MAC_RM_CAPA_BYTE_2_NON_OPER_CHAN_MSK        (0x7 << MAC_RM_CAPA_BYTE_2_NON_OPER_CHAN_OFT)
#define MAC_RM_CAPA_BYTE_3_PILOT_OFT                0
#define MAC_RM_CAPA_BYTE_3_PILOT_MSK                (0x7 << MAC_RM_CAPA_BYTE_3_PILOT_OFT)
#define MAC_RM_CAPA_BYTE_3_PILOT_TRANSMIT_INFO      CO_BIT(3)
#define MAC_RM_CAPA_BYTE_3_NEIGHBOR_TSF_OFT_REPORT  CO_BIT(4)
#define MAC_RM_CAPA_BYTE_3_RCPI                     CO_BIT(5)
#define MAC_RM_CAPA_BYTE_3_RSNI                     CO_BIT(6)
#define MAC_RM_CAPA_BYTE_3_BSS_AVG_ACCESS_DELAY     CO_BIT(7)
#define MAC_RM_CAPA_BYTE_4_BSS_AVAIL_ADMISSION      CO_BIT(0)
#define MAC_RM_CAPA_BYTE_4_ANTENNA                  CO_BIT(1)
#define MAC_RM_CAPA_BYTE_4_FTM_RANGE_REPORT         CO_BIT(2)
#define MAC_RM_CAPA_BYTE_4_CIVIC_LOCATION           CO_BIT(3)
#define MAC_RM_EN_CAPA_LEN                          5

// Multiple BSSID IE
#define MAC_MULTI_BSSID_ID_OFT                  0
#define MAC_MULTI_BSSID_LEN_OFT                 1
#define MAC_MULTI_BSSID_MAX_INDICATOR_OFT       2
#define MAC_MULTI_BSSID_SUB_IES_OFT             3
#define MAC_MULTI_BSSID_ELMT_MIN_LEN            4
#define MAC_MULTI_BSSID_MIN_LEN                 6

// Nontransmitted BSSID capabilities IE
#define MAC_NON_TXED_BSSID_CAPA_ID_OFT          0
#define MAC_NON_TXED_BSSID_CAPA_LEN_OFT         1
#define MAC_NON_TXED_BSSID_CAPA_OFT             2
#define MAC_NON_TXED_BSSID_CAPA_LEN             4

// Nontransmitted BSSID capabilities IE
#define MAC_MULTI_BSSID_INDEX_ID_OFT            0
#define MAC_MULTI_BSSID_INDEX_LEN_OFT           1
#define MAC_MULTI_BSSID_INDEX_OFT               2
#define MAC_MULTI_BSSID_DTIM_PERIOD_OFT         3
#define MAC_MULTI_BSSID_DTIM_COUNT_OFT          4
#define MAC_MULTI_BSSID_INDEX_MIN_LEN           3

// HT Capability IE
#define MAC_HT_CAPA_ID_OFT                      0
#define MAC_HT_CAPA_LEN_OFT                     1
#define MAC_HT_CAPA_INFO_OFT                    2
#define MAC_HT_CAPA_AMPDU_PARAM_OFT             4
#define MAC_HT_CAPA_SUPPORTED_MCS_SET_OFT       5
#define MAC_HT_CAPA_EXTENDED_CAPA_OFT           21
#define MAC_HT_CAPA_TX_BEAM_FORMING_CAPA_OFT    23
#define MAC_HT_CAPA_ASEL_CAPA_OFT               27
#define MAC_HT_CAPA_ELMT_LEN                    26
#define MAC_HT_CAPA_ELMT_LEN_WD                (CO_ALIGN4_HI(MAC_HT_CAPA_ELMT_LEN) / 4)
#define MAC_HT_CAPA_LEN                         28

// HT Operation IE
#define MAC_HT_OPER_ID_OFT                      0
#define MAC_HT_OPER_LEN_OFT                     1
#define MAC_HT_OPER_PRIM_CH_OFT                 2
#define MAC_HT_OPER_INFO_OFT                    3
#define MAC_HT_OPER_INFO_SUBSET2_OFT            4
#define MAC_HT_OPER_OP_MODE_MASK                0x0003
#define MAC_HT_OPER_NONGF_MASK                  0x0004
#define MAC_HT_OPER_OBSS_MASK                   0x0010
#define MAC_HT_OPER_INFO_SUBSET3_OFT            6
#define MAC_HT_OPER_BASIC_MSC_SET_OFT           8
#define MAC_HT_OPER_ELMT_LEN                    22
#define MAC_HT_OPER_LEN                         24

// HT Protection values
enum mac_ht_oper_prot_val
{
    MAC_HT_OPER_PROT_NO_PROT   = 0,
    MAC_HT_OPER_PROT_NONMEMBER,
    MAC_HT_OPER_PROT_20MHZ,
    MAC_HT_OPER_PROP_NON_HT_MIXED
};

// VHT Capability IE
#define MAC_VHT_CAPA_ELMT_LEN                   12
#define MAC_VHT_CAPA_ELMT_LEN_WD               (CO_ALIGN4_HI(MAC_VHT_CAPA_ELMT_LEN) / 4)
#define MAC_VHT_CAPA_LEN                        14
#define MAC_VHT_CAPA_ID_OFT                     0
#define MAC_VHT_CAPA_LEN_OFT                    1
#define MAC_VHT_CAPA_INFO_OFT                   2
#define MAC_VHT_RX_MCS_MAP_OFT                  6
#define MAC_VHT_RX_HIGHEST_RATE_OFT             8
#define MAC_VHT_TX_MCS_MAP_OFT                  10
#define MAC_VHT_TX_HIGHEST_RATE_OFT             12

// VHT Operation IE
#define MAC_VHT_OPER_ELMT_LEN                   5
#define MAC_VHT_OPER_LEN                        7
#define MAC_VHT_OPER_ID_OFT                     0
#define MAC_VHT_OPER_LEN_OFT                    1
#define MAC_VHT_CHAN_WIDTH_OFT                  2
#define MAC_VHT_CENTER_FREQ0_OFT                3
#define MAC_VHT_CENTER_FREQ1_OFT                4
#define MAC_VHT_OPER_INFO_LEN                   3
#define MAC_VHT_BASIC_MCS_OFT                   5

// Wide Bandwidth Channel Switch
#define MAC_WIDE_BW_CHAN_SW_ELMT_LEN         3
#define MAC_WIDE_BW_CHAN_SW_LEN              5
#define MAC_WIDE_BW_CHAN_SW_ID_OFT           0
#define MAC_WIDE_BW_CHAN_SW_LEN_OFT          1
#define MAC_WIDE_BW_CHAN_SW_WIDTH_OFT        2
#define MAC_WIDE_BW_CHAN_SW_CHAN_CENTER1_OFT 3
#define MAC_WIDE_BW_CHAN_SW_CHAN_CENTER2_OFT 4

// 20/40 Coexistence IE
#define MAC_20_40_COEXISTENCE_ID_OFT                0
#define MAC_20_40_COEXISTENCE_LEN_OFT               1
#define MAC_20_40_COEXISTENCE_INFO_OFT              2
#define MAC_20_40_COEXISTENCE_ELMT_LEN              1
#define MAC_20_40_COEXISTENCE_LEN                   3
#define MAC_20_40_COEXISTENCE_INFO_REQ_MASK         0x01
#define MAC_20_40_COEXISTENCE_40_INTOLERANT_MASK    0x02
#define MAC_20_40_COEXISTENCE_20_BSS_WIDTH_REQ_MASK 0x04
#define MAC_20_40_COEXISTENCE_OBSS_SCAN_REQ_MASK    0x08
#define MAC_20_40_COEXISTENCE_OBSS_SCAN_GRANT_MASK  0x10

// Mgmt MIC IE
#define MAC_MGMT_MIC_ID_OFT    0
#define MAC_MGMT_MIC_LEN_OFT   1
#define MAC_MGMT_MIC_KEYID_OFT 2
#define MAC_MGMT_MIC_IPN_OFT   4
#define MAC_MGMT_MIC_IPN_LEN   6
#define MAC_MGMT_MIC_MIC_OFT   10
#define MAC_MGMT_MIC_MIC_LEN   8
#define MAC_MGMT_MIC_LEN       18

// WME IE
#define MAC_WMM_PARAM_ELMT_LEN           24
#define MAC_WMM_PARAM_LEN                26
#define MAC_WMM_PARAM_QOS_INFO_OFT        8
#define MAC_WMM_PARAM_SET_CNT_MSK         0x0F
#define MAC_WMM_PARAM_RESERVED_OFT        9
#define MAC_WMM_PARAM_BK_PARAM_OFT       14
#define MAC_WMM_PARAM_BE_PARAM_OFT       10
#define MAC_WMM_PARAM_VI_PARAM_OFT       18
#define MAC_WMM_PARAM_VO_PARAM_OFT       22

// QOS INFORMATION FIELD
#define MAC_QINFO_OFT                           6

// QOS INFORMATION FIELD received from the AP
#define MAC_QINFO_SET_COUNT_OFT                 8
#define MAC_QINFO_SET_UAPSD_OFT                 8

// Bit position for QOS INFORMATION FEILD received from the AP
#define MAC_QINFO_SET_COUNT                     0x07   // Bits 0,1,2
#define MAC_QINFO_SET_UAPSD                     0x80   // Bit 7

#define SUP_MCS_RX_HIGHEST_RATE_OFT             10 // 10,11th Byte from Start of MCS.
#define SUP_MCS_RX_HIGHEST_RATE_SIZE            10 // 10 Bits
#define SUP_TX_MCS_SET_OFT                      12 // 12th Byte from Start of MCS.

// HE Capability IE
#define MAC_HE_CAPA_ID_OFT                      0
#define MAC_HE_CAPA_LEN_OFT                     1
#define MAC_HE_CAPA_EXT_ID_OFT                  2
#if MACSW_11AX_DRAFT_2_0
#define MAC_HE_CAPA_IE_MAC_CAPA_LEN            (MAC_HE_MAC_CAPA_LEN - 1)
#define MAC_HE_CAPA_IE_PHY_CAPA_LEN            (MAC_HE_PHY_CAPA_LEN - 2)
#else
#define MAC_HE_CAPA_IE_MAC_CAPA_LEN             MAC_HE_MAC_CAPA_LEN
#define MAC_HE_CAPA_IE_PHY_CAPA_LEN             MAC_HE_PHY_CAPA_LEN
#endif
#define MAC_HE_MAC_CAPA_INFO_OFT                3
#define MAC_HE_PHY_CAPA_INFO_OFT               (MAC_HE_MAC_CAPA_INFO_OFT + MAC_HE_CAPA_IE_MAC_CAPA_LEN)
#define MAC_HE_MCS_INFO_OFT                    (MAC_HE_PHY_CAPA_INFO_OFT + MAC_HE_CAPA_IE_PHY_CAPA_LEN)
#define MAC_HE_MCS_INFO_PER_BW_LEN              4
#define MAC_HE_CAPA_MIN_LEN                    (MAC_HE_MCS_INFO_OFT + MAC_HE_MCS_INFO_PER_BW_LEN)
#define MAC_HE_CAPA_MAX_LEN                    (MAC_HE_MCS_INFO_OFT + 3 * MAC_HE_MCS_INFO_PER_BW_LEN +\
                                                MAC_HE_PPE_THRES_MAX_LEN)

// HE Operation IE
#define MAC_HE_OPER_ID_OFT                      0
#define MAC_HE_OPER_LEN_OFT                     1
#define MAC_HE_OPER_EXT_ID_OFT                  2
#define MAC_HE_OPER_PARAM_OFT                   3
#define MAC_HE_OPER_PARAM_LEN                   4  // including BSS color info
#define MAC_HE_OPER_PE_DURATION_OFT             0
#define MAC_HE_OPER_PE_DURATION_MSK             (0x3 << MAC_HE_OPER_PE_DURATION_OFT)
#define MAC_HE_OPER_TWT_REQUIRED_BIT            CO_BIT(3)
#define MAC_HE_OPER_TXOP_DUR_RTS_THRES_OFT      4
#define MAC_HE_OPER_TXOP_DUR_RTS_THRES_MSK     (0x3FF << MAC_HE_OPER_TXOP_DUR_RTS_THRES_OFT)
#define MAC_HE_OPER_TXOP_DUR_RTS_THRES_DISABLED 1023
#define MAC_HE_OPER_VHT_OPER_PRESENT_BIT        CO_BIT(14)
#define MAC_HE_OPER_CO_HOSTED_BSS_BIT           CO_BIT(15)
#define MAC_HE_OPER_ER_SU_DISABLE               CO_BIT(16)
#define MAC_HE_OPER_6G_OP_PRESENT               CO_BIT(17)
#if MACSW_11AX_DRAFT_2_0
#define MAC_HE_OPER_BSS_COLOR_OFT               0
#define MAC_HE_OPER_BSS_COLOR_PARTIAL_BIT       CO_BIT(20)
#define MAC_HE_OPER_BSS_COLOR_DISABLED_BIT      CO_BIT(30)
#else
#define MAC_HE_OPER_BSS_COLOR_OFT               24
#define MAC_HE_OPER_BSS_COLOR_PARTIAL_BIT       CO_BIT(30)
#define MAC_HE_OPER_BSS_COLOR_DISABLED_BIT      CO_BIT(31)
#endif
#define MAC_HE_OPER_BSS_COLOR_MASK             (0x3F << MAC_HE_OPER_BSS_COLOR_OFT)
#define MAC_HE_OPER_TWT_REQUIRED_BIT            CO_BIT(3)
#define MAC_HE_OPER_BASIC_MCS_OFT               7
#define MAC_HE_OPER_VHT_OPER_INFO_OFT           9
#define MAC_HE_OPER_MIN_LEN                     9
#define MAC_HE_OPER_MAX_LEN                     18

// UORA Operation IE
#define MAC_UORA_ELMT_LEN                       1
#define MAC_UORA_LEN                            4
#define MAC_UORA_ID_OFT                         0
#define MAC_UORA_LEN_OFT                        1
#define MAC_UORA_EXT_ID_OFT                     2
#define MAC_UORA_OCW_RANGE                      3
#define MAC_UORA_EOCW_MIN_OFT                   0
#define MAC_UORA_EOCW_MIN_MASK                  (0x7 << MAC_UORA_EOCW_MIN_OFT)
#define MAC_UORA_EOCW_MAX_OFT                   3
#define MAC_UORA_EOCW_MAX_MASK                  (0x7 << MAC_UORA_EOCW_MAX_OFT)
#define MAC_UORA_EOCW_DEFAULT_MIN               3
#define MAC_UORA_EOCW_DEFAULT_MAX               5

// MU EDCA Operation IE
#define MAC_MU_EDCA_ELMT_LEN                    13
#define MAC_MU_EDCA_LEN                         16
#define MAC_MU_EDCA_ID_OFT                      0
#define MAC_MU_EDCA_LEN_OFT                     1
#define MAC_MU_EDCA_EXT_ID_OFT                  2
#define MAC_MU_EDCA_QOS_INFO_OFT                3
#define MAC_MU_EDCA_PARAM_SET_CNT_MSK           0x0F
#define MAC_MU_EDCA_AC_BE_OFT                   4
#define MAC_MU_EDCA_AC_BK_OFT                   7
#define MAC_MU_EDCA_AC_VI_OFT                   10
#define MAC_MU_EDCA_AC_VO_OFT                   13

#define MAC_MESHID_ELMT_MAX_LEN                 32
#define MAC_MESHID_MAX_LEN                      34

#define MAC_MPM_INFO_MIN_LEN                    4
#define MAC_MPM_PEERING_PROT_OFT               (MAC_INFOELT_INFO_OFT + 0)
#define MAC_MPM_LOCAL_LINK_ID_OFT              (MAC_INFOELT_INFO_OFT + 2)
#define MAC_MPM_OPT_FIELD_OFT                  (MAC_INFOELT_INFO_OFT + MAC_MPM_INFO_MIN_LEN)
#define MAC_MPM_MIN_LEN                         6
#define MAC_MPM_MAX_LEN                         26

#define MAC_MAW_AWAKE_WIND_OFT                  MAC_INFOELT_INFO_OFT
#define MAC_MAW_LEN                             4

#define MAC_MCFG_ELMT_LEN                       7
#define MAC_MCFG_PATH_SEL_PROT_OFT             (MAC_INFOELT_INFO_OFT + 0)
#define MAC_MCFG_PATH_METRIC_PROT_OFT          (MAC_INFOELT_INFO_OFT + 1)
#define MAC_MCFG_CONG_CTRL_MODE_OFT            (MAC_INFOELT_INFO_OFT + 2)
#define MAC_MCFG_SYNC_METHOD_OFT               (MAC_INFOELT_INFO_OFT + 3)
#define MAC_MCFG_AUTH_PROT_OFT                 (MAC_INFOELT_INFO_OFT + 4)
#define MAC_MCFG_MESH_FORM_INFO_OFT            (MAC_INFOELT_INFO_OFT + 5)
#define MAC_MCFG_MESH_CAP_OFT                  (MAC_INFOELT_INFO_OFT + 6)
#define MAC_MCFG_LEN                            9

// Extended Capabilities
#define MAC_EXT_CAPA_ID_OFT                     0
#define MAC_EXT_CAPA_LEN_OFT                    1
#define MAC_EXT_CAPA_MIN_LEN                    3

// Extended Capabilities values
#define MAC_EXT_CAPA_MULTIPLE_BSSID_POS         22
#define MAC_EXT_CAPA_FTM_RESPONDER_POS          70
#define MAC_EXT_CAPA_FTM_INITIATOR_POS          71

/** @} */

/**
 * @name TWT, S1G capabilities, S1G operation elements
 * @{
 ****************************************************************************************
 */
// TWT IE
#define MAC_TWT_ID_OFT                         0
#define MAC_TWT_LEN_LEN                        1
#define MAC_TWT_LEN_OFT                        1
#define MAC_TWT_CTRL_OFT                       2
#define MAC_TWT_CTRL_LEN                       1
#define MAC_TWT_PARAM_INFO_OFT                 3

// TWT Individual TWT Parameter Set
#define MAC_TWT_IND_REQ_TYPE_OFT               0
#define MAC_TWT_IND_REQ_TYPE_LEN               2
#define MAC_TWT_IND_TARGET_WAKE_TIME_OFT       2
#define MAC_TWT_IND_TARGET_WAKE_TIME_LEN       8
#define MAC_TWT_IND_NOM_MIN_WAKE_DUR_LEN       1
#define MAC_TWT_IND_WAKE_INT_MANT_LEN          2
#define MAC_TWT_IND_CHANNEL_LEN                1
#define MAC_TWT_IND_NDP_PAGING_LEN             4
#define MAC_TWT_IND_MIN_LEN                    6
#define MAC_TWT_IND_MAX_LEN                    27

// TWT Teardown
#define MAC_TWT_TEARDOWN_FLOW_ID_OFT           0
#define MAC_TWT_TEARDOWN_FLOW_ID_MASK          (0x7 << MAC_TWT_TEARDOWN_FLOW_ID_OFT)
#define MAC_TWT_TEARDOWN_NEGOT_TYPE_OFT        5
#define MAC_TWT_TEARDOWN_NEGOT_TYPE_MASK       (0x3 << MAC_TWT_TEARDOWN_NEGOT_TYPE_OFT)
#define MAC_TWT_TEARDOWN_ALL_TWT_BIT           CO_BIT(7)
#define MAC_TWT_TEARDOWN_BROAD_ID_OFT          0
#define MAC_TWT_TEARDOWN_BROAD_ID_MASK         (0x1F << MAC_TWT_TEARDOWN_BROAD_ID_OFT)

// TWT Control IE
#define MAC_TWT_CTRL_NDP_PAG_BIT                CO_BIT(0)
#define MAC_TWT_CTRL_RESP_PM_MODE_BIT           CO_BIT(1)
#define MAC_TWT_CTRL_NEGOT_TYPE_OFT             2
#define MAC_TWT_CTRL_NEGOT_TYPE_MASK            (0x3 << MAC_TWT_CTRL_NEGOT_TYPE_OFT)
#define MAC_TWT_CTRL_INFO_FRM_DIS_BIT           CO_BIT(4)
#define MAC_TWT_CTRL_WAKE_DUR_UNIT_BIT          CO_BIT(5)

#define MAC_TWT_CTRL_NEGOT_INDIV                0
#define MAC_TWT_CTRL_NEGOT_NEXT_WAKE_TBTT       1

#define MAC_TWT_REQ_TYPE_RESPONDER              0
#define MAC_TWT_REQ_TYPE_REQUESTER              1

// TWT Request Type
#define MAC_TWT_IND_TWT_REQ_OFT                 0
#define MAC_TWT_IND_REQ_TYPE_SETUP_OFT          1
#define MAC_TWT_IND_REQ_TYPE_SETUP_MASK         (0x7 << MAC_TWT_IND_REQ_TYPE_SETUP_OFT)
#define MAC_TWT_IND_TRIGGER_BIT                 CO_BIT(4)
#define MAC_TWT_IND_IMPLICIT_BIT                CO_BIT(5)
#define MAC_TWT_IND_FLOW_TYPE_BIT               CO_BIT(6)
#define MAC_TWT_IND_FLOW_ID_OFT                 7
#define MAC_TWT_IND_FLOW_ID_MASK                (0x7 << MAC_TWT_IND_FLOW_ID_OFT)
#define MAC_TWT_IND_WAKE_INT_EXP_OFT            10
#define MAC_TWT_IND_WAKE_INT_EXP_MASK           (0x1F << MAC_TWT_IND_WAKE_INT_EXP_OFT)

/// TWT Group Assignment field format
#define MAC_TWT_GRP_ASSIGN_ID_OFT               0
#define MAC_TWT_GRP_ASSIGN_ID_MASK              (0x7F << MAC_TWT_GRP_ASSIGN_ID_OFT)
#define MAC_TWT_GRP_ASSIGN_ID_LEN               7
#define MAC_TWT_GRP_ASSIGN_ZERO_PRES_OFT        7
#define MAC_TWT_GRP_ASSIGN_ZERO_OFT             8
#define MAC_TWT_GRP_ASSIGN_ZERO_MASK            (0xFFFFFFFFFFFF << MAC_TWT_GRP_ASSIGN_ZERO_MASK)
#define MAC_TWT_GRP_ASSIGN_ZERO_LEN             48
#define MAC_TWT_GRP_ASSIGN_UNIT_MASK            0xF
#define MAC_TWT_GRP_ASSIGN_UNIT_LEN             4
#define MAC_TWT_GRP_ASSIGN_OFFSET_MASK          0xFFF
#define MAC_TWT_GRP_ASSIGN_OFFSET_LEN           12

/// TWT Setup command
enum twt_setup_commands
{
    MAC_TWT_SETUP_REQUEST = 0,
    MAC_TWT_SETUP_SUGGEST,
    MAC_TWT_SETUP_DEMAND,
    MAC_TWT_SETUP_GROUPING,
    MAC_TWT_SETUP_ACCEPT,
    MAC_TWT_SETUP_ALTERNATE,
    MAC_TWT_SETUP_DICTATE,
    MAC_TWT_SETUP_REJECT,
};

/* S1G Capabilities */
#define MAC_S1G_CAP_ACTION_OFT                  1

#define MAC_S1G_CAP_ACTION_LEN                  17
#define MAC_S1G_CAP_ACTION_ID_OFT               0
#define MAC_S1G_CAP_ACTION_LEN_LEN              1
#define MAC_S1G_CAP_ACTION_LEN_OFT              1
#define MAC_S1G_CAP_ACTION_S1G_CAP_INFO_OFT     2
#define MAC_S1G_CAP_ACTION_S1G_CAP_INFO_LEN     10
#define MAC_S1G_CAP_ACTION_S1G_MCS_NSS_OFT      6

/// S1G Capabilities Info
#define MAC_S1G_CAP_ACTION_S1G_TWT_GRP_OFT      64
#define MAC_S1G_CAP_ACTION_S1G_TWT_REQ_SUP_OFT  69
#define MAC_S1G_CAP_ACTION_S1G_TWT_RESP_SUP_OFT 70

/* S1G Operation */
#define MAC_S1G_OP_ACTION_OFT                   1
#define MAC_S1G_OP_ACTION_LEN                   8
#define MAC_S1G_OP_ACTION_ID_OFT                0
#define MAC_S1G_OP_ACTION_LEN_LEN               1
#define MAC_S1G_OP_ACTION_LEN_OFT               1
#define MAC_S1G_OP_ACTION_INFO_OFT              2
#define MAC_S1G_OP_ACTION_INFO_LEN              4
#define MAC_S1G_OP_ACTION_BASIC_MCS_NSS_OFT     6

/// S1G Operation Info
#define MAC_S1G_OP_INFO_ACTION_CHAN_WIDTH_OFT          0
#define MAC_S1G_OP_INFO_ACTION_OP_CLASS_OFT            1
#define MAC_S1G_OP_INFO_ACTION_PRIM_CHAN_NUM_OFT       2
#define MAC_S1G_OP_INFO_ACTION_CHAN_CENT_FREQ_OFT      3

/* S1G Action Field values
*--------------------------------------------------------------------------------------*/
/// 802.11 S1G action frame definition
#define MAC_AID_SWITCH_REQ_S1G_ACTION           0
#define MAC_AID_SWITCH_RES_S1G_ACTION           1
#define MAC_SYNC_CTRL_S1G_ACTION                2
#define MAC_STA_INFO_ANNO_S1G_ACTION            3
#define MAC_EDCA_PAR_SET_S1G_ACTION             4
#define MAC_EL_OP_S1G_ACTION                    5
#define MAC_TWT_SETUP_S1G_ACTION                6
#define MAC_TWT_TEARDOWN_S1G_ACTION             7
#define MAC_SECT_GRP_ID_S1G_ACTION              8
#define MAC_SECT_ID_FEEDBACK_S1G_ACTION         9
#define MAC_TWT_INFO_S1G_ACTION                 11

/** @} */

/**
 * @name Multiple BSSID Subelement offsets, lengths and other definitions
 * @{
 ****************************************************************************************
 */
// Multiple BSSID IE
#define MAC_MBSSID_NON_TXED_PROF_ID_OFT         0
#define MAC_MBSSID_NON_TXED_PROF_LEN_OFT        1
#define MAC_MBSSID_NON_TXED_PROF_INFO_OFT       2
#define MAC_MBSSID_NON_TXED_PROF_ELMT_MIN_LEN   4
#define MAC_MBSSID_NON_TXED_PROF_MIN_LEN        6
/** @} */

/**
 * @name Management Frame Element definitions and offsets
 * @{
 ****************************************************************************************
 */
/// 802.11 Management frame element and offset definition
#define MAC_BEACON_MAX_LEN              333
#define MAC_BEACONINFO_SIZE              78   // size without TIM
#define MAC_TIM_SIZE                    251   // max TIM size
// Attention, in a non-AP STA the IBSS
// parameter set is stored in place of
// the TIM -> min size 4

#define MAC_BEACON_HDR_LEN               36

/*
 * Beacon Frame offset (Table 5 p46)
 */
#define MAC_BEACON_TIMESTAMP_OFT          MAC_SHORT_MAC_HDR_LEN   // Order 1
#define MAC_BEACON_INTERVAL_OFT          (MAC_SHORT_MAC_HDR_LEN + 8)   // Order 2
#define MAC_BEACON_CAPA_OFT              (MAC_SHORT_MAC_HDR_LEN + 10)   // Order 3
#define MAC_BEACON_VARIABLE_PART_OFT     (MAC_SHORT_MAC_HDR_LEN + 12)   // Order 4
/*
 * Probe Request Frame offset
 */
#define MAC_PROBEREQ_SSID_OFT             0   // Order 1

/*
 * Probe Response Frame offset (Table 12 p49)
 */
#define MAC_PROBE_TIMESTAMP_OFT           0   // Order 1
#define MAC_PROBE_INTERVAL_OFT            8   // Order 2
#define MAC_PROBE_CAPA_OFT               10   // Order 3
#define MAC_PROBE_SSID_OFT               12   // Order 4

/*
 * Authentication Frame offset
 */
#define MAC_AUTH_ALGONBR_OFT              0   // Order 1
#define MAC_AUTH_SEQNBR_OFT               2   // Order 2
#define MAC_AUTH_STATUS_OFT               4   // Order 3
#define MAC_AUTH_CHALLENGE_OFT            6   // Order 4
#define MAC_AUTH_CHALLENGE_LEN          128
#define MAC_AUTH_FT_IE_OFT                6
//challenge text IE size
#define CHALLENGE_TEXT_SIZE             130

/*
 * Association Req Frame offset
 */
#define MAC_ASSO_REQ_CAPA_OFT             0   // Order 1
#define MAC_ASSO_REQ_LISTEN_OFT           2   // Order 2
#define MAC_ASSO_REQ_SSID_OFT             4   // Order 3

/*
 * Association Rsp Frame offset
 */
#define MAC_ASSO_RSP_CAPA_OFT             0   // Order 1
#define MAC_ASSO_RSP_STATUS_OFT           2   // Order 2
#define MAC_ASSO_RSP_AID_OFT              4   // Order 3
#define MAC_ASSO_RSP_RATES_OFT            6   // Order 4

/*
 * Re association Req Frame offset
 */
#define MAC_REASSO_REQ_CAPA_OFT           0   // Order 1
#define MAC_REASSO_REQ_LISTEN_OFT         2   // Order 2
#define MAC_REASSO_REQ_AP_ADDR_OFT        4   // Order 3
#define MAC_REASSO_REQ_SSID_OFT          10   // Order 4

/*
 * Deauthentication Req Frame offset
 */

#define MAC_DEAUTH_REASON_OFT             0

/*
 * De association Req Frame offset
 */

#define MAC_DISASSOC_REASON_OFT           0
#define MAC_DISASSOC_REASON_LEN           2

/*
 * Category and action for all action frames
 */
#define MAC_ACTION_CATEGORY_OFT           0
#define MAC_ACTION_ACTION_OFT             1
#define MAC_ACTION_INFO_OFT               2
#define MAC_ACTION_TOKEN_OFT              MAC_ACTION_INFO_OFT
#define MAC_ACTION_P2P_ACTION_OFT         5
#define MAC_ACTION_P2P_TAGGED_OFT         7
/** @} */

/**
 * @name Action Frames categories and other definitions
 * @{
 ****************************************************************************************
 */
/// 802.11 Action frame category
#define MAC_SPECTRUM_ACTION_CATEGORY        0
#define MAC_WMMAC_QOS_ACTION_CATEGORY       1
#define MAC_DLS_ACTION_CATEGORY             2
#define MAC_BA_ACTION_CATEGORY              3
#define MAC_PUBLIC_ACTION_CATEGORY          4
#define MAC_RADIO_MEASURE_ACTION_CATEGORY   5
#define MAC_FAST_BSS_ACTION_CATEGORY        6
#define MAC_HT_ACTION_CATEGORY              7
#define MAC_SA_QUERY_ACTION_CATEGORY        8
#define MAC_PROT_PUBLIC_ACTION_CATEGORY     9
#define MAC_WNM_ACTION_CATEGORY            10
#define MAC_UNPROT_WNM_ACTION_CATEGORY     11
#define MAC_TDLS_ACTION_CATEGORY           12
#define MAC_MESH_ACTION_CATEGORY           13
#define MAC_MULTIHOP_ACTION_CATEGORY       14
#define MAC_SELF_PROT_ACTION_CATEGORY      15
#define MAC_VHT_ACTION_CATEGORY            21
#define MAC_UNPROT_S1G_ACTION_CATEGORY     22
#define MAC_VENDOR_PROT_ACTION_CATEGORY   126
#define MAC_VENDOR_ACTION_CATEGORY        127
#define MAC_ACTION_CATEGORY_ERROR         128

#define MAC_P2P_ACTION_NOA_SUBTYPE          0

#define MAC_SELF_PROT_ACTION_CAPA_OFT     2
#define MAC_SELF_PROT_ACTION_CAPA_LEN     2
#define MAC_SELF_PROT_ACTION_AID_OFT      4
#define MAC_SELF_PROT_ACTION_AID_LEN      2
/** @} */

/**
 * @name Capability Information definitions
 * See IEEE 802.1H for LLC/SNAP encapsulation/decapsulation
 * Ethernet-II snap header (RFC1042 for most EtherTypes)
 * @{
 ****************************************************************************************
 */
/// LLC definition
#define FRAME_BODY_LLC_H 0x0003AAAA
#define FRAME_BODY_LLC_L 0x0D890000
#define PAYLOAD_TYPE_TDLS 0x02

#define MAC_ENCAPSULATED_LLC_H_OFT  0
#define MAC_ENCAPSULATED_LLC_L_OFT  4
#define MAC_ENCAPSULATED_PAYLOAD_TYPE_OFT  8
#define MAC_ENCAPSULATED_PAYLOAD_OFT  9
/** @} */

/**
 * @name WMM-AC Frames and Values definitions
 * @{
 ****************************************************************************************
 */
/*
*---------------------------------------------------------------------------------------
*---------------------------------WMM-AC fields offsets---------------------------------
*---------------------------------------------------------------------------------------
*/
/// 802.11 WMM-AC frame definition
#define MAC_WMMAC_OUI_BYTE                              0x00
#define MAC_WMMAC_OUI_TYPE_BYTE                         0x50
#define MAC_WMMAC_OUI_SUBTYPE_BYTE                      0xF2

/* TSPEC field offsets
*--------------------------------------------------------------------------------------*/
#define MAC_WMMAC_TSPEC_HDR_OFT                         0
#define MAC_WMMAC_TSPEC_TSINFO_OFT                      8
#define MAC_WMMAC_TSPEC_NOM_MSDU_SIZE_OFT              11
#define MAC_WMMAC_TSPEC_MAX_MSDU_SIZE_OFT              13
#define MAC_WMMAC_TSPEC_MINSI_OFT                      15
#define MAC_WMMAC_TSPEC_MAXSI_OFT                      19
#define MAC_WMMAC_TSPEC_INACTIVITY_INTERVAL_OFT        23
#define MAC_WMMAC_TSPEC_SUSPENSION_INTERVAL_OFT        27
#define MAC_WMMAC_TSPEC_SERVICE_START_TIME_OFT         31
#define MAC_WMMAC_TSPEC_MIN_DATA_RATE_OFT              35
#define MAC_WMMAC_TSPEC_MEAN_DATA_RATE_OFT             39
#define MAC_WMMAC_TSPEC_PEAK_DATA_RATE_OFT             43
#define MAC_WMMAC_TSPEC_BURST_SIZE_OFT                 47
#define MAC_WMMAC_TSPEC_DELAY_BOUND_OFT                51
#define MAC_WMMAC_TSPEC_MIN_PHY_RATE_OFT               55
#define MAC_WMMAC_TSPEC_SURPLUS_BW_OFT                 59
#define MAC_WMMAC_TSPEC_MEDUIM_TIME_OFT                61

/* TSINFO field offsets
*--------------------------------------------------------------------------------------*/
#define MAC_WMMAC_TSINFO_HDR_OFT                        0
#define MAC_WMMAC_TSINFO_BODY_OFT                       8

/* TSDELAY field offsets
*--------------------------------------------------------------------------------------*/
#define MAC_WMMSA_TSDELAY_HDR_OFT                       0
#define MAC_WMMSA_TSDELAY_BODY_OFT                      8
/*
*---------------------------------------------------------------------------------------
*---------------------------------WMM-AC fields subtypes--------------------------------
*---------------------------------------------------------------------------------------
*/


#define MAC_WMMAC_OUI_SUBTYPE_TSPEC_LEN                 61


#define MAC_OUI_SUBTYPE_WMMAC_TSINFO_LEN                9

// Used by the unit test of the WMM-AC
#define MAC_OUI_SUBTYPE_WMMAC_TSDELAY_LEN              10

// Bit position for TSPEC info field
#define MAC_TSPEC_TRAFIC_TYPE_OFT                        0
#define MAC_TSPEC_TRAFIC_TYPE_MASK                       (CO_BIT(0))

#define MAC_TSPEC_TSID_OFT                               1
#define MAC_TSPEC_TSID_MASK                              (CO_BIT(1)|CO_BIT(2)|CO_BIT(3)|CO_BIT(4))

#define MAC_TSPEC_DIRECTION_OFT                          5
#define MAC_TSPEC_DIRECTION_MASK                         (CO_BIT(5)|CO_BIT(6))

#define MAC_TSPEC_ACCESS_POLICY_OFT                      7
#define MAC_TSPEC_ACCESS_POLICY_MASK                     (CO_BIT(7)|CO_BIT(8))

#define MAC_TSPEC_AGGREGATION_OFT                        9
#define MAC_TSPEC_AGGREGATION_MASK                       (CO_BIT(9))

#define MAC_TSPEC_APSD_OFT                               10
#define MAC_TSPEC_APSD_MASK                              (CO_BIT(10))

#define MAC_TSPEC_USER_PRIORITY_OFT                      11
#define MAC_TSPEC_USER_PRIORITY_MASK                     (CO_BIT(11)|CO_BIT(12)|CO_BIT(13))

#define MAC_TSPEC_ACK_POLICY_OFT                         14
#define MAC_TSPEC_ACK_POLICY_MASK                        (CO_BIT(14)|CO_BIT(15))

#define MAC_TSPEC_SCHEDULE_OFT                           16
#define MAC_TSPEC_SCHEDULE_MASK                          (CO_BIT(16))

/*
*---------------------------------------------------------------------------------------
*---------------------------------WMM-AC Status Codes-----------------------------------
*---------------------------------------------------------------------------------------
*/
#define MAC_WMMAC_ST_ADDTS_SUCESS                      0
#define MAC_WMMAC_ST_DELTS_POOR_CHANNEL_COND          34
#define MAC_WMMAC_ST_ADDTS_REQUEST_DECLINED           37
#define MAC_WMMAC_ST_ADDTS_INVALID_PARAMETERS         38
#define MAC_WMMAC_ST_ADDTS_REJ_W_SUG_CHANGES          39
#define MAC_WMMAC_ST_ADDTS_REJ_W_DELAY_PRD            47

/*
*---------------------------------------------------------------------------------------
*---------------------------------WMM-AC Reason Codes-----------------------------------
*---------------------------------------------------------------------------------------
*/
#define MAC_WMMAC_RS_DELTS_QSTA_LEAVING               36
#define MAC_WMMAC_RS_DELTS_END_TS                     37
#define MAC_WMMAC_RS_DELTS_UNKOWN_TS                  38
#define MAC_WMMAC_RS_DELTS_TIMEOUT                    39
#define MAC_WMMAC_RS_DELTS_UNSPECIFIED                32


/*
*---------------------------------------------------------------------------------------
*---------------------------------WMM-AC Action Frames----------------------------------
*---------------------------------------------------------------------------------------
*/


/* CATEGORY \ ACTION values
*--------------------------------------------------------------------------------------*/
#define MAC_WMMAC_ACTION_ADDTS_REQ        0
#define MAC_WMMAC_ACTION_ADDTS_RSP        1
#define MAC_WMMAC_ACTION_DELTS            2
#define MAC_WMMAC_ACTION_SCHEDULE         3


/* ADDTS_REQ offsets
*--------------------------------------------------------------------------------------*/

#define MAC_ADDTS_REQ_TSPEC_OFT           3
#define MAC_ADDTS_REQ_TCLASS_OFT         66


/* ADDTS_RSP offsets
*--------------------------------------------------------------------------------------*/

#define MAC_ADDTS_RSP_STATUS_OFT          3
#define MAC_ADDTS_RSP_TSDELAY_OFT         5
#define MAC_ADDTS_RSP_TSPEC_OFT          17
#define MAC_ADDTS_RSP_TCLASS_OFT         80

/* DELTS offsets
*--------------------------------------------------------------------------------------*/

#define MAC_DELTS_TSINFO_OFT              2
#define MAC_DELTS_REASON_OFT             13

/** @} */

/**
 * @name Block-Ack Frames and Values definitions
 * @{
 ****************************************************************************************
 */
/*
*---------------------------------------------------------------------------------------
*---------------------------------Block Ack fields offsets---------------------------------
*---------------------------------------------------------------------------------------
*/
/// 802.11 BlockAck action frame definition
#define MAC_BARINFO_PERTIDINFO_OFT              0
#define MAC_BARINFO_PERTIDINFO_STEP             1
#define MAC_BARINFO_SEQ_OFT                     2
#define MAC_BARINFO_PERTIDINFO_TID_STEP         4
#define MAC_CAPA_BA_OFT                         14

// the length of fields
#define MAC_BA_PARAMSET_LEN                     2
#define MAC_BA_TIMEOUT_LEN                      2
#define MAC_BA_BARINFO_LEN                      4
#define MAC_BA_DELBA_REASON_LEN                 2

// value of the The Fragment Number
#define MAC_BARINFO_SEQ_FRAG                    0

// offset of The DELPARAMSET
#define MAC_DELPARAMSET_START_OFT               0
#define MAC_DELPARAMSET_STEP_OFT                1

// Bit position for Parameter Set field
#define MAC_BA_PARAMSET_AMSDU_OFT               0
#define MAC_BA_PARAMSET_AMSDU_MASK              CO_BIT(0)

#define MAC_BA_PARAMSET_POLICY_OFT              1
#define MAC_BA_PARAMSET_POLICY_MASK             CO_BIT(1)

#define MAC_BA_PARAMSET_TID_OFT                 2
#define MAC_BA_PARAMSET_TID_MASK                (CO_BIT(2)|CO_BIT(3)|CO_BIT(4)|CO_BIT(5))

#define MAC_BA_PARAMSET_BUFFER_OFT              6
#define MAC_BA_PARAMSET_BUFFER_MASK             0xFFC0

//Bit position for Per TID Info field
#define MAC_BA_BARINFO_PERTIDINFO_TID_OFT       12
#define MAC_BA_BARINFO_PERTIDINFO_TID_MASK  (CO_BIT(12)|CO_BIT(13))|(CO_BIT(14)|CO_BIT(15))

//Bit position for Starting Sequence Control filed
#define MAC_BA_BARINFO_SEQ_FRAG_OFT             0
#define MAC_BA_BARINFO_SEQ_FRAG_MASK        (CO_BIT(0)|CO_BIT(1))|(CO_BIT(2)|CO_BIT(3))

#define MAC_BA_BARINFO_SEQ_STARTSEQ_OFT         4
#define MAC_BA_BARINFO_SEQ_STARTSEQ_MASK        0x7FF0

//Bit position for DELBA Parameter Set field
#define MAC_BA_DELPARAMSET_INIT_OFT             3
#define MAC_BA_DELBARINFO_INIT_MASK             CO_BIT(3)

#define MAC_BA_DELPARAMSET_TID_OFT              4
#define MAC_BA_DELPARAMSET_TID_MASK         (CO_BIT(4)|CO_BIT(5)|CO_BIT(6)|CO_BIT(7))

/*
*---------------------------------------------------------------------------------------
*---------------------------------Block Ack Status Codes-----------------------------------
*---------------------------------------------------------------------------------------
*/
#define MAC_BA_ST_SUCCESS                     0
#ifdef CFG_LPM
#define MAC_BA_ST_REQUEST_REFUSED             1
#else
#define MAC_BA_ST_REQUEST_REFUSED             37
#endif
#define MAC_BA_ST_INVALID_PARAMETERS          38

/*
*---------------------------------------------------------------------------------------
*---------------------------------Block Ack Reason Codes-----------------------------------
*---------------------------------------------------------------------------------------
*/
#define MAC_BA_RS_STA_LEAVING                 36
#define MAC_BA_RS_END_BA                      37
#define MAC_BA_RS_UNKNOWN_BA                  38
#define MAC_BA_RS_TIMEOUT                     39

/*
*---------------------------------------------------------------------------------------
*---------------------------------Block Ack Action Frames----------------------------------
*---------------------------------------------------------------------------------------
*/


/* CATEGORY \ ACTION values
*--------------------------------------------------------------------------------------*/

#define MAC_BA_ACTION_ADDBA_REQ         0
#define MAC_BA_ACTION_ADDBA_RSP         1
#define MAC_BA_ACTION_DELBA             2


/* ADDBA_REQ offsets
*--------------------------------------------------------------------------------------*/

#define MAC_ADDBA_REQ_PARAMSET_OFT      3
#define MAC_ADDBA_REQ_TIMEOUT_OFT       5
#define MAC_ADDBA_REQ_BARINFO_OFT       7


/* ADDBA_RSP offsets
*--------------------------------------------------------------------------------------*/

#define MAC_ADDBA_RSP_STATUS_OFT        3
#define MAC_ADDBA_RSP_PARAMSET_OFT      5
#define MAC_ADDBA_RSP_TIMEOUT_OFT       7


/* DELBA offsets
*--------------------------------------------------------------------------------------*/

#define MAC_DELBA_PARAMSET_OFT          2
#define MAC_DELBA_REASON_OFT            4

/** @} */


/**
 * @name TDLS Action Frames definitions
 * @{
 ****************************************************************************************
 */
/* CATEGORY \ ACTION values
*--------------------------------------------------------------------------------------*/
/// 802.11 TDLS action frame definition
#define MAC_TDLS_ACTION_SETUP_REQ           0
#define MAC_TDLS_ACTION_SETUP_RSP           1
#define MAC_TDLS_ACTION_SETUP_CFM           2
#define MAC_TDLS_ACTION_TEARDOWN            3
#define MAC_TDLS_ACTION_PEER_TRAFFIC_IND    4
#define MAC_TDLS_ACTION_CHANSW_REQ          5
#define MAC_TDLS_ACTION_CHANSW_RSP          6
#define MAC_TDLS_ACTION_PEER_TRAFFIC_RSP    9
#define MAC_TDLS_ACTION_DISCOVERY_REQ       10

#define TDLS_CHANSW_REQ_TARGET_CH_OFFSET    2
#define TDLS_CHANSW_REQ_OP_CLASS            3
#define TDLS_CHANSW_REQ_IES_OFFSET          4
#define TDLS_CHANSW_REQ_IE_SEC_CH_OFT_LEN   3
#define TDLS_CHANSW_REQ_IE_LINK_ID_LEN      20
#define TDLS_CHANSW_REQ_IE_CH_SWITCH_TIMING_LEN 6
#define TDLS_CHANSW_REQ_IE_WIDE_BW_CHAN_SWITCH_LEN 5

#define TDLS_CHANSW_RSP_STATUS_OFFSET       2
#define TDLS_CHANSW_RSP_IES_OFFSET          4

#define TDLS_CHANSW_REQUEST_ACCEPTED        0
#define TDLS_CHANSW_REQUEST_DECLINED        37

#define TDLS_PEER_TRAFFIC_IND_IES_OFFSET    3
#define TDLS_PEER_TRAFFIC_IND_IE_LINK_ID_LEN 20
#define TDLS_PEER_TRAFFIC_IND_IE_PTI_CTRL_LEN 5
#define TDLS_PEER_TRAFFIC_IND_IE_TPU_BUF_STATUS_LEN 3

#define TDLS_PEER_TRAFFIC_RSP_IES_OFFSET    3
#define TDLS_PEER_TRAFFIC_RSP_IE_LINK_ID_LEN 20

/** @} */


/**
 * @name Link Management definitions
 * @{
 ****************************************************************************************
 */
/// 802.11 Link Management frame definition
#define MAC_HEAD_HT_OFT                  32
#define MAC_MAI_MASK                     0x003C
#define MAC_MAI_OFT                      2
#define MAC_MAI_IND_ASELI                14
#define MAC_MFB_MASK                     0xFE00
#define MAC_MFB_OFT                      9
#define NO_FEEDBACK                      127
#define MAC_ADDR2_OFT                    10
#define MCS_IDX3_MASK                    0x0000FF
#define MCS_IDX2_MASK                    0x00FF00
#define MCS_IDX1_MASK                    0xFF0000
#define MCS_IDX2_OFT                     8
#define MCS_IDX1_OFT                     16

/** @} */

/**
 * @name Spectrum and Power Management Frame definitions
 * @{
 ****************************************************************************************
 */
/// 802.11 Spectrum and Power Management frame definition
#define MAC_ACTION_FRAME_INFO_OFT           3
#define MAC_ACTION_CH_SWITCH_FRAME_INFO_OFT 2
#define MAC_ACTION_FRAME_PRE_INFO_LEN       3

//Spectrum & Power Management Element Offset

// Power Constraint Element
#define MAC_INFOELT_POWER_CONSTRAINT_OFT     2

#define MAC_POWER_CONSTRAINT_ELT_LEN         3

// Power Capability Element
#define MAC_INFOELT_MIN_TX_POWER_OFT         2
#define MAC_INFOELT_MAX_TX_POWER_OFT         3

#define MAC_POWER_CAPABILITY_ELT_LEN         4
#define MAC_POWER_CAPABILITY_ELT_LEN_FIELD   2

// TPC Report Info Element sub-offsets
#define MAC_INFOELT_TX_POWER_OFT             2
#define MAC_INFOELT_LINK_MARGIN_OFT          3

// TPC Report and Request IE lengths
#define MAC_TPC_REQUEST_ELT_LEN              2
#define MAC_TPC_REPORT_ELT_LEN               4
// TPC Report and Request IE's length field values
#define MAC_INFOELT_LEN_VAL_TPC_REQ          0
#define MAC_INFOELT_LEN_VAL_TPC_REP          2
// TPC Report and Request Action frame lengths
#define MAC_ACTION_FRAME_TPC_REQ_LEN         5
#define MAC_ACTION_FRAME_TPC_REP_LEN         7


// Supported Channels Element
#define MAC_INFOELT_FIRST_CHANNEL_OFT        2
#define MAC_INFOELT_NUMBER_CHANNEL_OFT       3

// Channel Switch Announcement Element
#define MAC_INFOELT_SWITCH_MODE_OFT          2
#define MAC_INFOELT_SWITCH_NEW_CHAN_OFT      3
#define MAC_INFOELT_SWITCH_COUNT_OFT         4

#define MAC_CHANNEL_SWITCH_INFO_LEN          3
#define MAC_CHANNEL_SWITCH_ELT_LEN           5

// Extended Channel Switch Announcement Element
#define MAC_INFOELT_EXT_SWITCH_MODE_OFT      2
#define MAC_INFOELT_EXT_SWITCH_NEW_CLASS     3
#define MAC_INFOELT_EXT_SWITCH_NEW_CHAN_OFT  4
#define MAC_INFOELT_EXT_SWITCH_COUNT_OFT     5

#define MAC_EXT_CHANNEL_SWITCH_INFO_LEN      4
#define MAC_EXT_CHANNEL_SWITCH_ELT_LEN       6

// Secondary Channel Offset Element
#define MAC_INFOELT_SEC_CH_OFFSET_OFT        2

#define MAC_INFOELT_SEC_CH_OFFSET_INFO_LEN   1
#define MAC_INFOELT_SEC_CH_OFFSET_ELT_LEN    3

#define MAC_INFOELT_SEC_CH_OFFSET_SEC_CH_OFT 2
#define MAC_INFOELT_SEC_CH_OFFSET_NO_SEC_CH  0
#define MAC_INFOELT_SEC_CH_OFFSET_SEC_ABOVE  1
#define MAC_INFOELT_SEC_CH_OFFSET_SEC_BELOW  3

// Wide BAndwidth Channel switch
#define MAC_INFOELT_WIDE_BW_CHAN_SWITCH_NEW_CW_OFT 2
#define MAC_INFOELT_WIDE_BW_CHAN_SWITCH_NEW_CENTER1_OFT 3
#define MAC_INFOELT_WIDE_BW_CHAN_SWITCH_NEW_CENTER2_OFT 4
#define MAC_INFOELT_WIDE_BW_CHAN_SWITCH_INFO_LEN 3
#define MAC_INFOELT_WIDE_BW_CHAN_SWITCH_ELT_LEN  5

// Measurement Request/Report Element
#define MAC_INFOELT_MEASURE_TOKEN_OFT        2
#define MAC_INFOELT_MEASURE_MODE_OFT         3
#define MAC_INFOELT_MEASURE_TYPE_OFT         4
#define MAC_INFOELT_MEASURE_CH_NBR_OFT       5
#define MAC_INFOELT_MEASURE_START_TIME_OFT   6
#define MAC_INFOELT_MEASURE_DURATION_OFT     14

#define MAC_INFOELT_MEASURE_INFO_OFT        16

#define MAC_MEAS_REQ_CNF_ELT_LEN             5
#define MAC_MEAS_REQ_ELT_LEN                16
#define MAC_MEAS_REQ_FRAME_LEN              19
#define MAC_MEAS_REP_BASIC_ELT_LEN          17
#define MAC_MEAS_REP_BASIC_FRAME_LEN        20
#define MAC_MEAS_REP_CCA_ELT_LEN            17
#define MAC_MEAS_REP_CCA_FRAME_LEN          20
#define MAC_MEAS_REP_RPI_ELT_LEN            24
#define MAC_MEAS_REP_RPI_FRAME_LEN          27
#define MAC_MEAS_REP_ERR_ELT_LEN             5
#define MAC_MEAS_REP_ERR_FRAME_LEN           8

// Mobility Domain Element (MDE)
#define MAC_INFOELT_MDE_ID_OFT               0
#define MAC_INFOELT_MDE_LEN_OFT              1
#define MAC_INFOELT_MDE_MDID_OFT             2
#define MAC_INFOELT_MDE_FT_CAPA_POL_OFT      4
#define MAC_INFOELT_MDE_MDID_LEN             2
#define MAC_INFOELT_MDE_FT_CAPA_POL_LEN      1
#define MAC_INFOELT_MDE_ELMT_LEN             3
#define MAC_INFOELT_MDE_LEN                  5

// Timeout Interval Element (TIE)
#define MAC_INFOELT_TIE_ID_OFT               0
#define MAC_INFOELT_TIE_LEN_OFT              1
#define MAC_INFOELT_TIE_TYPE_OFT             2
#define MAC_INFOELT_TIE_VALUE_OFT            3

// Fast Transition Element (FTE)
#define MAC_INFOELT_FTE_ID_OFT               0
#define MAC_INFOELT_FTE_LEN_OFT              1
#define MAC_INFOELT_FTE_MIC_CTRL_OFT         2
#define MAC_INFOELT_FTE_MIC_CTRL_LEN         2
#define MAC_INFOELT_FTE_MIC_OFT              4
#define MAC_INFOELT_FTE_MIN_LEN              84

// Link Identifier Element
#define MAC_INFOELT_LINK_ID_LEN              18
#define MAC_INFOELT_LINK_ID_BSSID_OFT        2
#define MAC_INFOELT_LINK_ID_INIT_STA_OFT     8
#define MAC_INFOELT_LINK_ID_RESP_STA_OFT     14

// Link Identifier Element
#define MAC_INFOELT_CH_SWITCH_TIMING_LEN     4
#define MAC_INFOELT_CH_SWITCH_TIMING_SWTIME_OFT 2
#define MAC_INFOELT_CH_SWITCH_TIMING_SWTOUT_OFT 4

// PTI Control Element
#define MAC_INFOELT_PTI_CONTROL_LEN          3
#define MAC_INFOELT_PTI_CONTROL_TID_OFT      2
#define MAC_INFOELT_PTI_CONTROL_SEQ_CTRL_OFT 3

// TPU Buffer Status Element
#define MAC_INFOELT_TPU_BUF_STATUS_LEN       1
#define MAC_INFOELT_TPU_BUF_STATUS_AC_STATUS 2

// Spectrum management frame values:
//----------------------------------

//TO_DO_MMH:Remove unused fields

//MAC Action Spectrum Categorie => Sub Action
/// 802.11 Spectrum management frame definition
#define MAC_ACTION_MEASURE_REQUEST        0
#define MAC_ACTION_MEASURE_REPORT         1
#define MAC_ACTION_TPC_REQUEST            2
#define MAC_ACTION_TPC_REPORT             3
#define MAC_ACTION_CHANNEL_SWITCH         4

// Channel Switch Annoncement Mode
#define MAC_SWITCH_MODE_TX_TRAFFIC_CONT   0
#define MAC_SWITCH_MODE_TX_TRAFFIC_STOP   1

// Measurement Request Mode
#define MAC_MEASURE_MODE_ENABLE           CO_BIT(1)
#define MAC_MEASURE_MODE_REQUEST          CO_BIT(2)
#define MAC_MEASURE_MODE_REPORT           CO_BIT(3)

// Measurement Report Mode
#define MAC_MEASURE_MODE_LATE             CO_BIT(0)
#define MAC_MEASURE_MODE_INCAPABLE        CO_BIT(1)
#define MAC_MEASURE_MODE_REFUSED          CO_BIT(2)

// Measurement Request Type
#define MAC_MEASURE_TYPE_BASIC            0
#define MAC_MEASURE_TYPE_CCA              1
#define MAC_MEASURE_TYPE_RPI              2

// Measurement Report Map Bits
#define MAC_MEASURE_MAP_FREE              0x00
#define MAC_MEASURE_MAP_BSS               CO_BIT(0)
#define MAC_MEASURE_MAP_OFDM              CO_BIT(1)
#define MAC_MEASURE_MAP_UNIDENTIFIED      CO_BIT(2)
#define MAC_MEASURE_MAP_RADAR             CO_BIT(3)
#define MAC_MEASURE_MAP_UNMEASURED        CO_BIT(4)
#define MAC_MEASURE_MAP_MASK              0x1F

/*
 * We set the default switch count to Zero which indicates that the switch shall occur at
 * any time after the frame containing the element is transmitted.
 * This is done to be able to give the STA time to know the new channel number in case the
 * channel switch fails and another new channel was decided
 */
#define MAC_DEFAULT_SWITCH_COUNT            0
#define MAC_DEFAULT_RADAR_CHECK_TIME        60

enum
{
    REMOTE_MEAS_REQ_LATE = 1,
    REMOTE_MEAS_REQ_INCAPABLE,
    REMOTE_MEAS_REQ_REFUSED
};

/** @} */

/**
 * @name Radio Measurement sub elements
 * @{
 ****************************************************************************************
 */
// Beacon Request sub elements (offset from MAC_MEAS_REQ_BCN_SUB_ELEM_OFT)
enum mac_measure_req_bcn_sub_elem {
    MAC_MEAS_REQ_BCN_SUBELTID_SSID = 0,                  // same as element
    MAC_MEAS_REQ_BCN_SUBELTID_BCN_REPORTING = 1,
    MAC_MEAS_REQ_BCN_SUBELTID_REPORT_DETAIL = 2,
    MAC_MEAS_REQ_BCN_SUBELTID_REQUEST = 10,              // same as element
    MAC_MEAS_REQ_BCN_SUBELTID_REQUEST_EXT = 11,          // same as element (different ID)
    MAC_MEAS_REQ_BCN_SUBELTID_AP_CHAN_REPORT = 51,       // same as element
    MAC_MEAS_REQ_BCN_SUBELTID_WIDE_BW_CHAN_SWITCH = 163, // same as element
    MAC_MEAS_REQ_BCN_SUBELTID_LAST_BCN_REPORT_IND = 164,
};

// Beacon Reporting
#define MAC_BCN_REPORTING_COND_OFT 2
enum mac_bcn_reporting_cond {
    MAC_BCN_REPORTING_COND_ALL = 0,
    MAC_BCN_REPORTING_COND_GT_RCPI = 1,
    MAC_BCN_REPORTING_COND_LT_RCPI = 2,
    MAC_BCN_REPORTING_COND_GT_RSNI = 3,
    MAC_BCN_REPORTING_COND_LT_RSNI = 4,
    MAC_BCN_REPORTING_COND_GT_AP_RCPI = 5,
    MAC_BCN_REPORTING_COND_LT_AP_RCPI = 6,
    MAC_BCN_REPORTING_COND_GT_AP_RSNI = 7,
    MAC_BCN_REPORTING_COND_LT_AP_RSNI = 8,
    MAC_BCN_REPORTING_COND_RANGE_AP_RCPI = 9,
    MAC_BCN_REPORTING_COND_RANGE_AP_RSNI = 10,
};
#define MAC_BCN_REPORTING_THRESHOLD_OFT 3

// Beacon Detail
#define MAC_BCN_REPORT_DETAIL_OFT 2
enum mac_bcn_report_detail {
    MAC_BCN_REPORT_DETAIL_NONE = 0,
    MAC_BCN_REPORT_DETAIL_FIXED = 1,
    MAC_BCN_REPORT_DETAIL_ALL = 2,
};

// Last Beacon Report Indication
#define MAC_BCN_LAST_BCN_REPORT_IND_REQ_OFT 2
#define MAC_BCN_LAST_BCN_REPORT_IND_REQ_LEN 3

// Beacon Report sub elements (offset from MAC_MEAS_REP_BCN_SUB_ELEM_OFT)
enum mac_measure_rep_bcn_sub_elem {
    MAC_MEAS_REP_BCN_SUBELTID_SSID = 0,                  // same as element
    MAC_MEAS_REP_BCN_SUBELTID_FRAME_BODY = 1,
    MAC_MEAS_REP_BCN_SUBELTID_FRAME_FRAG_ID = 2,
    MAC_MEAS_REP_BCN_SUBELTID_WIDE_BW_CHAN_SWITCH = 163, // same as element
    MAC_MEAS_REP_BCN_SUBELTID_LAST_BCN_REPORT_IND = 164,
};

// Frame body
#define MAC_BCN_FRAME_BODY_OFT 2

// Frame body fragment ID
#define MAC_BCN_FRAME_BODY_FRAG_ID_OFT      2
#define MAC_BCN_FRAME_REPORT_ID_OFT         0
#define MAC_BCN_FRAME_REPORT_ID_MSK         (0xFF << MAC_BCN_FRAME_REPROT_ID_OFT)
#define MAC_BCN_FRAME_FRAG_ID_OFT           8
#define MAC_BCN_FRAME_FRAG_ID_MSK           (0x7F << MAC_BCN_FRAME_FRAG_ID_OFT)
#define MAC_BCN_FRAME_MORE_FRAG_BIT         CO_BIT(15)
#define MAC_BCN_FRAME_BODY_FRAG_ID_ELMT_LEN 2
#define MAC_BCN_FRAME_BODY_FRAG_ID_LEN      4


// Last Beacon Reporot Indication
#define MAC_BCN_LAST_BCN_REPORT_IND_OFT      2
#define MAC_BCN_LAST_BCN_REPORT_IND_ELMT_LEN 1
#define MAC_BCN_LAST_BCN_REPORT_IND_LEN      3

/** @} */

/**
 * @name Radio Measurement Action frame definitions
 * @{
 ****************************************************************************************
 */
enum mac_radio_measurement_action {
    MAC_RM_ACTION_REQUEST = 0,
    MAC_RM_ACTION_REPORT = 1,
    MAC_RM_ACTION_LINK_REQUEST = 2,
    MAC_RM_ACTION_LINK_REPORT = 3,
    MAC_RM_ACTION_NEIGHBOR_REQUEST = 4,
    MAC_RM_ACTION_NEIGHBOR_REPORT = 5,
};

// request
#define MAC_RM_ACTION_REQ_NUMBER_REPEAT_OFT 3
#define MAC_RM_ACTION_REQ_MEASURE_REQ_OFT   5

// report
#define MAC_RM_ACTION_REP_MEASURE_REP_OFT 3

/** @} */

/**
 * @name HT Action Frame definitions
 * @{
 ****************************************************************************************
 */

/* HT Action Field values
*--------------------------------------------------------------------------------------*/
/// 802.11 HT action frame definition
#define MAC_CHAN_WIDTH_HT_ACTION                0
#define MAC_SMPS_HT_ACTION                      1
#define MAC_PSMP_HT_ACTION                      2
#define MAC_SET_PCO_PHASE_HT_ACTION             3
#define MAC_CSI_HT_ACTION                       4
#define MAC_NON_COMP_BEAMFORMING_HT_ACTION      5
#define MAC_COMP_BEAMFORMING_HT_ACTION          6
#define MAC_ASEL_INDICES_FEEDBACK_HT_ACTION     7


/* HT channel width offsets
*--------------------------------------------------------------------------------------*/
#define MAC_CHAN_WIDTH_WIDTH_OFT                2

/* HT SM Power Control field offset
*--------------------------------------------------------------------------------------*/
#define MAC_SM_PRW_CTRL_OFT                     2
#define MAC_SMPS_ENABLE_BIT                     CO_BIT(0)
#define MAC_SMPS_DYNAMIC_BIT                    CO_BIT(1)

/** @} */


/**
 * @name VHT Action Frame definitions
 * @{
 ****************************************************************************************
 */
/* VHT Action Field values
*--------------------------------------------------------------------------------------*/
/// 802.11 VHT action frame definition
#define MAC_COMP_BEAMFORMING_VHT_ACTION         0
#define MAC_GROUP_ID_MGMT_VHT_ACTION            1
#define MAC_OP_MODE_NOTIF_VHT_ACTION            2


/* GroupId management offsets
*--------------------------------------------------------------------------------------*/
#define MAC_GROUP_ID_MGT_PAYL_LENGTH            26
#define MAC_GROUP_ID_MGT_MEMBERSHIP_OFT         2
#define MAC_GROUP_ID_MGT_USER_POS_OFT           10


/* VHT operation mode notification offsets
*--------------------------------------------------------------------------------------*/
#define MAC_OP_MODE_NOTIF_OPMODE_OFT            2
#define MAC_OP_MODE_NOTIF_LEN                   3
#define MAC_OP_MODE_NOTIF_PARAM_LEN             1
#define MAC_OPMODE_BW_OFT                       0
#define MAC_OPMODE_BW_MSK                       (0x3 << MAC_OPMODE_BW_OFT)
#define MAC_OPMODE_RXNSS_OFT                    4
#define MAC_OPMODE_RXNSS_MSK                    (0x7 << MAC_OPMODE_RXNSS_OFT)
#define MAC_OPMODE_RXNSS_TYPE_BIT               CO_BIT(7)

/** @} */

/**
 * @name SA QUERY Action Frame definitions
 * @{
 ****************************************************************************************
 */
#define MAC_SA_QUERY_ACTION_OFT 1
#define MAC_SA_QUERY_REQUEST    0
#define MAC_SA_QUERY_RESPONSE   1

#define MAC_SA_QUERY_TR_ID_OFT  2
#define MAC_SA_QUERY_TR_ID_LEN  2

#define MAC_SA_QUERY_MIN_LEN  (MAC_SA_QUERY_TR_ID_OFT + MAC_SA_QUERY_TR_ID_LEN)
/** @} */

/**
 * @name Information Element Identifier definitions
 * @{
 ****************************************************************************************
 */
/// 802.11 IE identifier
#define MAC_INFOELT_ID_OFT                0
#define MAC_INFOELT_LEN_OFT               1
#define MAC_INFOELT_EXT_ID_OFT            2
#define MAC_INFOELT_INFO_OFT              2
#define MAC_INFOELT_EXT_INFO_OFT          3
#define MAC_INFOELT_INFO_MAX_LEN          255
#define MAC_INFOELT_MAX_LEN               257

#define MAC_ELTID_SSID                    0
#define MAC_ELTID_RATES                   1
#define MAC_ELTID_FH                      2
#define MAC_ELTID_DS                      3
#define MAC_ELTID_CF                      4
#define MAC_ELTID_TIM                     5
#define MAC_ELTID_IBSS                    6
#define MAC_ELTID_COUNTRY                 7             // optional
#define MAC_ELTID_HOP_PARAM               8             // optional
#define MAC_ELTID_HOP_TABLE               9             // optional
#define MAC_ELTID_REQUEST                10             // optional
#define MAC_ELTID_BSS_LOAD               11
#define MAC_ELTID_EDCA_PARAM             12

#define MAC_ELTID_CHALLENGE              16
#define MAC_ELTID_POWER_CONSTRAINT       32             // Spectrum & Power Management
#define MAC_ELTID_POWER_CAPABILITY       33             // Spectrum & Power Management
#define MAC_POWER_CAPABILITY_IE_LEN      2              // Spectrum & Power Management
#define MAC_ELTID_TPC_REQUEST            34             // Spectrum & Power Management
#define MAC_ELTID_TPC_REPORT             35             // Spectrum & Power Management
#define MAC_ELTID_SUPPORTED_CHANNELS     36             // Spectrum & Power Management
#define MAC_ELTID_CHANNEL_SWITCH         37             // Spectrum & Power Management
#define MAC_ELTID_MEASUREMENT_REQUEST    38             // Spectrum & Power Management
#define MAC_ELTID_MEASUREMENT_REPORT     39             // Spectrum & Power Management
#define MAC_ELTID_QUIET                  40             // Spectrum & Power Management
#define MAC_ELTID_IBSS_DFS               41             // Spectrum & Power Management

#define MAC_ELTID_ERP                    42
#define MAC_ELTID_HT_CAPA                45
#define MAC_ELTID_QOS_CAPA               46
#define MAC_ELTID_RSN_IEEE               48
#define MAC_ELTID_EXT_RATES              50
#define MAC_ELTID_MDE                    54  // Mobility Domain
#define MAC_ELTID_FTE                    55  // Fast BSS Transition
#define MAC_ELTID_TIE                    56  // Timeout Interval
#define MAC_ELTID_SUPP_OPER_CLASS        59
#define MAC_ELTID_EXT_CHANNEL_SWITCH     60
#define MAC_ELTID_HT_OPERATION           61
#define MAC_ELTID_SEC_CH_OFFSET          62
#define MAC_ELTID_WAPI                   68
#define MAC_ELTID_RM_ENABLED_CAPA        70
#define MAC_ELTID_MULTIPLE_BSSID         71
#define MAC_ELTID_20_40_COEXISTENCE      72
#define MAC_ELTID_OBSS_SCAN_PARAM        74
#define MAC_ELTID_MGMT_MIC               76
#define MAC_ELTID_NON_TXED_BSSID_CAPA    83
#define MAC_ELTID_MULTI_BSSID_INDEX      85
#define MAC_ELTID_LINK_IDENTIFIER        101
#define MAC_ELTID_CHANNEL_SWITCH_TIMING  104
#define MAC_ELTID_PTI_CONTROL            105
#define MAC_ELTID_TPU_BUFFER_STATUS      106
#define MAC_ELTID_MESH_CONF              113
#define MAC_ELTID_MESH_ID                114
#define MAC_ELTID_MESH_LINK_METRIC_REP   115
#define MAC_ELTID_CONGESTION_NOTIF       116
#define MAC_ELTID_MESH_PEER_MGMT         117
#define MAC_ELTID_MESH_CHAN_SWITCH_PARAM 118
#define MAC_ELTID_MESH_AWAKE_WINDOW      119
#define MAC_ELTID_MESH_GANN              125
#define MAC_ELTID_MESH_RANN              126
#define MAC_ELTID_EXT_CAPA               127
#define MAC_ELTID_MESH_PREQ              130
#define MAC_ELTID_MESH_PREP              131
#define MAC_ELTID_MESH_PERR              132
#define MAC_ELTID_VHT_CAPA               191
#define MAC_ELTID_VHT_OPERATION          192
#define MAC_ELTID_WIDE_BANDWIDTH_CHAN_SWITCH 194
#define MAC_ELTID_VHT_TRANSMIT_PWR_ENVELOP   195
#define MAC_ELTID_CHAN_SWITCH_WRP        196
#define MAC_ELTID_OP_MODE_NOTIF          199
#define MAC_ELTID_FTM_PARAMS             206
#define MAC_ELTID_TWT                    216
#define MAC_ELTID_S1G_CAPA               217
#define MAC_ELTID_OUI                    221 // Proprietary   (0xDD)
#define MAC_ELTID_S1G_OP                 232
#define MAC_ELTID_EXT                    255

#define MAC_ELTID_EXT_EXTENDED_REQUEST   10
#define MAC_ELTID_EXT_HE_CAPA            35
#define MAC_ELTID_EXT_HE_OPERATION       36
#define MAC_ELTID_EXT_UORA               37
#define MAC_ELTID_EXT_MU_EDCA            38

#define MAC_OUI_TYPE_WPA                 1
#define MAC_OUI_TYPE_WME                 2
#define MAC_OUI_SUBTYPE_WME_INFORMATION  0
#define MAC_OUI_SUBTYPE_WME_PARAMETER    1
#define MAC_OUI_TYPE_WMMAC               2
#define MAC_OUI_SUBTYPE_WMMAC_TSPEC      2
#define MAC_OUI_SUBTYPE_WMMAC_TSDELAY    8
#define MAC_OUI_SUBTYPE_WMMAC_TSINFO     10

/** @} */

/**
 * @name Information Subelement Identifier definitions for Multiple BSSID element
 * @{
 ****************************************************************************************
 */
/// 802.11 Multiple BSSID subelement identifier
#define MAC_SUBID_MBSSID_NON_TXED_PROF   0
#define MAC_SUBID_MBSSID_VENDOR_SPECIFIC 221
/** @} */

/**
 * @name Standard Status Code definitions
 * @{
 ****************************************************************************************
 */
/// 802.11 Status Code
#define MAC_ST_SUCCESSFUL                   0
#define MAC_ST_FAILURE                      1
#define MAC_ST_RESERVED                     2
#define MAC_ST_CAPA_NOT_SUPPORTED           10
#define MAC_ST_REASSOC_NOT_ASSOC            11
#define MAC_ST_ASSOC_DENIED                 12
#define MAC_ST_AUTH_ALGO_NOT_SUPPORTED      13
#define MAC_ST_AUTH_FRAME_WRONG_SEQ         14
#define MAC_ST_AUTH_CHALLENGE_FAILED        15
#define MAC_ST_AUTH_TIMEOUT                 16
#define MAC_ST_ASSOC_TOO_MANY_STA           17
#define MAC_ST_ASSOC_RATES_NOT_SUPPORTED    18
#define MAC_ST_ASSOC_PREAMBLE_NOT_SUPPORTED 19

#define MAC_ST_ASSOC_SPECTRUM_REQUIRED   22
#define MAC_ST_ASSOC_POWER_CAPA          23
#define MAC_ST_ASSOC_SUPPORTED_CHANNEL   24
#define MAC_ST_ASSOC_SLOT_NOT_SUPPORTED  25

#define MAC_ST_REFUSED_TEMPORARILY       30
#define MAC_ST_INVALID_MFP_POLICY        31

#define MAC_ST_INVALID_IE                40             // draft 7.0 extention
#define MAC_ST_GROUP_CIPHER_INVALID      41             // draft 7.0 extention
#define MAC_ST_PAIRWISE_CIPHER_INVALID   42             // draft 7.0 extention
#define MAC_ST_AKMP_INVALID              43             // draft 7.0 extention
#define MAC_ST_UNSUPPORTED_RSNE_VERSION  44             // draft 7.0 extention
#define MAC_ST_INVALID_RSNE_CAPA         45             // draft 7.0 extention
#define MAC_ST_CIPHER_SUITE_REJECTED     46             // draft 7.0 extention
/** @} */


/**
 * @name Standard Reason Code definitions
 * @{
 ****************************************************************************************
 */
/// 802.11 Reason Code
#define MAC_RS_RESERVED                           0
#define MAC_RS_UNSPECIFIED                        1
#define MAC_RS_PREVIOUS_AUTH_NOT_VALID            2
#define MAC_RS_DEAUTH_SENDER_LEFT_IBSS_ESS        3
#define MAC_RS_DIASSOC_INACTIVITY                 4
#define MAC_RS_DIASSOC_TOO_MANY_STA               5
#define MAC_RS_CLASS_2_FORBIDDEN                  6
#define MAC_RS_CLASS_3_FORBIDDEN                  7
#define MAC_RS_DEAUTH_SENDER_LEFT_BSS             8
#define MAC_RS_STA_REQ_ASSOC_NOT_AUTH             9
#define MAC_RS_DIASSOC_POWER_CAPA                10
#define MAC_RS_DIASSOC_SUPPORTED_CHANNEL         11
#define MAC_RS_RESERVED_12                       12
#define MAC_RS_INVALID_IE                        13
#define MAC_RS_MIC_FAILURE                       14
#define MAC_RS_4WAY_HANDSHAKE_TIMEOUT            15
#define MAC_RS_GROUP_KEY_UPDATE_TIMEOUT          16
#define MAC_RS_BAD_RSNIE                         17
#define MAC_RS_GROUP_CIPHER_INVALID              18
#define MAC_RS_PAIRWISE_CIPHER_INVALID           19
#define MAC_RS_AKMP_INVALID                      20
#define MAC_RS_UNSUPPORTED_RSNE_VERSION          21
#define MAC_RS_INVALID_RSNE_CAPA                 22
#define MAC_RS_802_1X_AUTH_FAIL                  23
#define MAC_RS_CIPHER_SUITE_REJECTED             24
#define MAC_RS_TIMEOUT                           39
/** @} */


/**
 * @name WMMIE definitions
 * @{
 ****************************************************************************************
 */
/// WMM IE definition
#define MAC_WME_PARAM_LEN          16
/// Default WME information element format
#define MAC_RAW_WME_INFO_ELMT_DEFAULT                                         \
{                                                                             \
    MAC_ELTID_OUI, 0x07,                                                      \
    0x00, 0x50, 0xF2, MAC_OUI_TYPE_WME,         /* additionnal WPA OUI     */ \
    MAC_OUI_SUBTYPE_WME_INFORMATION, 0x01,      /* OUI subtype and version */ \
    0x00                                        /* QoS field               */ \
}

// Best effort
#define MAC_ACI_BE                        0
// Background
#define MAC_ACI_BK                        1
// Video
#define MAC_ACI_VI                        2
// Voice
#define MAC_ACI_VO                        3

#define MAC_ACI_OFT                       5

#define MAC_ECWMIN                        4
#define MAC_ECWMAX                        10

#define MAC_ECW_BK_MIN                  MAC_ECWMIN
#define MAC_ECW_BE_MIN                  MAC_ECWMIN
//#define MAC_ECW_VI_MIN                  ((MAC_ECWMIN+1)/2 - 1)
//#define MAC_ECW_VO_MIN                  ((MAC_ECWMIN+1)/4 - 1)
#define MAC_ECW_VI_MIN                  ((MAC_ECWMIN) - 1)
#define MAC_ECW_VO_MIN                  ((MAC_ECWMIN)/2)

#define MAC_ECW_BK_MAX                  MAC_ECWMAX
#define MAC_ECW_BE_MAX                  MAC_ECWMAX
#define MAC_ECW_VI_MAX                  MAC_ECWMIN
//#define MAC_ECW_VO_MAX                  ((MAC_ECWMIN+1)/2 - 1)
#define MAC_ECW_VO_MAX                  ((MAC_ECWMIN) - 1)

#define MAC_AIFSN_BK                    7
#define MAC_AIFSN_BE                    3
#define MAC_AIFSN_VI                    2
#define MAC_AIFSN_VO                    2

#define MAC_AC_BE_TXOP                  0
#define MAC_AC_BK_TXOP                  0
#define MAC_AC_VI_TXOP_b                188    // 6016us / 32 us
#define MAC_AC_VI_TXOP_ag               94     // 3008 ms / 32 usec


#define MAC_AC_VO_TXOP_b                102    // 3264us / 32 us
#define MAC_AC_VO_TXOP_ag                47    // 1504us / 32 us


 /** @} */

/**
 * @name RSNIE definitions
 * @{
 ****************************************************************************************
 */
// Cipher suites
#define MAC_RSNIE_CIPHER_USE_GROUP            0x000FAC00
#define MAC_RSNIE_CIPHER_WEP_40               0x000FAC01
#define MAC_RSNIE_CIPHER_TKIP                 0x000FAC02
#define MAC_RSNIE_CIPHER_RSVD                 0x000FAC03
#define MAC_RSNIE_CIPHER_CCMP_128             0x000FAC04
#define MAC_RSNIE_CIPHER_WEP_104              0x000FAC05
#define MAC_RSNIE_CIPHER_BIP_CMAC_128         0x000FAC06
#define MAC_RSNIE_CIPHER_NO_GROUP             0x000FAC07
#define MAC_RSNIE_CIPHER_GCMP_128             0x000FAC08
#define MAC_RSNIE_CIPHER_GCMP_256             0x000FAC09
#define MAC_RSNIE_CIPHER_CCMP_256             0x000FAC0A
#define MAC_RSNIE_CIPHER_BIP_GMAC_128         0x000FAC0B
#define MAC_RSNIE_CIPHER_BIP_GMAC_256         0x000FAC0C
#define MAC_RSNIE_CIPHER_BIP_CMAC_256         0x000FAC0D

// AKM suites
#define MAC_RSNIE_AKM_8021X                   0x000FAC01
#define MAC_RSNIE_AKM_PSK                     0x000FAC02
#define MAC_RSNIE_AKM_FT_8021X                0x000FAC03
#define MAC_RSNIE_AKM_FT_PSK                  0x000FAC04
#define MAC_RSNIE_AKM_8021X_SHA256            0x000FAC05
#define MAC_RSNIE_AKM_PSK_SHA256              0x000FAC06
#define MAC_RSNIE_AKM_TDLS                    0x000FAC07
#define MAC_RSNIE_AKM_SAE                     0x000FAC08
#define MAC_RSNIE_AKM_FT_OVER_SAE             0x000FAC09
#define MAC_RSNIE_AKM_8021X_SUITE_B           0x000FAC0B
#define MAC_RSNIE_AKM_8021X_SUITE_B_192       0x000FAC0C
#define MAC_RSNIE_AKM_FILS_SHA256             0x000FAC0D
#define MAC_RSNIE_AKM_FILS_SHA384             0x000FAC0F
#define MAC_RSNIE_AKM_FT_FILS_SHA256          0x000FAC10
#define MAC_RSNIE_AKM_FT_FILS_SHA384          0x000FAC11
#define MAC_RSNIE_AKM_OWE                     0x000FAC12

// RSN CAPA
#define MAC_RSNIE_CAPA_PREAUTH_BIT            CO_BIT(0)
#define MAC_RSNIE_CAPA_NO_PAIRWISE_BIT        CO_BIT(1)
#define MAC_RSNIE_CAPA_PTKSA_REPLAY_CNT_MSK   0x000C
#define MAC_RSNIE_CAPA_GTKSA_REPLAY_CNT_MSK   0x0030
#define MAC_RSNIE_CAPA_MFPR_BIT               CO_BIT(6)
#define MAC_RSNIE_CAPA_MFPC_BIT               CO_BIT(7)
#define MAC_RSNIE_CAPA_JOINT_MULIT_BAND_BIT   CO_BIT(8)
#define MAC_RSNIE_CAPA_PEERKEY_BIT            CO_BIT(9)
#define MAC_RSNIE_CAPA_SPP_AMSDU_CAP_BIT      CO_BIT(10)
#define MAC_RSNIE_CAPA_SPP_AMSDU_REQ_BIT      CO_BIT(11)
#define MAC_RSNIE_CAPA_PBAC_BIT               CO_BIT(12)
#define MAC_RSNIE_CAPA_EXT_KEY_ID_BIT         CO_BIT(13)

/** @} */

/**
 * @name WPA (pre-RSN) definitions
 * @{
 ****************************************************************************************
 */
// Cipher suites
#define MAC_WPA_CIPHER_TKIP                   0x0050F202
#define MAC_WPA_CIPHER_CCMP                   0x0050F204

// AKM suites
#define MAC_WPA_AKM_8021X                     0x0050F201
#define MAC_WPA_AKM_PSK                       0x0050F202
/** @} */

/**
 * @name WAPI definitions
 * @{
 ****************************************************************************************
 */
// Cipher suites
#define MAC_WAPI_CIPHER_WPI_SMS4              0x00147201

// AKM suites
#define MAC_WAPI_AKM_CERT                     0x00147201
#define MAC_WAPI_AKM_PSK                      0x00147202

/** @} */

/**
 * @name Country IE definitions
 * @{
 ****************************************************************************************
 */
/// Country IE definition
#define MAC_COUNTRY_2G4_FULL      {  3, {'x', 'x', ' '}, { 1, 14, 30}}
#define MAC_COUNTRY_2G4_USA       {  6, {'U', 'S', ' '}, { 1, 11, 30}} // X'10' FCC
#define MAC_COUNTRY_2G4_CANADA    {  6, {'C', 'A', ' '}, { 1, 11, 30}} // X'20' DOC/IC
#define MAC_COUNTRY_2G4_EUROPE    {  6, {'E', 'U', ' '}, { 1, 13, 30}} // X'30' ETSI
#define MAC_COUNTRY_2G4_SPAIN     {  6, {'S', 'P', ' '}, {10,  2, 30}} // X'31'
#define MAC_COUNTRY_2G4_FRANCE    {  6, {'F', 'R', ' '}, {10,  4, 20}} // X'32'
#define MAC_COUNTRY_2G4_JAPAN     {  6, {'J', 'P', ' '}, {14,  1, 30}} // X'40'
#define MAC_COUNTRY_2G4_CHINA     {  6, {'C', 'N', ' '}, { 1, 13, 30}} // X'50'

#define MAC_COUNTRY_5G_FULL       {  3, {'x', 'x', ' '}, {{ 36, 8, 23}}}
#define MAC_COUNTRY_5G_USA        { 18, {'U', 'S', ' '}, {{ 36, 4, 16}, /**/ {52, 4, 23}, /**/ {100, 11, 23}, /**/ {149, 4, 29}, /**/ {165, 1, 30}}}
#define MAC_COUNTRY_5G_FRANCE     { 12, {'F', 'R', ' '}, {{ 36, 4, 23}, /**/ {52, 4, 23}, /**/ {100, 11, 40}}}
#define MAC_COUNTRY_5G_JAPAN      { 12, {'J', 'P', ' '}, {{184, 4, 24}, /**/ { 8, 3, 24}, /**/ { 34,  4, 22}}}

/** @} */

/**
 * @name Authentication Algorithm definitions
 * @{
 ****************************************************************************************
 */
/// Authentication algorithm definition
#define MAC_AUTH_ALGO_OPEN                0
#define MAC_AUTH_ALGO_SHARED              1
#define MAC_AUTH_ALGO_FT                  2
#define MAC_AUTH_ALGO_SAE                 3


#define MAC_AUTH_FIRST_SEQ                1
#define MAC_AUTH_SECOND_SEQ               2
#define MAC_AUTH_THIRD_SEQ                3
#define MAC_AUTH_FOURTH_SEQ               4

/** @} */

/**
 * @name Security definitions
 * @{
 ****************************************************************************************
 */
/// Crypto definition
// Defines for encryption status
#define MAC_ENC_NONE               0
#define MAC_ENC_WEP                0x01
#define MAC_ENC_WPA                0x02
#define MAC_ENC_TKIP               0x04
#define MAC_ENC_WPA2               0x10
#define MAC_ENC_CCMP               0x20

// Key Offsets
#define  MAC_ENC_KEY_OFFSET         0
#define  MAC_AUTHENTICATOR_KEY     16
#define  MAC_SUPPLICANT_KEY        24
#define  MAC_ENC_KEY_LEN           16
#define  MAC_MICHAEL_KEY_LEN        8

/// Word 1 of CCMP IV
#define EIV_PRESENT     (0x2000)
/** @} */

/**
 * @name Various Frame Size definitions
 * @{
 ****************************************************************************************
 */
/// Maximum size of a beacon frame (default IE + TIM + remaining IE space)
#define MAC_BEACON_SIZE             (102 + MAC_TIM_SIZE + 128)
/// Maximum size of a probe request frame
#define MAC_PROBEREQ_SIZE           100
/// Maximum size of a probe response frame (default IE + remaining IE space)
#define MAC_PROBERSP_SIZE               (102 + 128)
/// Size of a NULL frame
#define MAC_NULL_FRAME_SIZE         MAC_SHORT_MAC_HDR_LEN
/// Size of a QoS-NULL frame
#define MAC_QOS_NULL_FRAME_SIZE     MAC_SHORT_QOS_MAC_HDR_LEN
/// Size of a PS poll
#define MAC_PSPOLL_FRAME_SIZE       MAC_LONG_CTRLFRAME_LEN
/** @} */

/// Structure of a short control frame MAC header
struct mac_hdr_ctrl_short
{
    /// Frame control
    uint16_t fctl;
    /// Duration/ID
    uint16_t durid;
    /// Address 1
    struct mac_addr addr1;
} __attribute__ ((__packed__));

/// Structure of a long control frame MAC header
struct mac_hdr_ctrl
{
    /// Frame control
    uint16_t fctl;
    /// Duration/ID
    uint16_t durid;
    /// Address 1
    struct mac_addr addr1;
    /// Address 2
    struct mac_addr addr2;
} __attribute__ ((__packed__));

/// Structure of a Data/Management frame MAC header
struct mac_hdr
{
    /// Frame control
    uint16_t fctl;
    /// Duration/ID
    uint16_t durid;
    /// Address 1
    struct mac_addr addr1;
    /// Address 2
    struct mac_addr addr2;
    /// Address 3
    struct mac_addr addr3;
    /// Sequence control
    uint16_t seq;
} __attribute__ ((__packed__));

/// Structure of a QoS Data frame MAC header
struct mac_hdr_qos
{
    /// Frame control
    uint16_t fctl;
    /// Duration/ID
    uint16_t durid;
    /// Address 1
    struct mac_addr addr1;
    /// Address 2
    struct mac_addr addr2;
    /// Address 3
    struct mac_addr addr3;
    /// Sequence control
    uint16_t seq;
    /// QoS control
    uint16_t qos;
} __attribute__ ((__packed__));

/// Structure of a QoS Data + HTC frame MAC header
struct mac_hdr_qos_htc
{
    /// Frame control
    uint16_t fctl;
    /// Duration/ID
    uint16_t durid;
    /// Address 1
    struct mac_addr addr1;
    /// Address 2
    struct mac_addr addr2;
    /// Address 3
    struct mac_addr addr3;
    /// Sequence control
    uint16_t seq;
    /// QoS control
    uint16_t qos;
    /// +HTC field
    uint32_t htc;
} __attribute__ ((__packed__));

/// Structure of a 4-address Data frame MAC header
struct mac_hdr_long
{
    /// Frame control
    uint16_t fctl;
    /// Duration/ID
    uint16_t durid;
    /// Address 1
    struct mac_addr addr1;
    /// Address 2
    struct mac_addr addr2;
    /// Address 3
    struct mac_addr addr3;
    /// Sequence control
    uint16_t seq;
    /// Address 4
    struct mac_addr addr4;
} __attribute__ ((__packed__));


/// Structure of a 4-address QoS Data frame MAC header
struct mac_hdr_long_qos
{
    /// Frame control
    uint16_t fctl;
    /// Duration/ID
    uint16_t durid;
    /// Address 1
    struct mac_addr addr1;
    /// Address 2
    struct mac_addr addr2;
    /// Address 3
    struct mac_addr addr3;
    /// Sequence control
    uint16_t seq;
    /// Address 4
    struct mac_addr addr4;
    /// QoS control
    uint16_t qos;
} __attribute__ ((__packed__));

/// Structure of Ethernet Header
struct mac_eth_hdr
{
    /// Destination Address
    struct mac_addr da;
    /// Source Address
    struct mac_addr sa;
    /// Length / Type
    uint16_t len;
} __attribute__ ((__packed__));

/// Structure of a Beacon or ProbeRsp frame
struct bcn_frame
{
    /// MAC Header
    struct mac_hdr h;
    /// Timestamp
    uint64_t tsf;
    /// Beacon interval
    uint16_t bcnint;
    /// Capability information
    uint16_t capa;
    /// Rest of the payload
    uint8_t variable[];
} __attribute__ ((__packed__));

/// Structure of a Probe Request frame
struct preq_frame
{
    /// MAC Header
    struct mac_hdr h;
    /// Rest of the payload
    uint8_t payload[];
} __attribute__ ((__packed__));

/// Structure of a Block Acknowledgment Request frame
struct bar_frame
{
    /// MAC Header
    struct mac_hdr_ctrl h;
    /// BAR control
    uint16_t bar_cntrl;
    /// BAR information
    uint16_t bar_information;
} __attribute__ ((__packed__));

/// Base structure of a Block Acknowledgment frame
struct ba_base
{
    /// MAC Header
    struct mac_hdr_ctrl h;
    /// BA control
    uint16_t ba_cntrl;
} __attribute__ ((__packed__));

/// Structure of generic BA SSC + bitmap
struct ba_ssc_bitmap
{
    /// BA Start Sequence Control
    uint16_t ssc;
    /// BA bitmap
    uint16_t bitmap[];
} __attribute__ ((__packed__));

/// Structure of BA SSC + 32-bit bitmap
struct ba_ssc_bitmap_32
{
    /// BA Start Sequence Control
    uint16_t ssc;
    /// BA bitmap
    uint16_t bitmap[2];
} __attribute__ ((__packed__));

/// Structure of BA SSC + 64-bit bitmap
struct ba_ssc_bitmap_64
{
    /// BA Start Sequence Control
    uint16_t ssc;
    /// BA bitmap
    uint16_t bitmap[4];
} __attribute__ ((__packed__));

/// Structure of BA SSC + 128-bit bitmap
struct ba_ssc_bitmap_128
{
    /// BA Start Sequence Control
    uint16_t ssc;
    /// BA bitmap
    uint16_t bitmap[8];
} __attribute__ ((__packed__));

/// Structure of BA SSC + 256-bit bitmap
struct ba_ssc_bitmap_256
{
    /// BA Start Sequence Control
    uint16_t ssc;
    /// BA bitmap
    uint16_t bitmap[16];
} __attribute__ ((__packed__));

/// Multi-STA AID/TID info field
struct ba_msta_aid_tid_info
{
    /// BA control
    uint16_t aid_tid;
    /// BA Start Sequence Control and Bitmap
    struct ba_ssc_bitmap ssc_bitmap;
} __attribute__ ((__packed__));

/// Structure of a Compressed Block Acknowledgment frame
struct ba_comp_frame
{
    /// MAC Header
    struct mac_hdr_ctrl h;
    /// BA control
    uint16_t ba_cntrl;
    /// BA Start Sequence Control and Bitmap
    struct ba_ssc_bitmap_64 ssc_bitmap;
} __attribute__ ((__packed__));

/// Structure of A-MSDU Header
struct amsdu_hdr
{
    /// Destination Address
    struct mac_addr da;
    /// Source Address
    struct mac_addr sa;
    /// Length
    uint16_t len;
} __attribute__ ((__packed__));

/// Structure of the User Info part of the HE Basic Trigger frame
struct he_basic_trigger_user_info
{
    /// First part of the user
    uint32_t user_info1;
    /// RSSI target for the transmission
    uint8_t ul_target_rssi;
    /// Second part of the user
    uint8_t user_info2;
} __attribute__ ((__packed__));

/// Base part of the User Info field present in the HE trigger frames
struct he_user_info_base
{
    /// First part of the user
    uint32_t user_info1;
    /// RSSI target for the transmission
    uint8_t ul_target_rssi;
} __attribute__ ((__packed__));

/// Base structure of a HE Trigger frame
struct he_trigger_base
{
    /// MAC Header
    struct mac_hdr_ctrl h;
    /// Common Info
    uint32_t common[2];
} __attribute__ ((__packed__));

/**
 * @name BAR/BA Frame definitions
 * @{
 ****************************************************************************************
 */
/// Payload length of the BAR frame
#define BAR_PAYLOAD_LEN        (sizeof_b(struct bar_frame))
/// Byte length of the BAR frame, including FCS
#define BAR_FRM_LEN_WITH_FCS   (BAR_PAYLOAD_LEN + MAC_FCS_LEN)

/// BAR frame body BAR Control field - Ack type
#define BAR_CNTL_ACK_TYPE_BIT    CO_BIT(0)
/// BAR frame body BAR Control field - Multi-TID flag
#define BAR_CNTL_MULTI_TID_BIT   CO_BIT(1)
/// BAR frame body BAR Control field - Compressed BA flag
#define BAR_CNTL_COMPRESSED_BIT  CO_BIT(2)

/// Offset of the TID in the BAR frame
#define BAR_CNTL_TID_OFT         12
/// Mask of the TID in the BAR frame
#define BAR_CNTL_TID_MSK         (0xF<<BAR_CNTL_TID_OFT)

/// Compressed BlockAck frame length
#define BA_COMPRESSED_LEN        (sizeof_b(struct ba_comp_frame))

/// TID offset in BA payload byte
#define BA_PAYL_TYPE_OFT        1
/// TID mask in BA payload byte
#define BA_PAYL_TYPE_MSK        (0xF << BA_PAYL_TYPE_OFT)
/// Basic BlockAck type
#define BA_BASIC_TYPE           (0x0 << BA_PAYL_TYPE_OFT)
/// Extended Compressed BlockAck type
#define BA_EXT_COMPRESSED_TYPE  (0x1 << BA_PAYL_TYPE_OFT)
/// Compressed BlockAck type
#define BA_COMPRESSED_TYPE      (0x2 << BA_PAYL_TYPE_OFT)
/// Multi-TID BlockAck type
#define BA_MULTI_TID_TYPE       (0x3 << BA_PAYL_TYPE_OFT)
/// GCR BlockAck type
#define BA_GCR_TYPE             (0x6 << BA_PAYL_TYPE_OFT)
/// GLK-GCR BlockAck type
#define BA_GLK_GCR_TYPE         (0xA << BA_PAYL_TYPE_OFT)
/// Multi-STA BlockAck type
#define BA_MULTI_STA_TYPE       (0xB << BA_PAYL_TYPE_OFT)

/// TID offset in BA payload byte
#define BA_PAYL_TID_BIT_OFT     12
/// TID mask in BA payload byte
#define BA_PAYL_TID_BIT_MSK     (0xF << BA_PAYL_TID_BIT_OFT)
/// SSN bit offset within SSC
#define SN_IN_SSC_BIT_OFT       4

/// Offset of AID in BA information of Multi-STA BA
#define BA_MULTI_STA_AID_OFT    0
/// Mask of AID in BA information of Multi-STA BA
#define BA_MULTI_STA_AID_MSK    (0x7FF << BA_MULTI_STA_AID_OFT)
/// Offset of AID in BA information of Multi-STA BA
#define BA_MULTI_STA_ACK_BIT    CO_BIT(11)
/// Offset of TID in BA information of Multi-STA BA
#define BA_MULTI_STA_TID_OFT    12
/// TID mask in BA payload byte
#define BA_MULTI_STA_TID_MSK    (0xF << BA_MULTI_STA_TID_OFT)

/** @} */

/**
 * @name Trigger Frame definitions
 * @{
 ****************************************************************************************
 */
/// Trigger type offset
#define HE_TRIG_TYPE_OFT         0
/// Trigger type mask
#define HE_TRIG_TYPE_MSK         (0xF << HE_TRIG_TYPE_OFT)
/// Basic trigger type
#define HE_TRIG_TYPE_BASIC       (0 << HE_TRIG_TYPE_OFT)
/// BSRP trigger type
#define HE_TRIG_TYPE_BSRP        (4 << HE_TRIG_TYPE_OFT)
/// UL length offset
#define HE_TRIG_UL_LENGTH_OFT    4
/// UL length mask
#define HE_TRIG_UL_LENGTH_MSK    (0xFFF << HE_TRIG_UL_LENGTH_OFT)
/// More TF
#define HE_TRIG_MORE_TF_BIT      CO_BIT(16)
/// More TF
#define HE_TRIG_CS_REQUIRED_BIT  CO_BIT(17)
/// UL BW offset
#define HE_TRIG_UL_BW_OFT        18
/// UL length mask
#define HE_TRIG_UL_BW_MSK        (0x3 << HE_TRIG_UL_BW_OFT)
/// GI/LTF type offset
#define HE_TRIG_GI_LTF_TYPE_OFT  20
/// GI/LTF type mask
#define HE_TRIG_GI_LTF_TYPE_MSK  (0x3 << HE_TRIG_GI_LTF_TYPE_OFT)

/// AID12 offset
#define HE_TRIG_AID12_OFT        0
/// AID12 mask
#define HE_TRIG_AID12_MSK        (0xFFF << HE_TRIG_AID12_OFT)
/// RU allocation in secondary 80MHz
#define HE_TRIG_RU_IN_SEC_BIT    CO_BIT(12)
/// RU allocation offset
#define HE_TRIG_RU_ALLOC_OFT     13
/// RU allocation mask
#define HE_TRIG_RU_ALLOC_MSK     (0x7F << HE_TRIG_RU_ALLOC_OFT)
/// FEC Coding
#define HE_TRIG_FEC_CODING_BIT   CO_BIT(20)
/// UL MCS offset
#define HE_TRIG_UL_MCS_OFT       21
/// UL MCS mask
#define HE_TRIG_UL_MCS_MSK       (0xF << HE_TRIG_UL_MCS_OFT)
/// RU allocation in secondary 80MHz
#define HE_TRIG_DCM_BIT          CO_BIT(25)
/// Starting Spatial Streams offset
#define HE_TRIG_UL_SSS_OFT       26
/// Starting Spatial Streams mask
#define HE_TRIG_UL_SSS_MSK       (0x7 << HE_TRIG_UL_SSS_OFT)
/// Number of Spatial Streams offset
#define HE_TRIG_UL_NSS_OFT       29
/// Number of Spatial Streams mask
#define HE_TRIG_UL_NSS_MSK       (0x7 << HE_TRIG_UL_NSS_OFT)
/// MU spacing factor offset
#define HE_TRIG_SPC_FACTOR_OFT   0
/// MU spacing factor mask
#define HE_TRIG_SPC_FACTOR_MSK   (0x3 << HE_TRIG_SPC_FACTOR_OFT)
/// TID aggregation limit offset
#define HE_TRIG_TID_AGG_OFT      2
/// TID aggregation limit mask
#define HE_TRIG_TID_AGG_MSK      (0x7 << HE_TRIG_TID_AGG_OFT)
/// Preferred AC offset
#define HE_TRIG_PREF_AC_OFT      6
/// UL MCS mask
#define HE_TRIG_PREF_AC_MSK      (0x3 << HE_TRIG_PREF_AC_OFT)

/// HE trigger frame minimal length
#define HE_TRIG_FRM_MIN_LEN      29

/** @} */

/**
 * @name FTM Frame definitions
 * @{
 ****************************************************************************************
 */
// Public Action Action field (see IEEE Std 802.11-2016 Section 9.6.8.1)
/// Fine Timing Measurement Request
#define FTM_MEAS_REQ               32
/// Fine Timing Measurement
#define FTM_MEAS                   33

// Fine Timing Measurement Request frame format(see IEEE Std 802.11-2016 Section 9.6.8.32)
/// Offset of the Trigger
#define FTM_REQ_TRIGGER           (MAC_ACTION_INFO_OFT + 0)
/// Offset of the FTM Parameters
#define FTM_REQ_PARAMS            (MAC_ACTION_INFO_OFT + 1)

// Fine Timing Measurement frame format (see IEEE Std 802.11-2016 Section 9.6.8.33)
/// Offset of the Dialog Token
#define FTM_MEAS_DIAG_TOKEN       (MAC_ACTION_INFO_OFT + 0)
/// Offset of the Follow Up Dialog Token
#define FTM_MEAS_FLW_DIAG_TOKEN   (MAC_ACTION_INFO_OFT + 1)
/// Offset of the TOD
#define FTM_MEAS_TOD              (MAC_ACTION_INFO_OFT + 2)
/// Offset of the TOA
#define FTM_MEAS_TOA              (MAC_ACTION_INFO_OFT + 8)
/// Offset of the TOD error
#define FTM_MEAS_TOD_ERR          (MAC_ACTION_INFO_OFT + 14)
/// Offset of the TOA error
#define FTM_MEAS_TOA_ERR          (MAC_ACTION_INFO_OFT + 16)
/// Offset of the FTM params
#define FTM_MEAS_PARAMS           (MAC_ACTION_INFO_OFT + 18)

/// Length of the FTM parameters element
#define MAC_FTM_PARAMS_LEN         (9)

/// Fine Timing Measurement Parameters (see IEEE Std 802.11-2016 Section 9.4.2.168)
#define FTM_STATUS_INDICATION_OFT                     0
#define FTM_STATUS_INDICATION_WIDTH                   2
#define FTM_STATUS_INDICATION_SUCCESSFULL             1
#define FTM_STATUS_INDICATION_REQ_INCAPABLE           2
#define FTM_STATUS_INDICATION_REQ_FAILED              3

#define FTM_VALUE_OFT                                 2
#define FTM_VALUE_WIDTH                               5

#define FTM_NB_BURSTS_EXP_OFT                         8
#define FTM_NB_BURSTS_EXP_WIDTH                       4
#define FTM_NB_BURSTS_EXP_ZERO                        0
#define FTM_NB_BURSTS_EXP_ONE                         1
#define FTM_NB_BURSTS_EXP_NO_PREFERENCE               15

#define FTM_BURST_DURATION_OFT                        12
#define FTM_BURST_DURATION_WIDTH                      4
#define FTM_BURST_DURATION_250US                      2
#define FTM_BURST_DURATION_500US                      3
#define FTM_BURST_DURATION_1MS                        4
#define FTM_BURST_DURATION_2MS                        5
#define FTM_BURST_DURATION_4MS                        6
#define FTM_BURST_DURATION_8MS                        7
#define FTM_BURST_DURATION_16MS                       8
#define FTM_BURST_DURATION_32MS                       9
#define FTM_BURST_DURATION_64MS                       10
#define FTM_BURST_DURATION_128MS                      11
#define FTM_BURST_DURATION_NO_PREFERENCE              15

#define FTM_MIN_DELTA_FTM_OFT                         16
#define FTM_MIN_DELTA_FTM_WIDTH                       8
#define FTM_MIN_DELTA_FTM_NO_PREFERENCE               0

#define FTM_PARTIAL_TSF_TIMER_OFT                     24
#define FTM_PARTIAL_TSF_TIMER_WIDTH                   16
#define FTM_PARTIAL_TSF_TIMER_NO_PREFERENCE           0

#define FTM_PARTIAL_TSF_TIMER_PREF_OFT                40
#define FTM_PARTIAL_TSF_TIMER_PREF_WIDTH              1
#define FTM_PARTIAL_TSF_TIMER_PREF_NO                 0
#define FTM_PARTIAL_TSF_TIMER_PREF_YES                1

#define FTM_ASAP_CAPABLE_OFT                          41
#define FTM_ASAP_CAPABLE_WIDTH                        1
#define FTM_ASAP_CAPABLE_YES                          1
#define FTM_ASAP_CAPABLE_NO                           0

#define FTM_ASAP_OFT                                  42
#define FTM_ASAP_WIDTH                                1
#define FTM_ASAP_YES                                  1
#define FTM_ASAP_NO                                   0

#define FTM_FTM_PER_BURST_OFT                         43
#define FTM_FTM_PER_BURST_WIDTH                       5
#define FTM_FTM_PER_BURST_NO_PREFERENCE               0

#define FTM_FORMAT_BANDWITH_OFT                       50
#define FTM_FORMAT_BANDWITH_WIDTH                     6
#define FTM_FORMAT_BANDWITH_NO_PREFERENCE             0
#define FTM_FORMAT_BANDWITH_NON_HT_5MHZ               4
#define FTM_FORMAT_BANDWITH_NON_HT_10MHZ              6
#define FTM_FORMAT_BANDWITH_NON_HT_20MHZ              8
#define FTM_FORMAT_BANDWITH_HT_MIXED_20MHZ            9
#define FTM_FORMAT_BANDWITH_VHT_20MHZ                 10
#define FTM_FORMAT_BANDWITH_HT_MIXED_40MHZ            11
#define FTM_FORMAT_BANDWITH_VHT_40MHZ                 12
#define FTM_FORMAT_BANDWITH_VHT_80MHZ                 13
#define FTM_FORMAT_BANDWITH_VHT_80_80MHZ              14
#define FTM_FORMAT_BANDWITH_VHT_160MHZ_SEPARATE       15
#define FTM_FORMAT_BANDWITH_VHT_160MHZ_SINGLE         16
#define FTM_FORMAT_BANDWITH_DMG_2160MHZ               31

#define FTM_FORMAT_BURST_PERIOD_OFT                   56
#define FTM_FORMAT_BURST_PERIOD_WIDTH                 16
#define FTM_FORMAT_BURST_PERIOD_NO_PREFERENCE         0
/** @} */

/**
 * @name MAC / PHY Management Information Base
 * @{
 ****************************************************************************************
 */
/// PHY type identifiers
enum dot11PHYType {
    DOT11PHY_FHSS = 1,
    DOT11PHY_DSSS = 2,
    DOT11PHY_IRBASEBAND = 3,
    DOT11PHY_OFDM = 4,
    DOT11PHY_HRDSSS = 5,
    DOT11PHY_ERP = 6,
    DOT11PHY_HT = 7,
    DOT11PHY_DMG = 8,
    DOT11PHY_VHT = 9,
    DOT11PHY_TVHT = 10,
    DOT11PHY_S1G = 11,
    DOT11PHY_CDMG = 12,
    DOT11PHY_CMMG = 13,
    DOT11PHY_HE = 14,
};

/** @} */

/**
 ****************************************************************************************
 * @brief Return QOS field of a MAC Header passed as parameter
 *
 * @param[in] machdr            Pointer to the MAC Header containing the QOS to return
 *
 * @return QOS field extracted from MAC Header
 ****************************************************************************************
 */
static __attribute__((__always_inline__)) inline uint16_t mac_get_qos(struct mac_hdr *machdr)
{
    uint16_t qos;

    if ((machdr->fctl & MAC_FCTRL_TODS_FROMDS) == MAC_FCTRL_TODS_FROMDS)
    {
        struct mac_hdr_long_qos *hdr = (struct mac_hdr_long_qos *)machdr;
        qos = hdr->qos;
    }
    else
    {
        struct mac_hdr_qos *hdr = (struct mac_hdr_qos *)machdr;
        qos = hdr->qos;
    }

    return qos;
}

/// @} end of group
#endif // _MAC_FRAME_H_
