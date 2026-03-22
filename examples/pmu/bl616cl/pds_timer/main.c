#include "bflb_mtimer.h"
#include "bflb_gpio.h"
#include "board.h"
#include "bl616cl_glb.h"
#include "bl616cl_pds.h"
#include "bl616cl_hbn.h"
#include "bl616cl_aon.h"
#include "bl616cl_pm.h"

#define SLEEP_TIME  (32768)

uint32_t pds_level = PM_PDS_LEVEL_15;
uint32_t wakeup_count;
struct bflb_device_s *gpio;

/* Get flash IO select from efuse */
static uint32_t get_sf_pin_select(void)
{
    uint32_t tmpVal;
    /* Read from efuse sw usage 0 */
    tmpVal = BL_RD_WORD(0x20056000 + 0x74);
    return (tmpVal >> 5) & 0x3f;
}

/* Check if the pin is flash IO */
static int is_flash_io(uint8_t pin, uint32_t sf_pin_select)
{
    if (sf_pin_select & (1 << 2)) {
        /* Flash IO is GPIO 6-11 */
        return (pin >= GPIO_PIN_6 && pin <= GPIO_PIN_11);
    } else if (sf_pin_select & (1 << 3)) {
        /* Flash IO is GPIO 24-29 */
        return (pin >= GPIO_PIN_24 && pin <= GPIO_PIN_29);
    }
    return 0;
}

int main(void)
{
    uint32_t sf_pin_select;
    uint8_t flash_io_start = 0, flash_io_end = 0;

    PDS_Disable_ALL_GPIO_Keep();
    board_init();
    HBN_32K_Sel(HBN_32K_RC);

    gpio = bflb_device_get_by_name("gpio");

    /* Get flash IO configuration from efuse */
    sf_pin_select = get_sf_pin_select();
    if (sf_pin_select & (1 << 2)) {
        flash_io_start = GPIO_PIN_6;
        flash_io_end = GPIO_PIN_11;
        printf("Flash IO: GPIO %d-%d\r\n", flash_io_start, flash_io_end);
    } else if (sf_pin_select & (1 << 3)) {
        flash_io_start = GPIO_PIN_24;
        flash_io_end = GPIO_PIN_29;
        printf("Flash IO: GPIO %d-%d\r\n", flash_io_start, flash_io_end);
    } else {
        printf("Flash IO: internal or unknown config (sf_pin_select=0x%x)\r\n", sf_pin_select);
    }

    printf("PDS wake up by timer start\r\n");

    while (1) {
        PDS_Mask_All_Wakeup_Src();
        HBN_Pin_WakeUp_Mask(0xF);
        wakeup_count = HBN_Get_Version();
        printf("wake up count = %u, ", wakeup_count);
        HBN_Set_Version(wakeup_count + 1);

        if (pds_level == PM_PDS_LEVEL_1) {
            pm_pds_irq_register();
        }
        printf("enter pds_%u mode\r\n", pds_level);

        /* Wait for all log output complete before configuring IO */
        arch_delay_ms(10);

        /* Configure all PDS GPIO (6-36) to analog + float mode, except flash IO */
        /* Enable all PDS GPIO keep for low power consumption */
        for (uint8_t i = GPIO_PIN_6; i < GPIO_PIN_MAX; i++) {
            if (!is_flash_io(i, sf_pin_select)) {
                bflb_gpio_init(gpio, i, GPIO_ANALOG | GPIO_FLOAT | GPIO_DRV_0);
                PDS_Enable_GPIO_Keep(i);
            }
        }

        arch_delay_us(200);
        pm_pds_mode_enter(pds_level, SLEEP_TIME);

    }
}
