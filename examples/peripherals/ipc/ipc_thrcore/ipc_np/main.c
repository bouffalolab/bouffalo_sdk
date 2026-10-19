#include "bflb_mtimer.h"
#include "board.h"
#include "bflb_ipc.h"
#include "bflb_irq.h"
#include "bflb_gpio.h"
#include "bflb_uart.h"
#define DBG_TAG "MAIN"
#include "log.h"
#include "shell.h"

#define AP_IPC_DEV_NAME "ipc0_ch0"
#define NP_IPC_DEV_NAME "ipc1_ch1"
#define LP_IPC_DEV_NAME "ipc1_ch0"

#define IPC_GPIO_MEASURE_ENABLE 0

#if IPC_GPIO_MEASURE_ENABLE
#define IPC_MEASURE_GPIOX_PIN GPIO_PIN_0
#define IPC_MEASURE_GPIOY_PIN GPIO_PIN_1
#define IPC_MEASURE_GPIO_CFG  (GPIO_OUTPUT | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_0)
#define IPC_MEASURE_GPIOX_HIGH() bflb_gpio_set(gpio, IPC_MEASURE_GPIOX_PIN)
#define IPC_MEASURE_GPIOX_LOW()  bflb_gpio_reset(gpio, IPC_MEASURE_GPIOX_PIN)
#define IPC_MEASURE_GPIOY_HIGH() bflb_gpio_set(gpio, IPC_MEASURE_GPIOY_PIN)
#define IPC_MEASURE_GPIOY_LOW()  bflb_gpio_reset(gpio, IPC_MEASURE_GPIOY_PIN)
#else
#define IPC_MEASURE_GPIOX_HIGH()
#define IPC_MEASURE_GPIOX_LOW()
#define IPC_MEASURE_GPIOY_HIGH()
#define IPC_MEASURE_GPIOY_LOW()
#endif

struct bflb_device_s *ap_ipc;
struct bflb_device_s *np_ipc;
struct bflb_device_s *lp_ipc;
#if IPC_GPIO_MEASURE_ENABLE
static struct bflb_device_s *gpio;
#endif
static struct bflb_device_s *uartx;

#if IPC_GPIO_MEASURE_ENABLE
static void ipc_measure_gpio_init(void)
{
    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, IPC_MEASURE_GPIOX_PIN, IPC_MEASURE_GPIO_CFG);
    bflb_gpio_init(gpio, IPC_MEASURE_GPIOY_PIN, IPC_MEASURE_GPIO_CFG);
    IPC_MEASURE_GPIOX_LOW();
    IPC_MEASURE_GPIOY_LOW();
}
#endif

void np_ipc_isr(int irq, void *arg)
{
    IPC_MEASURE_GPIOY_HIGH();
    uint32_t ipc_intsta = bflb_ipc_get_intsta(np_ipc);

    if (ipc_intsta & 0xffff0000) {
        LOG_I("AP->NP interrupt trigger!\r\n");
        LOG_I("Status=0x%08X\r\n", (ipc_intsta & 0xffff0000) >> 16);
    }

    if (ipc_intsta & 0x0000ffff) {
        LOG_I("LP->NP interrupt trigger!\r\n");
        LOG_I("Status=0x%08X\r\n", ipc_intsta & 0xffff);
    }

    bflb_ipc_clear(np_ipc, ipc_intsta);
    IPC_MEASURE_GPIOY_LOW();
}

int np_trigger_ap(int argc, char **argv)
{
    printf("Trigger NP->AP interrupt\r\n");
    if (argc != 2) {
        printf("Usage: np_trigger_ap <value>\r\n");
        return -1;
    }
    IPC_MEASURE_GPIOX_HIGH();
    bflb_ipc_trig(ap_ipc, atoi(argv[1]));
    IPC_MEASURE_GPIOX_LOW();
    return 0;
}

int np_trigger_lp(int argc, char **argv)
{
    printf("Trigger NP->LP interrupt\r\n");
    if (argc != 2) {
        printf("Usage: np_trigger_lp <value>\r\n");
        return -1;
    }
    IPC_MEASURE_GPIOX_HIGH();
    bflb_ipc_trig(lp_ipc, atoi(argv[1]));
    IPC_MEASURE_GPIOX_LOW();
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

#if IPC_GPIO_MEASURE_ENABLE
    ipc_measure_gpio_init();
#endif

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
