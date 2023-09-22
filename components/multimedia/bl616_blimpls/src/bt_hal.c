/**
 * Copyright (C) 2022 Bouffalolab, All rights reserved.
 *
 * @file:    tg_bredr.c
 * @brief:   vendor bluetooth interface implementation
 *
 */

#if 1
#include <stdio.h>
// #include <ulog/ulog.h>

#include "btble_lib_api.h"
#include "bluetooth.h"
#include "hci_driver.h"

#include "conn.h"
#include "hci_core.h"
#include "conn_internal.h"
#include "l2cap.h"
#include "a2dp.h"
#include "avrcp.h"
#include "rfcomm.h"
#include "hfp_hf.h"

#include "bt.h"
#include <classic/bt_stack_hfp_hf.h>
#include <board.h>
#include "keys.h"
// #include <aos/yloop.h>
#include "smart_audio.h"
// #include <wifi_mgmr_ext.h>
#define BT_HAL_DBG(fmt,...)  //printf(fmt", %s\r\n", ##__VA_ARGS__,__func__)//LOGD("BTHAL", __VA_ARGS__)
#define BT_RECONNECT_RETRY  10
/* 音量曲线，数组中的值是对应数字音量处理的db值的id号，对应db值可以使用 smta volmap命令进行查询*/
static uint8_t g_vol_map[101] = {
    1,   28,  30,  32,  34,  36,  38,  40,  42,  44,  46,  48,  50,  52,  54,  56,  58,  60,  62,  64,  66,
    68,  70,  72,  74,  76,  78,  80,  82,  84,  86,  88,  90,  92,  94,  96,  98,  100, 102, 104, 106, 108,
    110, 112, 114, 116, 118, 120, 122, 124, 126, 128, 130, 132, 134, 136, 138, 140, 142, 144, 146, 148, 150,
    152, 154, 156, 158, 160, 162, 164, 166, 168, 170, 172, 174, 176, 178, 180, 182, 184, 186, 188, 190, 192,
    194, 196, 198, 200, 202, 204, 206, 208, 210, 212, 214, 216, 218, 220, 222, 224, 226,
};

/* db_min和db_max 用来建立256个db值对应的计算因子 */
audio_vol_config_t g_vol_config = { -45, 6, g_vol_map };

audio_vol_config_t *board_audio_out_get_vol_config()
{
    return &g_vol_config;
}

static uint8_t bt_init_cb = 0;
static struct bt_conn_info t_bt_conn_info;
static struct bt_conn *t_bt_def_conn = NULL;
static void bt_hal_link_loss_evt(void *args);
static void tg_bt_a2dp_connect_remote(bt_addr_t *addr);
static void bt_hal_link_loss_cb(bt_addr_t *addr);

struct bt_link_loss_t {
    bt_addr_t dst;
    union {
        struct {
            uint32_t retry;
        } bt_reconn_retry;
        struct {
            uint32_t timeout;
            struct k_delayed_work delay_work;
        } bt_reconn_time;
    };
    void (*reconnect)(bt_addr_t *addr);
    void (*complete)(void);
    void (*cb)(bt_addr_t *addr);
    struct k_delayed_work d_work;
} __packed;

struct bt_link_loss_t *bt_link_loss = NULL;

static void bt_hal_link_loss_cb(bt_addr_t *addr)
{
    if (bt_link_loss->bt_reconn_retry.retry > 0) {
        bt_link_loss->bt_reconn_retry.retry--;
        k_delayed_work_submit(&bt_link_loss->d_work,K_MSEC(100));
    } else {
        if (bt_link_loss->complete)
            bt_link_loss->complete();
    }
}

static void bt_link_loss_complete(bt_addr_t *addr)
{
    BT_HAL_DBG("%s\n", __func__);
    //extern int tg_wifi_ps_mode_exit(void);
    //tg_wifi_ps_mode_exit();
}

static void t_bt_connected(struct bt_conn *conn, u8_t err);
static void t_bt_disconnected(struct bt_conn *conn, u8_t reason);

bt_prf_a2dp_cb_t t_a2dp_cb = NULL;
bt_prf_a2dp_sink_data_cb_t t_a2dp_sink_data = NULL;


#ifdef CONFIG_AV_AO_ALSA
#include <output/ao.h>
extern void smtaudio_btvol_set_attach_ao(ao_cls_t *ao, uint8_t *vol_map);
extern int8_t smtaudio_vol_get(void);
static ao_cls_t* ao;
static int pcm_write(uint8_t *data, uint32_t size)
{
    //printf("ao_write\r\n");
    return ao_write(ao, data, size);
}

static int pcm_start(void)
{
    ao_start(ao);
    return 0;
}

static int pcm_stop(void)
{
    return ao_stop(ao);
}

static int pcm_open(int sample,int channel)
{
    ao_conf_t ao_cnf;
    sf_t sf;

    sf = sf_make_channel(channel) | sf_make_rate(sample)
         | sf_make_bit(16) | sf_make_signed(1);

    ao_conf_init(&ao_cnf);
    //ao_register_alsa();

    audio_vol_config_t *vol_cfg = board_audio_out_get_vol_config();

    ao_cnf.resample_rate = 48000;
    ao_cnf.vol_en = 1;
    ao_cnf.vol_index = 208;
    ao_cnf.atempo_play_en = 1;

    if (vol_cfg && vol_cfg->map) {
        ao_cnf.vol_index = vol_cfg->map[smtaudio_vol_get()];
    }

    BT_HAL_DBG("%s, ao_conf rate=%d, volindex=%d\n", __func__, ao_cnf.resample_rate, ao_cnf.vol_index);

    ao = ao_open(sf, &ao_cnf);

    if (ao) {
        smtaudio_btvol_set_attach_ao(ao, vol_cfg->map);
    }

    return ao==NULL ? -1:0;
}

static int pcm_close(void)
{
    smtaudio_btvol_set_attach_ao(NULL, NULL);

    int ret = ao_close(ao);
    ao = NULL;

    return ret;
}

static int pcm_ioctl(int cmd , void *arg)
{   
    float speed = *(float *)arg;
    size_t size = sizeof(speed);

    BT_HAL_DBG("speed set %f\r\n", speed);
    
    return ao_control(ao, cmd, &speed, &size);
}

A2DP_PCM_PRCOESS pcm_callback = {
    .write = pcm_write,
    .start = pcm_start,
    .stop = pcm_stop,
    .open = pcm_open,
    .ioctl = pcm_ioctl,
    .close = pcm_close,
};
#endif

static struct bt_conn_cb t_bt_conn_callbacks = {
    .connected = t_bt_connected,
    .disconnected = t_bt_disconnected,
};

static void t_a2dp_chain(struct bt_conn *conn, u8_t state);
static void t_a2dp_stream(u8_t state);
static struct a2dp_callback t_a2dp_callbacks =
{
    .chain = t_a2dp_chain,
    .stream = t_a2dp_stream,
};

static void t_avrcp_chain(struct bt_conn *conn, u8_t state);
static void t_avrcp_absvol(u8_t vol);
static void t_avrcp_play_status(u32_t song_len, u32_t song_pos, u8_t status);
static void t_avrcp_tg_ntf_evt_status(u8_t evt, u8_t *para,u16_t para_len);

static struct avrcp_callback t_avrcp_callbacks =
{
    .chain = t_avrcp_chain,
    .abs_vol = t_avrcp_absvol,
    .play_status = t_avrcp_play_status,
    .tg_register_notification_event = t_avrcp_tg_ntf_evt_status,
};

bt_dev_addr_t t_remote_addr;
char t_remote_name[20] = {0};
bt_stack_event_cb_t t_bt_event_cb = NULL;


static void tg_bt_a2dp_connect_remote(bt_addr_t *addr)
{
    struct bt_a2dp *a2dp = NULL;
    struct bt_conn *conn = bt_conn_lookup_addr_br(addr);

    BT_HAL_DBG("%s, conn:%p\n", __func__, conn);
    if (conn && conn->state == BT_CONN_CONNECTED) {
        a2dp = bt_a2dp_connect(conn);
        if(a2dp) {
            BT_HAL_DBG("creating a2dp connect. \n");
        } else {
            BT_HAL_DBG("create a2dp connect fail. \n");
        }
    }
}

static void bt_hal_link_loss_evt(void *args)
{
    int8_t *addr = (int8_t *)args;
    bt_prf_a2dp_cb_param_t link_loss_a2dp_cb_param;

    BT_HAL_DBG("%s\n", __func__);
    if (t_a2dp_cb) {
        link_loss_a2dp_cb_param.conn_stat.state = BT_PRF_A2DP_CONNECTION_STATE_DISCONNECTED_ABNORMAL;
        memcpy(link_loss_a2dp_cb_param.conn_stat.peer_addr.val,addr,6);
        t_a2dp_cb(BT_PRF_A2DP_CONNECTION_STATE_EVT, &link_loss_a2dp_cb_param);
    }
}

static void bt_hal_link_loss_trigger(struct bt_conn *conn)
{
    static struct bt_link_loss_t link_loss_ops = {
        .bt_reconn_retry = {
            .retry = BT_RECONNECT_RETRY,
         },
        .cb = bt_hal_link_loss_cb,
        .reconnect = tg_bt_a2dp_connect_remote,
        .complete = bt_link_loss_complete,
    };

    memcpy(&bt_link_loss->dst,&conn->br.dst,sizeof(bt_addr_t));
    bt_link_loss = &link_loss_ops;
    k_delayed_work_init(&bt_link_loss->d_work,bt_hal_link_loss_evt);
    bt_link_loss->cb(&conn->br.dst);
}

static void t_remote_name_callback(const char *name)
{
    if (name) {
        if(strlen(name) + 1 <= sizeof(t_remote_name)){
            memcpy(t_remote_name, name, strlen(name) + 1);
        }
        BT_HAL_DBG("%s, remote name len: %d,  : %s\n", __func__, strlen(name), name);
    } else {
        BT_HAL_DBG("%s, remote name request fail\n", __func__);
    }

    if (t_bt_event_cb)
    {
        BT_HAL_DBG("%s, BT_STACK_AUTH_CMPL_EVT, call back.\n", __func__);
 
        bt_stack_cb_param_t auth_evt;
        auth_evt.auth_cmpl.status = BT_STACK_STATUS_SUCCESS;
        memcpy(auth_evt.auth_cmpl.bda.val, t_remote_addr.val, BT_STACK_BD_ADDR_LEN);
        memcpy(auth_evt.auth_cmpl.device_name, t_remote_name, sizeof(t_remote_name));
        t_bt_event_cb(BT_STACK_AUTH_CMPL_EVT, &auth_evt);
    }

    if (bt_link_loss && 
        bt_link_loss->reconnect) {
        bt_link_loss->reconnect(&bt_link_loss->dst);
    }
}

static void t_bt_connected(struct bt_conn *conn, u8_t err)
{
    if(conn->type != BT_CONN_TYPE_BR)
    {
        return;
    }

    if (err) {
        BT_HAL_DBG("%s, failed to connection (%u) \n", __func__, err);
        /*
         *The Page Timeout error code indicates that a page timed out because of the 
         *Page Timeout configuration parameter. This error code may occur only with the 
         *HCI_Remote_Name_Request and HCI_Create_Connection commands.
        */
        if (err == 0x4 || err == 0xb) {
            if (bt_link_loss) {
                bt_link_loss->cb(&bt_link_loss->dst);
            }
        }
        return;
    }

    char addr[BT_ADDR_STR_LEN];

    bt_conn_get_info(conn, &t_bt_conn_info);
    bt_addr_to_str(t_bt_conn_info.br.dst, addr, sizeof(addr));

    BT_HAL_DBG("%s, connected: %s \n", __func__, addr);

    if (!t_bt_def_conn)
    {
        t_bt_def_conn = conn;
    }

    memcpy(t_remote_addr.val, t_bt_conn_info.br.dst->val, BT_STACK_BD_ADDR_LEN);

    int ret = remote_name_req(t_bt_conn_info.br.dst, t_remote_name_callback);
    if (!ret) {
        BT_HAL_DBG("remote name request pending.\n");
    } else {
        BT_HAL_DBG("remote name request fail.\n");
    }
    //extern int tg_wifi_ps_mode_entry(uint32_t delay_ms, uint32_t duty);
    //tg_wifi_ps_mode_entry(1000, 50);
}

static void t_bt_disconnected(struct bt_conn *conn, u8_t reason)
{
    char addr[BT_ADDR_STR_LEN];

    if(conn->type != BT_CONN_TYPE_BR) {
        return;
    }

    bt_conn_get_info(conn, &t_bt_conn_info);
    bt_addr_to_str(t_bt_conn_info.br.dst, addr, sizeof(addr));

    BT_HAL_DBG("%s, disconnected: %s (reason %u) \n", __func__, addr, reason);
    if (t_bt_def_conn == conn) {
        t_bt_def_conn = NULL;
    }

    //connection timeout
    if (reason == 0x08) {
        BT_HAL_DBG("%s, call back, a2dpStateCB\n", __func__);
        if (!bt_link_loss) {
            bt_hal_link_loss_trigger(conn);
        } else {
            bt_link_loss->bt_reconn_retry.retry = BT_RECONNECT_RETRY;
            bt_link_loss->cb(&bt_link_loss->dst);
        }
    } else {
        if (!bt_link_loss) {
            ;
            //extern int tg_wifi_ps_mode_exit(void);
            //tg_wifi_ps_mode_exit();
        }
    }
}

#define T_BTSTACK_START

/**
 * @brief      register callback function.
 * @param[in]  callback         callback function
 */
void bt_stack_register_callback(bt_stack_cb_t *callback)
{
    BT_HAL_DBG("%s\n", __func__);
    t_bt_event_cb = callback->callback;
}

static void t_bt_enable_cb(int err)
{
    BT_HAL_DBG("%s, err: %d\n", __func__, err);

    if (!err) {

        bt_conn_cb_register(&t_bt_conn_callbacks);
        a2dp_cb_register(&t_a2dp_callbacks);
        avrcp_cb_register(&t_avrcp_callbacks);
#ifdef CONFIG_AV_AO_ALSA
        register_pcm(&pcm_callback);
#endif
        bt_init_cb = 1;
    }
}

/**
 * @brief      Initialize the legacy bluetooth stack
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_init(void)
{
    BT_HAL_DBG("%s\n", __func__);

    btble_controller_init(configMAX_PRIORITIES - 1);
    hci_driver_init();
    bt_enable(t_bt_enable_cb);

    while(!bt_init_cb)
    {
        msp_msleep(1);
    }

    return BT_STACK_STATUS_SUCCESS;
}

/**
 * @brief      Set connectability mode for legacy bluetooth.
 * @param[in]  enable             enable/disable mode
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_set_connectable(int enable)
{
    BT_HAL_DBG("%s, enable: %d \n", __func__, enable);

    int err = -1;

    if (enable)
    {
        err = bt_br_set_connectable(true);
    } else {
        err = bt_br_set_connectable(false);
    }

    if (!err)
    {
        BT_HAL_DBG("bredr set connectable success. \n");
        err = BT_STACK_STATUS_SUCCESS;
    } else {
        BT_HAL_DBG("bredr set connectable fail, err: %d. \n", err);
        err =  BT_STACK_STATUS_EIO;
    }

    return err;
}

/**
 * @brief      Set discoverability mode for legacy bluetooth.
 * @param[in]  enable             enable/disable mode
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_set_discoverable(int enable)
{
    BT_HAL_DBG("%s, enable: %d \n", __func__, enable);

    int err = -1;

    if (enable)
    {
        err = bt_br_set_discoverable(true);
    } else {
        err = bt_br_set_discoverable(false);
    }

    if (!err)
    {
        BT_HAL_DBG("bredr set discoverable success. \n");
        err = BT_STACK_STATUS_SUCCESS;
    } else {
        BT_HAL_DBG("bredr set discoverable fail, err: %d. \n", err);
        err =  BT_STACK_STATUS_EIO;
    }

    return err;
}

/**
 * @brief      name for legacy bluetooth.
 * @param[in]  name               name of the device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_set_name(const char *name)
{
    BT_HAL_DBG("%s, set name: %s \n", __func__, name);

    int err = -1;

    err = bt_br_write_local_name((char *)name);
    if (!err) {
        BT_HAL_DBG("bredr write name done.\n");
        err = BT_STACK_STATUS_SUCCESS;
    } else {
        BT_HAL_DBG("bredr write name failed, err: %d.\n", err);
    }

    return err;
}

/**
 * @brief      Set device Extended Inquiry Response data
 * @param[in]  eir                Extended Inquiry Response data
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_set_eir(uint8_t fec_required, bt_eir_data_t *eir_data, size_t eir_num)
{
    //BT_HAL_DBG("%s, fec_required: %d, include_txpower: %d, include_uuid: %d, flag: %d, manufacturer_len: %d, url_len: %d \n",
    //    __func__, eir->fec_required, eir->include_txpower, eir->include_uuid, eir->flag, eir->manufacturer_len, eir->url_len);
    int i;
    int err = -1;

    uint8_t data[240] = {0};
    uint8_t index = 0;

    for (i = 0; i < eir_num; i++)
    {
        data[index++] = eir_data[i].len + 1;
        data[index++] = eir_data[i].type;
        memcpy(data + index, eir_data[i].data, eir_data[i].len);
        index += eir_data[i].len;
    }
    
    err = bt_br_write_eir(fec_required, data);
    if (!err) {
        BT_HAL_DBG("bredr set eir done.\n");
        err = BT_STACK_STATUS_SUCCESS;
    } else {
        BT_HAL_DBG("bredr set eir failed, err: %d.\n", err);
    }

    return err;
}

/**
 * @brief      Set device class of device
 * @param[in]  cod                device class of device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_set_cod(bt_stack_cod_t cod)
{
    BT_HAL_DBG("%s\n", __func__);

    //Non-essential function
    
    return BT_STACK_STATUS_SUCCESS;
}

/**
 * @brief      Get device class of device
 * @param[in]  cod                device class of device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_get_cod(bt_stack_cod_t *cod)
{
    BT_HAL_DBG("%s\n", __func__);

    //loudspeaker
    uint8_t dev_cod[4] = {0x14, 0x04, 0x24, 0x00};

    memcpy(cod, &dev_cod, sizeof(bt_stack_cod_t));

    return BT_STACK_STATUS_SUCCESS;
}

/**
 * @brief      Set IO Capability
 * @param[in]  io_cap             IO Capability
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_set_io_capability(bt_stack_io_cap_t io_cap)
{
    BT_HAL_DBG("%s\n", __func__);

    //Non-essential function

    return BT_STACK_STATUS_SUCCESS;
}

/**
 * @brief      Removes a device from the security database list of peer device.
 * @param[in]  bd_addr            peer device bluetooth device address, NULL for remove all peer device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_dev_unpair(bt_dev_addr_t  *peer_addr)
{
    BT_HAL_DBG("%s\n", __func__);

    int err = -1;
    bt_addr_le_t addr = { 0 };

    if (!t_bt_def_conn){
        BT_HAL_DBG("Not connected.\n");
        return BT_STACK_STATUS_SUCCESS;
    }

    err = bt_conn_disconnect(t_bt_def_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
    if (!err) {
        BT_HAL_DBG("unpair successfully.\n");
        err = BT_STACK_STATUS_SUCCESS;
    } else {
        BT_HAL_DBG("unpair failed, err: %d.\n", err);
        err =  BT_STACK_STATUS_EIO;
    }

    addr.type = peer_addr->type;
    
    memcpy(addr.a.val,peer_addr->val,BT_STACK_BD_ADDR_LEN);

    return bt_unpair(0,&addr);
}

/**
 * @brief      call func for each paired devices from the security database
 * @param[in]  func              callback func
 * @param[in]  data              func priv data
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_paired_dev_foreach(void (*func)(bt_dev_addr_t *addr, void *data), void *data)
{
    BT_HAL_DBG("%s\n", __func__);

    if (func)
    {
        func(&t_remote_addr, NULL);
    }

    return BT_STACK_STATUS_SUCCESS;
}

#define T_BTSTACK_END

#define T_A2DP_START


static void t_a2dp_chain(struct bt_conn *conn, u8_t state)
{
    BT_HAL_DBG("%s, a2dp chain, conn: %p, state: %d \n", __func__, conn, state);

    if (t_a2dp_cb)
    {
        bt_prf_a2dp_cb_param_t a2dp_cb_param;
        memcpy(a2dp_cb_param.conn_stat.peer_addr.val, t_remote_addr.val, BT_STACK_BD_ADDR_LEN);
        if (state == BT_A2DP_CHAIN_CONNECTED) {
            a2dp_cb_param.conn_stat.state = BT_PRF_A2DP_CONNECTION_STATE_CONNECTED;
        } else if (state == BT_A2DP_CHAIN_DISCONNECTED) {
            a2dp_cb_param.conn_stat.state = BT_PRF_A2DP_CONNECTION_STATE_DISCONNECTED_NORMAL;
        }
        t_a2dp_cb(BT_PRF_A2DP_CONNECTION_STATE_EVT, &a2dp_cb_param);
    }

    if (state == BT_A2DP_CHAIN_CONNECTED) {
        if (bt_link_loss) {
            bt_link_loss = NULL;
        }

        bt_stack_set_connectable(false);
        bt_stack_set_discoverable(false);
    }

}

static void t_a2dp_stream(u8_t state)
{
    BT_HAL_DBG("%s, a2dp stream, state: %d \n", __func__, state);

    if (t_a2dp_cb)
    {
        bt_prf_a2dp_cb_param_t a2dp_cb_param;
        memset(&a2dp_cb_param,0,sizeof(bt_prf_a2dp_cb_param_t));
        memcpy(a2dp_cb_param.audio_stat.peer_addr.val, t_remote_addr.val, BT_STACK_BD_ADDR_LEN);
        if (state == BT_A2DP_STREAM_START) {
            a2dp_cb_param.audio_stat.state = BT_PRF_A2DP_AUDIO_STATE_STARTED;
            //printf("WiFi BT coex, WiFi duty is 30\r\n"); 
            //tg_wifi_ps_mode_entry(0, 15);
            //wifi_mgmr_set_wifi_active_time(15);
        } else if (state == BT_A2DP_STREAM_SUSPEND) {
            a2dp_cb_param.audio_stat.state = BT_PRF_A2DP_AUDIO_STATE_REMOTE_SUSPEND;
            //printf("WiFi BT coex, WiFi duty is 50, BT state is SUSPEND\r\n"); 
            //tg_wifi_ps_mode_entry(0, 50);
            //wifi_mgmr_set_wifi_active_time(50);
        }else{
            a2dp_cb_param.audio_stat.state = BT_PRF_A2DP_AUDIO_STATE_STOPPED;
            //printf("WiFi BT coex, WiFi duty is 50, BT state is STOP\r\n"); 
            //tg_wifi_ps_mode_entry(0, 50);
            //wifi_mgmr_set_wifi_active_time(50);
        }
        t_a2dp_cb(BT_PRF_A2DP_AUDIO_STATE_EVT, &a2dp_cb_param);
    }

    if (bt_link_loss) {
        bt_link_loss = NULL;
    }
}


/**
 * @brief      register event callback function. 
 * @param[in]  callback         callback function
 */
void bt_prf_a2dp_register_callback(bt_prf_a2dp_cb_t callback)
{
    BT_HAL_DBG("%s\n", __func__);
    t_a2dp_cb = callback;
}

/**
 * @brief      register data callback function. 
 * @param[in]  callback         callback function
 */
void bt_prf_a2dp_sink_register_data_callback(bt_prf_a2dp_sink_data_cb_t callback)
{
    BT_HAL_DBG("%s\n", __func__);
    t_a2dp_sink_data = callback;
}

/**
 * @brief      Initialize the bluetooth A2DP sink module.
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_a2dp_sink_init(void)
{
    BT_HAL_DBG("%s\n", __func__);

    return BT_STACK_STATUS_SUCCESS;
}

/**
 * @brief      Connect to remote bluetooth A2DP source.
 * @param[in]  peer_addr         bluetooth device address of peer device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_a2dp_sink_connect(bt_dev_addr_t *peer_addr)
{
    BT_HAL_DBG("%s\n", __func__);
    int ret = BT_STACK_STATUS_SUCCESS;
    struct bt_a2dp *a2dp = NULL;
    struct bt_conn *conn = NULL;
    struct bt_br_conn_param param = {0};
    bt_addr_t set_addr = {0};

    if(!peer_addr){
        return BT_STACK_STATUS_EINVAL;
    }

    if(!t_bt_def_conn){
        memcpy(set_addr.val,peer_addr->val,sizeof(peer_addr->val));
        param.allow_role_switch = true;
        conn = bt_conn_create_br(&set_addr, &param);
        if (conn) {
            BT_HAL_DBG("Connect bredr ACL success.\n");
        } else {
            BT_HAL_DBG("Connect bredr ACL fail.\n");
            if (bt_link_loss) {
                conn = bt_conn_lookup_addr_br(&set_addr);
                if (conn) {
                    memset(conn->br.dst.val,0x0,6);
                    atomic_clear(&conn->ref);
                }
                bt_link_loss->cb(&bt_link_loss->dst);
            }
        }
    }else{
        a2dp = bt_a2dp_connect(t_bt_def_conn);
        if(a2dp) {
            BT_HAL_DBG("creating a2dp sink connect. \n");
            ret = BT_STACK_STATUS_SUCCESS;
        } else {
            BT_HAL_DBG("create a2dp sink connect fail. \n");
            ret = BT_STACK_STATUS_EIO;
        }
    }
    return ret;
}

/**
 * @brief      Disconnect to remote bluetooth A2DP source.
 * @param[in]  peer_addr         bluetooth device address of peer device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_a2dp_sink_disconnect(bt_dev_addr_t *peer_addr)
{
    BT_HAL_DBG("%s\n", __func__);
    int err;

    if (!t_bt_def_conn){
        BT_HAL_DBG("Not connected.\n");
        return BT_STACK_STATUS_SUCCESS;
    }

    err = bt_conn_disconnect(t_bt_def_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
    if (err) {
        BT_HAL_DBG("disconnect a2dp sink failed.\n");
        err = BT_STACK_STATUS_EIO;
    } else {
        BT_HAL_DBG("disconnect a2dp sink successfully.\n");
        err = BT_STACK_STATUS_SUCCESS;
    }

    return err;
}

#define T_A2DP_END


#define T_AVRCP_START

bt_prf_avrcp_ct_cb_t t_avrcp_ct_cb = NULL;

static void t_avrcp_chain(struct bt_conn *conn, u8_t state)
{
    BT_HAL_DBG("%s, avrcp chain, conn: %p, state: %d \n", __func__, conn, state);

    if (t_avrcp_ct_cb)
    {
        bt_prf_avrcp_ct_cb_param_t avrcp_ct_cb_param;
        memcpy(avrcp_ct_cb_param.conn_stat.peer_addr.val, t_remote_addr.val, BT_STACK_BD_ADDR_LEN);
        if (state == BT_AVRCP_CHAIN_CONNECTED)
        {
            avrcp_ct_cb_param.conn_stat.connected = true;
        }
        else if (state == BT_AVRCP_CHAIN_DISCONNECTED)
        {
            avrcp_ct_cb_param.conn_stat.connected = false;
        }
        t_avrcp_ct_cb(BT_PRF_AVRCP_CT_CONNECTION_STATE_EVT, &avrcp_ct_cb_param);
    }

}

static void t_avrcp_absvol(u8_t vol)
{
    BT_HAL_DBG("%s, avrcp abs vol: %d \n", __func__, vol);

    if (t_avrcp_ct_cb)
    {
        bt_prf_avrcp_ct_cb_param_t avrcp_ct_cb_param;
        avrcp_ct_cb_param.set_volume_rsp.volume = vol;
        t_avrcp_ct_cb(BT_PRF_AVRCP_CT_SET_ABSOLUTE_VOLUME_RSP_EVT, &avrcp_ct_cb_param);
    }

}

static void t_avrcp_play_status(u32_t song_len, u32_t song_pos, u8_t status)
{
    BT_HAL_DBG("%s, avrcp get play status, song length: %u, song position: %u, status: %d \n",
    		__func__, song_len, song_pos, status);

    if (t_avrcp_ct_cb)
    {
        bt_prf_avrcp_ct_cb_param_t avrcp_ct_cb_param;
        avrcp_ct_cb_param.get_rn_play_status_rsp.song_len = song_len;
        avrcp_ct_cb_param.get_rn_play_status_rsp.song_pos = song_pos;
        avrcp_ct_cb_param.get_rn_play_status_rsp.play_status = status;
        t_avrcp_ct_cb(BT_PRF_AVRCP_CT_PLAY_STATUS_RSP_EVT, &avrcp_ct_cb_param);
    }
}

static void t_avrcp_tg_ntf_evt_status(u8_t evt, u8_t *para,u16_t para_len)
{
    BT_HAL_DBG("%s, evt: 0x%x \n",__func__, evt);
    printf("%s, evt: 0x%x \r\n",__func__, evt);

    if (t_avrcp_ct_cb)
    {
        bt_prf_avrcp_ct_cb_param_t avrcp_ct_cb_param = { 0 };
        avrcp_ct_cb_param.change_ntf.event_id = evt;
        avrcp_ct_cb_param.change_ntf.event_parameter.playback = *para;
        t_avrcp_ct_cb(BT_PRF_AVRCP_TG_REGISTER_NOTIFICATION_EVT, &avrcp_ct_cb_param);
    }

    extern void *a2dp_pcm_handle;
    if (evt==BT_PRF_AVRCP_NOTIFICATION_PLAY_STATUS_CHANGE) {
        if (*para == BT_PRF_AVRCP_PLAYBACK_STOPPED ||
            *para == BT_PRF_AVRCP_PLAYBACK_PAUSED) {
            if(a2dp_pcm_handle){
                sbc2pcm_player_stop(a2dp_pcm_handle);
                sbc2pcm_player_close(a2dp_pcm_handle);
                a2dp_pcm_handle = NULL;
            }
        } else if (*para == BT_PRF_AVRCP_PLAYBACK_PLAYING) {
            if (!a2dp_pcm_handle) {
                a2dp_pcm_handle = sbc2pcm_player_open(0, 44100, 2, 0);
                int ret = sbc2pcm_player_start(a2dp_pcm_handle);
            }
        }
    }
}


/**
 * @brief      register callback function.
 * @param[in]  callback         callback function
 */
void bt_prf_avrcp_ct_register_callback(bt_prf_avrcp_ct_cb_t callback)
{
    BT_HAL_DBG("%s\n", __func__);
    t_avrcp_ct_cb = callback;
}

/**
 * @brief      Initialize the bluetooth AVRCP controller module.
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_avrcp_ct_init(void)
{
    BT_HAL_DBG("%s\n", __func__);
    return BT_STACK_STATUS_SUCCESS;
}

/**
 * @brief      Send register notification command to AVRCP target
 * @param[in]  event_id           id of notification events
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_avrcp_ct_cmd_register_notification(bt_prf_avrcp_rn_event_ids_t event_id)
{
    BT_HAL_DBG("%s\n", __func__);
    int err;

    err = avrcp_reg_not_cmd(NULL, event_id);
    if (!err) {
        BT_HAL_DBG("ct cmd register notification success. \n");
        err = BT_STACK_STATUS_SUCCESS;
    } else {
        BT_HAL_DBG("ct cmd register notification fail. \n");
        err = BT_STACK_STATUS_EIO;
    }

    return err;
}

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
                                                  bt_prf_avrcp_op_stat_t operation_state)
{
    BT_HAL_DBG("%s\n", __func__);
    int err;

    err = avrcp_pasthr_cmd(NULL, operation_state, operation_id);
    if (!err) {
        BT_HAL_DBG("ct cmd passthrough success. \n");
        err = BT_STACK_STATUS_SUCCESS;
    } else {
        BT_HAL_DBG("ct cmd passthrough fail. \n");
        err = BT_STACK_STATUS_EIO;
    }

    return err;
}

/**
 * @brief      Send GET_PLAY_STATUS PDU to AVRCP target, the result return on
 *             BT_PRF_AVRCP_CT_PLAY_STATUS_RSP_EVT
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_avrcp_ct_cmd_get_play_status(void)
{
    BT_HAL_DBG("%s\n", __func__);
    int err;

    err = avrcp_get_play_status_cmd(NULL);
    if (!err) {
        BT_HAL_DBG("ct cmd passthrough success. \n");
        err = BT_STACK_STATUS_SUCCESS;
    } else {
        BT_HAL_DBG("ct cmd passthrough fail. \n");
        err = BT_STACK_STATUS_EIO;
    }
    return BT_STACK_STATUS_SUCCESS;
}

/**
 * @brief      Send set absolute volume command to AVRCP target
 * @param[in]  volume             volume from 0 to 0x7f, means 0% to 100%
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_avrcp_ct_cmd_set_absolute_volume(uint8_t volume)
{
#if 0
    BT_HAL_DBG("%s\n", __func__);
    int err;

    err = avrcp_change_volume(volume);
    if (!err) {
        BT_HAL_DBG("ct cmd set absolute volume success. \n");
        err = BT_STACK_STATUS_SUCCESS;
    } else {
        BT_HAL_DBG("ct cmd set absolute volume fail. \n");
        err = BT_STACK_STATUS_EIO;
    }

    return err;
#endif
    return BT_STACK_STATUS_SUCCESS;
}

bt_stack_status_t bt_prf_avrcp_tg_send_rn_rsp(bt_prf_avrcp_rn_event_ids_t event_id,
                                              bt_prf_avrcp_ctype_response_t rsp,
                                              bt_prf_avrcp_rn_param_t *param)
{
    int err = BT_STACK_STATUS_SUCCESS;

    if(!param){
        return BT_STACK_STATUS_EINVAL;
    }

    if(event_id == BT_PRF_AVRCP_NOTIFICATION_VOLUME_CHANGE
       && rsp == BT_AVRCP_RESPONSE_CHANGED_STABLE){
        err = avrcp_change_volume(param->volume);
        if (!err) {
            BT_HAL_DBG("tg cmd set absolute volume success. \n");
            err = BT_STACK_STATUS_SUCCESS;
        } else {
            BT_HAL_DBG("tg cmd set absolute volume fail. \n");
            err = BT_STACK_STATUS_EIO;
        }
        return err;
    } else {
        return BT_STACK_STATUS_EINVAL;
    }

}

#define T_AVRCP_END


#define T_HFP_START

bt_prf_hfp_hf_cb_t t_hfp_cb = NULL;

static void t_hf_cb_connected(struct bt_conn *conn)
{
    BT_HAL_DBG("HFP HF Connected!");
    if (t_hfp_cb)
    {
        bt_prf_hfp_hf_cb_param_t hf_cb_param;
        hf_cb_param.conn_stat.state = BT_PRF_HFP_HF_CONNECTION_STATE_CONNECTED;
        memcpy(hf_cb_param.conn_stat.peer_addr.val, t_remote_addr.val, BT_STACK_BD_ADDR_LEN);
        t_hfp_cb(BT_PRF_HFP_HF_CONNECTION_STATE_EVT, &hf_cb_param);
    }
}

static void t_hf_cb_disconnected(struct bt_conn *conn)
{
    BT_HAL_DBG("HFP HF Disconnected!");
    if (t_hfp_cb)
    {
        bt_prf_hfp_hf_cb_param_t hf_cb_param;
        hf_cb_param.conn_stat.state = BT_PRF_HFP_HF_CONNECTION_STATE_DISCONNECTED;
        memcpy(hf_cb_param.conn_stat.peer_addr.val, t_remote_addr.val, BT_STACK_BD_ADDR_LEN);
        t_hfp_cb(BT_PRF_HFP_HF_CONNECTION_STATE_EVT, &hf_cb_param);
    }
}

static void t_hf_cb_service(struct bt_conn *conn, uint32_t value)
{
    BT_HAL_DBG("Service indicator value: %u", value);
    if (t_hfp_cb)
    {
        bt_prf_hfp_hf_cb_param_t hf_cb_param;
        if (value == 0) {
            hf_cb_param.service_availability.status = BT_PRF_HFP_HF_SERVICE_AVAILABILITY_STATUS_UNAVAILABLE;
        } else if (value == 1) {
            hf_cb_param.service_availability.status = BT_PRF_HFP_HF_SERVICE_AVAILABILITY_STATUS_AVAILABLE;
        } else {
            BT_HAL_DBG("invalid value.\n");
        }
        t_hfp_cb(BT_PRF_HFP_HF_CIND_SERVICE_AVAILABILITY_EVT, &hf_cb_param);
    }
}

static void t_hf_cb_call(struct bt_conn *conn, uint32_t value)
{
    BT_HAL_DBG("Call indicator value: %u", value);
    if (t_hfp_cb)
    {
        bt_prf_hfp_hf_cb_param_t hf_cb_param;
        if (value == 0) {
            hf_cb_param.call.status = BT_PRF_HFP_HF_CALL_STATUS_NO_CALLS;
        } else if (value == 1) {
            hf_cb_param.call.status = BT_PRF_HFP_HF_CALL_STATUS_CALL_IN_PROGRESS;
        } else {
            BT_HAL_DBG("invalid value.\n");
        }
        t_hfp_cb(BT_PRF_HFP_HF_CIND_CALL_EVT, &hf_cb_param);
    }
}

static void t_hf_cb_call_setup(struct bt_conn *conn, uint32_t value)
{
    BT_HAL_DBG("Call Setup indicator value: %u", value);
    if (t_hfp_cb)
    {
        bt_prf_hfp_hf_cb_param_t hf_cb_param;
        if (value == 0) {
            hf_cb_param.call_setup.status = BT_PRF_HFP_HF_CALL_SETUP_STATUS_NONE;
        } else if (value == 1) {
            hf_cb_param.call_setup.status = BT_PRF_HFP_HF_CALL_SETUP_STATUS_INCOMING;
        } else if (value == 2) {
            hf_cb_param.call_setup.status = BT_PRF_HFP_HF_CALL_SETUP_STATUS_OUTGOING_DIALING;
        } else if (value == 3) {
            hf_cb_param.call_setup.status = BT_PRF_HFP_HF_CALL_SETUP_STATUS_OUTGOING_ALERTING;
        } else {
            BT_HAL_DBG("invalid value.\n");
        }
        t_hfp_cb(BT_PRF_HFP_HF_CIND_CALL_SETUP_EVT, &hf_cb_param);
    }
}

static void t_hf_cb_call_held(struct bt_conn *conn, uint32_t value)
{
    BT_HAL_DBG("Call Held indicator value: %u", value);
    if (t_hfp_cb)
    {
        bt_prf_hfp_hf_cb_param_t hf_cb_param;
        if (value == 0) {
            hf_cb_param.call_held.status = BT_PRF_HFP_HF_CALL_HELD_STATUS_NONE;
        } else if (value == 1) {
            hf_cb_param.call_held.status = BT_PRF_HFP_HF_CALL_HELD_STATUS_HELD_AND_ACTIVE;
        } else if (value == 2) {
            hf_cb_param.call_held.status = BT_PRF_HFP_HF_CALL_HELD_STATUS_HELD;
        } else {
            BT_HAL_DBG("invalid value.\n");
        }
        t_hfp_cb(BT_PRF_HFP_HF_CIND_CALL_HELD_EVT, &hf_cb_param);
    }
}

static void t_hf_cb_signal(struct bt_conn *conn, uint32_t value)
{
    BT_HAL_DBG("Signal indicator value: %u", value);
    if (t_hfp_cb)
    {
        bt_prf_hfp_hf_cb_param_t hf_cb_param;
        hf_cb_param.signal_strength.value = value;
        t_hfp_cb(BT_PRF_HFP_HF_CIND_SIGNAL_STRENGTH_EVT, &hf_cb_param);
    }
}

static void t_hf_cb_roam(struct bt_conn *conn, uint32_t value)
{
    BT_HAL_DBG("Roaming indicator value: %u", value);
    if (t_hfp_cb)
    {
        bt_prf_hfp_hf_cb_param_t hf_cb_param;
        if (value == 0) {
            hf_cb_param.roaming.status = BT_PRF_HFP_HF_ROAMING_STATUS_INACTIVE;
        } else if (value == 1) {
            hf_cb_param.roaming.status = BT_PRF_HFP_HF_ROAMING_STATUS_ACTIVE;
        } else {
            BT_HAL_DBG("invalid value.\n");
        }
        t_hfp_cb(BT_PRF_HFP_HF_CIND_ROAMING_STATUS_EVT, &hf_cb_param);
    }
}

static void t_hf_cb_battery(struct bt_conn *conn, uint32_t value)
{
    BT_HAL_DBG("Battery indicator value: %u", value);
    if (t_hfp_cb)
    {
        bt_prf_hfp_hf_cb_param_t hf_cb_param;
        hf_cb_param.battery_level.value = value;
        t_hfp_cb(BT_PRF_HFP_HF_CIND_BATTERY_LEVEL_EVT, &hf_cb_param);
    }
}

static void t_hf_cb_ring_cb(struct bt_conn *conn)
{
    BT_HAL_DBG("Incoming Call...");
    if (t_hfp_cb)
    {
        t_hfp_cb(BT_PRF_HFP_HF_RING_IND_EVT, NULL);
    }
}

static struct bt_hfp_hf_cb t_hf_cb = {
	.connected = t_hf_cb_connected,
	.disconnected = t_hf_cb_disconnected,
	.service = t_hf_cb_service,
	.call = t_hf_cb_call,
	.call_setup = t_hf_cb_call_setup,
	.call_held = t_hf_cb_call_held,
	.signal = t_hf_cb_signal,
	.roam = t_hf_cb_roam,
	.battery = t_hf_cb_battery,
	.ring_indication = t_hf_cb_ring_cb,
};


/**
 * @brief      register callback function.
 * @param[in]  callback         callback function
 */
void bt_prf_hfp_hf_register_callback(bt_prf_hfp_hf_cb_t callback)
{
    BT_HAL_DBG("%s\n", __func__);
    t_hfp_cb = callback;
}

/**
 * @brief      Initialize the bluetooth HFP client module.
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_hfp_hf_init(void)
{
    BT_HAL_DBG("%s\n", __func__);

    bt_hfp_hf_register(&t_hf_cb);

    return BT_STACK_STATUS_SUCCESS;
}


static void rfcomm_recv(struct bt_rfcomm_dlc *dlci, struct net_buf *buf)
{
    BT_HAL_DBG("hfp incoming data dlc %p len %u \n", dlci, buf->len);
}

static void rfcomm_connected(struct bt_rfcomm_dlc *dlci)
{
    BT_HAL_DBG("hfp connected success \n");
}

static void rfcomm_disconnected(struct bt_rfcomm_dlc *dlci)
{
    BT_HAL_DBG("hfp disconnected \n");
}

static struct bt_rfcomm_dlc_ops rfcomm_ops = {
	.recv		= rfcomm_recv,
	.connected	= rfcomm_connected,
	.disconnected	= rfcomm_disconnected,
};

static struct bt_rfcomm_dlc rfcomm_dlc = {
	.ops = &rfcomm_ops,
	.mtu = 30,
};

/**
 * @brief      Connect to remote bluetooth HFP audio gateway(AG) device.
 * @param[in]  peer_addr         bluetooth device address of peer device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_hfp_hf_connect(bt_dev_addr_t *peer_addr)
{
    BT_HAL_DBG("%s\n", __func__);
    int err;

    if(!t_bt_def_conn){
        BT_HAL_DBG("Not connected.\n");
        return BT_STACK_STATUS_EIO;
    }

    err = bt_rfcomm_dlc_connect(t_bt_def_conn, &rfcomm_dlc, 0x01);
    if (err) {
        BT_HAL_DBG("hfp connect fail.\n");
        err = BT_STACK_STATUS_EIO;
    } else {
        BT_HAL_DBG("hfp connect pending.\n");
        err = BT_STACK_STATUS_SUCCESS;
    }

    return err;
}

/**
 * @brief      Disconnect from the remote HFP audio gateway
 * @param[in]  peer_addr         bluetooth device address of peer device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_hfp_hf_disconnect(bt_dev_addr_t *peer_addr)
{
    BT_HAL_DBG("%s\n", __func__);
    int err;

    if(!t_bt_def_conn){
        BT_HAL_DBG("Not connected.\n");
        return BT_STACK_STATUS_SUCCESS;
    }

    err = bt_conn_disconnect(t_bt_def_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
    if (err) {
        BT_HAL_DBG("hfp disconnection fail.\n");
        err = BT_STACK_STATUS_EIO;
    } else {
        BT_HAL_DBG("hfp disconnect success.\n");
        err = BT_STACK_STATUS_SUCCESS;
    }

    return err;
}

/**
 * @brief      Create audio connection with remote HFP AG.
 * @param[in]  peer_addr         bluetooth device address of peer device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_hfp_hf_audio_connect(bt_dev_addr_t *peer_addr)
{
    BT_HAL_DBG("%s\n", __func__);
    int err;
    struct bt_conn *conn;
    bt_addr_t sco_addr;
    char addr_str[18];

    memcpy(sco_addr.val, peer_addr, 6);

    bt_addr_to_str(&sco_addr, addr_str, sizeof(addr_str));
    BT_HAL_DBG("%s, create sco connection with : %s \n", __func__, addr_str);

    if(!t_bt_def_conn){
        BT_HAL_DBG("Not connected.\n");
        return BT_STACK_STATUS_EIO;
    }
    /* TO BE CHECK */
    conn = bt_conn_create_sco(&sco_addr,ESCO_PARAM_S1);
    if (!conn) {
        BT_HAL_DBG("hfp audio connect fail.\n");
        err = BT_STACK_STATUS_EIO;
    } else {
        BT_HAL_DBG("hfp audio connect success.\n");
        err = BT_STACK_STATUS_SUCCESS;

        if (t_hfp_cb)
        {
            bt_prf_hfp_hf_cb_param_t hf_cb_param;
            hf_cb_param.audio_stat.state = BT_PRF_HFP_HF_AUDIO_STATE_CONNECTED;
            memcpy(hf_cb_param.conn_stat.peer_addr.val, t_remote_addr.val, BT_STACK_BD_ADDR_LEN);
            t_hfp_cb(BT_PRF_HFP_HF_AUDIO_STATE_EVT, &hf_cb_param);
        }
    }

    return err;
}

/**
 * @brief      Release the established audio connection with remote HFP AG.
 * @param[in]  peer_addr         bluetooth device address of peer device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_hfp_hf_audio_disconnect(bt_dev_addr_t *peer_addr)
{
    BT_HAL_DBG("%s\n", __func__);
    int err;

    if(!t_bt_def_conn){
        BT_HAL_DBG("Not connected.\n");
        return BT_STACK_STATUS_SUCCESS;
    }

    err = bt_conn_disconnect(t_bt_def_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
    if (err) {
        BT_HAL_DBG("hfp audio disconnection fail.\n");
        err = BT_STACK_STATUS_EIO;
    } else {
        BT_HAL_DBG("hfp audio disconnect success.\n");
        err = BT_STACK_STATUS_SUCCESS;
        if (t_hfp_cb)
        {
            bt_prf_hfp_hf_cb_param_t hf_cb_param;
            hf_cb_param.audio_stat.state = BT_PRF_HFP_HF_AUDIO_STATE_DISCONNECTED;
            memcpy(hf_cb_param.conn_stat.peer_addr.val, t_remote_addr.val, BT_STACK_BD_ADDR_LEN);
            t_hfp_cb(BT_PRF_HFP_HF_AUDIO_STATE_EVT, &hf_cb_param);
        }
    }

    return err;
}

/**
 * @brief      Send call hold and multiparty commands,
 *             or enhanced call control commands(Use AT+CHLD).
 * @param[in]  chld             AT+CHLD call hold and multiparty handling AT command.
 * @param[in]  idx              used in Enhanced Call Control Mechanisms, used if chld is
 *                              BT_PRF_HFP_HF_CHLD_TYPE_REL_X or BT_PRF_HFP_HF_CHLD_TYPE_PRIV_X
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_hfp_hf_send_chld_cmd(bt_prf_hfp_hf_chld_type_t chld, int idx)
{
    BT_HAL_DBG("%s\n", __func__);
    return BT_STACK_STATUS_SUCCESS;
}

/**
 * @brief      Answer an incoming call(send ATA command).
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_hfp_hf_answer_call(void)
{
    BT_HAL_DBG("%s\n", __func__);
    int err;

    if(!t_bt_def_conn){
        BT_HAL_DBG("Not connected.\n");
        return BT_STACK_STATUS_EIO;
    }
    /* TO BE CHECK */
    err = bt_hfp_hf_send_cmd(t_bt_def_conn, BT_HFP_HF_ATA, NULL);
    if (err) {
        BT_HAL_DBG("hfp answer call fail.\n");
        err = BT_STACK_STATUS_EIO;
    } else {
        BT_HAL_DBG("hfp answer call success.\n");
        err = BT_STACK_STATUS_SUCCESS;
    }

    return err;
}

/**
 * @brief      Reject an incoming call(send AT+CHUP command).
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_hfp_hf_reject_call(void)
{
    BT_HAL_DBG("%s\n", __func__);
    int err;

    if(!t_bt_def_conn){
        BT_HAL_DBG("Not connected.\n");
        return BT_STACK_STATUS_EIO;
    }
    /* TO BE CHECK */
    err = bt_hfp_hf_send_cmd(t_bt_def_conn, BT_HFP_HF_AT_CHUP, NULL);
    if (err) {
        BT_HAL_DBG("hfp reject call fail.\n");
        err = BT_STACK_STATUS_EIO;
    } else {
        BT_HAL_DBG("hfp reject call success.\n");
        err = BT_STACK_STATUS_SUCCESS;
    }

    return err;
}

/**
 * @brief      Place a call with a specified number
 * @param[in]  number           number string of the call.
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_hfp_hf_dial(char *number)
{
    BT_HAL_DBG("%s\n", __func__);
    return BT_STACK_STATUS_SUCCESS;
}

/**
 * @brief      Volume synchronization with AG.
 * @param[in]  type             volume control target, speaker or microphone
 * @param[in]  volume           gain of the speaker of microphone, ranges 0 to 15
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_prf_hfp_hf_vol_update(pbt_prf_hfp_hf_vol_ctrl_target_t type, int volume)
{
    BT_HAL_DBG("%s\n", __func__);
    int err;
    int at_cmd;

    if(!t_bt_def_conn){
        BT_HAL_DBG("Not connected.\n");
        return BT_STACK_STATUS_EIO;
    }

    if (type == BT_PRF_HFP_HF_VOLUME_CONTROL_TARGET_SPK) {
        at_cmd = BT_HFP_HF_AT_VGS;
    } else {
        at_cmd = BT_HFP_HF_AT_VGM;
    }

    err = bt_hfp_hf_send_cmd_arg(t_bt_def_conn, at_cmd, volume);
    if (err) {
        BT_HAL_DBG("hfp vol update fail.\n");
        err = BT_STACK_STATUS_EIO;
    } else {
        BT_HAL_DBG("hfp vol update success.\n");
        err = BT_STACK_STATUS_SUCCESS;
    }

    return err;
}

#define T_HFP_END

bool hal_bt_is_connected(void)
{
    return (bool)(t_bt_def_conn != NULL);
}

#endif

