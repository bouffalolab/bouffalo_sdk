#include "bflb_mtimer.h"
#include "board.h"
#include "bflb_uart.h"
#define DBG_TAG "MAIN"
#include "log.h"
#include "shell.h"
#include "bl618dg_glb.h"
#include "bl618dg_pds.h"
#include "bl618dg_pm.h"
#include "bl618dg_hbn.h"
#include "bl618dg_aon.h"
#include "bl_lp.h"

static struct bflb_device_s *uartx;

int main(void)
{
    int ch;

    board_init();
    HBN_Trim_RC32K();
    HBN_32K_Sel(HBN_32K_RC);

    uartx = bflb_device_get_by_name("uart0");
    LOG_I("pds_lp_wakeup test start\r\n");

    *((volatile uint32_t *)0x20000000) |= (1 << 9);

    shell_init();
    while (1) {
        if ((ch = bflb_uart_getchar(uartx)) != -1) {
            shell_handler(ch);
        }
    }
}

int dump_regs(int argc, char **argv)
{
    uint32_t addr = 0x20085000;
    uint32_t count = 32;
    volatile uint32_t *ptr;

    if (argc >= 2) {
        addr = (uint32_t)strtoul(argv[1], NULL, 0);
    }
    if (argc >= 3) {
        count = (uint32_t)strtoul(argv[2], NULL, 0);
    }

    ptr = (volatile uint32_t *)addr;
    printf("Dump registers from 0x%08X, count=%u\r\n", addr, count);
    for (uint32_t i = 0; i < count; i++) {
        if (i % 4 == 0) {
            printf("0x%08X: ", (uint32_t)(addr + i * 4));
        }
        printf("0x%08X ", ptr[i]);
        if (i % 4 == 3) {
            printf("\r\n");
        }
    }
    if (count % 4 != 0) {
        printf("\r\n");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(dump_regs, dump_regs, dump regs [addr] [count]);

int write_reg(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage: write_reg <addr> <value>\r\n");
        return -1;
    }

    uint32_t addr = (uint32_t)strtoul(argv[1], NULL, 0);
    uint32_t value = (uint32_t)strtoul(argv[2], NULL, 0);

    *(volatile uint32_t *)addr = value;
    printf("Write 0x%08X = 0x%08X\r\n", addr, value);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(write_reg, write_reg, write reg <addr> <value>);

int enter_pds(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: enter_pds <level> [time_us]\r\n"
               "  level: 1-15 (PDS level)\r\n"
               "  time_us: RTC wakeup time in us, 0 = no RTC wakeup\r\n");
        return -1;
    }

    int level = atoi(argv[1]);
    uint32_t time_us = 0;
    if (argc >= 3) {
        time_us = (uint32_t)strtoul(argv[2], NULL, 0);
    }

    PDS_Mask_All_Wakeup_Src();
    PDS_Set_Wakeup_Src_IntMask(PDS_WAKEUP_BY_SW_CONFIG, UNMASK);
    HBN_Pin_WakeUp_Mask(0xF);

    /* enable RC32M to mini sys */
    GLB_RC32M_Force_On(ENABLE);

    printf("Entering PDS%d, time_us=%u\r\n", level, time_us);
    arch_delay_us(200);
    pm_pds_irq_register();
    pm_pds_mode_enter(level, BL_US_TO_PDS_CNT(time_us));

    // bl_lp_pds_enter_with_restore(level, time_us);

    printf("PDS%d wakeup\r\n", level);
    shell_init();

    GLB_Halt_Mini_Sys();
    GLB_Release_Mini_Sys();

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(enter_pds, enter_pds, enter pds <level> [time_us]);
