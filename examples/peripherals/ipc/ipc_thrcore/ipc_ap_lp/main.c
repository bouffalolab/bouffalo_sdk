#include "bflb_mtimer.h"
#include "board.h"
#include "bflb_ipc.h"
#include "bflb_irq.h"
#include "bflb_uart.h"
#define DBG_TAG "MAIN"
#include "log.h"
#include "shell.h"

#define NP_IPC_DEV_NAME "ipc1_ch1"
#define LP_IPC_DEV_NAME "ipc1_ch0"
#define AP_IPC_DEV_NAME "ipc0_ch0"

#if defined(CPU_AP)
#include "incbin.h"
INCBIN(mini_sys, "../../examples/peripherals/ipc/ipc_thrcore/ipc_ap_lp/build/build_out/ipc_trig_bl616d_lp.bin");
#endif

struct bflb_device_s *np_ipc;
struct bflb_device_s *lp_ipc;
struct bflb_device_s *ap_ipc;
static struct bflb_device_s *uartx;

#if defined(CPU_AP)
void ap_ipc_isr(int irq, void *arg)
{
    uint32_t ipc_intsta = bflb_ipc_get_intsta(ap_ipc);
    if (ipc_intsta & 0xffff0000) {
        LOG_I("LP->AP interrupt trigger!\r\n");
        LOG_I("Status=0x%08X\r\n", (ipc_intsta & 0xffff0000) >> 16);
    }

    if (ipc_intsta & 0x0000ffff) {
        LOG_I("NP->AP interrupt trigger!\r\n");
        LOG_I("Status=0x%08X\r\n", ipc_intsta & 0xffff);
    }

    bflb_ipc_clear(ap_ipc, ipc_intsta);
}
#elif defined(CPU_LP)
void lp_ipc_isr(int irq, void *arg)
{
    uint32_t ipc_intsta = bflb_ipc_get_intsta(lp_ipc);

    if (ipc_intsta & 0xffff0000) {
        LOG_I("AP->LP interrupt trigger!\r\n");
        LOG_I("Status=0x%08X\r\n", (ipc_intsta & 0xffff0000) >> 16);
    }

    if (ipc_intsta & 0x0000ffff) {
        LOG_I("NP->LP interrupt trigger!\r\n");
        LOG_I("Status=0x%08X\r\n", ipc_intsta & 0xffff);
    }

    bflb_ipc_clear(lp_ipc, ipc_intsta);
}

#endif

#if defined(CPU_AP)
int ap_trigger_lp(int argc, char **argv)
{
    printf("Trigger AP->LP interrupt\r\n");
    if (argc != 2) {
        printf("Usage: ap_trigger_lp <value>\r\n");
        return -1;
    }
    bflb_ipc_trig(lp_ipc, atoi(argv[1]));
    return 0;
}

int ap_trigger_np(int argc, char **argv)
{
    printf("Trigger AP->NP interrupt\r\n");
    if (argc != 2) {
        printf("Usage: ap_trigger_np <value>\r\n");
        return -1;
    }
    bflb_ipc_trig(np_ipc, atoi(argv[1]));
    return 0;
}

#elif defined(CPU_LP)
int lp_trigger_ap(int argc, char **argv)
{
    printf("Trigger LP->AP interrupt\r\n");
    if (argc != 2) {
        printf("Usage: lp_trigger_ap <value>\r\n");
        return -1;
    }
    bflb_ipc_trig(ap_ipc, atoi(argv[1]));
    return 0;
}

int lp_trigger_np(int argc, char **argv)
{
    printf("Trigger LP->NP interrupt\r\n");
    if (argc != 2) {
        printf("Usage: lp_trigger_np <value>\r\n");
        return -1;
    }
    bflb_ipc_trig(np_ipc, atoi(argv[1]));
    return 0;
}
#endif

#if defined(CPU_AP)
void ap_ipc_init()
{
    lp_ipc = bflb_device_get_by_name(LP_IPC_DEV_NAME);

    np_ipc = bflb_device_get_by_name(NP_IPC_DEV_NAME);

    ap_ipc = bflb_device_get_by_name(AP_IPC_DEV_NAME);
    bflb_ipc_init(ap_ipc);
    bflb_ipc_int_unmask(ap_ipc, IPC_BITS_ALL);
    bflb_irq_attach(ap_ipc->irq_num, ap_ipc_isr, NULL);
    bflb_irq_enable(ap_ipc->irq_num);
}
#elif defined(CPU_LP)
void lp_ipc_init()
{
    ap_ipc = bflb_device_get_by_name(AP_IPC_DEV_NAME);

    np_ipc = bflb_device_get_by_name(NP_IPC_DEV_NAME);

    lp_ipc = bflb_device_get_by_name(LP_IPC_DEV_NAME);
    bflb_ipc_init(lp_ipc);
    bflb_ipc_int_unmask(lp_ipc, IPC_BITS_ALL);
    bflb_irq_attach(lp_ipc->irq_num, lp_ipc_isr, NULL);
    bflb_irq_enable(lp_ipc->irq_num);
}
#endif

int main(void)
{
    int ch;
    board_init();

#if defined(CPU_AP)
    ap_ipc_init();
#elif defined(CPU_LP)
    lp_ipc_init();
#endif

#if defined(CPU_AP)
    uartx = bflb_device_get_by_name("uart0");
#elif defined(CPU_LP)
    uartx = bflb_device_get_by_name("uart3");
#endif
    LOG_I("IPC test start\r\n");
    shell_init();
    while (1) {
        if ((ch = bflb_uart_getchar(uartx)) != -1) {
            shell_handler(ch);
        }
    }
}

#if defined(CPU_AP)
SHELL_CMD_EXPORT_ALIAS(ap_trigger_lp, ap_trigger_lp, shell trigger lp interrupt.);
SHELL_CMD_EXPORT_ALIAS(ap_trigger_np, ap_trigger_np, shell trigger np interrupt.);
#elif defined(CPU_LP)
SHELL_CMD_EXPORT_ALIAS(lp_trigger_ap, lp_trigger_ap, shell trigger ap interrupt.);
SHELL_CMD_EXPORT_ALIAS(lp_trigger_np, lp_trigger_np, shell trigger np interrupt.);
#endif
