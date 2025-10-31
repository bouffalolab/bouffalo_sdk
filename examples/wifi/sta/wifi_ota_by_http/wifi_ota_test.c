
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <lwip/api.h>
#include <lwip/arch.h>
#include <lwip/opt.h>
#include <lwip/inet.h>
#include <lwip/errno.h>
#include <lwip/apps/http_client.h>
#include <netdb.h>

#include "bflb_mtimer.h"
#include "bflb_flash.h"
#include "bflb_l1c.h"

#include "shell.h"
#include "utils_getopt.h"
#include "partition.h"
#include "ring_buffer.h"

#define UTILS_SHA256 0
#define MBEDTLS_SHA256 1
#define SHA256_SEL MBEDTLS_SHA256

#if SHA256_SEL
#include "mbedtls/sha256.h"
#include "mbedtls_sample_config.h"
mbedtls_sha256_context ctx_sha256;
#else
#include "utils_sha256.h"
sha256_context ctx_sha256;
#endif

uint8_t sha256_img[32];
uint8_t sha256_result[32];

typedef struct ota_header {
    union {
        struct {
            uint8_t header[16];

            uint8_t type[4]; //RAW XZ
            uint32_t len;    //body len
            uint8_t pad0[8];

            uint8_t ver_hardware[16];
            uint8_t ver_software[16];

            uint8_t sha256[32];
        } s;
        uint8_t _pad[512];
    } u;
} ota_header_t;

#define OTA_HEADER_SIZE               (sizeof(ota_header_t))
#define OTA_PROGRAM_SIZE              (TCP_MSS)
#define OTA_PARTITION_NAME_TYPE_FW    "FW"
#define OTA_PARTITION_NAME_TYPE_PSM   "PSM"
#define OTA_PARTITION_NAME_TYPE_ROMFS "media"

// clang-format on

static uint64_t total_cnt = 0;

static pt_table_stuff_config pt_table_stuff[2];
static pt_table_id_type active_id;
static pt_table_entry_config pt_fw_entry;

static int ota_header_found = 0, use_xz = 0;
static ota_header_t *ota_header;
// ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t recv_buffer[OTA_PROGRAM_SIZE] __attribute__((aligned(32)));
static uint8_t *recv_buffer;
static int status = 0;
static uint32_t buffer_offset = 0, write_flash_offset = 0, ota_addr;
static uint32_t bin_size; // part_size;

static uint32_t httpc_get_done_flag = 0;

static int _check_ota_header(ota_header_t *ota_header, uint32_t *ota_len, int *use_xz)
{
    char str[33]; //assume max segment size
    int i;

    memcpy(str, ota_header->u.s.header, sizeof(ota_header->u.s.header));
    str[sizeof(ota_header->u.s.header)] = '\0';
    printf("[OTA] [HEADER] ota header is %s\r\n", str);

    memcpy(str, ota_header->u.s.type, sizeof(ota_header->u.s.type));
    str[sizeof(ota_header->u.s.type)] = '\0';
    printf("[OTA] [HEADER] file type is %s\r\n", str);

    if (strstr(str, "XZ")) {
        *use_xz = 1;
    } else {
        *use_xz = 0;
    }

    if ((!strcmp(str, "RAW")) && (*use_xz != 1)) {
        return 1;
    }

    memcpy(ota_len, &(ota_header->u.s.len), 4);
    printf("[OTA] [HEADER] file length (exclude ota header) is %lu\r\n", *ota_len);

    memcpy(str, ota_header->u.s.ver_hardware, sizeof(ota_header->u.s.ver_hardware));
    str[sizeof(ota_header->u.s.ver_hardware)] = '\0';
    printf("[OTA] [HEADER] ver_hardware is %s\r\n", str);

    memcpy(str, ota_header->u.s.ver_software, sizeof(ota_header->u.s.ver_software));
    str[sizeof(ota_header->u.s.ver_software)] = '\0';
    printf("[OTA] [HEADER] ver_software is %s\r\n", str);

    memcpy(str, ota_header->u.s.sha256, sizeof(ota_header->u.s.sha256));
    str[sizeof(ota_header->u.s.sha256)] = '\0';
    printf("[OTA] [HEADER] sha256 is ");
    for (i = 0; i < sizeof(ota_header->u.s.sha256); i++) {
        printf("%02X", str[i]);
    }
    printf("\r\n");

    return 0;
}

#define PING_USAGE                    \
    "wifi_ota_test [ip] [port] [url]\r\n"   \
    "\t ip: dest ip or server ip\r\n" \
    "\t port: dest server listen port\r\n" \
    "\t url: dest server url xxx.bin.ota file.\r\n"

void my_dump_partition(pt_table_stuff_config *pt_stuff)
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

static void cb_httpc_result(void *arg, httpc_result_t httpc_result, uint32_t rx_content_len, uint32_t srv_res, err_t err)
{
    httpc_state_t **req = (httpc_state_t **)arg;
    printf("download done!\r\n");
    httpc_get_done_flag  = 1;
    *req = NULL;

    if (bin_size == total_cnt) {
        total_cnt = 0;
#if SHA256_SEL
        mbedtls_sha256_finish_ret(&ctx_sha256, sha256_result);
#else
        utils_sha256_finish(&ctx_sha256, sha256_result);
#endif
        printf("\r\nCalculated SHA256 Checksum:");
        for (int i = 0; i < sizeof(sha256_result); i++) {
            printf("%02X", sha256_result[i]);
        }
        printf("\r\nHeader SET SHA256 Checksum:");
        for (int i = 0; i < sizeof(sha256_img); i++) {
            printf("%02X", sha256_img[i]);
        }
        printf("\r\n");
        if (memcmp(sha256_img, sha256_result, sizeof(sha256_img))) {
            /*Error found*/
            printf("[OTA] [TCP] SHA256 NOT Correct\r\n");

            // uint32_t *p = (uint32_t *)(FLASH_XIP_BASE + ota_addr - 0x11000);
            // printf("0x%x ", p);
            // for (int j = 0; j < 32; j ++) {
            //     printf("0x%x ", *(p + j));
            // }
            // printf("\r\n");
            return;
        }

        printf("[OTA] [TCP] prepare OTA partition info\r\n");
        pt_fw_entry.len = bin_size;
        printf("[OTA] [TCP] Update PARTITION, partition len is %lu\r\n", pt_fw_entry.len);
        pt_fw_entry.active_index = !(pt_fw_entry.active_index & 0x01);
        pt_fw_entry.age++;
        status = pt_table_update_entry(!active_id, &pt_table_stuff[active_id], &pt_fw_entry);
        if (status != 0) {
            printf("pt table update fail! %d\r\n", status);
        }
        printf("[OTA] [TCP] Rebooting\r\n");
        vTaskDelay(10);
        GLB_SW_POR_Reset();
        vTaskDelay(10);
    }
}

static err_t cb_httpc_headers_done_fn(httpc_state_t *connection, void *arg, struct pbuf *hdr, u16_t hdr_len, u32_t content_len)
{
    printf("http hdr_len is %u, content_len is %u\r\n", hdr_len, content_len);
    return 0;
}

static err_t cb_altcp_recv_fn(void *arg, struct altcp_pcb *conn, struct pbuf *p, err_t err)
{
    struct pbuf *current_pbuf = p;

    if (p == NULL) {
        printf("No pbuf to process.\n");
        return 1;
    }

    /* process pbuf list */
    while (current_pbuf != NULL) {
        /* recv all data to recv_buffer */
        memcpy(recv_buffer + buffer_offset, current_pbuf->payload, current_pbuf->len);
        buffer_offset += current_pbuf->len;
        current_pbuf = current_pbuf->next;

        /* first recv data find ota header */
        if (!ota_header_found) {
            /* recv data */
            if (buffer_offset >= OTA_PROGRAM_SIZE) {
                ota_header = (ota_header_t *)recv_buffer;
                if (_check_ota_header(ota_header, &bin_size, &use_xz)) {
                    /*ota header is NOT OK*/
                    printf("ota header is NOT OK\r\n");
                    break;
                }
                /* found ota header */
                ota_header_found = 1;
                memcpy(sha256_img, ota_header->u.s.sha256, sizeof(sha256_img));
                buffer_offset = buffer_offset - OTA_HEADER_SIZE;
                if (buffer_offset) {
                    memmove(recv_buffer, recv_buffer + OTA_HEADER_SIZE, buffer_offset);
                }
                total_cnt = buffer_offset;
            }
        } else {
            total_cnt += buffer_offset;
        }
    }

    /* if total cnt > bin_size is error */
    if (total_cnt > bin_size) {
        printf("[OTA] [TCP] Server has bug?\r\n");
        // return;
        while (1) {
            /*empty*/
        }
    }

    printf("Will Write %u to %08X from %p left %lu.\r\n", buffer_offset, ota_addr + write_flash_offset, recv_buffer, bin_size - total_cnt);

#if SHA256_SEL
    bflb_l1c_dcache_clean_range(recv_buffer, buffer_offset);
    status = mbedtls_sha256_update_ret(&ctx_sha256, recv_buffer, buffer_offset);
    if (status != 0) {
        printf("sha256 update fail! %d\r\n", status);
    }
#else
    utils_sha256_update(&ctx_sha256, recv_buffer, buffer_offset);
#endif
    // printf("sha256 update! %d\r\n", status);
    /* program data to flash */
    status = bflb_flash_write((ota_addr + write_flash_offset), recv_buffer, buffer_offset);
    if (status != 0) {
        printf("flash write fail! %d\r\n", status);
    }

    write_flash_offset += buffer_offset;
    buffer_offset = 0;

    altcp_recved(conn, p->tot_len);
    pbuf_free(p);

    return ERR_OK;
}

static void wifi_ota_use_lwip_httpc(int argc, char **argv)
{
    static httpc_connection_t settings;
    static httpc_state_t *req;

    char *port;

    printf("ota test task start ...\r\n");

    /* Set flash operation function, read via xip */
    pt_table_set_flash_operation(bflb_flash_erase, bflb_flash_write, bflb_flash_read);

    active_id = pt_table_get_active_partition_from_ram(pt_table_stuff);
    if (PT_TABLE_ID_INVALID == active_id) {
        printf("No valid PT\r\n");
        return;
    }
    printf("Active PT:%d,Age %d\r\n", active_id, pt_table_stuff[active_id].pt_table.age);

    my_dump_partition(&pt_table_stuff[active_id]);

    recv_buffer = pvPortMalloc(OTA_PROGRAM_SIZE);
    printf("Starting OTA test. OTA bin addr is %p\r\n", recv_buffer);
    printf("[OTA] [TEST] active_id is %u\r\n", active_id);

    if (pt_table_get_active_entries_by_name(&pt_table_stuff[active_id], (uint8_t *)OTA_PARTITION_NAME_TYPE_FW, &pt_fw_entry)) {
        printf("PtTable get active entry fail!\r\n");
        vPortFree(recv_buffer);
        return;
    }

    ota_addr = pt_fw_entry.start_address[!pt_fw_entry.active_index];
    bin_size = pt_fw_entry.max_len[!pt_fw_entry.active_index];
    // part_size = pt_fw_entry.max_len[!pt_fw_entry.active_index];

    printf("Starting OTA test. OTA size is %lu\r\n", bin_size);

    printf("[OTA] [TEST] activeIndex is %u, use OTA address=%08x\r\n", pt_fw_entry.active_index, (unsigned int)ota_addr);

    printf("[OTA] [TEST] Erase flash with size %lu...\r\n", bin_size);

    status = bflb_flash_erase(ota_addr, bin_size); /* erase flash */
    if (status != 0) {
        printf("flash erase fail! %d\r\n", status);
        return;
    }
    printf("Done\r\n");

    if (argc < 4) {
        printf("%s", PING_USAGE);
        return;
    }

    /* get port number (argv[2] if present) */
    if (argc > 2) {
        port = argv[2];
    } else {
        port = "3333";
    }
    printf("uar:%s!\r\n", argv[3]);
    if (req) {
        printf("req err!\r\n");
    }

    memset(&settings, 0, sizeof(settings));
    settings.use_proxy = 0;
    settings.result_fn = cb_httpc_result;
    settings.headers_done_fn = cb_httpc_headers_done_fn;

    printf("Server ip Address : %s:%s\r\n", argv[1], port);

    /* init sha */
#if SHA256_SEL
    mbedtls_sha256_init(&ctx_sha256);
    mbedtls_sha256_starts_ret(&ctx_sha256, 0);
#else
    utils_sha256_init(&ctx_sha256);
    utils_sha256_starts(&ctx_sha256);
#endif
    memset(sha256_result, 0, sizeof(sha256_result));

    /* get ota file and program to flash */
    httpc_get_file_dns(
        argv[1],
        atoi(port),
        argv[3],
        &settings,
        cb_altcp_recv_fn,
        &req,
        &req
    );

}

#ifdef CONFIG_SHELL
#include <shell.h>

int cmd_wifi_ota_test(int argc, char **argv)
{
    /* default cmd like this: <wifi_ota_test <host_ip> <port> <url> \r\n> */
    /* default cmd like this: <wifi_ota_test 192.168.123.120 3365 /wifi_ota_bl616.bin.ota\r\n> */
    wifi_ota_use_lwip_httpc(argc, argv);

    while(!httpc_get_done_flag);

    printf("ota done!!!\r\nreboot now!\r\n");
    GLB_SW_POR_Reset();
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_wifi_ota_test, wifi_ota_test, wifi ota test);
#endif
