#include "wifi_ota.h"
#include "wifi_ota_internal.h"
#include "partition.h"
#include "shell.h"
#include "bflb_flash.h"

extern bool g_ota_initialized;
extern bool g_ota_in_progress;

/**
 * @brief Dump partition table information for debugging
 * @param pt_stuff Pointer to partition table stuff
 */
static void wifi_ota_dump_partition(pt_table_stuff_config *pt_stuff)
{
    printf("======= PtTable_Config @%p=======\r\n", pt_stuff);
    printf("magicCode 0x%08X;", (unsigned int)(pt_stuff->pt_table.magicCode));
    printf(" version 0x%04X;", pt_stuff->pt_table.version);
    printf(" entryCnt %u;", pt_stuff->pt_table.entryCnt);
    printf(" age %lu;", pt_stuff->pt_table.age);
    printf(" crc32 0x%08X\r\n", (unsigned int)pt_stuff->pt_table.crc32);
    printf(" idx  type device active_index    name    address[0]   address[1]   length[0]    length[1]   age \r\n");
    for (uint32_t i = 0; i < pt_stuff->pt_table.entryCnt; i++) {
        printf("[%02d] ", i);
        printf("  %02u", (pt_stuff->pt_entries[i].type));
        printf("     %u", (pt_stuff->pt_entries[i].device));
        printf("        %u", (pt_stuff->pt_entries[i].active_index));
        printf("       %8s", (pt_stuff->pt_entries[i].name));
        printf("   0x%08lx", (pt_stuff->pt_entries[i].start_address[0]));
        printf("   0x%08lx", (pt_stuff->pt_entries[i].start_address[1]));
        printf("   0x%08lx", (pt_stuff->pt_entries[i].max_len[0]));
        printf("   0x%08lx", (pt_stuff->pt_entries[i].max_len[1]));
        printf("   %lu\r\n", (pt_stuff->pt_entries[i].age));
    }
}

/**
 * @brief Shell command handler for wifi_ota_start
 * @param argc Argument count
 * @param argv Argument vector
 * @return int 0 on success, negative on error
 */
int cmd_wifi_ota_start(int argc, char **argv)
{
    // Auto-initialize OTA if not already initialized
    if (!g_ota_initialized) {
        wifi_ota_init();
    } else {
        printf("Error: initialized already.\r\n");
        return -1;
    }

    // Parse arguments
    if (argc < 2) {
        printf("Usage: wifi_ota_start [ip] [port]\r\n");
        printf("  ip: OTA server IP address\r\n");
        printf("  port: OTA server port (optional)\r\n");
        return -1;
    }

    // Extract server IP and port from arguments
    const char *server_ip = argv[1];
    const char *server_port = (argc >= 3) ? argv[2] : NULL;

    printf("Starting OTA update from %s:%s\r\n", server_ip, server_port ? server_port : "default");

    // Start OTA update
    int ret = wifi_ota_start_update(server_ip, server_port);
    if (ret != 0) {
        printf("OTA update failed with error: %d\r\n", ret);
    }

    return ret;
}

/**
 * @brief Shell command handler for wifi_ota_info
 * @param argc Argument count
 * @param argv Argument vector
 * @return int 0 on success
 */
int cmd_wifi_ota_info(int argc, char **argv)
{
    pt_table_stuff_config pt_table_stuff[2];
    pt_table_id_type active_id;

    // bflb_flash_init();
    pt_table_set_flash_operation(bflb_flash_erase, bflb_flash_write, bflb_flash_read);

    active_id = pt_table_get_active_partition_need_lock(pt_table_stuff);
    if (PT_TABLE_ID_INVALID == active_id) {
        printf("No valid PT\r\n");
        return -1;
    }
    printf("Active PT:%d,Age %d\r\n", active_id, pt_table_stuff[active_id].pt_table.age);

    wifi_ota_dump_partition(&pt_table_stuff[active_id]);
    return 0;
}

// Register shell commands
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_ota_start, wifi_ota_start, Start WiFi OTA update);
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_ota_info, wifi_ota_info, Show OTA partition info);