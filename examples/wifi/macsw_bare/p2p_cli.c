#if defined(CONFIG_BL_SUPPLICANT_P2P)

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "bl_supplicant/bl_p2p.h"
#include "shell.h"
#include "wifi_mgmr.h"

#define WL_P2P_DEFAULT_LISTEN_WPS_DURATION_SEC 20
#define WL_P2P_MAX_2G_CHANNEL 13
#define WL_P2P_MAX_GO_INTENT 15

static int wl_p2p_is_2g_channel(unsigned int channel)
{
    return channel >= 1 && channel <= WL_P2P_MAX_2G_CHANNEL;
}

static int wl_p2p_parse_peer(const char *arg, uint8_t peer[6])
{
    if (arg == NULL || wifi_mgmr_mac_str_to_addr(arg, peer) != 0) {
        printf("invalid peer mac: %s\n", arg ? arg : "(null)");
        return -1;
    }

    return 0;
}

static enum bl_p2p_wps_mode wl_p2p_parse_mode(const char *arg)
{
    if (arg != NULL) {
        if (strcmp(arg, "display") == 0) {
            return BL_P2P_WPS_PIN_DISPLAY;
        }
        if (strcmp(arg, "keypad") == 0) {
            return BL_P2P_WPS_PIN_KEYPAD;
        }
    }

    return BL_P2P_WPS_PBC;
}

static int wl_p2p_apply_device_name_arg(int argc, char **argv)
{
    if (argc >= 2 && argv[1] != NULL && argv[1][0] != '\0') {
        return bl_p2p_set_device_name(argv[1]);
    }

    return 0;
}

static int wl_p2p_init_cmd(int argc, char **argv)
{
    int ret;

    if (wl_p2p_apply_device_name_arg(argc, argv) < 0) {
        printf("wl_p2p_init failed to set device name\n");
        return -1;
    }

    ret = bl_p2p_init();
    if (ret < 0) {
        printf("wl_p2p_init failed (%d)\n", ret);
        return -1;
    }

    printf("wl_p2p_init ok\n");
    return 0;
}

static int wl_p2p_find_cmd(int argc, char **argv)
{
    unsigned int timeout = 30;

    if (argc >= 2) {
        timeout = (unsigned int) atoi(argv[1]);
    }

    if (bl_p2p_find(timeout) < 0) {
        printf("wl_p2p_find failed\n");
        return -1;
    }

    printf("wl_p2p_find started timeout=%u sec\n", timeout);
    return 0;
}

static int wl_p2p_listen_cmd(int argc, char **argv)
{
    unsigned int duration = 5000;
    unsigned int channel = 0;
    int ret;

    if (argc >= 2) {
        duration = (unsigned int) atoi(argv[1]);
    }
    if (argc >= 3) {
        channel = (unsigned int) atoi(argv[2]);
    }
    if (channel != 0 && !wl_p2p_is_2g_channel(channel)) {
        printf("invalid channel=%u\n", channel);
        return -1;
    }

    ret = channel != 0 ? bl_p2p_listen_on_channel(duration, (uint8_t) channel) :
                         bl_p2p_listen(duration);
    if (ret < 0) {
        printf("usage: wl_p2p_listen [duration_ms] [channel]\n");
        printf("wl_p2p_listen failed\n");
        return -1;
    }

    if (channel != 0) {
        printf("wl_p2p_listen started channel=%u duration=%u ms\n",
               channel, duration);
    } else {
        printf("wl_p2p_listen started duration=%u ms\n", duration);
    }
    return 0;
}

static int wl_p2p_listen_ch_cmd(int argc, char **argv)
{
    unsigned int channel;
    unsigned int duration = 5000;

    if (argc < 2) {
        printf("usage: wl_p2p_listen_ch <channel> [duration_ms]\n");
        return -1;
    }

    channel = (unsigned int) atoi(argv[1]);
    if (argc >= 3) {
        duration = (unsigned int) atoi(argv[2]);
    }
    if (!wl_p2p_is_2g_channel(channel)) {
        printf("invalid channel=%u\n", channel);
        return -1;
    }

    if (bl_p2p_listen_on_channel(duration, (uint8_t) channel) < 0) {
        printf("wl_p2p_listen_ch failed channel=%u duration=%u ms\n",
               channel, duration);
        return -1;
    }

    printf("wl_p2p_listen_ch started channel=%u duration=%u ms\n",
           channel, duration);
    return 0;
}

static int wl_p2p_group_add_pbc_cmd(int argc, char **argv)
{
    unsigned int freq = 0;

    if (argc >= 2) {
        freq = (unsigned int) atoi(argv[1]);
    }

    if (bl_p2p_group_add(freq, BL_P2P_WPS_PBC, NULL) < 0) {
        printf("wl_p2p_group_add_pbc failed\n");
        return -1;
    }

    printf("wl_p2p_group_add_pbc requested freq=%u\n", freq);
    return 0;
}

static int wl_p2p_group_add_pin_cmd(int argc, char **argv)
{
    unsigned int freq = 0;
    const char *pin = NULL;

    if (argc >= 2) {
        freq = (unsigned int) atoi(argv[1]);
    }
    if (argc >= 3 && strcmp(argv[2], "auto") != 0) {
        pin = argv[2];
    }

    if (bl_p2p_group_add(freq, BL_P2P_WPS_PIN_DISPLAY, pin) < 0) {
        printf("wl_p2p_group_add_pin failed\n");
        return -1;
    }

    return 0;
}

static int wl_p2p_connect_pbc_cmd(int argc, char **argv)
{
    uint8_t peer[6];

    if (argc < 2) {
        printf("usage: wl_p2p_connect_pbc <go_dev_addr>\n");
        return -1;
    }
    if (wl_p2p_parse_peer(argv[1], peer) < 0) {
        return -1;
    }

    if (bl_p2p_join(peer, BL_P2P_WPS_PBC, NULL) < 0) {
        printf("wl_p2p_connect_pbc failed\n");
        return -1;
    }

    printf("wl_p2p_connect_pbc requested join\n");
    return 0;
}

static int wl_p2p_connect_pin_cmd(int argc, char **argv)
{
    uint8_t peer[6];
    const char *pin = NULL;

    if (argc < 2) {
        printf("usage: wl_p2p_connect_pin <go_dev_addr> [pin]\n");
        return -1;
    }
    if (wl_p2p_parse_peer(argv[1], peer) < 0) {
        return -1;
    }
    if (argc >= 3 && strcmp(argv[2], "auto") != 0) {
        pin = argv[2];
    }

    if (bl_p2p_join(peer, BL_P2P_WPS_PIN_KEYPAD, pin) < 0) {
        printf("wl_p2p_connect_pin failed\n");
        return -1;
    }

    return 0;
}

static int wl_p2p_prov_disc_cmd(int argc, char **argv)
{
    uint8_t peer[6];
    enum bl_p2p_wps_mode mode;

    if (argc < 3) {
        printf("usage: wl_p2p_prov_disc <peer_mac> <display|keypad|pbc> [join]\n");
        return -1;
    }
    if (wl_p2p_parse_peer(argv[1], peer) < 0) {
        return -1;
    }

    mode = wl_p2p_parse_mode(argv[2]);
    if (bl_p2p_prov_disc(peer, mode, argc >= 4 && strcmp(argv[3], "join") == 0) <
        0) {
        printf("wl_p2p_prov_disc failed\n");
        return -1;
    }

    printf("wl_p2p_prov_disc requested mode=%s\n", argv[2]);
    return 0;
}

static int wl_p2p_stop_cmd(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    if (bl_p2p_stop() < 0) {
        printf("wl_p2p_stop failed\n");
        return -1;
    }

    printf("wl_p2p_stop ok\n");
    return 0;
}

static int wl_p2p_deinit_cmd(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    bl_p2p_deinit();
    printf("wl_p2p_deinit ok\n");
    return 0;
}

static int p2p_init_cmd(int argc, char **argv)
{
    int ret;

    if (wl_p2p_apply_device_name_arg(argc, argv) < 0) {
        printf("p2p_init failed to set device name\n");
        return -1;
    }

    ret = bl_p2p_init();
    if (ret < 0) {
        printf("p2p_init failed (%d)\n", ret);
        return -1;
    }

    printf("p2p_init ok\n");
    return 0;
}

static int p2p_find_cmd(int argc, char **argv)
{
    return wl_p2p_find_cmd(argc, argv);
}

static int p2p_listen_cmd(int argc, char **argv)
{
    return wl_p2p_listen_cmd(argc, argv);
}

static int p2p_listen_ch_cmd(int argc, char **argv)
{
    return wl_p2p_listen_ch_cmd(argc, argv);
}

static int p2p_group_add_cmd(int argc, char **argv)
{
    return wl_p2p_group_add_pbc_cmd(argc, argv);
}

static int p2p_wps_pbc_cmd(int argc, char **argv)
{
    return wl_p2p_group_add_pbc_cmd(argc, argv);
}

static int p2p_wps_pin_cmd(int argc, char **argv)
{
    return wl_p2p_group_add_pin_cmd(argc, argv);
}

static int wl_p2p_listen_wps_cmd(int argc, char **argv,
                                 enum bl_p2p_wps_mode mode,
                                 const char *cmd)
{
    const char *pin = NULL;
    unsigned int duration_sec = WL_P2P_DEFAULT_LISTEN_WPS_DURATION_SEC;
    unsigned int channel;

    if (argc < 2) {
        printf("usage: %s <channel> [duration_sec] [pin]\n", cmd);
        return -1;
    }

    channel = (unsigned int) atoi(argv[1]);
    if (!wl_p2p_is_2g_channel(channel)) {
        printf("invalid channel=%u\n", channel);
        return -1;
    }

    if (argc >= 3) {
        duration_sec = (unsigned int) atoi(argv[2]);
        if (duration_sec == 0) {
            duration_sec = WL_P2P_DEFAULT_LISTEN_WPS_DURATION_SEC;
        }
    }

    if (mode == BL_P2P_WPS_PIN_DISPLAY && argc >= 4 &&
        argv[3] != NULL && argv[3][0] != '\0' &&
        strcmp(argv[3], "auto") != 0) {
        pin = argv[3];
    }

    if (bl_p2p_listen_wps_start(mode, (uint8_t) channel,
                                duration_sec * 1000U, pin) < 0) {
        printf("%s failed (call p2p_init first)\n", cmd);
        return -1;
    }

    if (mode == BL_P2P_WPS_PBC) {
        printf("%s armed channel=%u duration=%u sec method=pbc\n",
               cmd, channel, duration_sec);
        printf("Start PBC on the peer while this device is listening.\n");
    } else if (pin != NULL) {
        printf("%s armed channel=%u duration=%u sec method=pin pin=%s\n",
               cmd, channel, duration_sec, pin);
        printf("When the peer prompts for a PIN, enter %s.\n", pin);
    } else {
        printf("%s armed channel=%u duration=%u sec method=pin\n",
               cmd, channel, duration_sec);
        printf("PIN will be generated after a peer requests PIN display.\n");
    }
    return 0;
}

static int p2p_listen_pin_cmd(int argc, char **argv)
{
    return wl_p2p_listen_wps_cmd(argc, argv, BL_P2P_WPS_PIN_DISPLAY,
                                 "p2p_listen_pin");
}

static int p2p_listen_pbc_cmd(int argc, char **argv)
{
    return wl_p2p_listen_wps_cmd(argc, argv, BL_P2P_WPS_PBC,
                                 "p2p_listen_pbc");
}

static int p2p_connect_pbc_cmd(int argc, char **argv)
{
    return wl_p2p_connect_pbc_cmd(argc, argv);
}

static int p2p_connect_pin_cmd(int argc, char **argv)
{
    return wl_p2p_connect_pin_cmd(argc, argv);
}

static int p2p_stop_cmd(int argc, char **argv)
{
    return wl_p2p_stop_cmd(argc, argv);
}

static int p2p_deinit_cmd(int argc, char **argv)
{
    return wl_p2p_deinit_cmd(argc, argv);
}

static int p2p_go_intent_cmd(int argc, char **argv)
{
    int go_intent;
    int listen_wps_go_intent;

    if (argc < 3) {
        bl_p2p_get_go_intent(&go_intent, &listen_wps_go_intent);
        printf("usage: p2p_go_intent <go_intent 0-%d> "
               "<listen_wps_go_intent 0-%d>\n",
               WL_P2P_MAX_GO_INTENT, WL_P2P_MAX_GO_INTENT);
        printf("go_intent=%d listen_wps_go_intent=%d\n",
               go_intent, listen_wps_go_intent);
        return -1;
    }

    go_intent = atoi(argv[1]);
    listen_wps_go_intent = atoi(argv[2]);
    if (go_intent < 0 || go_intent > WL_P2P_MAX_GO_INTENT ||
        listen_wps_go_intent < 0 || listen_wps_go_intent > WL_P2P_MAX_GO_INTENT) {
        printf("invalid go_intent=%d listen_wps_go_intent=%d (range 0-%d)\n",
               go_intent, listen_wps_go_intent, WL_P2P_MAX_GO_INTENT);
        return -1;
    }

    if (bl_p2p_set_go_intent(go_intent, listen_wps_go_intent) < 0) {
        printf("p2p_go_intent failed\n");
        return -1;
    }

    printf("p2p_go_intent ok go_intent=%d listen_wps_go_intent=%d\n",
           go_intent, listen_wps_go_intent);
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(wl_p2p_init_cmd, wl_p2p_init, wl80211 p2p runtime init);
SHELL_CMD_EXPORT_ALIAS(wl_p2p_find_cmd, wl_p2p_find, wl80211 p2p find);
SHELL_CMD_EXPORT_ALIAS(wl_p2p_listen_cmd, wl_p2p_listen, wl80211 p2p listen);
SHELL_CMD_EXPORT_ALIAS(wl_p2p_listen_ch_cmd, wl_p2p_listen_ch,
                       wl80211 p2p listen on channel);
SHELL_CMD_EXPORT_ALIAS(wl_p2p_group_add_pbc_cmd, wl_p2p_group_add_pbc,
                       wl80211 p2p autonomous go with pbc);
SHELL_CMD_EXPORT_ALIAS(wl_p2p_group_add_pin_cmd, wl_p2p_group_add_pin,
                       wl80211 p2p autonomous go with pin);
SHELL_CMD_EXPORT_ALIAS(wl_p2p_connect_pbc_cmd, wl_p2p_connect_pbc,
                       wl80211 p2p connect via pbc);
SHELL_CMD_EXPORT_ALIAS(wl_p2p_connect_pin_cmd, wl_p2p_connect_pin,
                       wl80211 p2p connect via pin display);
SHELL_CMD_EXPORT_ALIAS(wl_p2p_prov_disc_cmd, wl_p2p_prov_disc,
                       wl80211 p2p provision discovery);
SHELL_CMD_EXPORT_ALIAS(wl_p2p_stop_cmd, wl_p2p_stop, wl80211 p2p stop);
SHELL_CMD_EXPORT_ALIAS(wl_p2p_deinit_cmd, wl_p2p_deinit, wl80211 p2p runtime deinit);
SHELL_CMD_EXPORT_ALIAS(p2p_init_cmd, p2p_init, prepare wl80211 p2p runtime);
SHELL_CMD_EXPORT_ALIAS(p2p_find_cmd, p2p_find, start wl80211 p2p discovery scan);
SHELL_CMD_EXPORT_ALIAS(p2p_listen_cmd, p2p_listen, start wl80211 p2p listen mode);
SHELL_CMD_EXPORT_ALIAS(p2p_listen_ch_cmd, p2p_listen_ch,
                       start wl80211 p2p listen mode on channel);
SHELL_CMD_EXPORT_ALIAS(p2p_group_add_cmd, p2p_group_add,
                       create a wl80211 P2P GO with PBC onboarding);
SHELL_CMD_EXPORT_ALIAS(p2p_wps_pbc_cmd, p2p_wps_pbc,
                       start wl80211 P2P GO with WPS PBC);
SHELL_CMD_EXPORT_ALIAS(p2p_wps_pin_cmd, p2p_wps_pin,
                       start wl80211 P2P GO with WPS PIN display mode);
SHELL_CMD_EXPORT_ALIAS(p2p_listen_pin_cmd, p2p_listen_pin,
                       listen for peer-initiated WPS PIN onboarding);
SHELL_CMD_EXPORT_ALIAS(p2p_listen_pbc_cmd, p2p_listen_pbc,
                       listen for peer-initiated WPS PBC onboarding);
SHELL_CMD_EXPORT_ALIAS(p2p_connect_pbc_cmd, p2p_connect_pbc,
                       join a wl80211 P2P GO with WPS PBC);
SHELL_CMD_EXPORT_ALIAS(p2p_connect_pin_cmd, p2p_connect_pin,
                       join a wl80211 P2P GO with WPS PIN display mode);
SHELL_CMD_EXPORT_ALIAS(p2p_stop_cmd, p2p_stop, stop wl80211 p2p runtime activity);
SHELL_CMD_EXPORT_ALIAS(p2p_deinit_cmd, p2p_deinit, deinit wl80211 p2p runtime);
SHELL_CMD_EXPORT_ALIAS(p2p_go_intent_cmd, p2p_go_intent,
                       set go_intent and listen_wps_go_intent);

#endif /* CONFIG_BL_SUPPLICANT_P2P */
