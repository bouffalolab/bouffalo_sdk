#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <shell.h>

#include "macsw.h"

/* Shell command only. Station lifetime, counters, role, and connected time
 * stay in macsw. PHY rate formatting stays in this application so the shared
 * wifi6 libraries keep their original code layout.
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

static const char *station_stats_rate_format(const struct macsw_rx_rate *rate)
{
    switch (rate->format) {
    case FORMATMOD_NON_HT: return rate->mcs < 4 ? "CCK" : "OFDM";
    case FORMATMOD_NON_HT_DUP_OFDM: return "NON_HT_DUP";
    case FORMATMOD_HT_MF: return "HT-MF";
    case FORMATMOD_HT_GF: return "HT-GF";
    case FORMATMOD_VHT: return "VHT";
    case FORMATMOD_HE_SU: return "HE-SU";
    case FORMATMOD_HE_MU: return "HE-MU";
    case FORMATMOD_HE_ER: return "HE-ER";
    default: return "unknown";
    }
}

static void station_stats_rx_rate_print(const struct macsw_rx_rate *sample)
{
    /* Report N/A until a data sample is available; a default index is not MCS0. */
    struct macsw_rx_rate rate;
    uint32_t mbps10;

    if (!sample) {
        printf("  %-15s N/A (no data sample)\r\n", "last rx rate:");
        return;
    }
    rate = *sample;
    if (!rate.rate_kbps) {
        printf("  %-15s N/A (unsupported RX vector, format:%u, age:%lums)\r\n",
               "last rx rate:", rate.format, (unsigned long)rate.age_ms);
        return;
    }
    /* Integer formatting: round kbps once to the displayed tenth of a Mbps. */
    mbps10 = (rate.rate_kbps + 50) / 100;
    printf("  %-15s %lu.%lu Mbps PHY, format:%s",
           "last rx rate:", (unsigned long)(mbps10 / 10), (unsigned long)(mbps10 % 10),
           station_stats_rate_format(&rate));
    if (rate.format > FORMATMOD_NON_HT_DUP_OFDM) {
        printf(",mcs:%u,BW:%uMHz,NSS:%u,GI:%u.%uus,DCM:%u",
               rate.mcs, rate.bw_mhz, rate.nss,
               rate.gi_ns / 1000, (rate.gi_ns % 1000) / 100, rate.dcm);
        if (rate.ru_tones)
            printf(",RU:%u", rate.ru_tones);
    }
    printf(",age:%lums\r\n", (unsigned long)rate.age_ms);
}

static void station_stats_tx_rate_print(const struct macsw_tx_rate *sample)
{
    /* Show the completed transmission's rate, result and age, rather than the RC request. */
    struct macsw_tx_rate tx;
    const struct macsw_rx_rate *rate = &tx.phy;
    const char *result;
    uint32_t mbps10;

    if (!sample) {
        printf("  %-15s N/A (no completed data sample)\r\n", "last tx rate:");
        return;
    }
    tx = *sample;
    if (rate->rate_kbps) {
        mbps10 = (rate->rate_kbps + 50) / 100;
        printf("  %-15s %lu.%lu Mbps PHY, format:%s",
               "last tx rate:", (unsigned long)(mbps10 / 10), (unsigned long)(mbps10 % 10),
               station_stats_rate_format(rate));
        if (rate->format > FORMATMOD_NON_HT_DUP_OFDM)
            printf(",mcs:%u,BW:%uMHz,NSS:%u,GI:%u.%uus,DCM:%u",
                   rate->mcs, rate->bw_mhz, rate->nss,
                   rate->gi_ns / 1000, (rate->gi_ns % 1000) / 100, rate->dcm);
    } else {
        printf("  %-15s N/A (ambiguous or unsupported TX completion)", "last tx rate:");
    }
    result = tx.acked ? "ACK" : tx.expired ? "expired" :
             tx.retry_limit ? "retry-limit" : "no-ACK";
    printf(",result:%s,retries:%u,step:%u,aggregated:%u,seq:%lu,"
           "status:0x%08lx,source:tx-status+policy,age:%lums\r\n",
           result, tx.retries, tx.retry_step, tx.aggregated,
           (unsigned long)tx.sequence, (unsigned long)tx.status,
           (unsigned long)rate->age_ms);
}

static void station_stats_peer_rates_print(const struct macsw_peer_rates *rates)
{
    if (!rates) {
        printf("  %-15s N/A (station changed during query)\r\n", "last rx rate:");
        printf("  %-15s N/A (station changed during query)\r\n", "last tx rate:");
        return;
    }
    station_stats_rx_rate_print(rates->rx_valid ? &rates->rx : NULL);
    station_stats_tx_rate_print(rates->tx_valid ? &rates->tx : NULL);
}

static int station_stats_counter_width(uint64_t value, int min_width)
{
    int width = 1;

    while (value >= 10) {
        value /= 10;
        width++;
    }
    return width > min_width ? width : min_width;
}

static void station_stats_print(const struct macsw_station_snapshot *snapshot)
{
    /* Keep MPDU/management totals separate from the per-TID/non-QoS MSDU rows.
     * Identity, counters and rates share one snapshot; do not query a reused slot while printing.
     */
    const struct macsw_station_stats *stats = &snapshot->stats;

    printf("Station %02x:%02x:%02x:%02x:%02x:%02x\r\n",
           stats->mac[0], stats->mac[1], stats->mac[2],
           stats->mac[3], stats->mac[4], stats->mac[5]);
    /* Use spaces and fixed label widths; timestamps change terminal tab stops. */
    printf("  %-15s %s\r\n", "local role:", station_stats_role(stats->local_role));
    printf("  %-15s %llu\r\n", "rx bytes:", (unsigned long long)stats->rx_bytes);
    printf("  %-15s %u\r\n", "rx packets:", stats->rx_packets);
    printf("  %-15s %u\r\n", "rx dropped:", stats->rx_dropped);
    printf("  %-15s %llu\r\n", "tx bytes:", (unsigned long long)stats->tx_bytes);
    printf("  %-15s %u\r\n", "tx packets:", stats->tx_packets);
    if (stats->retries_valid)
        printf("  %-15s %u\r\n", "tx retries:", stats->tx_retries);
    printf("  %-15s %u\r\n", "tx failed:", stats->tx_failed);
    printf("  %-15s %llu\r\n", "beacon rx:", (unsigned long long)stats->rx_beacons);
    if (stats->signal_valid)
        printf("  %-15s %d dBm\r\n", "signal:", stats->signal_dbm);
    station_stats_peer_rates_print(&snapshot->rates);
    {
        int rx_width = 2, tx_width = 2, retries_width = 10, failed_width = 9;

        /* Allow room for non-QoS and size each numeric column from its header and
         * largest counter in the snapshot. Keep 64-bit values aligned without tabs.
         */
        for (unsigned int i = 0; i < MACSW_TID_STATS_COUNT; i++) {
            const struct macsw_tid_stats *ts = &snapshot->tids.tid[i];

            rx_width = station_stats_counter_width(ts->rx_msdu, rx_width);
            tx_width = station_stats_counter_width(ts->tx_msdu, tx_width);
            retries_width = station_stats_counter_width(ts->tx_msdu_retries, retries_width);
            failed_width = station_stats_counter_width(ts->tx_msdu_failed, failed_width);
        }
        printf("  MSDU:\r\n    %-7s %*s %*s %*s %*s\r\n",
               "TID", rx_width, "rx", tx_width, "tx",
               retries_width, "tx retries", failed_width, "tx failed");
        for (unsigned int i = 0; i < MACSW_TID_STATS_COUNT; i++) {
            const struct macsw_tid_stats *ts = &snapshot->tids.tid[i];

            if (i == MACSW_TID_STATS_NON_QOS)
                printf("    %-7s", "non-QoS");
            else
                printf("    %-7u", i);
            printf(" %*llu %*llu %*llu %*llu\r\n",
                   rx_width, (unsigned long long)ts->rx_msdu,
                   tx_width, (unsigned long long)ts->tx_msdu,
                   retries_width, (unsigned long long)ts->tx_msdu_retries,
                   failed_width, (unsigned long long)ts->tx_msdu_failed);
        }
    }
    if (stats->time_valid) {
        uint32_t seconds = stats->connected_seconds;
        uint32_t days = seconds / 86400;
        uint32_t hours = (seconds / 3600) % 24;
        uint32_t minutes = (seconds / 60) % 60;

        seconds %= 60;
        if (days)
            printf("  %-15s %u d %u h %u min %u s\r\n",
                   "connected time:", days, hours, minutes, seconds);
        else if (hours)
            printf("  %-15s %u h %u min %u s\r\n",
                   "connected time:", hours, minutes, seconds);
        else if (minutes)
            printf("  %-15s %u min %u s\r\n", "connected time:", minutes, seconds);
        else
            printf("  %-15s %u s\r\n", "connected time:", seconds);
    }
}

static void station_stats_usage(void)
{
    printf("Usage: wifi_station_stats [enable|disable|-c] [aa:bb:cc:dd:ee:ff]\r\n");
}

static void station_stats_print_peers(const uint8_t *mac)
{
    struct macsw_station_stats stats;
    struct macsw_station_snapshot snapshot;
    uint8_t cursor = 0xff;
    int found = 0;
    int changed = 0;

    while (!export_station_stats_next(&cursor, &stats)) {
        if (mac && memcmp(stats.mac, mac, sizeof(stats.mac)))
            continue;
        /* Skip the entire report if the cursor's generation has expired. Never print
         * an old peer's MAC/counters with a replacement peer's rates or TID counters.
         */
        if (export_station_snapshot_get(cursor, stats.generation, &snapshot)) {
            printf("Station changed during query; retry command\r\n");
            changed = 1;
            continue;
        }
        station_stats_print(&snapshot);
        found++;
        if (mac)
            break;
    }
    /* A generation-change message already explains why no report was printed. */
    if (!found && !changed)
        printf(mac ? "Station not found\r\n" : "No associated stations\r\n");
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
