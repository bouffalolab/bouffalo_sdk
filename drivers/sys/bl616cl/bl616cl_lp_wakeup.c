#include "bl_lp_internal.h"

static ATTR_NOCACHE_RAM_SECTION lp_fw_gpio_cfg_t *gp_lp_io_cfg = NULL;

/* Get flash IO select from efuse */
static uint32_t ATTR_TCM_SECTION get_sf_pin_select(void)
{
    uint32_t tmpVal;
    /* Read from efuse sw usage 0 */
    tmpVal = BL_RD_WORD(0x20056000 + 0x74);
    return (tmpVal >> 5) & 0x3f;
}

/* Check if the pin is flash IO */
static int ATTR_TCM_SECTION is_flash_io(uint8_t pin, uint32_t sf_pin_select)
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

static int is_gpio_input_floating(uint32_t gpio_cfg)
{
    return ((gpio_cfg & GLB_REG_GPIO_0_IE_MSK) != 0U) &&
           ((gpio_cfg & GLB_REG_GPIO_0_OE_MSK) == 0U) &&
           ((gpio_cfg & GLB_REG_GPIO_0_PU_MSK) == 0U) &&
           ((gpio_cfg & GLB_REG_GPIO_0_PD_MSK) == 0U);
}

void bl_lp_check_gpio_leakage_risk(void)
{
    uint32_t gpio_cfg;

    for (uint8_t i = 0; i < GPIO_PIN_MAX; i++) {
        gpio_cfg = BL_RD_WORD(GLB_BASE + GLB_GPIO_CFG0_OFFSET + (i << 2));
        // BL_LP_LOG("[LP] GPIO%u CFG: 0x%08lX\r\n", i, (unsigned long)gpio_cfg);
        if (is_gpio_input_floating(gpio_cfg)) {
            BL_LP_LOG("[LP][WARN] GPIO%u is input enabled and floating, current gpio cfg may leak current in low power.\r\n", i);
        }
    }
}


int bl_lp_io_wakeup_cfg(void *io_wakeup_cfg)
{
    gp_lp_io_cfg = (lp_fw_gpio_cfg_t *)io_wakeup_cfg;
    return 0;
}

void bl_lp_io_wakeup_init(lp_fw_gpio_cfg_t *cfg)
{
    int ret;

    ret = pm_lowpower_gpio_cfg((lp_gpio_cfg_type *)cfg);
    if (ret) {
        BL_LP_LOG("[LP] io wakeup init fail!\r\n");
    } else {
        BL_LP_LOG("[LP] io wakeup init succeed!\r\n");
    }
}

void bl616cl_lp_io_wakeup_prepare(void)
{
    lp_fw_gpio_cfg_t *cfg = gp_lp_io_cfg;

    bl_lp_check_gpio_leakage_risk();

    if (cfg) {
        iot2lp_para->wakeup_source_parameter->io_wakeup_parameter = (void *)cfg;
        BL_LP_LOG("io_wakeup_unmask: 0x%llX\r\n",
                  (unsigned long long)((lp_fw_gpio_cfg_t *)iot2lp_para->wakeup_source_parameter->io_wakeup_parameter)
                      ->io_wakeup_unmask);
        bl_lp_io_wakeup_init((lp_fw_gpio_cfg_t *)iot2lp_para->wakeup_source_parameter->io_wakeup_parameter);
    } else {
        uint32_t sf_pin_select;
        /* Get flash IO configuration and configure GPIO for low power */
        sf_pin_select = get_sf_pin_select();
        if (sf_pin_select == 0x3f) {
            sf_pin_select = SF_IO_EXT_SF2;
        }
        /* Enable all PDS GPIO keep for low power consumption */
        for (uint8_t i = GPIO_PIN_6; i < GPIO_PIN_MAX; i++) {
            if (!is_flash_io(i, sf_pin_select) /* && !is_uart_io(i)*/ ) {
                PDS_Enable_GPIO_Keep(i);
            }
        }
    }
}

int bl_lp_wakeup_io_get_mode(uint8_t io_num)
{
    lp_fw_gpio_cfg_t *b_lp_io_cfg_bak = NULL;
    uint64_t wakeup_io_bits = iot2lp_para->wakeup_reason_info->wakeup_io_bits;
    uint8_t trig_mode;

    b_lp_io_cfg_bak = (lp_fw_gpio_cfg_t *)iot2lp_para->wakeup_source_parameter->io_wakeup_parameter;

    if (io_num >= BL_LP_WAKEUP_IO_MAX_NUM) {
        return -1;
    }

    if ((wakeup_io_bits & ((uint64_t)0x1 << io_num)) == 0) {
        return 0;
    }

    trig_mode = b_lp_io_cfg_bak->io_0_36_trig_mode[io_num];

    if (trig_mode == BL_LP_PDS_IO_TRIG_SYNC_FALLING_EDGE || trig_mode == BL_LP_PDS_IO_TRIG_ASYNC_FALLING_EDGE) {
        return BL_LP_IO_WAKEUP_MODE_FALLING;
    } else if (trig_mode == BL_LP_PDS_IO_TRIG_SYNC_RISING_EDGE || trig_mode == BL_LP_PDS_IO_TRIG_ASYNC_RISING_EDGE) {
        return BL_LP_IO_WAKEUP_MODE_RISING;
    } else if (trig_mode == BL_LP_PDS_IO_TRIG_SYNC_LOW_LEVEL || trig_mode == BL_LP_PDS_IO_TRIG_ASYNC_LOW_LEVEL) {
            return BL_LP_IO_WAKEUP_MODE_LOW;
    } else if (trig_mode == BL_LP_PDS_IO_TRIG_SYNC_HIGH_LEVEL || trig_mode == BL_LP_PDS_IO_TRIG_ASYNC_HIGH_LEVEL) {
        return BL_LP_IO_WAKEUP_MODE_HIGH;
    } else if (trig_mode == BL_LP_PDS_IO_TRIG_SYNC_RISING_FALLING_EDGE) {
        return BL_LP_IO_WAKEUP_MODE_RISING_FALLING;
    } else {
        return -1;
    }
}
