
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <lwip/api.h>
#include <lwip/arch.h>
#include <lwip/opt.h>
#include <lwip/inet.h>
#include <lwip/errno.h>
#include <netdb.h>

#include "bflb_mtimer.h"
#include "bflb_flash.h"
#include "bflb_l1c.h"

#include "shell.h"
#include "utils_getopt.h"
#include "partition.h"
#include "ring_buffer.h"

#define SW_SHA256 0
#define HW_SHA256 1
#define SHA256_EN HW_SHA256

#if SHA256_EN
#include "bflb_sec_sha.h"
ATTR_NOCACHE_NOINIT_RAM_SECTION struct bflb_sha256_ctx_s ctx_sha256;
#else
#include "utils_sha256.h"
#endif

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

#define OTA_PROGRAM_SIZE              (512)
#define OTA_HEADER_SIZE               (sizeof(ota_header_t))
#define OTA_PARTITION_NAME_TYPE_FW    "FW"
#define OTA_PARTITION_NAME_TYPE_PSM   "PSM"
#define OTA_PARTITION_NAME_TYPE_ROMFS "media"

// clang-format off
static const uint8_t get_buf[] = "GET /wifi_ota_bl616.bin.ota HTTP/1.1 \r\nHost: 192.168.123.120\r\n\r\n";
// clang-format on

// extern Ring_Buffer_Type shell_rb;
// extern void shell_release_sem(void);

shell_sig_func_ptr abort_exec;
uint64_t total_cnt = 0;
int sock_client = -1;

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

static void test_close(int sig)
{
    if (sock_client) {
        closesocket(sock_client);
    }
    abort_exec(sig);
    if (total_cnt > 0) {
        printf("Total send data=%lld\r\n", total_cnt);
    }
}

#define PING_USAGE                    \
    "wifi_ota_test [ip] [port]\r\n"   \
    "\t ip: dest ip or server ip\r\n" \
    "\t port: dest server listen port\r\n"

// extern void my_dump_partition(pt_table_stuff_config *pt_stuff);
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

static void wifi_test_ota_test_init(int argc, char **argv)
{
    abort_exec = shell_signal(SHELL_SIGINT, test_close);
    printf("ota test task start ...\r\n");

    pt_table_stuff_config pt_table_stuff[2];
    pt_table_id_type active_id;

    /* Set flash operation function, read via xip */
    pt_table_set_flash_operation(bflb_flash_erase, bflb_flash_write, bflb_flash_read);

    active_id = pt_table_get_active_partition_need_lock(pt_table_stuff);
    if (PT_TABLE_ID_INVALID == active_id) {
        printf("No valid PT\r\n");
        return;
    }
    printf("Active PT:%d,Age %d\r\n", active_id, pt_table_stuff[active_id].pt_table.age);

    my_dump_partition(&pt_table_stuff[active_id]);

    char *port;
    struct sockaddr_in remote_addr;
    struct hostent *hostinfo;

    if (argc < 3) {
        printf("%s", PING_USAGE);
        return;
    }

    /* get address (argv[1] if present) */
    hostinfo = gethostbyname(argv[1]);
    if (!hostinfo) {
        printf("gethostbyname Failed\r\n");
        return;
    }

    /* get port number (argv[2] if present) */
    if (argc > 2) {
        port = argv[2];
    } else {
        port = "3333";
    }

    /* Create a socket */
    if ((sock_client = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("ota test create socket error\r\n");
        return;
    }

    /* init server addr/port */
    memset(&remote_addr, 0, sizeof(0));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(atoi(port));
    remote_addr.sin_addr = *((struct in_addr *)hostinfo->h_addr);
    // memset(&(remote_addr.sin_zero), 0, sizeof(remote_addr.sin_zero));
    char *ip = inet_ntoa(remote_addr.sin_addr.s_addr);

    int ret = 0;
    uint32_t i = 0;
    int total_cnt = 0;
    int ota_header_found = 0, use_xz = 0;
    ota_header_t *ota_header;
    uint8_t *recv_buffer;

    recv_buffer = pvPortMalloc(OTA_HEADER_SIZE);

    int status = 0;
    unsigned int buffer_offset = 0, flash_offset = 0, ota_addr;
    uint32_t bin_size; // part_size;
    pt_table_entry_config pt_fw_entry;

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

    printf("Server ip Address : %s:%s\r\n", ip, port);
    if (connect(sock_client, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) != 0) {
        printf("ota test connect server falied!\r\n");
        closesocket(sock_client);
        vPortFree(recv_buffer);
        return;
    }

    /* init sha */
    uint8_t sha256_img[32];
    uint8_t sha256_result[32];
#if SHA256_EN
    struct bflb_device_s *sha;
    sha = bflb_device_get_by_name("sha");
    bflb_group0_request_sha_access(sha);
    bflb_sha_init(sha, SHA_MODE_SHA256);
    bflb_sha256_start(sha, &ctx_sha256);
#else
    sha256_context ctx_sha256;
    utils_sha256_init(&ctx_sha256);
    utils_sha256_starts(&ctx_sha256);
#endif
    memset(sha256_result, 0, sizeof(sha256_result));

    /* http ota */
    write(sock_client, get_buf, sizeof(get_buf));

    /* first 207 http ack*/
    read(sock_client, recv_buffer, 207);
    /* first 512 bytes of TCP stream is OTA header; now not support */
    printf("ota test connect server success!\r\n");
    printf("Press CTRL-C to exit.\r\n");
    while (1) {
        // printf("read!\r\n");
        ret = read(sock_client, recv_buffer + buffer_offset, OTA_PROGRAM_SIZE - buffer_offset);
        if (ret < 0) {
            printf("ret = %d, err = %d\r\n", ret, errno);
            break;
        } else {
            total_cnt += ret;
            if (ret == 0) {
                printf("[OTA] [TEST] seems ota file ends unexpectedly, already transfer %u\r\n", total_cnt);
                break;
            }
            printf("total = %d, ret = %d\n\r", total_cnt, ret);
            buffer_offset += ret;

            if (ota_header_found == 0) {
                if (buffer_offset < OTA_PROGRAM_SIZE) {
                    continue;
                } else if (buffer_offset > OTA_PROGRAM_SIZE) {
                    printf("[OTA] [TCP] Assert for unexpected error %d\r\n", buffer_offset);
                    // return;
                    while (1) {
                        /*empty*/
                    }
                }
                /*ota_header is got, we assume alignment of recv_buffer is OK*/
                ota_header = (ota_header_t *)recv_buffer;
                if (_check_ota_header(ota_header, &bin_size, &use_xz)) {
                    /*ota header is NOT OK*/
                    break;
                }
                ota_header_found = 1;
                memcpy(sha256_img, ota_header->u.s.sha256, sizeof(sha256_img));
                /*we think OTA_PROGRAM_SIZE is the same OTA_HEADER_SIZE, and refix total to exclude OTA_PROGRAM_SIZE*/
                total_cnt -= OTA_HEADER_SIZE;
                buffer_offset = 0;
                printf("[OTA] [TCP] Update bin_size to %lu, file status %s\r\n", bin_size, use_xz ? "XZ" : "RAW");
                continue;
            }

            if (bin_size != total_cnt) {
                if (buffer_offset < OTA_PROGRAM_SIZE) {
                    continue;
                } else if (buffer_offset > OTA_PROGRAM_SIZE) {
                    printf("[OTA] [TCP] Assert for unexpected error %d\r\n", buffer_offset);
                    // return;
                    while (1) {
                        /*empty*/
                    }
                }
            } else if (total_cnt > bin_size) {
                printf("[OTA] [TCP] Server has bug?\r\n");
                // return;
                while (1) {
                    /*empty*/
                }
            }
            printf("Will Write %u to %08X from %p left %lu.\r\n", buffer_offset, ota_addr + flash_offset, recv_buffer, bin_size - total_cnt);
#if SHA256_EN
            bflb_l1c_dcache_clean_range(recv_buffer, buffer_offset);
            status = bflb_sha256_update(sha, &ctx_sha256, recv_buffer, buffer_offset);
            if (status != 0) {
                printf("sha256 update fail! %d\r\n", status);
            }
#else
            utils_sha256_update(&ctx_sha256, recv_buffer, buffer_offset);
#endif
            // printf("sha256 update! %d\r\n", status);
            /* program flash */
            status = bflb_flash_write((ota_addr + flash_offset), recv_buffer, buffer_offset);

            if (status != 0) {
                printf("flash write fail! %d\r\n", status);
            }

            flash_offset += buffer_offset;
            buffer_offset = 0;

            if (bin_size == total_cnt) {
                closesocket(sock_client);
#if SHA256_EN
                bflb_sha256_finish(sha, &ctx_sha256, sha256_result);
#else
                utils_sha256_finish(&ctx_sha256, sha256_result);
#endif
                printf("\r\nCalculated SHA256 Checksum:");
                for (i = 0; i < sizeof(sha256_result); i++) {
                    printf("%02X", sha256_result[i]);
                }
                printf("\r\nHeader SET SHA256 Checksum:");
                for (i = 0; i < sizeof(sha256_img); i++) {
                    printf("%02X", sha256_img[i]);
                }
                printf("\r\n");
                if (memcmp(sha256_img, sha256_result, sizeof(sha256_img))) {
                    /*Error found*/
                    printf("[OTA] [TCP] SHA256 NOT Correct\r\n");
                    break;
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
                // Ring_Buffer_Write(&shell_rb, send_buf, sizeof(send_buf) - 1);
                // shell_release_sem();
                vTaskDelay(10);
            }
        }
    }
    if (sock_client)
        closesocket(sock_client);
    vPortFree(recv_buffer);

    return;
}

#ifdef CONFIG_SHELL
#include <shell.h>

int cmd_wifi_ota_test(int argc, char **argv)
{
    wifi_test_ota_test_init(argc, argv);
    printf("ota failed!!!\r\nreboot now!\r\n");
    GLB_SW_POR_Reset();
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_wifi_ota_test, wifi_ota_test, wifi ota test);
#endif
