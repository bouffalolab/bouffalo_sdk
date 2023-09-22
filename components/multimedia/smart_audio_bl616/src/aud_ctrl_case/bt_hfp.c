/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#if defined(CONFIG_BT_HFP) && (CONFIG_BT_HFP == 1)

#include <stdlib.h>
#include <ulog/ulog.h>
#include <aos/aos.h>
#include "msp_app_bt.h"
#include "smart_audio.h"
#include "aos/bt.h"

// #include <drv/i2s.h>
#include <aos/ringbuffer.h>
#include "speex/speex_resampler.h"
// #include <sys_freq.h>
#include <yoc/mic.h>
// #include <voice.h>
#include <output/ao.h>

#define TAG "smtaudio_ctrl_bt_hfp"

#define I2S_BUF_SIZE       (640 * 20)
#define I2S_PERIOD_SIZE    (640 * 2)

#define NUM_POINTS 1024

typedef struct {
    ao_cls_t *ao;
    int       samplerate;
    int       channels;
    int       format;
    sf_t      ao_sf;
    ao_conf_t ao_cnf;
    uint8_t   first_frame_output;
} snd_t;

// static SpeexResamplerState *s_pcm_srs;
static aos_sem_t            s_pcm_send_sem;
// static uint8_t              s_pcm_send_flag = 0;
// static int                  g_pcm_error;

// static i2s_handle_t  i2s_handle;
// static uint8_t       i2s_recv_buf[I2S_BUF_SIZE] __attribute__((aligned(16)));
// static uint8_t       i2s_send_buf[I2S_BUF_SIZE] __attribute__((aligned(16)));
static aos_sem_t     g_sem_hfp_pcm;
static dev_ringbuf_t hfp_ringbuff;
static aos_sem_t     g_sem_hfp_aec;

static uint8_t                       msp_app_bt_hfp_connect_flag;
static uint8_t                       msp_app_bt_hfp_disconnect_flag;
static MSP_APP_BT_HFP_IMPL_CB_FUNC_T g_hfp_callback;
static msp_app_bt_hfp_call_status_t  g_hfp_call_status = CALL_NONE;

static aos_sem_t msp_app_bt_hfp_disconnect_sem;

static int bt_hfp_init(void);
static int bt_hfp_deinit(void);
static int bt_hfp_start(const char *url, uint64_t seek_time, int resume);
static int bt_hfp_pause(void);
static int bt_hfp_stop(void);
static int bt_hfp_resume(void);
static int bt_hfp_vol_get(void);
static int bt_hfp_vol_set(int vol);
static int bt_hfp_vol_up(int vol);
static int bt_hfp_vol_down(int vol);

smtaudio_ops_node_t ctrl_bt_hfp = {
    .name     = "bt_hfp",
    .prio     = 0,
    .id       = SMTAUDIO_BT_HFP,
    .status   = SMTAUDIO_STATE_STOP,
    .init     = bt_hfp_init,
    .deinit   = bt_hfp_deinit,
    .start    = bt_hfp_start,
    .pause    = bt_hfp_pause,
    .stop     = bt_hfp_stop,
    .resume   = bt_hfp_resume,
    .vol_get  = bt_hfp_vol_get,
    .vol_set  = bt_hfp_vol_set,
    .vol_up   = bt_hfp_vol_up,
    .vol_down = bt_hfp_vol_down,
};

// static void i2s_event_cb_fun(int32_t idx, i2s_event_e event, void *arg)
// {
//     if (event == I2S_EVENT_RECEIVE_COMPLETE || event == I2S_EVENT_RX_BUFFER_FULL) {
//         aos_sem_signal(&g_sem_hfp_pcm);
//     }

//     if (event == I2S_EVENT_RX_BUFFER_FULL) {
//         LOGE(TAG, "%s I2S_EVENT_RX_BUFFER_FULL", __func__);
//     }
// }

/* mic 数据回调(20 ms)，格式为mic1/mic2/ref/null/null从回调中取出一路 mic 数据 */
// static void mic_data_get_cb(void *data, int len)
// {
//     int    ret_len = len / 5;
//     short *mic1;

//     mic1 = ((short *)data);

//     int ret = ringbuffer_write(&hfp_ringbuff, (uint8_t *)mic1, len / 5);

//     if (ret != ret_len) {
//         LOGE(TAG, "%s buffer full %d\r\n", __func__, ret);
//     }

//     aos_sem_signal(&g_sem_hfp_aec);
//     return;
// }

// static snd_t *hfp_snd;
// static unsigned char *audio_eqcfg;
// static unsigned int audio_eqcfg_size;
// static uint32_t audio_resample_rate;
static void   start_pcm_send(void)
{
#if 0
    hfp_snd                     = malloc(sizeof(snd_t));
    hfp_snd->samplerate         = 8000;
    hfp_snd->channels           = 2;
    hfp_snd->format             = 16;
    hfp_snd->first_frame_output = 0;

    ao_conf_t ao_cnf;
    ao_cls_t *ao;
    sf_t      sf = sf_make_channel(hfp_snd->channels) | sf_make_rate(hfp_snd->samplerate) |
              sf_make_bit(hfp_snd->format) | sf_make_signed(1);

    ao_conf_init(&ao_cnf);
    ao_cnf.name          = "alsa";
    ao_cnf.resample_rate = audio_resample_rate;
    //ao_cnf.period_ms     = 15;
    ao_cnf.aef_conf = audio_eqcfg;
    ao_cnf.aef_conf_size = audio_eqcfg_size;
    // ao_cnf.period_num = 7;

    ao_cnf.vol_en    = 1;
    ao_cnf.vol_index = 255;
    hfp_snd->ao_sf   = sf;
    hfp_snd->ao_cnf  = ao_cnf;

    ao = ao_open(sf, &ao_cnf);
    if (ao == NULL) {
        free(hfp_snd);
        return;
    }
    hfp_snd->ao = ao;

    ao_start(hfp_snd->ao);

    csi_i2s_enable(i2s_handle, 1);
    csi_i2s_receive_ctrl(i2s_handle, I2S_STREAM_START);

    voice_t *v = (voice_t *)mic_get_privdata();
    voice_set_pcm_hook(v, mic_data_get_cb);

    s_pcm_send_flag = 0;
    aos_sem_signal(&s_pcm_send_sem);
#endif
}

static void stop_pcm_send(void)
{
#if 0
    s_pcm_send_flag = 1;

    voice_t *v = (voice_t *)mic_get_privdata();
    voice_set_pcm_hook(v, NULL);

    csi_i2s_enable(i2s_handle, 0);

    ringbuffer_clear(&hfp_ringbuff);

    if (hfp_snd != NULL) {
        ao_drain(hfp_snd->ao);
        ao_stop(hfp_snd->ao);
        ao_close(hfp_snd->ao);
        hfp_snd->ao = NULL;
        free(hfp_snd);
    }
#endif
}

static void hfp_pcm_recv_entry(void *arg)
{
#if 0
    uint32_t recv_len;

    uint8_t *pcm_tmp = aos_malloc_check(I2S_PERIOD_SIZE);

    while (1) {
        aos_sem_wait(&g_sem_hfp_pcm, AOS_WAIT_FOREVER);

        recv_len = csi_i2s_receive(i2s_handle, (uint8_t *)pcm_tmp, I2S_PERIOD_SIZE);

        if (hfp_snd->first_frame_output == 0) {
            LOGW(TAG, "enter %s, first frame %d", __func__, recv_len);
            hfp_snd->first_frame_output = -1;
        }

        int ret = ao_write(hfp_snd->ao, pcm_tmp, recv_len);
        if (ret < 0) {
            LOGW(TAG, "APP_PCM_STATE_UNDERRUN");
        }
    }
    aos_free(pcm_tmp);
#endif
}

static void hfp_pcm_send_entry(void *arg)
{
#if 0
    int       read_buffer_len;
    int       buffer_len = NUM_POINTS * sizeof(uint16_t);
    uint16_t *buffer     = aos_malloc_check(buffer_len);

    int       resample_len = NUM_POINTS * sizeof(uint16_t);
    uint16_t *resample     = aos_malloc_check(resample_len);

    while (1) {
        aos_sem_wait(&s_pcm_send_sem, AOS_WAIT_FOREVER);

        while (s_pcm_send_flag == 0) {

            /* wait data */
            aos_sem_wait(&g_sem_hfp_aec, AOS_WAIT_FOREVER);

            read_buffer_len =
                ringbuffer_read(&hfp_ringbuff, (uint8_t *)buffer, NUM_POINTS * sizeof(uint16_t));

            /* resample 16K -> 8K */
            speex_resampler_process_int(s_pcm_srs, 0, (spx_int16_t *)buffer,
                                        (spx_uint32_t *)&read_buffer_len, (spx_int16_t *)resample,
                                        (spx_uint32_t *)&resample_len);

            // send pcm data to bt controler
            csi_i2s_send(i2s_handle, (uint8_t *)resample, resample_len);
        }
    }

    aos_free(buffer);
    aos_free(resample);
#endif
}

const char *c_hf_evt_str[] = {
    "CONNECTION_STATE_EVT",      /*!< connection state changed event */
    "AUDIO_STATE_EVT",           /*!< audio connection state change event */
    "VR_STATE_CHANGE_EVT",       /*!< voice recognition state changed */
    "CALL_IND_EVT",              /*!< call indication event */
    "CALL_SETUP_IND_EVT",        /*!< call setup indication event */
    "CALL_HELD_IND_EVT",         /*!< call held indicator event */
    "NETWORK_STATE_EVT",         /*!< network state change event */
    "SIGNAL_STRENGTH_IND_EVT",   /*!< signal strength indication event */
    "ROAMING_STATUS_IND_EVT",    /*!< roaming status indication event */
    "BATTERY_LEVEL_IND_EVT",     /*!< battery level indication event */
    "CURRENT_OPERATOR_EVT",      /*!< current operator name event */
    "RESP_AND_HOLD_EVT",         /*!< response and hold event */
    "CLIP_EVT",                  /*!< Calling Line Identification notification event */
    "CALL_WAITING_EVT",          /*!< call waiting notification */
    "CLCC_EVT",                  /*!< listing current calls event */
    "VOLUME_CONTROL_EVT",        /*!< audio volume control event */
    "AT_RESPONSE",               /*!< audio volume control event */
    "SUBSCRIBER_INFO_EVT",       /*!< subscriber information event */
    "INBAND_RING_TONE_EVT",      /*!< in-band ring tone settings */
    "LAST_VOICE_TAG_NUMBER_EVT", /*!< requested number from AG event */
    "RING_IND_EVT",              /*!< ring indication event */
};
// msp_hf_client_connection_state_t
const char *c_connection_state_str[] = {
    "disconnected", "connecting", "connected", "slc_connected", "disconnecting",
};
// msp_hf_client_audio_state_t
const char *c_audio_state_str[] = {
    "disconnected",
    "connecting",
    "connected",
    "connected_msbc",
};
/// msp_hf_vr_state_t
const char *c_vr_state_str[] = {
    "disabled",
    "enabled",
};
// msp_hf_service_availability_status_t
const char *c_service_availability_status_str[] = {
    "unavailable",
    "available",
};
// msp_hf_roaming_status_t
const char *c_roaming_status_str[] = {
    "inactive",
    "active",
};
// msp_hf_client_call_state_t
const char *c_call_str[] = {
    "NO call in progress",
    "call in progress",
};
// msp_hf_client_callsetup_t
const char *c_call_setup_str[] = {"NONE", "INCOMING", "OUTGOING_DIALING", "OUTGOING_ALERTING"};
// msp_hf_client_callheld_t
const char *c_call_held_str[] = {
    "NONE held",
    "Held and Active",
    "Held",
};
// msp_hf_response_and_hold_status_t
const char *c_resp_and_hold_str[] = {
    "HELD",
    "HELD ACCEPTED",
    "HELD REJECTED",
};
// msp_hf_client_call_direction_t
const char *c_call_dir_str[] = {
    "outgoing",
    "incoming",
};
// msp_hf_client_call_state_t
const char *c_call_state_str[] = {
    "active", "held", "dialing", "alerting", "incoming", "waiting", "held_by_resp_hold",
};
// msp_hf_current_call_mpty_type_t
const char *c_call_mpty_type_str[] = {
    "single",
    "multi",
};
// msp_hf_volume_control_target_t
const char *c_volume_control_target_str[] = {"SPEAKER", "MICROPHONE"};
// msp_hf_at_response_code_t
const char *c_at_response_code_str[] = {
    "OK",
    "ERROR"
    "ERR_NO_CARRIER",
    "ERR_BUSY",
    "ERR_NO_ANSWER",
    "ERR_DELAYED",
    "ERR_BLACKLILSTED",
    "ERR_CME",
};
// msp_hf_subscriber_service_type_t
const char *c_subscriber_service_type_str[] = {
    "unknown",
    "voice",
    "fax",
};
// msp_hf_client_in_band_ring_state_t
const char *c_inband_ring_state_str[] = {
    "NOT provided",
    "Provided",
};

static bool mac_str_to_bin(const char *str, uint8_t *mac)
{
    int32_t i;
    char *  s, *e;

    if ((mac == NULL) || (str == NULL)) {
        return false;
    }
    s = (char *)str;
    for (i = 5; i >= 0; i--) {
        mac[i] = s ? strtoul(s, &e, 16) : 0;
        if (s) {
            s = (*e) ? e + 1 : e;
        }
    }
    return true;
}

static bool mac_bin_to_str(char *str, uint8_t *mac)
{
    int32_t i;

    if ((mac == NULL) || (str == NULL)) {
        return false;
    }

    for (i = 5; i >= 0; i--) {
        sprintf((char *)(str + 3 * (5 - i)), "%02X:", mac[i]);
    }

    str[17] = '\0';
    return true;
}

static bt_dev_addr_t peer_addr;
/* callback for HF_CLIENT */
static void bt_app_hf_client_cb(bt_prf_hfp_hf_cb_event_t event, bt_prf_hfp_hf_cb_param_t *param)
{
    if (event <= BT_PRF_HFP_HF_RING_IND_EVT) {
        LOGD(TAG, "APP HFP event: %s", c_hf_evt_str[event]);
    } else {
        LOGE(TAG, "APP HFP invalid event %d", event);
    }
    switch (event) {
    case BT_PRF_HFP_HF_CONNECTION_STATE_EVT: {
        LOGI(TAG, "--connection state %s, peer feats 0x%x, chld_feats 0x%x",
             c_connection_state_str[param->conn_stat.state], param->conn_stat.peer_feat,
             param->conn_stat.chld_feat);
        memcpy(peer_addr.val, param->conn_stat.peer_addr.val, BT_BD_ADDR_LEN);
        char bdAddr[MSP_APP_BDADDR_MAX_LEN];

        if (param->conn_stat.state == BT_PRF_HFP_HF_CONNECTION_STATE_CONNECTED ||
            param->conn_stat.state == BT_PRF_HFP_HF_CONNECTION_STATE_SLC_CONNECTED) {
            msp_app_bt_hfp_connect_flag = 1;
        } else {
            msp_app_bt_hfp_connect_flag = 0;
        }

        mac_bin_to_str(bdAddr, param->conn_stat.peer_addr.val);

        if (g_hfp_callback.hfpStateChangedCB)
            g_hfp_callback.hfpStateChangedCB(param->conn_stat.state, bdAddr);

        if (msp_app_bt_hfp_disconnect_flag == 1 &&
            param->conn_stat.state == BT_PRF_HFP_HF_CONNECTION_STATE_DISCONNECTED) {
            aos_sem_signal(&msp_app_bt_hfp_disconnect_sem);
            msp_app_bt_hfp_disconnect_flag = 0;
        }

        break;
    }
    case BT_PRF_HFP_HF_AUDIO_STATE_EVT: {
        LOGI(TAG, "--audio state %s", c_audio_state_str[param->audio_stat.state]);

        if (g_hfp_callback.hfpAudioStateCB)
            g_hfp_callback.hfpAudioStateCB(param->audio_stat.state);

        if (param->audio_stat.state == BT_PRF_HFP_HF_AUDIO_STATE_CONNECTED ||
            param->audio_stat.state == BT_PRF_HFP_HF_AUDIO_STATE_CONNECTED_MSBC) {
            start_pcm_send();
        } else if (param->audio_stat.state == BT_PRF_HFP_HF_AUDIO_STATE_DISCONNECTED) {
            stop_pcm_send();
        }
        break;
    }
    case BT_PRF_HFP_HF_BVRA_EVT: {
        LOGI(TAG, "--VR state %s", c_vr_state_str[param->bvra.value]);
        break;
    }
    case BT_PRF_HFP_HF_CIND_SERVICE_AVAILABILITY_EVT: {
        LOGI(TAG, "--NETWORK STATE %s",
             c_service_availability_status_str[param->service_availability.status]);
        break;
    }
    case BT_PRF_HFP_HF_CIND_ROAMING_STATUS_EVT: {
        LOGI(TAG, "--ROAMING: %s", c_roaming_status_str[param->roaming.status]);
        break;
    }
    case BT_PRF_HFP_HF_CIND_SIGNAL_STRENGTH_EVT: {
        LOGI(TAG, "-- signal strength: %d", param->signal_strength.value);
        break;
    }
    case BT_PRF_HFP_HF_CIND_BATTERY_LEVEL_EVT: {
        LOGI(TAG, "--battery level %d", param->battery_level.value);
        break;
    }
    case BT_PRF_HFP_HF_COPS_CURRENT_OPERATOR_EVT: {
        LOGI(TAG, "--operator name: %s", param->cops.name);
        break;
    }
    case BT_PRF_HFP_HF_CIND_CALL_EVT: {
        LOGI(TAG, "--Call indicator %s", c_call_str[param->call.status]);

        if (param->call.status == BT_PRF_HFP_HF_CALL_STATUS_NO_CALLS) {
            g_hfp_call_status = CALL_NO_PROGRESS;
            if (ctrl_bt_hfp.callback) {
                ctrl_bt_hfp.callback(SMTAUDIO_BT_HFP, SMTAUDIO_PLAYER_EVENT_STOP);
            }
        } else if (param->call.status == BT_PRF_HFP_HF_CALL_STATUS_CALL_IN_PROGRESS) {
            g_hfp_call_status = CALL_IN_PROGRESS;
            if (ctrl_bt_hfp.callback) {
                ctrl_bt_hfp.callback(SMTAUDIO_BT_HFP, SMTAUDIO_PLAYER_EVENT_START);
            }
        }
        break;
    }
    case BT_PRF_HFP_HF_CIND_CALL_SETUP_EVT: {
        LOGI(TAG, "--Call setup indicator %s", c_call_setup_str[param->call_setup.status]);

        if (param->call_setup.status == BT_PRF_HFP_HF_CALL_SETUP_STATUS_INCOMING) {
            g_hfp_call_status = CALL_INCOMING;
            if (ctrl_bt_hfp.callback) {
                ctrl_bt_hfp.callback(SMTAUDIO_BT_HFP, SMTAUDIO_PLAYER_EVENT_START);
            }
        } else if (param->call_setup.status == BT_PRF_HFP_HF_CALL_SETUP_STATUS_OUTGOING_DIALING) {
            g_hfp_call_status = CALL_OUTGOING_DIALING;
            if (ctrl_bt_hfp.callback) {
                ctrl_bt_hfp.callback(SMTAUDIO_BT_HFP, SMTAUDIO_PLAYER_EVENT_START);
            }
        } else if (param->call_setup.status == BT_PRF_HFP_HF_CALL_SETUP_STATUS_NONE) {
            if (g_hfp_call_status != CALL_IN_PROGRESS) {
                g_hfp_call_status = CALL_NONE;
                if (ctrl_bt_hfp.callback) {
                    ctrl_bt_hfp.callback(SMTAUDIO_BT_HFP, SMTAUDIO_PLAYER_EVENT_STOP);
                }
            }
        }
        break;
    }
    case BT_PRF_HFP_HF_CIND_CALL_HELD_EVT: {
        LOGI(TAG, "--Call held indicator %s", c_call_held_str[param->call_held.status]);
        break;
    }
    case BT_PRF_HFP_HF_BTRH_EVT: {
        LOGI(TAG, "--response and hold %s", c_resp_and_hold_str[param->btrh.status]);
        break;
    }
    case BT_PRF_HFP_HF_CLIP_EVT: {
        LOGI(TAG, "--clip number %s", (param->clip.number == NULL) ? "NULL" : (param->clip.number));
        break;
    }
    case BT_PRF_HFP_HF_CCWA_EVT: {
        LOGI(TAG, "--call_waiting %s",
             (param->ccwa.number == NULL) ? "NULL" : (param->ccwa.number));
        break;
    }
    case BT_PRF_HFP_HF_CLCC_EVT: {
        LOGI(TAG, "--Current call: idx %d, dir %s, state %s, mpty %s, number %s", param->clcc.idx,
             c_call_dir_str[param->clcc.dir], c_call_state_str[param->clcc.status],
             c_call_mpty_type_str[param->clcc.mpty],
             (param->clcc.number == NULL) ? "NULL" : (param->clcc.number));
        break;
    }
    case BT_PRF_HFP_HF_VOLUME_CONTROL_EVT: {
        LOGI(TAG, "--volume_target: %s, volume %d",
             c_volume_control_target_str[param->volume_control.type], param->volume_control.volume);
        if (g_hfp_callback.hfpVolumeChangedCB &&
            param->volume_control.type == BT_PRF_HFP_HF_VOLUME_CONTROL_TARGET_SPK)
            g_hfp_callback.hfpVolumeChangedCB(param->volume_control.volume);
        break;
    }

    case BT_PRF_HFP_HF_AT_RESPONSE_EVT: {
        LOGI(TAG, "--AT response event, code %d, cme %d", param->at_response.code,
             param->at_response.cme);
        break;
    }

    case BT_PRF_HFP_HF_CNUM_EVT: {
        LOGI(TAG, "--subscriber type %s, number %s",
             c_subscriber_service_type_str[param->cnum.type],
             (param->cnum.number == NULL) ? "NULL" : param->cnum.number);
        break;
    }
    case BT_PRF_HFP_HF_BSIR_EVT: {
        LOGI(TAG, "--inband ring state %s", c_inband_ring_state_str[param->bsir.state]);
        break;
    }
    case BT_PRF_HFP_HF_BINP_EVT: {
        LOGI(TAG, "--last voice tag number: %s",
             (param->binp.number == NULL) ? "NULL" : param->binp.number);
        break;
    }
    default:
        LOGE(TAG, "HF_CLIENT EVT: %d", event);
        break;
    }
}

int32_t msp_app_bt_hfp_reg_callback(MSP_APP_BT_HFP_IMPL_CB_FUNC_T *callback)
{
    g_hfp_callback.hfpAudioStateCB    = callback->hfpAudioStateCB;
    g_hfp_callback.hfpStateChangedCB  = callback->hfpStateChangedCB;
    g_hfp_callback.hfpRingIndCB       = callback->hfpRingIndCB;
    g_hfp_callback.hfpVolumeChangedCB = callback->hfpVolumeChangedCB;

    return 0;
}

int32_t msp_app_bt_hfp_connect(const char *mac)
{
    bt_dev_addr_t addr;

    mac_str_to_bin(mac, addr.val);

    bt_prf_hfp_hf_connect(&addr);
    return 0;
}

int32_t msp_app_bt_hfp_disconnect(const char *mac)
{
    bt_dev_addr_t addr;

    mac_str_to_bin(mac, addr.val);

    if (msp_app_bt_hfp_connect_flag == 0) {
        LOGE(TAG, "%s no conn", __func__);
        return 0;
    }

    msp_app_bt_hfp_disconnect_flag = 1;

    int ret = bt_prf_hfp_hf_disconnect(&addr);

    if (ret == BT_STACK_STATUS_SUCCESS) {
        aos_sem_wait(&msp_app_bt_hfp_disconnect_sem, -1);
    }
    return 0;
}

int32_t msp_app_bt_hfp_interrupt_cur_conn(void)
{
    if (msp_app_bt_hfp_connect_flag == 0) {
        LOGE(TAG, "%s no conn", __func__);
        return 0;
    }

    msp_app_bt_hfp_disconnect_flag = 1;

    int ret = bt_prf_hfp_hf_disconnect(&peer_addr);

    if (ret == BT_STACK_STATUS_SUCCESS) {
        aos_sem_wait(&msp_app_bt_hfp_disconnect_sem, -1);
    }
    return 0;
}

int32_t msp_app_bt_hfp_connect_audio(const char *mac)
{
    bt_dev_addr_t addr;

    mac_str_to_bin(mac, addr.val);

    bt_prf_hfp_hf_audio_connect(&addr);
    return 0;
}

int32_t msp_app_bt_hfp_disconnect_audio(const char *mac)
{
    bt_dev_addr_t addr;

    mac_str_to_bin(mac, addr.val);

    bt_prf_hfp_hf_audio_disconnect(&addr);
    return 0;
}

int32_t msp_app_bt_hfp_send_command(MSP_APP_BT_HFP_COMMAND_T command)
{
    switch (command) {
        /* Three-way calling */
    case MSP_APP_BT_HFP_COMMAND_TYPE_REJECT_INCOMING:
        bt_prf_hfp_hf_send_chld_cmd(BT_PRF_HFP_HF_CHLD_TYPE_REL, 0);
        break;
    case MSP_APP_BT_HFP_COMMAND_TYPE_TERMINATE_ACTIVE_ACCEPT_INCOMING:
        bt_prf_hfp_hf_send_chld_cmd(BT_PRF_HFP_HF_CHLD_TYPE_REL_ACC, 0);
        break;
    case MSP_APP_BT_HFP_COMMAND_TYPE_HOLD_ACTIVE_ACCEPT_INCOMING:
        bt_prf_hfp_hf_send_chld_cmd(BT_PRF_HFP_HF_CHLD_TYPE_HOLD_ACC, 0);
        break;
        /* Two-way calling */
    case MSP_APP_BT_HFP_COMMAND_TYPE_ANSWER:
        if (g_hfp_call_status != CALL_IN_PROGRESS) {
            bt_prf_hfp_hf_answer_call();
        }
        break;
    case MSP_APP_BT_HFP_COMMAND_TYPE_TERMINATE:
        bt_prf_hfp_hf_reject_call();
        break;
    default:
        break;
    }

    return 0;
}

int32_t msp_app_bt_hfp_dial(char *number)
{
    bt_prf_hfp_hf_dial(number);

    return 0;
}

int32_t msp_app_bt_hfp_get_call_status(void)
{
    return g_hfp_call_status;
}

int32_t msp_app_bt_hfp_volume_update(int type, int volume)
{
    bt_prf_hfp_hf_vol_update(type, volume);

    return 0;
}

static int32_t msp_app_bt_hfp_init(void)
{
    aos_sem_new(&msp_app_bt_hfp_disconnect_sem, 0);

    bt_prf_hfp_hf_register_callback(bt_app_hf_client_cb);
    bt_prf_hfp_hf_init();

    return 0;
}

static int bt_hfp_init(void)
{
#if 0
    drv_clk_enable(I2S0_CLK);

    // enable audio PLL and IP under this PLL
    *((volatile uint32_t *)0x30000010) = 0x024013b9;

    i2s_handle = csi_i2s_initialize(0, i2s_event_cb_fun, NULL);
    if (!i2s_handle) {
        LOGE(TAG, "i2s init error\n");
        return -1;
    }

    int32_t      ret;
    i2s_config_t i2s_config;
    memset(&i2s_config, 0, sizeof(i2s_config));

    i2s_config.cfg.left_polarity  = I2S_LEFT_POLARITY_LOW;
    i2s_config.cfg.mclk_freq      = I2S_MCLK_256FS;
    i2s_config.cfg.mode           = I2S_MODE_FULL_DUPLEX_MASTER;
    i2s_config.cfg.protocol       = I2S_PROTOCOL_PCM;
    i2s_config.cfg.sclk_freq      = I2S_SCLK_32FS;
    i2s_config.cfg.rx_mono_enable = 0;
    i2s_config.cfg.tx_mono_enable = 1;
    i2s_config.cfg.width          = I2S_SAMPLE_16BIT;
    i2s_config.rate               = 8000;
    i2s_config.rx_buf             = (uint8_t *)i2s_recv_buf;
    i2s_config.rx_buf_length      = I2S_BUF_SIZE;
    i2s_config.rx_period          = I2S_PERIOD_SIZE;
    i2s_config.tx_buf             = (uint8_t *)i2s_send_buf;
    i2s_config.tx_buf_length      = I2S_BUF_SIZE;
    i2s_config.tx_period          = I2S_PERIOD_SIZE;

    ret = csi_i2s_config(i2s_handle, &i2s_config);
    if (ret != 0) {
        LOGE(TAG, "i2s config error %d\n", ret);
        return -1;
    }

    s_pcm_srs = speex_resampler_init((spx_uint32_t)1, (spx_uint32_t)16000, (spx_uint32_t)8000, 0,
                                     &g_pcm_error);
#endif

    aos_sem_new(&g_sem_hfp_aec, 0);
    aos_sem_new(&g_sem_hfp_pcm, 0);
    aos_sem_new(&s_pcm_send_sem, 0);

    aos_task_t recv_task_handle;
    aos_task_t send_task_handle;

    aos_task_new_ext(&recv_task_handle, "hfp_recv", hfp_pcm_recv_entry, NULL, 4096, 28);
    aos_task_new_ext(&send_task_handle, "hfp_send", hfp_pcm_send_entry, NULL, 4096, 29);

    ringbuffer_create(&hfp_ringbuff, aos_malloc_check(1024 * 20 + 1), 1024 * 20 + 1);

    msp_app_bt_hfp_init();

    return 0;
}

static int bt_hfp_deinit(void)
{
    return 0;
}

static int bt_hfp_start(const char *url, uint64_t seek_time, int resume)
{
    return msp_app_bt_hfp_send_command(MSP_APP_BT_HFP_COMMAND_TYPE_ANSWER);
}

static int bt_hfp_pause(void)
{
    ctrl_bt_hfp.status = SMTAUDIO_STATE_PAUSE;
    return msp_app_bt_hfp_send_command(MSP_APP_BT_HFP_COMMAND_TYPE_HOLD_ACTIVE_ACCEPT_INCOMING);
}

static int bt_hfp_stop(void)
{
    ctrl_bt_hfp.status = SMTAUDIO_STATE_STOP;
    return msp_app_bt_hfp_send_command(MSP_APP_BT_HFP_COMMAND_TYPE_TERMINATE);
}

static int bt_hfp_resume(void)
{
    printf("%s invalid\n", __func__);
    return 0;
}

static int bt_hfp_vol_get(void)
{
    printf("%s invalid\n", __func__);
    return 0;
}

static int bt_hfp_vol_set(int vol)
{
    msp_app_bt_hfp_volume_update(BT_PRF_HFP_HF_VOLUME_CONTROL_TARGET_SPK, vol * 15 / 100);
    return 0;
}

static int bt_hfp_vol_up(int vol)
{
    int ret = -1;
    /*调整 bt call 音量*/

    /*同时提高本地音音量*/
    smtaudio_ops_node_t *audio_default_ops;

    extern smtaudio_ops_node_t *get_default_audio_ops(void);
    audio_default_ops = get_default_audio_ops();
    if (audio_default_ops) {
        audio_default_ops->vol_up(vol);
    }

    return ret;
}

static int bt_hfp_vol_down(int vol)
{
    int ret = -1;
    /*调整 bt call 音量*/

    /*同时提高本地音音量*/
    smtaudio_ops_node_t *audio_default_ops;

    extern smtaudio_ops_node_t *get_default_audio_ops(void);
    audio_default_ops = get_default_audio_ops();
    if (audio_default_ops) {
        audio_default_ops->vol_down(vol);
    }

    return ret;
}

int8_t smtaudio_register_bt_hfp(uint8_t min_vol, uint8_t *aef_conf, size_t aef_conf_size, float speed, int resample)
{
    int8_t ret;
    ret = smtaudio_ops_register(&ctrl_bt_hfp);
    ctrl_bt_hfp.init();

    // if(aef_conf) {
    //     audio_eqcfg = aef_conf;
    //     audio_eqcfg_size = aef_conf_size;
    //     audio_resample_rate = resample;
    // }

    return ret;
}
#endif
