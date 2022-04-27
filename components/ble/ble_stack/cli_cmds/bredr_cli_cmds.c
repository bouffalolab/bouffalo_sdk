/** @file
 * @brief Bluetooth BR/EDR shell module
 *
 * Provide some BR/EDR commands that can be useful to applications.
 */

#include <stdlib.h>
#include <string.h>
#include <byteorder.h>
#include <bluetooth.h>
#include <hci_host.h>
#include <hci_core.h>
#include <conn.h>
#include <conn_internal.h>
#include <l2cap.h>
#if CONFIG_BT_A2DP
#include <a2dp.h>
#endif
#if CONFIG_BT_AVRCP
#include <avrcp.h>
#endif

#include "cli.h"

#if PCM_PRINTF
#include "oi_codec_sbc.h"
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

#if CONFIG_BT_A2DP
static void a2dp_chain(struct bt_conn *conn, uint8_t state);
static void a2dp_stream(uint8_t state);

static struct a2dp_callback a2dp_callbacks = {
    .chain = a2dp_chain,
    .stream = a2dp_stream,
};
#endif

#if CONFIG_BT_AVRCP
static void avrcp_chain(struct bt_conn *conn, uint8_t state);
static void avrcp_absvol(uint8_t vol);

static struct a2dp_callback avrcp_callbacks = {
    .chain = avrcp_chain,
    .stream = avrcp_absvol,
};
#endif

#if PCM_PRINTF
static void pcm(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
#endif
static void bredr_init(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
static void bredr_write_eir(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void bredr_discoverable(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void bredr_connectable(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void bredr_connect(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void bredr_disconnect(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void bredr_remote_name(char *p_write_buffer, int write_buffer_len, int argc, char **argv);

#if CONFIG_BT_A2DP
static void a2dp_connect(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
#endif

#if CONFIG_BT_AVRCP
static void avrcp_connect(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void avrcp_pth_key(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void avrcp_pth_key_act(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
static void avrcp_change_vol(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
#endif

const struct cli_command bredr_cmd_set[] STATIC_CLI_CMD_ATTRIBUTE = {
#if PCM_PRINTF
    { "pcm", "", pcm },
#endif
    { "bredr_init", "", bredr_init },
    { "bredr_eir", "", bredr_write_eir },
    { "bredr_connectable", "", bredr_connectable },
    { "bredr_discoverable", "", bredr_discoverable },
    { "bredr_connect", "", bredr_connect },
    { "bredr_disconnect", "", bredr_disconnect },
    { "bredr_remote_name", "", bredr_remote_name },

#if CONFIG_BT_A2DP
    { "a2dp_connect", "", a2dp_connect },
#endif

#if CONFIG_BT_AVRCP
    { "avrcp_connect", "", avrcp_connect },
    { "avrcp_pth_key", "", avrcp_pth_key },
    { "avrcp_pth_key_act", "", avrcp_pth_key_act },
    { "avrcp_change_vol", "", avrcp_change_vol },
#endif

};

#if PCM_PRINTF
extern OI_BYTE sbc_frame[];
extern OI_UINT32 sbc_frame_len;
OI_INT16 pcm_data[1024];
OI_UINT32 pcm_bytes = sizeof(pcm_data);
OI_INT16 cool_edit[600000];
OI_UINT32 byte_index = 0;
static void pcm(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    //a2dp_sbc_decode_init();
    //a2dp_sbc_decode_process(sbc_frame, sbc_frame_len);

    printf("pcm data count: %d \n", byte_index);

    OI_UINT32 samps = byte_index / sizeof(OI_INT16);

    printf("SAMPLES:    %d\n", samps);
    printf("BITSPERSAMPLE:  16\n");
    printf("CHANNELS:   2\n");
    printf("SAMPLERATE: 44100\n");
    printf("NORMALIZED: FALSE\n");

    for (int i = 0; i < samps; i++) {
        printf("%d\n", cool_edit[i]);
    }
}
#endif

static void bredr_init(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
    if (init) {
        printf("bredr has initialized\n");
        return;
    }

    default_conn = NULL;
    bt_conn_cb_register(&conn_callbacks);
#if CONFIG_BT_A2DP
    a2dp_cb_register(&a2dp_callbacks);
#endif
#if CONFIG_BT_AVRCP
    avrcp_cb_register(&avrcp_callbacks);
#endif

    init = true;
    printf("bredr init successfully\n");
}

static void bredr_connected(struct bt_conn *conn, u8_t err)
{
    if (err || conn->type != BT_CONN_TYPE_BR) {
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

    if (!default_conn) {
        default_conn = conn;
    }

    bt_br_set_connectable(false);
}

static void bredr_disconnected(struct bt_conn *conn, u8_t reason)
{
    if (conn->type != BT_CONN_TYPE_BR) {
        return;
    }

    char addr[BT_ADDR_STR_LEN];

    bt_conn_get_info(conn, &conn_info);
    bt_addr_to_str(conn_info.br.dst, addr, sizeof(addr));

    printf("bredr disconnected: %s (reason %u) \r\n", addr, reason);

    if (default_conn == conn) {
        default_conn = NULL;
    }
}

static void bredr_write_eir(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err;
    char *name = "Bouffalolab-classic-bluetooth";
    uint8_t rec = 1;
    uint8_t data[32] = { 0 };

    data[0] = 30;
    data[1] = 0x09;
    memcpy(data + 2, name, strlen(name));

    for (int i = 0; i < strlen(name); i++) {
        printf("0x%02x ", data[2 + i]);
    }
    printf("\n");

    err = bt_br_write_eir(rec, data);
    if (err) {
        printf("BR/EDR write EIR failed, (err %d)\n", err);
    } else {
        printf("BR/EDR write EIR done.\n");
    }
}

static void bredr_discoverable(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err;
    uint8_t action;

    if (argc != 2) {
        printf("Number of parameters is not correct\n");
        return;
    }

    get_uint8_from_string(&argv[1], &action);

    if (action == 1) {
        err = bt_br_set_discoverable(true);
    } else if (action == 0) {
        err = bt_br_set_discoverable(false);
    } else {
        printf("Arg1 is invalid\n");
        return;
    }

    if (err) {
        printf("BR/EDR set discoverable failed, (err %d)\n", err);
    } else {
        printf("BR/EDR set discoverable done.\n");
    }
}

static void bredr_connectable(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err;
    uint8_t action;

    if (argc != 2) {
        printf("Number of parameters is not correct\n");
        return;
    }

    get_uint8_from_string(&argv[1], &action);

    if (action == 1) {
        err = bt_br_set_connectable(true);
    } else if (action == 0) {
        err = bt_br_set_connectable(false);
    } else {
        printf("Arg1 is invalid\n");
        return;
    }

    if (err) {
        printf("BR/EDR set connectable failed, (err %d)\n", err);
    } else {
        printf("BR/EDR set connectable done.\n");
    }
}

static void bredr_connect(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    struct bt_conn *conn;
    u8_t addr_val[6];
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

static void bredr_disconnect(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    if (!default_conn) {
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

void remote_name(const char *name)
{
    if (name) {
        printf("%s, remote name len: %d,  : %s\n", __func__, strlen(name), name);
    } else {
        printf("%s, remote name request fail\n", __func__);
    }
}

static void bredr_remote_name(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    struct bt_conn *conn;
    u8_t addr_val[6];
    bt_addr_t peer_addr;
    char addr_str[18];

    get_bytearray_from_string(&argv[1], addr_val, 6);
    reverse_bytearray(addr_val, peer_addr.val, 6);

    bt_addr_to_str(&peer_addr, addr_str, sizeof(addr_str));
    printf("%s, create bredr connection with : %s \n", __func__, addr_str);

    int err = remote_name_req(&peer_addr, remote_name);
    if (!err) {
        printf("remote name request pending.\n");
    } else {
        printf("remote name request fail.\n");
    }
}

#if CONFIG_BT_A2DP
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

static void a2dp_connect(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err;

    if (!default_conn) {
        printf("Not connected.\n");
        return;
    }

    err = bt_a2dp_connect(default_conn);
    if (err) {
        printf("a2dp connect failed, err: %d\n", err);
    } else {
        printf("a2dp connect successfully.\n");
    }
}
#endif

#if CONFIG_BT_AVRCP
static void avrcp_chain(struct bt_conn *conn, uint8_t state)
{
    printf("%s, conn: %p \n", __func__, conn);

    if (state == BT_AVRCP_CHAIN_CONNECTED) {
        printf("avrcp connected. \n");
    } else if (state == BT_AVRCP_CHAIN_DISCONNECTED) {
        printf("avrcp disconnected. \n");
    }
}

static void avrcp_absvol(uint8_t vol)
{
    printf("%s, vol: %d \n", __func__, vol);
}

static void avrcp_connect(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err;

    if (!default_conn) {
        printf("Not connected.\n");
        return;
    }

    err = bt_avrcp_connect(default_conn);
    if (err) {
        printf("avrcp connect failed, err: %d\n", err);
    } else {
        printf("avrcp connect successfully.\n");
    }
}

static void avrcp_pth_key(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err;
    uint8_t key;

    if (!default_conn) {
        printf("Not connected.\n");
        return;
    }

    get_uint8_from_string(&argv[1], &key);

    err = avrcp_pasthr_cmd(NULL, PASTHR_STATE_PRESSED, key);
    if (err) {
        printf("avrcp key pressed failed, err: %d\n", err);
    } else {
        printf("avrcp key pressed successfully.\n");
    }

    err = avrcp_pasthr_cmd(NULL, PASTHR_STATE_RELEASED, key);
    if (err) {
        printf("avrcp key released failed, err: %d\n", err);
    } else {
        printf("avrcp key play released successfully.\n");
    }
}

static void avrcp_pth_key_act(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err;
    uint8_t key;
    uint8_t action;

    if (!default_conn) {
        printf("Not connected.\n");
        return;
    }

    get_uint8_from_string(&argv[1], &key);
    get_uint8_from_string(&argv[2], &action);

    if (action != PASTHR_STATE_PRESSED && action != PASTHR_STATE_RELEASED) {
        printf("invalid key action.\n");
        return;
    }

    err = avrcp_pasthr_cmd(NULL, action, key);
    if (err) {
        printf("avrcp key action failed, err: %d\n", err);
    } else {
        printf("avrcp %s key %d successfully.\n", action ? "released" : "pressed", key);
    }
}

static void avrcp_change_vol(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err;
    uint8_t vol;
    if (!default_conn) {
        printf("Not connected.\n");
        return;
    }

    get_uint8_from_string(&argv[1], &vol);
    err = avrcp_change_volume(vol);
    if (err) {
        printf("avrcp change volume fail\n");
    }
}

#endif

int bredr_cli_register(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //aos_cli_register_commands(bredr_cmd_set, sizeof(bredr_cmd_set)/sizeof(bredr_cmd_set[0]));
    return 0;
}
