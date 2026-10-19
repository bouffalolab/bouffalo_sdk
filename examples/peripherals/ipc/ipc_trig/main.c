#include "bflb_mtimer.h"
#include "board.h"

#include "bflb_ipc.h"
#include "bflb_irq.h"

#define DBG_TAG "MAIN"
#include "log.h"

struct bflb_device_s *ipc0_ch0;
struct bflb_device_s *ipc0_ch1;
struct bflb_device_s *ipc1_ch0;
struct bflb_device_s *ipc1_ch1;

void ipc0_ch0_isr(int irq, void *arg)
{
    uint32_t ipc_intsta = bflb_ipc_get_intsta(ipc0_ch0);

    LOG_I("ipc0_ch0 isr come, irq %d, sta 0x%08X\r\n", irq, ipc_intsta);

    bflb_ipc_clear(ipc0_ch0, ipc_intsta);
}

void ipc0_ch1_isr(int irq, void *arg)
{
    uint32_t ipc_intsta = bflb_ipc_get_intsta(ipc0_ch1);

    LOG_I("ipc0_ch1 isr come, irq %d, sta 0x%08X\r\n", irq, ipc_intsta);

    bflb_ipc_clear(ipc0_ch1, ipc_intsta);
}

void ipc1_ch0_isr(int irq, void *arg)
{
    uint32_t ipc_intsta = bflb_ipc_get_intsta(ipc1_ch0);

    LOG_I("ipc1_ch0 isr come, irq %d, sta 0x%08X\r\n", irq, ipc_intsta);

    bflb_ipc_clear(ipc1_ch0, ipc_intsta);
}

void ipc1_ch1_isr(int irq, void *arg)
{
    uint32_t ipc_intsta = bflb_ipc_get_intsta(ipc1_ch1);

    LOG_I("ipc1_ch1 isr come, irq %d, sta 0x%08X\r\n", irq, ipc_intsta);

    bflb_ipc_clear(ipc1_ch1, ipc_intsta);
}

void ipc_test(void)
{
    ipc0_ch0 = bflb_device_get_by_name("ipc0_ch0");
    /* ipc0_ch0 */
    bflb_ipc_init(ipc0_ch0);
    bflb_ipc_int_unmask(ipc0_ch0, IPC_BITS_ALL);
    /* enable irq */
    bflb_irq_attach(ipc0_ch0->irq_num, ipc0_ch0_isr, NULL);
    bflb_irq_enable(ipc0_ch0->irq_num);
    /* trigger */
    LOG_I("IPC trigger ipc0_ch0: 0x%08X\r\n", IPC_BIT_NUM(0));
    bflb_ipc_trig(ipc0_ch0, IPC_BIT_NUM(0));

    ipc0_ch1 = bflb_device_get_by_name("ipc0_ch1");
    /* ipc0_ch1 */
    bflb_ipc_init(ipc0_ch1);
    bflb_ipc_int_unmask(ipc0_ch1, IPC_BITS_ALL);
    /* enable irq */
    bflb_irq_attach(ipc0_ch1->irq_num, ipc0_ch1_isr, NULL);
    bflb_irq_enable(ipc0_ch1->irq_num);
    /* trigger */
    LOG_I("IPC trigger ipc0_ch1: 0x%08X\r\n", IPC_BIT_NUM(1));
    bflb_ipc_trig(ipc0_ch1, IPC_BIT_NUM(1));

    ipc1_ch0 = bflb_device_get_by_name("ipc1_ch0");
    /* ipc1_ch0 */
    bflb_ipc_init(ipc1_ch0);
    bflb_ipc_int_unmask(ipc1_ch0, IPC_BITS_ALL);
    /* enable irq */
    bflb_irq_attach(ipc1_ch0->irq_num, ipc1_ch0_isr, NULL);
    bflb_irq_enable(ipc1_ch0->irq_num);
    /* trigger */
    LOG_I("IPC trigger ipc1_ch0: 0x%08X\r\n", IPC_BIT_NUM(2) | IPC_BIT_NUM(3));
    bflb_ipc_trig(ipc1_ch0, IPC_BIT_NUM(2) | IPC_BIT_NUM(3));

    ipc1_ch1 = bflb_device_get_by_name("ipc1_ch1");
    /* ipc1_ch1 */
    bflb_ipc_init(ipc1_ch1);
    bflb_ipc_int_unmask(ipc1_ch1, IPC_BITS_ALL);
    /* enable irq */
    bflb_irq_attach(ipc1_ch1->irq_num, ipc1_ch1_isr, NULL);
    bflb_irq_enable(ipc1_ch1->irq_num);
    /* trigger */
    LOG_I("IPC trigger ipc1_ch1: 0x%08X\r\n", IPC_BITS_ALL);
    bflb_ipc_trig(ipc1_ch1, IPC_BITS_ALL);

    LOG_I("IPC trigger ipc0_ch0: 0x%08X\r\n", 0x0A);
    bflb_ipc_trig(ipc0_ch0, 0x0A);

    LOG_I("IPC trigger ipc0_ch1: 0x%08X\r\n", 0x0B);
    bflb_ipc_trig(ipc0_ch1, 0x0B);

    LOG_I("IPC trigger ipc1_ch0: 0x%08X\r\n", 0x0C);
    bflb_ipc_trig(ipc1_ch0, 0x0C);

    LOG_I("IPC trigger ipc1_ch1: 0x%08X\r\n", 0x0D);
    bflb_ipc_trig(ipc1_ch1, 0x0D);
}

int main(void)
{
    board_init();

    LOG_I("IPC test start\r\n");

    ipc_test();

    LOG_I("IPC test end\r\n");

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
