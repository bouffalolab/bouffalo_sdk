/****************************************************************************
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rtos_al.h"
#include "shell.h"
#include "wifi_mgmr_ext.h"

#define WIFI_P2P_JOIN_FIND_POLL_MS 200U

typedef struct wifi_p2p_help_entry
{
    const char *name;
    const char *usage;
    const char *details;
    const char *example;
} wifi_p2p_help_entry_t;

static const wifi_p2p_help_entry_t wifi_p2p_help_entries[] = {
    {
        "p2p_help",
        "p2p_help [command]",
        "Show detailed help for all commands or for one exact command name.",
        "p2p_help p2p_connect",
    },
    {
        "p2p_init",
        "p2p_init [channel]",
        "Initialize fixed interface wl2/VIF 1, clear stale discovery state, "
        "enable WPS methods, and select a 2.4/5 GHz P2P channel. Without a "
        "channel, the active STA channel is reused; otherwise channel 1 is "
        "used. Dynamic group interfaces are not supported.",
        "p2p_init 36",
    },
    {
        "p2p_channel",
        "p2p_channel <channel>",
        "Set operating, listen, and preferred P2P channel. The channel must "
        "be valid for 2.4/5 GHz and allowed by country, NO_IR, and radar "
        "rules. Requires initialized P2P without negotiation/active group.",
        "p2p_channel 6",
    },
    {
        "p2p_config",
        "p2p_config <channel> [no_group_iface]",
        "Reconfigure channel and fixed-interface policy. no_group_iface "
        "defaults to 1 and 0 is rejected. This does not initialize a missing "
        "VIF; run p2p_init first.",
        "p2p_config 36 1",
    },
    {
        "p2p_name",
        "p2p_name <device-name>",
        "Set the advertised P2P/WPS device name. The name must be 1..32 "
        "bytes and cannot contain CR/LF. Set it before negotiation or group "
        "formation.",
        "p2p_name bl618-p2p",
    },
    {
        "p2p_find",
        "p2p_find [timeout] [freq=<MHz>] [dev_id=<peer-mac>]",
        "Start discovery. timeout is the optional first positional argument "
        "in seconds; omit it for no configured timeout. freq restricts the "
        "search frequency. dev_id is the target P2P Device Address. freq and "
        "dev_id may each appear once. Results and completion are asynchronous.",
        "p2p_find 30 freq=5180 dev_id=02:11:22:33:44:55",
    },
    {
        "p2p_stop_find",
        "p2p_stop_find",
        "Stop current find/listen operation while retaining the initialized "
        "VIF and peer cache. It is not valid during negotiation or an active "
        "group.",
        "p2p_stop_find",
    },
    {
        "p2p_listen",
        "p2p_listen [seconds] [channel]",
        "Enter listen state. seconds defaults to 30. channel 0 or omission "
        "keeps the current channel; a nonzero channel first updates operating, "
        "listen, and preferred P2P channels. Incoming GO negotiation requests "
        "are reported as events for application policy; they are not accepted "
        "automatically. Expiration is asynchronous.",
        "p2p_listen 60 36",
    },
    {
        "p2p_peers",
        "p2p_peers [discovered]",
        "Print cached peer Device Addresses. discovered excludes entries "
        "learned only from Probe Requests. The API stores at most 8 addresses "
        "and reports when the supplicant returned more.",
        "p2p_peers discovered",
    },
    {
        "p2p_flush",
        "p2p_flush",
        "Stop discovery and clear cached peer/provision-discovery state. The "
        "fixed VIF and any active group are preserved; use p2p_stop for full "
        "teardown.",
        "p2p_flush",
    },
    {
        "p2p_peer",
        "p2p_peer <peer-mac>",
        "Query one cached P2P Device Address and print parsed device identity, "
        "configuration methods, capabilities, and signal level.",
        "p2p_peer 02:11:22:33:44:55",
    },
    {
        "p2p_peer_raw",
        "p2p_peer_raw <peer-mac>",
        "Print the raw supplicant P2P_PEER response for diagnostics. Output "
        "uses a 1024-byte buffer and explicitly reports truncation.",
        "p2p_peer_raw 02:11:22:33:44:55",
    },
    {
        "p2p_connect",
        "p2p_connect <peer-mac> <pbc|pin|PIN> [display|keypad] "
        "[join|auto|auth] [go_intent=<0..15>] [freq=<MHz>]",
        "pbc must be explicit. 'pin' or 'pin display' generates and prints an "
        "automatic display PIN. A 4/8-digit PIN defaults to keypad; append "
        "display for a fixed display PIN. auto joins a running GO or falls back "
        "to GO Negotiation. join, auto, and auth are mutually exclusive. "
        "auth only authorizes a later peer request and keeps lifecycle state. "
        "Without freq, an active STA frequency is reused. Final result is "
        "asynchronous.",
        "p2p_connect 02:11:22:33:44:55 pin display go_intent=7",
    },
    {
        "p2p_cancel",
        "p2p_cancel",
        "Issue the standard P2P_CANCEL command for in-progress formation, "
        "join, or invitation work. It preserves the VIF and active group; it "
        "is distinct from p2p_stop_find and p2p_stop.",
        "p2p_cancel",
    },
    {
        "p2p_group_add",
        "p2p_group_add [persistent|persistent=<id>] [freq=<MHz>] "
        "[max_oper_chwidth=<20|40|80>] [ht40] "
        "[vht] [he] [passphrase=<8..63 chars>] "
        "[passphrase_hex=<16..126 hex chars>]",
        "Create an autonomous or persistent GO. persistent and persistent=<id> "
        "are mutually exclusive. max_oper_chwidth requests the maximum channel "
        "width. passphrase accepts 8..63 printable characters; passphrase_hex "
        "supports values that need hexadecimal transport. Neither can replace a "
        "saved persistent group's passphrase. Group start/failure is "
        "asynchronous; verify the negotiated width from the PHY status.",
        "p2p_group_add freq=5180 ht40 vht he passphrase=12345678",
    },
    {
        "list_networks",
        "list_networks",
        "List saved persistent-group profiles using the standard supplicant "
        "LIST_NETWORKS command. Use the reported network ID with "
        "p2p_group_add persistent=<id>, p2p_invite persistent=<id>, or "
        "remove_network <id>. Profiles are restored from PSM by p2p_init.",
        "list_networks",
    },
    {
        "remove_network",
        "remove_network <id>",
        "Delete one saved persistent-group profile using the standard "
        "supplicant REMOVE_NETWORK command and write the updated list to PSM. "
        "The ID must come from list_networks.",
        "remove_network 0",
    },
    {
        "p2p_group_remove",
        "p2p_group_remove",
        "Remove the active wl2 group while preserving the initialized fixed "
        "P2P VIF. The command requires GROUP_ACTIVE state.",
        "p2p_group_remove",
    },
    {
        "p2p_wps_pbc",
        "p2p_wps_pbc [peer-mac]",
        "Start standard WPS_PBC on wl2, optionally restricted by the peer P2P "
        "Device Address. Normally run this after the local GO group has started.",
        "p2p_wps_pbc 02:11:22:33:44:55",
    },
    {
        "p2p_prov_disc",
        "p2p_prov_disc <peer-mac> [pbc|pin|display|keypad] [join|auto]",
        "Start Provision Discovery with a cached peer. The method defaults to "
        "pbc; pin is an alias for display. join and auto are mutually exclusive. "
        "The response, displayed PIN, keypad request, or failure is reported "
        "asynchronously.",
        "p2p_prov_disc 02:11:22:33:44:55 display join",
    },
    {
        "p2p_invite",
        "p2p_invite group=wl2 peer=<mac> [go_dev_addr=<mac>]\r\n"
        "         p2p_invite persistent=<id> [peer=<mac>] [freq=<MHz>] "
        "[pref=<MHz>] [max_oper_chwidth=<20|40|80>] [ht40] [vht] [he]",
        "Active-group mode requires GROUP_ACTIVE and peer; go_dev_addr is "
        "optional. For a persistent profile where this device is GO, peer is "
        "required and must already be in the peer cache; discover it with "
        "p2p_find first. peer may be omitted when the saved peer is GO. "
        "Frequency and PHY options apply only to persistent reinvocation. "
        "Unsupported channel capabilities are rejected by supplicant. The "
        "invitation result is asynchronous.",
        "p2p_invite persistent=1 peer=02:11:22:33:44:55 freq=2437 he",
    },
    {
        "p2p_join",
        "p2p_join [peer=<mac>|<peer-mac>] [name=<substring>] "
        "[freq=<MHz>] [channel=<channel>] [go_intent=<0..15>] "
        "[find=<seconds>] [wait=<seconds>] [no_find]",
        "Join a peer with PBC. This command initializes P2P, optionally "
        "runs discovery for 60 seconds by default, selects a target by Device "
        "Address or device name, selects join mode, and waits up to 35 "
        "seconds for COMPLETED. A discovered GO is joined explicitly; "
        "otherwise auto mode may use GO Negotiation. go_intent only "
        "controls that negotiation; omit it to use the supplicant default. "
        "Higher values prefer GO, and 15 requires GO. Both peers must support "
        "PBC. Discovery stops as soon as peer=<mac> or name=<substring> "
        "matches. Prefer peer=<mac> because device names may not be unique. "
        "freq and channel must match; no_find skips discovery.",
        "p2p_join peer=02:11:22:33:44:55 freq=2437 go_intent=7 find=60 wait=35",
    },
    {
        "p2p_stop",
        "p2p_stop",
        "Fully stop fixed-interface P2P: stop discovery, remove groups, flush "
        "cached state, remove wl2 from the supplicant, and release VIF ownership.",
        "p2p_stop",
    },
    {
        "p2p_status",
        "p2p_status",
        "Print the local lifecycle state and a synchronous supplicant snapshot: "
        "role-related mode, BSSID, frequency, SSID, WPA state, P2P Device "
        "Address, and interface address when available.",
        "p2p_status",
    },
};

static int wifi_p2p_print_help(const char *command)
{
    size_t i;
    int found = 0;

    printf("P2P demo help (fixed interface %s, VIF %d):\r\n",
           WIFI_MGMR_P2P_DEFAULT_IFACE, WIFI_MGMR_P2P_FHOST_VIF_IDX);
    for (i = 0; i < sizeof(wifi_p2p_help_entries) /
                        sizeof(wifi_p2p_help_entries[0]); i++) {
        const wifi_p2p_help_entry_t *entry = &wifi_p2p_help_entries[i];

        if (command && strcmp(command, entry->name) != 0)
            continue;
        found = 1;
        printf("\r\n%s\r\n", entry->name);
        printf("  Usage: %s\r\n", entry->usage);
        printf("  Details: %s\r\n", entry->details);
        printf("  Example: %s\r\n", entry->example);
    }

    if (!found) {
        printf("p2p_help: unknown command '%s'\r\n", command);
        printf("Run p2p_help without an argument to list all commands.\r\n");
        return -1;
    }

    printf("\r\nCommon rules:\r\n");
    printf("  Run p2p_init before discovery, listen, connect, or group commands.\r\n");
    printf("  p2p_join initializes P2P itself.\r\n");
    printf("  MAC parameters use xx:xx:xx:xx:xx:xx P2P Device Addresses.\r\n");
    printf("  Command return 0 means the supplicant accepted the request.\r\n");
    printf("  Use P2P events and p2p_status for asynchronous protocol results.\r\n");
    return 0;
}

static void wifi_p2p_usage(void)
{
    (void)wifi_p2p_print_help(NULL);
}

static int wifi_p2p_parse_uint(const char *s, unsigned int max_value,
                               unsigned int *value)
{
    unsigned int parsed = 0;

    if (!s || !s[0] || !value)
        return -1;

    while (*s) {
        unsigned int digit;

        if (*s < '0' || *s > '9')
            return -1;
        digit = (unsigned int)(*s - '0');
        if (parsed > max_value / 10 ||
            (parsed == max_value / 10 && digit > max_value % 10))
            return -1;
        parsed = parsed * 10 + digit;
        s++;
    }

    *value = parsed;
    return 0;
}

static int wifi_p2p_pin_valid(const char *pin)
{
    size_t len;

    if (!pin)
        return 0;
    len = strlen(pin);
    if (len != 4 && len != 8)
        return 0;

    while (*pin) {
        if (*pin < '0' || *pin > '9')
            return 0;
        pin++;
    }

    return 1;
}

static int wifi_p2p_parse_connect_method(
    int argc, char **argv, wifi_mgmr_p2p_method_t *method,
    const char **pin, int *next_arg)
{
    int i = 3;

    if (argc < 3 || !argv || !method || !pin || !next_arg)
        return -1;

    *pin = NULL;
    if (strcmp(argv[2], "pbc") == 0) {
        *method = WIFI_MGMR_P2P_METHOD_PBC;
    } else if (strcmp(argv[2], "pin") == 0) {
        *method = WIFI_MGMR_P2P_METHOD_PIN_DISPLAY;
        if (i < argc && strcmp(argv[i], "display") == 0)
            i++;
    } else if (wifi_p2p_pin_valid(argv[2])) {
        *pin = argv[2];
        *method = WIFI_MGMR_P2P_METHOD_PIN_KEYPAD;
        if (i < argc && strcmp(argv[i], "display") == 0) {
            *method = WIFI_MGMR_P2P_METHOD_PIN_DISPLAY;
            i++;
        } else if (i < argc && strcmp(argv[i], "keypad") == 0) {
            i++;
        }
    } else {
        return -1;
    }

    *next_arg = i;
    return 0;
}

static int wifi_p2p_hex_value(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;

    return -1;
}

static void wifi_p2p_clear_secret(void *buf, size_t len)
{
    volatile uint8_t *pos = buf;

    while (len--)
        *pos++ = 0;
}

static int wifi_p2p_parse_passphrase(
    const char *value,
    char passphrase[WIFI_MGMR_P2P_PASSPHRASE_MAX_LEN + 1])
{
    size_t len;
    size_t i;

    if (!value || !passphrase)
        return -1;

    wifi_p2p_clear_secret(passphrase,
                          WIFI_MGMR_P2P_PASSPHRASE_MAX_LEN + 1);
    len = strnlen(value, WIFI_MGMR_P2P_PASSPHRASE_MAX_LEN + 1);
    if (len < WIFI_MGMR_P2P_PASSPHRASE_MIN_LEN ||
        len > WIFI_MGMR_P2P_PASSPHRASE_MAX_LEN) {
        return -1;
    }

    for (i = 0; i < len; i++) {
        uint8_t byte = (uint8_t)value[i];

        if (byte < 32 || byte == 127)
            return -1;
    }

    memcpy(passphrase, value, len);
    passphrase[len] = '\0';
    return 0;
}

static int wifi_p2p_parse_passphrase_hex(
    const char *hex,
    char passphrase[WIFI_MGMR_P2P_PASSPHRASE_MAX_LEN + 1])
{
    size_t hex_len;
    size_t passphrase_len;
    size_t i;

    if (!hex || !passphrase)
        return -1;

    wifi_p2p_clear_secret(passphrase,
                          WIFI_MGMR_P2P_PASSPHRASE_MAX_LEN + 1);
    hex_len = strnlen(hex, WIFI_MGMR_P2P_PASSPHRASE_MAX_LEN * 2 + 1);
    if ((hex_len & 1) != 0 ||
        hex_len < WIFI_MGMR_P2P_PASSPHRASE_MIN_LEN * 2 ||
        hex_len > WIFI_MGMR_P2P_PASSPHRASE_MAX_LEN * 2) {
        return -1;
    }

    passphrase_len = hex_len / 2;
    for (i = 0; i < passphrase_len; i++) {
        int hi = wifi_p2p_hex_value(hex[i * 2]);
        int lo = wifi_p2p_hex_value(hex[i * 2 + 1]);
        uint8_t value;

        if (hi < 0 || lo < 0)
            goto fail;
        value = (uint8_t)((hi << 4) | lo);
        if (value < 32 || value == 127)
            goto fail;
        passphrase[i] = (char)value;
    }
    passphrase[passphrase_len] = '\0';
    return 0;

fail:
    wifi_p2p_clear_secret(passphrase,
                          WIFI_MGMR_P2P_PASSPHRASE_MAX_LEN + 1);
    return -1;
}

static int wifi_p2p_parse_mac(
    const char *str, uint8_t mac[WIFI_MGMR_P2P_ADDR_LEN])
{
    int i;

    if (!str || !mac || strlen(str) != WIFI_MGMR_P2P_ADDR_STR_LEN - 1)
        return -1;

    for (i = 0; i < WIFI_MGMR_P2P_ADDR_LEN; i++) {
        int hi = wifi_p2p_hex_value(str[i * 3]);
        int lo = wifi_p2p_hex_value(str[i * 3 + 1]);

        if (hi < 0 || lo < 0 ||
            (i < WIFI_MGMR_P2P_ADDR_LEN - 1 && str[i * 3 + 2] != ':'))
            return -1;
        mac[i] = (uint8_t)((hi << 4) | lo);
    }

    return 0;
}

static int wifi_p2p_parse_find_args(
    int argc, char **argv, wifi_mgmr_p2p_find_params_t *config)
{
    unsigned int parsed;
    uint8_t timeout_seen = 0;
    uint8_t freq_seen = 0;
    uint8_t dev_id_seen = 0;
    int i;

    if (argc < 1 || !argv || !config) {
        return -1;
    }

    memset(config, 0, sizeof(*config));
    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "freq=", 5) == 0) {
            if (freq_seen ||
                wifi_p2p_parse_uint(argv[i] + 5, UINT16_MAX, &parsed) ||
                parsed == 0) {
                return -1;
            }
            config->freq = (uint16_t)parsed;
            freq_seen = 1;
        } else if (strncmp(argv[i], "dev_id=", 7) == 0) {
            if (dev_id_seen ||
                wifi_p2p_parse_mac(argv[i] + 7, config->dev_id)) {
                return -1;
            }
            config->dev_id_valid = 1;
            dev_id_seen = 1;
        } else if (i == 1 && !timeout_seen &&
                   wifi_p2p_parse_uint(argv[i], UINT16_MAX, &parsed) == 0) {
            config->timeout = (uint16_t)parsed;
            timeout_seen = 1;
        } else {
            return -1;
        }
    }

    return 0;
}

static int wifi_p2p_freq_from_channel(int channel);
static int wifi_p2p_channel_valid(int channel);
static int wifi_p2p_channel_from_freq(int freq);

static int wifi_p2p_sta_channel(void)
{
    uint8_t channel;

    if (wifi_mgmr_sta_channel_get(&channel))
        return 0;

    return channel;
}

static int wifi_p2p_freq_from_channel(int channel)
{
    if (channel >= 1 && channel <= 13)
        return 2407 + channel * 5;
    if ((channel >= 36 && channel <= 144 && channel % 4 == 0) ||
        (channel >= 149 && channel <= 177 && (channel - 149) % 4 == 0))
        return 5000 + channel * 5;

    return 0;
}

static int wifi_p2p_channel_valid(int channel)
{
    int freq = wifi_p2p_freq_from_channel(channel);

    return freq && channel != 14;
}

static const char *wifi_p2p_state_string(wifi_mgmr_p2p_state_t state)
{
    switch (state) {
    case WIFI_MGMR_P2P_STATE_STOPPED:
        return "STOPPED";
    case WIFI_MGMR_P2P_STATE_IDLE:
        return "IDLE";
    case WIFI_MGMR_P2P_STATE_FINDING:
        return "FINDING";
    case WIFI_MGMR_P2P_STATE_LISTENING:
        return "LISTENING";
    case WIFI_MGMR_P2P_STATE_NEGOTIATING:
        return "NEGOTIATING";
    case WIFI_MGMR_P2P_STATE_GROUP_ACTIVE:
        return "GROUP_ACTIVE";
    case WIFI_MGMR_P2P_STATE_STOPPING:
        return "STOPPING";
    default:
        return "UNKNOWN";
    }
}

static int wifi_p2p_result(const char *operation, int ret)
{
    if (ret) {
        printf("p2p: %s failed ret=%d reason=%s state=%s\r\n",
               operation, ret, wifi_mgmr_p2p_error_string(ret),
               wifi_p2p_state_string(wifi_mgmr_p2p_state_get()));
    }

    return ret;
}

static void wifi_p2p_format_mac(const uint8_t mac[WIFI_MGMR_P2P_ADDR_LEN],
                                char *buf, size_t buf_len)
{
    if (!mac || !buf || buf_len == 0)
        return;

    snprintf(buf, buf_len, "%02x:%02x:%02x:%02x:%02x:%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

static int wifi_p2p_sta_freq(void)
{
    int channel = wifi_p2p_sta_channel();

    if (channel == 0)
        return 0;

    if (!wifi_p2p_channel_valid(channel))
        return -1;

    return wifi_p2p_freq_from_channel(channel);
}

static int wifi_p2p_channel_from_freq(int freq)
{
    int channel;

    if (freq >= 2412 && freq <= 2472 && ((freq - 2407) % 5) == 0)
        return (freq - 2407) / 5;
    if (freq >= 5170 && freq <= 5885 && ((freq - 5000) % 5) == 0) {
        channel = (freq - 5000) / 5;
        if (wifi_p2p_channel_valid(channel))
            return channel;
    }

    return 0;
}

static int wifi_p2p_init_channel(int channel, int channel_forced)
{
    int sta_channel;
    int ret;

    if (!channel_forced) {
        sta_channel = wifi_p2p_sta_channel();
        if (sta_channel > 0) {
            channel = sta_channel;
            printf("p2p: using active STA channel %d for coexistence\r\n",
                   channel);
            if (!wifi_p2p_channel_valid(channel)) {
                printf("p2p: active STA channel is not a supported P2P channel\r\n");
                return -1;
            }
        } else {
            channel = 1;
        }
    }

    if (!wifi_p2p_channel_valid(channel)) {
        printf("p2p: channel must be a valid 2.4/5 GHz channel\r\n");
        return -1;
    }

    /* Discovery must always start on social channels. */
    ret = wifi_mgmr_p2p_init(channel, 1);
    if (ret) {
        return wifi_p2p_result("init", ret);
    }

    printf("p2p: %s initialized on channel %d\r\n",
           WIFI_MGMR_P2P_DEFAULT_IFACE, channel);

    return 0;
}

static int cmd_wifi_p2p_help(int argc, char **argv)
{
    if (argc > 2) {
        printf("Usage: p2p_help [command]\r\n");
        return -1;
    }

    return wifi_p2p_print_help(argc == 2 ? argv[1] : NULL);
}

/*
 * Legacy helper retained for reference. It only creates or rebinds the fixed
 * P2P VIF and can leave setup incomplete; use p2p_init for full initialization.
static int cmd_wifi_p2p_iface_init(int argc, char **argv)
{
    int ret;

    (void)argv;
    if (argc != 1) {
        wifi_p2p_usage();
        return -1;
    }

    ret = wifi_mgmr_p2p_iface_init();
    if (ret == 0)
        printf("p2p: %s interface initialized\r\n",
               WIFI_MGMR_P2P_DEFAULT_IFACE);
    return wifi_p2p_result("iface_init", ret);
}
 */

static int cmd_wifi_p2p_init(int argc, char **argv)
{
    int channel = 0;
    int channel_forced = 0;
    unsigned int parsed;

    if (argc > 2 ||
        (argc > 1 && wifi_p2p_parse_uint(argv[1], INT_MAX, &parsed))) {
        wifi_p2p_usage();
        return -1;
    }

    if (argc > 1) {
        channel = (int)parsed;
        channel_forced = 1;
    }

    return wifi_p2p_init_channel(channel, channel_forced);
}

static int cmd_wifi_p2p_channel(int argc, char **argv)
{
    unsigned int channel;
    int ret;

    if (argc != 2 || wifi_p2p_parse_uint(argv[1], UINT8_MAX, &channel) ||
        !wifi_p2p_channel_valid((int)channel)) {
        printf("p2p: channel must be a valid 2.4/5 GHz channel\r\n");
        return -1;
    }

    ret = wifi_mgmr_p2p_set_channel((uint8_t)channel);
    if (ret == 0)
        printf("p2p: channel configured to %u\r\n", channel);
    return wifi_p2p_result("set_channel", ret);
}

static int cmd_wifi_p2p_config(int argc, char **argv)
{
    unsigned int channel;
    unsigned int no_group_iface = 1;
    int ret;

    if (argc < 2 || argc > 3 ||
        wifi_p2p_parse_uint(argv[1], UINT8_MAX, &channel) ||
        !wifi_p2p_channel_valid((int)channel) ||
        (argc == 3 &&
         wifi_p2p_parse_uint(argv[2], 1, &no_group_iface))) {
        wifi_p2p_usage();
        return -1;
    }

    if (!no_group_iface) {
        printf("p2p: no-group-iface=0 is not supported by fixed VIF mode\r\n");
        return wifi_p2p_result("config", WIFI_MGMR_P2P_ERR_NOT_SUPPORTED);
    }

    ret = wifi_mgmr_p2p_config((uint8_t)channel,
                               (uint8_t)no_group_iface);
    if (ret == 0) {
        printf("p2p: config channel=%u no_group_iface=%u\r\n", channel,
               no_group_iface);
    }
    return wifi_p2p_result("config", ret);
}

static int cmd_wifi_p2p_name(int argc, char **argv)
{
    if (argc != 2) {
        wifi_p2p_usage();
        return -1;
    }

    return wifi_p2p_result("set_device_name",
                           wifi_mgmr_p2p_set_device_name(argv[1]));
}

static int cmd_wifi_p2p_find(int argc, char **argv)
{
    wifi_mgmr_p2p_find_params_t config;

    if (wifi_p2p_parse_find_args(argc, argv, &config)) {
        printf("p2p_find: expected [timeout:0..65535] [freq=<1..65535>] [dev_id=<peer-mac>]\r\n");
        return -1;
    }

    return wifi_p2p_result("find", wifi_mgmr_p2p_find(&config));
}

static int cmd_wifi_p2p_stop_find(int argc, char **argv)
{
    (void)argv;

    if (argc != 1) {
        wifi_p2p_usage();
        return -1;
    }

    return wifi_p2p_result("stop_find", wifi_mgmr_p2p_stop_find());
}

static int cmd_wifi_p2p_listen(int argc, char **argv)
{
    unsigned int seconds = 30;
    unsigned int channel = 0;

    if (argc > 3 ||
        (argc > 1 &&
         wifi_p2p_parse_uint(argv[1], UINT16_MAX, &seconds)) ||
        (argc > 2 &&
         (wifi_p2p_parse_uint(argv[2], UINT8_MAX, &channel) ||
          (channel != 0 && !wifi_p2p_channel_valid((int)channel))))) {
        wifi_p2p_usage();
        return -1;
    }

    return wifi_p2p_result("listen", wifi_mgmr_p2p_listen(
        (uint16_t)seconds, (uint8_t)channel));
}

static int cmd_wifi_p2p_peers(int argc, char **argv)
{
    wifi_mgmr_p2p_peers_t peers;
    char peer_addr[WIFI_MGMR_P2P_ADDR_STR_LEN];
    uint8_t discovered_only = 0;
    uint8_t i;
    int ret;

    if (argc > 2 ||
        (argc == 2 && strcmp(argv[1], "discovered") != 0)) {
        wifi_p2p_usage();
        return -1;
    }
    discovered_only = argc == 2;

    ret = wifi_mgmr_p2p_peers(discovered_only, &peers);
    if (ret)
        return wifi_p2p_result("peers", ret);

    for (i = 0; i < peers.peer_count; i++) {
        wifi_p2p_format_mac(peers.peer_addr[i], peer_addr, sizeof(peer_addr));
        printf("%s\r\n", peer_addr);
    }

    if (peers.peer_count < peers.peer_count_total) {
        printf("p2p: showing first %u of %u peers\r\n",
               peers.peer_count, peers.peer_count_total);
    }

    return 0;
}

static int cmd_wifi_p2p_flush(int argc, char **argv)
{
    (void)argv;

    if (argc != 1) {
        wifi_p2p_usage();
        return -1;
    }

    return wifi_p2p_result("flush", wifi_mgmr_p2p_flush());
}

static void wifi_p2p_print_peer(const wifi_mgmr_p2p_peer_info_t *peer)
{
    char peer_addr[WIFI_MGMR_P2P_ADDR_STR_LEN];

    if (!peer)
        return;

    wifi_p2p_format_mac(peer->peer_addr, peer_addr, sizeof(peer_addr));
    printf("peer_addr=%s\r\n", peer_addr);
    if (peer->pri_dev_type[0])
        printf("pri_dev_type=%s\r\n", peer->pri_dev_type);
    if (peer->device_name[0])
        printf("device_name=%s\r\n", peer->device_name);
    if (peer->manufacturer[0])
        printf("manufacturer=%s\r\n", peer->manufacturer);
    if (peer->model_name[0])
        printf("model_name=%s\r\n", peer->model_name);
    if (peer->model_number[0])
        printf("model_number=%s\r\n", peer->model_number);
    if (peer->serial_number[0])
        printf("serial_number=%s\r\n", peer->serial_number);

    printf("config_methods=0x%04x\r\n", peer->config_methods);
    printf("dev_capab=0x%02x\r\n", peer->dev_capab);
    printf("group_capab=0x%02x\r\n", peer->group_capab);
    printf("level=%d\r\n", peer->level);
}

static int cmd_wifi_p2p_peer(int argc, char **argv)
{
    wifi_mgmr_p2p_peer_info_t peer;
    uint8_t peer_addr[WIFI_MGMR_P2P_ADDR_LEN];
    int ret;

    if (argc != 2) {
        wifi_p2p_usage();
        return -1;
    }

    if (wifi_p2p_parse_mac(argv[1], peer_addr)) {
        printf("p2p: invalid peer MAC address\r\n");
        return -1;
    }

    ret = wifi_mgmr_p2p_peer(peer_addr, &peer);
    if (ret)
        return wifi_p2p_result("peer", ret);

    wifi_p2p_print_peer(&peer);

    return 0;
}

static int cmd_wifi_p2p_peer_raw(int argc, char **argv)
{
    char raw[WIFI_MGMR_P2P_PEER_RAW_LEN];
    uint8_t peer_addr[WIFI_MGMR_P2P_ADDR_LEN];
    uint16_t copied_len;
    uint8_t truncated;
    int ret;

    if (argc != 2) {
        wifi_p2p_usage();
        return -1;
    }

    if (wifi_p2p_parse_mac(argv[1], peer_addr)) {
        printf("p2p: invalid peer MAC address\r\n");
        return -1;
    }

    ret = wifi_mgmr_p2p_peer_raw(peer_addr, raw, sizeof(raw), &copied_len,
                                 &truncated);
    if (ret)
        return wifi_p2p_result("peer_raw", ret);

    if (copied_len > 0)
        printf("%s", raw);
    if (truncated)
        printf("\r\np2p: peer raw info truncated\r\n");

    return 0;
}

static int cmd_wifi_p2p_connect(int argc, char **argv)
{
    wifi_mgmr_p2p_method_t method;
    const char *pin = NULL;
    int join = 0;
    int auto_join = 0;
    int auth = 0;
    int go_intent = -1;
    int freq = 0;
    int freq_forced = 0;
    wifi_mgmr_p2p_connect_params_t config;
    wifi_mgmr_p2p_connect_result_t result;
    int ret;
    int i;

    if (argc < 3) {
        wifi_p2p_usage();
        return -1;
    }

    if (wifi_p2p_parse_connect_method(argc, argv, &method, &pin, &i)) {
        printf("p2p: method must be pbc, pin, or a 4/8 digit PIN\r\n");
        return -1;
    }

    for (; i < argc; i++) {
        if (strcmp(argv[i], "join") == 0) {
            join = 1;
        } else if (strcmp(argv[i], "auto") == 0) {
            auto_join = 1;
        } else if (strcmp(argv[i], "auth") == 0) {
            auth = 1;
        } else if (strncmp(argv[i], "go_intent=", 10) == 0) {
            unsigned int parsed;

            if (wifi_p2p_parse_uint(argv[i] + 10, 15, &parsed)) {
                printf("p2p: go_intent must be 0..15\r\n");
                return -1;
            }
            go_intent = (int)parsed;
        } else if (strncmp(argv[i], "freq=", 5) == 0) {
            unsigned int parsed;

            if (wifi_p2p_parse_uint(argv[i] + 5, UINT16_MAX, &parsed) ||
                parsed == 0) {
                printf("p2p: freq must be a positive MHz value in 1..65535\r\n");
                return -1;
            }
            freq = (int)parsed;
            freq_forced = 1;
        } else if (argv[i][0]) {
            printf("p2p: unknown argument %s\r\n", argv[i]);
            return -1;
        }
    }

    if ((join && auto_join) || (join && auth) || (auto_join && auth)) {
        printf("p2p: join, auto, and auth are mutually exclusive\r\n");
        return -1;
    }
    if (!freq_forced) {
        freq = wifi_p2p_sta_freq();
        if (freq < 0) {
            printf("p2p: active STA channel is not a supported P2P channel\r\n");
            return -1;
        }
        if (freq > 0)
            printf("p2p: using active STA frequency %d MHz for coexistence\r\n",
                   freq);
    }

    memset(&config, 0, sizeof(config));
    if (wifi_p2p_parse_mac(argv[1], config.peer_addr)) {
        printf("p2p: invalid peer MAC address\r\n");
        return -1;
    }
    config.method = method;
    config.pin = pin;
    config.join = join;
    config.auto_join = auto_join;
    config.auth = auth;
    config.go_intent = go_intent;
    config.freq = freq;

    if (method == WIFI_MGMR_P2P_METHOD_PIN_DISPLAY && !pin) {
        ret = wifi_mgmr_p2p_connect(&config, &result);
        if (ret)
            return wifi_p2p_result("connect", ret);
        if (!result.generated_pin_valid) {
            return wifi_p2p_result("connect", WIFI_MGMR_P2P_ERR_SUPPLICANT);
        }
        printf("p2p: generated_pin=%s\r\n", result.generated_pin);
        return 0;
    }

    return wifi_p2p_result("connect", wifi_mgmr_p2p_connect(&config, NULL));
}

static int cmd_wifi_p2p_cancel(int argc, char **argv)
{
    int ret;

    (void)argv;

    if (argc != 1) {
        wifi_p2p_usage();
        return -1;
    }

    ret = wifi_mgmr_p2p_cancel();
    return wifi_p2p_result("cancel", ret);
}

static int cmd_wifi_p2p_group_add(int argc, char **argv)
{
    wifi_mgmr_p2p_group_add_params_t config;
    char passphrase[WIFI_MGMR_P2P_PASSPHRASE_MAX_LEN + 1];
    uint8_t passphrase_set = 0;
    int persistent_mode = 0;
    int ret;
    int i;

    memset(&config, 0, sizeof(config));
    memset(passphrase, 0, sizeof(passphrase));

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "persistent") == 0) {
            if (persistent_mode) {
                printf("p2p: persistent mode may be specified once\r\n");
                goto fail;
            }
            persistent_mode = 1;
            config.persistent = 1;
        } else if (strncmp(argv[i], "persistent=", 11) == 0) {
            const char *id = argv[i] + 11;
            unsigned int parsed;

            if (persistent_mode ||
                wifi_p2p_parse_uint(id, INT_MAX, &parsed)) {
                printf("p2p: persistent id must be a non-negative integer\r\n");
                goto fail;
            }
            persistent_mode = 1;
            config.persistent_id_valid = 1;
            config.persistent_id = (int)parsed;
        } else if (strncmp(argv[i], "freq=", 5) == 0) {
            unsigned int freq;

            if (wifi_p2p_parse_uint(argv[i] + 5, UINT16_MAX, &freq) ||
                freq == 0) {
                printf("p2p: freq must be a positive MHz value in 1..65535\r\n");
                goto fail;
            }
            config.freq = (uint16_t)freq;
        } else if (strncmp(argv[i], "max_oper_chwidth=", 17) == 0) {
            unsigned int chwidth;

            if (wifi_p2p_parse_uint(argv[i] + 17, 80, &chwidth) ||
                (chwidth != 20 && chwidth != 40 && chwidth != 80)) {
                printf("p2p: max_oper_chwidth must be 20, 40, or 80\r\n");
                goto fail;
            }
            config.max_oper_chwidth = (uint16_t)chwidth;
        } else if (strcmp(argv[i], "ht40") == 0) {
            config.ht40 = 1;
        } else if (strcmp(argv[i], "vht") == 0) {
            config.vht = 1;
        } else if (strcmp(argv[i], "he") == 0) {
            config.he = 1;
        } else if (strncmp(argv[i], "passphrase=", 11) == 0) {
            if (passphrase_set ||
                wifi_p2p_parse_passphrase(argv[i] + 11, passphrase)) {
                printf("p2p: passphrase must contain 8..63 printable characters\r\n");
                goto fail;
            }
            passphrase_set = 1;
            config.passphrase = passphrase;
        } else if (strncmp(argv[i], "passphrase_hex=", 15) == 0) {
            if (passphrase_set ||
                wifi_p2p_parse_passphrase_hex(argv[i] + 15, passphrase)) {
                printf("p2p: passphrase_hex must be 16..126 valid hex characters\r\n");
                goto fail;
            }
            passphrase_set = 1;
            config.passphrase = passphrase;
        } else {
            printf("p2p: unknown group_add argument\r\n");
            goto fail;
        }
    }

    if (config.persistent_id_valid && passphrase_set) {
        printf("p2p: persistent=<id> cannot replace its saved passphrase\r\n");
        goto fail;
    }

    ret = wifi_mgmr_p2p_group_add(argc > 1 ? &config : NULL);
    wifi_p2p_clear_secret(passphrase, sizeof(passphrase));
    return wifi_p2p_result("group_add", ret);

fail:
    wifi_p2p_clear_secret(passphrase, sizeof(passphrase));
    return -1;
}

static int cmd_wifi_p2p_group_remove(int argc, char **argv)
{
    (void)argv;

    if (argc != 1) {
        wifi_p2p_usage();
        return -1;
    }

    return wifi_p2p_result("group_remove", wifi_mgmr_p2p_group_remove());
}

static int cmd_wifi_p2p_list_networks(int argc, char **argv)
{
    wifi_mgmr_p2p_persistent_groups_t groups;
    char go_dev_addr[WIFI_MGMR_P2P_ADDR_STR_LEN];
    int ret;
    int i;

    (void)argv;

    if (argc != 1) {
        wifi_p2p_usage();
        return -1;
    }

    ret = wifi_mgmr_p2p_persistent_list(&groups);
    if (ret) {
        return wifi_p2p_result("list_networks", ret);
    }

    printf("network id / ssid / go device address / flags\r\n");
    for (i = 0; i < groups.group_count; i++) {
        wifi_p2p_format_mac(groups.group[i].go_dev_addr, go_dev_addr,
                            sizeof(go_dev_addr));
        printf("%d\t%s\t%s\t[P2P-PERSISTENT]\r\n",
               groups.group[i].id, groups.group[i].ssid, go_dev_addr);
    }
    if (groups.group_count_total > groups.group_count) {
        printf("p2p: showing %u of %u persistent groups\r\n",
               groups.group_count, groups.group_count_total);
    }

    return 0;
}

static int cmd_wifi_p2p_remove_network(int argc, char **argv)
{
    unsigned int id;

    if (argc != 2 || wifi_p2p_parse_uint(argv[1], INT_MAX, &id)) {
        printf("Usage: remove_network <non-negative-id>\r\n");
        return -1;
    }

    return wifi_p2p_result(
        "remove_network", wifi_mgmr_p2p_persistent_remove((int)id));
}

static int cmd_wifi_p2p_wps_pbc(int argc, char **argv)
{
    uint8_t peer_addr[WIFI_MGMR_P2P_ADDR_LEN];
    const uint8_t *peer = NULL;
    int ret;

    if (argc > 2) {
        wifi_p2p_usage();
        return -1;
    }

    if (argc == 2) {
        if (wifi_p2p_parse_mac(argv[1], peer_addr)) {
            printf("p2p: invalid peer MAC address\r\n");
            return -1;
        }
        peer = peer_addr;
    }

    ret = wifi_mgmr_p2p_wps_pbc(peer);
    printf("p2p: WPS_PBC vif=%d ret=%d\r\n",
           WIFI_MGMR_P2P_FHOST_VIF_IDX, ret);
    return ret;
}

static int cmd_wifi_p2p_prov_disc(int argc, char **argv)
{
    wifi_mgmr_p2p_prov_disc_params_t config;
    int i;

    if (argc < 2) {
        wifi_p2p_usage();
        return -1;
    }

    memset(&config, 0, sizeof(config));
    if (wifi_p2p_parse_mac(argv[1], config.peer_addr)) {
        printf("p2p: invalid peer MAC address\r\n");
        return -1;
    }
    config.method = WIFI_MGMR_P2P_METHOD_PBC;

    for (i = 2; i < argc; i++) {
        if (strcmp(argv[i], "pbc") == 0) {
            config.method = WIFI_MGMR_P2P_METHOD_PBC;
        } else if (strcmp(argv[i], "pin") == 0 ||
                   strcmp(argv[i], "display") == 0) {
            config.method = WIFI_MGMR_P2P_METHOD_PIN_DISPLAY;
        } else if (strcmp(argv[i], "keypad") == 0) {
            config.method = WIFI_MGMR_P2P_METHOD_PIN_KEYPAD;
        } else if (strcmp(argv[i], "join") == 0) {
            config.join = 1;
        } else if (strcmp(argv[i], "auto") == 0) {
            config.auto_join = 1;
        } else {
            printf("p2p: unknown argument %s\r\n", argv[i]);
            return -1;
        }
    }

    if (config.join && config.auto_join) {
        printf("p2p: join and auto are mutually exclusive\r\n");
        return -1;
    }

    return wifi_p2p_result("prov_disc", wifi_mgmr_p2p_prov_disc(&config));
}

static int cmd_wifi_p2p_invite(int argc, char **argv)
{
    wifi_mgmr_p2p_invite_params_t config;
    int invite_mode = -1;
    int i;

    if (argc < 2) {
        wifi_p2p_usage();
        return -1;
    }

    memset(&config, 0, sizeof(config));

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "group=", 6) == 0) {
            if (invite_mode != -1 ||
                strcmp(argv[i] + 6, WIFI_MGMR_P2P_DEFAULT_IFACE) != 0) {
                printf("p2p: group must be specified once as group=%s\r\n",
                       WIFI_MGMR_P2P_DEFAULT_IFACE);
                return -1;
            }
            invite_mode = 0;
            config.persistent = 0;
        } else if (strncmp(argv[i], "persistent=", 11) == 0) {
            const char *id = argv[i] + 11;
            unsigned int parsed;

            if (invite_mode != -1) {
                printf("p2p: invite mode may be specified once\r\n");
                return -1;
            }
            if (wifi_p2p_parse_uint(id, INT_MAX, &parsed)) {
                printf("p2p: persistent id must be a non-negative integer\r\n");
                return -1;
            }
            invite_mode = 1;
            config.persistent = 1;
            config.persistent_id = (int)parsed;
        } else if (strncmp(argv[i], "peer=", 5) == 0) {
            if (config.peer_addr_valid ||
                wifi_p2p_parse_mac(argv[i] + 5, config.peer_addr)) {
                printf("p2p: invalid or duplicate peer MAC address\r\n");
                return -1;
            }
            config.peer_addr_valid = 1;
        } else if (strncmp(argv[i], "go_dev_addr=", 12) == 0) {
            if (config.go_dev_addr_valid ||
                wifi_p2p_parse_mac(argv[i] + 12, config.go_dev_addr)) {
                printf("p2p: invalid or duplicate GO device MAC address\r\n");
                return -1;
            }
            config.go_dev_addr_valid = 1;
        } else if (strncmp(argv[i], "freq=", 5) == 0) {
            unsigned int freq;

            if (wifi_p2p_parse_uint(argv[i] + 5, UINT16_MAX, &freq) ||
                freq == 0) {
                printf("p2p: freq must be a positive MHz value in 1..65535\r\n");
                return -1;
            }
            config.freq = (uint16_t)freq;
        } else if (strncmp(argv[i], "pref=", 5) == 0) {
            unsigned int pref;

            if (wifi_p2p_parse_uint(argv[i] + 5, UINT16_MAX, &pref) ||
                pref == 0) {
                printf("p2p: pref must be a positive MHz value in 1..65535\r\n");
                return -1;
            }
            config.pref_freq = (uint16_t)pref;
        } else if (strncmp(argv[i], "max_oper_chwidth=", 17) == 0) {
            unsigned int chwidth;

            if (wifi_p2p_parse_uint(argv[i] + 17, 80, &chwidth) ||
                (chwidth != 20 && chwidth != 40 && chwidth != 80)) {
                printf("p2p: max_oper_chwidth must be 20, 40, or 80\r\n");
                return -1;
            }
            config.max_oper_chwidth = (uint16_t)chwidth;
        } else if (strcmp(argv[i], "ht40") == 0) {
            config.ht40 = 1;
        } else if (strcmp(argv[i], "vht") == 0) {
            config.vht = 1;
        } else if (strcmp(argv[i], "he") == 0) {
            config.he = 1;
        } else {
            printf("p2p: unknown argument %s\r\n", argv[i]);
            return -1;
        }
    }

    if (invite_mode == -1) {
        printf("p2p: group=%s or persistent=<id> is required\r\n",
               WIFI_MGMR_P2P_DEFAULT_IFACE);
        return -1;
    }
    if (!config.persistent && !config.peer_addr_valid) {
        printf("p2p: peer is required for active group invite\r\n");
        return -1;
    }
    if (!config.persistent &&
        (config.freq || config.pref_freq ||
         config.max_oper_chwidth || config.ht40 || config.vht || config.he)) {
        printf("p2p: frequency and PHY options require persistent invite mode\r\n");
        return -1;
    }
    if (config.persistent && config.go_dev_addr_valid) {
        printf("p2p: go_dev_addr is only valid for active group invite\r\n");
        return -1;
    }

    return wifi_p2p_result("invite", wifi_mgmr_p2p_invite(&config));
}

/* A discovered GO already has a role: join it without GO Negotiation
 * fallback. After the GO is known, park listen on its operating channel so
 * a single-radio chip does not hop back to 1/6/11 before PD/WPS. */
static void wifi_p2p_join_select_mode(wifi_mgmr_p2p_connect_params_t *config)
{
    wifi_mgmr_p2p_peer_info_t peer;
    int ret;
    int go_channel;

    ret = wifi_mgmr_p2p_peer(config->peer_addr, &peer);
    if (!ret && (peer.group_capab & 0x01)) {
        config->join = 1;
        config->auto_join = 0;
        if (peer.oper_freq > 0)
            config->freq = (uint16_t)peer.oper_freq;
        go_channel = wifi_p2p_channel_from_freq(config->freq);
        if (go_channel > 0) {
            ret = wifi_mgmr_p2p_set_channel_for_join((uint8_t)go_channel);
            printf("p2p_join: peer is GO freq=%u; listen follows channel %d ret=%d\r\n",
                   (unsigned int)config->freq, go_channel, ret);
        }
        printf("p2p_join: discovered GO; using explicit join freq=%u\r\n",
               (unsigned int)config->freq);
    } else {
        printf("p2p_join: GO not confirmed (peer ret=%d); using auto\r\n", ret);
    }
}

static int wifi_p2p_join_select_peer(const char *name,
                                     uint8_t peer_addr[WIFI_MGMR_P2P_ADDR_LEN])
{
    wifi_mgmr_p2p_peers_t peers;
    wifi_mgmr_p2p_peer_info_t peer;
    uint8_t i;
    int ret;

    if (!peer_addr)
        return -1;

    ret = wifi_mgmr_p2p_peers(1, &peers);
    if (ret)
        return ret;

    if (peers.peer_count == 0) {
        printf("p2p_join: no peer found\r\n");
        return -1;
    }

    for (i = 0; i < peers.peer_count; i++) {
        char peer_addr_str[WIFI_MGMR_P2P_ADDR_STR_LEN];

        wifi_p2p_format_mac(peers.peer_addr[i], peer_addr_str,
                            sizeof(peer_addr_str));
        memset(&peer, 0, sizeof(peer));
        ret = wifi_mgmr_p2p_peer(peers.peer_addr[i], &peer);
        if (ret == 0) {
            printf("p2p_join: candidate peer=%s name=%s\r\n",
                   peer_addr_str,
                   peer.device_name[0] ? peer.device_name : "-");
            if (name && name[0] && peer.device_name[0] &&
                strstr(peer.device_name, name)) {
                memcpy(peer_addr, peers.peer_addr[i],
                       WIFI_MGMR_P2P_ADDR_LEN);
                return 0;
            }
        } else {
            printf("p2p_join: candidate peer=%s\r\n",
                   peer_addr_str);
        }
    }

    if (!name || !name[0]) {
        char peer_addr_str[WIFI_MGMR_P2P_ADDR_STR_LEN];

        wifi_p2p_format_mac(peers.peer_addr[0], peer_addr_str,
                            sizeof(peer_addr_str));
        memcpy(peer_addr, peers.peer_addr[0], WIFI_MGMR_P2P_ADDR_LEN);
        return 0;
    }

    printf("p2p_join: no peer matched name=%s\r\n", name);
    return -1;
}

static int wifi_p2p_join_match_target(
    const uint8_t target_addr[WIFI_MGMR_P2P_ADDR_LEN], const char *name,
    uint8_t peer_addr[WIFI_MGMR_P2P_ADDR_LEN])
{
    wifi_mgmr_p2p_peers_t peers;
    wifi_mgmr_p2p_peer_info_t peer;
    uint8_t i;
    int ret;

    if ((!target_addr && (!name || !name[0])) || !peer_addr)
        return 0;

    ret = wifi_mgmr_p2p_peers(1, &peers);
    if (ret)
        return ret;

    for (i = 0; i < peers.peer_count; i++) {
        if (target_addr) {
            if (memcmp(peers.peer_addr[i], target_addr,
                       WIFI_MGMR_P2P_ADDR_LEN) == 0) {
                memcpy(peer_addr, peers.peer_addr[i],
                       WIFI_MGMR_P2P_ADDR_LEN);
                return 1;
            }
            continue;
        }

        memset(&peer, 0, sizeof(peer));
        ret = wifi_mgmr_p2p_peer(peers.peer_addr[i], &peer);
        if (ret == 0 && peer.device_name[0] &&
            strstr(peer.device_name, name)) {
            memcpy(peer_addr, peers.peer_addr[i],
                   WIFI_MGMR_P2P_ADDR_LEN);
            return 1;
        }
    }

    return 0;
}

static int wifi_p2p_join_wait_target(
    const uint8_t target_addr[WIFI_MGMR_P2P_ADDR_LEN], const char *name,
    int find_seconds, uint8_t peer_addr[WIFI_MGMR_P2P_ADDR_LEN],
    uint32_t *elapsed_ms)
{
    uint32_t timeout_ms = (uint32_t)find_seconds * 1000U;
    uint32_t waited_ms = 0;
    uint32_t delay_ms;
    int ret;

    while (1) {
        ret = wifi_p2p_join_match_target(target_addr, name, peer_addr);
        if (ret != 0 || waited_ms >= timeout_ms ||
            wifi_mgmr_p2p_state_get() != WIFI_MGMR_P2P_STATE_FINDING)
            break;

        delay_ms = timeout_ms - waited_ms;
        if (delay_ms > WIFI_P2P_JOIN_FIND_POLL_MS)
            delay_ms = WIFI_P2P_JOIN_FIND_POLL_MS;
        rtos_task_suspend(delay_ms);
        waited_ms += delay_ms;
    }

    if (elapsed_ms)
        *elapsed_ms = waited_ms;
    return ret;
}

static void wifi_p2p_join_print_status(const wifi_mgmr_p2p_status_t *status)
{
    if (!status)
        return;

    if (status->bssid[0])
        printf("bssid=%s\r\n", status->bssid);
    if (status->freq)
        printf("freq=%u\r\n", status->freq);
    if (status->ssid[0])
        printf("ssid=%s\r\n", status->ssid);
    if (status->mode[0])
        printf("mode=%s\r\n", status->mode);
    if (status->wpa_state[0])
        printf("wpa_state=%s\r\n", status->wpa_state);
    if (status->p2p_device_addr[0])
        printf("p2p_device_address=%s\r\n", status->p2p_device_addr);
    if (status->address[0])
        printf("address=%s\r\n", status->address);
}

static int wifi_p2p_join_wait_completed(int wait_seconds)
{
    wifi_mgmr_p2p_status_t status;
    int elapsed;
    int ret;

    if (wait_seconds <= 0)
        return 0;

    for (elapsed = 0; elapsed < wait_seconds; elapsed++) {
        memset(&status, 0, sizeof(status));
        ret = wifi_mgmr_p2p_status(&status);
        if (ret == 0 && status.completed) {
            printf("p2p_join: connected after %d seconds\r\n", elapsed);
            wifi_p2p_join_print_status(&status);
            return 0;
        }
        rtos_task_suspend(1000);
    }

    memset(&status, 0, sizeof(status));
    if (wifi_mgmr_p2p_status(&status) == 0) {
        if (status.completed) {
            printf("p2p_join: connected after %d seconds\r\n",
                   wait_seconds);
            wifi_p2p_join_print_status(&status);
            return 0;
        }
        wifi_p2p_join_print_status(&status);
    }

    printf("p2p_join: timeout waiting for COMPLETED\r\n");
    return -1;
}

static int wifi_p2p_join_find_5g(
    int reported_freq, const uint8_t *target_addr, const char *name,
    int find_seconds, uint8_t peer_addr[WIFI_MGMR_P2P_ADDR_LEN],
    uint32_t *elapsed_ms, int *target_found)
{
    wifi_mgmr_p2p_find_params_t find_config;
    int ch;
    int ret;
    uint32_t waited_ms = 0;

    ch = wifi_p2p_channel_from_freq(reported_freq);
    if (ch <= 0)
        return wifi_p2p_result("join_find_channel", -1);

    ret = wifi_mgmr_p2p_set_channel_for_join((uint8_t)ch);
    printf("p2p_join: 5G find freq=%d; listen follows channel %d ret=%d\r\n",
           reported_freq, ch, ret);
    /* Let SET listen/oper take effect before P2P_FIND. */
    rtos_task_suspend(300);

    memset(&find_config, 0, sizeof(find_config));
    find_config.timeout = (uint16_t)find_seconds;
    find_config.freq = (uint16_t)reported_freq;
    ret = wifi_mgmr_p2p_find(&find_config);
    if (ret)
        return wifi_p2p_result("join_find", ret);

    ret = wifi_p2p_join_wait_target(target_addr, name, find_seconds,
                                    peer_addr, &waited_ms);
    if (ret < 0) {
        if (wifi_mgmr_p2p_state_get() == WIFI_MGMR_P2P_STATE_FINDING)
            (void)wifi_mgmr_p2p_stop_find();
        return wifi_p2p_result("join_find_target", ret);
    }
    if (elapsed_ms)
        *elapsed_ms = waited_ms;
    *target_found = ret > 0;
    if (*target_found) {
        printf("p2p_join: target discovered after %lu ms on freq=%d\r\n",
               (unsigned long)waited_ms, reported_freq);
    }
    if (wifi_mgmr_p2p_state_get() == WIFI_MGMR_P2P_STATE_FINDING) {
        ret = wifi_mgmr_p2p_stop_find();
        if (ret)
            return wifi_p2p_result("join_stop_find", ret);
    }
    return 0;
}

static int cmd_wifi_p2p_join(int argc, char **argv)
{
    const char *peer_arg = NULL;
    const char *name = NULL;
    int freq = 0;
    int channel = 0;
    int go_intent = -1;
    int find_seconds = 60;
    int wait_seconds = 35;
    int do_find = 1;
    int target_found = 0;
    int i;
    int ret;
    uint32_t find_elapsed_ms = 0;
    uint8_t peer_addr[WIFI_MGMR_P2P_ADDR_LEN];
    char peer_addr_str[WIFI_MGMR_P2P_ADDR_STR_LEN];
    char go_intent_str[8] = "default";
    wifi_mgmr_p2p_find_params_t find_config;
    wifi_mgmr_p2p_connect_params_t config;

    memset(peer_addr, 0, sizeof(peer_addr));

    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "peer=", 5) == 0) {
            peer_arg = argv[i] + 5;
        } else if (strncmp(argv[i], "name=", 5) == 0) {
            name = argv[i] + 5;
        } else if (strncmp(argv[i], "freq=", 5) == 0) {
            unsigned int parsed;

            if (wifi_p2p_parse_uint(argv[i] + 5, UINT16_MAX, &parsed) ||
                parsed == 0) {
                printf("p2p_join: freq must be 1..65535 MHz\r\n");
                return -1;
            }
            freq = (int)parsed;
        } else if (strncmp(argv[i], "channel=", 8) == 0) {
            unsigned int parsed;

            if (wifi_p2p_parse_uint(argv[i] + 8, UINT8_MAX, &parsed) ||
                !wifi_p2p_channel_valid((int)parsed)) {
                printf("p2p_join: channel must be a valid 2.4/5 GHz channel\r\n");
                return -1;
            }
            channel = (int)parsed;
        } else if (strncmp(argv[i], "go_intent=", 10) == 0) {
            unsigned int parsed;

            if (wifi_p2p_parse_uint(argv[i] + 10, 15, &parsed)) {
                printf("p2p_join: go_intent must be 0..15\r\n");
                return -1;
            }
            go_intent = (int)parsed;
        } else if (strncmp(argv[i], "find=", 5) == 0) {
            unsigned int parsed;

            if (wifi_p2p_parse_uint(argv[i] + 5, UINT16_MAX, &parsed)) {
                printf("p2p_join: find must be 0..65535\r\n");
                return -1;
            }
            find_seconds = (int)parsed;
        } else if (strncmp(argv[i], "wait=", 5) == 0) {
            unsigned int parsed;

            if (wifi_p2p_parse_uint(argv[i] + 5, UINT16_MAX, &parsed)) {
                printf("p2p_join: wait must be 0..65535\r\n");
                return -1;
            }
            wait_seconds = (int)parsed;
        } else if (strcmp(argv[i], "no_find") == 0) {
            do_find = 0;
        } else if (!peer_arg) {
            peer_arg = argv[i];
        } else {
            printf("p2p_join: unknown argument %s\r\n", argv[i]);
            return -1;
        }
    }

    if (freq && !channel) {
        channel = wifi_p2p_channel_from_freq(freq);
        if (!wifi_p2p_channel_valid(channel)) {
            printf("p2p_join: unsupported P2P freq=%d\r\n", freq);
            return -1;
        }
    }

    if (freq && channel && wifi_p2p_freq_from_channel(channel) != freq) {
        printf("p2p_join: freq and channel do not match\r\n");
        return -1;
    }

    if (!freq && channel)
        freq = wifi_p2p_freq_from_channel(channel);

    if (peer_arg && wifi_p2p_parse_mac(peer_arg, peer_addr)) {
        printf("p2p_join: invalid peer MAC address\r\n");
        return -1;
    }

    if (go_intent >= 0)
        snprintf(go_intent_str, sizeof(go_intent_str), "%d", go_intent);

    printf("p2p_join: init channel=%d freq=%d peer=%s name=%s "
           "go_intent=%s find=%d wait=%d\r\n",
           channel, freq, peer_arg ? peer_arg : "-", name ? name : "-",
           go_intent_str, find_seconds, wait_seconds);

    /* Discovery uses social listen unless the GO frequency is already known.
     * Skip a second init: it flushes peer/BSS state and bounces 5 GHz. */
    if (wifi_mgmr_p2p_state_get() == WIFI_MGMR_P2P_STATE_STOPPED) {
        ret = wifi_p2p_init_channel(channel, channel != 0);
        if (ret)
            return ret;
    }
    if (do_find && find_seconds > 0 && freq >= 5000) {
        ret = wifi_p2p_join_find_5g(freq, peer_arg ? peer_addr : NULL, name,
                                    find_seconds, peer_addr, &find_elapsed_ms,
                                    &target_found);
        if (ret)
            return ret;
    } else if (channel > 14) {
        ret = wifi_mgmr_p2p_set_channel_for_join((uint8_t)channel);
        printf("p2p_join: 5G find freq=%d; listen follows channel %d ret=%d\r\n",
               freq, channel, ret);
    }

    if (do_find && find_seconds > 0 && freq < 5000) {
        memset(&find_config, 0, sizeof(find_config));
        find_config.timeout = (uint16_t)find_seconds;
        find_config.freq = (uint16_t)freq;
        if (peer_arg) {
            memcpy(find_config.dev_id, peer_addr, sizeof(find_config.dev_id));
            find_config.dev_id_valid = 1;
        }
        ret = wifi_mgmr_p2p_find(&find_config);
        if (ret)
            return wifi_p2p_result("join_find", ret);

        if (peer_arg || (name && name[0])) {
            ret = wifi_p2p_join_wait_target(peer_arg ? peer_addr : NULL,
                                            name, find_seconds, peer_addr,
                                            &find_elapsed_ms);
            if (ret < 0) {
                if (wifi_mgmr_p2p_state_get() ==
                    WIFI_MGMR_P2P_STATE_FINDING)
                    (void)wifi_mgmr_p2p_stop_find();
                return wifi_p2p_result("join_find_target", ret);
            }
            target_found = ret > 0;
            if (target_found) {
                printf("p2p_join: target discovered after %lu ms; "
                       "stopping discovery\r\n",
                       (unsigned long)find_elapsed_ms);
            }
            if (wifi_mgmr_p2p_state_get() ==
                WIFI_MGMR_P2P_STATE_FINDING) {
                ret = wifi_mgmr_p2p_stop_find();
                if (ret)
                    return wifi_p2p_result("join_stop_find", ret);
            }
        } else {
            rtos_task_suspend((find_seconds + 1) * 1000);
        }
    }

    if (!peer_arg && !target_found) {
        ret = wifi_p2p_join_select_peer(name, peer_addr);
        if (ret)
            return ret;
    }

    wifi_p2p_format_mac(peer_addr, peer_addr_str, sizeof(peer_addr_str));
    printf("p2p_join: selected peer=%s\r\n", peer_addr_str);
    printf("p2p_join: peer must keep WPS PBC active while connecting\r\n");

    memset(&config, 0, sizeof(config));
    memcpy(config.peer_addr, peer_addr, sizeof(config.peer_addr));
    config.method = WIFI_MGMR_P2P_METHOD_PBC;
    config.auto_join = 1;
    config.go_intent = (int8_t)go_intent;
    config.freq = (uint16_t)freq;
    wifi_p2p_join_select_mode(&config);

    ret = wifi_mgmr_p2p_connect(&config, NULL);
    if (ret)
        return wifi_p2p_result("join_connect", ret);

    return wifi_p2p_join_wait_completed(wait_seconds);
}

static int cmd_wifi_p2p_stop(int argc, char **argv)
{
    int ret;

    if (argc > 1) {
        wifi_p2p_usage();
        return -1;
    }

    ret = wifi_mgmr_p2p_stop();
    return wifi_p2p_result("stop", ret);
}

static int cmd_wifi_p2p_status(int argc, char **argv)
{
    wifi_mgmr_p2p_status_t status;
    int ret;

    if (argc > 1) {
        wifi_p2p_usage();
        return -1;
    }

    ret = wifi_mgmr_p2p_status(&status);
    if (ret)
        return wifi_p2p_result("status", ret);

    printf("p2p_state=%s\r\n",
           wifi_p2p_state_string(wifi_mgmr_p2p_state_get()));

    if (status.bssid[0])
        printf("bssid=%s\r\n", status.bssid);
    if (status.freq)
        printf("freq=%u\r\n", status.freq);
    if (status.ssid[0])
        printf("ssid=%s\r\n", status.ssid);
    if (status.mode[0])
        printf("mode=%s\r\n", status.mode);
    if (status.wpa_state[0])
        printf("wpa_state=%s\r\n", status.wpa_state);
    if (status.p2p_device_addr[0])
        printf("p2p_device_address=%s\r\n", status.p2p_device_addr);
    if (status.address[0])
        printf("address=%s\r\n", status.address);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_help, p2p_help, show detailed P2P help for all commands or one command);
// SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_iface_init, p2p_iface_init, init fixed p2p interface only);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_init, p2p_init, initialize fixed wl2 P2P VIF with optional 2.4/5 GHz channel);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_channel, p2p_channel, set P2P operating/listen/preferred channel);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_config, p2p_config, reconfigure initialized P2P channel and fixed-interface mode);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_name, p2p_name, set 1..32-byte advertised P2P device name);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_find, p2p_find, discover peers with optional timeout/freq/dev_id filters);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_stop_find, p2p_stop_find, stop find/listen while preserving VIF and peers);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_listen, p2p_listen, listen for optional seconds on optional channel);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_peers, p2p_peers, list cached peers with optional discovered-only filter);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_flush, p2p_flush, clear discovery/provisioning cache but preserve VIF/group);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_peer, p2p_peer, show parsed properties for one cached peer);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_peer_raw, p2p_peer_raw, show raw P2P_PEER response and truncation status);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_connect, p2p_connect, start PBC/PIN negotiation join auto or authorization);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_cancel, p2p_cancel, issue standard P2P_CANCEL for pending P2P work);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_group_add, p2p_group_add, create autonomous/persistent GO with frequency/capability/passphrase options);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_list_networks, list_networks, list saved P2P persistent groups and their network IDs);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_remove_network, remove_network, delete one saved P2P persistent group by network ID);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_group_remove, p2p_group_remove, remove active wl2 group while preserving P2P VIF);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_wps_pbc, p2p_wps_pbc, start GO WPS PBC optionally restricted to one peer);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_prov_disc, p2p_prov_disc, start PBC/display/keypad Provision Discovery);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_invite, p2p_invite, invite to active group or reinvoke persistent group);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_join, p2p_join, automatically join or negotiate a PBC peer using an init/find/connect/wait workflow);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_stop, p2p_stop, fully stop P2P then flush state and remove wl2 VIF);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_p2p_status, p2p_status, show lifecycle and synchronous supplicant status);
