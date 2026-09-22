#include "flashloader_port.h"

#include <stddef.h>
#include <string.h>

#include "bflb_clock.h"
#include "bflb_common.h"
#include "bflb_ef_ctrl.h"
#include "bflb_efuse.h"
#include "bflb_mtimer.h"
#include "bflb_sf_cfg.h"
#include "bflb_sflash.h"
#include "bl618dg_aon.h"
#include "bl618dg_clock.h"
#include "bl618dg_glb.h"
#include "bl618dg_hbn.h"

#define EF_SW_USAGE0_OFFSET 0x5CU
#define FLASH_PIN_SHIFT     14U
#define FLASH_PIN_MASK      0x3FU
#define BL618DG_EFUSE_SIZE  512U

static uint32_t g_efuse_words[BL618DG_EFUSE_SIZE / sizeof(uint32_t)]
    __attribute__((aligned(4)));
static uint32_t g_efuse_verify[BL618DG_EFUSE_SIZE / sizeof(uint32_t)]
    __attribute__((aligned(4)));

static uint8_t normalize_flash_pin(uint8_t flash_pin)
{
    if (flash_pin == FLASH_PIN_MASK) {
        return SF_IO_EXT_SF2;
    }
    if ((flash_pin & 0x08U) != 0U) {
        return SF_IO_EXT_SF3;
    }
    return flash_pin;
}

static void flashloader_clock_init(void)
{
    GLB_Power_On_XTAL_And_PLL_CLK(GLB_XTAL_40M,
                                  GLB_PLL_WIFIPLL | GLB_PLL_CPUPLL);
    GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_CPUPLL_DIV1);
    HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_XTAL);
    AON_Set_RC32M_Speed_As_8M(0);
    CPU_Reset_MTimer();
    CPU_Set_MTimer_CLK(ENABLE, BL_MTIMER_SOURCE_CLOCK_MCU_XCLK,
                       Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK) /
                               1000000U -
                           1U);
    PERIPHERAL_CLOCK_SEC_ENABLE();
}

void flashloader_port_init(void)
{
    flashloader_clock_init();
    GLB_Halt_CPU(GLB_CORE_ID_NP);
    GLB_Halt_CPU(GLB_CORE_ID_LP);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SEC_ENG);
}

int flashloader_port_flash_init(void)
{
    struct sf_ctrl_cfg_type controller = {
        .owner = SF_CTRL_OWNER_SAHB,
        .en32b_addr = 0,
        .clk_delay = 1,
        .clk_invert = 1,
        .rx_clk_invert = 0,
        .do_delay = 0,
        .di_delay = 0,
        .oe_delay = 0,
    };
    uint32_t sw_usage0 = 0U;
    uint8_t flash_pin;

    bflb_ef_ctrl_read_direct(NULL, EF_SW_USAGE0_OFFSET, &sw_usage0, 1, 0);
    flash_pin = normalize_flash_pin(
        (uint8_t)((sw_usage0 >> FLASH_PIN_SHIFT) & FLASH_PIN_MASK));

    AON_Set_Ldo18_AON_Power_Switch_For_FLASH(0);
    bflb_mtimer_delay_ms(1);
    AON_Set_Ldo18_AON_Power_Switch_For_FLASH(1);
    bflb_mtimer_delay_ms(10);
    GLB_Set_SF_CLK(1, GLB_SFLASH_CLK_BCLK, 3);
    GLB_Set_Flash_Xip(1);

    if (bflb_sf_cfg_flash_init(flash_pin, &controller, NULL) != 0) {
        return -1;
    }
    GLB_Set_Flash_Id_Value(0);
    return 0;
}

int flashloader_port_flash_configure(uint32_t setting)
{
    struct sf_ctrl_cfg_type controller = {
        .owner = SF_CTRL_OWNER_SAHB,
        .en32b_addr = 0,
        .clk_delay = 0,
        .clk_invert = 1,
        .rx_clk_invert = 0,
        .do_delay = 0,
        .di_delay = 0,
        .oe_delay = 0,
    };
    uint8_t flash_pin = (uint8_t)setting;
    uint8_t flash_clock_cfg = (uint8_t)(setting >> 8);
    uint8_t flash_clock_source = flash_clock_cfg >> 4;
    uint8_t flash_clock_divider = flash_clock_cfg & 0x0FU;
    uint8_t flash_clock_delay = (uint8_t)(setting >> 24);

    if ((flash_clock_source > GLB_SFLASH_CLK_WIFIPLL_96M) ||
        (flash_clock_divider > 7U) || (flash_clock_delay > 3U)) {
        return -1;
    }

    controller.clk_delay = flash_clock_delay & 1U;
    controller.rx_clk_invert = (flash_clock_delay >> 1) & 1U;

    GLB_Set_SF_CLK(1, GLB_SFLASH_CLK_BCLK, 3);
    if (flash_pin != UINT8_MAX) {
        flash_pin = normalize_flash_pin(flash_pin);
        if (bflb_sf_cfg_flash_init(flash_pin, &controller, NULL) != 0) {
            return -1;
        }
    } else {
        bflb_sflash_init(&controller, NULL);
    }
    if (GLB_Set_SF_CLK(1, flash_clock_source, flash_clock_divider) != SUCCESS) {
        return -1;
    }
    bflb_sflash_init(&controller, NULL);
    GLB_Set_Flash_Id_Value(0);
    return 0;
}

void flashloader_port_reset(void)
{
    HBN_Set_User_Boot_Config(2);
    GLB_SW_System_Reset();
    for (;;) {
        __WFI();
    }
}

int flashloader_port_read_mac(uint8_t mac[6])
{
    uint8_t chip_id[8];

    if ((mac == NULL) || (bflb_efuse_get_chipid(chip_id) != 0)) {
        return -1;
    }
    for (uint32_t i = 0U; i < 6U; i++) {
        mac[i] = chip_id[i];
    }
    return 0;
}

static int efuse_range_valid(uint32_t address, uint32_t length)
{
    return ((address & 3U) == 0U) && ((length & 3U) == 0U) &&
           (length <= BL618DG_EFUSE_SIZE) &&
           (address <= BL618DG_EFUSE_SIZE - length);
}

int flashloader_port_efuse_read(uint32_t address, uint8_t *data,
                               uint32_t length)
{
    if ((data == NULL) || !efuse_range_valid(address, length)) {
        return -1;
    }
    if (length == 0U) {
        return 0;
    }
    bflb_ef_ctrl_read_direct(NULL, address, g_efuse_words,
                             length / sizeof(uint32_t), 1);
    memcpy(data, g_efuse_words, length);
    return 0;
}

int flashloader_port_efuse_write(uint32_t address, const uint8_t *data,
                                uint32_t length)
{
    if ((data == NULL) || (length == 0U) ||
        !efuse_range_valid(address, length)) {
        return -1;
    }
    memcpy(g_efuse_words, data, length);
    bflb_ef_ctrl_write_direct(NULL, address, g_efuse_words,
                              length / sizeof(uint32_t), 1);
    bflb_ef_ctrl_read_direct(NULL, address, g_efuse_verify,
                             length / sizeof(uint32_t), 1);
    for (uint32_t index = 0U; index < length / sizeof(uint32_t); index++) {
        if ((g_efuse_verify[index] & g_efuse_words[index]) !=
            g_efuse_words[index]) {
            return -1;
        }
    }
    return 0;
}

int flashloader_port_write_mac(const uint8_t mac[6])
{
    uint8_t writable_mac[6];

    if (mac == NULL) {
        return -1;
    }
    memcpy(writable_mac, mac, sizeof(writable_mac));
    for (uint8_t slot = 0U; slot < 3U; slot++) {
        if (bflb_efuse_is_mac_address_slot_empty(slot, 1) != 0U) {
            return bflb_efuse_write_mac_address_opt(slot, writable_mac, 1);
        }
    }
    return -1;
}

uint32_t flashloader_port_crc32(const void *data, uint32_t length)
{
    return bflb_soft_crc32((void *)(uintptr_t)data, length);
}

uint64_t flashloader_port_time_ms(void)
{
    return bflb_mtimer_get_time_ms();
}
