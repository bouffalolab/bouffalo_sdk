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
    uint8_t wakeup_source_found = 0;

    for (uint8_t i = HBN_INT_GPIO0; i <= HBN_INT_GPIO7; i++) {
        if (SET == HBN_Get_INT_State(i)) {
            printf("gpio%d int state: 1\r\n", i);
            wakeup_source_found = 1;
            HBN_Clear_IRQ(i);
        }
    }
    if (SET == HBN_Get_INT_State(HBN_INT_RTC)) {
        printf("[ERR]HBN wakeup by RTC\r\n");
        wakeup_source_found = 1;
        HBN_Clear_IRQ(HBN_INT_RTC);
    }
    if (!wakeup_source_found) {
        printf("first power on\r\n");
    } else {
        printf("wakeup source found\r\n");
    }
}

int main(void)
{
    int ch;
    struct bflb_device_s *uartx;

    board_init();

    app_print_wakeup_source();

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

int hbn_io_wakeup_test(int argc, char **argv)
{
    uint32_t trig_mode = 0;
    uint32_t test_io = GPIO_PIN_0;
    uint32_t hbn_mode = 0;
    uint32_t pull = 0;

    if (argc != 5) {
        printf("Usage: hbn_io_wake_up <hbn_mode> <test_io> <trigger_mode> <pull>\r\n");
        return 0;
    }

    hbn_mode = atoi(argv[1]);
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

    printf("hbn_mode:%d\r\n", hbn_mode);
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
    } else {
        printf("[ERR]test_io >= %d\r\n", GPIO_PIN_MAX);
        return 0;
    }

    pm_lowpower_gpio_cfg((lp_gpio_cfg_type *)&lp_wake_io_cfg);

    switch (hbn_mode) {
        case 0:
            printf("enter hbn0 mode\r\n");
            arch_delay_us(50);
            pm_hbn_mode_enter(PM_HBN_LEVEL_0, 0);
            break;
        case 1:
            printf("enter hbn1 mode\r\n");
            arch_delay_us(50);
            pm_hbn_mode_enter(PM_HBN_LEVEL_1, 0);
        default:
            printf("enter hbn0 mode\r\n");
            arch_delay_us(50);
            pm_hbn_mode_enter(PM_HBN_LEVEL_0, 0);
            break;
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(hbn_io_wakeup_test, hbn_io_wake_up, pds io wakeup test);
