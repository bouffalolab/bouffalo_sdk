#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "shell.h"
#include "board.h"
#include "log.h"
#include <assert.h>
#include "bl616l_glb.h"
#include "bl616l_pds.h"
#include "bl616l_hbn.h"
#include "bl616l_aon.h"
#include "bl616l_pm.h"
#include "bflb_flash.h"

volatile lp_gpio_cfg_Type lp_wake_io_cfg;

struct bflb_device_s *gpio;

static void app_print_wakeup_source(void)
{
    if (SET == PDS_Get_IntStatus(PDS_INT_WAKEUP)) {
        if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_PDS_GPIO_INT)) {
            for (uint32_t i = 6; i < GPIO_PIN_MAX; i++) {
                if (PDS_Get_GPIO_Pad_IntStatus(i)) {
                    printf("gpio_%d wakeup pds\r\n", i);
                    PDS_Set_GPIO_Pad_IntClr(i);
                    bflb_gpio_int_clear(gpio, i);
                }
            }
        }

        if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_HBN_IRQ_OUT)) {
            if (SET == HBN_Get_INT_State(HBN_INT_GPIO0)) {
                HBN_Clear_IRQ(HBN_INT_GPIO0);
                printf("gpio_%d wakeup pds\r\n", 0);
            }
            if (SET == HBN_Get_INT_State(HBN_INT_GPIO1)) {
                HBN_Clear_IRQ(HBN_INT_GPIO1);
                printf("gpio_%d wakeup pds\r\n", 1);
            }
            if (SET == HBN_Get_INT_State(HBN_INT_GPIO2)) {
                HBN_Clear_IRQ(HBN_INT_GPIO2);
                printf("gpio_%d wakeup pds\r\n", 2);
            }
            if (SET == HBN_Get_INT_State(HBN_INT_GPIO3)) {
                HBN_Clear_IRQ(HBN_INT_GPIO3);
                printf("gpio_%d wakeup pds\r\n", 3);
            }
            if (SET == HBN_Get_INT_State(HBN_INT_GPIO4)) {
                HBN_Clear_IRQ(HBN_INT_GPIO4);
                printf("gpio_%d wakeup pds\r\n", 4);
            }
            if (SET == HBN_Get_INT_State(HBN_INT_GPIO5)) {
                HBN_Clear_IRQ(HBN_INT_GPIO5);
                printf("gpio_%d wakeup pds\r\n", 5);
            }
        }
        PDS_IntClear();
    } else {
        printf("first power on\r\n");
    }
}

void pm_irq_callback(uint32_t event)
{
    bflb_gpio_uart_init(gpio, GPIO_PIN_34, GPIO_UART_FUNC_UART0_TX);
    bflb_gpio_uart_init(gpio, GPIO_PIN_35, GPIO_UART_FUNC_UART0_RX);
    if (event < PM_PDS_WAKEUP_EVENT_MIN) {
        printf("wakeup event by HBN, event = %u\r\n", event);
    } else if (event < PM_PDS_WAKEUP_EVENT_MAX) {
        printf("wakeup event by PDS, event = %u\r\n", event - PM_PDS_WAKEUP_EVENT_MIN);
    } else {
        printf(" this event has no process, event = %u\r\n", event);
    }
}

int main(void)
{
    int ch;
    struct bflb_device_s *uartx;

    board_init();
    HBN_32K_Sel(HBN_32K_RC);

    gpio = bflb_device_get_by_name("gpio");

    /* gpio latch clear */
    for (uint8_t i = 0; i < GPIO_PIN_MAX; i++) {
        PDS_Disable_GPIO_Keep(i);
    }

    app_print_wakeup_source();
    pm_pds_mask_all_wakeup_src();
    HBN_Pin_WakeUp_Mask(0xF);

    for (uint8_t i = GPIO_PIN_0; i <= GPIO_PIN_5; i++) {
        HBN_Set_Aon_Pad_IntMask(i, MASK);
    }
    for (uint8_t i = GPIO_PIN_6; i < GPIO_PIN_MAX; i++) {
        PDS_Set_GPIO_Pad_IntMask(i, MASK);
    }

    uartx = bflb_device_get_by_name("uart0");
    shell_init();
    printf("\r\nUsage: pds <pds_mode> <test_io> <trig_type> <pull>\r\n");
    while (1) {
        if ((ch = bflb_uart_getchar(uartx)) != -1) {
            shell_handler(ch);
        }
    }
}

int pds_io_wakeup_test(int argc, char **argv)
{
    uint32_t trig_mode = 0;
    uint32_t test_io = GPIO_PIN_0;
    uint32_t pds_mode = PM_PDS_LEVEL_15;
    uint32_t pull = 0;

    if (argc != 5) {
        printf("Usage: pds <pds_mode> <test_io> <trig_type> <pull>\r\n");
        return 0;
    }

    pds_mode = atoi(argv[1]);
    test_io = atoi(argv[2]);
    if (test_io >= GPIO_PIN_MAX) {
        printf("test_io must be less than %d\r\n", GPIO_PIN_MAX);
        return 0;
    }
    trig_mode = atoi(argv[3]);
    pull = atoi(argv[4]);
    if (pull >= 3) {
        printf("pull must be less than %d\r\n", 3);
        return 0;
    }

    /* lp_wake_io_cfg init */
    memset((void *)&lp_wake_io_cfg, 0x00, sizeof(lp_wake_io_cfg));

    printf("pds_mode: PDS_%d\r\n", pds_mode);
    printf("test_io: %d\r\n", test_io);
    printf("test_trig_type:%s\r\n", pm_get_trig_mode_desc(trig_mode));
    if (1 == pull) {
        printf("pull: up\r\n");
        lp_wake_io_cfg.io_pu = (uint64_t)1 << test_io;
        lp_wake_io_cfg.io_pd = (uint64_t)0 << test_io;
    } else if (2 == pull) {
        printf("pull: down\r\n");
        lp_wake_io_cfg.io_pu = (uint64_t)0 << test_io;
        lp_wake_io_cfg.io_pd = (uint64_t)1 << test_io;
    } else {
        printf("pull: none\r\n");
        lp_wake_io_cfg.io_pu = (uint64_t)0 << test_io;
        lp_wake_io_cfg.io_pd = (uint64_t)0 << test_io;
    }
    arch_delay_us(500);

    lp_wake_io_cfg.io_ie = (uint64_t)1 << test_io;
    lp_wake_io_cfg.io_wakeup_unmask = (uint64_t)1 << test_io;
    if (test_io <= 5) {
        lp_wake_io_cfg.io_0_5_trig_mode = trig_mode;
    } else if (test_io < GPIO_PIN_MAX) {
        lp_wake_io_cfg.io_6_36_trig_mode[test_io - 6] = trig_mode;
    } else {
        printf("[ERR]test_io >= %d\r\n", GPIO_PIN_MAX);
        return 0;
    }
    pm_lowpower_gpio_cfg((lp_gpio_cfg_Type *)&lp_wake_io_cfg);
    PDS_Set_All_GPIO_Pad_IntClr();

    switch (pds_mode) {
        case 1:
            pm_pds_irq_register();
            pm_pds_mode_enter(PM_PDS_LEVEL_1, 0);
            bflb_gpio_uart_init(gpio, GPIO_PIN_34, GPIO_UART_FUNC_UART0_TX);
            bflb_gpio_uart_init(gpio, GPIO_PIN_35, GPIO_UART_FUNC_UART0_RX);
            break;
        case 2:
            pm_pds_mode_enter(PM_PDS_LEVEL_2, 0);
            break;
        case 3:
            pm_pds_mode_enter(PM_PDS_LEVEL_3, 0);
            break;
        case 7:
            pm_pds_mode_enter(PM_PDS_LEVEL_7, 0);
            break;
        case 15:
            pm_pds_mode_enter(PM_PDS_LEVEL_15, 0);
            break;
        default:
            pm_pds_mode_enter(PM_PDS_LEVEL_15, 0);
            break;
    }
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(pds_io_wakeup_test, pds, pds io wakeup test);
