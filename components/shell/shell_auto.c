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
