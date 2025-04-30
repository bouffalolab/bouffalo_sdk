
#if __has_include("board_flash_psram_overlay.h")
/* Use board_flash_psram_overlay.c instead of this file */
#else

#include "bflb_flash.h"
#include "bflb_xip_sflash.h"
#include "bflb_sf_ctrl.h"
#include "bflb_efuse.h"
#include "bflb_clock.h"

#include "bl616_glb.h"
#include "bl616_psram.h"
#include "bl616_tzc_sec.h"

#ifndef CONFIG_BOARD_FLASH_LOW_SPEED
typedef struct {
    uint8_t flash_clock;
    uint8_t flash_clock_div;
    uint8_t delay_value;
    uint32_t sdmin;
} flash_clock_para_t;

flash_clock_para_t flash_clock_para_list[] = {
    [0].flash_clock = GLB_SFLASH_CLK_WIFIPLL_120M,
    [0].flash_clock_div = 3,
    [0].sdmin = 23320330,
    [0].delay_value = 180,

    [1].flash_clock = GLB_SFLASH_CLK_WIFIPLL_120M,
    [1].flash_clock_div = 3,
    [1].sdmin = 23991419,
    [1].delay_value = 175,

    [2].flash_clock = GLB_SFLASH_CLK_WIFIPLL_120M,
    [2].flash_clock_div = 3,
    [2].sdmin = 24662508,
    [2].delay_value = 170,

    [3].flash_clock = GLB_SFLASH_CLK_WIFIPLL_120M,
    [3].flash_clock_div = 3,
    [3].sdmin = 25231360,
    [3].delay_value = 165,

    [4].flash_clock = GLB_SFLASH_CLK_WIFIPLL_120M,
    [4].flash_clock_div = 3,
    [4].sdmin = 26282666,
    [4].delay_value = 160,

    [5].flash_clock = GLB_SFLASH_CLK_WIFIPLL_120M,
    [5].flash_clock_div = 3,
    [5].sdmin = 27123712,
    [5].delay_value = 155,

    [6].flash_clock = GLB_SFLASH_CLK_WIFIPLL_120M,
    [6].flash_clock_div = 3,
    [6].sdmin = 27961852,
    [6].delay_value = 150,

    [7].flash_clock = GLB_SFLASH_CLK_MUXPLL_80M,
    [7].flash_clock_div = 1,
    [7].sdmin = 21705523,
    [7].delay_value = 145,

    [8].flash_clock = GLB_SFLASH_CLK_MUXPLL_80M,
    [8].flash_clock_div = 1,
    [8].sdmin = 22465741,
    [8].delay_value = 140,

    [9].flash_clock = GLB_SFLASH_CLK_MUXPLL_80M,
    [9].flash_clock_div = 1,
    [9].sdmin = 23278387,
    [9].delay_value = 135,

    [10].flash_clock = GLB_SFLASH_CLK_MUXPLL_80M,
    [10].flash_clock_div = 1,
    [10].sdmin = 24222106,
    [10].delay_value = 130,

    [11].flash_clock = GLB_SFLASH_CLK_MUXPLL_80M,
    [11].flash_clock_div = 1,
    [11].sdmin = 25165824,
    [11].delay_value = 125,

    [12].flash_clock = GLB_SFLASH_CLK_MUXPLL_80M,
    [12].flash_clock = 1,
    [12].sdmin = 26214400,
    [12].delay_value = 120,

    [13].flash_clock = GLB_SFLASH_CLK_MUXPLL_80M,
    [13].flash_clock_div = 1,
    [13].sdmin = 27341619,
    [13].delay_value = 115,

    [14].flash_clock = GLB_SFLASH_CLK_WIFIPLL_96M,
    [14].flash_clock_div = 1,
    [14].sdmin = 23890944,
    [14].delay_value = 110,

    [15].flash_clock = GLB_SFLASH_CLK_WIFIPLL_96M,
    [15].flash_clock_div = 1,
    [15].sdmin = 25031611,
    [15].delay_value = 105,

    [16].flash_clock = GLB_SFLASH_CLK_WIFIPLL_96M,
    [16].flash_clock_div = 1,
    [16].sdmin = 26282666,
    [16].delay_value = 100,

    [17].flash_clock = GLB_SFLASH_CLK_WIFIPLL_96M,
    [17].flash_clock_div = 1,
    [17].sdmin = 27665134,
    [17].delay_value = 95,

    [18].flash_clock = GLB_SFLASH_CLK_WIFIPLL_120M,
    [18].flash_clock_div = 1,
    [18].sdmin = 23364239,
    [18].delay_value = 90,

    [19].flash_clock = GLB_SFLASH_CLK_WIFIPLL_120M,
    [19].flash_clock_div = 1,
    [19].sdmin = 24735143,
    [19].delay_value = 85,

    [20].flash_clock = GLB_SFLASH_CLK_WIFIPLL_120M,
    [20].flash_clock_div = 1,
    [20].sdmin = 26282666,
    [20].delay_value = 80,

    [21].flash_clock = GLB_SFLASH_CLK_WIFIPLL_120M,
    [21].flash_clock_div = 1,
    [21].sdmin = 28032040,
    [21].delay_value = 75,

    [22].flash_clock = GLB_SFLASH_CLK_MUXPLL_80M,
    [22].flash_clock_div = 0,
    [22].sdmin = 22525296,
    [22].delay_value = 70,

    [23].flash_clock = GLB_SFLASH_CLK_MUXPLL_80M,
    [23].flash_clock_div = 0,
    [23].sdmin = 24259952,
    [23].delay_value = 65,
};
#endif

static uint8_t flash_org_delay = 0;
static uint8_t flash_final_delay = 0;
static uint8_t flash_added_delay = 0;

#ifndef CONFIG_BOARD_FLASH_LOW_SPEED
static void ATTR_TCM_SECTION clk_pll_set(uint32_t sdmin)
{
    uint32_t regval = 0;

    regval = BL_RD_WORD(0x20000828);
    regval = regval & 0xFC000000;
    regval = regval | sdmin;
    BL_WR_WORD(0x20000828, regval);

    GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_RC32M);
    GLB_Set_MCU_System_CLK_Div(0, 0);

    /* toggle sdm_reset (pulse 0 > 1us) */
    /* cfg0 : wifipll_sdm_reset */
    regval = BL_RD_WORD(0x20000810 + 4 * 0);
    regval = BL_SET_REG_BITS_VAL(regval, GLB_WIFIPLL_SDM_RSTB, 0);
    regval = BL_SET_REG_BITS_VAL(regval, GLB_WIFIPLL_FBDV_RSTB, 0);
    BL_WR_WORD(0x20000810 + 4 * 0, regval);

    arch_delay_us(2);

    regval = BL_RD_WORD(0x20000810 + 4 * 0);
    regval = BL_SET_REG_BITS_VAL(regval, GLB_WIFIPLL_SDM_RSTB, 1);
    BL_WR_WORD(0x20000810 + 4 * 0, regval);

    arch_delay_us(2);

    regval = BL_RD_WORD(0x20000810 + 4 * 0);
    regval = BL_SET_REG_BITS_VAL(regval, GLB_WIFIPLL_FBDV_RSTB, 1);
    BL_WR_WORD(0x20000810 + 4 * 0, regval);

    arch_delay_us(40);

    GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M);
    GLB_Set_MCU_System_CLK_Div(0, 3);
}

static int ATTR_TCM_SECTION flash_check_bootheader(spi_flash_cfg_type *pflash_cfg, int index)
{
    uint8_t read_data[256];
    uint32_t stat = 0;
    uint32_t crc32 = 0;
    uint8_t *pcrc32 = read_data + 252;

    stat = bflb_sflash_read(pflash_cfg, SF_CTRL_QIO_MODE, 0, 0x00000000, read_data, sizeof(read_data));
    if (0 != stat) {
        return -1;
    }

    crc32 = bflb_soft_crc32(read_data, 252);
    if (arch_memcmp(&crc32, pcrc32, 4) != 0) {
        return -1;
    }
    return 0;
}

static int ATTR_TCM_SECTION board_get_flash_delay(spi_flash_cfg_type *p_flash_cfg)
{
    uint8_t success_sts = 0;
    uint8_t cal_delay = 0;
    uint32_t regval;
    uint32_t pre_sdmin;

    /* save register changed by calibration */
    regval = BL_RD_WORD(0x20000828);
    pre_sdmin = regval & 0x3ffffff;

    for (int index = 0; index < sizeof(flash_clock_para_list) / sizeof(flash_clock_para_list[0]); index++) {
        GLB_Set_SF_CLK(1, flash_clock_para_list[index].flash_clock, flash_clock_para_list[index].flash_clock_div);
        clk_pll_set(flash_clock_para_list[index].sdmin);
        if (flash_check_bootheader(p_flash_cfg, index) != 0) {
            /* fail */
            if (success_sts == 1) {
                success_sts = 2;
                cal_delay = flash_clock_para_list[index].delay_value;
                break;
            }
        } else {
            /* success  */
            if (success_sts == 0) {
                success_sts = 1;
            }
        }
    }
    clk_pll_set(pre_sdmin);

    return cal_delay;
}

void ATTR_CLOCK_SECTION __attribute__((noinline)) board_set_flash_80m(void)
{
    uint8_t is_aes_enable = 0;
    uint32_t offset = 0;
    uintptr_t flag;
    struct sf_ctrl_cfg_type sf_ctrl_cfg = { 0 };
    spi_flash_cfg_type *p_flash_cfg;
    uint32_t flash_cfg_len = 0;
    uint32_t sf_ctrl_0, sf_ctrl_1, sf_clk_cfg;
    uint32_t config_change = 0;
    uint32_t delay_found = 0;

    sf_ctrl_cfg.owner = SF_CTRL_OWNER_SAHB;
    sf_ctrl_cfg.en32b_addr = 0;
    sf_ctrl_cfg.clk_delay = 0;
    sf_ctrl_cfg.clk_invert = 1;
    sf_ctrl_cfg.rx_clk_invert = 1;
    sf_ctrl_cfg.do_delay = 0;
    sf_ctrl_cfg.di_delay = 0;
    sf_ctrl_cfg.oe_delay = 0;
    /* disable interrupt */
    flag = bflb_irq_save();
    /* save register changed by calibration */
    sf_ctrl_0 = BL_RD_WORD(0x2000b000);
    sf_ctrl_1 = BL_RD_WORD(0x2000b004);
    sf_clk_cfg = BL_RD_REG(GLB_BASE, GLB_SF_CFG0);

    bflb_flash_get_cfg((uint8_t **)&p_flash_cfg, &flash_cfg_len);
    bflb_xip_sflash_opt_enter(&is_aes_enable);
    bflb_xip_sflash_state_save(p_flash_cfg, &offset, 0, 0);

    bflb_sflash_init(&sf_ctrl_cfg, NULL);
    bflb_sflash_reset_continue_read(p_flash_cfg);

    /* cal delay value */
    flash_org_delay = board_get_flash_delay(p_flash_cfg);
    flash_final_delay = flash_org_delay;

    if (0 != flash_org_delay) {
        if (flash_org_delay >= 120 && flash_org_delay <= 155) {
            /* flash can run @ 80M @1.5T */
            sf_ctrl_cfg.clk_delay = 1;
            sf_ctrl_cfg.clk_invert = 1;
            sf_ctrl_cfg.rx_clk_invert = 1;
            bflb_sflash_init(&sf_ctrl_cfg, NULL);
            GLB_Set_SF_CLK(1, GLB_SFLASH_CLK_MUXPLL_80M, 0);
            config_change = 1;
        } else if (flash_org_delay >= 50 && flash_org_delay <= 70) {
            /* flash can run @ 80M @1T */
            sf_ctrl_cfg.clk_delay = 1;
            sf_ctrl_cfg.clk_invert = 1;
            sf_ctrl_cfg.rx_clk_invert = 0;
            bflb_sflash_init(&sf_ctrl_cfg, NULL);
            GLB_Set_SF_CLK(1, GLB_SFLASH_CLK_MUXPLL_80M, 0);
            config_change = 1;
        } else if (flash_org_delay > 70 && flash_org_delay < 120) {
            /* flash can run @ 80M @1.5T with added delay */
            flash_added_delay = 1;
            while (1) {
                if (delay_found) {
                    /* step2:set 1.5T */
                    sf_ctrl_cfg.clk_delay = 1;
                    sf_ctrl_cfg.clk_invert = 1;
                    sf_ctrl_cfg.rx_clk_invert = 1;
                    bflb_sflash_init(&sf_ctrl_cfg, NULL);
                    GLB_Set_SF_CLK(1, GLB_SFLASH_CLK_MUXPLL_80M, 0);
                    flash_added_delay--;
                }
                if (flash_added_delay > 6) {
                    flash_final_delay = flash_org_delay;
                    flash_added_delay = 0;
                    break;
                }

                /* step3:set added delay */
                if (flash_added_delay <= 3) {
                    /* add ~3 ns delay */
                    bflb_sf_ctrl_set_io_delay(0, 0, flash_added_delay & 0x03, 0);
                    bflb_sf_ctrl_set_io_delay(1, 0, flash_added_delay & 0x03, 0);
                    bflb_sf_ctrl_set_io_delay(2, 0, flash_added_delay & 0x03, 0);
                } else {
                    /* add ~3 ns delay */
                    bflb_sf_ctrl_set_io_delay(0, 0, 3, 0);
                    bflb_sf_ctrl_set_io_delay(1, 0, 3, 0);
                    bflb_sf_ctrl_set_io_delay(2, 0, 3, 0);
                    bflb_sf_ctrl_set_cs_clk_delay(0, 0, (flash_added_delay - 3) & 0x03);
                    bflb_sf_ctrl_set_cs_clk_delay(1, 0, (flash_added_delay - 3) & 0x03);
                    bflb_sf_ctrl_set_cs_clk_delay(2, 0, (flash_added_delay - 3) & 0x03);
                }

                /* step4:break */
                if (delay_found) {
                    config_change = 1;
                    break;
                }

                flash_final_delay = board_get_flash_delay(p_flash_cfg);
                /* step1: delay found */
                if (flash_final_delay >= 125) {
                    delay_found = 1;
                } else if (flash_final_delay >= 110 && flash_added_delay == 6) {
                    delay_found = 1;
                }
                flash_added_delay++;
            }
        }
    }

    if (config_change == 0) {
        /* restore flash config */
        BL_WR_WORD(0x2000b000, sf_ctrl_0);
        BL_WR_WORD(0x2000b004, sf_ctrl_1);
        BL_WR_REG(GLB_BASE, GLB_SF_CFG0, sf_clk_cfg);
    } else {
        p_flash_cfg->clk_delay = sf_ctrl_cfg.clk_delay;
        p_flash_cfg->clk_invert = sf_ctrl_cfg.clk_invert | (sf_ctrl_cfg.rx_clk_invert << 1);
    }

    /* to makesure bus interface is system bus */
    bflb_sf_ctrl_set_owner(SF_CTRL_OWNER_SAHB);
    bflb_xip_sflash_state_restore(p_flash_cfg, offset, 0, 0);
    bflb_xip_sflash_opt_exit(is_aes_enable);
    bflb_irq_restore(flag);
}
#endif

void bl_show_flashinfo(void)
{
    spi_flash_cfg_type flashCfg;
    uint8_t *pFlashCfg = NULL;
    uint32_t flashSize = 0;
    uint32_t flashCfgLen = 0;
    uint32_t flashJedecId = 0;

    flashJedecId = bflb_flash_get_jedec_id();
    flashSize = bflb_flash_get_size();
    bflb_flash_get_cfg((uint8_t **)&pFlashCfg, &flashCfgLen);
    arch_memcpy((void *)&flashCfg, pFlashCfg, flashCfgLen);
    printf("======== flash cfg ========\r\n");
    printf("flash clock      %dMHz\r\n", bflb_clk_get_peripheral_clock(BFLB_DEVICE_TYPE_FLASH, 0) / 1000000);
    printf("flash delay      %d,%d,%d\r\n", flash_org_delay, flash_final_delay, flash_added_delay);
    printf("flash size       0x%08X\r\n", flashSize);
    printf("jedec id         0x%06X\r\n", flashJedecId);
    printf("mid              0x%02X\r\n", flashCfg.mid);
    printf("iomode           0x%02X\r\n", flashCfg.io_mode);
    printf("clk delay        0x%02X\r\n", flashCfg.clk_delay);
    printf("clk invert       0x%02X\r\n", flashCfg.clk_invert);
    printf("read reg cmd0    0x%02X\r\n", flashCfg.read_reg_cmd[0]);
    printf("read reg cmd1    0x%02X\r\n", flashCfg.read_reg_cmd[1]);
    printf("write reg cmd0   0x%02X\r\n", flashCfg.write_reg_cmd[0]);
    printf("write reg cmd1   0x%02X\r\n", flashCfg.write_reg_cmd[1]);
    printf("qe write len     0x%02X\r\n", flashCfg.qe_write_reg_len);
    printf("cread support    0x%02X\r\n", flashCfg.c_read_support);
    printf("cread code       0x%02X\r\n", flashCfg.c_read_mode);
    printf("burst wrap cmd   0x%02X\r\n", flashCfg.burst_wrap_cmd);
    printf("===========================\r\n");
}

static void bflb_init_psram_gpio(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    for (uint8_t i = 0; i < 12; i++) {
        bflb_gpio_init(gpio, (41 + i), GPIO_INPUT | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_0);
    }
}

static void psram_winbond_default_init(uint8_t psram_info)
{
    bflb_ef_ctrl_com_trim_t trim;
    uint16_t dqs_val[] = {
        0x8000,
        0xC000,
        0xE000,
        0xF000,
        0xF800,
        0xFC00,
        0xFE00,
        0xFF00,
        0xFF80,
        0xFFC0,
        0xFFE0,
        0xFFF0,
        0xFFF8,
        0xFFFC,
        0xFFFE,
        0xFFFF,
    };
    int32_t left_flag = 0, right_flag = 0, c_val = 0;
    PSRAM_Ctrl_Cfg_Type default_psram_ctrl_cfg = {
        .vendor = PSRAM_CTRL_VENDOR_WINBOND,
        .ioMode = PSRAM_CTRL_X8_MODE,
        .size = PSRAM_SIZE_4MB,
        .dqs_delay = 0xfff0,
    };

    PSRAM_Winbond_Cfg_Type default_winbond_cfg = {
        .rst = DISABLE,
        .clockType = PSRAM_CLOCK_DIFF,
        .inputPowerDownMode = DISABLE,
        .hybridSleepMode = DISABLE,
        .linear_dis = ENABLE,
        .PASR = PSRAM_PARTIAL_REFRESH_FULL,
        .disDeepPowerDownMode = ENABLE,
        .fixedLatency = DISABLE,
        .burstLen = PSRAM_WINBOND_BURST_LENGTH_64_BYTES,
        .burstType = PSRAM_WRAPPED_BURST,
        .latency = PSRAM_WINBOND_6_CLOCKS_LATENCY,
        .driveStrength = PSRAM_WINBOND_DRIVE_STRENGTH_35_OHMS_FOR_4M,
    };

    bflb_ef_ctrl_read_common_trim(NULL, "psram", &trim, 1);
    if (trim.en) {
        if (trim.parity == bflb_ef_ctrl_get_trim_parity(trim.value, trim.len)) {
            left_flag = ((trim.value & (0xf0)) >> 0x4);
            right_flag = (trim.value & (0xf));
            c_val = ((left_flag + right_flag) >> 0x1);
            default_psram_ctrl_cfg.dqs_delay = dqs_val[c_val];
        } else {
            printf("\r\nPSRAM trim is corrupted\r\n");
        }
    } else {
        printf("\r\n!!!!!!PSRAM INIT WITHOUT PSRAM TRIM!!!!!!!!!!!!!!!!!!\r\n");
    }

    switch (psram_info) {
        case 1:
            default_psram_ctrl_cfg.size = PSRAM_SIZE_4MB;
            default_winbond_cfg.driveStrength = PSRAM_WINBOND_DRIVE_STRENGTH_35_OHMS_FOR_4M;
            break;
        case 2:
            default_psram_ctrl_cfg.size = PSRAM_SIZE_8MB;
            default_winbond_cfg.driveStrength = PSRAM_WINBOND_DRIVE_STRENGTH_25_OHMS_FOR_8M;
            break;
        case 3:
            default_psram_ctrl_cfg.size = PSRAM_SIZE_16MB;
            default_winbond_cfg.driveStrength = PSRAM_WINBOND_DRIVE_STRENGTH_25_OHMS_FOR_16M;
            break;
        default:
            break;
    }
    PSram_Ctrl_Init(PSRAM0_ID, &default_psram_ctrl_cfg);
    // PSram_Ctrl_Winbond_Reset(PSRAM0_ID);
    PSram_Ctrl_Winbond_Write_Reg(PSRAM0_ID, PSRAM_WINBOND_REG_CR0, &default_winbond_cfg);
}

uint32_t board_psram_x8_init(void)
{
    bflb_efuse_device_info_type device_info;
    uint16_t reg_read = 0;

    /* Check if the chip supports PSRAM */
    bflb_efuse_get_device_info(&device_info);
    if (device_info.psram_info == 0) {
        puts("This chip has no psram, please disable CONFIG_PSRAM\r\n");
        while (1) {}
    }

    /* Verify whether the PSRAM initialization has been completed on the chip */
    if (BL616_PSRAM_INIT_DONE) {
        puts("psram already init\r\n");
        return 0;
    }

    /* clk init */
    GLB_Set_PSRAMB_CLK_Sel(ENABLE, GLB_PSRAMB_EMI_WIFIPLL_320M, 0);
    /* gpio init */
    bflb_init_psram_gpio();

    /* psram init*/
    psram_winbond_default_init(device_info.psram_info);
    /* check psram work or not */
    PSram_Ctrl_Winbond_Read_Reg(PSRAM0_ID, PSRAM_WINBOND_REG_ID0, &reg_read);

    Tzc_Sec_PSRAMB_Access_Release();

    return reg_read;
}

#endif
