/**
  ******************************************************************************
  * @file    tcp_fota_shell.c
  * @version V1.0
  * @date
  * @brief   Shell commands for TCP OTA
  ******************************************************************************
  */

#include <stdio.h>
#include <string.h>

#ifdef CONFIG_SHELL
#include "shell.h"
#include "partition.h"
#include "bflb_flash.h"
#include "tcp_fota.h"
#include "ota/ota.h"

#ifdef CONFIG_IPC
#include <bflb_multi_core_sync.h>
#include <flash_ops_rpmsg.h>
#endif

/**
 * @brief Dump partition table information for debugging
 */
static void tcp_fota_dump_partition(pt_table_stuff_config *pt_stuff)
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
 * @brief Shell command: tcp_ota_start <ip> [port]
 */
int cmd_tcp_ota_start(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: tcp_ota_start <ip> [port]\r\n");
        return -1;
    }

    const char *ip = argv[1];
    const char *port = (argc >= 3) ? argv[2] : NULL;

    printf("Starting TCP OTA from %s:%s\r\n", ip, port ? port : TCP_FOTA_DEFAULT_PORT);

    int ret = tcp_fota(ip, port, NULL);
    if (ret != 0) {
        printf("TCP OTA failed: %d\r\n", ret);
    }

    return ret;
}

/**
 * @brief Shell command: tcp_ota_info - dump partition table
 */
int cmd_tcp_ota_info(int argc, char **argv)
{
    pt_table_stuff_config pt_table_stuff[2];
    pt_table_id_type active_id;

#ifdef CONFIG_IPC
#ifdef CONFIG_RPMSG_SERVICE_MODE_MASTER
    pt_table_set_flash_operation(bflb_flash_erase_mcs, bflb_flash_write_mcs, bflb_flash_read_mcs);
#else
    pt_table_set_flash_operation(bflb_flash_erase_rpmsg, bflb_flash_write_rpmsg, bflb_flash_read_rpmsg);
#endif
#else
    pt_table_set_flash_operation(bflb_flash_erase, bflb_flash_write, bflb_flash_read);
#endif

    active_id = pt_table_get_active_partition_need_lock(pt_table_stuff);
    if (PT_TABLE_ID_INVALID == active_id) {
        printf("No valid PT\r\n");
        return -1;
    }
    printf("Active PT:%d, Age %lu\r\n", active_id, pt_table_stuff[active_id].pt_table.age);

    tcp_fota_dump_partition(&pt_table_stuff[active_id]);
    return 0;
}

/**
 * @brief Shell command: tcp_ota_rollback - rollback to previous firmware
 */
int cmd_tcp_ota_rollback(int argc, char **argv)
{
    int ret = tcp_ota_rollback();
    if (ret != 0) {
        printf("Rollback failed: %d\r\n", ret);
    } else {
        printf("Rollback successful, reboot to apply\r\n");
    }
    return ret;
}

SHELL_CMD_EXPORT_ALIAS(cmd_tcp_ota_start, tcp_ota_start, Start TCP OTA update);
SHELL_CMD_EXPORT_ALIAS(cmd_tcp_ota_info, tcp_ota_info, Show OTA partition info);
SHELL_CMD_EXPORT_ALIAS(cmd_tcp_ota_rollback, tcp_ota_rollback, Rollback to previous firmware);
#endif
