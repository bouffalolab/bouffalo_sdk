/**
 * AVRCP Header File
 *
 * Copyright (c) 2021-2022 Bouffalolab Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef _INCLUDE_BLUETOOTH_AVRCP_H_
#define _INCLUDE_BLUETOOTH_AVRCP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <avctp.h>

/** AVRCP ctype */
#define AVRCP_CONTROL           0x00
#define AVRCP_STATUS            0x01
#define AVRCP_SPECIFIC_INQUIRY  0x02
#define AVRCP_NOTIFY            0x03
#define AVRCP_GENERAL_INQUIRY   0x04

/** AVRCP response */
#define AVRCP_NOT_IMPLEMENTED   0x08
#define AVRCP_ACCEPTED          0x09
#define AVRCP_REJECTED          0x0A
#define AVRCP_IN_TRANSITION     0x0B
#define AVRCP_IMPLEMENTED       0x0C
#define AVRCP_CHANGED           0x0D
#define AVRCP_INTERIM           0x0F

/** AVRCP subuint type */
#define AVRCP_PANEL             0x09
#define AVRCP_UNIT              0x1F
#define SUBUINT_TYPE_POSI  0x03

/** AVRCP subuint id */
#define AVRCP_SUB_UNIT_ID       0x00
#define AVRCP_UINT_INFO_SUB_UNIT_ID 0x07

/** AVRCP opcode */
#define OPCODE_UNIT_INFO            0x30
#define OPCODE_SUBUNIT_INFO        0x31
#define OPCODE_PASS_THROUGH         0x7C
#define OPCODE_VENDOR_DEPENDENT     0x00

/** AVRCP pass through state flag */
#define PASTHR_STATE_PRESSED        0x00
#define PASTHR_STATE_RELEASED       0x01
#define STATE_FLAG_POSI                     0x07

/** AVRCP get play status */
#define PLAY_STATUS_STOPPED          0x00
#define PLAY_STATUS_PLAYING           0x01
#define PLAY_STATUS_PAUSED            0x02
#define PLAY_STATUS_FWD_SEEK        0x03
#define PLAY_STATUS_REV_SEEK         0x04
#define PLAY_STATUS_ERROR               0xFF

/** AVRCP pass through operation id */
#define AVRCP_KEY_VOL_UP                  0x41
#define AVRCP_KEY_VOL_DOWN             0x42
#define AVRCP_KEY_PLAY                       0x44
#define AVRCP_KEY_STOP                       0x45
#define AVRCP_KEY_PAUSE                     0x46
#define AVRCP_KEY_REWIND                   0x48
#define AVRCP_KEY_FAST_FORWARD      0x49
#define AVRCP_KEY_FORWARD                0x4B
#define AVRCP_KEY_BACKWARD              0x4C
#define AVRCP_KEY_MASK                       0x7f

/** AVRCP pass through mask*/
#define PASS_TH_OP_ID_MASK 0x7F
#define PASS_TH_STATE_FLAG_MASK 0x80
#define PASS_TH_OP_ID_OFFSET 0
#define PASS_TH_STATE_FLAG_OFFSET 0x07

/** AVRCP vendor operands compay id */
#define COMPANY_BT_SIG                  0x00, 0x19, 0x58

/** AVRCP vendor operands pdu id */
#define PDU_ID_GET_CAPABILITIES              0x10    //AV/C STATUS
#define PDU_ID_GET_ELEMENT_ATTRS          0x20    //AV/C STATUS
#define PDU_ID_GET_PLAY_STATUS               0x30    //AV/C STATUS
#define PDU_ID_REGISTER_NOTIFICATION    0x31    //AV/C NOTIFY
#define PDU_ID_REQUEST_CONTINUE_RSP     0x40    //AV/C CONTROL
#define PDU_ID_ABORT_CONTINUE_RSP         0x41    //AV/C CONTROL
#define PDU_ID_SET_ABSOLUTE_VOLUME      0x50    //AV/C CONTROL

#define ABS_VOL_MASK 0x7f

/** AVRCP capability id */
#define CAP_ID_COMPANY_ID                       0x02
#define CAP_ID_EVENTS_SUPPORTED           0x03

/** AVRCP vendor notification event id */
#define EVENT_PLAYBACK_STATUS_CHANGED                         0x01
#define EVENT_TRACK_CHANGED                                             0x02
#define EVENT_TRACK_REACHED_END                                     0x03
#define EVENT_TRACK_REACHED_START                                 0x04
#define EVENT_PLAYBACK_POS_CHANGED                               0x05
#define EVENT_BATT_STATUS_CHANGED                                 0x06
#define EVENT_SYSTEM_STATUS_CHANGED                            0x07
#define EVENT_PLAYER_APPLICATION_SETTING_CHANGED   0x08
#define EVENT_NOW_PLAYING_CONTENT_CHANGED               0x09
#define EVENT_AVAILABLE_PLAYERS_CHANGED                     0x0a
#define EVENT_ADDRESSED_PLAYER_CHANGED                      0x0b
#define EVENT_UIDS_CHANGED                                               0x0c
#define EVENT_VOLUME_CHANGED                                          0x0d

enum bt_avrcp_error_code{
    BT_AVRCP_ERR_INVALID_CMD = 0,
    BT_AVRCP_ERR_INVALID_PARAM = 0x01,
    BT_AVRCP_ERR_PARAM_CONT_ERROR = 0x02,
    BT_AVRCP_ERR_INTERNAL_ERROR = 0x03,
    BT_AVRCP_ERR_OP_CMPLT_WIHTOUT_ERROR = 0x04,
    BT_AVRCP_ERR_UID_CHANGED = 0x05,
    //BT_AVRCP_ERR_RVSD = 0x06,//reserved
    BT_AVRCP_ERR_INVALID_DIRECTION = 0x07,
    BT_AVRCP_ERR_NOT_A_DIRECTORY = 0x08,
    BT_AVRCP_ERR_DOES_NOT_EXIST = 0x09,
    BT_AVRCP_ERR_INVALID_SCOPE = 0x0a,
    BT_AVRCP_ERR_RANGE_OUT_OF_BOUNDS = 0x0b,
    BT_AVRCP_ERR_FOLDER_ITEM_IS_NOT_PLAYABLE = 0x0c,
    BT_AVRCP_ERR_MEDIA_IN_USE = 0x0d,
    BT_AVRCP_ERR_NOW_PLAYING_LIST_FULL = 0x0e,
    BT_AVRCP_ERR_SEARCH_NOT_SUPPORTED = 0x0f,
    BT_AVRCP_ERR_SEARCH_IN_PROGRESS = 0x10,
    BT_AVRCP_ERR_INVALID_PLAYER_ID = 0x11,
    BT_AVRCP_ERR_PLAYER_NOT_BROWSABLE = 0x12,
    BT_AVRCP_ERR_PLAYER_NOT_ADDRESSED = 0x13,
    BT_AVRCP_ERR_NO_VALID_SEARCH_RESULTS = 0x14,
    BT_AVRCP_ERR_NO_AVAILABLE_PLAYERS = 0x15,
    BT_AVRCP_ERR_ADDRESSED_PLAYER_CHANGED = 0x16,
};

enum bt_avrcp_unit_type{
    BT_AVRCP_UNIT_TYPE_MONITOR = 0x00,
    BT_AVRCP_UNIT_TYPE_AUDIO,
    BT_AVRCP_UNIT_TYPE_PRINTER,
    BT_AVRCP_UNIT_TYPE_DISC,
    BT_AVRCP_UNIT_TYPE_TAPE_RECORDER_PLAYER,
    BT_AVRCP_UNIT_TYPE_TUNER,
    BT_AVRCP_UNIT_TYPE_CA,
    BT_AVRCP_UNIT_TYPE_CAMERA,
    BT_AVRCP_UNIT_TYPE_PANEL = 0x09,
    BT_AVRCP_UNIT_TYPE_BULLETIN_BOARD,
    BT_AVRCP_UNIT_TYPE_CAMERA_STORAGE,
    BT_AVRCP_UNIT_TYPE_VENDOR_UNIQUE = 0x1c,
    BT_AVRCP_UNIT_TYPE_SUBUNIT_TYPE_EXTENDED = 0x1e,
    BT_AVRCP_UNIT_TYPE_UNIT,
};
/** @brief AVRCP Callback State */
enum AVRCP_CB_STATE {
	/** Chain State */
	BT_AVRCP_CHAIN_CONNECTED = 0,
	/** Chain State */
	BT_AVRCP_CHAIN_DISCONNECTED = 1,
};

struct bt_avrcp {
	struct bt_avctp session;
};

struct avrcp_callback {
	void (*chain)(struct bt_conn *conn, uint8_t state);
	void (*abs_vol)(uint8_t vol);
	void (*play_status)(uint32_t song_len, uint32_t song_pos, uint8_t status);
	void (*tg_register_notification_event)(uint8_t evt, uint8_t *para,uint16_t para_len);
};

int bt_avrcp_init(void);
void avrcp_cb_register(struct avrcp_callback *cb);
int avrcp_recv(struct bt_l2cap_chan *chan, struct net_buf *buf);
struct bt_avrcp *bt_avrcp_connect(struct bt_conn *conn);

int avrcp_pasthr_cmd(struct bt_avctp *session, uint8_t released, uint8_t opid);
int avrcp_get_play_status_cmd(struct bt_avctp *session);
int avrcp_reg_not_cmd(struct bt_avctp *session, uint8_t event);

int avrcp_change_volume(uint8_t vol);
int avrcp_hdl_set_abs_vol(struct bt_avctp *session, uint8_t trans_lab, uint8_t *params);
int avrcp_hdl_reg_not_vol(struct bt_avctp *session, uint8_t trans_lab, uint8_t rsp_type);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_BLUETOOTH_AVRCP_H_ */


