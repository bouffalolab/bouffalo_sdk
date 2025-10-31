#include <FreeRTOS.h>
#include "task.h"
#include "board.h"

#include "bluetooth.h"
#include "conn.h"
#include "conn_internal.h"
#if defined(BL702) || defined(BL602)
#include "ble_lib_api.h"
#elif defined(BL616)
#include "btble_lib_api.h"
#include "bl616_glb.h"
#include "rfparam_adapter.h"
#elif defined(BL808)
#include "btble_lib_api.h"
#include "bl808_glb.h"
#endif

#include "hci_driver.h"
#include "hci_core.h"

#include "bflb_mtd.h"
#include "easyflash.h"

#include <byteorder.h>
#include <bluetooth.h>
#include <hci_host.h>
#include <l2cap.h>
#include <l2cap_internal.h>

#ifdef CONFIG_BT_A2DP
#include <a2dp.h>
#endif
#ifdef CONFIG_BT_AVRCP
#include <avrcp.h>
#endif
#include "a2dp_source_audio.h"

#include "coex.h"
#include "bflb_romfs.h"

#if defined(CONFIG_SHELL)
#include "shell.h"
#endif

struct bt_br_discovery_result result[10] = { 0 };

static TaskHandle_t g_btinittask_hdl = NULL;

#if defined(CONFIG_SHELL)
#define BT_CLI(func) void bredr_##func(int argc, char **argv)
#define BT_A2DP_CLI(func) void a2dp_##func(int argc, char **argv)
#define BT_AVRCP_CLI(func) void avrcp_##func(int argc, char **argv)
#endif
#ifdef CONFIG_BT_A2DP
struct k_thread media_transport;
static void a2dp_chain(struct bt_conn *conn, uint8_t state);
static void a2dp_stream(uint8_t state);
static void a2dp_start_cfm(void);
static bool media_task_create = false;

static int _bt_coex_init(coex_notify_cb cb, void* env);
static int _bt_coex_update(int act_type);

static struct a2dp_callback a2dp_callbacks =
{
    .chain = a2dp_chain,
    .stream = a2dp_stream,
#ifdef CONFIG_BT_A2DP_SOURCE
    .start_cfm = a2dp_start_cfm,
#endif
};
#endif

#ifdef CONFIG_BT_AVRCP
static void avrcp_chain(struct bt_conn *conn, uint8_t state);
static void avrcp_absvol(uint8_t vol);
static void avrcp_play_status(uint32_t song_len, uint32_t song_pos, uint8_t status);
static void avrcp_passthrough_response(bool released, u8_t option_id);
static void avrcp_passthrough_handler(bool released, u8_t option_id);
static void avrcp_handle_play(void);
static void avrcp_handle_stop(void);
static void avrcp_handle_pause(void);
static void avrcp_handle_next(void);
static void avrcp_handle_previous(void);
static bool steam_pause = false;

struct avrcp_pth_handler {
	uint8_t op;
	void (*func) (void);
};

static const struct avrcp_pth_handler avrcp_pth_handlers[] = {
    { AVRCP_KEY_PLAY, avrcp_handle_play },
    { AVRCP_KEY_STOP, avrcp_handle_stop },
    { AVRCP_KEY_PAUSE, avrcp_handle_pause },
    { AVRCP_KEY_FORWARD, avrcp_handle_next },
    { AVRCP_KEY_BACKWARD, avrcp_handle_previous },
};
static struct avrcp_callback avrcp_callbacks =
{
    .chain = avrcp_chain,
    .abs_vol = avrcp_absvol,
    .play_status = avrcp_play_status,
    .rp_passthrough = NULL,//avrcp_passthrough_response,
    .passthrough_handler=avrcp_passthrough_handler,
};
#endif

static void bredr_connected(struct bt_conn *conn, u8_t err);
static void bredr_disconnected(struct bt_conn *conn, u8_t reason);

static bool init = false;
static struct bt_conn_info conn_info;
static struct bt_conn *default_conn = NULL;

static struct bt_conn_cb conn_callbacks = {
    .connected = bredr_connected,
    .disconnected = bredr_disconnected,
};

BT_CLI(init);
BT_CLI(connect);
BT_CLI(disconnect);
BT_CLI(start_inquiry);
BT_CLI(stop_inquiry);


#ifdef CONFIG_BT_A2DP
BT_A2DP_CLI(connect);
BT_A2DP_CLI(suspend);
BT_A2DP_CLI(resume);
#endif

#ifdef CONFIG_BT_AVRCP
BT_AVRCP_CLI(connect);
BT_AVRCP_CLI(pth_key);
BT_AVRCP_CLI(pth_key_act);
BT_AVRCP_CLI(change_vol);
BT_AVRCP_CLI(set_vol);
BT_AVRCP_CLI(get_vol);
BT_AVRCP_CLI(send_play_status);
#endif

#if defined(CONFIG_SHELL)
    SHELL_CMD_EXPORT_ALIAS(bredr_init,bredr_init,BREDR Initialize Parameter:[Null]);
    SHELL_CMD_EXPORT_ALIAS(bredr_connect,bredr_connect,
                            bredr_connect Parameter:[address eg.18bf591245]);
    SHELL_CMD_EXPORT_ALIAS(bredr_disconnect,bredr_disconnect,
                            bredr_disconnect Parameter:[address eg.18bf591245]);
    SHELL_CMD_EXPORT_ALIAS(bredr_start_inquiry,bredr_start_inquiry,"");
    SHELL_CMD_EXPORT_ALIAS(bredr_stop_inquiry,bredr_stop_inquiry,"");
    #ifdef CONFIG_BT_A2DP
    SHELL_CMD_EXPORT_ALIAS(a2dp_connect,a2dp_connect,"");
    #ifdef CONFIG_BT_A2DP_SOURCE
    SHELL_CMD_EXPORT_ALIAS(a2dp_suspend,a2dp_source_suspend, "");
    SHELL_CMD_EXPORT_ALIAS(a2dp_resume, a2dp_source_resume, "");
    #endif
    #endif

    #ifdef CONFIG_BT_AVRCP
    SHELL_CMD_EXPORT_ALIAS(avrcp_connect,avrcp_connect,"");
    SHELL_CMD_EXPORT_ALIAS(avrcp_pth_key,avrcp_pth_key,"");
    SHELL_CMD_EXPORT_ALIAS(avrcp_pth_key_act,avrcp_pth_key_act,"");
    SHELL_CMD_EXPORT_ALIAS(avrcp_change_vol,avrcp_change_vol,"");
    SHELL_CMD_EXPORT_ALIAS(avrcp_set_vol,avrcp_set_vol,"");
    SHELL_CMD_EXPORT_ALIAS(avrcp_get_vol,avrcp_get_vol,"");
    SHELL_CMD_EXPORT_ALIAS(avrcp_send_play_status,avrcp_send_play_status,"");
    #endif
#endif
void bt_enable_cb(int err)
{
    if (!err) {
        bt_addr_le_t bt_addr;
        bt_get_local_public_address(&bt_addr);
        printf("BD_ADDR:(MSB)%02x:%02x:%02x:%02x:%02x:%02x(LSB) \r\n",
               bt_addr.a.val[5], bt_addr.a.val[4], bt_addr.a.val[3], bt_addr.a.val[2], bt_addr.a.val[1], bt_addr.a.val[0]);
    }
}

static TaskHandle_t app_start_handle;

static void app_start_task(void *pvParameters)
{
    // Initialize BLE controller
    #if defined(BL702) || defined(BL602)
    ble_controller_init(configMAX_PRIORITIES - 1);
    #else
    btble_controller_init(configMAX_PRIORITIES - 1);
    #endif
    // Initialize BLE Host stack
    hci_driver_init();
    bt_enable(bt_enable_cb);

    vTaskDelete(NULL);
}

void app_bt_init(void)
{
    xTaskCreate(app_start_task, (char *)"app_start", 1024, NULL, configMAX_PRIORITIES - 2, &app_start_handle);
}


BT_CLI(init)
{
    if(init){
        printf("bredr has initialized\n");
        return;
    }
    default_conn = NULL;
    bt_conn_cb_register(&conn_callbacks);
#ifdef CONFIG_BT_A2DP
    a2dp_cb_register(&a2dp_callbacks);
#endif
#ifdef CONFIG_BT_AVRCP
    avrcp_cb_register(&avrcp_callbacks);
#endif
    init = true;
    printf("bredr init successfully\n");
}
static void bredr_connected(struct bt_conn *conn, u8_t err)
{
    if(conn->type != BT_CONN_TYPE_BR)
    {
        return;
    }

    char addr[BT_ADDR_STR_LEN];

    bt_conn_get_info(conn, &conn_info);
    bt_addr_to_str(conn_info.br.dst, addr, sizeof(addr));

    if (err) {
        printf("bredr failed to connect to %s (%u) \r\n", addr, err);
        return;
    }

    printf("bredr connected: %s \r\n", addr);

    if (!default_conn)
    {
        default_conn = conn;
    }

    _bt_coex_update(ACT_BT_CONNECT_DONE_OK);
}

static void bredr_disconnected(struct bt_conn *conn, u8_t reason)
{
    if(conn->type != BT_CONN_TYPE_BR)
    {
        return;
    }

    char addr[BT_ADDR_STR_LEN];

    bt_conn_get_info(conn, &conn_info);
    bt_addr_to_str(conn_info.br.dst, addr, sizeof(addr));

    printf("bredr disconnected: %s (reason %u) \r\n", addr, reason);

    if (default_conn == conn)
    {
        default_conn = NULL;
    }

    _bt_coex_update(ACT_BT_CONNECT_DONE_FAIL);
}

BT_CLI(connect)
{
    struct bt_conn *conn;
    u8_t  addr_val[6];
    bt_addr_t peer_addr;
    struct bt_br_conn_param param;
    char addr_str[18];

    get_bytearray_from_string(&argv[1], addr_val, 6);
    reverse_bytearray(addr_val, peer_addr.val, 6);

    bt_addr_to_str(&peer_addr, addr_str, sizeof(addr_str));
    printf("%s, create bredr connection with : %s \n", __func__, addr_str);

    param.allow_role_switch = true;

    conn = bt_conn_create_br(&peer_addr, &param);
    if (conn) {
        printf("Connect bredr ACL success.\n");
    } else {
        printf("Connect bredr ACL fail.\n");
    }
}

BT_CLI(disconnect)
{
    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    int err = bt_conn_disconnect(default_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
    if (err) {
        printf("Disconnection failed.\n");
    } else {
        printf("Disconnect successfully.\n");
    }

}




typedef struct {
    uint8_t data_type;
    uint8_t data_length;
    uint8_t *data;
} eir_data_t;

static void bredr_parse_eir_data(const uint8_t *eir, size_t eir_len)
{
    size_t pos = 0;
    while (pos < eir_len) {
        eir_data_t data_field;
        data_field.data_length = eir[pos++];
        data_field.data_type = eir[pos++];
        data_field.data = &eir[pos];

        if (pos + data_field.data_length - 1 >= eir_len) {
            break;
        }

        switch (data_field.data_type) {
            case 0x08: // Shortened Local Name
            case 0x09: // Complete Local Name
                printf("Device Name: %.*s\n", data_field.data_length - 1, data_field.data);
                break;
            default:
                pos += data_field.data_length - 1;
                break;
        }
    }
}

void bt_br_discv_cb(struct bt_br_discovery_result *results,
				  size_t count)
{
    char addr_str[18];
    uint32_t dev_class;
    int i;

    if (!results || count== 0) {
        return;
    }

    for (i=0;i<count;i++) {
        dev_class = (results[i].cod[0] | (results[i].cod[1] << 8) | 
                     (results[i].cod[1] << 16));
        bt_addr_to_str(&results[i].addr, addr_str, sizeof(addr_str));
        printf("addr %s,class 0x%lx,rssi %d\r\n",addr_str,
                     dev_class,results[i].rssi);
        bredr_parse_eir_data(&results[i].eir,240);
    }

    /* update coex status */
    _bt_coex_update(ACT_BT_SCAN_DONE);
}


BT_CLI(start_inquiry)
{
    struct bt_br_discovery_param param;

    //Valid range 0x01 - 0x30.
    param.length = 0x05;
    param.limited = 0;

    int err = bt_br_discovery_start(&param,result,10,bt_br_discv_cb);
    if (err) {
        printf("BREDR discovery failed\n");
    }
}

BT_CLI(stop_inquiry)
{
    int err = bt_br_discovery_stop();
    if (err) {
        printf("BREDR stop discovery failed\n");
    }
}

#ifdef CONFIG_BT_A2DP
static void a2dp_chain(struct bt_conn *conn, uint8_t state)
{
    printf("%s, conn: %p \n", __func__, conn);

    if (state == BT_A2DP_CHAIN_CONNECTED) {
        printf("a2dp connected. \n");
    } else if (state == BT_A2DP_CHAIN_DISCONNECTED) {
        printf("a2dp disconnected. \n");
    }
}

static void a2dp_stream(uint8_t state)
{
    printf("%s, state: %d \n", __func__, state);

    if (state == BT_A2DP_STREAM_START) {
        printf("a2dp play. \n");
    } else if (state == BT_A2DP_STREAM_SUSPEND) {
        printf("a2dp stop. \n");
    }
}

#ifdef CONFIG_BT_A2DP_SOURCE
static void media_thread(void *args)
{
   while (1) {
        if(steam_pause == false) {
            int err;
            romfs_file_t    fp;
            romfs_filebuf_t filebuf;

            romfs_open(&fp, "/romfs/bp_stereo_441.pcm", 0);
            romfs_getbuf(&fp, &filebuf);
            //printf("xip:0x%08lX, size:%d\r\n", filebuf.buf, filebuf.bufsize);

            err = bt_a2dp_send_media(filebuf.buf, filebuf.bufsize);
            if (err) {
                printf("send media fail %d\r\n", err);
            }
        } else {
            vTaskDelay(1);
        }
    }

    vTaskDelete(NULL);
}

static void a2dp_start_cfm()
{
   printf("%s \n", __func__);
   if (!media_task_create)
   {
      k_thread_create(&media_transport,
          "media_transport",
           4096,
           (k_thread_entry_t)media_thread,
            #if 0
            K_PRIO_COOP(36)
            #else
            K_PRIO_COOP(configMAX_PRIORITIES - 1)
            #endif
            );
      media_task_create = true;  
   }
}
#endif

BT_A2DP_CLI(connect)
{
    struct bt_a2dp *a2dp;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    a2dp = bt_a2dp_connect(default_conn);
    if(a2dp) {
        printf("a2dp connect successfully.\n");
    } else {
        printf("a2dp connect fail. \n");
    }
}

#ifdef CONFIG_BT_A2DP_SOURCE
BT_A2DP_CLI(suspend)
{
    int ret = 0;

    if (!default_conn) {
        printf("Not connected.\n");
        return;
    }

    ret = bt_stream_suspend(default_conn);
    if (!ret) {
        printf("A2dp suspend stream successfully.\n");
    } else {
        printf("A2dp suspend stream fail. ret(%d)\n",ret);
    }
}

BT_A2DP_CLI(resume)
{
    int ret = 0;

    if (!default_conn) {
        printf("Not connected.\n");
        return;
    }

    ret = bt_stream_resume(default_conn);
    if (!ret) {
        printf("A2dp resume stream successfully.\n");
    } else {
        printf("A2dp resume stream fail. ret(%d)\n",ret);
    }
}
#endif

#endif


#ifdef CONFIG_BT_AVRCP
static void avrcp_chain(struct bt_conn *conn, uint8_t state)
{
    printf("%s, conn: %p \n", __func__, conn);

    if (state == BT_AVRCP_CHAIN_CONNECTED) {
        #ifdef CONFIG_BT_A2DP_SOURCE
        avrcp_send_volume_notification(NULL);
        #endif
        printf("avrcp connected. \n");
    } else if (state == BT_AVRCP_CHAIN_DISCONNECTED) {
        printf("avrcp disconnected. \n");
    }
}

static void avrcp_absvol(uint8_t vol)
{
    printf("%s, vol: %d \n", __func__, vol);
}

static void avrcp_play_status(uint32_t song_len, uint32_t song_pos, uint8_t status)
{
    printf("%s, song length: %lu, song position: %lu, play status: %u \n", __func__, song_len, song_pos, status);
}

static void avrcp_passthrough_response(bool released, u8_t option_id)
{
	printf("released: %d option id: 0x%x \n",released, option_id);

	if(released == 0)
	{
		//user todo 

	}
}

static void avrcp_passthrough_handler(bool released, u8_t option_id)
{
    printf("released: %d option id: 0x%x \n",released, option_id);
    if(released==PASTHR_STATE_RELEASED)
    {
        for (int i = 0; i < ARRAY_SIZE(avrcp_pth_handlers); i++) {

            if(avrcp_pth_handlers[i].op==option_id)
            {
                avrcp_pth_handlers[i].func();
            }

        }
    }
    
}

static void avrcp_handle_play(void)
{
    printf("%s\r\n",__func__);
    steam_pause = false;
}

static void avrcp_handle_stop(void)
{
    printf("%s\r\n",__func__);
}

static void avrcp_handle_pause(void)
{
    printf("%s\r\n",__func__);
    steam_pause = true;
}

static void avrcp_handle_next(void)
{
    printf("%s\r\n",__func__);
}

static void avrcp_handle_previous(void)
{
    printf("%s\r\n",__func__);
}

BT_AVRCP_CLI(connect)
{
    struct bt_avrcp *avrcp = NULL;

    if (!default_conn) {
        printf("Not connected.\n");
        return;
    }

    avrcp = bt_avrcp_connect(default_conn);
    if(!avrcp) {
        printf("avrcp connect failed\n");
    } else {
        printf("avrcp connect successfully.\n");
    }
}

BT_AVRCP_CLI(pth_key)
{
    int err;
    uint8_t key;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    get_uint8_from_string(&argv[1], &key);

    err = avrcp_pasthr_cmd(NULL, PASTHR_STATE_PRESSED, key);
    if(err) {
        printf("avrcp key pressed failed, err: %d\n", err);
    } else {
        printf("avrcp key pressed successfully.\n");
    }

    err = avrcp_pasthr_cmd(NULL, PASTHR_STATE_RELEASED, key);
    if(err) {
        printf("avrcp key released failed, err: %d\n", err);
    } else {
        printf("avrcp key play released successfully.\n");
    }
}
BT_AVRCP_CLI(pth_key_act)
{
    int err;
    uint8_t key;
    uint8_t action;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    get_uint8_from_string(&argv[1], &key);
    get_uint8_from_string(&argv[2], &action);

    if (action != PASTHR_STATE_PRESSED && action != PASTHR_STATE_RELEASED)
    {
        printf("invalid key action.\n");
        return;
    }

    err = avrcp_pasthr_cmd(NULL, action, key);
    if(err) {
        printf("avrcp key action failed, err: %d\n", err);
    } else {
        printf("avrcp %s key %d successfully.\n", action ? "released":"pressed", key);
    }
}

BT_AVRCP_CLI(change_vol)
{
    int err;
    uint8_t vol;
    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    get_uint8_from_string(&argv[1], &vol);
    err = avrcp_change_volume(vol);
    if (err) {
        printf("avrcp change volume fail\n");
    } else {
        printf("avrcp change volume success\n");
    }
}


BT_AVRCP_CLI(set_vol)
{
    int err;
    uint8_t vol;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }
    
    get_uint8_from_string(&argv[1], &vol);

    err = avrcp_set_absvol_cmd(NULL,vol);
    if(err) {
        printf("change volume failed, err: %d\n", err);
    } else {
        printf("change volume pressed successfully.\n");
    }

}

BT_AVRCP_CLI(get_vol)
{
    int err;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }
    
    err = avrcp_send_volume_notification(NULL); //this is test cmd. volume notification will handle volume at bt_profile internal.
    if(err) {
        printf("send get volume failed, err: %d\n", err);
    } else {
        printf("send get volume successfully.\n");
    }
    
}

BT_AVRCP_CLI(send_play_status)
{
    int err;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }
    //this is test cmd. play status will handle at bt_profile internal, user needs to periodically update the player parameters using the avrcp_set_player_parameter function.
    avrcp_set_player_parameter(PLAY_STATUS_PLAYING,0x00003916,0x0003e030);//0x00003916 =14614ms;0x0003e030 = 254000ms
    err = avrcp_response_play_status(NULL,12);
    
    if(err) {
        printf("avrcp_send_play_status failed, err: %d\n", err);
    } else {
        printf("avrcp_send_play_status successfully.\n");
    }
}

#endif

/* ============================  pri coex ============================ */
static int _bt_coex_init(coex_notify_cb cb, void* env)
{
    struct coex_evt_arg arg = {
        .role             = COEX_ROLE_BT,
        .type             = COEX_EVT_INIT,
        .arg.init.cb      = cb,
        .arg.init.env     = env,
    };

    if (COEX_OK != coex_event(&arg)) {
        return -1;
    }
}

static int _bt_coex_update(int act_type)
{
    struct coex_evt_arg arg = {
        .role             = COEX_ROLE_BT,
        .type             = COEX_EVT_SET_ACTIVITY,
        .arg.set_act.type = act_type,
    };

    if (COEX_OK != coex_event(&arg)) {
        return -1;
    }

    return 0;
}
/* ============================  pri func ============================ */
static void ble_connected(struct bt_conn *conn, u8_t err)
{
    if(err || conn->type != BT_CONN_TYPE_LE) {
        return;
    }
    printf("%s",__func__);
}

static void ble_disconnected(struct bt_conn *conn, u8_t reason)
{ 
    int ret;

    if(conn->type != BT_CONN_TYPE_LE)
    {
        return;
    }

    printf("%s",__func__);

    // enable adv
    ret = set_adv_enable(true);
    if(ret) {
        printf("Restart adv fail. \r\n");
    }
}

static void _btcoex_init_done_cb(void *env, struct coex_notify_args *args)
{
    (void)env;
    if (!args) {
        return;
    }

    // TODO
    // printf("_btcoex_init_done_cb\r\n");
}
 
/* ============================  pri cli ============================ */
int bt_coex_init(void)
{
    printf("bt_coex_init...\r\n");
    _bt_coex_init(_btcoex_init_done_cb, NULL);
    bredr_init(0, NULL);

    return 0;
}

int bt_coex_deinit(void)
{
    printf("Not support deinit.\r\n");
    return 0;
}

int bt_coex_scan(void)
{
    /* update coex status */
    printf("Scanning...\r\n");
    _bt_coex_update(ACT_BT_SCAN_START);

    /* bredr_start_inquiry */
    struct bt_br_discovery_param param;

    //Valid range 0x01 - 0x30.
    param.length = 0x05;
    param.limited = 0;

    int err = bt_br_discovery_start(&param,result,10,bt_br_discv_cb);
    if (err) {
        printf("BREDR discovery failed\n");
    }
    return 0;
}

void __reverse_bytearray(uint8_t *src, uint8_t *result, int array_size)
{
    for(int i=0; i < array_size;i++){
        result[array_size - i -1] = src[i];
    }
}
int bt_coex_connect(char *str)
{
    struct bt_conn *conn;
    u8_t  addr_val[6];
    bt_addr_t peer_addr;
    struct bt_br_conn_param param;
    char addr_str[18];

    /* update coext status */
    printf("Connecting %s...\r\n", str);
    _bt_coex_update(ACT_BT_CONNECT_START);

    /* bredr_connect(...) */
    get_bytearray_from_string(&str, addr_val, 6);
    reverse_bytearray(addr_val, peer_addr.val, 6);

    bt_addr_to_str(&peer_addr, addr_str, sizeof(addr_str));
    printf("%s, create bredr connection with : %s \n", __func__, addr_str);

    param.allow_role_switch = true;

    conn = bt_conn_create_br(&peer_addr, &param);
    if (conn) {
        printf("Connect bredr ACL success.\n");
    } else {
        printf("Connect bredr ACL fail.\n");
    }

    return 0;
}

int bt_coex_disconnect(void)
{
    printf("Disconnecting...\r\n");

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    int err = bt_conn_disconnect(default_conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
    if (err) {
        printf("Disconnection failed.\n");
    } else {
        printf("Disconnect successfully.\n");
    }

    return 0;
}

int bt_coex_a2dpstart(void)
{
    printf("A2DP started.\r\n");
    _bt_coex_update(ACT_BT_A2DP_START);

    /* a2dp_connect */
    struct bt_a2dp *a2dp;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    a2dp = bt_a2dp_connect(default_conn);
    if(a2dp) {
        printf("a2dp connect successfully.\n");
    } else {
        printf("a2dp connect fail. \n");
    }
    return 0;
}

int bt_coex_a2dpstop(void)
{
    printf("A2DP stopped.\r\n");
    int ret = 0;
    if (!default_conn) {
        printf("Not connected.\n");
        return;
    }

    ret = bt_stream_suspend(default_conn);
    if (!ret) {
        printf("A2dp suspend stream successfully.\n");
    } else {
        printf("A2dp suspend stream fail. ret(%d)\n",ret);
    }
    return 0;
}

int bt_coex_a2dpresume(void)
{
    printf("A2DP resume...\r\n");
    int ret = 0;

    if (!default_conn) {
        printf("Not connected.\n");
        return;
    }

    ret = bt_stream_resume(default_conn);
    if (!ret) {
        printf("A2dp resume stream successfully.\n");
    } else {
        printf("A2dp resume stream fail. ret(%d)\n",ret);
    }
    return 0;
}

static struct bt_conn_cb ble_conn_callbacks = {
	.connected	    = ble_connected,
	.disconnected	= ble_disconnected,
};

static void __bt_enable_cb(int err)
{
    if (!err) {
        bt_addr_le_t bt_addr;
        bt_get_local_public_address(&bt_addr);
        printf("BD_ADDR:(MSB)%02x:%02x:%02x:%02x:%02x:%02x(LSB) \r\n",
               bt_addr.a.val[5], bt_addr.a.val[4], bt_addr.a.val[3],
               bt_addr.a.val[2], bt_addr.a.val[1], bt_addr.a.val[0]);
        bt_conn_cb_register(&ble_conn_callbacks);
        ble_cli_register();
        #if defined(CONFIG_BT_BREDR)
        bredr_cli_register();
        #endif
    }
}

static void btinit_task_entry(void *arg)
{
    // Initialize BLE controller
    #if defined(BL702) || defined(BL602)
    ble_controller_init(configMAX_PRIORITIES - 1);
    #else
    btble_controller_init(configMAX_PRIORITIES - 1);
    #endif
    // Initialize BLE Host stack
    hci_driver_init();
    bt_enable(__bt_enable_cb);

    vTaskDelete(NULL);
}

void app_a2dpsource_init(void)
{
    if (g_btinittask_hdl) {
        printf("Task:btinit reinit?\r\n");
        return;
    }

    if (pdPASS != xTaskCreate(btinit_task_entry, (char *)"btinit", 1024,
                            NULL, configMAX_PRIORITIES - 2,
                            &g_btinittask_hdl)) {
        printf("Task create error.\r\n");
    }
}

int cmd_bt(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: bt [init|scan|connect <device>|disconnect|a2dpstart|a2dpresume]\r\n");
        return -1;
    }

    if (strcmp(argv[1], "init") == 0) {
        return bt_coex_init();
    } else if (strcmp(argv[1], "deinit") == 0) {
        return bt_coex_deinit();
    } else if (strcmp(argv[1], "scan") == 0) {
        return bt_coex_scan();
    } else if (strcmp(argv[1], "connect") == 0) {
        if (argc < 3) {
            printf("Error: No device specified for connect.\r\n");
            return -1;
        }
        return bt_coex_connect(argv[2]);
    } else if (strcmp(argv[1], "disconnect") == 0) {
        return bt_coex_disconnect();
    } else if (strcmp(argv[1], "a2dpstart") == 0) {
        return bt_coex_a2dpstart();
    } else if (strcmp(argv[1], "a2dpstop") == 0) {
        return bt_coex_a2dpstop();
    } else if (strcmp(argv[1], "a2dpresume") == 0) {
        return bt_coex_a2dpresume();
    } else {
        printf("Invalid command: %s\r\n", argv[1]);
        return -1;
    }
}

#if defined(CONFIG_SHELL)
SHELL_CMD_EXPORT_ALIAS(cmd_bt, bt, bt [init|scan|connect <device>|disconnect|a2dpstart|a2dpresume]);
#endif

