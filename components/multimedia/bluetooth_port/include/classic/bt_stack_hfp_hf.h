/**
 *
 * Copyright (c) 2022  Alibaba Group Holding Limited
 *
 */

#ifndef __BT_STACK_HFP_HF_H__
#define __BT_STACK_HFP_HF_H__

#include <classic/bt_stack.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Bluetooth HFP RFCOMM connection and service level connection status */
typedef enum {
    BT_PRF_HFP_HF_CONNECTION_STATE_DISCONNECTED = 0, /*!< RFCOMM data link channel released */
    BT_PRF_HFP_HF_CONNECTION_STATE_CONNECTING,       /*!< connecting remote device on the RFCOMM data link*/
    BT_PRF_HFP_HF_CONNECTION_STATE_CONNECTED,        /*!< RFCOMM connection established */
    BT_PRF_HFP_HF_CONNECTION_STATE_SLC_CONNECTED,    /*!< service level connection established */
    BT_PRF_HFP_HF_CONNECTION_STATE_DISCONNECTING,    /*!< disconnecting with remote device on the RFCOMM data link*/
} bt_prf_hfp_hf_connection_state_t;

/** @brief Bluetooth HFP audio connection status */
typedef enum {
    BT_PRF_HFP_HF_AUDIO_STATE_DISCONNECTED = 0, /*!< audio connection released */
    BT_PRF_HFP_HF_AUDIO_STATE_CONNECTING,       /*!< audio connection has been initiated */
    BT_PRF_HFP_HF_AUDIO_STATE_CONNECTED,        /*!< audio connection is established */
    BT_PRF_HFP_HF_AUDIO_STATE_CONNECTED_MSBC,   /*!< mSBC audio connection is established */
} bt_prf_hfp_hf_audio_state_t;

/** @brief voice recognition state */
typedef enum {
    BT_PRF_HFP_HF_VR_STATE_DISABLED = 0, /*!< voice recognition disabled */
    BT_PRF_HFP_HF_VR_STATE_ENABLED,      /*!< voice recognition enabled */
} bt_prf_hfp_hf_vr_state_t;

/** @brief +CIND network service availability status */
typedef enum {
    BT_PRF_HFP_HF_SERVICE_AVAILABILITY_STATUS_UNAVAILABLE = 0, /*!< service not available */
    BT_PRF_HFP_HF_SERVICE_AVAILABILITY_STATUS_AVAILABLE,       /*!< service available */
} bt_prf_hfp_hf_service_availability_status_t;

/// +CIND roaming status indicator values
typedef enum {
    BT_PRF_HFP_HF_ROAMING_STATUS_INACTIVE = 0, /*!< roaming is not active */
    BT_PRF_HFP_HF_ROAMING_STATUS_ACTIVE,       /*!< a roaming is active */
} bt_prf_hfp_hf_roaming_status_t;

/// +CIND call status indicator values
typedef enum {
    BT_PRF_HFP_HF_CALL_STATUS_NO_CALLS         = 0, /*!< no call in progress  */
    BT_PRF_HFP_HF_CALL_STATUS_CALL_IN_PROGRESS = 1, /*!< call is present(active or held) */
} bt_prf_hfp_hf_status_t;

/// +CIND call setup status indicator values
typedef enum {
    BT_PRF_HFP_HF_CALL_SETUP_STATUS_NONE              = 0, /*!< no call setup in progress */
    BT_PRF_HFP_HF_CALL_SETUP_STATUS_INCOMING          = 1, /*!< incoming call setup in progress */
    BT_PRF_HFP_HF_CALL_SETUP_STATUS_OUTGOING_DIALING  = 2, /*!< outgoing call setup in dialing state */
    BT_PRF_HFP_HF_CALL_SETUP_STATUS_OUTGOING_ALERTING = 3, /*!< outgoing call setup in alerting state */
} bt_prf_hfp_hf_call_setup_status_t;

/// +CIND call held indicator values
typedef enum {
    BT_PRF_HFP_HF_CALL_HELD_STATUS_NONE            = 0, /*!< no calls held */
    BT_PRF_HFP_HF_CALL_HELD_STATUS_HELD_AND_ACTIVE = 1, /*!< both active and held call */
    BT_PRF_HFP_HF_CALL_HELD_STATUS_HELD            = 2, /*!< call on hold, no active call*/
} bt_prf_hfp_hf_call_held_status_t;

/// +BTRH response and hold result code
typedef enum {
    BT_PRF_HFP_HF_BTRH_STATUS_HELD = 0, /*!< incoming call is put on held in AG */
    BT_PRF_HFP_HF_BTRH_STATUS_ACCEPTED, /*!< held incoming call is accepted in AG */
    BT_PRF_HFP_HF_BTRH_STATUS_REJECTED, /*!< held incoming call is rejected in AG */
} bt_prf_hfp_hf_btrh_status_t;

/// +CLCC status of the call
typedef enum {
    BT_PRF_HFP_HF_CURRENT_CALL_STATUS_ACTIVE            = 0, /*!< active */
    BT_PRF_HFP_HF_CURRENT_CALL_STATUS_HELD              = 1, /*!< held */
    BT_PRF_HFP_HF_CURRENT_CALL_STATUS_DIALING           = 2, /*!< dialing (outgoing calls only) */
    BT_PRF_HFP_HF_CURRENT_CALL_STATUS_ALERTING          = 3, /*!< alerting (outgoing calls only) */
    BT_PRF_HFP_HF_CURRENT_CALL_STATUS_INCOMING          = 4, /*!< incoming (incoming calls only) */
    BT_PRF_HFP_HF_CURRENT_CALL_STATUS_WAITING           = 5, /*!< waiting (incoming calls only) */
    BT_PRF_HFP_HF_CURRENT_CALL_STATUS_HELD_BY_RYOC_HOLD = 6, /*!< call held by response and hold */
} bt_prf_hfp_hf_current_call_status_t;

/// +CLCC direction of the call
typedef enum {
    BT_PRF_HFP_HF_CURRENT_CALL_DIRECTION_OUTGOING = 0, /*!< outgoing */
    BT_PRF_HFP_HF_CURRENT_CALL_DIRECTION_INCOMING = 1, /*!< incoming */
} bt_prf_hfp_hf_current_call_direction_t;

/// +CLCC multi-party call flag
typedef enum {
    BT_PRF_HFP_HF_CURRENT_CALL_MPTY_TYPE_SINGLE = 0, /*!< not a member of a multi-party call */
    BT_PRF_HFP_HF_CURRENT_CALL_MPTY_TYPE_MULTI  = 1, /*!< member of a multi-party call */
} bt_prf_hfp_hf_current_call_mpty_type_t;

/// +CLCC call mode
typedef enum {
    BT_PRF_HFP_HF_CURRENT_CALL_MODE_VOICE = 0,
    BT_PRF_HFP_HF_CURRENT_CALL_MODE_DATA  = 1,
    BT_PRF_HFP_HF_CURRENT_CALL_MODE_FAX   = 2,
} bt_prf_hfp_hf_current_call_mode_t;

/// +CLCC address type
typedef enum {
    BT_PRF_HFP_HF_CALL_ADDR_TYPE_UNKNOWN       = 0x81, /*!< unkown address type */
    BT_PRF_HFP_HF_CALL_ADDR_TYPE_INTERNATIONAL = 0x91, /*!< international address */
} bt_prf_hfp_hf_call_addr_type_t;

/// Bluetooth HFP audio volume control target
typedef enum {
    BT_PRF_HFP_HF_VOLUME_CONTROL_TARGET_SPK = 0, /*!< speaker */
    BT_PRF_HFP_HF_VOLUME_CONTROL_TARGET_MIC,     /*!< microphone */
} pbt_prf_hfp_hf_vol_ctrl_target_t;

/// response indication codes for AT commands
typedef enum {
    BT_PRF_HFP_HF_AT_RESPONSE_CODE_OK = 0,      /*!< acknowledges execution of a command line */
    BT_PRF_HFP_HF_AT_RESPONSE_CODE_ERR,         /*!< command not accepted */
    BT_PRF_HFP_HF_AT_RESPONSE_CODE_NO_CARRIER,  /*!< connection terminated */
    BT_PRF_HFP_HF_AT_RESPONSE_CODE_BUSY,        /*!< busy signal detected */
    BT_PRF_HFP_HF_AT_RESPONSE_CODE_NO_ANSWER,   /*!< connection completion timeout */
    BT_PRF_HFP_HF_AT_RESPONSE_CODE_DELAYED,     /*!< delayed */
    BT_PRF_HFP_HF_AT_RESPONSE_CODE_BLACKLISTED, /*!< blacklisted */
    BT_PRF_HFP_HF_AT_RESPONSE_CODE_CME,         /*!< CME error */
} bt_prf_hfp_hf_at_response_code_t;

/// AT+CHLD command values
typedef enum {
    BT_PRF_HFP_HF_CHLD_TYPE_REL = 0,      /*!< <0>, Terminate all held or set UDUB("busy") to a waiting call */
    BT_PRF_HFP_HF_CHLD_TYPE_REL_ACC,      /*!< <1>, Terminate all active calls and accepts a waiting/held call */
    BT_PRF_HFP_HF_CHLD_TYPE_HOLD_ACC,     /*!< <2>, Hold all active calls and accepts a waiting/held call */
    BT_PRF_HFP_HF_CHLD_TYPE_MERGE,        /*!< <3>, Add all held calls to a conference */
    BT_PRF_HFP_HF_CHLD_TYPE_MERGE_DETACH, /*!< <4>, connect the two calls and disconnects the subscriber from both
                                             calls */
    BT_PRF_HFP_HF_CHLD_TYPE_REL_X,        /*!< <1x>, releases specified calls only */
    BT_PRF_HFP_HF_CHLD_TYPE_PRIV_X,       /*!< <2x>, request private consultation mode with specified call */
} bt_prf_hfp_hf_chld_type_t;

/// Extended Audio Gateway Error Result Code Response
typedef enum {
    BT_PRF_HFP_HF_CME_AG_FAILURE                        = 0,  /*!< ag failure */
    BT_PRF_HFP_HF_CME_NO_CONNECTION_TO_PHONE            = 1,  /*!< no connection to phone */
    BT_PRF_HFP_HF_CME_OPERATION_NOT_ALLOWED             = 3,  /*!< operation not allowed */
    BT_PRF_HFP_HF_CME_OPERATION_NOT_SUPPORTED           = 4,  /*!< operation not supported */
    BT_PRF_HFP_HF_CME_PH_SIM_PIN_REQUIRED               = 5,  /*!< PH-SIM PIN Required */
    BT_PRF_HFP_HF_CME_SIM_NOT_INSERTED                  = 10, /*!< SIM not inserted */
    BT_PRF_HFP_HF_CME_SIM_PIN_REQUIRED                  = 11, /*!< SIM PIN required */
    BT_PRF_HFP_HF_CME_SIM_PUK_REQUIRED                  = 12, /*!< SIM PUK required */
    BT_PRF_HFP_HF_CME_SIM_FAILURE                       = 13, /*!< SIM failure */
    BT_PRF_HFP_HF_CME_SIM_BUSY                          = 14, /*!< SIM busy */
    BT_PRF_HFP_HF_CME_INCORRECT_PASSWORD                = 16, /*!< incorrect password */
    BT_PRF_HFP_HF_CME_SIM_PIN2_REQUIRED                 = 17, /*!< SIM PIN2 required */
    BT_PRF_HFP_HF_CME_SIM_PUK2_REQUIRED                 = 18, /*!< SIM PUK2 required */
    BT_PRF_HFP_HF_CME_MEMEORY_FULL                      = 20, /*!< memory full */
    BT_PRF_HFP_HF_CME_INVALID_INDEX                     = 21, /*!< invalid index */
    BT_PRF_HFP_HF_CME_MEMEORY_FAILURE                   = 23, /*!< memory failure */
    BT_PRF_HFP_HF_CME_TEXT_STRING_TOO_LONG              = 24, /*!< test string too long */
    BT_PRF_HFP_HF_CME_INVALID_CHARACTERS_IN_TEXT_STRING = 25, /*!< invalid characters in text string */
    BT_PRF_HFP_HF_CME_DIAL_STRING_TOO_LONG              = 26, /*!< dial string too long*/
    BT_PRF_HFP_HF_CME_INVALID_CHARACTERS_IN_DIAL_STRING = 27, /*!< invalid characters in dial string */
    BT_PRF_HFP_HF_CME_NO_NETWORK_SERVICE                = 30, /*!< no network service */
    BT_PRF_HFP_HF_CME_NETWORK_TIMEOUT                   = 31, /*!< network timeout */
    BT_PRF_HFP_HF_CME_NETWORK_NOT_ALLOWED               = 32, /*!< network not allowed --emergency calls only */
} bt_prf_hfp_hf_cme_err_t;

/// +CNUM service type of the phone number
typedef enum {
    BT_PRF_HFP_HF_SUBSCRIBER_SERVICE_TYPE_UNKNOWN = 0, /*!< unknown */
    BT_PRF_HFP_HF_SUBSCRIBER_SERVICE_TYPE_VOICE,       /*!< voice service */
    BT_PRF_HFP_HF_SUBSCRIBER_SERVICE_TYPE_FAX,         /*!< fax service */
} bt_prf_hfp_hf_subscriber_service_type_t;

/// in-band ring tone state
typedef enum {
    BT_PRF_HFP_HF_CLIENT_IN_BAND_RINGTONE_NOT_PROVIDED = 0,
    BT_PRF_HFP_HF_CLIENT_IN_BAND_RINGTONE_PROVIDED,
} bt_prf_hfp_hf_client_in_band_ring_state_t;

/// HFP HF callback events
typedef enum {
    BT_PRF_HFP_HF_CONNECTION_STATE_EVT = 0,      /*!< connection state changed event */
    BT_PRF_HFP_HF_AUDIO_STATE_EVT,               /*!< audio connection state change event */
    BT_PRF_HFP_HF_BVRA_EVT,                      /*!< voice recognition state change event */
    BT_PRF_HFP_HF_CIND_CALL_EVT,                 /*!< call indication */
    BT_PRF_HFP_HF_CIND_CALL_SETUP_EVT,           /*!< call setup indication */
    BT_PRF_HFP_HF_CIND_CALL_HELD_EVT,            /*!< call held indication */
    BT_PRF_HFP_HF_CIND_SERVICE_AVAILABILITY_EVT, /*!< network service availability indication */
    BT_PRF_HFP_HF_CIND_SIGNAL_STRENGTH_EVT,      /*!< signal strength indication */
    BT_PRF_HFP_HF_CIND_ROAMING_STATUS_EVT,       /*!< roaming status indication */
    BT_PRF_HFP_HF_CIND_BATTERY_LEVEL_EVT,        /*!< battery level indication */
    BT_PRF_HFP_HF_COPS_CURRENT_OPERATOR_EVT,     /*!< current operator information */
    BT_PRF_HFP_HF_BTRH_EVT,                      /*!< call response and hold event */
    BT_PRF_HFP_HF_CLIP_EVT,                      /*!< Calling Line Identification notification */
    BT_PRF_HFP_HF_CCWA_EVT,                      /*!< call waiting notification */
    BT_PRF_HFP_HF_CLCC_EVT,                      /*!< list of current calls notification */
    BT_PRF_HFP_HF_VOLUME_CONTROL_EVT,            /*!< audio volume control command from AG, provided by +VGM or +VGS message */
    BT_PRF_HFP_HF_AT_RESPONSE_EVT,               /*!< AT command response event */
    BT_PRF_HFP_HF_CNUM_EVT,                      /*!< subscriber information response from AG */
    BT_PRF_HFP_HF_BSIR_EVT,                      /*!< setting of in-band ring tone */
    BT_PRF_HFP_HF_BINP_EVT,                      /*!< requested number of last voice tag from AG */
    BT_PRF_HFP_HF_RING_IND_EVT,                  /*!< ring indication event */
} bt_prf_hfp_hf_cb_event_t;

/// HFP HF callback parameters
typedef union
{
    /**
     * @brief  BT_PRF_HFP_HF_CONNECTION_STATE_EVT
     */
    struct hf_client_conn_stat_param {
        bt_prf_hfp_hf_connection_state_t state;             /*!< HF connection state */
        uint32_t                         peer_feat;         /*!< AG supported features */
        uint32_t                         chld_feat;         /*!< AG supported features on call hold and multiparty services */
        bt_dev_addr_t                    peer_addr;         /*!< remote bluetooth device address */
    } conn_stat;                                            /*!< HF callback param of BT_PRF_HFP_HF_CONNECTION_STATE_EVT */

    /**
     * @brief BT_PRF_HFP_HF_AUDIO_STATE_EVT
     */
    struct hf_client_audio_stat_param {
        bt_prf_hfp_hf_audio_state_t state;                  /*!< audio connection state */
        bt_dev_addr_t               peer_addr;              /*!< remote bluetooth device address */
    } audio_stat;                                           /*!< HF callback param of BT_PRF_HFP_HF_AUDIO_STATE_EVT */

    /**
     * @brief BT_PRF_HFP_HF_BVRA_EVT
     */
    struct hf_client_bvra_param {
        bt_prf_hfp_hf_vr_state_t value;                     /*!< voice recognition state */
    } bvra;                                                 /*!< HF callback param of BT_PRF_HFP_HF_BVRA_EVT */

    /**
     * @brief BT_PRF_HFP_HF_CIND_SERVICE_AVAILABILITY_EVT
     */
    struct hf_client_service_availability_param {
        bt_prf_hfp_hf_service_availability_status_t status; /*!< service availability status */
    } service_availability;                                 /*!< HF callback param of BT_PRF_HFP_HF_CIND_SERVICE_AVAILABILITY_EVT */

    /**
     * @brief BT_PRF_HFP_HF_CIND_ROAMING_STATUS_EVT
     */
    struct hf_client_network_roaming_param {
        bt_prf_hfp_hf_roaming_status_t status;              /*!< roaming status */
    } roaming;                                              /*!< HF callback param of BT_PRF_HFP_HF_CIND_ROAMING_STATUS_EVT */

    /**
     * @brief BT_PRF_HFP_HF_CIND_SIGNAL_STRENGTH_EVT
     */
    struct hf_client_signal_strength_ind_param {
        int value;                                          /*!< signal strength value, ranges from 0 to 5 */
    } signal_strength;                                      /*!< HF callback param of BT_PRF_HFP_HF_CIND_SIGNAL_STRENGTH_EVT */

    /**
     * @brief BT_PRF_HFP_HF_CIND_BATTERY_LEVEL_EVT
     */
    struct hf_client_battery_level_ind_param {
        int value;                                         /*!< battery charge value, ranges from 0 to 5 */
    } battery_level;                                       /*!< HF callback param of BT_PRF_HFP_HF_CIND_BATTERY_LEVEL_EVT */

    /**
     * @brief BT_PRF_HFP_HF_COPS_CURRENT_OPERATOR_EVT
     */
    struct hf_client_current_operator_param {
        const char *name;                                  /*!< name of the network operator */
    } cops;                                                /*!< HF callback param of BT_PRF_HFP_HF_COPS_CURRENT_OPERATOR_EVT */

    /**
     * @brief BT_PRF_HFP_HF_CIND_CALL_EVT
     */
    struct hf_client_call_ind_param {
        bt_prf_hfp_hf_status_t status;                     /*!< call status indicator */
    } call;                                                /*!< HF callback param of BT_PRF_HFP_HF_CIND_CALL_EVT */

    /**
     * @brief BT_PRF_HFP_HF_CIND_CALL_SETUP_EVT
     */
    struct hf_client_call_setup_ind_param {
        bt_prf_hfp_hf_call_setup_status_t status;         /*!< call setup status indicator */
    } call_setup;                                         /*!< HF callback param of BT_PRF_HFP_HF_CIND_CALL_SETUP_EVT */

    /**
     * @brief BT_PRF_HFP_HF_CIND_CALL_HELD_EVT
     */
    struct hf_client_call_held_ind_param {
        bt_prf_hfp_hf_call_held_status_t status;          /*!< bluetooth proprietary call hold status indicator */
    } call_held;                                          /*!< HF callback param of BT_PRF_HFP_HF_CIND_CALL_HELD_EVT */

    /**
     * @brief BT_PRF_HFP_HF_BTRH_EVT
     */
    struct hf_client_btrh_param {
        bt_prf_hfp_hf_status_t status;                    /*!< call hold and response status result code */
    } btrh;                                               /*!< HF callback param of BT_PRF_HFP_HF_BTRH_EVT */

    /**
     * @brief BT_PRF_HFP_HF_CLIP_EVT
     */
    struct hf_client_clip_param {
        const char *number;                               /*!< phone number string of call */
    } clip;                                               /*!< HF callback param of BT_PRF_HFP_HF_CLIP_EVT */

    /**
     * @brief BT_PRF_HFP_HF_CCWA_EVT
     */
    struct hf_client_ccwa_param {
        const char *number;                              /*!< phone number string of waiting call */
    } ccwa;                                              /*!< HF callback param of BT_PRF_HFP_HF_CCWA_EVT */

    /**
     * @brief BT_PRF_HFP_HF_CLCC_EVT
     */
    struct hf_client_clcc_param {
        int                                    idx;      /*!< numbering(starting with 1) of the call */
        bt_prf_hfp_hf_current_call_direction_t dir;      /*!< direction of the call */
        bt_prf_hfp_hf_current_call_status_t    status;   /*!< status of the call */
        bt_prf_hfp_hf_current_call_mpty_type_t mpty;     /*!< multi-party flag */
        char *                                 number;   /*!< phone number(optional) */
    } clcc;                                              /*!< HF callback param of BT_PRF_HFP_HF_CLCC_EVT */

    /**
     * @brief BT_PRF_HFP_HF_VOLUME_CONTROL_EVT
     */
    struct hf_client_volume_control_param {
        pbt_prf_hfp_hf_vol_ctrl_target_t type;           /*!< volume control target, speaker or microphone */
        int                              volume;         /*!< gain, ranges from 0 to 15 */
    } volume_control;                                    /*!< HF callback param of BT_PRF_HFP_HF_VOLUME_CONTROL_EVT */

    /**
     * @brief BT_PRF_HFP_HF_AT_RESPONSE_EVT
     */
    struct hf_client_at_response_param {
        bt_prf_hfp_hf_at_response_code_t code;           /*!< AT response code */
        bt_prf_hfp_hf_cme_err_t          cme;            /*!< Extended Audio Gateway Error Result Code */
    } at_response;                                       /*!< HF callback param of BT_PRF_HFP_HF_AT_RESPONSE_EVT */

    /**
     * @brief BT_PRF_HFP_HF_CNUM_EVT
     */
    struct hf_client_cnum_param {
        const char *                            number;  /*!< phone number string */
        bt_prf_hfp_hf_subscriber_service_type_t type;    /*!< service type that the phone number relates to */
    } cnum;                                              /*!< HF callback param of BT_PRF_HFP_HF_CNUM_EVT */

    /**
     * @brief BT_PRF_HFP_HF_BSIR_EVT
     */
    struct hf_client_bsirparam {
        bt_prf_hfp_hf_client_in_band_ring_state_t state; /*!< setting state of in-band ring tone */
    } bsir;                                              /*!< HF callback param of BT_PRF_HFP_HF_BSIR_EVT */

    /**
     * @brief BT_PRF_HFP_HF_BINP_EVT
     */
    struct hf_client_binp_param {
        const char *number;                              /*!< phone number corresponding to the last voice tag in the HF */
    } binp;                                              /*!< HF callback param of BT_PRF_HFP_HF_BINP_EVT */

} bt_prf_hfp_hf_cb_param_t;

/**
 * @brief      HFP HF event callback function type
 * @param[in]  event         Event type
 * @param[in]  param         Pointer to callback parameter
 */
typedef void (*bt_prf_hfp_hf_cb_t)(bt_prf_hfp_hf_cb_event_t event, bt_prf_hfp_hf_cb_param_t *param);

/**
 * @brief      register callback function.
 * @param[in]  callback         callback function
 */
void bt_prf_hfp_hf_register_callback(bt_prf_hfp_hf_cb_t callback);

/**
 * @brief      Initialize the bluetooth HFP client module.
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_hfp_hf_init(void);

/**
 * @brief      Connect to remote bluetooth HFP audio gateway(AG) device.
 * @param[in]  peer_addr         bluetooth device address of peer device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_hfp_hf_connect(bt_dev_addr_t *peer_addr);

/**
 * @brief      Disconnect from the remote HFP audio gateway
 * @param[in]  peer_addr         bluetooth device address of peer device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_hfp_hf_disconnect(bt_dev_addr_t *peer_addr);

/**
 * @brief      Create audio connection with remote HFP AG.
 * @param[in]  peer_addr         bluetooth device address of peer device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_hfp_hf_audio_connect(bt_dev_addr_t *peer_addr);

/**
 * @brief      Release the established audio connection with remote HFP AG.
 * @param[in]  peer_addr         bluetooth device address of peer device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_hfp_hf_audio_disconnect(bt_dev_addr_t *peer_addr);

/**
 * @brief      Send call hold and multiparty commands,
 *             or enhanced call control commands(Use AT+CHLD).
 * @param[in]  chld             AT+CHLD call hold and multiparty handling AT command.
 * @param[in]  idx              used in Enhanced Call Control Mechanisms, used if chld is
 *                              BT_PRF_HFP_HF_CHLD_TYPE_REL_X or BT_PRF_HFP_HF_CHLD_TYPE_PRIV_X
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_hfp_hf_send_chld_cmd(bt_prf_hfp_hf_chld_type_t chld, int idx);

/**
 * @brief      Answer an incoming call(send ATA command).
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_hfp_hf_answer_call(void);

/**
 * @brief      Reject an incoming call(send AT+CHUP command).
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_hfp_hf_reject_call(void);

/**
 * @brief      Place a call with a specified number
 * @param[in]  number           number string of the call.
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_hfp_hf_dial(char *number);

/**
 * @brief      Volume synchronization with AG.
 * @param[in]  type             volume control target, speaker or microphone
 * @param[in]  volume           gain of the speaker of microphone, ranges 0 to 15
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_hfp_hf_vol_update(pbt_prf_hfp_hf_vol_ctrl_target_t type, int volume);

#ifdef __cplusplus
}
#endif

#endif /* __BT_STACK_HFP_HF_H__ */
