/**
 *
 * Copyright (c) 2022  Alibaba Group Holding Limited
 *
 */

#ifndef __BT_STACK_AVRCP_H__
#define __BT_STACK_AVRCP_H__

#ifdef __cplusplus
extern "C" {
#endif

#define BT_AVRCP_MAX_VOLUME         0x7F

/** @brief AVRCP Controller callback events */
typedef enum {
    BT_PRF_AVRCP_CT_CONNECTION_STATE_EVT        = 0, /*!< connection state changed event */
    BT_PRF_AVRCP_CT_PASSTHROUGH_RSP_EVT         = 1, /*!< passthrough response event */
    BT_PRF_AVRCP_CT_METADATA_RSP_EVT            = 2, /*!< metadata response event */
    BT_PRF_AVRCP_CT_PLAY_STATUS_RSP_EVT         = 3, /*!< play status response event */
    BT_PRF_AVRCP_CT_CHANGE_NOTIFY_EVT           = 4, /*!< notification event */
    BT_PRF_AVRCP_CT_REMOTE_FEATURES_EVT         = 5, /*!< feature of remote device indication event */
    BT_PRF_AVRCP_CT_GET_RN_CAPABILITIES_RSP_EVT = 6, /*!< supported notification events capability of peer device */
    BT_PRF_AVRCP_CT_SET_ABSOLUTE_VOLUME_RSP_EVT = 7, /*!< set absolute volume response event */
} bt_prf_avrcp_ct_cb_event_t;

/** @brief AVRCP Target callback events */
typedef enum {
    BT_PRF_AVRCP_TG_CONNECTION_STATE_EVT        = 0, /*!< connection state changed event */
    BT_PRF_AVRCP_TG_REMOTE_FEATURES_EVT         = 1, /*!< feature of remote device indication event */
    BT_PRF_AVRCP_TG_PASSTHROUGH_CMD_EVT         = 2, /*!< passthrough command event */
    BT_PRF_AVRCP_TG_SET_ABSOLUTE_VOLUME_CMD_EVT = 3, /*!< set absolute volume command from remote device */
    BT_PRF_AVRCP_TG_REGISTER_NOTIFICATION_EVT   = 4, /*!< register notification event */
} bt_prf_avrcp_tg_cb_event_t;

/** @brief AV/C packet head command type */
typedef enum {
    BT_AVRCP_CTYPE_CONTROL = 0,
    BT_AVRCP_CTYPE_STATUS,
    BT_AVRCP_CTYPE_SPECIFIC_INQUIRY,
    BT_AVRCP_CTYPE_NOTIFY,
    BT_AVRCP_CTYPE_GENERAL_INQUIRY,
    BT_AVRCP_CTYPE_RESERVED5,
    BT_AVRCP_CTYPE_RESERVED6,
    BT_AVRCP_CTYPE_RESERVED7,
    BT_AVRCP_RESPONSE_NOT_IMPLEMENTED = 8,
    BT_AVRCP_RESPONSE_ACCEPTED,
    BT_AVRCP_RESPONSE_REJECTED,
    BT_AVRCP_RESPONSE_IN_TRANSITION,
    BT_AVRCP_RESPONSE_IMPLEMENTED_STABLE,
    BT_AVRCP_RESPONSE_CHANGED_STABLE,
    BT_AVRCP_RESPONSE_RESERVED,
    BT_AVRCP_RESPONSE_INTERIM,
} bt_prf_avrcp_ctype_response_t;

/** @brief AVRCP passthrough operation id */
typedef enum {
    BT_PRF_AVRCP_OP_ID_SELECT       = 0x00, /*!< select */
    BT_PRF_AVRCP_OP_ID_UP           = 0x01, /*!< up */
    BT_PRF_AVRCP_OP_ID_DOWN         = 0x02, /*!< down */
    BT_PRF_AVRCP_OP_ID_LEFT         = 0x03, /*!< left */
    BT_PRF_AVRCP_OP_ID_RIGHT        = 0x04, /*!< right */
    BT_PRF_AVRCP_OP_ID_RIGHT_UP     = 0x05, /*!< right-up */
    BT_PRF_AVRCP_OP_ID_RIGHT_DOWN   = 0x06, /*!< right-down */
    BT_PRF_AVRCP_OP_ID_LEFT_UP      = 0x07, /*!< left-up */
    BT_PRF_AVRCP_OP_ID_LEFT_DOWN    = 0x08, /*!< left-down */
    BT_PRF_AVRCP_OP_ID_ROOT_MENU    = 0x09, /*!< root menu */
    BT_PRF_AVRCP_OP_ID_SETUP_MENU   = 0x0A, /*!< setup menu */
    BT_PRF_AVRCP_OP_ID_CONT_MENU    = 0x0B, /*!< contents menu */
    BT_PRF_AVRCP_OP_ID_FAV_MENU     = 0x0C, /*!< favorite menu */
    BT_PRF_AVRCP_OP_ID_EXIT         = 0x0D, /*!< exit */
    BT_PRF_AVRCP_OP_ID_0            = 0x20, /*!< 0 */
    BT_PRF_AVRCP_OP_ID_1            = 0x21, /*!< 1 */
    BT_PRF_AVRCP_OP_ID_2            = 0x22, /*!< 2 */
    BT_PRF_AVRCP_OP_ID_3            = 0x23, /*!< 3 */
    BT_PRF_AVRCP_OP_ID_4            = 0x24, /*!< 4 */
    BT_PRF_AVRCP_OP_ID_5            = 0x25, /*!< 5 */
    BT_PRF_AVRCP_OP_ID_6            = 0x26, /*!< 6 */
    BT_PRF_AVRCP_OP_ID_7            = 0x27, /*!< 7 */
    BT_PRF_AVRCP_OP_ID_8            = 0x28, /*!< 8 */
    BT_PRF_AVRCP_OP_ID_9            = 0x29, /*!< 9 */
    BT_PRF_AVRCP_OP_ID_DOT          = 0x2A, /*!< dot */
    BT_PRF_AVRCP_OP_ID_ENTER        = 0x2B, /*!< enter */
    BT_PRF_AVRCP_OP_ID_CLEAR        = 0x2C, /*!< clear */
    BT_PRF_AVRCP_OP_ID_CHAN_UP      = 0x30, /*!< channel up */
    BT_PRF_AVRCP_OP_ID_CHAN_DOWN    = 0x31, /*!< channel down */
    BT_PRF_AVRCP_OP_ID_PREV_CHAN    = 0x32, /*!< previous channel */
    BT_PRF_AVRCP_OP_ID_SOUND_SEL    = 0x33, /*!< sound select */
    BT_PRF_AVRCP_OP_ID_INPUT_SEL    = 0x34, /*!< input select */
    BT_PRF_AVRCP_OP_ID_DISP_INFO    = 0x35, /*!< display information */
    BT_PRF_AVRCP_OP_ID_HELP         = 0x36, /*!< help */
    BT_PRF_AVRCP_OP_ID_PAGE_UP      = 0x37, /*!< page up */
    BT_PRF_AVRCP_OP_ID_PAGE_DOWN    = 0x38, /*!< page down */
    BT_PRF_AVRCP_OP_ID_POWER        = 0x40, /*!< power */
    BT_PRF_AVRCP_OP_ID_VOL_UP       = 0x41, /*!< volume up */
    BT_PRF_AVRCP_OP_ID_VOL_DOWN     = 0x42, /*!< volume down */
    BT_PRF_AVRCP_OP_ID_MUTE         = 0x43, /*!< mute */
    BT_PRF_AVRCP_OP_ID_PLAY         = 0x44, /*!< play */
    BT_PRF_AVRCP_OP_ID_STOP         = 0x45, /*!< stop */
    BT_PRF_AVRCP_OP_ID_PAUSE        = 0x46, /*!< pause */
    BT_PRF_AVRCP_OP_ID_RECORD       = 0x47, /*!< record */
    BT_PRF_AVRCP_OP_ID_REWIND       = 0x48, /*!< rewind */
    BT_PRF_AVRCP_OP_ID_FAST_FORWARD = 0x49, /*!< fast forward */
    BT_PRF_AVRCP_OP_ID_EJECT        = 0x4A, /*!< eject */
    BT_PRF_AVRCP_OP_ID_FORWARD      = 0x4B, /*!< forward */
    BT_PRF_AVRCP_OP_ID_BACKWARD     = 0x4C, /*!< backward */
    BT_PRF_AVRCP_OP_ID_ANGLE        = 0x50, /*!< angle */
    BT_PRF_AVRCP_OP_ID_SUBPICT      = 0x51, /*!< subpicture */
    BT_PRF_AVRCP_OP_ID_F1           = 0x71, /*!< F1 */
    BT_PRF_AVRCP_OP_ID_F2           = 0x72, /*!< F2 */
    BT_PRF_AVRCP_OP_ID_F3           = 0x73, /*!< F3 */
    BT_PRF_AVRCP_OP_ID_F4           = 0x74, /*!< F4 */
    BT_PRF_AVRCP_OP_ID_F5           = 0x75, /*!< F5 */
    BT_PRF_AVRCP_OP_ID_VENDOR       = 0x7E, /*!< vendor unique */
} bt_prf_avrcp_op_id_t;

/** @brief AVRCP passthrough operation state */
typedef enum {
    BT_PRF_AVRCP_OP_PRESSED = 0,
    BT_PRF_AVRCP_OP_RELEASED,
} bt_prf_avrcp_op_stat_t;

/** @brief AVRCP regist notification event ids */
typedef enum {
    BT_PRF_AVRCP_NOTIFICATION_PLAY_STATUS_CHANGE       = 0x01, /*!< track status change, eg. from playing to paused */
    BT_PRF_AVRCP_NOTIFICATION_TRACK_CHANGE             = 0x02, /*!< new track is loaded */
    BT_PRF_AVRCP_NOTIFICATION_TRACK_REACHED_END        = 0x03, /*!< current track reached end */
    BT_PRF_AVRCP_NOTIFICATION_TRACK_REACHED_START      = 0x04, /*!< current track reached start position */
    BT_PRF_AVRCP_NOTIFICATION_PLAY_POS_CHANGED         = 0x05, /*!< track playing position changed */
    BT_PRF_AVRCP_NOTIFICATION_BATTERY_STATUS_CHANGE    = 0x06, /*!< battery status changed */
    BT_PRF_AVRCP_NOTIFICATION_SYSTEM_STATUS_CHANGE     = 0x07, /*!< system status changed */
    BT_PRF_AVRCP_NOTIFICATION_APP_SETTING_CHANGE       = 0x08, /*!< application settings changed */
    BT_PRF_AVRCP_NOTIFICATION_NOW_PLAYING_CHANGE       = 0x09, /*!< now playing content changed */
    BT_PRF_AVRCP_NOTIFICATION_AVAILABLE_PLAYERS_CHANGE = 0x0a, /*!< available players changed */
    BT_PRF_AVRCP_NOTIFICATION_ADDRESSED_PLAYER_CHANGE  = 0x0b, /*!< the addressed player changed */
    BT_PRF_AVRCP_NOTIFICATION_UIDS_CHANGE              = 0x0c, /*!< UIDs changed */
    BT_PRF_AVRCP_NOTIFICATION_VOLUME_CHANGE
    = 0x0d, /*!< volume changed locally on TG , will not event upper layer if enable CONFIG_BT_AVRCP_VOL_CONTROL */
    BT_PRF_AVRCP_NOTIFICATION_MAX_EVT
} bt_prf_avrcp_rn_event_ids_t;

/** @brief AVRCP battery status */
typedef enum {
    BT_PRF_AVRCP_BATT_NORMAL      = 0, /*!< normal state */
    BT_PRF_AVRCP_BATT_WARNING     = 1, /*!< unable to operate soon */
    BT_PRF_AVRCP_BATT_CRITICAL    = 2, /*!< cannot operate any more */
    BT_PRF_AVRCP_BATT_EXTERNAL    = 3, /*!< plugged to external power supply */
    BT_PRF_AVRCP_BATT_FULL_CHARGE = 4, /*!< when completely charged from external power supply */
} bt_prf_avrcp_batt_stat_t;

/** @brief AVRCP current status of playback */
typedef enum {
    BT_PRF_AVRCP_PLAYBACK_STOPPED  = 0,    /*!< stopped */
    BT_PRF_AVRCP_PLAYBACK_PLAYING  = 1,    /*!< playing */
    BT_PRF_AVRCP_PLAYBACK_PAUSED   = 2,    /*!< paused */
    BT_PRF_AVRCP_PLAYBACK_FWD_SEEK = 3,    /*!< forward seek */
    BT_PRF_AVRCP_PLAYBACK_REV_SEEK = 4,    /*!< reverse seek */
    BT_PRF_AVRCP_PLAYBACK_ERROR    = 0xFF, /*!< error */
} bt_prf_avrcp_playback_stat_t;

/** @brief AVRC target notification event capability bit mask */
typedef struct {
    uint16_t bits; /*!< bit mask representation of capability event id */
} bt_prf_avrcp_rn_evt_cap_mask_t;

/** @brief AVRCP notification parameters */
typedef union
{
    uint8_t volume;                        /*!< response data for BT_PRF_AVRCP_NOTIFICATION_VOLUME_CHANGE,
                                               ranges 0..127 */
    bt_prf_avrcp_playback_stat_t playback; /*!< response data for
                                                 BT_PRF_AVRCP_NOTIFICATION_PLAY_STATUS_CHANGE */
    uint8_t  elm_id[8];                    /*!< response data for BT_PRF_AVRCP_NOTIFICATION_TRACK_CHANGE */
    uint32_t play_pos;                     /*!< response data for
                                                 BT_PRF_AVRCP_NOTIFICATION_PLAY_POS_CHANGED, in
                                              millisecond */
    bt_prf_avrcp_batt_stat_t batt;         /*!< response data for
                                                 BT_PRF_AVRCP_NOTIFICATION_BATTERY_STATUS_CHANGE */
} bt_prf_avrcp_rn_param_t;

/** @brief AVRCP controller callback parameters */
typedef union
{
    /**
     * @brief BT_PRF_AVRCP_CT_CONNECTION_STATE_EVT
     */
    struct avrcp_ct_conn_stat_param {
        uint8_t       connected; /*!< whether AVRCP connection is set up */
        bt_dev_addr_t peer_addr; /*!< remote bluetooth device address */
    } conn_stat;                 /*!< AVRCP connection status */

    /**
     * @brief BT_PRF_AVRCP_CT_PASSTHROUGH_RSP_EVT
     */
    struct avrcp_ct_psth_rsp_param {
        uint8_t operation_id;    /*!< passthrough command code \ref
                                    bt_prf_avrcp_op_id_t */
        uint8_t operation_state; /*!< 0 for PRESSED, 1 for RELEASED */
    } psth_rsp;                  /*!< passthrough command response */

    /**
     * @brief BT_PRF_AVRCP_CT_METADATA_RSP_EVT
     */
    struct avrcp_ct_meta_rsp_param {
        uint8_t  attr_id;     /*!< id of metadata attribute */
        uint8_t *attr_text;   /*!< attribute itself */
        int      attr_length; /*!< attribute character length */
    } meta_rsp;               /*!< metadata attributes response */

    /**
     * @brief BT_PRF_AVRCP_CT_CHANGE_NOTIFY_EVT
     */
    struct avrcp_ct_change_notify_param {
        bt_prf_avrcp_rn_event_ids_t event_id;        /*!< id of AVRCP event notification */
        bt_prf_avrcp_rn_param_t     event_parameter; /*!< event notification parameter */
    } change_ntf;                                    /*!< notifications */

    /**
     * @brief BT_PRF_AVRCP_CT_REMOTE_FEATURES_EVT
     */
    struct avrcp_ct_rmt_feats_param {
        uint32_t      feat_mask;    /*!< AVRCP feature mask of remote device */
        uint16_t      tg_feat_flag; /*!< feature flag of remote device as TG */
        bt_dev_addr_t peer_addr;    /*!< remote bluetooth device address */
    } rmt_feats;                    /*!< AVRCP features discovered from remote SDP server */

    /**
     * @brief BT_PRF_AVRCP_CT_GET_RN_CAPABILITIES_RSP_EVT
     */
    struct avrcp_ct_get_rn_caps_rsp_param {
        uint8_t cap_count;                      /*!< number of items provided in event or company_id
                                                     according to cap_id used */
        bt_prf_avrcp_rn_evt_cap_mask_t evt_set; /*!< supported event_ids represented in bit-mask */
    } get_rn_caps_rsp;                          /*!< get supported event capabilities response from AVRCP
                                                     target */

    /**
     * @brief BT_PRF_AVRCP_CT_PLAY_STATUS_RSP_EVT
     */
    struct avrcp_ct_get_rn_play_status_rsp_param {
        uint32_t song_len;    /*!< play song total len */
        uint32_t song_pos;    /*!< play song position */
        uint8_t  play_status; /*!< 0x0:stoped 0x1:play 0x2:paused 0x3:fwd_seek
                                    0x4:rev_seek 0xff:error*/
    } get_rn_play_status_rsp; /*!< get play status response from AVRCP target */

    /**
     * @brief BT_PRF_AVRCP_CT_SET_ABSOLUTE_VOLUME_RSP_EVT
     */
    struct avrcp_ct_set_volume_rsp_param {
        uint8_t volume; /*!< the volume which has actually been set, range is 0 to
                            0x7f, means 0% to 100% */
    } set_volume_rsp;   /*!< set absolute volume response event */
} bt_prf_avrcp_ct_cb_param_t;

/** @brief AVRCP target callback parameters */
typedef union
{
    /**
     * @brief BT_PRF_AVRCP_TG_CONNECTION_STATE_EVT
     */
    struct avrc_tg_conn_stat_param {
        uint8_t       connected; /*!< whether AVRC connection is set up */
        bt_dev_addr_t peer_addr; /*!< remote bluetooth device address */
    } conn_stat;                 /*!< AVRC connection status */

    /**
     * @brief BT_PRF_AVRCP_TG_REMOTE_FEATURES_EVT
     */
    struct avrc_tg_rmt_feats_param {
        uint32_t      feat_mask;    /*!< AVRC feature mask of remote device */
        uint16_t      ct_feat_flag; /*!< feature flag of remote device as CT */
        bt_dev_addr_t peer_addr;    /*!< remote bluetooth device address */
    } rmt_feats;                    /*!< AVRC features discovered through SDP */

    /**
     * @brief BT_PRF_AVRCP_TG_PASSTHROUGH_CMD_EVT
     */
    struct avrc_tg_psth_cmd_param {
        uint8_t operation_id;    /*!< passthrough command code */
        uint8_t operation_state; /*!< 0 for PRESSED, 1 for RELEASED */
    } psth_cmd;                  /*!< passthrough command */

    /**
     * @brief BT_PRF_AVRCP_TG_SET_ABSOLUTE_VOLUME_CMD_EVT
     */
    struct avrc_tg_set_abs_vol_param {
        uint8_t volume; /*!< volume ranges from 0 to 127 */
    } set_abs_vol;      /*!< set absolute volume command targeted on audio sink */

    /**
     * @brief BT_PRF_AVRCP_TG_REGISTER_NOTIFICATION_EVT
     */
    struct avrc_tg_reg_ntf_param {
        uint8_t  tl;
        uint8_t  event_id; /*!< event id of AVRC RegisterNotification */
        uint32_t interval; /*!< event playback interval  */
    } reg_ntf;             /*!< register notification */
} bt_prf_avrcp_tg_cb_param_t;

/**
 * @brief      AVRCP controller event callback function type
 * @param[in]  event         Event type
 * @param[in]  param         Pointer to callback parameter
 */
typedef void (*bt_prf_avrcp_ct_cb_t)(bt_prf_avrcp_ct_cb_event_t event, bt_prf_avrcp_ct_cb_param_t *param);

/**
 * @brief           AVRCP target event callback function type
 * @param           event : Event type
 * @param           param : Pointer to callback parameter
 */
typedef void (*bt_prf_avrcp_tg_cb_t)(bt_prf_avrcp_tg_cb_event_t event, bt_prf_avrcp_tg_cb_param_t *param);

/**
 * @brief      register callback function.
 * @param[in]  callback         callback function
 */
void bt_prf_avrcp_ct_register_callback(bt_prf_avrcp_ct_cb_t callback);

/**
 * @brief      Initialize the bluetooth AVRCP controller module.
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_avrcp_ct_init(void);

/**
 * @brief      Send get rgister notification capabilites command to AVRCP target
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_avrcp_ct_cmd_get_rn_capabilities(void);

/**
 * @brief      Send register notification command to AVRCP target
 * @param[in]  event_id           id of notification events
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_avrcp_ct_cmd_register_notification(bt_prf_avrcp_rn_event_ids_t event_id);

/**
 * @brief      Send passthrough command to AVRCP target
 * @param[in]  operation_id         passthrough command code \ref
 * bt_prf_avrcp_op_id_t
 * @param[in]  operation_state      passthrough command key state \ref
 * bt_prf_avrcp_op_stat_t
 * @return     bt_stack_status_t    BT_STACK_STATUS_SUCCESS: success
 *                                  other: failed
 */
bt_stack_status_t bt_prf_avrcp_ct_cmd_passthrough(bt_prf_avrcp_op_id_t   operation_id,
                                                  bt_prf_avrcp_op_stat_t operation_state);

/**
 * @brief      Send GET_PLAY_STATUS PDU to AVRCP target, the result return on
 *             BT_PRF_AVRCP_CT_PLAY_STATUS_RSP_EVT
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_avrcp_ct_cmd_get_play_status(void);

/**
 * @brief      Send set absolute volume command to AVRCP target
 * @param[in]  volume             volume from 0 to 0x7f, means 0% to 100%
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_avrcp_ct_cmd_set_absolute_volume(uint8_t volume);

/**
 * @brief      register callback function.
 * @param[in]  callback         callback function
 */
void bt_prf_avrcp_tg_register_callback(bt_prf_avrcp_tg_cb_t callback);

/**
 * @brief      Initialize the bluetooth AVRCP target module.
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_avrcp_tg_init(void);

/**
 *
 * @brief      Set the event notification capabilities on local AVRCP target.
 * @param[in]  cap_bit_mask       notification event_id capability bit mask \ref bt_prf_avrcp_rn_event_ids_t
 *
 */
void bt_prf_avrcp_tg_set_rn_evt_cap(uint16_t cap_bit_mask);

/**
 *
 * @brief      Get the event notification capabilities on local AVRCP target.
 * @return     cap_bit_mask       notification event_id capability bit mask \ref bt_prf_avrcp_rn_event_ids_t
 *
 */
uint16_t bt_prf_avrcp_tg_get_rn_evt_cap(void);

/**
 *
 * @brief           Send RegisterNotification Response to remote AVRCP controller.
 * @param[in]       event_id: notification event ID that remote AVRCP CT registers
 * @param[in]       rsp: notification response code
 * @param[in]       param: parameters included in the specific notification
 *
 * @return          bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 *
 */
bt_stack_status_t bt_prf_avrcp_tg_send_rn_rsp(bt_prf_avrcp_rn_event_ids_t event_id, bt_prf_avrcp_ctype_response_t rsp,
                                              bt_prf_avrcp_rn_param_t *param);

#if defined(CONFIG_BT_AVRCP_VOL_CONTROL) && CONFIG_BT_AVRCP_VOL_CONTROL
/**
 *
 * @brief           Send RegisterNotification Response of BT_PRF_AVRCP_NOTIFICATION_VOLUME_CHANGE to remote AVRCP
 * controller.
 * @param[in]       volume: absolute volume range 0 ~ 127.
 *
 * @return          bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 *
 */
bt_stack_status_t bt_prf_avrcp_tg_notify_vol_changed(uint8_t volume);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BT_STACK_AVRCP_H__ */
