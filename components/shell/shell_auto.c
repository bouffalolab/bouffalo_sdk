#include "shell.h"
#include "multi_bins.h"

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

static int shell_auto_find_desc(const uint8_t **start_addr, const uint8_t **end_addr)
{
    if (start_addr == NULL || end_addr == NULL) {
        return -1;
    }

    *start_addr = multi_bins_get_start("AUTOLIST");
    *end_addr = multi_bins_get_end("AUTOLIST");
    if (*start_addr != NULL && *end_addr > *start_addr) {
        return 0;
    }

    *start_addr = NULL;
    *end_addr = NULL;
    return -1;
}

static int shell_auto_read(char *buf, uint32_t buf_size)
{
    const uint8_t *start_addr;
    const uint8_t *end_addr;
    uint32_t size;

    if (buf == NULL || buf_size == 0) {
        return -1;
    }

    memset(buf, 0, buf_size);

    if (shell_auto_find_desc(&start_addr, &end_addr) != 0) {
        return -1;
    }

    size = (uint32_t)(end_addr - start_addr);
    if (size > CONFIG_SHELL_AUTO_LIST_SIZE) {
        size = CONFIG_SHELL_AUTO_LIST_SIZE;
    }
    if (size >= buf_size) {
        size = buf_size - 1;
    }

    memcpy(buf, start_addr, size);

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
    char cmd[CONFIG_SHELL_CMD_SIZE];
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
