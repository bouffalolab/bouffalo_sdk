/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#if defined(CONFIG_BT_A2DP) && (CONFIG_BT_A2DP == 1)

#include <stdlib.h>
// #include <aos/kv.h>
// #include <ulog/ulog.h>
#include "bt.h"
#include "msp_app_bt.h"
#include <smart_audio.h>

#define TAG "smtaudio_ctrl_bt_a2dp"

#define AVRCP_PLAY_STATUS_INQUIRE_TIME 2000

static const char *  s_a2d_conn_state_str[]  = {"Disconnected_normal", "Disconnected_abnormal", "Connecting", "Connected", "Disconnecting"};
static const char *  s_a2d_audio_state_str[] = {"Suspended", "Stopped", "Started"};
static msp_mutex_t   s_a2dp_mutex;
static msp_timer_t   s_a2dp_inquire_timer;
static int           g_a2dp_connect_state        = BT_PRF_A2DP_CONNECTION_STATE_DISCONNECTED_NORMAL;
static unsigned char cur_bt_addr[BT_BD_ADDR_LEN] = {0};

static void                  avrcp_play_status_inquire_timer_start(void);
static void                  avrcp_play_status_inquire_timer_stop(void);
static void                  bt_callback(msp_app_bt_event_t event, msp_app_bt_param_t *param);
static msp_app_bt_callback_t g_bt_cb = NULL;

static int bt_reconnect_flag;

static int bt_a2dp_init(void);
static int bt_a2dp_deinit(void);
static int bt_a2dp_start(const char *url, uint64_t seek_time, int resume);
static int bt_a2dp_pause(void);
static int bt_a2dp_stop(void);
static int bt_a2dp_resume(void);
static int bt_a2dp_vol_set(int vol);
static int bt_a2dp_vol_up(int vol);
static int bt_a2dp_vol_down(int vol);

static smtaudio_ops_node_t ctrl_bt_a2dp = {
    .name     = "bt_a2dp",
    .prio     = 2,
    .id       = SMTAUDIO_BT_A2DP,
    .status   = SMTAUDIO_STATE_STOP,
    .init     = bt_a2dp_init,
    .deinit   = bt_a2dp_deinit,
    .start    = bt_a2dp_start,
    .pause    = bt_a2dp_pause,
    .stop     = bt_a2dp_stop,
    .resume   = bt_a2dp_resume,
    .vol_get  = NULL, /* use system volume */
    .vol_set  = bt_a2dp_vol_set,
    .vol_up   = bt_a2dp_vol_up,
    .vol_down = bt_a2dp_vol_down,
};

static uint8_t _bt_get_lable(void)
{
    static uint8_t lable = 0;

    lable++;
    lable = lable % 15;

    return lable;
}
static void _bt_gap_cb(bt_stack_cb_event_t event, bt_stack_cb_param_t *param)
{
    switch (event) {
    case BT_STACK_AUTH_CMPL_EVT: {
        if (param->auth_cmpl.status == BT_STACK_STATUS_SUCCESS) {
            LOGD(TAG, "authentication success: %s", param->auth_cmpl.device_name);

            msp_app_bt_param_t param_m;
            memcpy(param_m.paired.remote_addr, param->auth_cmpl.bda.val, BT_BD_ADDR_LEN);
            bt_callback(MSP_APP_BT_PAIRED, &param_m);
            if (g_bt_cb) {
                g_bt_cb(MSP_APP_BT_PAIRED, &param_m);
            }
        } else {
            LOGE(TAG, "authentication failed, status:%d", param->auth_cmpl.status);
        }
        break;
    }
    case BT_STACK_CFM_REQ_EVT:
        LOGD(TAG, "MSP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d",
             param->cfm_req.num_val);
        // MSP_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    case BT_STACK_KEY_NOTIF_EVT:
        LOGD(TAG, "MSP_BT_GAP_KEY_NOTIF_EVT passkey:%d", param->key_notif.passkey);
        break;
    case BT_STACK_KEY_REQ_EVT:
        LOGD(TAG, "MSP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
        break;
    // case MSP_BT_GAP_READ_RSSI_DELTA_EVT:
    //     LOGD(TAG, "rssi delta %d", param->read_rssi_delta.rssi_delta);
    //     break;
    case BT_STACK_SET_EIR_DATA_EVT:
        LOGD(TAG, "EIR DATA %d", param->set_eir.stat);
        break;
    default: {
        LOGE(TAG, "unhandled event: %d", event);
        break;
    }
    }
    return;
}

static void _bt_app_a2d_cb(bt_prf_a2dp_cb_event_t event, bt_prf_a2dp_cb_param_t *param)
{
    bt_prf_a2dp_cb_param_t *a2d = (bt_prf_a2dp_cb_param_t *)(param);

    LOGD(TAG, "%s evt %d", __func__, event);
    switch (event) {
    case BT_PRF_A2DP_CONNECTION_STATE_EVT: {
        uint8_t *bda;

        bda = a2d->conn_stat.peer_addr.val;
        LOGD(TAG, "A2DP connection state: %s, [%02x:%02x:%02x:%02x:%02x:%02x]",
             s_a2d_conn_state_str[a2d->conn_stat.state], bda[0], bda[1], bda[2], bda[3], bda[4],
             bda[5]);
        if (a2d->conn_stat.state == BT_PRF_A2DP_CONNECTION_STATE_DISCONNECTED_NORMAL) {
            avrcp_play_status_inquire_timer_stop();
            msp_app_bt_param_t param;
            memcpy(param.a2dp_conn.remote_addr, bda, BT_BD_ADDR_LEN);
            if (bt_reconnect_flag) {
                LOGD(TAG, "try reconnect");
                msp_app_bt_a2dp_connect(cur_bt_addr);
            } else {
                bt_callback(MSP_APP_BT_A2DP_DISCONNECTED, &param);
                if (g_bt_cb) {
                    g_bt_cb(MSP_APP_BT_A2DP_DISCONNECTED, &param);
                }
            }
            g_a2dp_connect_state = BT_PRF_A2DP_CONNECTION_STATE_DISCONNECTED_NORMAL;
        } else if(a2d->conn_stat.state == BT_PRF_A2DP_CONNECTION_STATE_DISCONNECTED_ABNORMAL) {
            avrcp_play_status_inquire_timer_stop();
            msp_app_bt_param_t param;
            memcpy(param.a2dp_conn.remote_addr, bda, BT_BD_ADDR_LEN);
            bt_callback(MSP_APP_BT_A2DP_LINK_LOSS, &param);
            if (g_bt_cb) {
                g_bt_cb(MSP_APP_BT_A2DP_LINK_LOSS, &param);
            }
            LOGD(TAG, "try reconnect");
            msp_app_bt_a2dp_connect(cur_bt_addr);
            g_a2dp_connect_state = BT_PRF_A2DP_CONNECTION_STATE_DISCONNECTED_ABNORMAL;
            bt_reconnect_flag    = 1;
        } else if (a2d->conn_stat.state == BT_PRF_A2DP_CONNECTION_STATE_CONNECTED) {
            if ((bt_reconnect_flag == 0) || (memcmp(cur_bt_addr, bda, BT_BD_ADDR_LEN) != 0)) {
                msp_app_bt_param_t param;
                memcpy(param.a2dp_conn.remote_addr, bda, BT_BD_ADDR_LEN);
                bt_callback(MSP_APP_BT_A2DP_CONNECTED, &param);
                if (g_bt_cb) {
                    g_bt_cb(MSP_APP_BT_A2DP_CONNECTED, &param);
                }
                g_a2dp_connect_state = BT_PRF_A2DP_CONNECTION_STATE_CONNECTED;
            }
            memcpy((void *)cur_bt_addr, (void *)bda, BT_BD_ADDR_LEN);
            bt_reconnect_flag = 0;
        } else if (a2d->conn_stat.state == BT_PRF_A2DP_CONNECTION_STATE_CONNECTING) {
            msp_app_bt_param_t param;
            memcpy(param.a2dp_conn.remote_addr, bda, BT_BD_ADDR_LEN);
            bt_callback(MSP_APP_BT_A2DP_CONNECTING, &param);
            if (g_bt_cb) {
                g_bt_cb(MSP_APP_BT_A2DP_CONNECTING, &param);
            }
            g_a2dp_connect_state = BT_PRF_A2DP_CONNECTION_STATE_CONNECTING;
        }
        break;
    }
    case BT_PRF_A2DP_AUDIO_STATE_EVT: {

        LOGD(TAG, "A2DP audio state: %s", s_a2d_audio_state_str[a2d->audio_stat.state]);
        // g_tg_bt_audio_state = a2d->audio_stat.state;
        if (BT_PRF_A2DP_AUDIO_STATE_STARTED == a2d->audio_stat.state) {
            avrcp_play_status_inquire_timer_start();

            bt_callback(MSP_APP_BT_A2DP_PLAY_STATUS_PLAYING, NULL);
            if (g_bt_cb) {
                g_bt_cb(MSP_APP_BT_A2DP_PLAY_STATUS_PLAYING, NULL);
            }

        } else if (BT_PRF_A2DP_AUDIO_STATE_REMOTE_SUSPEND == a2d->audio_stat.state ||
                   BT_PRF_A2DP_AUDIO_STATE_STOPPED == a2d->audio_stat.state) {
            avrcp_play_status_inquire_timer_stop();

            bt_callback(MSP_APP_BT_A2DP_PLAY_STATUS_STOPPED, NULL);
            if (g_bt_cb) {
                g_bt_cb(MSP_APP_BT_A2DP_PLAY_STATUS_STOPPED, NULL);
            }
        }

        break;
    }
    case BT_PRF_A2DP_AUDIO_SBC_CFG_EVT: {
        LOGD(TAG, "A2DP audio stream configuration");
        // for now only SBC stream is supported
        LOGD(TAG, "Configure audio player %x-%x-%x-%x", a2d->audio_cfg.sbc.config[0],
                a2d->audio_cfg.sbc.config[1], a2d->audio_cfg.sbc.min_bitpool,
                a2d->audio_cfg.sbc.max_bitpool);
        break;
    }
    default:
        LOGE(TAG, "%s unhandled evt %d", __func__, event);
        break;
    }
}

static void bt_av_new_track()
{
    //Register notifications and request metadata
    // msp_avrc_ct_send_metadata_cmd(_bt_get_lable(),
    //                               MSP_AVRC_MD_ATTR_TITLE | MSP_AVRC_MD_ATTR_ARTIST |
    //                                   MSP_AVRC_MD_ATTR_ALBUM | MSP_AVRC_MD_ATTR_GENRE);
    // bt_prf_avrcp_ct_cmd_register_notification(BT_PRF_AVRCP_NOTIFICATION_TRACK_CHANGE);
}

static void _bt_avrcp_notify_evt_handler(bt_prf_avrcp_rn_event_ids_t event_id, bt_prf_avrcp_rn_param_t *event_parameter)
{
    LOGE(TAG, "%s %d", __func__, event_id);
    msp_app_bt_param_t param;

    switch (event_id) {
    case BT_PRF_AVRCP_NOTIFICATION_TRACK_CHANGE:
        bt_av_new_track();
        break;
    case BT_PRF_AVRCP_NOTIFICATION_VOLUME_CHANGE:
        /* FIXME */
        param.a2dp_vol.volume = event_parameter->volume;
        bt_callback(MSP_APP_BT_A2DP_VOLUME_CHANGE, &param);
        if (g_bt_cb) {
            g_bt_cb(MSP_APP_BT_A2DP_VOLUME_CHANGE, &param);
        }
        break;
    case BT_PRF_AVRCP_NOTIFICATION_PLAY_STATUS_CHANGE: {
        msp_app_bt_event_t status;

        switch (event_parameter->playback) {
        case BT_PRF_AVRCP_PLAYBACK_STOPPED:
            status = MSP_APP_BT_AVRCP_STATUS_STOPPED;
            break;
        case BT_PRF_AVRCP_PLAYBACK_PLAYING:
            status = MSP_APP_BT_AVRCP_STATUS_PLAYING;
            break;
        case BT_PRF_AVRCP_PLAYBACK_PAUSED:
            status = MSP_APP_BT_AVRCP_STATUS_PAUSEED;
            break;
        default:
            return;
        }
        bt_prf_avrcp_ct_cmd_register_notification(BT_PRF_AVRCP_NOTIFICATION_PLAY_STATUS_CHANGE);
        bt_callback(status, NULL);
        if (g_bt_cb) {
            g_bt_cb(status, NULL);
        }
        break;
        }
    default:
        break;
    }
}

static void _bt_app_avrcp_ct_cb(bt_prf_avrcp_ct_cb_event_t event, bt_prf_avrcp_ct_cb_param_t *param)
{
    bt_prf_avrcp_ct_cb_param_t *rc = (bt_prf_avrcp_ct_cb_param_t *)(param);

    LOGD(TAG, "%s evt %d", __func__, event);
    switch (event) {
    case BT_PRF_AVRCP_CT_CONNECTION_STATE_EVT: {
        uint8_t *bda = rc->conn_stat.peer_addr.val;
        LOGD(TAG, "AVRC conn_state evt: state %d, [%02x:%02x:%02x:%02x:%02x:%02x]",
             rc->conn_stat.connected, bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);

        if (rc->conn_stat.connected) {
            bt_av_new_track();
            bt_prf_avrcp_ct_cmd_register_notification(BT_PRF_AVRCP_NOTIFICATION_PLAY_STATUS_CHANGE);
        }
        break;
    }
    case BT_PRF_AVRCP_CT_PASSTHROUGH_RSP_EVT: {
        msp_app_avrcp_cmd_type_t cmd;

        LOGD(TAG, "AVRC passthrough rsp: key_code 0x%x, key_state %d", rc->psth_rsp.operation_state,
             rc->psth_rsp.operation_id);
        switch (rc->psth_rsp.operation_state) {
        case BT_PRF_AVRCP_OP_ID_PLAY:
            cmd = MSP_APP_BT_AVRCP_CMD_PLAY;
            break;
        case BT_PRF_AVRCP_OP_ID_PAUSE:
            cmd = MSP_APP_BT_AVRCP_CMD_PAUSE;
            break;
        case BT_PRF_AVRCP_OP_ID_FORWARD:
            cmd = MSP_APP_BT_AVRCP_CMD_FORWARD;
            break;
        case BT_PRF_AVRCP_OP_ID_BACKWARD:
            cmd = MSP_APP_BT_AVRCP_CMD_BACKWARD;
            break;
        case BT_PRF_AVRCP_OP_ID_FAST_FORWARD:
            cmd = MSP_APP_BT_AVRCP_CMD_FAST_FORWARD;
            break;
        case BT_PRF_AVRCP_OP_ID_REWIND:
            cmd = MSP_APP_BT_AVRCP_CMD_REWIND;
            break;
        case BT_PRF_AVRCP_OP_ID_STOP:
            cmd = MSP_APP_BT_AVRCP_CMD_STOP;
            break;
        default:
            return;
        }

        msp_app_bt_param_t param;
        param.a2dp_cmd.cmd = cmd;
        bt_callback(MSP_APP_BT_A2DP_CMD, &param);
        if (g_bt_cb) {
            g_bt_cb(MSP_APP_BT_A2DP_CMD, &param);
        }
        break;
    }
    case BT_PRF_AVRCP_CT_METADATA_RSP_EVT: {
        uint8_t *attr_text;

        attr_text = (uint8_t *)malloc(rc->meta_rsp.attr_length + 1);
        if (attr_text == NULL) {
            LOGD(TAG, "attr_text malloc failed");
            return;
        }
        memcpy(attr_text, rc->meta_rsp.attr_text, rc->meta_rsp.attr_length);
        attr_text[rc->meta_rsp.attr_length] = 0;

        LOGD(TAG, "AVRC metadata rsp: attribute id 0x%x, %s", rc->meta_rsp.attr_id, attr_text);
        free(attr_text);
        break;
    }
    case BT_PRF_AVRCP_CT_CHANGE_NOTIFY_EVT: {
        LOGD(TAG, "AVRC event notification: %d, param: %d", rc->change_ntf.event_id,
             rc->change_ntf.event_parameter);
        _bt_avrcp_notify_evt_handler(rc->change_ntf.event_id, &rc->change_ntf.event_parameter);
        break;
    }
    case BT_PRF_AVRCP_CT_REMOTE_FEATURES_EVT: {
        LOGD(TAG, "AVRC remote features %x", rc->rmt_feats.feat_mask);
        break;
    }
    case BT_PRF_AVRCP_CT_PLAY_STATUS_RSP_EVT: {
        LOGD(TAG, "AVRC get play status event");
        msp_app_bt_param_t param;
        param.avrcp_get_play_status.play_status = rc->get_rn_play_status_rsp.play_status;
        param.avrcp_get_play_status.song_len    = rc->get_rn_play_status_rsp.song_len;
        param.avrcp_get_play_status.song_pos    = rc->get_rn_play_status_rsp.song_pos;

        bt_callback(MSP_APP_BT_AVRCP_GET_PLAY_STATUS, &param);
        if (g_bt_cb) {
            g_bt_cb(MSP_APP_BT_AVRCP_GET_PLAY_STATUS, &param);
        }
        break;
    }
    case BT_PRF_AVRCP_CT_SET_ABSOLUTE_VOLUME_RSP_EVT: {
        LOGD(TAG, "AVRC VOLUME set %d", rc->set_volume_rsp.volume);
        msp_app_bt_param_t param;
        param.a2dp_vol.volume = rc->set_volume_rsp.volume;
        bt_callback(MSP_APP_BT_A2DP_VOLUME_CHANGE, &param);
        if (g_bt_cb) {
            g_bt_cb(MSP_APP_BT_A2DP_VOLUME_CHANGE, &param);
        }
        break;
    }
    default:
        LOGE(TAG, "%s unhandled evt %d", __func__, event);
        break;
    }
}
#if 0
static void _bt_app_avrcp_tg_cb(msp_avrc_tg_cb_event_t event, msp_avrc_tg_cb_param_t *param)
{
    msp_avrc_tg_cb_param_t *rc = (msp_avrc_tg_cb_param_t *)(param);

    LOGD(TAG, "%s evt %d", __func__, event);
    switch (event) {
    case MSP_AVRC_TG_REGISTER_NOTIFICATION_EVT: {
        if (rc->reg_ntf.event_id == MSP_AVRC_RN_VOLUME_CHANGE) {
            msp_avrc_rn_param_t param;

            param.volume = 0x7f;
            msp_avrc_tg_send_rn_rsp(MSP_AVRC_RN_VOLUME_CHANGE, MSP_AVRC_RN_RSP_INTERIM, &param);
        }
    } break;
    case MSP_AVRC_TG_SET_ABSOLUTE_VOLUME_CMD_EVT: {
        LOGD(TAG, "volume: %d", rc->set_abs_vol.volume);
        msp_app_bt_param_t param;
        param.a2dp_vol.volume = rc->set_abs_vol.volume;
        bt_callback(MSP_APP_BT_A2DP_VOLUME_CHANGE, &param);
        if (g_bt_cb) {
            g_bt_cb(MSP_APP_BT_A2DP_VOLUME_CHANGE, &param);
        }
    } break;
    default:;
    }
}
#endif
static void _bt_app_a2d_data_cb(const uint8_t *data, uint32_t len)
{
    static uint32_t s_pkt_cnt = 0;

    if (++s_pkt_cnt % 100 == 0) {
        LOGD(TAG, "Audio packet count %u", s_pkt_cnt);
    }
}

int msp_app_bt_gap_set_scan_mode(int enable)
{
    bt_stack_set_connectable(enable);
    bt_stack_set_discoverable(enable);
    LOGD(TAG, "set scan mode:%d", enable);
    return 0;
}

int msp_app_bt_avrcp_send_passthrouth_cmd(msp_app_avrcp_cmd_type_t cmd_type)
{
    int32_t cmd;

    LOGD(TAG, "%s start: %d", __func__, cmd_type);
    switch (cmd_type) {
    case MSP_APP_BT_AVRCP_CMD_PLAY:
        cmd = BT_PRF_AVRCP_OP_ID_PLAY;
        break;
    case MSP_APP_BT_AVRCP_CMD_PAUSE:
        cmd = BT_PRF_AVRCP_OP_ID_PAUSE;
        break;
    case MSP_APP_BT_AVRCP_CMD_FORWARD:
        cmd = BT_PRF_AVRCP_OP_ID_FORWARD;
        break;
    case MSP_APP_BT_AVRCP_CMD_BACKWARD:
        cmd = BT_PRF_AVRCP_OP_ID_BACKWARD;
        break;
    case MSP_APP_BT_AVRCP_CMD_FAST_FORWARD:
        cmd = BT_PRF_AVRCP_OP_ID_FAST_FORWARD;
        break;
    case MSP_APP_BT_AVRCP_CMD_REWIND:
        cmd = BT_PRF_AVRCP_OP_ID_REWIND;
        break;
    case MSP_APP_BT_AVRCP_CMD_STOP:
        cmd = BT_PRF_AVRCP_OP_ID_STOP;
        break;
    case MSP_APP_BT_AVRCP_CMD_VOL_UP:
        cmd = BT_PRF_AVRCP_OP_ID_VOL_UP;
        break;
    case MSP_APP_BT_AVRCP_CMD_VOL_DOWN:
        cmd = BT_PRF_AVRCP_OP_ID_VOL_DOWN;
        break;
    default:
        return -1;
    }
    bt_stack_status_t ret;

    msp_mutex_lock(&s_a2dp_mutex, MSP_WAIT_FOREVER);
    ret = bt_prf_avrcp_ct_cmd_passthrough(cmd, 0);

    if (ret != BT_STACK_STATUS_SUCCESS) {
        LOGE(TAG, "%s %d", __func__, 0);
    }

    ret = bt_prf_avrcp_ct_cmd_passthrough(cmd, 1);

    if (ret != BT_STACK_STATUS_SUCCESS) {
        LOGE(TAG, "%s %d", __func__, 1);
    }

    msp_mutex_unlock(&s_a2dp_mutex);

    return 0;
}

int msp_app_bt_avrcp_change_vol(uint8_t vol)
{
    bt_prf_avrcp_rn_param_t param;
    param.volume = vol;

    return bt_prf_avrcp_tg_send_rn_rsp(BT_PRF_AVRCP_NOTIFICATION_VOLUME_CHANGE, BT_AVRCP_RESPONSE_CHANGED_STABLE, &param);
}

void msp_app_bt_avrcp_get_play_status(void)
{
    bt_prf_avrcp_ct_cmd_get_play_status();
}

int msp_app_bt_a2dp_connect(uint8_t remote_addr[])
{
    return bt_prf_a2dp_sink_connect((bt_dev_addr_t *)remote_addr);
}

int msp_app_bt_a2dp_disconnect(void)
{
    int ret;

    ret = bt_prf_a2dp_sink_disconnect((bt_dev_addr_t *)cur_bt_addr);
    memset((void *)cur_bt_addr, 0, BT_BD_ADDR_LEN); // 清除连接的MAC地址
    return ret;
}

int msp_app_bt_a2dp_get_connect_status(void)
{
    return g_a2dp_connect_state;
}

unsigned char *msp_app_bt_get_remote_addrss(void)
{
    return cur_bt_addr;
}

static int a2dp_audio_status;
int        msp_app_bt_a2dp_get_status(void)
{
    return a2dp_audio_status;
}

int msp_app_bt_set_device_name(char *name)
{
    int ret;
    ret = bt_stack_set_name(name);
    return ret;
}

int msp_app_bt_a2dp_register_cb(msp_app_bt_callback_t callback)
{
    g_bt_cb = callback;

    return 0;
}

static int bt_audio_state_app = BT_PRF_AVRCP_PLAYBACK_STOPPED;
void bt_audio_state_check(void)
{
    if((ctrl_bt_a2dp.status == SMTAUDIO_STATE_PLAYING) && ((bt_audio_state_app == BT_PRF_AVRCP_PLAYBACK_STOPPED) || (bt_audio_state_app == BT_PRF_AVRCP_PLAYBACK_PAUSED))) {
        //设备端处于播放状态, app处于非播放状态
        ctrl_bt_a2dp.status = SMTAUDIO_STATE_STOP;
        LOGD(TAG, "change a2dp status: %d", ctrl_bt_a2dp.status);
    } else if((bt_audio_state_app == BT_PRF_AVRCP_PLAYBACK_PLAYING) && ((ctrl_bt_a2dp.status == SMTAUDIO_STATE_STOP) || (ctrl_bt_a2dp.status == SMTAUDIO_STATE_PAUSE))) {
        //设备端处于非播放状态, app处于播放状态
        ctrl_bt_a2dp.status = SMTAUDIO_STATE_PLAYING;
        LOGD(TAG, "change a2dp status: %d", ctrl_bt_a2dp.status);
    }
}
static void bt_callback(msp_app_bt_event_t event, msp_app_bt_param_t *param)
{
    uint8_t *addr;

    int        smt_cur_state, smt_last_state;
    static int a2dp_tg_play_status;

    switch (event) {
    case MSP_APP_BT_PAIRED:
        addr = param->paired.remote_addr;
        LOGD(TAG, "PAIRED remote_addr: %02x:%02x:%02x:%02x:%02x:%02x", addr[0], addr[1], addr[2],
             addr[3], addr[4], addr[5]);
        break;
    case MSP_APP_BT_A2DP_CONNECTED:
        addr = param->a2dp_conn.remote_addr;
        LOGD(TAG, "CONNECTED remote_addr: %02x:%02x:%02x:%02x:%02x:%02x", addr[0], addr[1], addr[2],
             addr[3], addr[4], addr[5]);
        break;

    case MSP_APP_BT_A2DP_DISCONNECTED:
        addr = param->a2dp_conn.remote_addr;
        LOGD(TAG, "DISCONNECTED remote_addr: %02x:%02x:%02x:%02x:%02x:%02x", addr[0], addr[1],
             addr[2], addr[3], addr[4], addr[5]);

        a2dp_audio_status = AUI_PLAYER_STOP;
        if (ctrl_bt_a2dp.callback) {
            ctrl_bt_a2dp.callback(SMTAUDIO_BT_A2DP, SMTAUDIO_PLAYER_EVENT_STOP);
        }
        break;
    case MSP_APP_BT_A2DP_CONNECTING:
        addr = param->a2dp_conn.remote_addr;
        LOGD(TAG, "CONNECTING remote_addr: %02x:%02x:%02x:%02x:%02x:%02x", addr[0], addr[1],
             addr[2], addr[3], addr[4], addr[5]);
        break;
    case MSP_APP_BT_A2DP_LINK_LOSS:
        addr = param->a2dp_conn.remote_addr;
        LOGD(TAG, "LINK_LOSS remote_addr: %02x:%02x:%02x:%02x:%02x:%02x", addr[0], addr[1], addr[2],
             addr[3], addr[4], addr[5]);

        a2dp_audio_status = AUI_PLAYER_STOP;
        if (ctrl_bt_a2dp.callback) {
            ctrl_bt_a2dp.callback(SMTAUDIO_BT_A2DP, SMTAUDIO_PLAYER_EVENT_STOP);
        }
        break;
    case MSP_APP_BT_A2DP_PLAY_STATUS_STOPPED:
        LOGD(TAG, "MSP_APP_BT_A2DP_PLAY_STATUS_STOPPED");
        a2dp_audio_status = AUI_PLAYER_STOP;
        if (ctrl_bt_a2dp.callback) {
            ctrl_bt_a2dp.callback(SMTAUDIO_BT_A2DP, SMTAUDIO_PLAYER_EVENT_STOP);
        }
        break;
    case MSP_APP_BT_A2DP_PLAY_STATUS_PLAYING:
        LOGD(TAG, "MSP_APP_BT_A2DP_PLAY_STATUS_PLAYING");
        a2dp_audio_status = AUI_PLAYER_PLAYING;
        if (ctrl_bt_a2dp.callback) {
            ctrl_bt_a2dp.callback(SMTAUDIO_BT_A2DP, SMTAUDIO_PLAYER_EVENT_START);
        }
        break;
    case MSP_APP_BT_A2DP_PLAY_STATUS_PAUSEED:
        LOGD(TAG, "MSP_APP_BT_A2DP_PLAY_STATUS_PAUSEED");
        a2dp_audio_status = AUI_PLAYER_PAUSED;
        if (ctrl_bt_a2dp.callback) {
            ctrl_bt_a2dp.callback(SMTAUDIO_BT_A2DP, SMTAUDIO_PLAYER_EVENT_PAUSE);
        }
        break;
    case MSP_APP_BT_AVRCP_STATUS_PAUSEED:
        LOGD(TAG, "MSP_APP_BT_AVRCP_STATUS_PAUSEED");
        break;
    case MSP_APP_BT_AVRCP_STATUS_STOPPED:
        LOGD(TAG, "MSP_APP_BT_AVRCP_STATUS_STOPPED");
        break;
    case MSP_APP_BT_AVRCP_STATUS_PLAYING:
        LOGD(TAG, "MSP_APP_BT_AVRCP_STATUS_PLAYING");
        break;
    case MSP_APP_BT_AVRCP_GET_PLAY_STATUS:
        LOGD(TAG, "AVRC PLAY STATUS total_len:%d ms  cur:%d ms  play_status:%d",
             param->avrcp_get_play_status.song_len, param->avrcp_get_play_status.song_pos,
             param->avrcp_get_play_status.play_status);
        extern void smtaudio_substate_get(int *cur_state, int *last_state);
        smtaudio_substate_get(&smt_cur_state, &smt_last_state);
        a2dp_tg_play_status = param->avrcp_get_play_status.play_status;
        LOGD(TAG, "play status, tg:%d ct_cur:%d ct_last:%d", a2dp_tg_play_status, smt_cur_state,
             smt_last_state);
        static int bt_audio_state_app_last;
        bt_audio_state_app = param->avrcp_get_play_status.play_status;
        if(bt_audio_state_app == bt_audio_state_app_last) {
            bt_audio_state_check();
        }
        bt_audio_state_app_last = bt_audio_state_app;
        break;
    case MSP_APP_BT_A2DP_VOLUME_CHANGE: {
        smtaudio_ops_node_t *audio_default_ops;
        extern smtaudio_ops_node_t *get_default_audio_ops(void);
        audio_default_ops = get_default_audio_ops();
        if (audio_default_ops) {
            audio_default_ops->vol_set(param->a2dp_vol.volume * 100 / 127);
        }
        LOGD(TAG, "VOLUME_CHANGE: %d/127", param->a2dp_vol.volume);
        break;
    }
    case MSP_APP_BT_A2DP_CMD:
        break;
    default:
        break;
    }
}

static void avrcp_play_status_inquire_timer_entry(void *timer, void *arg)
{
    msp_app_bt_avrcp_get_play_status();
}
static void avrcp_play_status_inquire_timer_start(void)
{
    msp_timer_start(&s_a2dp_inquire_timer);
}
static void avrcp_play_status_inquire_timer_stop(void)
{
    msp_timer_stop(&s_a2dp_inquire_timer);
}

int msp_app_bt_init()
{
    static int bt_init_flag;
    if (bt_init_flag) {
        return 0;
    }

    bt_init_flag = 1;
    bt_stack_init();

    bt_stack_cb_t callback;
    callback.callback = _bt_gap_cb;
    bt_stack_register_callback(&callback);
    return 0;
}

static int msp_app_bt_a2dp_init()
{
    static int a2dp_init_flag;
    if (a2dp_init_flag) {
        return 0;
    }
    a2dp_init_flag = 1;

    bt_prf_a2dp_register_callback(_bt_app_a2d_cb);
    bt_prf_a2dp_sink_register_data_callback(_bt_app_a2d_data_cb);
    bt_prf_a2dp_sink_init();

    /* initialize AVRCP controller */
    bt_prf_avrcp_ct_init();
    bt_prf_avrcp_ct_register_callback(_bt_app_avrcp_ct_cb);

    msp_mutex_new(&s_a2dp_mutex);
    msp_timer_new_ext(&s_a2dp_inquire_timer, avrcp_play_status_inquire_timer_entry, NULL,
                      AVRCP_PLAY_STATUS_INQUIRE_TIME, 1, 0);

    // msp_avrc_rn_evt_cap_mask_t evt_set;

    /* volume sync setup for IOS volume control */
    // evt_set.bits = 0x2000;
    // msp_avrc_tg_set_rn_evt_cap(&evt_set);

    // bt_stack_io_cap_t bt_io_cap  = BT_STACK_IO_CAP_NONE;
    // bt_stack_set_io_capability(bt_io_cap);

    return 0;
}

static int bt_a2dp_init(void)
{
    msp_app_bt_a2dp_init();
    return 0;
}

static int bt_a2dp_deinit(void)
{
    return 0;
}

static int bt_a2dp_start(const char *url, uint64_t seek_time, int resume)
{
    return msp_app_bt_avrcp_send_passthrouth_cmd(MSP_APP_BT_AVRCP_CMD_PLAY);
}

static int bt_a2dp_pause(void)
{
    ctrl_bt_a2dp.status = SMTAUDIO_STATE_PAUSE;
    return msp_app_bt_avrcp_send_passthrouth_cmd(MSP_APP_BT_AVRCP_CMD_PAUSE);
}

static int bt_a2dp_stop(void)
{
    ctrl_bt_a2dp.status = SMTAUDIO_STATE_STOP;
    return msp_app_bt_avrcp_send_passthrouth_cmd(MSP_APP_BT_AVRCP_CMD_PAUSE);
}

static int bt_a2dp_resume(void)
{
    return msp_app_bt_avrcp_send_passthrouth_cmd(MSP_APP_BT_AVRCP_CMD_PLAY);
}


static int bt_a2dp_vol_set(int vol)
{
    /*同时设置本地音音量*/
    smtaudio_ops_node_t *audio_default_ops;

    extern smtaudio_ops_node_t *get_default_audio_ops(void);
    audio_default_ops = get_default_audio_ops();
    if (audio_default_ops) {
        audio_default_ops->vol_set(vol);
    }

    return msp_app_bt_avrcp_change_vol(vol * 127 / 100);
}

static int bt_a2dp_vol_up(int vol)
{
    int ret;
    /*调整 bt music 音量*/
    ret = msp_app_bt_avrcp_send_passthrouth_cmd(MSP_APP_BT_AVRCP_CMD_VOL_UP);

    /*同时提高本地音音量*/
    smtaudio_ops_node_t *audio_default_ops;

    extern smtaudio_ops_node_t *get_default_audio_ops(void);
    audio_default_ops = get_default_audio_ops();
    if (audio_default_ops) {
        audio_default_ops->vol_up(vol);
    }

    return ret;
}

static int bt_a2dp_vol_down(int vol)
{
    int ret;
    /*调整 bt music 音量*/
    ret = msp_app_bt_avrcp_send_passthrouth_cmd(MSP_APP_BT_AVRCP_CMD_VOL_DOWN);

    /*同时提高本地音音量*/
    smtaudio_ops_node_t *audio_default_ops;

    extern smtaudio_ops_node_t *get_default_audio_ops(void);
    audio_default_ops = get_default_audio_ops();
    if (audio_default_ops) {
        audio_default_ops->vol_down(vol);
    }

    return ret;
}

int8_t smtaudio_register_bt_a2dp(uint8_t min_vol, uint8_t *aef_conf, size_t aef_conf_size, float speed, int resample)
{
    int8_t ret;
    ret = smtaudio_ops_register(&ctrl_bt_a2dp);
    ctrl_bt_a2dp.init();
    return ret;
}
#endif
