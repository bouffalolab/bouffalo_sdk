/**
 * @file nhsdiowifi_cli.c
 * @brief SDIO WiFi port adapter layer CLI command implementation
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef CONFIG_NETHUB_DEBUG
#include "shell.h"
#include "nhsdiowifi.h"

/* Interface type mapping table */
typedef struct {
    const char *name;
    nhif_type_t type;
} interface_map_t;

static const interface_map_t interface_map[] = {
    {"sdio", NHIF_TYPE_SDIO},
    {"sta", NHIF_TYPE_STA},
    {"tcpipsta", NHIF_TYPE_TCPIPSTA},
    {"ap", NHIF_TYPE_AP},
    {"tcpipap", NHIF_TYPE_TCPIPAP},
};

static const int interface_count = sizeof(interface_map) / sizeof(interface_map_t);

/**
 * @brief Get interface type by interface name
 * @param name Interface name
 * @return Interface type, return -1 if not found
 */
static nhif_type_t get_interface_type_by_name(const char *name)
{
    for (int i = 0; i < interface_count; i++) {
        if (strcmp(interface_map[i].name, name) == 0) {
            return interface_map[i].type;
        }
    }
    return -1;
}

/**
 * @brief Get interface name by interface type
 * @param type Interface type
 * @return Interface name, return NULL if not found
 */
static const char *get_interface_name_by_type(nhif_type_t type)
{
    for (int i = 0; i < interface_count; i++) {
        if (interface_map[i].type == type) {
            return interface_map[i].name;
        }
    }
    return NULL;
}

/**
 * @brief Create test packet
 * @param data Data to be sent
 * @param interface_name Interface name
 * @return Created skb pointer, return NULL on failure
 */
static nh_skb_t *create_test_skb(const char *data, const char *interface_name)
{
    nh_skb_t *skb = malloc(sizeof(nh_skb_t));
    if (!skb) {
        printf("[SDIOWIFI_CLI] skb allocation failed\n");
        return NULL;
    }

    /* Create data buffer */
    size_t data_len = strlen(data) + strlen(interface_name) + 4; // +4 for "[], " and null
    char *buffer = malloc(data_len);
    if (!buffer) {
        printf("[SDIOWIFI_CLI] data buffer allocation failed\n");
        free(skb);
        return NULL;
    }

    snprintf(buffer, data_len, "[%s] %s", interface_name, data);

    skb->data = (uint8_t *)buffer;
    skb->len = strlen(buffer);
    skb->next = NULL;
    skb->cb_priv = buffer;
    skb->cb_arg = NULL;
    skb->free_cb = NULL;

    return skb;
}

/**
 * @brief Free test packet
 * @param skb skb to be freed
 */
static void free_test_skb(nh_skb_t *skb)
{
    if (skb) {
        if (skb->cb_priv) {
            free(skb->cb_priv);
        }
        free(skb);
    }
}

/**
 * @brief Show help information
 */
static void show_help(void)
{
    printf("SDIOWIFI CLI command help:\n");
    printf("  sdiowifi send <interface> <data>  - Send data to specified interface\n");
    printf("    interface: sdio, sta, tcpipsta, ap, tcpipap\n");
    printf("    data: String data to be sent\n");
    printf("  sdiowifi dump [rules]            - Print nethub related information\n");
    printf("    rules: Print forwarding rules (default)\n");
    printf("  sdiowifi help                    - Show this help information\n");
    printf("\n");
    printf("Examples:\n");
    printf("  sdiowifi send sta \"Hello World\"    - Send \"Hello World\" to STA interface\n");
    printf("  sdiowifi dump rules               - Print forwarding rules\n");
}

/**
 * @brief Send data to specified interface
 * @param interface_name Interface name
 * @param data Data to be sent
 * @return 0 for success, -1 for failure
 */
static int send_data_to_interface(const char *interface_name, const char *data)
{
    nhif_type_t interface_type = get_interface_type_by_name(interface_name);
    if (interface_type == -1) {
        printf("[SDIOWIFI_CLI] Error: Unknown interface type '%s'\n", interface_name);
        return -1;
    }

    /* Create test packet */
    nh_skb_t *skb = create_test_skb(data, interface_name);
    if (!skb) {
        printf("[SDIOWIFI_CLI] Error: Failed to create packet\n");
        return -1;
    }

    printf("[SDIOWIFI_CLI] Sending data to %s interface: %s\n", interface_name, data);

    /* Send data to nethub */
    nh_forward_result_t result = nethub_process_input(skb, interface_type);

    if (result == NH_FORWARD_STOP) {
        printf("[SDIOWIFI_CLI] Data consumed, forwarding stopped\n");
    } else if (result == NH_FORWARD_CONTINUE) {
        printf("[SDIOWIFI_CLI] Data forwarding continues\n");
    } else {
        printf("[SDIOWIFI_CLI] Forwarding result abnormal: %d\n", result);
    }

    /* Free packet */
    free_test_skb(skb);

    return 0;
}

/**
 * @brief Print dump information
 * @param dump_type dump type
 * @return 0 for success, -1 for failure
 */
static int dump_info(const char *dump_type)
{
    printf("\n=== nethub information print ===\n");

    if (dump_type == NULL || strcmp(dump_type, "rules") == 0) {
        printf("--- Forwarding rules configuration ---\n");
        nh_forward_print_rules();
        printf("----------------------\n");
    } else {
        printf("[SDIOWIFI_CLI] Error: Unknown dump type '%s'\n", dump_type);
        printf("Supported types: rules\n");
        return -1;
    }

    printf("=======================\n\n");
    return 0;
}

/**
 * @brief sdiowifi CLI main command handler function
 * @param argc Parameter count
 * @param argv Parameter array
 * @return 0 for success, other values for failure
 */
int cmd_sdiowifi(int argc, char **argv)
{
    if (argc < 2) {
        printf("[SDIOWIFI_CLI] Error: Insufficient parameters\n");
        show_help();
        return -1;
    }

    const char *subcmd = argv[1];

    if (strcmp(subcmd, "help") == 0) {
        show_help();
        return 0;
    } else if (strcmp(subcmd, "send") == 0) {
        if (argc != 4) {
            printf("[SDIOWIFI_CLI] Error: send command requires 3 parameters\n");
            printf("Usage: sdiowifi send <interface> <data>\n");
            return -1;
        }
        return send_data_to_interface(argv[2], argv[3]);
    } else if (strcmp(subcmd, "dump") == 0) {
        const char *dump_type = (argc >= 3) ? argv[2] : "rules";
        return dump_info(dump_type);
    } else {
        printf("[SDIOWIFI_CLI] Error: Unknown subcommand '%s'\n", subcmd);
        show_help();
        return -1;
    }
}

/* Register shell command */
SHELL_CMD_EXPORT_ALIAS(cmd_sdiowifi, sdiowifi, SDIO WiFi port CLI commands);
#endif

