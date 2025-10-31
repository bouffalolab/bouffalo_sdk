/*
 *  Copyright (C) Bouffalo Lab 2016-2023
 *  SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>

#include "blctl.h"
#include "../rnm_msg.h"
#include "../common.h"
#include "utils.h"
#include "blctl_log.h"
#include "version.h"

#define MIN_ARG_LEN 2

#define ARG_LEN_CONNECT_CIPHER_AP (MIN_ARG_LEN + 2)
#define ARG_LEN_CONNECT_OPEN_AP   (MIN_ARG_LEN + 1)
#define ARG_LEN_DISCONNECT_AP     (MIN_ARG_LEN + 0)
#define ARG_LEN_OTA               (MIN_ARG_LEN + 1)
#define ARG_LEN_CODE              (MIN_ARG_LEN + 1)
#define ARG_LEN_START_CIPHER_AP   (MIN_ARG_LEN + 2)
#define ARG_LEN_START_OPEN_AP     (MIN_ARG_LEN + 1)
#define ARG_LEN_GET_LINK_STATUS   (MIN_ARG_LEN + 0)
#define ARG_LEN_WIFI_SCAN         (MIN_ARG_LEN + 0)
#define ARG_LEN_SET_AUTO_CONNECT  (MIN_ARG_LEN + 1)

static blctl_handle_t ctl = NULL;

void usage(const char *prog)
{
    fprintf(stderr, "blctl version " BLCTL_VERSION "\n\n"
                    "Usage: %s <cmd> [cmd arg]\n"
                    "          <cmd>:\n"
                    "               connect_ap <ssid> [password]\n"
                    "                   Connect to router\n"
                    "               disconnect_ap\n"
                    "                   Disconnect router\n"
                    "               start_ap <ssid> [password]\n"
                    "                   Start AP with ssid and password. If password omitted, an open AP is created\n"
                    "               stop_ap\n"
                    "                   Stop AP\n"
                    "               wifi_scan\n"
                    "                   Scan wifi\n"
                    "               wifi_scan_results\n"
                    "                   Scan results\n"
                    "               set_auto_connect <cfg>\n"
                    "                   Set auto reconnect policy, cfg is one of: 1, 0\n"
                    "               get_link_status\n"
                    "                   Get link status\n"
                    "               ota        <ota.bin.ota>\n"
                    "                   Upgrade card firmware\n"
                    "               user_ext\n"
                    "                   Demo sending&receiving user ext msg\n"
                    "               user_ext_rsp\n"
                    "                   Demo sending&receiving user ext msg\n"
                    "               user_ext_no_rsp\n"
                    "                   Demo sending but no receiving user ext msg\n"
                    "               reboot\n"
                    "                   Reboot card\n"
                    "               version\n"
                    "                   Display this message\n"
            , prog);
    exit(-1);
}

int wifi_scan()
{
    rnm_base_msg_t cmd;

    memset(&cmd, 0, sizeof(cmd));
    cmd.cmd = BF1B_CMD_SCAN;

    return blctl_rnm_msg_expect_simple(ctl, (void *)&cmd, sizeof(cmd), NULL, 100);
}

int wifi_scan_results()
{
    int ret;
    rnm_base_msg_t cmd;
    rnm_scan_ind_msg_t *rsp;
    int i;

    memset(&cmd, 0, sizeof(cmd));
    cmd.cmd = BF1B_CMD_SCAN_RESULTS;

    if ((ret = blctl_rnm_msg_expect(ctl, &cmd, sizeof(cmd), (void *)&rsp, 100)))
        return ret;
    for (i = 0; i < rsp->num; ++i) {
        struct bf1b_wifi_scan_record *r = &rsp->records[i];
        printf("BSSID " MAC_FMT ", channel %u, rssi %d, auth %s, cipher %s, SSID %s\n",
                ARR_ELE_6(r->bssid), r->channel, r->rssi,
                blctl_auth_to_str(r->auth_mode), blctl_cipher_to_str(r->cipher), r->ssid);
    }
    free(rsp);

    return 0;
}

int connect_ap(const char *ssid, const int ssid_len, const char *password, const int pwd_len)
{
    int ret;
    rnm_sta_connect_msg_t cmd;
    uint16_t status = -1;

    memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.cmd = BF1B_CMD_STA_CONNECT;
    cmd.ssid_len = ssid_len;
    memcpy(cmd.ssid, ssid, ssid_len);
    if (password) {
        memcpy(cmd.password, password, pwd_len);
    }
    if ((ret = blctl_rnm_msg_expect_simple(ctl, (void *)&cmd, sizeof(cmd), &status, 100))) {
        bl_loge("failed to connect to AP, %d\n", ret);
        return ret;
    }

    return 0;
}

int disconnect_ap()
{
    int ret;
    rnm_base_msg_t cmd;
    uint16_t status;

    memset(&cmd, 0, sizeof(cmd));
    cmd.cmd = BF1B_CMD_STA_DISCONNECT;

    if ((ret = blctl_rnm_msg_expect_simple(ctl, (void *)&cmd, sizeof(cmd), &status, 100))) {
        bl_loge("failed to disconnect from AP, %d\n", ret);
        return ret;
    }
    return 0;
}

int stop_ap()
{
    rnm_base_msg_t cmd;
    uint16_t status;

    memset(&cmd, 0, sizeof(cmd));
    cmd.cmd = BF1B_CMD_AP_STOP;

    return blctl_rnm_msg_expect_simple(ctl, (void *)&cmd, sizeof(cmd), &status, 100);
}

int start_ap(const char *ssid, const int ssid_len, const char *password, const int pwd_len, uint8_t is_open)
{
    rnm_ap_start_msg_t cmd;

    memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.cmd = BF1B_CMD_AP_START;
    cmd.ssid_len = ssid_len;
    memcpy(cmd.ssid, ssid, ssid_len);
    memcpy(cmd.password, password, pwd_len);
	cmd.is_open = is_open;
	cmd.channel = 1;

    return blctl_rnm_msg_expect_simple(ctl, (void *)&cmd, sizeof(cmd), NULL, 100);
}

int set_auto_connect(const char *opt)
{
    rnm_sta_set_auto_reconnect_msg_t cmd;

    memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.cmd = BF1B_CMD_STA_SET_AUTO_RECONNECT;
    if (opt[0] == '1') {
        cmd.en = 1;
    } else {
        cmd.en = 0;
    }

    return blctl_rnm_msg_expect_simple(ctl, (void *)&cmd, sizeof(cmd), NULL, 100);
}

int blctl_handle_one_rnm(blctl_handle_t handle, rnm_base_msg_t *msg,
        void *data, int len);

static void wildcard_msg_handler(void *msg, void *arg)
{
    int len;
    unsigned char *ptr;
    ctl_port_msg_hdr_t *hdr = msg;

    switch (hdr->type) {
    case CTL_PORT_MSG_TRANSPARENT_DEVICE2HOST:
    {
        // Only care about such message
        rnm_base_msg_t *rm = (rnm_base_msg_t *)hdr->payload;
        if (rm->cmd == BF1B_CMD_USER_EXT) {
            const char *resp = (char *)((rnm_user_ext_msg_t *)rm)->payload;
            bl_logi("Response: %s\n", resp);
        }
        len = hdr->length - sizeof(*hdr);
        ptr = hdr->payload;
        ptr += sizeof(*rm);
        len -= sizeof(*rm);
        blctl_handle_one_rnm(ctl, rm, ptr, len);
    }
        break;
    case CTL_PORT_MSG_IP_UPDATE:
    case CTL_PORT_MSG_DISCONNECT:
    default:
        bl_loge("%s: no handler for type %u\n", __func__, hdr->type);
        break;
    }

    // Don't forget to free the message
    free(msg);
}

int demo_user_ext(void)
{
    int ret;
    const char m[] = "hello, world!";

    if ((ret = blctl_rnm_user_ext_send(ctl, m, sizeof(m))))
        return ret;
    usleep(100 * 1000);

    return 0;
}

int user_ext_cmd_example(uint8_t is_response)
{
    int ret;
    const char m[] = "hello, world!";
    
    blctl_register_wildcard_msg_handler(ctl, wildcard_msg_handler, ctl);
    if (is_response) {
        if ((ret = blctl_rnm_user_send_extension(ctl, m, sizeof(m), BF1B_CMD_USER_EXT_RSP))) {
            return ret;
        }
    } else {
        if ((ret = blctl_rnm_user_send_extension(ctl, m, sizeof(m), BF1B_CMD_USER_EXT_NO_RSP))) {
            return ret;
        }
    }

    usleep(100 * 1000);

    return 0;
}

int reboot(void)
{
    rnm_base_msg_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    cmd.cmd = BF1B_CMD_REBOOT;
    return blctl_rnm_msg_expect_simple(ctl, (void *)&cmd, sizeof(cmd), NULL, 100);
}

int get_link_status()
{
    int ret;
    rnm_base_msg_t cmd;
    rnm_sta_link_status_ind_msg_t *rsp;

    memset(&cmd, 0, sizeof(cmd));
    cmd.cmd = BF1B_CMD_STA_GET_LINK_STATUS;

    if ((ret = blctl_rnm_msg_expect(ctl, &cmd, sizeof(cmd), (void *)&rsp, 100)))
        return ret;
    if (rsp->record.link_status == BF1B_WIFI_LINK_STATUS_UP) {
        printf("UP RSSI %d\n", rsp->record.rssi);
    } else {
        printf("DOWN\n");
    }
    free(rsp);

    return 0;
}

static void exit_handler(void)
{
    blctl_deinit(ctl);
}

static void sig_handler(int signum)
{
    (void)signum;
    exit(0);
}

#define IS_CMD(left, right) (!strcmp(left, right))

int main(int argc, char *argv[])
{
    int ret = 0;
    const char *cmd;

    fprintf(stdout, "blctl version %s\n", BLCTL_VERSION);
    atexit(exit_handler);
    signal(SIGINT, sig_handler);

    if (argc < MIN_ARG_LEN) {
        usage(*argv);
    }

    if ((ret = blctl_init(&ctl))) {
        bl_loge("blctl init failed, %d\n", ret);
        ret = 1;
        goto ret;
    }

    blctl_register_wildcard_msg_handler(ctl, wildcard_msg_handler, NULL);
    cmd = argv[1];
    if (IS_CMD(cmd, "connect_ap")) {
		switch (argc) {
        case ARG_LEN_CONNECT_CIPHER_AP:
            connect_ap(argv[2], strlen(argv[2]), argv[3], strlen(argv[3]));
            break;
        case ARG_LEN_CONNECT_OPEN_AP:
            connect_ap(argv[2], strlen(argv[2]), NULL, 0);
            break;
        default:
            usage(*argv);
        }
    } else if (IS_CMD(cmd, "disconnect_ap")) {
        disconnect_ap();
    } else if (IS_CMD(cmd, "start_ap")) {
		switch (argc){
			case ARG_LEN_START_CIPHER_AP:
                start_ap(argv[2], strlen(argv[2]), argv[3], strlen(argv[3]), 0);
			break;
			case ARG_LEN_START_OPEN_AP:
                start_ap(argv[2], strlen(argv[2]), "1", 1, 1);
			break;
			default:
                usage(*argv);
        }
    } else if (IS_CMD(cmd, "stop_ap")) {
        stop_ap();
    } else if (IS_CMD(cmd, "wifi_scan")) {
        if (argc == ARG_LEN_WIFI_SCAN) {
            wifi_scan();
        } else {
            usage(*argv);
        }
    } else if (IS_CMD(cmd, "wifi_scan_results")) {
        wifi_scan_results();
    } else if (IS_CMD(cmd, "set_auto_connect")) {
        if (argc == ARG_LEN_SET_AUTO_CONNECT) {
            set_auto_connect(argv[2]);
        } else {
            usage(*argv);
        }
	} else if (IS_CMD(cmd, "ota")) {
        if (argc != ARG_LEN_OTA) {
            usage(*argv);
        }
        blctl_ota(ctl, argv[2]);
	} else if (IS_CMD(cmd, "user_ext")) {
        demo_user_ext();
    } else if (IS_CMD(cmd, "user_ext_rsp")) {
        user_ext_cmd_example(1);
    } else if (IS_CMD(cmd, "user_ext_no_rsp")) {
        user_ext_cmd_example(0);
    }else if (IS_CMD(cmd, "version")) {
        usage(*argv);
    } else if (IS_CMD(cmd, "get_link_status")) {
        if (argc < ARG_LEN_GET_LINK_STATUS) {
            usage(*argv);
        }
        get_link_status();
    } else if (IS_CMD(cmd, "reboot")) {
        reboot();
    } else {
        usage(*argv);
    }

ret:
    return ret;
}
