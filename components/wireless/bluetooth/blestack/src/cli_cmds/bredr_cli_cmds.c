/** @file
 * @brief Bluetooth BR/EDR shell module
 *
 * Provide some BR/EDR commands that can be useful to applications.
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <byteorder.h>
#include <bluetooth.h>
#include <hci_host.h>
#include <conn.h>
#include <conn_internal.h>
#include <hci_core.h>
#include <l2cap.h>
#include <l2cap_internal.h>
#if defined(CONFIG_BT_A2DP)
#include <a2dp.h>
#endif
#if defined(CONFIG_BT_AVRCP)
#include <avrcp.h>
#endif
#if defined(CONFIG_BT_AVRCP)
//#include <rfcomm.h>
#endif
#if defined(CONFIG_BT_HFP)
#include <FreeRTOS.h>
#include <task.h>
#include <hfp_hf.h>
#include "btble_lib_api.h"
#if defined(CONFIG_BT_HFP_SCO_TEST)
#include "hfp_sco_audio.h"
#endif
#endif
#if defined(CONFIG_BT_SPP)
#include <spp.h>
#include "spp_throughput.h"
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
#include "work_q.h"

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
#define BT_AVDTP_CLI(func) static void avdtp_##func(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
#define BT_SPP_CLI(func) static void spp_##func(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
#endif
#define 		PASSKEY_MAX  		0xF423F
struct bt_br_discovery_result result[10] = { 0 };

static void bredr_connected(struct bt_conn *conn, u8_t err);
static void bredr_disconnected(struct bt_conn *conn, u8_t reason);
#if defined(BFLB_BREDR_PATCH_EXTENDED_INQUIRY_RESULT_CALLBACK)
static void bredr_discovery_result_cb(struct bt_br_discovery_result *result);
#endif

static bool init = false;
static bool auto_role_switch = false;
static struct bt_conn_info conn_info;
static struct bt_conn *default_conn = NULL;

static struct bt_conn_cb conn_callbacks = {
    .connected = bredr_connected,
    .disconnected = bredr_disconnected,
};
#if defined(CONFIG_BT_SPP)

static void bt_recv_callback(struct bt_conn *conn, u8_t *data, u16_t length)
{
    BT_WARN("conn %p len %u data %s", conn, length, bt_hex(data, length));
};
static void bt_spp_connected(struct bt_conn *conn)
{
    BT_WARN("conn %p", conn);
};
static void bt_spp_disconnected(struct bt_conn *conn)
{
    BT_WARN("conn %p", conn);
};

struct spp_callback_t spp_conn_callbacks={
    .connected=bt_spp_connected,
    .disconnected=bt_spp_disconnected,
    .bt_spp_recv=bt_recv_callback,
};
#endif
#if defined(CONFIG_BT_A2DP)
static bool stream_pause = false;
static void a2dp_chain(struct bt_conn *conn, uint8_t state);
static void a2dp_stream(uint8_t state);

#if defined(CONFIG_BT_A2DP_SOURCE)
enum avrcp_stream_action {
    AVRCP_STREAM_ACTION_NONE,
    AVRCP_STREAM_ACTION_PLAY,
    AVRCP_STREAM_ACTION_PAUSE,
};

static atomic_t avrcp_pending_action;
static struct k_work avrcp_stream_work;

#if defined(CONFIG_BT_AVRCP)
#define AVRCP_PLAYBACK_STATUS_NONE (-1)
static atomic_t avrcp_pending_playback_status;
static struct k_work avrcp_playback_status_work;
#endif

static void avrcp_stream_work_handler(struct k_work *work)
{
    enum avrcp_stream_action action;
    int ret;

    (void)work;

    action = atomic_set(&avrcp_pending_action,
                        AVRCP_STREAM_ACTION_NONE);

    if (!default_conn) {
        printf("Not connected.\n");
        return;
    }

    if (action == AVRCP_STREAM_ACTION_PLAY) {
        ret = bt_stream_resume(default_conn);
        if (ret) {
            printf("A2DP resume stream failed. ret(%d)\n", ret);
        }
    } else if (action == AVRCP_STREAM_ACTION_PAUSE) {
        ret = bt_stream_suspend(default_conn);
        if (ret) {
            printf("A2DP suspend stream failed. ret(%d)\n", ret);
        }
    }

    if (atomic_get(&avrcp_pending_action) != AVRCP_STREAM_ACTION_NONE) {
        k_work_submit(&avrcp_stream_work);
    }
}

#if defined(CONFIG_BT_AVRCP)
static void avrcp_playback_status_work_handler(struct k_work *work)
{
    atomic_val_t status;
    int err;

    (void)work;

    status = atomic_set(&avrcp_pending_playback_status,
                        AVRCP_PLAYBACK_STATUS_NONE);
    if (status == AVRCP_PLAYBACK_STATUS_NONE) {
        return;
    }

    err = avrcp_set_playback_status((uint8_t)status);
    if (err) {
        printf("AVRCP playback status update failed. ret(%d)\n", err);
    }

    if (atomic_get(&avrcp_pending_playback_status) !=
        AVRCP_PLAYBACK_STATUS_NONE) {
        k_work_submit(&avrcp_playback_status_work);
    }
}

static void avrcp_schedule_playback_status(uint8_t status)
{
    atomic_set(&avrcp_pending_playback_status, status);
    k_work_submit(&avrcp_playback_status_work);
}
#endif
#endif

#if defined(BFLB_BREDR_PATCH_A2DP_DISCOVERY_CALLBACK)
static void a2dp_discovery_probe_complete(struct bt_conn *conn,
                                          uint16_t sep_count)
{
    (void)conn;
    printf("A2DP_DISCOVERY_RSP count=%u\n", sep_count);
}
#endif

#if defined(CONFIG_BT_A2DP_SOURCE)
struct k_thread media_transport;
static void a2dp_start_cfm(void);
static void a2dp_suspend_cfm(void);

#define A2DP_SOURCE_TEST_TONE_SBC_FRAMES 10
#define A2DP_SOURCE_TEST_TONE_SAMPLES_PER_FRAME 128
#define A2DP_SOURCE_TEST_TONE_CHANNELS 2

static int16_t a2dp_source_test_tone_pcm[A2DP_SOURCE_TEST_TONE_SBC_FRAMES *
                                          A2DP_SOURCE_TEST_TONE_SAMPLES_PER_FRAME *
                                          A2DP_SOURCE_TEST_TONE_CHANNELS];
static bool a2dp_source_test_tone_ready = false;

static void a2dp_source_test_tone_init(void)
{
    static const int16_t sine_lut[] = {
        0, 3196, 6269, 9102, 11585, 13623, 15136, 16069,
        16383, 16069, 15136, 13623, 11585, 9102, 6269, 3196,
        0, -3196, -6269, -9102, -11585, -13623, -15136, -16069,
        -16383, -16069, -15136, -13623, -11585, -9102, -6269, -3196,
    };
    const size_t sample_frames = A2DP_SOURCE_TEST_TONE_SBC_FRAMES *
                                 A2DP_SOURCE_TEST_TONE_SAMPLES_PER_FRAME;

    if (a2dp_source_test_tone_ready) {
        return;
    }

    for (size_t i = 0; i < sample_frames; i++) {
        int16_t sample = sine_lut[i % ARRAY_SIZE(sine_lut)];

        a2dp_source_test_tone_pcm[i * A2DP_SOURCE_TEST_TONE_CHANNELS] = sample;
        a2dp_source_test_tone_pcm[i * A2DP_SOURCE_TEST_TONE_CHANNELS + 1] = sample;
    }

    a2dp_source_test_tone_ready = true;
}
#endif
static struct a2dp_callback a2dp_callbacks =
{
    .chain = a2dp_chain,
    .stream = a2dp_stream,
#if defined(CONFIG_BT_A2DP_SOURCE)
    .start_cfm = a2dp_start_cfm,
    .suspend_cfm = a2dp_suspend_cfm,
#endif
};
#endif

#if defined(CONFIG_BT_AVRCP)
static void avrcp_chain(struct bt_conn *conn, uint8_t state);
static void avrcp_absvol(uint8_t vol);
static void avrcp_play_status(uint32_t song_len, uint32_t song_pos, uint8_t status);
static void avrcp_passthrough_handler(uint8_t released, u8_t option_id);
static void avrcp_handle_play(void);
static void avrcp_handle_stop(void);
static void avrcp_handle_pause(void);
static void avrcp_handle_next(void);
static void avrcp_handle_previous(void);

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
BT_CLI(role_switch);
#if defined(BFLB_BREDR_PATCH_DEINIT_CLEANUP)
BT_CLI(deinit);
#endif
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
BT_CLI(get_all_conn_info);
BT_CLI(get_bond_list);
BT_CLI(start_inquiry);
BT_CLI(stop_inquiry);
BT_CLI(set_min_enc_key_size);
BT_CLI(set_tx_pwr);
#if defined(BFLB_BREDR_PATCH_ENABLE_SNIFF_MODE)
BT_CLI(sniff);
BT_CLI(unsniff);
#endif

#if defined(BR_EDR_PTS_TEST)
BT_CLI(sdp_client_connect);
BT_AVDTP_CLI(set_conf_reject);
#endif

#if defined(CONFIG_BT_A2DP)
BT_A2DP_CLI(connect);
BT_A2DP_CLI(stats);
#if defined(BFLB_BREDR_PATCH_A2DP_DISCOVERY_CALLBACK)
BT_A2DP_CLI(discovery_probe);
#endif
#if defined(CONFIG_BT_A2DP_SOURCE)
BT_A2DP_CLI(discovery);
BT_A2DP_CLI(suspend);
BT_A2DP_CLI(resume);
#endif
#if defined(BR_EDR_PTS_TEST)
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

#if defined(CONFIG_BT_AVRCP)
BT_AVRCP_CLI(connect);
BT_AVRCP_CLI(pth_key);
BT_AVRCP_CLI(pth_key_act);
BT_AVRCP_CLI(change_vol);
BT_AVRCP_CLI(get_play_status);
BT_AVRCP_CLI(set_vol);
BT_AVRCP_CLI(get_vol);
BT_AVRCP_CLI(send_play_status);

#endif

#if defined(CONFIG_BT_HFP)
#if defined(CONFIG_BT_HFP_SCO_TEST)
static void hfp_sco_audio_task_init(void);
#endif
static void codec_bt_hfp_cb(uint16_t interval_halfslot,
                        uint32_t tx_buffer_0,
                        uint32_t tx_buffer_1,
                        uint32_t rx_buffer_0,
                        uint32_t rx_buffer_1,
                        uint32_t tx_buffer_size,
                        uint32_t rx_buffer_size,
                        uint32_t start_time_halfslot,
                        uint8_t buffer_index);
static void hfp_bsir_indication(struct bt_conn *conn, uint8_t inband)
{
    printf("HFP in-band ring tone: %s\n", inband ? "enabled" : "disabled");
}

static void hfp_clip_indication(struct bt_conn *conn, const char *number, uint8_t type)
{
    printf("HFP CLIP: number=%s type=%u\n", number, type);
}

static void hfp_ccwa_indication(struct bt_conn *conn, const char *number, uint8_t type)
{
    printf("HFP CCWA: number=%s type=%u\n", number, type);
}

static void hfp_bvra_indication(struct bt_conn *conn, uint8_t active)
{
    printf("HFP voice recognition: %s\n", active ? "activated" : "deactivated");
}

static void hfp_clcc_indication(struct bt_conn *conn,
                                const struct bt_hfp_clcc_info *info)
{
    if (!info) {
        printf("HFP CLCC: end of list\n");
        return;
    }
    printf("HFP CLCC: idx=%d dir=%s status=%d mode=%d mpty=%d number=%s type=%d\n",
        info->idx,
        info->dir ? "MT" : "MO",
        info->status,
        info->mode,
        info->mpty,
        info->number[0] ? info->number : "(none)",
        info->type);
}

static struct bt_hfp_hf_cb hfp_hf_callbacks = {
    .bsir_indication = hfp_bsir_indication,
    .clip_indication = hfp_clip_indication,
    .ccwa_indication = hfp_ccwa_indication,
    .bvra_indication = hfp_bvra_indication,
    .clcc_indication = hfp_clcc_indication,
};

#if defined(BR_EDR_PTS_TEST)
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

#if defined(CONFIG_BT_SPP)
BT_SPP_CLI(send);
BT_SPP_CLI(disconnect);
BT_SPP_CLI(connect);
BT_SPP_CLI(mtu);
BT_SPP_CLI(throughput_start);
BT_SPP_CLI(throughput_stop);
#endif

#if defined(CONFIG_SHELL)
    SHELL_CMD_EXPORT_ALIAS(bredr_init,bredr_init,BREDR Initialize Parameter:[Null]);
    SHELL_CMD_EXPORT_ALIAS(bredr_role_switch,bredr_role_switch,
                            bredr_role_switch Parameter:[1:enable 0:disable]);
    #if defined(BFLB_BREDR_PATCH_DEINIT_CLEANUP)
    SHELL_CMD_EXPORT_ALIAS(bredr_deinit,bredr_deinit,Reset bredr demo init state so next bredr_init re-registers callbacks);
    #endif
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
    #if defined(BFLB_BREDR_PATCH_ENABLE_SNIFF_MODE)
    SHELL_CMD_EXPORT_ALIAS(bredr_sniff,bredr_sniff,bredr_sniff Parameter:[address] [interval_ms]);
    SHELL_CMD_EXPORT_ALIAS(bredr_unsniff,bredr_unsniff,bredr_unsniff Parameter:[address]);
    #endif
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
    SHELL_CMD_EXPORT_ALIAS(bredr_get_all_conn_info, bredr_conn_info, BR/EDR get all connection devices info Parameter:[Null]);
    SHELL_CMD_EXPORT_ALIAS(bredr_get_bond_list, bredr_get_bond_list, BT get Bond List);
    #if defined(BR_EDR_PTS_TEST)
    SHELL_CMD_EXPORT_ALIAS(bredr_sdp_client_connect,bredr_sdp_client_connect,"");
    #endif
    #if defined(CONFIG_BT_A2DP)
    SHELL_CMD_EXPORT_ALIAS(a2dp_connect,a2dp_connect,"");
    SHELL_CMD_EXPORT_ALIAS(a2dp_stats,a2dp_stats,a2dp_stats Parameter:[Null: print once; seconds: periodic dump; 0: stop]);
    #if defined(BFLB_BREDR_PATCH_A2DP_DISCOVERY_CALLBACK)
    SHELL_CMD_EXPORT_ALIAS(a2dp_discovery_probe,a2dp_discovery_probe,"");
    #endif
    #if defined(CONFIG_BT_A2DP_SOURCE)
    SHELL_CMD_EXPORT_ALIAS(a2dp_discovery,a2dp_start_disc, "");
    SHELL_CMD_EXPORT_ALIAS(a2dp_suspend,a2dp_source_suspend, "");
    SHELL_CMD_EXPORT_ALIAS(a2dp_resume, a2dp_source_resume, "");
    #endif
    #if defined(BR_EDR_PTS_TEST)
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

    #if defined(CONFIG_BT_AVRCP)
    SHELL_CMD_EXPORT_ALIAS(avrcp_connect,avrcp_connect,"");
    SHELL_CMD_EXPORT_ALIAS(avrcp_pth_key,avrcp_pth_key,"");
    SHELL_CMD_EXPORT_ALIAS(avrcp_pth_key_act,avrcp_pth_key_act,"");
    SHELL_CMD_EXPORT_ALIAS(avrcp_change_vol,avrcp_change_vol,"");
    SHELL_CMD_EXPORT_ALIAS(avrcp_get_play_status,avrcp_get_play_status,"");
    SHELL_CMD_EXPORT_ALIAS(avrcp_set_vol,avrcp_set_vol,"");
    SHELL_CMD_EXPORT_ALIAS(avrcp_get_vol,avrcp_get_vol,"");
    SHELL_CMD_EXPORT_ALIAS(avrcp_send_play_status,avrcp_send_play_status,"");
    #endif

    #if defined(CONFIG_BT_HFP)
    #if defined(BR_EDR_PTS_TEST)
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
    #if defined(CONFIG_BT_SPP)
    SHELL_CMD_EXPORT_ALIAS(spp_send,spp_send,"");
    SHELL_CMD_EXPORT_ALIAS(spp_connect,spp_connect,"");
    SHELL_CMD_EXPORT_ALIAS(spp_disconnect,spp_disconnect,"");
    SHELL_CMD_EXPORT_ALIAS(spp_mtu,spp_mtu,"");
    SHELL_CMD_EXPORT_ALIAS(spp_throughput_start,spp_tp_start,spp_tp_start Parameter:[idx|all] [len] [rate_kbps]);
    SHELL_CMD_EXPORT_ALIAS(spp_throughput_stop,spp_tp_stop,spp_tp_stop Parameter:[idx|all]);
    #endif

#else
const struct cli_command bredr_cmd_set[] STATIC_CLI_CMD_ATTRIBUTE = {
    #if PCM_PRINTF
    {"pcm", "", pcm},
    #endif
    {"bredr_init", "", bredr_init},
    {"bredr_role_switch", "", bredr_role_switch},
    #if defined(BFLB_BREDR_PATCH_DEINIT_CLEANUP)
    {"bredr_deinit", "", bredr_deinit},
    #endif
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
    {"bredr_conn_info", "", bredr_get_all_conn_info},
    {"bredr_get_bond_list","",bredr_get_bond_list},
    {"bredr_set_tx_pwr","",bredr_set_tx_pwr},
    #if defined(BFLB_BREDR_PATCH_ENABLE_SNIFF_MODE)
    {"bredr_sniff","",bredr_sniff},
    {"bredr_unsniff","",bredr_unsniff},
    #endif
    #if defined(BR_EDR_PTS_TEST)
    {"bredr_sdp_client_connect", "", bredr_sdp_client_connect},
    #endif
        
    #if defined(CONFIG_BT_A2DP)
    {"a2dp_connect", "", a2dp_connect},
    {"a2dp_stats", "", a2dp_stats},
    #if defined(BFLB_BREDR_PATCH_A2DP_DISCOVERY_CALLBACK)
    {"a2dp_discovery_probe", "", a2dp_discovery_probe},
	#endif
	#if defined(CONFIG_BT_A2DP_SOURCE)
    {"a2dp_start_disc", "", a2dp_discovery},
    {"a2dp_source_suspend", "", a2dp_suspend},
    {"a2dp_source_resume", "", a2dp_resume},
	#endif
    #if defined(BR_EDR_PTS_TEST)
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
    #if defined(CONFIG_BT_AVRCP)
    {"avrcp_connect", "", avrcp_connect},
    {"avrcp_pth_key", "", avrcp_pth_key},
    {"avrcp_pth_key_act", "", avrcp_pth_key_act},
    {"avrcp_change_vol", "", avrcp_change_vol},
    {"avrcp_play_status", "", avrcp_get_play_status},
    {"avrcp_get_vol", "", avrcp_get_vol},
    {"avrcp_set_vol", "", avrcp_set_vol},
    #endif

    #if defined(CONFIG_BT_HFP)
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
    #if defined(CONFIG_BT_SPP)
    {"spp_send","",spp_send},
    {"spp_connect","",spp_connect},
    {"spp_disconnect","",spp_disconnect},
    {"spp_mtu","",spp_mtu},
    {"spp_tp_start","",spp_throughput_start},
    {"spp_tp_stop","",spp_throughput_stop},
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
#if defined(BFLB_BREDR_PATCH_EXTENDED_INQUIRY_RESULT_CALLBACK)
    bt_br_discovery_result_cb_register(bredr_discovery_result_cb);
#endif
#if defined(CONFIG_BT_A2DP_SOURCE)
    atomic_set(&avrcp_pending_action, AVRCP_STREAM_ACTION_NONE);
    k_work_init(&avrcp_stream_work, avrcp_stream_work_handler);
#if defined(CONFIG_BT_AVRCP)
    atomic_set(&avrcp_pending_playback_status,
               AVRCP_PLAYBACK_STATUS_NONE);
    k_work_init(&avrcp_playback_status_work,
                avrcp_playback_status_work_handler);
#endif
#endif
    bt_conn_cb_register(&conn_callbacks);
#if defined(CONFIG_BT_A2DP)
    a2dp_cb_register(&a2dp_callbacks);
#if defined(BFLB_BREDR_PATCH_A2DP_DISCOVERY_CALLBACK)
    bt_a2dp_discovery_cb_register(a2dp_discovery_probe_complete);
#endif
#endif
#if defined(CONFIG_BT_AVRCP)
    avrcp_cb_register(&avrcp_callbacks);
#endif
#if defined(CONFIG_BT_SPP)
    spp_cb_register(&spp_conn_callbacks);
#endif
#if defined(CONFIG_BT_HFP)
#if defined(CONFIG_BT_HFP_SCO_TEST)
    hfp_sco_audio_task_init();
#endif
    bt_hfp_hf_register(&hfp_hf_callbacks);
    btble_controller_sco_codec_callback_register(codec_bt_hfp_cb);
#endif
    init = true;
    printf("bredr init successfully\n");
}
BT_CLI(role_switch)
{
    if (argc != 2 || (strcmp(argv[1], "0") && strcmp(argv[1], "1"))) {
        printf("Usage: bredr_role_switch [1:enable 0:disable]\n");
        return;
    }
    auto_role_switch = !strcmp(argv[1], "1");
    printf("bredr role switch after connection: %s\n",
           auto_role_switch ? "enabled" : "disabled");
}

#if defined(BFLB_BREDR_PATCH_DEINIT_CLEANUP)
/* bredr_deinit — clear this file's local demo-CLI "init" guard and the
 * default_conn handle so the next bredr_init actually re-registers the
 * conn/profile callbacks instead of short-circuiting on the stale flag.
 *
 * Required after a ble_disable()/ble_enable() cycle: bt_disable_action() calls
 * bt_conn_cb_clear(), unlinking conn_callbacks from the global callback_list,
 * but the static `init` flag is CLI-local state that nothing in the disable
 * path resets. Without this reset, a 2nd bredr_init prints "bredr has
 * initialized" and returns early, so bredr_connected()/bredr_disconnected()
 * (and the a2dp/avrcp/spp/hfp profile callbacks) are never re-registered even
 * though the BR/EDR ACL link itself reconnects fine at the HCI layer.
 *
 * Test scripts call this explicitly between ble_enable and the 2nd bredr_init,
 * which keeps ble_cli_cmds.c free of any CONFIG_BT_BREDR reference while
 * letting bredr_init keep its "don't re-init when already initialized"
 * short-circuit for interactive use. */
BT_CLI(deinit)
{
    init = false;
    default_conn = NULL;
    printf("bredr deinit done\n");
}
#endif

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

#if defined(CONFIG_BT_A2DP_SINK)
    /* Sink interoperability: only switch locally initiated ACL connections.
     * Use asynchronous HCI submission from the connection callback.
     */
    if (auto_role_switch &&
        atomic_test_bit(conn->flags, BT_CONN_BR_INITIAL_MASTER) &&
        conn->role == BT_CONN_ROLE_MASTER) {
        int ret = bt_conn_br_switch_role(conn, BT_CONN_ROLE_SLAVE);

        BT_WARN("[BR_ROLE] request slave: %s queue_result=%d", addr, ret);
    }
#endif

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
                     (results[i].cod[2] << 16));
        bt_addr_to_str(&results[i].addr, addr_str, sizeof(addr_str));
        printf("addr %s,class 0x%lx,rssi %d\r\n",addr_str,
                     dev_class,results[i].rssi);
        bredr_parse_eir_data((u8_t*)results[i].eir, 240);
    }
}

#if defined(BFLB_BREDR_PATCH_EXTENDED_INQUIRY_RESULT_CALLBACK)
static void bredr_discovery_result_cb(struct bt_br_discovery_result *result)
{
    char addr_str[BT_ADDR_STR_LEN];
    uint32_t dev_class;

    if (!result) {
        return;
    }

    dev_class = (result->cod[0] | (result->cod[1] << 8) |
                 (result->cod[2] << 16));
    bt_addr_to_str(&result->addr, addr_str, sizeof(addr_str));
    printf("extended inquiry result addr %s,class 0x%lx,rssi %d\r\n",
           addr_str, dev_class, result->rssi);
    bredr_parse_eir_data((u8_t *)result->eir, sizeof(result->eir));
}
#endif /* BFLB_BREDR_PATCH_EXTENDED_INQUIRY_RESULT_CALLBACK */

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

#if defined(BFLB_BREDR_PATCH_ENABLE_SNIFF_MODE)
/* bredr_sniff <addr> [interval_ms]
 *   Put the BR/EDR link with the given address into sniff mode so an idle or
 *   low-rate link stops hogging radio time. interval_ms default 100.
 */
BT_CLI(sniff)
{
    u8_t addr_val[6];
    bt_addr_t peer_addr;
    struct bt_conn *conn;
    uint16_t interval_ms = 100;
    int err;

    if (argc < 2) {
        printf("Usage: bredr_sniff <addr> [interval_ms]\r\n");
        return;
    }

    get_bytearray_from_string(&argv[1], addr_val, 6);
    reverse_bytearray(addr_val, peer_addr.val, 6);

    if (argc >= 3) {
        get_uint16_from_string(&argv[2], &interval_ms);
    }

    conn = bt_conn_lookup_addr_br(&peer_addr);
    if (!conn) {
        printf("bredr_sniff, no BR/EDR connection for this address\r\n");
        return;
    }

    /* interval in 0.625ms slots; give a small min/max window around it */
    uint16_t slots = (uint16_t)(((uint32_t)interval_ms * 1000) / 625);
    uint16_t min_slots = slots > 16 ? slots - 16 : slots;

    err = bt_br_conn_enter_sniff(conn, min_slots, slots);
    if (err) {
        printf("bredr_sniff, command rejected by controller (err %d)\r\n", err);
    } else {
        printf("bredr_sniff, command accepted (%u ms, %u slots), "
               "wait for Mode Change event\r\n", interval_ms, slots);
    }

    bt_conn_unref(conn);
}

/* bredr_unsniff <addr>  -- return the link to active mode */
BT_CLI(unsniff)
{
    u8_t addr_val[6];
    bt_addr_t peer_addr;
    struct bt_conn *conn;
    int err;

    if (argc < 2) {
        printf("Usage: bredr_unsniff <addr>\r\n");
        return;
    }

    get_bytearray_from_string(&argv[1], addr_val, 6);
    reverse_bytearray(addr_val, peer_addr.val, 6);

    conn = bt_conn_lookup_addr_br(&peer_addr);
    if (!conn) {
        printf("bredr_unsniff, no BR/EDR connection for this address\r\n");
        return;
    }

    err = bt_br_conn_exit_sniff(conn);
    if (err) {
        printf("bredr_unsniff, command rejected by controller (err %d)\r\n", err);
    } else {
        printf("bredr_unsniff, command accepted, wait for Mode Change event\r\n");
    }

    bt_conn_unref(conn);
}
#endif /* BFLB_BREDR_PATCH_ENABLE_SNIFF_MODE */

#if defined(BR_EDR_PTS_TEST)
BT_CLI(sdp_client_connect)
{
    extern int bt_sdp_client_connect(struct bt_conn *conn);
    bt_sdp_client_connect(default_conn);
}
#endif

#if defined(CONFIG_BT_A2DP)
static void a2dp_chain(struct bt_conn *conn, uint8_t state)
{
    printf("%s, conn: %p \n", __func__, conn);

    if (state == BT_A2DP_CHAIN_CONNECTED) {
        printf("a2dp connected. \n");
        stream_pause = false;
    } else if (state == BT_A2DP_CHAIN_DISCONNECTED) {
        printf("a2dp disconnected. \n");
        stream_pause = true;
    }
}

static void a2dp_stream(uint8_t state)
{
    printf("%s, state: %d \n", __func__, state);

    if (state == BT_A2DP_STREAM_START) {
        printf("a2dp play. \n");
#if defined(CONFIG_BT_A2DP_SOURCE)
        stream_pause = false;
#if defined(CONFIG_BT_AVRCP)
        avrcp_schedule_playback_status(PLAY_STATUS_PLAYING);
#endif
#endif
    } else if (state == BT_A2DP_STREAM_SUSPEND) {
        printf("a2dp stop. \n");
#if defined(CONFIG_BT_A2DP_SOURCE)
        stream_pause = true;
#if defined(CONFIG_BT_AVRCP)
        avrcp_schedule_playback_status(PLAY_STATUS_PAUSED);
#endif
#endif
    }
}

#if defined(CONFIG_BT_A2DP_SOURCE)
static bool media_task_create = false;
static void media_thread(void *args)
{
   a2dp_source_test_tone_init();

   while (1) 
   {
        if(stream_pause == false)
        {
            int err;
            err = bt_a2dp_send_media(a2dp_source_test_tone_pcm, sizeof(a2dp_source_test_tone_pcm));
            if (err) 
            {
                printf("send media fail %d\r\n", err);
            }
            else 
            {
                vTaskDelay(1);
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
   stream_pause = false;
#if defined(CONFIG_BT_AVRCP)
   avrcp_schedule_playback_status(PLAY_STATUS_PLAYING);
#endif
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

static void a2dp_suspend_cfm(void)
{
    stream_pause = true;
#if defined(CONFIG_BT_AVRCP)
    avrcp_schedule_playback_status(PLAY_STATUS_PAUSED);
#endif
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

BT_A2DP_CLI(stats)
{
    char *end;
    unsigned long period;

    if (argc < 2) {
        bt_a2dp_sink_stats_print();
        return;
    }

    period = strtoul(argv[1], &end, 10);
    if (end == argv[1] || *end != 0 || period > UINT32_MAX) {
        printf("Invalid period.\n");
        return;
    }

    if (bt_a2dp_sink_stats_set_period((uint32_t)period) != 0) {
        printf("Failed to update A2DP stats period.\n");
    } else if (period) {
        printf("A2DP stats every %lus.\n", period);
    } else {
        printf("A2DP stats stopped.\n");
    }
}

#if defined(BFLB_BREDR_PATCH_A2DP_DISCOVERY_CALLBACK)
BT_A2DP_CLI(discovery_probe)
{
    int err;

    if (!default_conn) {
        printf("Not connected.\n");
        return;
    }

    err = bt_start_discovery(default_conn);
    if (err < 0) {
        printf("A2DP_DISCOVERY_PROBE submit failed err=%d\n", err);
    } else {
        printf("A2DP_DISCOVERY_PROBE submitted\n");
    }
}
#endif

#if defined(CONFIG_BT_A2DP_SOURCE)
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

#if defined(BR_EDR_PTS_TEST)
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

#if defined(CONFIG_BT_AVRCP)
static void avrcp_chain(struct bt_conn *conn, uint8_t state)
{
    printf("%s, conn: %p \n", __func__, conn);

    if (state == BT_AVRCP_CHAIN_CONNECTED) {
        #if defined(CONFIG_BT_A2DP_SOURCE)
        avrcp_send_volume_notification(NULL);
        #endif
        #if defined(CONFIG_BT_A2DP_SINK)
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
#if defined(CONFIG_BT_A2DP_SOURCE)
    atomic_set(&avrcp_pending_action, AVRCP_STREAM_ACTION_PLAY);
    k_work_submit(&avrcp_stream_work);
#endif
}

static void avrcp_handle_stop(void)
{
    printf("%s\r\n",__func__);
}

static void avrcp_handle_pause(void)
{
    printf("%s\r\n",__func__);
#if defined(CONFIG_BT_A2DP_SOURCE)
    atomic_set(&avrcp_pending_action, AVRCP_STREAM_ACTION_PAUSE);
    k_work_submit(&avrcp_stream_work);
#endif
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

#if defined(CONFIG_BT_HFP)
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

#if defined(BR_EDR_PTS_TEST)
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

#if defined(CONFIG_BT_HFP_SCO_TEST)
#define HFP_SCO_CLOCK_MASK          0x0FFFFFFFU
#define HFP_SCO_CLOCK_HALF_RANGE    0x08000000U
#define HFP_SCO_FEED_TASK_STACK     512
#define HFP_SCO_FEED_TASK_PRIO      (CONFIG_BT_RX_PRIO - 1)

struct hfp_sco_sync_state {
    uint32_t tx_buffer[2];
    uint32_t tx_size;
    uint32_t start_time_halfslot;
    uint32_t session_id;
    uint32_t buffer_epoch;
    uint32_t sync_epoch;
    uint16_t interval_halfslot;
    uint8_t buffer_index;
    bool active;
};

static volatile struct hfp_sco_sync_state hfp_sco_sync;
static volatile uint32_t hfp_sco_sync_version;
static TaskHandle_t hfp_sco_feed_task_handle;
static uint32_t sample_pos = 0;

extern uint32_t lld_read_clock(void);

static int32_t hfp_sco_clock_diff(uint32_t from, uint32_t to)
{
    uint32_t diff = (to - from) & HFP_SCO_CLOCK_MASK;

    if (diff >= HFP_SCO_CLOCK_HALF_RANGE) {
        return -(int32_t)((from - to) & HFP_SCO_CLOCK_MASK);
    }

    return (int32_t)diff;
}

static uint32_t hfp_sco_clock_add(uint32_t clock, uint32_t halfslots)
{
    return (clock + halfslots) & HFP_SCO_CLOCK_MASK;
}

static void sco_audio_fill(uint32_t buffer, uint32_t size)
{
    uint16_t *samples = (uint16_t *)buffer;
    uint32_t num_samples = size / sizeof(samples[0]);
    uint32_t sample_count = sizeof(sco_audio_buf) / sizeof(sco_audio_buf[0]);

    for (uint32_t i = 0; i < num_samples; i++) {
        samples[i] = sco_audio_buf[(sample_pos + i) % sample_count];
    }
    sample_pos = (sample_pos + num_samples) % sample_count;
}

static uint32_t hfp_sco_sync_snapshot(struct hfp_sco_sync_state *state)
{
    uint32_t version;

    taskENTER_CRITICAL();
    version = hfp_sco_sync_version;
    state->tx_buffer[0] = hfp_sco_sync.tx_buffer[0];
    state->tx_buffer[1] = hfp_sco_sync.tx_buffer[1];
    state->tx_size = hfp_sco_sync.tx_size;
    state->start_time_halfslot = hfp_sco_sync.start_time_halfslot;
    state->session_id = hfp_sco_sync.session_id;
    state->buffer_epoch = hfp_sco_sync.buffer_epoch;
    state->sync_epoch = hfp_sco_sync.sync_epoch;
    state->interval_halfslot = hfp_sco_sync.interval_halfslot;
    state->buffer_index = hfp_sco_sync.buffer_index;
    state->active = hfp_sco_sync.active;
    taskEXIT_CRITICAL();

    return version;
}

static bool hfp_sco_fill_if_current(const struct hfp_sco_sync_state *state,
                                    uint32_t version,
                                    uint8_t first_buffer,
                                    uint8_t buffer_count)
{
    bool filled = false;

    taskENTER_CRITICAL();
    if (hfp_sco_sync_version == version && hfp_sco_sync.active) {
        for (uint8_t i = 0; i < buffer_count; i++) {
            uint8_t buffer_index = first_buffer ^ i;

            sco_audio_fill(state->tx_buffer[buffer_index], state->tx_size);
        }
        filled = true;
    }
    taskEXIT_CRITICAL();

    return filled;
}

static void hfp_sco_feed_task(void *arg)
{
    struct hfp_sco_sync_state state = { 0 };
    uint32_t session_id = 0;
    uint32_t buffer_epoch = 0;
    uint32_t sync_epoch = 0;
    uint32_t next_refill_halfslot = 0;
    uint8_t active_buffer = 0;
    bool scheduled = false;

    (void)arg;

    while (1) {
        uint32_t state_version;

        if (!scheduled) {
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        }

        state_version = hfp_sco_sync_snapshot(&state);
        if (!state.active) {
            scheduled = false;
            continue;
        }

        if (state.session_id != session_id) {
            sample_pos = 0;
            session_id = state.session_id;
        }

        if (state.buffer_epoch != buffer_epoch) {
            int32_t halfslots_to_start = hfp_sco_clock_diff(lld_read_clock(),
                                                            state.start_time_halfslot);

            active_buffer = state.buffer_index;
            if (halfslots_to_start > 0) {
                if (!hfp_sco_fill_if_current(&state, state_version,
                                             active_buffer, 2)) {
                    continue;
                }
                next_refill_halfslot = hfp_sco_clock_add(state.start_time_halfslot,
                                                         state.interval_halfslot);
            } else {
                uint32_t elapsed_intervals =
                    (uint32_t)(-halfslots_to_start) / state.interval_halfslot;

                if (elapsed_intervals & 1U) {
                    active_buffer ^= 1U;
                }
                if (!hfp_sco_fill_if_current(&state, state_version,
                                             active_buffer ^ 1U, 1)) {
                    continue;
                }
                next_refill_halfslot = hfp_sco_clock_add(
                    state.start_time_halfslot,
                    (elapsed_intervals + 1U) * state.interval_halfslot);
            }
            buffer_epoch = state.buffer_epoch;
            sync_epoch = state.sync_epoch;
            scheduled = true;
            continue;
        }

        if (state.sync_epoch != sync_epoch) {
            active_buffer = state.buffer_index;
            sync_epoch = state.sync_epoch;
            next_refill_halfslot = hfp_sco_clock_add(state.start_time_halfslot,
                                                     state.interval_halfslot);
        }

        int32_t halfslots_to_refill = hfp_sco_clock_diff(lld_read_clock(),
                                                         next_refill_halfslot);
        if (halfslots_to_refill > 2) {
            ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1));
            continue;
        }
        if (halfslots_to_refill > 0) {
            taskYIELD();
            continue;
        }

        do {
            active_buffer ^= 1U;
            next_refill_halfslot = hfp_sco_clock_add(next_refill_halfslot,
                                                     state.interval_halfslot);
        } while (hfp_sco_clock_diff(lld_read_clock(), next_refill_halfslot) <= 0);

        hfp_sco_fill_if_current(&state, state_version,
                                active_buffer ^ 1U, 1);
    }
}

static void hfp_sco_audio_task_init(void)
{
    if (hfp_sco_feed_task_handle != NULL) {
        return;
    }

    if (xTaskCreate(hfp_sco_feed_task, "hfp_sco_feed",
                    HFP_SCO_FEED_TASK_STACK, NULL,
                    HFP_SCO_FEED_TASK_PRIO,
                    &hfp_sco_feed_task_handle) != pdPASS) {
        printf("Failed to create HFP SCO feed task\r\n");
    }
}

static void hfp_sco_feed_task_notify(void)
{
    if (hfp_sco_feed_task_handle == NULL) {
        return;
    }

    if (k_is_in_isr()) {
        BaseType_t higher_priority_task_woken = pdFALSE;

        vTaskNotifyGiveFromISR(hfp_sco_feed_task_handle,
                               &higher_priority_task_woken);
        portYIELD_FROM_ISR(higher_priority_task_woken);
    } else {
        xTaskNotifyGive(hfp_sco_feed_task_handle);
    }
}
#endif
static void codec_bt_hfp_cb(uint16_t interval_halfslot,
                        uint32_t tx_buffer_0,
                        uint32_t tx_buffer_1,
                        uint32_t rx_buffer_0,
                        uint32_t rx_buffer_1,
                        uint32_t tx_buffer_size,
                        uint32_t rx_buffer_size,
                        uint32_t start_time_halfslot,
                        uint8_t buffer_index)
{
#if defined(CONFIG_BT_HFP_SCO_TEST)
    bool in_isr = k_is_in_isr();

    (void)rx_buffer_0;
    (void)rx_buffer_1;
    (void)rx_buffer_size;

    if (!in_isr) {
        taskENTER_CRITICAL();
    }

    if (buffer_index != 0xFF) {
        bool new_session = !hfp_sco_sync.active;
        bool buffers_changed = new_session ||
            hfp_sco_sync.tx_size != tx_buffer_size ||
            hfp_sco_sync.tx_buffer[0] != tx_buffer_0 ||
            hfp_sco_sync.tx_buffer[1] != tx_buffer_1;

        hfp_sco_sync_version++;
        hfp_sco_sync.tx_buffer[0] = tx_buffer_0;
        hfp_sco_sync.tx_buffer[1] = tx_buffer_1;
        hfp_sco_sync.tx_size = tx_buffer_size;
        hfp_sco_sync.start_time_halfslot = start_time_halfslot;
        hfp_sco_sync.interval_halfslot = interval_halfslot;
        hfp_sco_sync.buffer_index = buffer_index;
        hfp_sco_sync.active = true;
        hfp_sco_sync.sync_epoch++;
        if (new_session) {
            hfp_sco_sync.session_id++;
        }
        if (buffers_changed) {
            hfp_sco_sync.buffer_epoch++;
        }
        hfp_sco_sync_version++;
    } else {
        hfp_sco_sync_version++;
        hfp_sco_sync.active = false;
        hfp_sco_sync.sync_epoch++;
        hfp_sco_sync_version++;
    }

    if (!in_isr) {
        taskEXIT_CRITICAL();
    }

    hfp_sco_feed_task_notify();
#endif
}
#endif
#if defined(CONFIG_BT_SPP)
static uint8_t spp_test_buffer[1024];
BT_SPP_CLI(send)
{
    int err = 0;
    uint16_t len = 0;

    get_uint16_from_string(&argv[1], &len);

    if (len == 0 || len > 1024) {
        printf("Invalid length. Must be 1-1024.\n");
        return;
    }

    for (uint16_t i = 0; i < len; i++) {
        spp_test_buffer[i] = (i + 1) & 0xFF;
    }

    if(!default_conn){
        printf("Not connected.\n");
        return;
    }
    
    err=bt_spp_send(default_conn, spp_test_buffer, len);
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

BT_SPP_CLI(mtu)
{
    uint16_t mtu;
    int err;

    if (!default_conn) {
        printf("Not connected.\n");
        return;
    }

    err = bt_spp_get_tx_mtu(default_conn, &mtu);
    if (err) {
        printf("bt_spp_get_tx_mtu failed: %d\r\n", err);
        return;
    }

    printf("SPP TX MTU: %u\r\n", mtu);
}

/* spp_tp_start <idx|all> [len] [rate_kbps]
 *   idx       : SPP link index, or "all" for every connected link
 *   len       : per-send payload size, default 672
 *   rate_kbps : per-link target rate in KB/s, default 0 (full speed)
 */
BT_SPP_CLI(throughput_start)
{
    int idx = -1;
    uint16_t len = 672;
    uint16_t rate_kbps = 0;

    if (argc < 2) {
        printf("Usage: spp_tp_start <idx|all> [len] [rate_kbps]\r\n");
        return;
    }

    if (strcmp(argv[1], "all") != 0) {
        uint8_t v = 0;
        get_uint8_from_string(&argv[1], &v);
        idx = v;
    }

    if (argc >= 3) {
        get_uint16_from_string(&argv[2], &len);
    }

    if (argc >= 4) {
        get_uint16_from_string(&argv[3], &rate_kbps);
    }

    spp_tp_start(idx, len, (uint32_t)rate_kbps * 1024);
}

/* spp_tp_stop [idx|all]  (default all) */
BT_SPP_CLI(throughput_stop)
{
    int idx = -1;

    if (argc >= 2 && strcmp(argv[1], "all") != 0) {
        uint8_t v = 0;
        get_uint8_from_string(&argv[1], &v);
        idx = v;
    }

    spp_tp_stop(idx);
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

void print_br_bond_info(const struct bt_br_bond_info *info, void *user_data)
{
    char addr_str[BT_ADDR_STR_LEN];
    char key_str[33];
    
    bt_addr_to_str(info->addr, addr_str, sizeof(addr_str));
    
    for (size_t i = 0; i < info->link_key_size; i++) {
        sprintf(&key_str[i*2], "%02X", info->link_key[i]);
    }
    key_str[32] = '\0';
    
    printf("BR/EDR Bonded Device:\n");
    printf("  Address: %s\n", addr_str);
    printf("  Link Key: %s\n", key_str);
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

BT_CLI(get_all_conn_info)
{
    struct bt_conn_info info[CONFIG_BT_ACL_CONN];
    char br_addr[BT_ADDR_STR_LEN];
    int link_num;

    link_num = bt_conn_get_remote_dev_info(info, BT_CONN_TYPE_BR);
    printf("br connected devices count: %d\r\n", link_num);
    for (int i = 0; i < link_num; i++) {
        bt_addr_to_str(info[i].br.dst, br_addr, sizeof(br_addr));
        printf("[%d]: bredr address %s\r\n", i, br_addr);
    }
}

BT_CLI(get_bond_list)
{
    bt_br_foreach_bond(print_br_bond_info, NULL);
}

int bredr_cli_register(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //aos_cli_register_commands(bredr_cmd_set, sizeof(bredr_cmd_set)/sizeof(bredr_cmd_set[0]));
    return 0;
}
