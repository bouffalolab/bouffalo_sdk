#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "shell.h"
#include "board.h"
#include "bl616l_pds.h"
#include "bl616l_hbn.h"
#include "bl616l_pm.h"

volatile lp_gpio_cfg_Type lp_wake_io_cfg;

int main(void)
{
    int ch;
    struct bflb_device_s *uartx;

    board_init();
    HBN_32K_Sel(HBN_32K_RC);
    for (uint8_t i = 0; i < GPIO_PIN_MAX; i++) {
        pm_set_gpio_int_mask(i, MASK);
    }
    uartx = bflb_device_get_by_name("uart0");
    shell_init();
    printf("\r\nUsage: hbn <hbn_mode> <test_io> <trig_type> <pull>\r\n");
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
    uint32_t hbn_mode = PM_HBN_LEVEL_0;
    uint32_t pull = 0;

    if (argc != 5) {
        printf("Usage: hbn <hbn_mode> <test_io> <trig_type> <pull>\r\n");
        return 0;
    }

    hbn_mode = atoi(argv[1]);
    if (hbn_mode >= PM_HBN_LEVEL_2) {
        printf("hbn_mode must be less than %d\r\n", PM_HBN_LEVEL_2);
        return 0;
    }
    test_io = atoi(argv[2]);
    if (test_io >= GPIO_PIN_6) {
        printf("test_io must be less than %d\r\n", GPIO_PIN_6);
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

    printf("hbn_mode: HBN_%d\r\n", hbn_mode);
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

    switch (hbn_mode) {
        case 0:
            pm_hbn_mode_enter(PM_HBN_LEVEL_0, 0);
            break;
        case 1:
            pm_hbn_mode_enter(PM_HBN_LEVEL_1, 0);
            break;
        default:
            pm_hbn_mode_enter(PM_HBN_LEVEL_0, 0);
            break;
    }
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(hbn_io_wakeup_test, hbn, hbn io wakeup test);
