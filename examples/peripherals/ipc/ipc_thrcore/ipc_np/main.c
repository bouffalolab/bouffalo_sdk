#include "bflb_mtimer.h"
#include "board.h"
#include "bflb_ipc.h"
#include "bflb_irq.h"
#include "bflb_uart.h"
#define DBG_TAG "MAIN"
#include "log.h"
#include "shell.h"

#define AP_IPC_DEV_NAME "ipc0_ch0"
#define NP_IPC_DEV_NAME "ipc1_ch1"
#define LP_IPC_DEV_NAME "ipc1_ch0"

struct bflb_device_s *ap_ipc;
struct bflb_device_s *np_ipc;
struct bflb_device_s *lp_ipc;
static struct bflb_device_s *uartx;

void np_ipc_isr(int irq, void *arg)
{
    uint32_t ipc_intsta = bflb_ipc_get_intsta(np_ipc);

    if (ipc_intsta & 0xffff0000) {
        LOG_I("AP->NP interrupt trigger!\r\n");
        LOG_I("Status=0x%08X\r\n", irq, (ipc_intsta & 0xffff0000) >> 16);
    }

    if (ipc_intsta & 0x0000ffff) {
        LOG_I("LP->NP interrupt trigger!\r\n");
        LOG_I("Status=0x%08X\r\n", irq, ipc_intsta & 0xffff);
    }

    bflb_ipc_clear(np_ipc, ipc_intsta);
}

int np_trigger_ap(int argc, char **argv)
{
    printf("Trigger NP->AP interrupt\r\n");
    if (argc != 2) {
        printf("Usage: np_trigger_ap <value>\r\n");
        return -1;
    }
    bflb_ipc_trig(ap_ipc, atoi(argv[1]));
    return 0;
}

int np_trigger_lp(int argc, char **argv)
{
    printf("Trigger NP->LP interrupt\r\n");
    if (argc != 2) {
        printf("Usage: np_trigger_lp <value>\r\n");
        return -1;
    }
    bflb_ipc_trig(lp_ipc, atoi(argv[1]));
    return 0;
}

void np_ipc_init()
{
    ap_ipc = bflb_device_get_by_name(AP_IPC_DEV_NAME);

    lp_ipc = bflb_device_get_by_name(LP_IPC_DEV_NAME);

    np_ipc = bflb_device_get_by_name(NP_IPC_DEV_NAME);
    bflb_ipc_init(np_ipc);
    bflb_ipc_int_unmask(np_ipc, IPC_BITS_ALL);
    bflb_irq_attach(np_ipc->irq_num, np_ipc_isr, NULL);
    bflb_irq_enable(np_ipc->irq_num);
}

int main(void)
{
    int ch;
    board_init();
    np_ipc_init();
    uartx = bflb_device_get_by_name("uart1");
    LOG_I("IPC test start\r\n");
    shell_init();
    while (1) {
        if ((ch = bflb_uart_getchar(uartx)) != -1) {
            shell_handler(ch);
        }
    }
}

SHELL_CMD_EXPORT_ALIAS(np_trigger_ap, np_trigger_ap, shell trigger ap interrupt.);
SHELL_CMD_EXPORT_ALIAS(np_trigger_lp, np_trigger_lp, shell trigger lp interrupt.);
