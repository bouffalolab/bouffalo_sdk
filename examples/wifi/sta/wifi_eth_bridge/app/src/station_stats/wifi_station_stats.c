#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <shell.h>

#include "macsw.h"

/* Shell command only. Station lifetime, counters, role, and connected time
 * stay in macsw. This file does not call fhost.
 */

static int station_stats_parse_mac(const char *str, uint8_t mac[6])
{
    const char *ptr = str;
    unsigned int i;

    if (!str || !mac || strlen(str) < 17)
        return -1;
    for (i = 0; i < 6; i++) {
        char *next;
        long hex = strtol(ptr, &next, 16);

        if ((unsigned)hex > 255 || (hex == 0 && next == ptr) ||
            (i < 5 && *next != ':') || (i == 5 && *next != '\0'))
            return -1;
        mac[i] = (uint8_t)hex;
        ptr = next + 1;
    }
    return 0;
}

static const char *station_stats_role(uint8_t role)
{
    if (role == MACSW_STATION_STATS_ROLE_STA)
        return "STA";
    if (role == MACSW_STATION_STATS_ROLE_SOFTAP)
        return "SoftAP";
    return "other";
}

static void station_stats_print(const struct macsw_station_stats *stats)
{
    printf("Station %02x:%02x:%02x:%02x:%02x:%02x\r\n",
           stats->mac[0], stats->mac[1], stats->mac[2],
           stats->mac[3], stats->mac[4], stats->mac[5]);
    printf("\tlocal role:\t%s\r\n", station_stats_role(stats->local_role));
    printf("\trx bytes:\t%llu\r\n", (unsigned long long)stats->rx_bytes);
    printf("\trx packets:\t%u\r\n", stats->rx_packets);
    printf("\ttx bytes:\t%llu\r\n", (unsigned long long)stats->tx_bytes);
    printf("\ttx packets:\t%u\r\n", stats->tx_packets);
    if (stats->retries_valid)
        printf("\ttx retries:\t%u\r\n", stats->tx_retries);
    printf("\ttx failed:\t%u\r\n", stats->tx_failed);
    printf("\tbeacon rx:\t%llu\r\n", (unsigned long long)stats->rx_beacons);
    if (stats->signal_valid)
        printf("\tsignal:\t%d dBm\r\n", stats->signal_dbm);
    if (stats->time_valid) {
        uint32_t seconds = stats->connected_seconds;
        uint32_t days = seconds / 86400;
        uint32_t hours = (seconds / 3600) % 24;
        uint32_t minutes = (seconds / 60) % 60;

        seconds %= 60;
        if (days)
            printf("\tconnected time:\t%u d %u h %u min %u s\r\n",
                   days, hours, minutes, seconds);
        else if (hours)
            printf("\tconnected time:\t%u h %u min %u s\r\n",
                   hours, minutes, seconds);
        else if (minutes)
            printf("\tconnected time:\t%u min %u s\r\n", minutes, seconds);
        else
            printf("\tconnected time:\t%u s\r\n", seconds);
    }
}

static void station_stats_usage(void)
{
    printf("Usage: wifi_station_stats [enable|disable|-c] [aa:bb:cc:dd:ee:ff]\r\n");
}

static void station_stats_print_peers(const uint8_t *mac)
{
    struct macsw_station_stats stats;
    uint8_t cursor = 0xff;
    int found = 0;

    if (mac) {
        if (export_station_stats_get_mac(mac, &stats))
            printf("Station not found\r\n");
        else
            station_stats_print(&stats);
        return;
    }

    while (!export_station_stats_next(&cursor, &stats)) {
        station_stats_print(&stats);
        found++;
    }
    if (!found)
        printf("No associated stations\r\n");
}

void wifi_station_stats_cmd(int argc, char **argv)
{
    uint8_t mac[6];
    const uint8_t *filter = NULL;
    int ret;

    if (argc == 2 && strcmp(argv[1], "enable") == 0) {
        export_station_stats_set(1);
        printf("%s\r\n", export_station_stats_enabled() ? "enabled" : "disabled");
        return;
    }
    if (argc == 2 && strcmp(argv[1], "disable") == 0) {
        export_station_stats_set(0);
        printf("disabled\r\n");
        return;
    }
    if (argc >= 2 && strcmp(argv[1], "-c") == 0) {
        if (argc > 3 || (argc == 3 && station_stats_parse_mac(argv[2], mac))) {
            station_stats_usage();
            return;
        }
        ret = (argc == 3) ? export_station_stats_reset_mac(mac) :
                            export_station_stats_reset(0xff, 0);
        printf(ret ? "Station reset failed\r\n" : "Station stats reset\r\n");
        return;
    }
    if (argc > 2 || (argc == 2 && station_stats_parse_mac(argv[1], mac))) {
        station_stats_usage();
        return;
    }
    if (!export_station_stats_enabled()) {
        printf("disabled\r\n");
        return;
    }
    if (argc == 2)
        filter = mac;
    station_stats_print_peers(filter);
}
SHELL_CMD_EXPORT_ALIAS(wifi_station_stats_cmd, wifi_station_stats, Enable disable query or clear per-peer station statistics);
