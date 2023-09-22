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
#include <conn.h>
#include <conn_internal.h>
#include <hci_core.h>
#include <l2cap.h>
#include <l2cap_internal.h>
#if CONFIG_BT_A2DP
#include <a2dp.h>
#endif
#if CONFIG_BT_AVRCP
#include <avrcp.h>
#endif
#if CONFIG_BT_AVRCP
//#include <rfcomm.h>
#endif
#if CONFIG_BT_HFP
#include <hfp_hf.h>
#endif

#if defined(CONFIG_SHELL)
#include "shell.h"
#else
#include "cli.h"
#endif /* CONFIG_SHELL */

#if PCM_PRINTF
#include "oi_codec_sbc.h"
#endif

#include "bt_log.h"

#if defined(CONFIG_SHELL)
#define BT_CLI(func) static void bredr_##func(int argc, char **argv)
#define BT_A2DP_CLI(func) static void a2dp_##func(int argc, char **argv)
#define BT_AVRCP_CLI(func) static void avrcp_##func(int argc, char **argv)
#define BT_HFP_CLI(func) static void hfp_##func(int argc, char **argv)
#else
#define BT_CLI(func) static void bredr_##func(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
#define BT_A2DP_CLI(func) static void a2dp_##func(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
#define BT_AVRCP_CLI(func) static void avrcp_##func(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
#define BT_HFP_CLI(func) static void hfp_##func(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
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

static struct a2dp_callback a2dp_callbacks =
{
    .chain = a2dp_chain,
    .stream = a2dp_stream,
};
#endif

#if CONFIG_BT_AVRCP
static void avrcp_chain(struct bt_conn *conn, uint8_t state);
static void avrcp_absvol(uint8_t vol);
static void avrcp_play_status(uint32_t song_len, uint32_t song_pos, uint8_t status);

static struct avrcp_callback avrcp_callbacks =
{
    .chain = avrcp_chain,
    .abs_vol = avrcp_absvol,
    .play_status = avrcp_play_status,
};
#endif

#if PCM_PRINTF
static void pcm(char *p_write_buffer, int write_buffer_len, int argc, char **argv);
#endif
BT_CLI(init);
BT_CLI(write_local_name);
BT_CLI(write_eir);
BT_CLI(discoverable);
BT_CLI(connectable);
BT_CLI(connect);
BT_CLI(disconnect);
BT_CLI(remote_name);
BT_CLI(l2cap_send_test_data);
BT_CLI(l2cap_disconnect);
BT_CLI(l2cap_echo_req);
BT_CLI(security);

#if BR_EDR_PTS_TEST
BT_CLI(sdp_client_connect);
#endif

#if CONFIG_BT_A2DP
BT_A2DP_CLI(connect);
#if BR_EDR_PTS_TEST
BT_A2DP_CLI(disconnect);
BT_A2DP_CLI(start_discovery);
BT_A2DP_CLI(get_cap);
BT_A2DP_CLI(set_conf);
BT_A2DP_CLI(close_stream);
BT_A2DP_CLI(open_stream);
BT_A2DP_CLI(start_stream);
BT_A2DP_CLI(suspend_stream);
BT_A2DP_CLI(delay_report);
BT_A2DP_CLI(set_conf_reject);
#endif
#endif

#if CONFIG_BT_AVRCP
BT_AVRCP_CLI(connect);
BT_AVRCP_CLI(pth_key);
BT_AVRCP_CLI(pth_key_act);
BT_AVRCP_CLI(change_vol);
BT_AVRCP_CLI(get_play_status);
#endif

#if CONFIG_BT_HFP
BT_HFP_CLI(connect);
BT_HFP_CLI(hf_disconnect);
BT_HFP_CLI(sco_connect);
BT_HFP_CLI(answer);
BT_HFP_CLI(terminate_call);
BT_HFP_CLI(outgoint_call);
BT_HFP_CLI(outgoint_call_with_mem_loc);
BT_HFP_CLI(outgoint_call_last_number_dialed);
BT_HFP_CLI(disable_nrec);
BT_HFP_CLI(voice_recognition);
BT_HFP_CLI(voice_req_phone_num);
BT_HFP_CLI(accept_incoming_caller);
BT_HFP_CLI(set_mic_volume);
BT_HFP_CLI(set_speaker_volume);
BT_HFP_CLI(query_list_calls);
BT_HFP_CLI(response_call);
BT_HFP_CLI(subscriber_number_info);
BT_HFP_CLI(hf_send_indicator);
BT_HFP_CLI(hf_update_indicator);
#endif

#if defined(CONFIG_SHELL)
    SHELL_CMD_EXPORT_ALIAS(bredr_init,bredr_init,BREDR Initialize Parameter:[Null]);
    SHELL_CMD_EXPORT_ALIAS(bredr_write_local_name,bredr_name,bredr_name Parameter:[name]);
    SHELL_CMD_EXPORT_ALIAS(bredr_write_eir,bredr_eir,bredr_eir Parameter:[Null]);
    SHELL_CMD_EXPORT_ALIAS(bredr_connectable,bredr_connectable,
                            bredr_connectable Parameter:[1:enable 0:disable]);
    SHELL_CMD_EXPORT_ALIAS(bredr_discoverable,bredr_discoverable,
                            bredr_discoverable Parameter:[1:enable 0:disable]);
    SHELL_CMD_EXPORT_ALIAS(bredr_connect,bredr_connect,
                            bredr_connect Parameter:[address eg.18bf591245]);
    SHELL_CMD_EXPORT_ALIAS(bredr_disconnect,bredr_disconnect,
                            bredr_disconnect Parameter:[address eg.18bf591245]);
    SHELL_CMD_EXPORT_ALIAS(bredr_remote_name,bredr_remote_name,
                            bredr_remote_name Parameter:[Null]);
    SHELL_CMD_EXPORT_ALIAS(bredr_l2cap_send_test_data,bredr_l2cap_send_test_data,"");
    SHELL_CMD_EXPORT_ALIAS(bredr_l2cap_disconnect,bredr_l2cap_disconnect_req,"");
    SHELL_CMD_EXPORT_ALIAS(bredr_security,bredr_security,"");
    #if BR_EDR_PTS_TEST
    SHELL_CMD_EXPORT_ALIAS(bredr_sdp_client_connect,bredr_sdp_client_connect,"");
    #endif
    #if CONFIG_BT_A2DP
    SHELL_CMD_EXPORT_ALIAS(a2dp_connect,a2dp_connect,"");
    #if BR_EDR_PTS_TEST
    SHELL_CMD_EXPORT_ALIAS(a2dp_disconnect,a2dp_disconnect,"");
    SHELL_CMD_EXPORT_ALIAS(a2dp_start_discovery,a2dp_start_discovery,"");
    SHELL_CMD_EXPORT_ALIAS(a2dp_get_cap,a2dp_get_cap,"");
    SHELL_CMD_EXPORT_ALIAS(a2dp_set_conf,a2dp_set_conf,"");
    SHELL_CMD_EXPORT_ALIAS(a2dp_open_stream,a2dp_open_stream,"");
    SHELL_CMD_EXPORT_ALIAS(a2dp_close_stream,a2dp_close_stream,"");
    SHELL_CMD_EXPORT_ALIAS(a2dp_start_stream,a2dp_start_stream,"");
    SHELL_CMD_EXPORT_ALIAS(a2dp_suspend_stream,a2dp_suspend_stream,"");
    SHELL_CMD_EXPORT_ALIAS(a2dp_delay_report,a2dp_delay_report,"");
    SHELL_CMD_EXPORT_ALIAS(avdtp_set_conf_reject,avdtp_set_conf_reject,"");
    #endif
    #endif

    #if CONFIG_BT_AVRCP
    SHELL_CMD_EXPORT_ALIAS(avrcp_connect,avrcp_connect,"");
    SHELL_CMD_EXPORT_ALIAS(avrcp_pth_key,avrcp_pth_key,"");
    SHELL_CMD_EXPORT_ALIAS(avrcp_pth_key_act,avrcp_pth_key_act,"");
    SHELL_CMD_EXPORT_ALIAS(avrcp_change_vol,avrcp_change_vol,"");
    SHELL_CMD_EXPORT_ALIAS(avrcp_get_play_status,avrcp_get_play_status,"");
    #endif

    #if CONFIG_BT_HFP
    SHELL_CMD_EXPORT_ALIAS(hfp_connect,hfp_connect,"");
    SHELL_CMD_EXPORT_ALIAS(hfp_hf_disconnect,hfp_hf_disconnect,"");
    SHELL_CMD_EXPORT_ALIAS(hfp_sco_connect,hfp_sco_connect,"");
    SHELL_CMD_EXPORT_ALIAS(hfp_answer,hfp_answer,"");
     SHELL_CMD_EXPORT_ALIAS(hfp_terminate_call,hfp_terminate_call,"");
    SHELL_CMD_EXPORT_ALIAS(hfp_outgoint_call,hfp_outgoint_call,"");
    SHELL_CMD_EXPORT_ALIAS(hfp_outgoint_call_with_mem_loc,hfp_outgoint_call_with_mem_loc,"");
    SHELL_CMD_EXPORT_ALIAS(hfp_outgoint_call_last_number_dialed,hfp_outgoint_call_last_number_dialed,"");
    SHELL_CMD_EXPORT_ALIAS(hfp_disable_nrec,hfp_disable_nrec,"");
    SHELL_CMD_EXPORT_ALIAS(hfp_voice_recognition,hfp_voice_recognition,"");
    SHELL_CMD_EXPORT_ALIAS(hfp_voice_req_phone_num,hfp_voice_req_phone_num,"");
    SHELL_CMD_EXPORT_ALIAS(hfp_accept_incoming_caller,hfp_accept_incoming_caller,"");
    SHELL_CMD_EXPORT_ALIAS(hfp_set_mic_volume,hfp_set_mic_volume,"");
    SHELL_CMD_EXPORT_ALIAS(hfp_set_speaker_volume,hfp_set_speaker_volume,"");
    SHELL_CMD_EXPORT_ALIAS(hfp_query_list_calls,hfp_query_list_calls,"");
    SHELL_CMD_EXPORT_ALIAS(hfp_response_call,hfp_response_call,"");
    SHELL_CMD_EXPORT_ALIAS(hfp_subscriber_number_info,hfp_subscriber_number_info,"");
    SHELL_CMD_EXPORT_ALIAS(hfp_hf_send_indicator,hfp_hf_send_indicator,"");
    SHELL_CMD_EXPORT_ALIAS(hfp_hf_update_indicator,hfp_hf_update_indicator,"");
    #endif

#else
const struct cli_command bredr_cmd_set[] STATIC_CLI_CMD_ATTRIBUTE = {
    #if PCM_PRINTF
    {"pcm", "", pcm},
    #endif
    {"bredr_init", "", bredr_init},
    {"bredr_name", "", bredr_write_local_name},
    {"bredr_eir", "", bredr_write_eir},
    {"bredr_connectable", "", bredr_connectable},
    {"bredr_discoverable", "", bredr_discoverable},
    {"bredr_connect", "", bredr_connect},
    {"bredr_disconnect", "", bredr_disconnect},
    {"bredr_remote_name", "", bredr_remote_name},
    {"bredr_l2cap_send_test_data", "", bredr_l2cap_send_test_data},
    {"bredr_l2cap_disconnect_req", "", bredr_l2cap_disconnect},
    {"bredr_l2cap_echo_req", "", bredr_l2cap_echo_req},
    {"bredr_security", "", bredr_security},
    #if BR_EDR_PTS_TEST
    {"bredr_sdp_client_connect", "", bredr_sdp_client_connect},
    #endif
        
    #if CONFIG_BT_A2DP
    {"a2dp_connect", "", a2dp_connect},
    #if BR_EDR_PTS_TEST
    {"a2dp_disconnect", "", a2dp_disconnect},
    {"a2dp_start_disc", "", a2dp_start_discovery},
    {"a2dp_get_cap", "", a2dp_get_cap},
    {"a2dp_set_conf", "", a2dp_set_conf},
    {"a2dp_open_stream", "", a2dp_open_stream},
    {"a2dp_close_stream", "", a2dp_close_stream},
    {"a2dp_start_stream", "", a2dp_start_stream},
    {"a2dp_suspend_stream", "", a2dp_suspend_stream},
    {"a2dp_delay_report","",a2dp_delay_report},
    {"avdtp_set_conf_reject", "", avdtp_set_conf_reject},
    #endif
    #endif
    #if CONFIG_BT_AVRCP
    {"avrcp_connect", "", avrcp_connect},
    {"avrcp_pth_key", "", avrcp_pth_key},
    {"avrcp_pth_key_act", "", avrcp_pth_key_act},
    {"avrcp_change_vol", "", avrcp_change_vol},
    {"avrcp_play_status", "", avrcp_get_play_status},
    #endif

    #if CONFIG_BT_HFP
    {"hfp_connect", "", hfp_connect},
    {"hfp_diconnect","",hfp_hf_disconnect},
    {"sco_connect", "", hfp_sco_connect},
    {"hfp_answer", "", hfp_answer},
    {"hfp_terminate_call", "", hfp_terminate_call},
    {"hfp_outgoing_call", "", hfp_outgoint_call},
    {"hfp_outgoint_call_with_mem_loc", "", hfp_outgoint_call_with_mem_loc},
    {"hfp_outgoint_call_last_number_dialed", "", hfp_outgoint_call_last_number_dialed},
    {"hfp_disable_nrec", "", hfp_disable_nrec},
    {"hfp_voice_recognition", "", hfp_voice_recognition},
    {"hfp_voice_req_phone_num", "", hfp_voice_req_phone_num},
    {"hfp_accept_incoming_caller", "", hfp_accept_incoming_caller},
    {"hfp_set_mic_vol", "", hfp_set_mic_volume},
    {"hfp_set_spk_vol", "", hfp_set_speaker_volume},
    {"hfp_query_list_calls","",hfp_query_list_calls},
    {"hfp_response_call","",hfp_response_call},
    {"hfp_subs_num_info","",hfp_subscriber_number_info},
    {"hfp_hf_send_ind","",hfp_hf_send_indicator},
    {"hfp_hf_update_ind","",hfp_hf_update_indicator},
    #endif
};
#endif /* CONFIG_SHELL */

#if PCM_PRINTF
extern OI_BYTE sbc_frame[];
extern OI_UINT32 sbc_frame_len;
OI_INT16 pcm_data[1024];
OI_UINT32 pcm_bytes = sizeof(pcm_data);
OI_INT16 cool_edit[600000];
OI_UINT32 byte_index = 0;
static void pcm(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    printf("pcm data count: %d \n", byte_index);

    OI_UINT32 samps = byte_index / sizeof(OI_INT16);

    printf("SAMPLES:    %d\n", samps);
    printf("BITSPERSAMPLE:  16\n");
    printf("CHANNELS:   2\n");
    printf("SAMPLERATE: 44100\n");
    printf("NORMALIZED: FALSE\n");

    for (int i = 0; i < samps; i ++) {
        printf("%d\n", cool_edit[i]);
    }
}
#endif

BT_CLI(init)
{
    if(init){
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
    if(err || conn->type != BT_CONN_TYPE_BR)
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

    bt_br_set_connectable(false);

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

BT_CLI(write_local_name)
{
    int err;
    char *name = "BL-BT";

    err = bt_br_write_local_name(name);
    if (err) {
        printf("BR/EDR write local name failed, (err %d)\n", err);
    } else {
        printf("BR/EDR write local name done.\n");
    }
}

BT_CLI(write_eir)
{
    int err;
    char *name = "Bouffalolab-classic-bluetooth";
    uint8_t fec = 1;
    uint8_t data[32] = {0};

    data[0] = 30;
    data[1] = 0x09;
    memcpy(data+2, name, strlen(name));

    for(int i = 0; i < strlen(name); i++)
    {
        printf("0x%02x ", data[2+i]);
    }
    printf("\n");

    err = bt_br_write_eir(fec, data);
    if (err) {
        printf("BR/EDR write EIR failed, (err %d)\n", err);
    } else {
        printf("BR/EDR write EIR done.\n");
    }
}

BT_CLI(discoverable)
{
    int err;
    uint8_t action;
    
    if(argc != 2){
        printf("Number of parameters is not correct\n");
        return;
    }

    btble_get_uint8_from_string(&argv[1], &action);
    
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

BT_CLI(connectable)
{
    int err;
    uint8_t action;
    
    if(argc != 2){
        printf("Number of parameters is not correct\n");
        return;
    }

    btble_get_uint8_from_string(&argv[1], &action);
    
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

BT_CLI(connect)
{
    struct bt_conn *conn;
    u8_t  addr_val[6];
    bt_addr_t peer_addr;
    struct bt_br_conn_param param;
    char addr_str[18];

    btble_get_bytearray_from_string(&argv[1], addr_val, 6);
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

void remote_name(const char *name)
{
    if (name) {
        printf("%s, remote name len: %d,  : %s\n", __func__, strlen(name), name);
    } else {
        printf("%s, remote name request fail\n", __func__);
    }
}

BT_CLI(remote_name)
{
    u8_t  addr_val[6];
    bt_addr_t peer_addr;
    char addr_str[18];

    btble_get_bytearray_from_string(&argv[1], addr_val, 6);
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

BT_CLI(l2cap_send_test_data)
{
    int err = 0;
    uint8_t test_data[10]={0x01, 0x02, 0x3,0x04,0x05,0x06,0x07,0x08,0x09,0xa0};
    uint16_t cid;

    btble_get_uint16_from_string(&argv[1], &cid);

    extern int bt_l2cap_send_data(struct bt_conn *conn, uint16_t tx_cid, uint8_t *data, uint8_t len);
    err = bt_l2cap_send_data(default_conn, cid, test_data, 10);

    if(err)
        printf("Fail to send l2cap test data with error (%d)\r\n", err);
    else
        printf("Send l2cap test data successfully\r\n");
}

BT_CLI(l2cap_disconnect)
{
    int err = 0;
    uint16_t tx_cid;
    
    btble_get_uint16_from_string(&argv[1], &tx_cid);

    err = bt_l2cap_disconnect(default_conn, tx_cid);

    if(err)
        printf("Fail to send l2cap disconnect request with error (%d)\r\n", err);
    else
        printf("Send l2cap disconnect request successfully\r\n");
}

BT_CLI(l2cap_echo_req)
{
    int err = 0;

    err = bt_l2cap_br_echo_req(default_conn);
    if (err)
        printf("Fail to send l2cap echo request with error (%d)\r\n", err);
    else
        printf("Send l2cap echo request successfully\r\n");
}

BT_CLI(security)
{
    int err;
    u8_t sec_level = BT_SECURITY_L2;

    if(!default_conn){
        printf("Not connected\r\n");
        return;
    }

    if(argc == 2)
        btble_get_uint8_from_string(&argv[1], &sec_level);
    
    err = bt_conn_set_security(default_conn, sec_level);

    if(err){
        printf("Failed to start security, (err %d) \r\n", err);
    }else{
        printf("Start security successfully\r\n");
    }
}

#if BR_EDR_PTS_TEST
BT_CLI(sdp_client_connect)
{
    extern int bt_sdp_client_connect(struct bt_conn *conn);
    bt_sdp_client_connect(default_conn);
}
#endif

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

BT_A2DP_CLI(connect)
{
    int ret;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    ret = bt_a2dp_connect(default_conn);
    if(ret) {
        printf("a2dp connect successfully.\n");
    } else {
        printf("a2dp connect fail. \n");
    }
}

#if BR_EDR_PTS_TEST
BT_A2DP_CLI(disconnect)
{
    int ret;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    ret = bt_a2dp_disconnect(default_conn);
    if(ret) {
        printf("A2dp disconnect successfully.\n");
    } else {
        printf("A2dp disconnect fail. ret(%d)\n",ret);
    }
}

BT_A2DP_CLI(start_discovery)
{
    int ret;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    ret = bt_a2dp_start_discovery(default_conn);
    if(ret) {
        printf("A2dp start discovery successfully.\n");
    } else {
        printf("A2dp start discovery fail. ret(%d)\n",ret);
    }
}

BT_A2DP_CLI(get_cap)
{
    int ret;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    ret = bt_a2dp_get_cap(default_conn);
    if(ret) {
        printf("A2dp start discovery successfully.\n");
    } else {
        printf("A2dp start discovery fail. ret(%d)\n",ret);
    }
}

BT_A2DP_CLI(set_conf)
{
    int ret;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    ret = bt_a2dp_set_conf(default_conn);
    if(ret) {
        printf("A2dp start discovery successfully.\n");
    } else {
        printf("A2dp start discovery fail. ret(%d)\n",ret);
    }
}

BT_A2DP_CLI(close_stream)
{
    int ret;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    ret = bt_a2dp_close_stream(default_conn);
    if(ret) {
        printf("A2dp disconnect successfully.\n");
    } else {
        printf("A2dp disconnect fail. ret(%d)\n",ret);
    }
}

BT_A2DP_CLI(open_stream)
{
    int ret;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    ret = bt_a2dp_open_stream(default_conn);
    if(ret) {
        printf("A2dp disconnect successfully.\n");
    } else {
        printf("A2dp disconnect fail. ret(%d)\n",ret);
    }
}

BT_A2DP_CLI(start_stream)
{
    int ret;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    ret = bt_a2dp_start_stream(default_conn);
    if(ret) {
        printf("A2dp disconnect successfully.\n");
    } else {
        printf("A2dp disconnect fail. ret(%d)\n",ret);
    }
}

BT_A2DP_CLI(suspend_stream)
{
    int ret;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    ret = bt_a2dp_suspend_stream(default_conn);
    if(ret) {
        printf("A2dp disconnect successfully.\n");
    } else {
        printf("A2dp disconnect fail. ret(%d)\n",ret);
    }
}

BT_A2DP_CLI(delay_report)
{
    int ret;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    ret = bt_a2dp_delay_report(default_conn);
    if(ret) {
        printf("A2dp disconnect successfully.\n");
    } else {
        printf("A2dp disconnect fail. ret(%d)\n",ret);
    }
}

extern uint8_t reject_set_conf_pts;
extern uint8_t reject_error_code;
static void avdtp_set_conf_reject(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    btble_get_uint8_from_string(&argv[1], &reject_set_conf_pts);
    btble_get_uint8_from_string(&argv[2], &reject_error_code);
}

#endif
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

static void avrcp_play_status(uint32_t song_len, uint32_t song_pos, uint8_t status)
{
    printf("%s, song length: %d, song position: %d, play status: %d \n", __func__, song_len, song_pos, status);
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

    btble_get_uint8_from_string(&argv[1], &key);

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

    btble_get_uint8_from_string(&argv[1], &key);
    btble_get_uint8_from_string(&argv[2], &action);

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

    btble_get_uint8_from_string(&argv[1], &vol);
    err = avrcp_change_volume(vol);
    if (err) {
        printf("avrcp change volume fail\n");
    } else {
        printf("avrcp change volume success\n");
    }
}

BT_AVRCP_CLI(get_play_status)
{
    int err;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    err = avrcp_get_play_status_cmd(NULL);
    if(err) {
        printf("avrcp get play status fail\n");
    } else {
        printf("avrcp get play status success\n");
    }
}

#endif

#if CONFIG_BT_HFP
#if 0
static void rfcomm_recv(struct bt_rfcomm_dlc *dlci, struct net_buf *buf)
{
	printf("hfp incoming data dlc %p len %u \n", dlci, buf->len);
}

static void rfcomm_connected(struct bt_rfcomm_dlc *dlci)
{
	printf("hfp dlc %p connected \n", dlci);
}

static void rfcomm_disconnected(struct bt_rfcomm_dlc *dlci)
{
	printf("hfp dlc %p disconnected \n", dlci);
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
#endif

BT_HFP_CLI(connect)
{
    int err;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    err = bt_hfp_hf_initiate_connect(default_conn);
    if (err) {
        printf("hfp connect fail.\n");
    } else {
        printf("hfp connect pending.\n");
    }
}

BT_HFP_CLI(sco_connect)
{
    struct bt_conn *conn = NULL;
    uint8_t sco_type;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    btble_get_uint8_from_string(&argv[1], &sco_type);

    if(sco_type == 1){
        conn = bt_conn_create_sco(&default_conn->br.dst,ESCO_PARAM_S1);
    }else if(sco_type == 2){
        conn = bt_conn_create_sco(&default_conn->br.dst,ESCO_PARAM_S2);
    }else if(sco_type == 3){
        conn = bt_conn_create_sco(&default_conn->br.dst,ESCO_PARAM_S3);
    }else if(sco_type == 4){
        conn = bt_conn_create_sco(&default_conn->br.dst,ESCO_PARAM_S4);
    }else if(sco_type == 5){
        conn = bt_conn_create_sco(&default_conn->br.dst,SCO_PARAM_D0);
    }else if(sco_type == 6){
        conn = bt_conn_create_sco(&default_conn->br.dst,SCO_PARAM_D1);
    }else{
        printf("Invaild type %d\r\n",sco_type);
    }

    if (!conn) {
        printf("sco connect fail.\n");
    } else {
        printf("sco connect success.\n");
    }
}

BT_HFP_CLI(answer)
//static void hfp_answer(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err = 0;
    
    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    err = bt_hfp_hf_send_cmd(default_conn, BT_HFP_HF_ATA, NULL);
    if(err)
        printf("Fail to send answer AT command with err:%d\r\n", err);
    else
        printf("send answer AT command successfully\r\n");
        
}

BT_HFP_CLI(terminate_call)
//static void hfp_terminate_call(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err = 0;
    
    if(!default_conn){
            printf("Not connected.\n");
            return;
    }

    err = bt_hfp_hf_send_cmd(default_conn, BT_HFP_HF_AT_CHUP, NULL);
    if(err)
        printf("Fail to send terminate call AT command with err:%d\r\n", err);
    else
        printf("send terminate call AT command successfully\r\n");
        
}

BT_HFP_CLI(outgoint_call)
//static void hfp_outgoint_call(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err = 0;
    
    if(!default_conn){
            printf("Not connected.\n");
            return;
    }

    err = bt_hfp_hf_send_cmd(default_conn, BT_HFP_HF_AT_DDD, "D1234567;");
    if(err)
        printf("Fail to send outgoing call AT command with err:%d\r\n", err);
    else
        printf("send outgoing call AT command successfully\r\n");
        
}

BT_HFP_CLI(outgoint_call_with_mem_loc)
//static void hfp_outgoint_call_with_mem_loc(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err = 0;
    uint8_t phone_mem_loc = 0;
    char str[5] = "D>";
    
    if(!default_conn){
            printf("Not connected.\n");
            return;
    }

    btble_get_uint8_from_string(&argv[1], &phone_mem_loc);
    sprintf(str, ">%d;", phone_mem_loc);
    err = bt_hfp_hf_send_cmd(default_conn, BT_HFP_HF_AT_DDD, str);
    
    if(err)
        printf("Fail to send outgoing call with memory location AT command with err:%d\r\n", err);
    else
        printf("send outgoing call with memory location AT command successfully\r\n");
        
}

BT_HFP_CLI(outgoint_call_last_number_dialed)
{
    int err = 0;
    char *str = "+BLDN";
    
    if(!default_conn){
            printf("Not connected.\n");
            return;
    }

    err = bt_hfp_hf_send_cmd(default_conn, BT_HFP_HF_AT_DDD, str);
    
    if(err)
        printf("Fail to send outgoing call to the last number dialed AT command with err:%d\r\n", err);
    else
        printf("send outgoing call to the last number dialed  AT command successfully\r\n");
        
}

BT_HFP_CLI(disable_nrec)
{
    int err = 0;
    
    if(!default_conn){
            printf("Not connected.\n");
            return;
    }

    err = bt_hfp_hf_send_cmd(default_conn, BT_HFP_HF_AT_NREC, NULL);
    if(err)
        printf("Fail to send disable nrec AT command with err:%d\r\n", err);
    else
        printf("send disable nrec AT command successfully\r\n");
        
}

BT_HFP_CLI(voice_recognition)
//static void hfp_voice_recognition(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err = 0;
    uint8_t enable = 0;

    btble_get_uint8_from_string(&argv[1], &enable);

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    err = bt_hfp_hf_send_cmd_arg(default_conn, BT_HFP_HF_AT_BVRA, enable);
    if(err)
        printf("Fail to send voice recognition AT command with err:%d\r\n", err);
    else
        printf("send voice recognition AT command successfully\r\n");
}

BT_HFP_CLI(voice_req_phone_num)
//static void hfp_voice_req_phone_num(char *p_write_buffer, int write_buffer_len, int argc, char **argv)
{
    int err = 0;
    uint8_t enable = 0;

    btble_get_uint8_from_string(&argv[1], &enable);

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    err = bt_hfp_hf_send_cmd_arg(default_conn, BT_HFP_HF_AT_BINP, enable);
    if(err)
        printf("Fail to send reqeust phone number to the AG AT command with err:%d\r\n", err);
    else
        printf("send reqeust phone number to the AG AT command successfully\r\n");
}

BT_HFP_CLI(accept_incoming_caller)
{
    int err = 0;
    uint8_t call_id = 0;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    btble_get_uint8_from_string(&argv[1], &call_id);

    err = bt_hfp_hf_send_cmd_arg(default_conn, BT_HFP_ACCEPT_INCOMING_CALLER_ID, call_id);
    if(err)
        printf("Fail to accept a incoming call err:%d\r\n", err);
    else
        printf("Accept a incoming call successfully\r\n");
}

BT_HFP_CLI(set_mic_volume)
{
    int err = 0;
    uint8_t vol = 0;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    btble_get_uint8_from_string(&argv[1], &vol);

    if(vol > 15 ){
        printf("Volume out of range %d\n",vol);
        return;
    }

    err = bt_hfp_hf_send_cmd_arg(default_conn, BT_HFP_SET_MIC_VOL,vol);
    if(err)
        printf("Fail to set mic volume err:%d\r\n", err);
    else
        printf("Set mic volume successfully\r\n");
}

BT_HFP_CLI(set_speaker_volume)
{
    int err = 0;
    uint8_t vol = 0;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    btble_get_uint8_from_string(&argv[1], &vol);

    if(vol > 15 ){
        printf("Volume out of range %d\n",vol);
        return;
    }

    err = bt_hfp_hf_send_cmd_arg(default_conn, BT_HFP_HF_AT_VGS,vol);
    if(err)
        printf("Fail to set speaker volume err:%d\r\n", err);
    else
        printf("Set speaker volume successfully\r\n");
}

BT_HFP_CLI(query_list_calls)
{
    int err = 0;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    err = bt_hfp_hf_send_cmd(default_conn, BT_HFP_QUERY_LIST_CALLS,NULL);
    if(err)
        printf("Fail to query the list calls err:%d\r\n", err);
    else
        printf("Query the list calls successfully\r\n");
}

BT_HFP_CLI(response_call)
{
    int err = 0;
    uint8_t method = 0;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    btble_get_uint8_from_string(&argv[1], &method);
    if(method > 2){
        printf("Unexception %d\n",method);
        return;
    }
    err = bt_hfp_hf_send_cmd_arg(default_conn, BT_HFP_RESPONSE_CALLS,method);
    if(err)
        printf("Fail to response a call err:%d\r\n", err);
    else
        printf("Response a call successfully\r\n");
}

BT_HFP_CLI(subscriber_number_info)
{
    int err = 0;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    err = bt_hfp_hf_send_cmd(default_conn, BT_HFP_SUBSCRIBE_NUM_INFO,NULL);
    if(err)
        printf("Fail to response a call err:%d\r\n", err);
    else
        printf("Response a call successfully\r\n");
}

BT_HFP_CLI(hf_send_indicator)
{
    int err = 0;
    uint16_t id = 0;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    btble_get_uint16_from_string(&argv[1], &id);

    err = bt_hfp_hf_send_cmd_arg(default_conn, BT_HFP_SEND_INDICATOR,id);
    if(err)
        printf("Fail to send a indicator err:%d\r\n", err);
    else
        printf("Send a indicato successfully\r\n");
}
// 1,Enhanced safety : the value is 0 indicate disable it or 1 enable
// 2,Battery Level : 0-100
BT_HFP_CLI(hf_update_indicator)
{
    uint16_t val = 0;
    int err = 0;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    btble_get_uint16_from_string(&argv[1], &val);

    err = bt_hfp_hf_send_cmd_arg(default_conn, BT_HFP_UPDATE_INDICATOR,val);
    if(err)
        printf("Fail to update indicator err:%d\r\n", err);
    else
        printf("Update indicator successfully\r\n");
}

BT_HFP_CLI(hf_disconnect)
{
    int err = 0;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    err = bt_hfp_hf_send_disconnect(default_conn);
    if(err){
        printf("hf disconnect fail %d",err);
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
