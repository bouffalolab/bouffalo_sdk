#include "bflb_mtimer.h"
#include "bflb_clock.h"
#include "board.h"
#include "bflb_uart.h"
#define DBG_TAG "MAIN"
#include "log.h"
#include "shell.h"
#include "bl618dg_glb.h"
#include "bl618dg_pds.h"
#include "bl618dg_hbn.h"

static struct bflb_device_s *uartx;

static void dump_uart_clock_info(void)
{
    uint32_t hbn_glb = BL_RD_REG(HBN_BASE, HBN_GLB);
    uint8_t sel2 = BL_GET_REG_BITS_VAL(hbn_glb, HBN_UART_CLK_SEL2);
    uint8_t sel = BL_GET_REG_BITS_VAL(hbn_glb, HBN_UART_CLK_SEL);
    const char *src_name;

    if (sel2) {
        src_name = "XCLK";
    } else if (sel == HBN_UART_CLK_WIFIPLL_160M) {
        src_name = "WIFIPLL_160M";
    } else {
        src_name = "MCU_BCLK";
    }

    LOG_I("UART clock source: %s, frequency: %u Hz\r\n",
          src_name, bflb_clk_get_peripheral_clock(BFLB_DEVICE_TYPE_UART, 3));
}

static void cmd_triger_sw_wakeup_pds(int argc, char **argv)
{
    LOG_I("LP waking up AP from PDS...\r\n");
    PDS_Triger_SW_Wakeup_PDS();
    arch_delay_ms(1);
    PDS_Clear_SW_Wakeup_PDS();
    LOG_I("LP wakeup AP done\r\n");
}
SHELL_CMD_EXPORT_ALIAS(cmd_triger_sw_wakeup_pds, Triger_PDS_Wakeup, trigger SW wakeup AP from PDS);

int main(void)
{
    int ch;
    board_init();

    uartx = bflb_device_get_by_name("uart3");

    LOG_I("pds_lp_wakeup test start\r\n");
    dump_uart_clock_info();

    shell_init();
    while (1) {
        if ((ch = bflb_uart_getchar(uartx)) != -1) {
            shell_handler(ch);
        }
    }
}
