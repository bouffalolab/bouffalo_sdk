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
#if CONFIG_BT_SPP
#include <spp.h>
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

#include "a2dp_source_audio.h"
#include "keys.h"

#if defined(CONFIG_SHELL)
#define BT_CLI(func) static void bredr_##func(int argc, char **argv)
#define BT_A2DP_CLI(func) static void a2dp_##func(int argc, char **argv)
#define BT_AVRCP_CLI(func) static void avrcp_##func(int argc, char **argv)
#define BT_HFP_CLI(func) static void hfp_##func(int argc, char **argv)
#define BT_AVDTP_CLI(func) static void avdtp_##func(int argc, char **argv)
#define BT_SPP_CLI(func) static void spp_##func(int argc, char **argv)
#else
#define BT_CLI(func) static void bredr_##func(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
#define BT_A2DP_CLI(func) static void a2dp_##func(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
#define BT_AVRCP_CLI(func) static void avrcp_##func(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
#define BT_HFP_CLI(func) static void hfp_##func(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
#endif
#define 		PASSKEY_MAX  		0xF423F
struct bt_br_discovery_result result[10] = { 0 };

static void bredr_connected(struct bt_conn *conn, u8_t err);
static void bredr_disconnected(struct bt_conn *conn, u8_t reason);

static bool init = false;
static struct bt_conn_info conn_info;
static struct bt_conn *default_conn = NULL;

static struct bt_conn_cb conn_callbacks = {
    .connected = bredr_connected,
    .disconnected = bredr_disconnected,
};
#if CONFIG_BT_SPP

static void bt_recv_callback(u8_t *data, u16_t length)
{
    BT_WARN("len %u data %s",length,bt_hex(data,length));
};
static void bt_spp_connected(void)
{
    BT_WARN("");
};
static void bt_spp_disconnected(void)
{
    BT_WARN("");
};

struct spp_callback_t spp_conn_callbacks={
    .connected=bt_spp_connected,
    .disconnected=bt_spp_disconnected,
    .bt_spp_recv=bt_recv_callback,
};
#endif
#if CONFIG_BT_A2DP
static void a2dp_chain(struct bt_conn *conn, uint8_t state);
static void a2dp_stream(uint8_t state);
#if CONFIG_BT_A2DP_SOURCE
struct k_thread media_transport;
static void a2dp_start_cfm(void);
#endif
static struct a2dp_callback a2dp_callbacks =
{
    .chain = a2dp_chain,
    .stream = a2dp_stream,
#if CONFIG_BT_A2DP_SOURCE
    .start_cfm = a2dp_start_cfm,
#endif
};
#endif

#if CONFIG_BT_AVRCP
static void avrcp_chain(struct bt_conn *conn, uint8_t state);
static void avrcp_absvol(uint8_t vol);
static void avrcp_play_status(uint32_t song_len, uint32_t song_pos, uint8_t status);
static void avrcp_passthrough_handler(uint8_t released, u8_t option_id);
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
    .rp_passthrough = NULL,
    .passthrough_handler = avrcp_passthrough_handler,
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
BT_CLI(auth);
BT_CLI(unpair);
BT_CLI(auth_cancel);
BT_CLI(auth_passkey_confirm);
BT_CLI(auth_pairing_confirm);
BT_CLI(auth_passkey);
BT_CLI(get_bond_list);
BT_CLI(start_inquiry);
BT_CLI(stop_inquiry);
BT_CLI(set_min_enc_key_size);
BT_CLI(set_tx_pwr);

#if BR_EDR_PTS_TEST
BT_CLI(sdp_client_connect);
BT_AVDTP_CLI(set_conf_reject);
#endif

#if CONFIG_BT_A2DP
BT_A2DP_CLI(connect);
#if CONFIG_BT_A2DP_SOURCE
BT_A2DP_CLI(discovery);
BT_A2DP_CLI(suspend);
BT_A2DP_CLI(resume);
#endif
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
BT_AVRCP_CLI(set_vol);
BT_AVRCP_CLI(get_vol);
BT_AVRCP_CLI(send_play_status);

#endif

#if CONFIG_BT_HFP
#if BR_EDR_PTS_TEST
BT_CLI(rfcomm_test_mode);
#endif
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

#if CONFIG_BT_SPP
BT_SPP_CLI(send);
BT_SPP_CLI(disconnect);
BT_SPP_CLI(connect);
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
    SHELL_CMD_EXPORT_ALIAS(bredr_set_min_enc_key_size,bredr_set_min_enc_key_size, brder_set_min_enc_key_size Parameter:[key size]);
    SHELL_CMD_EXPORT_ALIAS(bredr_set_tx_pwr,bredr_set_tx_pwr, bredr_set_tx_pwr Parameter:[br_power:1 octet; value:0~10; edr_power:1 octet; value:0~8;0xff:use default power;]);
    SHELL_CMD_EXPORT_ALIAS(bredr_l2cap_send_test_data,bredr_l2cap_send_test_data,"");
    SHELL_CMD_EXPORT_ALIAS(bredr_l2cap_echo_req,bredr_l2cap_echo_req,"");
    SHELL_CMD_EXPORT_ALIAS(bredr_l2cap_disconnect,bredr_l2cap_disconnect_req,"");
    SHELL_CMD_EXPORT_ALIAS(bredr_security,bredr_security,"");
    SHELL_CMD_EXPORT_ALIAS(bredr_unpair,bredr_unpair,"");
    SHELL_CMD_EXPORT_ALIAS(bredr_start_inquiry,bredr_start_inquiry,"");
    SHELL_CMD_EXPORT_ALIAS(bredr_stop_inquiry,bredr_stop_inquiry,"");
    SHELL_CMD_EXPORT_ALIAS(bredr_auth, bredr_auth, Register auth callback Parameter:[Null]);
    SHELL_CMD_EXPORT_ALIAS(bredr_auth_cancel, bredr_auth_cancel, Cancel register auth callback Parameter:[Null]]);
    SHELL_CMD_EXPORT_ALIAS(bredr_auth_passkey_confirm, bredr_auth_passkey_confirm, Confirm passkey Parameter:[Null]]);
    SHELL_CMD_EXPORT_ALIAS(bredr_auth_pairing_confirm, bredr_auth_pairing_confirm, Confirm pairing in secure connection Parameter:[Null]);
    SHELL_CMD_EXPORT_ALIAS(bredr_auth_passkey, bredr_auth_passkey, Input passkey Parameter:[Passkey: 00000000-000F423F]);
    SHELL_CMD_EXPORT_ALIAS(bredr_get_bond_list, bredr_get_bond_list, BT get Bond List);
    #if BR_EDR_PTS_TEST
    SHELL_CMD_EXPORT_ALIAS(bredr_sdp_client_connect,bredr_sdp_client_connect,"");
    #endif
    #if CONFIG_BT_A2DP
    SHELL_CMD_EXPORT_ALIAS(a2dp_connect,a2dp_connect,"");
    #if CONFIG_BT_A2DP_SOURCE
    SHELL_CMD_EXPORT_ALIAS(a2dp_discovery,a2dp_start_disc, "");
    SHELL_CMD_EXPORT_ALIAS(a2dp_suspend,a2dp_source_suspend, "");
    SHELL_CMD_EXPORT_ALIAS(a2dp_resume, a2dp_source_resume, "");
    #endif
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
    SHELL_CMD_EXPORT_ALIAS(avrcp_set_vol,avrcp_set_vol,"");
    SHELL_CMD_EXPORT_ALIAS(avrcp_get_vol,avrcp_get_vol,"");
    SHELL_CMD_EXPORT_ALIAS(avrcp_send_play_status,avrcp_send_play_status,"");
    #endif

    #if CONFIG_BT_HFP
    #if BR_EDR_PTS_TEST
    SHELL_CMD_EXPORT_ALIAS(bredr_rfcomm_test_mode,bredr_rfcomm_test_mode,"");
    #endif
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
    #if CONFIG_BT_SPP
    SHELL_CMD_EXPORT_ALIAS(spp_send,spp_send,"");
    SHELL_CMD_EXPORT_ALIAS(spp_connect,spp_connect,"");
    SHELL_CMD_EXPORT_ALIAS(spp_disconnect,spp_disconnect,"");
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
    {"bredr_start_inquiry", "", bredr_start_inquiry},
    {"bredr_stop_inquiry", "", bredr_stop_inquiry},
    {"bredr_auth", "", bredr_auth},
    {"bredr_unpair", "", bredr_unpair},
    {"bredr_auth_cancel", "", bredr_auth_cancel},
    {"bredr_auth_passkey_confirm", "", bredr_auth_passkey_confirm},
    {"bredr_auth_pairing_confirm", "", bredr_auth_pairing_confirm},
    {"bredr_auth_passkey", "", bredr_auth_passkey},
    {"bredr_get_bond_list","",bredr_get_bond_list},
    {"bredr_start_inquiry", "", bredr_start_inquiry},
    {"bredr_set_tx_pwr","",bredr_set_tx_pwr},
    #if BR_EDR_PTS_TEST
    {"bredr_sdp_client_connect", "", bredr_sdp_client_connect},
    #endif
        
    #if CONFIG_BT_A2DP
    {"a2dp_connect", "", a2dp_connect},
	#if CONFIG_BT_A2DP_SOURCE
    {"a2dp_start_disc", "", a2dp_discovery},
    {"a2dp_source_suspend", "", a2dp_suspend},
    {"a2dp_source_resume", "", a2dp_resume},
	#endif
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
    {"avrcp_get_vol", "", avrcp_get_vol},
    {"avrcp_set_vol", "", avrcp_set_vol},
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
    #if CONFIG_BT_SPP
    {"spp_send","",spp_send},
    {"spp_connect","",spp_connect},
    {"spp_disconnect","",spp_disconnect},
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
#if CONFIG_BT_SPP
    spp_cb_register(&spp_conn_callbacks);
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

BT_CLI(connectable)
{
    int err;
    uint8_t action;
    
    if(argc != 2){
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

BT_CLI(l2cap_send_test_data)
{
    int err = 0;
    uint8_t test_data[10]={0x01, 0x02, 0x3,0x04,0x05,0x06,0x07,0x08,0x09,0xa0};
    uint16_t cid;

    get_uint16_from_string(&argv[1], &cid);

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
    
    get_uint16_from_string(&argv[1], &tx_cid);

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
        get_uint8_from_string(&argv[1], &sec_level);
    
    err = bt_conn_set_security(default_conn, sec_level);

    if(err){
        printf("Failed to start security, (err %d) \r\n", err);
    }else{
        printf("Start security successfully\r\n");
    }
}
BT_CLI(unpair)
{
    bt_addr_le_t addr;
    u8_t  addr_val[6];
    int err;

    if(argc != 3){
        printf("Number of Parameters is not correct\r\n");
        return;
    }

    /*Get addr type, 0:ADDR_PUBLIC, 1:ADDR_RAND, 2:ADDR_RPA_OR_PUBLIC, 3:ADDR_RPA_OR_RAND*/
    get_uint8_from_string(&argv[1], &addr.type);
  
    get_bytearray_from_string(&argv[2], addr_val,6);

    reverse_bytearray(addr_val, addr.a.val, 6);

    err = bt_unpair(0, &addr);

    if(err){
        printf("Failed to unpair\r\n");
    }else{
        printf("Unpair successfully\r\n");
    }
}

typedef struct {
    uint8_t data_type;
    uint8_t data_length;
    uint8_t *data;
} eir_data_t;

static void bredr_parse_eir_data(u8_t *eir, size_t eir_len)
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
        bredr_parse_eir_data((u8_t*)results[i].eir, 240);
    }
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

BT_CLI(set_min_enc_key_size)
{
    int err;
    u8_t key_size;
    if(argc != 2){
       printf("Number of Parameters is not correct\r\n");
       return;
    }
    get_uint8_from_string(&argv[1], (uint8_t *)&key_size); 
    err = bt_br_set_min_enc_key_size(key_size);
    if (err)
    {
        printf("set BREDR min encryption key size failed (err %d)\r\n", err);
    }
    else
    {
        printf("set BREDR min encryption key size success\r\n");
    }
}

BT_CLI(set_tx_pwr)
{
    int err;
    u8_t br_power,edr_power;

    if(argc != 3){
        printf("Number of Parameters is not correct\r\n");
        return;
    }
    get_uint8_from_string(&argv[1],&br_power);
    get_uint8_from_string(&argv[2],&edr_power);
    if ( (br_power > 10) && (br_power != 0xff) )
    {
        printf("bt_set_tx_pwr, invalid value, br power value shall be in [%d - %d] or 0xff\r\n", 0, 10);
        return;
    }

    if ( (edr_power > 8) && (edr_power != 0xff) )
    {
        printf("bt_set_tx_pwr, invalid value, edr power value shall be in [%d - %d] or 0xff\r\n", 0, 8);
        return;
    }

    err = bt_br_set_tx_pwr((int8_t)br_power, (int8_t)edr_power);

    if(err){
		printf("bt_set_tx_pwr, Fail to set tx power (err %d)\r\n", err);
	}
	else{
		printf("bt_set_tx_pwr, Set tx power successfully\r\n");
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

#if CONFIG_BT_A2DP_SOURCE
static bool media_task_create = false;
static void media_thread(void *args)
{
   while (1) 
   {
        if(steam_pause == false)
        {
            int err;
            err = bt_a2dp_send_media(audio_buf, audio_buf_size);
            if (err) 
            {
                printf("send media fail %d\r\n", err);
            }
            else 
            {
                vTaskDelay(3000);
            }
        }
        else
        {
            vTaskDelay(3000);
        }
    }
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

#if CONFIG_BT_A2DP_SOURCE
BT_A2DP_CLI(discovery)
{
    int ret;

    if (!default_conn) {
        printf("Not connected.\n");
        return;
    }

    ret = bt_start_discovery(default_conn);
    if(ret) {
        printf("A2dp start discovery successfully.\n");
    } else {
        printf("A2dp start discovery fail. ret(%d)\n",ret);
    }
}

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
        printf("A2dp get cap successfully.\n");
    } else {
        printf("A2dp get cap discovery fail. ret(%d)\n",ret);
    }
}

BT_A2DP_CLI(set_conf)
{
    int ret;
    uint8_t acp_seid;

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }

    get_uint8_from_string(&argv[1], &acp_seid);
    ret = bt_a2dp_set_conf(default_conn,acp_seid);
    if(ret) {
        printf("A2dp set conf successfully.\n");
    } else {
        printf("A2dp set conf fail. ret(%d)\n",ret);
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
        printf("A2dp close stream successfully.\n");
    } else {
        printf("A2dp close stream. ret(%d)\n",ret);
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
        printf("A2dp open stream successfully.\n");
    } else {
        printf("A2dp open stream  fail. ret(%d)\n",ret);
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
        printf("A2dp start stream successfully.\n");
    } else {
        printf("A2dp start stream fail. ret(%d)\n",ret);
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
        printf("A2dp suspend stream successfully.\n");
    } else {
        printf("A2dp suspend stream fail. ret(%d)\n",ret);
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
        printf("A2dp set delay report successfully.\n");
    } else {
        printf("A2dp set delay report fail. ret(%d)\n",ret);
    }
}

extern uint8_t reject_set_conf_pts;
extern uint8_t reject_error_code;
BT_AVDTP_CLI(set_conf_reject)
{
    get_uint8_from_string(&argv[1], &reject_set_conf_pts);
    get_uint8_from_string(&argv[2], &reject_error_code);
}

#endif
#endif

#if CONFIG_BT_AVRCP
static void avrcp_chain(struct bt_conn *conn, uint8_t state)
{
    printf("%s, conn: %p \n", __func__, conn);

    if (state == BT_AVRCP_CHAIN_CONNECTED) {
        #if CONFIG_BT_A2DP_SOURCE
        avrcp_send_volume_notification(NULL);
        #endif
        #if CONFIG_BT_A2DP_SINK
        avrcp_reg_play_status_notification(NULL);
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

static void avrcp_passthrough_handler(uint8_t released, u8_t option_id)
{
    BT_WARN("released: %d option id: 0x%x \n",released, option_id);
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

#if BR_EDR_PTS_TEST
extern uint8_t rfcomm_test_enable;
BT_CLI(rfcomm_test_mode)
{
    get_uint8_from_string(&argv[1], &rfcomm_test_enable);
}
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

    get_uint8_from_string(&argv[1], &sco_type);

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
{
    int err = 0;
    
    if(!default_conn){
            printf("Not connected.\n");
            return;
    }

    err = bt_hfp_hf_send_cmd(default_conn, BT_HFP_HF_AT_DDD, "ATD1234567;");
    if(err)
        printf("Fail to send outgoing call AT command with err:%d\r\n", err);
    else
        printf("send outgoing call AT command successfully\r\n");
        
}

BT_HFP_CLI(outgoint_call_with_mem_loc)
{
    int err = 0;
    uint8_t phone_mem_loc = 0;
    char str[7] = "ATD";
    
    if(!default_conn){
            printf("Not connected.\n");
            return;
    }

    get_uint8_from_string(&argv[1], &phone_mem_loc);
    snprintf(&str[3], 4, ">%d;", phone_mem_loc);
    err = bt_hfp_hf_send_cmd(default_conn, BT_HFP_HF_AT_DDD, str);
    
    if(err)
        printf("Fail to send outgoing call with memory location AT command with err:%d\r\n", err);
    else
        printf("send outgoing call with memory location AT command successfully\r\n");
        
}

BT_HFP_CLI(outgoint_call_last_number_dialed)
{
    int err = 0;
    char *str = "AT+BLDN";
    
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
{
    int err = 0;
    uint8_t enable = 0;

    get_uint8_from_string(&argv[1], &enable);

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
{
    int err = 0;
    uint8_t enable = 0;

    get_uint8_from_string(&argv[1], &enable);

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

    get_uint8_from_string(&argv[1], &call_id);

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

    get_uint8_from_string(&argv[1], &vol);

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

    get_uint8_from_string(&argv[1], &vol);

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

    get_uint8_from_string(&argv[1], &method);
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

    get_uint16_from_string(&argv[1], &id);

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

    get_uint16_from_string(&argv[1], &val);

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
#if CONFIG_BT_SPP
BT_SPP_CLI(send)
{
    int err = 0;
    uint8_t test_data[10] = {0x01, 0x02, 0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a};
    if(!default_conn){
        printf("Not connected.\n");
        return;
    }
    
    err=bt_spp_send(test_data,10);
    if(err)
        printf("bt spp send err:%d\r\n", err);
    else
        printf("bt spp send successfully\r\n");
}

BT_SPP_CLI(connect)
{
    int err = 0;
    if(!default_conn){
        printf("Not connected.\n");
        return;
    }
    err= bt_spp_connect(default_conn);
    if(err)
        printf("bt spp connect err:%d\r\n", err);
    else
        printf("bt spp connect successfully\r\n");
}

BT_SPP_CLI(disconnect)
{
    int err = 0;
    if(!default_conn){
        printf("Not connected.\n");
        return;
    }
    err= bt_spp_disconnect(default_conn);
    if(err)
        printf("bt spp disconnect err:%d\r\n", err);
    else
        printf("bt spp disconnect successfully\r\n");
}
#endif
static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
    char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    printf("passkey_str is: %06u\r\n", passkey);
}

static void auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printf("Confirm passkey for %s: %06u\r\n", addr, passkey);
}

static void auth_passkey_entry(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printf("Enter passkey for %s\r\n", addr);
}

static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	printf("Pairing cancelled: %s\r\n", addr);
}

static void auth_pairing_confirm(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printf("Confirm pairing for %s\r\n", addr);
}

static void auth_pairing_complete(struct bt_conn *conn, bool bonded)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printf("%s with %s\r\n", bonded ? "Bonded" : "Paired", addr);
}

static void auth_pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	printf("Pairing failed with %s\r\n", addr);
}

static struct bt_conn_auth_cb auth_cb_display = {
	.passkey_display = auth_passkey_display,
	.passkey_entry = auth_passkey_entry,
	.passkey_confirm = auth_passkey_confirm,
	.cancel = auth_cancel,
	.pairing_confirm = auth_pairing_confirm,
	.pairing_failed = auth_pairing_failed,
	.pairing_complete = auth_pairing_complete,
};

static void bt_foreach_bond_info_cb(const struct bt_bond_info *info, void *user_data)
{
    /**************bond info dump *************/
    char addr[BT_ADDR_LE_STR_LEN];
    struct bt_keys *keys=NULL;
    if(user_data)
        (*(u8_t *)user_data)++;

    bt_addr_le_to_str(&info->addr, addr, sizeof(addr));
    keys = bt_keys_find(BT_KEYS_ALL, 0, &info->addr);
    printf("BTADDR:%s LTK:%s\r\n",addr,bt_hex(keys->ltk.val,16));
}

BT_CLI(auth)
{
    int err;

    err = bt_conn_auth_cb_register(&auth_cb_display);

    if(err){
        printf("Auth callback has already been registered\r\n");
    }else{
        printf("Register auth callback successfully\r\n");
    }
}

BT_CLI(auth_cancel)
{
	struct bt_conn *conn;
	if (default_conn) {
		conn = default_conn;
	}else {
		conn = NULL;
	}

	if (!conn) {
        printf("Not connected\r\n");
		return;
	}

	bt_conn_auth_cancel(conn);
}

BT_CLI(auth_passkey_confirm)
{
	if (!default_conn) {
        printf("Not connected\r\n");
		return;
	}

	bt_conn_auth_passkey_confirm(default_conn);
}

BT_CLI(auth_pairing_confirm)
{
	if (!default_conn) {
        printf("Not connected\r\n");
		return;
	}

	bt_conn_auth_pairing_confirm(default_conn);
}

BT_CLI(auth_passkey)
{
    uint32_t passkey;

    if(argc != 2){
        printf("Number of Parameters is not correct\r\n");
        return;
    }

    if (!default_conn) {
        printf("Not connected\r\n");
        return;
    }

    passkey = atoi(argv[1]);
    if (passkey > PASSKEY_MAX) {
        printf("Passkey should be between 0-999999\r\n");
        return;
    }

    bt_conn_auth_passkey_entry(default_conn, passkey);
}

BT_CLI(get_bond_list)
{
    bt_foreach_bond(0, bt_foreach_bond_info_cb, NULL);
}

int bredr_cli_register(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //aos_cli_register_commands(bredr_cmd_set, sizeof(bredr_cmd_set)/sizeof(bredr_cmd_set[0]));
    return 0;
}
