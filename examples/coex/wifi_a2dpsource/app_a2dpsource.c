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
#include "bflb_romfs.h"

#if defined(CONFIG_SHELL)
#include "shell.h"
#endif

struct bt_br_discovery_result result[10] = { 0 };

static TaskHandle_t g_btinittask_hdl = NULL;

#ifdef CONFIG_BT_A2DP
struct k_thread media_transport;
static void a2dp_chain(struct bt_conn *conn, uint8_t state);
static void a2dp_stream(uint8_t state);
static void a2dp_start_cfm(void);
static bool media_task_create = false;

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

static br_a2dp_init(void)
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
            K_PRIO_COOP(configMAX_PRIORITIES - 10)
            );
      media_task_create = true;  
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
#endif

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

/* ============================  pri cli ============================ */
int bt_coex_init(void)
{
    bredr_init(0, NULL);

    return 0;
}

int bt_coex_deinit(void)
{
    printf("Not support deinit.\r\n");
    return 0;
}

int bt_a2dp_scan(void)
{
    /* update coex status */
    printf("Scanning...\r\n");

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
int bt_br_connect(char *str)
{
    struct bt_conn *conn;
    u8_t  addr_val[6];
    bt_addr_t peer_addr;
    struct bt_br_conn_param param;
    char addr_str[18];

    /* update coext status */
    printf("Connecting %s...\r\n", str);

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

int bt_br_disconnect(void)
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

int bt_a2dpstart(void)
{
    printf("A2DP started.\r\n");

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

int bt_a2dpstop(void)
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

int bt_a2dpresume(void)
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

int cmd_bt(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: bt [init|scan|connect <device>|disconnect|a2dpstart|a2dpresume]\r\n");
        return -1;
    }

    if (strcmp(argv[1], "init") == 0) {
        return br_a2dp_init();
    } else if (strcmp(argv[1], "scan") == 0) {
        return bt_a2dp_scan();
    } else if (strcmp(argv[1], "connect") == 0) {
        if (argc < 3) {
            printf("Error: No device specified for connect.\r\n");
            return -1;
        }
        return bt_br_connect(argv[2]);
    } else if (strcmp(argv[1], "disconnect") == 0) {
        return bt_br_disconnect();
    } else if (strcmp(argv[1], "a2dpstart") == 0) {
        return bt_a2dpstart();
    } else if (strcmp(argv[1], "a2dpstop") == 0) {
        return bt_a2dpstop();
    } else if (strcmp(argv[1], "a2dpresume") == 0) {
        return bt_a2dpresume();
    } else {
        printf("Invalid command: %s\r\n", argv[1]);
        return -1;
    }
}

#if defined(CONFIG_SHELL)
SHELL_CMD_EXPORT_ALIAS(cmd_bt, bt, bt [init|scan|connect <device>|disconnect|a2dpstart|a2dpresume]);
#endif

