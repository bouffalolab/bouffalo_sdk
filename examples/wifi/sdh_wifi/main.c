#include "bflb_mtimer.h"
#include "board.h"

#define DBG_TAG "MAIN"
#include "log.h"
extern int  skw_sdio_io_init(void);


#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include "sdh_sdio.h"

#include <board.h>
#include <shell.h>

#define INIT_STACK_SIZE    (8096)
#define INIT_TASK_PRIORITY (16)
#define TEST_SIZE          (512)
static struct bflb_device_s *uart0;
extern void shell_init_with_task(struct bflb_device_s *shell);

static struct sdh_sdio_s g_sdio;
uint8_t __attribute__((aligned(64))) w_data[TEST_SIZE] = { 0 };
uint8_t __attribute__((aligned(64))) r_data[TEST_SIZE] = { 0 };

void app_init_entry(void *param)
{
#if 1
    int ret;
    uint8_t data;
    uint32_t actual_clock = 0;
    uint32_t test_reg = 0xf;

    LOG_I("sdh_sdio_basic_test\r\n");

    ret = sdh_sdio_scan_done(&g_sdio);
    if (ret < 0) {
        LOG_E("sdh_sdio_init failed, ret=%d\r\n", ret);
        while (1) {
            bflb_mtimer_delay_ms(1000);
        }
    }

    ret = sdh_sdio_set_clock(&g_sdio, 2*1000*1000, &actual_clock);
    LOG_I("Set SDIO clock ret=%d actual=%lu Hz\r\n", ret, (unsigned long)actual_clock);

    data = sdh_sdio_readb(&g_sdio.functions[0], SDIO_CCCR_IO_ENABLE, &ret);
    LOG_I("CMD52 IO_ENABLE ret=%d val=0x%02X\r\n", ret, data);

    data = sdh_sdio_readb(&g_sdio.functions[0], SDIO_CCCR_IO_READY, &ret);
    LOG_I("CMD52 IO_READY  ret=%d val=0x%02X\r\n", ret, data);

    if (g_sdio.card_flag & SDH_SDIO_FLAG_SUP_4BIT) {
        ret = sdh_sdio_set_bus_width(&g_sdio, SDH_SDIO_BUS_WIDTH_4);
        LOG_I("Set 4-bit bus width ret=%d\r\n", ret);
    } else {
        LOG_W("Card does not report 4-bit support\r\n");
    }

    if (g_sdio.num_funcs >= 1U) {
        ret = sdh_sdio_func_enable(&g_sdio.functions[1]);
        LOG_I("Enable function 1 ret=%d\r\n", ret);

        ret = sdh_sdio_func_set_block_size(&g_sdio.functions[0], SDIO_DEFAULT_BLOCK_SIZE);
        ret = sdh_sdio_func_set_block_size(&g_sdio.functions[1], SDIO_DEFAULT_BLOCK_SIZE);
        LOG_I("Set function 1 block size ret=%d\r\n", ret);

        //memset(w_data, 0x5A, sizeof(w_data));
        sdh_sdio_memcpy_toio(&g_sdio.functions[1], 0x0F, w_data, sizeof(w_data));
        sdh_sdio_memcpy_fromio(&g_sdio.functions[1], r_data, 0x0F, sizeof(r_data));

        //if (memcmp(r_data, w_data, sizeof(r_data)) != 0) {
        //    LOG_E("sdio w/r test fail\r\n");
        //} else {
        //    LOG_I("sdio w/r test successsful\r\n");
        //}

        uint32_t addr = 0x40000000;

        // 1. 设置地址窗口：reg 0x15C~0x15F
        for (int i = 0; i < 4; i++) {
            uint8_t val = (addr >> (i * 8)) & 0xFF;
            sdh_sdio_writeb(&g_sdio.functions[0], val, 0x15C + i, &ret);
        }

        // 2. 通过固定 reg=0x0F 读 16 字节（byte mode）
        sdh_sdio_memcpy_fromio(&g_sdio.functions[1], r_data, 0x0F, 16);

        printf("chipid:");
        for (int i = 0; i < 16; i++) {
            printf("%c", r_data[i], r_data[i]);
        }
        printf("\r\n");

    } else {
        LOG_W("No SDIO IO function reported by card\r\n");
    }
#endif

    vTaskDelete(NULL);
}

int main(void)
{
    board_init();

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    vTaskStartScheduler();

    while (1) {
    }
}

static void cmd_scan_card(int argc, char **argv)
{
    xTaskCreate(app_init_entry, (char *)"init", INIT_STACK_SIZE, NULL, INIT_TASK_PRIORITY, NULL);
}

SHELL_CMD_EXPORT_ALIAS(cmd_scan_card, scan_card, scan sdh card);

