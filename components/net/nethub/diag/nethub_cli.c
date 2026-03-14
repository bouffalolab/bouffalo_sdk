#ifdef CONFIG_NETHUB_DEBUG

#include "shell.h"
#include "nethub.h"

#include <stdio.h>

static const char *nethub_cli_channel_name(nethub_channel_t channel)
{
    switch (channel) {
        case NETHUB_CHANNEL_WIFI_STA:
            return "wifi_sta";
        case NETHUB_CHANNEL_WIFI_AP:
            return "wifi_ap";
        case NETHUB_CHANNEL_STACK_STA:
            return "stack_sta";
        case NETHUB_CHANNEL_STACK_AP:
            return "stack_ap";
        case NETHUB_CHANNEL_STACK_NAT:
            return "stack_nat";
        case NETHUB_CHANNEL_BRIDGE:
            return "bridge";
        case NETHUB_CHANNEL_SDIO:
            return "sdio";
        case NETHUB_CHANNEL_USB:
            return "usb";
        case NETHUB_CHANNEL_SPI:
            return "spi";
        default:
            return "unknown";
    }
}

static void nethub_print_stats(void)
{
    nethub_runtime_status_t status;

    if (nethub_get_status(&status) != NETHUB_OK) {
        return;
    }

    printf("dnld total=%u drop=%u ok=%u\n",
           status.statistics.dnld_total_packets,
           status.statistics.dnld_total_dropped,
           status.statistics.dnld_total_success);
    printf("upld total=%u producer=%u free=%u\n",
           status.statistics.upld_total_packets,
           status.statistics.upld_producer_count,
           status.statistics.upld_free_count);
}

static int cmd_nethub(int argc, char **argv)
{
    nethub_runtime_status_t status;

    (void)argc;
    (void)argv;

    if (nethub_get_status(&status) != NETHUB_OK) {
        return -1;
    }

    printf("profile      : %s\n", status.profile_name);
    printf("initialized  : %s\n", status.initialized ? "yes" : "no");
    printf("started      : %s\n", status.started ? "yes" : "no");
    printf("wifi filter  : %s\n", status.custom_wifi_rx_filter_active ? "custom" : "builtin");
    printf("host link    : %s\n", nethub_cli_channel_name(status.host_channel));
    printf("wifi channel : %s\n", nethub_cli_channel_name(status.active_wifi_channel));
    nethub_print_stats();
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_nethub, nethub, nethub status);

#endif
