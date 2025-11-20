#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef CONFIG_NETHUB_DEBUG
#include "shell.h"
#include "nethub.h"
#include "nh_forward.h"
#include "nethub_types.h"

/* External NETHUB statistics variable declaration */
extern nethub_statistics_t g_nethub_stats;

/* Interface type mapping table */
static const struct {
    const char *name;
    nhif_type_t type;
} interface_map[] = {
    {"sta", NHIF_TYPE_STA},
    {"ap", NHIF_TYPE_AP},
    {"tcpipsta", NHIF_TYPE_TCPIPSTA},
    {"tcpipap", NHIF_TYPE_TCPIPAP},
    {"tcpipnat", NHIF_TYPE_TCPIPNAT},
    {"bridge", NHIF_TYPE_BRIDGE},
    {"sdio", NHIF_TYPE_SDIO},
    {"usb", NHIF_TYPE_USB},
    {"spi", NHIF_TYPE_SPI},
};

static const int interface_count = sizeof(interface_map) / sizeof(interface_map[0]);

/**
 * @brief Print framework status information
 */
static void print_framework_status(void)
{
    if (nethub_is_initialized()) {
        printf("nethub framework: initialized\n");
    } else {
        printf("nethub framework: not initialized\n");
    }
}

/**
 * @brief Print NETHUB statistics information
 */
static void print_nethub_statistics(void)
{
    printf("\n--- DNLD Download Statistics ---\n");
    printf("Producer Statistics:\n");
    printf("  Total packets generated: %u\n", g_nethub_stats.dnld_producer_count);

    printf("Consumer Statistics:\n");
    printf("  Dropped due to network interface down: %u\n", g_nethub_stats.dnld_consumer_netif_down);
    printf("  Dropped due to TX failure: %u\n", g_nethub_stats.dnld_consumer_tx_fail);
    printf("  Normally released packets: %u\n", g_nethub_stats.dnld_consumer_free_normal);
    printf("  Abnormally released packets: %u\n", g_nethub_stats.dnld_consumer_free_abnormal);

    printf("DNLD Overall Statistics:\n");
    printf("  Total processed packets: %u\n", g_nethub_stats.dnld_total_packets);
    printf("  Total dropped packets: %u\n", g_nethub_stats.dnld_total_dropped);
    printf("  Total successfully processed packets: %u\n", g_nethub_stats.dnld_total_success);

    printf("\n--- UPLD Upload Statistics ---\n");
    printf("Producer and Flow Through Statistics:\n");
    printf("  WiFi data entered system: %u\n", g_nethub_stats.upld_producer_count);
    printf("  Data flow through tcpipsta: %u\n", g_nethub_stats.upld_flow_through);

    printf("Consumer Statistics:\n");
    printf("  Start transferring to SDIO module: %u\n", g_nethub_stats.upld_start_consume);
    printf("  SDIO transfer complete: %u\n", g_nethub_stats.upld_transfer_complete);
    printf("  Finally released packets: %u\n", g_nethub_stats.upld_free_count);

    printf("UPLD Overall Statistics:\n");
    printf("  Total processed packets: %u\n", g_nethub_stats.upld_total_packets);
    printf("---------------------------\n");
}

/**
 * @brief Show help information
 */
static void show_help(void)
{
    printf("Nethub CLI Command Help:\n");
    printf("  nethub                           - Show nethub framework status (default command)\n");
    printf("  nethub status                    - Show nethub framework status\n");
    printf("  nethub help                      - Show this help information\n");
    printf("\n");
    printf("Examples:\n");
    printf("  nethub                           - View nethub status and statistics information\n");
    printf("  nethub status                    - View nethub status information\n");
}

static int show_macsw_status(void)
{
    //int show_fhost_status(void);
    //show_fhost_status();
    printf("HW rx rb start:0x%08X, end:0x%08X, r:0x%08X, w:0x%08X\n",
            *(volatile uint32_t *)0x24b081c8, *(volatile uint32_t *)0x24b081cc,
            *(volatile uint32_t *)0x24b081d0, *(volatile uint32_t *)0x24b081d4);
    return 0;
}
/**
 * @brief Show nethub status information
 * @return 0 for success, -1 for failure
 */
static int show_status(void)
{
    printf("\n=== nethub Framework Status ===\n");

    printf("--- Framework Status ---\n");
    print_framework_status();

    printf("--- Supported Interface Types ---\n");
    for (int i = 0; i < interface_count; i++) {
        printf("  %-12s (type: %d)\n", interface_map[i].name, interface_map[i].type);
    }

    /* Display NETHUB statistics information */
    print_nethub_statistics();

    printf("--- Forwarding Rules Configuration ---\n");
    nh_forward_print_rules();
    printf("----------------------\n");

    printf("--- Other Configuration ---\n");
    show_macsw_status();
    printf("----------------------\n");

    printf("========================\n\n");
    return 0;
}


/**
 * @brief nethub CLI main command handler function
 * @param argc Number of arguments
 * @param argv Argument array
 * @return 0 for success, other values for failure
 */
int cmd_nethub(int argc, char **argv)
{
    /* Default behavior: execute status command when no arguments or only "nethub" */
    if (argc == 1) {
        print_nethub_statistics();
        show_macsw_status();
        return 0;
    }

    const char *subcmd = argv[1];

    if (strcmp(subcmd, "help") == 0) {
        show_help();
        return 0;
    } else if (strcmp(subcmd, "status") == 0) {
        return show_status();
    } else {
        printf("[NETHUB_CLI] Error: Unknown subcommand '%s'\n", subcmd);
        show_help();
        return -1;
    }
}

/* Register shell command */
SHELL_CMD_EXPORT_ALIAS(cmd_nethub, nethub, Nethub framework management commands);
#endif
