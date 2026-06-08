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

    printf("dnld total=%u drop=%u ok=%u\n", status.statistics.dnld_total_packets, status.statistics.dnld_total_dropped,
           status.statistics.dnld_total_success);
    printf("upld total=%u producer=%u free=%u\n", status.statistics.upld_total_packets,
           status.statistics.upld_producer_count, status.statistics.upld_free_count);
}

static int cmd_nethub(int argc, char **argv)
{
    nethub_runtime_status_t status;

    if (argc >= 2) {
        if (strcmp(argv[1], "host") == 0) {
            nethub_channel_t host_link;
            int ret;

            if (argc != 3) {
                printf("Usage: nethub host <sdio|usb|spi>\n");
                return -1;
            }

            if (strcmp(argv[2], "sdio") == 0) {
                host_link = NETHUB_CHANNEL_SDIO;
            } else if (strcmp(argv[2], "usb") == 0) {
                host_link = NETHUB_CHANNEL_USB;
            } else if (strcmp(argv[2], "spi") == 0) {
                host_link = NETHUB_CHANNEL_SPI;
            } else {
                printf("Invalid host link: %s\n", argv[2]);
                return -1;
            }

            ret = nethub_set_active_host_link(host_link);
            if (ret != NETHUB_OK) {
                printf("set host link failed: %d\n", ret);
                return -1;
            }
        } else if (strcmp(argv[1], "status") != 0) {
            printf("Usage: nethub [status|host <sdio|usb|spi>]\n");
            return -1;
        }
    }

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

SHELL_CMD_EXPORT_ALIAS(cmd_nethub, nethub, nethub status|host <sdio|usb|spi>);

#if !defined(CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE) || !CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE
static void cmd_nethub_set_wifi_channel(int argc, char **argv)
{
    if (argc < 2 || argv[1] == NULL) {
        printf("Usage: nethub_set_wifi_channel <sta|ap>\r\n");
        return;
    }

    if (strcmp(argv[1], "sta") == 0) {
        nethub_set_active_wifi_channel(NETHUB_CHANNEL_WIFI_STA);
        printf("Active WiFi channel set to STA\r\n");
    } else if (strcmp(argv[1], "ap") == 0) {
        nethub_set_active_wifi_channel(NETHUB_CHANNEL_WIFI_AP);
        printf("Active WiFi channel set to AP\r\n");
    } else {
        printf("Invalid argument: %s (use 'sta' or 'ap')\r\n", argv[1]);
    }
}
SHELL_CMD_EXPORT_ALIAS(cmd_nethub_set_wifi_channel, nethub_set_wifi_channel, set active wifi channel to sta or ap);
#endif
#endif
