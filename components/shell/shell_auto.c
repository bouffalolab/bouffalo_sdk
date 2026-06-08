#include "shell.h"
#include "bflb_flash.h"

#if IS_ENABLED(CONFIG_FREERTOS)
#include "FreeRTOS.h"
#include "task.h"
#endif

#ifndef CONFIG_SHELL_AUTO_LIST_SIZE
#define CONFIG_SHELL_AUTO_LIST_SIZE 128
#endif

#ifndef CONFIG_SHELL_AUTO_EXEC_TIMEOUT_MS
#define CONFIG_SHELL_AUTO_EXEC_TIMEOUT_MS 30000
#endif

#define SHELL_AUTO_DESC_NAME "AUTOLIST"
#define SHELL_AUTO_FW_HEADER_OFFSET 0x1000

typedef struct {
    char name[8];
    uint32_t start_addr;
    uint32_t end_addr;
} __attribute__((packed)) multi_bin_desc_t;

extern const uint8_t __multi_bins__[];

static int shell_auto_find_desc(const multi_bin_desc_t **out_desc)
{
    const multi_bin_desc_t *desc;

    if (out_desc == NULL) {
        return -1;
    }

    for (int i = 0; i < 16; i++) {
        desc = (const multi_bin_desc_t *)(__multi_bins__ + i * sizeof(multi_bin_desc_t));
        if (desc->start_addr == 0xFFFFFFFF) {
            break;
        }
        if (memcmp(desc->name, SHELL_AUTO_DESC_NAME, sizeof(desc->name)) == 0) {
            if (desc->end_addr > desc->start_addr) {
                *out_desc = desc;
                return 0;
            }
            break;
        }
    }

    *out_desc = NULL;
    return -1;
}

static uint32_t shell_auto_capacity(const multi_bin_desc_t *desc)
{
    uint32_t size = desc->end_addr - desc->start_addr;

    if (size > CONFIG_SHELL_AUTO_LIST_SIZE) {
        size = CONFIG_SHELL_AUTO_LIST_SIZE;
    }

    return size;
}

static uint32_t shell_auto_flash_addr(uint32_t bin_offset)
{
    uint32_t image_offset = bflb_flash_get_image_offset();

    if (image_offset >= SHELL_AUTO_FW_HEADER_OFFSET) {
        return bin_offset + image_offset - SHELL_AUTO_FW_HEADER_OFFSET;
    }

    return bin_offset;
}

static int shell_auto_read(char *buf, uint32_t buf_size)
{
    const multi_bin_desc_t *desc;
    uint32_t size;
    int ret;

    if (buf == NULL || buf_size == 0) {
        return -1;
    }

    memset(buf, 0, buf_size);

    if (shell_auto_find_desc(&desc) != 0) {
        return -1;
    }

    size = shell_auto_capacity(desc);
    if (size >= buf_size) {
        size = buf_size - 1;
    }

    ret = bflb_flash_read(shell_auto_flash_addr(desc->start_addr), (uint8_t *)buf, size);
    if (ret != 0) {
        return ret;
    }

    buf[size] = '\0';
    for (uint32_t i = 0; i < size; i++) {
        if ((uint8_t)buf[i] == 0xFF) {
            buf[i] = '\0';
            break;
        }
    }

    return 0;
}

static int shell_auto_write(const char *text)
{
    const multi_bin_desc_t *desc;
    uint8_t data[CONFIG_SHELL_AUTO_LIST_SIZE];
    uint32_t capacity;
    uint32_t len = 0;
    int ret;

    if (shell_auto_find_desc(&desc) != 0) {
        printf("AUTOLIST not found.\r\n");
        return -1;
    }

    capacity = shell_auto_capacity(desc);
    if (capacity == 0 || capacity > sizeof(data)) {
        printf("AUTOLIST invalid size.\r\n");
        return -1;
    }

    memset(data, 0, sizeof(data));
    if (text) {
        len = strlen(text);
        if (len >= capacity) {
            printf("auto list too long, max %u bytes.\r\n", capacity - 1);
            return -1;
        }
        memcpy(data, text, len);
    }

    ret = bflb_flash_erase(shell_auto_flash_addr(desc->start_addr), capacity);
    if (ret != 0) {
        printf("erase AUTO_LIST failed: %d\r\n", ret);
        return ret;
    }

    ret = bflb_flash_write(shell_auto_flash_addr(desc->start_addr), data, capacity);
    if (ret != 0) {
        printf("write AUTO_LIST failed: %d\r\n", ret);
        return ret;
    }

    return 0;
}

static void shell_auto_decode_escapes(char *dst, uint32_t dst_size, const char *src)
{
    uint32_t pos = 0;

    if (dst_size == 0) {
        return;
    }

    while (*src && pos < dst_size - 1) {
        if (src[0] == '\\' && src[1] == 'n') {
            dst[pos++] = '\n';
            src += 2;
        } else if (src[0] == '\\' && src[1] == 'r') {
            dst[pos++] = '\r';
            src += 2;
        } else {
            dst[pos++] = *src++;
        }
    }
    dst[pos] = '\0';
}

static void shell_auto_print_list(const char *list)
{
    char line[SHELL_CMD_SIZE];
    uint32_t line_pos = 0;
    uint32_t line_no = 1;

    printf("shell_auto_list:\r\n");
    for (uint32_t i = 0;; i++) {
        char ch = list[i];

        if (ch == '\r' || ch == '\n' || ch == '\0') {
            if (line_pos > 0) {
                line[line_pos] = '\0';
                printf("%u: %s\r\n", line_no++, line);
                line_pos = 0;
            }
            if (ch == '\0') {
                break;
            }
            continue;
        }

        if (line_pos < sizeof(line) - 1) {
            line[line_pos++] = ch;
        }
    }

    if (line_no == 1) {
        printf("(empty)\r\n");
    }
}

int shell_auto_run(void)
{
    char list[CONFIG_SHELL_AUTO_LIST_SIZE + 1];
    char cmd[SHELL_CMD_SIZE];
    uint32_t cmd_pos = 0;
    int ret;

    ret = shell_auto_read(list, sizeof(list));
    if (ret != 0 || list[0] == '\0') {
        return ret;
    }

    for (uint32_t i = 0;; i++) {
        char ch = list[i];

        if (ch == '\r' || ch == '\n' || ch == '\0') {
            if (cmd_pos > 0) {
                cmd[cmd_pos] = '\0';
                printf("[auto] %s\r\n", cmd);
                ret = shell_exec(cmd, cmd_pos);
                if (ret == 0) {
                    shell_wait_exec_done(CONFIG_SHELL_AUTO_EXEC_TIMEOUT_MS);
                }
                memset(cmd, 0, sizeof(cmd));
                cmd_pos = 0;
            }
            if (ch == '\0') {
                break;
            }
            continue;
        }

        if (cmd_pos < sizeof(cmd) - 1) {
            cmd[cmd_pos++] = ch;
        }
    }

    return 0;
}

#if IS_ENABLED(CONFIG_FREERTOS)
static void shell_auto_task(void *arg)
{
    (void)arg;
    vTaskDelay(pdMS_TO_TICKS(50));
    shell_auto_run();
    vTaskDelete(NULL);
}

void shell_auto_start(void)
{
    xTaskCreate(shell_auto_task, "shell_auto", 1024, NULL, SHELL_THREAD_PRIO, NULL);
}
#else
void shell_auto_start(void)
{
    shell_auto_run();
}
#endif

static int shell_auto_cmd(int argc, char **argv)
{
    char list[CONFIG_SHELL_AUTO_LIST_SIZE + 1];
    char joined[CONFIG_SHELL_AUTO_LIST_SIZE];
    char decoded[CONFIG_SHELL_AUTO_LIST_SIZE];
    uint32_t pos = 0;
    int ret;

    if (argc == 1 || strcmp(argv[1], "list") == 0) {
        ret = shell_auto_read(list, sizeof(list));
        if (ret != 0) {
            printf("read AUTOLIST failed: %d\r\n", ret);
            return ret;
        }
        shell_auto_print_list(list);
        return 0;
    }

    if (strcmp(argv[1], "run") == 0) {
        return shell_auto_run();
    }

    if (strcmp(argv[1], "clear") == 0) {
        ret = shell_auto_write("");
        if (ret == 0) {
            printf("AUTOLIST cleared.\r\n");
        }
        return ret;
    }

    if (strcmp(argv[1], "save") == 0) {
        if (argc < 3) {
            printf("usage: shell_auto save \"cmd1\\ncmd2\"\r\n");
            return -1;
        }

        memset(joined, 0, sizeof(joined));
        for (int i = 2; i < argc; i++) {
            uint32_t arg_len = strlen(argv[i]);
            if (pos + arg_len + (i > 2 ? 1 : 0) >= sizeof(joined)) {
                printf("auto list too long, max %u bytes.\r\n", CONFIG_SHELL_AUTO_LIST_SIZE - 1);
                return -1;
            }
            if (i > 2) {
                joined[pos++] = ' ';
            }
            memcpy(&joined[pos], argv[i], arg_len);
            pos += arg_len;
        }

        shell_auto_decode_escapes(decoded, sizeof(decoded), joined);
        ret = shell_auto_write(decoded);
        if (ret == 0) {
            printf("AUTOLIST saved.\r\n");
        }
        return ret;
    }

    printf("usage: shell_auto [list|run|clear|save \"cmd1\\ncmd2\"]\r\n");
    return -1;
}
SHELL_CMD_EXPORT_ALIAS(shell_auto_cmd, shell_auto, shell auto list);
