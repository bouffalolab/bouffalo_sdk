/**
 * @file test_bflbwifi.c
 * @brief BFLB WiFi module simple example
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bflbwifi.h"
#include "bflbwifi_log.h"

int main(int argc, char *argv[])
{
    int ret;
    bflbwifi_ap_info_t aps[50];
    int count;

    /* Check parameters */
    if (argc < 2) {
        printf("Usage: %s <tty_device> [ssid] [password]\n", argv[0]);
        printf("Example: %s /dev/ttyUSB0 MyWiFi password123\n", argv[0]);
        return 1;
    }

    /* Set log level */
    bflbwifi_log_set_level(LOG_INFO);

    printf("==========================================\n");
    printf("BFLB WiFi Test\n");
    printf("==========================================\n");

    /* 1. Initialize */
    printf("\n ========== [1] Initializing...\n");
    ret = bflbwifi_init(argv[1], 115200);
    if (ret != 0) {
        printf("Init failed: %s\n", bflbwifi_strerror(ret));
        return 1;
    }
    printf("Init OK\n");

    /* 2. Get firmware version */
    printf("\n ========== [2] Getting firmware version...\n");
    char version[64] = {0};
    ret = bflbwifi_get_version(version, sizeof(version));
    if (ret == 0) {
        printf("Firmware version: %s\n", version);
    }

    /* 3. Scan APs */
    printf("\n ========== [3] Scanning APs...\n");
    ret = bflbwifi_scan(aps, 50, &count, 0);
    if (ret == 0 || ret == -E_ERR_BUFFER_TOO_SMALL) {
        printf("Found %d APs:\n", count);
        for (int i = 0; i < count && i < 10; i++) {
            printf("  [%d] %s - %d dBm, CH:%d, ENC:%d\n",
                   i + 1, aps[i].ssid, aps[i].rssi, aps[i].channel, aps[i].enc);
        }
        if (count > 10) {
            printf("  ... and %d more\n", count - 10);
        }
    } else {
        printf("Scan failed: %s\n", bflbwifi_strerror(ret));
    }

    /* If SSID and password are provided, try to connect */
    if (argc >= 4) {
        printf("\n ========== [4] Connecting to %s...\n", argv[2]);

        /* Prepare connection configuration */
        bflbwifi_sta_config_t config;
        strncpy(config.ssid, argv[2], sizeof(config.ssid) - 1);
        strncpy(config.pwd, argv[3], sizeof(config.pwd) - 1);

        ret = bflbwifi_sta_connect(&config, 0);
        if (ret != 0) {
            printf("Connect failed: %s\n", bflbwifi_strerror(ret));
            goto cleanup;
        }
        printf("Connected!\n");

        /* 5. Query status */
        printf("\n ========== [5] Getting connection state...\n");
        sleep(1);  /* Wait for IP assignment */
        bflbwifi_sta_state_t state;
        ret = bflbwifi_sta_get_state(&state);
        if (ret == 0) {
            const char *state_str[] = {
                "IDLE", "CONNECTING", "CONNECTED", "GOTIP", "DISCONNECTED", "ERROR"
            };
            printf("State: %s\n", state < 6 ? state_str[state] : "UNKNOWN");
        }

        /* 6. Wait for a while */
        printf("\n ========== [6] Waiting 10 seconds...\n");
        sleep(10);

        /* 7. Disconnect */
        printf("\n ========== [7] Disconnecting...\n");
        bflbwifi_sta_disconnect();
        printf("Disconnected\n");
    } else {
        printf("\n ========== [4] No SSID/password provided, skipping connect test\n");
        printf("    To test connection: %s %s <ssid> <password>\n", argv[0], argv[1]);
    }

cleanup:
    /* Clean up */
    printf("\n ========== [0] Cleaning up...\n");
    bflbwifi_deinit();
    printf("Done\n");

    return 0;
}
