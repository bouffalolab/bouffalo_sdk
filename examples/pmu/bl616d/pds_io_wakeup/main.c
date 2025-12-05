#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "shell.h"
#include "board.h"
#include "bl616d_pm.h"
#include "bl616d_hbn.h"
#include "bl616d_pds.h"

volatile lp_gpio_cfg_type lp_wake_io_cfg;

static void app_print_wakeup_source(void)
{
    if (SET == PDS_Get_IntStatus(PDS_INT_WAKEUP)) {
        if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_PDS_GPIO_INT)) {
            for (uint32_t i = 8; i <= 49; i++) {
                if (PDS_Get_GPIO_Pad_IntStatus(i)) {
                    printf("gpio_%d wakeup pds\r\n", i);
                    PDS_Set_GPIO_Pad_IntClr(i);
                }
            }
        }
        if (SET == PDS_Get_Wakeup_Src(PDS_WAKEUP_BY_HBN_IRQ_OUT)) {
            for (uint8_t i = HBN_INT_GPIO0; i <= HBN_INT_GPIO7; i++) {
                if (SET == HBN_Get_INT_State(i)) {
                    printf("gpio%d wakeup pds\r\n", i);
                    HBN_Clear_IRQ(i);
                }
            }
        }
        PDS_IntClear();
    } else {
        printf("first power on\r\n");
    }
}

int main(void)
{
    int ch;
    struct bflb_device_s *uartx;

    board_init();

    app_print_wakeup_source();

    pm_pds_mask_all_wakeup_src();

    for (uint8_t i = 0; i < GPIO_PIN_MAX; i++) {
        pm_set_gpio_int_mask(i, MASK);
    }
    uartx = bflb_device_get_by_name("uart0");

    shell_init();
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
    uint32_t pds_mode = PM_PDS_LEVEL_3;
    uint32_t pull = 0;

    if (argc != 5) {
        printf("Usage: pds_io_wake_up <pds_mode> <test_io> <trigger_mode> <pull>\r\n");
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

    printf("pds_mode:%d\r\n", pds_mode);
    printf("test_io:%d\r\n", test_io);
    printf("test_trig_type:%s\r\n", pm_get_trig_mode_desc(trig_mode));
    if (1 == pull) {
        printf("pull:up\r\n");
        lp_wake_io_cfg.io_pu = (uint64_t)1 << test_io;
    } else if (2 == pull) {
        printf("pull:down\r\n");
        lp_wake_io_cfg.io_pd = (uint64_t)1 << test_io;
    } else {
        printf("pull:none\r\n");
    }

    lp_wake_io_cfg.io_ie = (uint64_t)1 << test_io;
    lp_wake_io_cfg.io_wakeup_unmask = (uint64_t)1 << test_io;
    if (test_io <= 7) {
        lp_wake_io_cfg.io_0_7_trig_mode = trig_mode;
    } else if (test_io <= 15) {
        lp_wake_io_cfg.io_8_15_trig_mode = trig_mode;
    } else if (test_io <= 23) {
        lp_wake_io_cfg.io_16_23_trig_mode = trig_mode;
    } else if (test_io <= 31) {
        lp_wake_io_cfg.io_24_31_trig_mode = trig_mode;
    } else if (test_io <= 39) {
        lp_wake_io_cfg.io_32_39_trig_mode = trig_mode;
    } else if (test_io <= 47) {
        lp_wake_io_cfg.io_40_47_trig_mode = trig_mode;
    } else if (test_io < GPIO_PIN_MAX) {
        lp_wake_io_cfg.io_48_52_trig_mode = trig_mode;
    } else {
        printf("[ERR]test_io >= %d\r\n", GPIO_PIN_MAX);
        return 0;
    }

    pm_lowpower_gpio_cfg((lp_gpio_cfg_type *)&lp_wake_io_cfg);

    switch (pds_mode) {
        case 1:
            printf("enter PDS1 mode\r\n");
            arch_delay_us(500);
            pm_pds_irq_register();
            pm_pds_mode_enter(PM_PDS_LEVEL_1, 0);
            printf("PDS1 wakeup\r\n");
            break;
        case 2:
            printf("enter PDS2 mode\r\n");
            arch_delay_us(500);
            pm_pds_mode_enter(PM_PDS_LEVEL_2, 0);
            /* should not reach here */
            printf("PDS2 wakeup\r\n");
            break;
        case 3:
            printf("enter PDS3 mode\r\n");
            arch_delay_us(500);
            pm_pds_mode_enter(PM_PDS_LEVEL_3, 0);
            /* should not reach here */
            printf("PDS3 wakeup\r\n");
            break;
        case 7:
            printf("enter PDS7 mode\r\n");
            arch_delay_us(500);
            pm_pds_mode_enter(PM_PDS_LEVEL_7, 0);
            /* should not reach here */
            printf("PDS7 wakeup\r\n");
            break;
        case 15:
            printf("enter PDS15 mode\r\n");
            arch_delay_us(500);
            pm_pds_mode_enter(PM_PDS_LEVEL_15, 0);
            /* should not reach here */
            printf("PDS15 wakeup\r\n");
            break;
        default:
            printf("enter pds15 mode\r\n");
            arch_delay_us(500);
            pm_pds_mode_enter(PM_PDS_LEVEL_15, 0);
            /* should not reach here */
            printf("PDS15 wakeup\r\n");
            break;
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(pds_io_wakeup_test, pds_io_wake_up, pds io wakeup test);
