#include <assert.h>
#include "ef_data_reg.h"
#include "sf_ctrl_reg.h"
#include "tzc_sec_reg.h"
#include "bl702_romdriver.h"
#include "bflb_sec_sha.h"
#include "bflb_flash.h"
#include "bflb_sflash.h"
#include "bflb_sf_ctrl.h"
#include "bflb_sf_cfg.h"
#include "bl_lp.h"

static __inline uint8_t ATTR_CLOCK_SECTION Clock_Get_SF_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_SEL);
}

static __inline uint8_t ATTR_CLOCK_SECTION Clock_Get_SF_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_DIV);
}

static __inline uint8_t ATTR_CLOCK_SECTION Clock_Get_SF_Clk_Sel2_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_SEL2);
}


static void bl_lp_xip_get_flash_clock(uint8_t *flash_clk, uint8_t *flash_clk_div)
{
    uint8_t sel = Clock_Get_SF_Clk_Sel_Val();
    uint8_t sel2 = Clock_Get_SF_Clk_Sel2_Val();

    if (sel == 0) {
        /* sf sel2 */
        if (sel2 == 0) {
            /* 144m */
            *flash_clk = 0;
        } else if (sel2 == 1) {
            /* xtal */
            *flash_clk = 1;
        } else if (sel2 == 2) {
            /* 144m */
            *flash_clk = 0;
        } else {
            /* 57.6m */
            *flash_clk = 2;
        }
    } else if (sel == 1) {
        /* pll 72m */
        *flash_clk = 3;
    } else if (sel == 2) {
        /* BCLK */
        *flash_clk = 4;
    } else {
        /* 96m */
        *flash_clk = 5;
    }
    *flash_clk_div = Clock_Get_SF_Div_Val();
}

void bl_lp_xip_para_save(void)
{
#define SF_Ctrl_Get_AES_Region(addr, r) (addr + SF_CTRL_AES_REGION_OFFSET + (r) * 0x80)
    uint32_t tmpVal, i = 3;
    uint32_t regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, 0);
    uint8_t *iv = iot2lp_para->aesiv;
    uint32_t read_data = 0;
    uint8_t flash_clk, flash_clk_div;

    iot2lp_para->xts_mode = 0;

    tmpVal = SF_CTRL_SF_AES_IV_W0_OFFSET;
    while (i--) {
        read_data = getreg32(regionRegBase + tmpVal);
        memcpy(iv, &read_data, 4);
        iv += 4;
        tmpVal += 4;
    }

    tmpVal = getreg32(SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);
    iot2lp_para->encrypt_type = BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_MODE);

    bl_lp_xip_get_flash_clock(&flash_clk, &flash_clk_div);
    iot2lp_para->flash_clk = flash_clk;
    iot2lp_para->flash_clk_div = flash_clk_div;

    BL_LP_LOG("iot2lp_para->flash_clk: %d\r\n", iot2lp_para->flash_clk);
    BL_LP_LOG("iot2lp_para->flash_clk_div: %d\r\n", iot2lp_para->flash_clk_div);

#if 0
    uint8_t b_flash_cfg = 0;
    uint8_t b_psram_cfg = 0;
    uint8_t b_swap_cfg = 0;

    tmpVal = BL_RD_WORD(0x40007074);
    b_flash_cfg = ((tmpVal >> 26) & 7);
    b_psram_cfg = ((tmpVal >> 24) & 3);
    b_swap_cfg = ((tmpVal >> 22) & 3);
    BL_LP_LOG("b_flash_cfg: %d\r\n", b_flash_cfg);
    BL_LP_LOG("b_psram_cfg: %d\r\n", b_psram_cfg);
    BL_LP_LOG("b_swap_cfg: %d\r\n", b_swap_cfg);
#endif


    iot2lp_para->do_xip_recovery = 0;
}



static void ATTR_TCM_SECTION bl_lp_set_sf_ctrl(spi_flash_cfg_type *pFlashCfg)
{
    struct sf_ctrl_cfg_type sfCtrlCfg;

    sfCtrlCfg.owner = SF_CTRL_OWNER_SAHB;

#ifdef BFLB_SF_CTRL_32BITS_ADDR_ENABLE
    /* address 32bits */
    sfCtrlCfg.en32b_addr = 0;
#endif
    /* bit0-3 for clk delay */
    sfCtrlCfg.clk_delay = (pFlashCfg->clk_delay & 0x0f);
    /* bit0 for clk invert */
    sfCtrlCfg.clk_invert = pFlashCfg->clk_invert & 0x01;
    /* bit1 for rx clk invert */
    sfCtrlCfg.rx_clk_invert = (pFlashCfg->clk_invert >> 1) & 0x01;
    /* bit4-6 for do delay */
    sfCtrlCfg.do_delay = 0;
    /* bit2-4 for di delay */
    sfCtrlCfg.di_delay = 0;
    /* bit5-7 for oe delay */
    sfCtrlCfg.oe_delay = 0;

    bflb_sflash_init(&sfCtrlCfg);
}

static int ATTR_TCM_SECTION bl_lp_xip_read_enable(spi_flash_cfg_type *pFlashCfg, uint8_t cont_read, uint32_t image_offset)
{
    uint32_t tmp[1];

    if ((pFlashCfg->c_read_support & 0x01) == 0) {
        cont_read = 0;
    }

    if (cont_read == 1) {
        bflb_sf_ctrl_set_owner(SF_CTRL_OWNER_SAHB);
        bflb_sflash_read(pFlashCfg, pFlashCfg->io_mode & 0xf, 1, 0x00000000, (uint8_t *)tmp, sizeof(tmp));
    }

    bflb_sf_ctrl_set_flash_image_offset(image_offset, 0, SF_CTRL_SEL_FLASH);
    bflb_sflash_xip_read_enable(pFlashCfg, pFlashCfg->io_mode & 0xf, cont_read, SF_CTRL_SEL_FLASH);

    return 0;
}

static void ATTR_TCM_SECTION bflb_bootrom_media_boot_set_encrypt(void)
{
    uint32_t encrypt_type = iot2lp_para->encrypt_type; /* the origin val */
    uint32_t img_offset = iot2lp_para->flash_offset;
    uint32_t img_len = iot2lp_para->img_len;


    bflb_sf_ctrl_disable_wrap_access(1);
    bflb_sf_ctrl_aes_set_mode(SF_CTRL_AES_CTR_MODE);
    bflb_sf_ctrl_aes_set_key_be(0, NULL, (uint8_t)(encrypt_type));
    bflb_sf_ctrl_aes_set_iv_be(0, iot2lp_para->aesiv, img_offset);

    bflb_sf_ctrl_aes_set_region(0, 1 /*enable this region*/, 1 /*hardware key*/,
                                img_offset,
                                img_offset + img_len - 1,
                                1 /*lock*/);

    bflb_sf_ctrl_aes_enable_be();
    bflb_sf_ctrl_aes_enable();
}

// static void bl_lp_set_power(void)
// {
//     AON_Trim_DcdcDis();
//     HBN_Trim_Ldo33VoutTrim();
//     AON_Trim_DcdcVoutSel();
//     AON_Trim_DcdcVoutTrim();
//     AON_Trim_Ldo11socVoutTrim();
// }

void ATTR_TCM_SECTION bl_lp_flash_set_cmds(spi_flash_cfg_type *p_flash_cfg)
{
    struct sf_ctrl_cmds_cfg cmds_cfg;

    cmds_cfg.cmds_en = 1;
    cmds_cfg.burst_toggle_en = 1;
    cmds_cfg.cmds_wrap_mode = 1;
    cmds_cfg.cmds_wrap_len = 9;

    if ((p_flash_cfg->io_mode & 0x1f) == SF_CTRL_QIO_MODE) {
        cmds_cfg.cmds_wrap_mode = 2;
        cmds_cfg.cmds_wrap_len = 2;
    }
    bflb_sf_ctrl_cmds_set(&cmds_cfg, 0);
}

static void ATTR_TCM_SECTION bflb_bootrom_sboot_set(uint8_t val)
{
    uint32_t tmpVal;
    tmpVal = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_ROM_CTRL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, TZC_SEC_TZC_SBOOT_DONE, val);
    BL_WR_REG(TZC_SEC_BASE, TZC_SEC_TZC_ROM_CTRL, tmpVal);
}

void ATTR_TCM_SECTION sf_io_select(void)
{
    uint32_t tmpVal = 0;
    uint8_t flashCfg = 0;
    uint8_t psramCfg = 0;
    uint8_t isInternalFlash = 0;
    uint8_t isInternalPsram = 0;

    /* SF io select from efuse value */
    tmpVal = BL_RD_WORD(0x40007074);
    flashCfg = ((tmpVal >> 26) & 7);
    psramCfg = ((tmpVal >> 24) & 3);
    if (flashCfg == 1 || flashCfg == 2) {
        isInternalFlash = 1;
    } else {
        isInternalFlash = 0;
    }
    if (psramCfg == 1) {
        isInternalPsram = 1;
    } else {
        isInternalPsram = 0;
    }
    tmpVal = BL_RD_REG(GLB_BASE, GLB_GPIO_USE_PSRAM__IO);
    if (isInternalFlash == 1 && isInternalPsram == 0) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CFG_GPIO_USE_PSRAM_IO, 0x3f);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CFG_GPIO_USE_PSRAM_IO, 0x00);
    }
    BL_WR_REG(GLB_BASE, GLB_GPIO_USE_PSRAM__IO, tmpVal);
}

int ATTR_TCM_SECTION bl_lp_sf_cfg_init_ext_flash_gpio(uint8_t ext_flash_pin)
{
    GLB_GPIO_Type flash_pin[6]={23,24,25,26,27,28};

    if (ext_flash_pin == 0) {
        flash_pin[0] = BFLB_EXTFLASH_CLK0_GPIO;
        flash_pin[1] = BFLB_EXTFLASH_CS0_GPIO;
        flash_pin[2] = BFLB_EXTFLASH_DATA00_GPIO;
        flash_pin[3] = BFLB_EXTFLASH_DATA10_GPIO;
        flash_pin[4] = BFLB_EXTFLASH_DATA20_GPIO;
        flash_pin[5] = BFLB_EXTFLASH_DATA30_GPIO;
    } else if (ext_flash_pin == 1) {
        flash_pin[0] = BFLB_EXTFLASH_CLK1_GPIO;
        flash_pin[1] = BFLB_EXTFLASH_CS1_GPIO;
        flash_pin[2] = BFLB_EXTFLASH_DATA01_GPIO;
        flash_pin[3] = BFLB_EXTFLASH_DATA11_GPIO;
        flash_pin[4] = BFLB_EXTFLASH_DATA21_GPIO;
        flash_pin[5] = BFLB_EXTFLASH_DATA31_GPIO;
    } else if (ext_flash_pin == 2) {
        flash_pin[0] = BFLB_EXTFLASH_CLK2_GPIO;
        flash_pin[1] = BFLB_EXTFLASH_CS2_GPIO;
        flash_pin[2] = BFLB_EXTFLASH_DATA02_GPIO;
        flash_pin[3] = BFLB_EXTFLASH_DATA12_GPIO;
        flash_pin[4] = BFLB_EXTFLASH_DATA22_GPIO;
        flash_pin[5] = BFLB_EXTFLASH_DATA32_GPIO;
    } else {
        return -1;
    }

    GLB_GPIO_Cfg_Type gpioCfg = {
        .gpioPin = GLB_GPIO_PIN_0,
        .gpioFun = (uint8_t)GPIO_FUN_FLASH_PSRAM,
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_UP,
        .drive = 1,
        .smtCtrl = 1
    };

    for (uint8_t i = 0; i < 6; i++) {
        gpioCfg.gpioPin = flash_pin[i];
        GLB_GPIO_Init(&gpioCfg);
    }

    return 0;
}

__WEAK int ATTR_TCM_SECTION bl_lp_sf_cfg_init_flash_gpio(uint8_t flash_pin_cfg)
{
#define GLB_BASE_ADDR                      0x40000000
#define GLB_PARM_OFFSET_ADDR               0x80
#define GLB_GPIO_USE_PSRAM__IO_OFFSET_ADDR 0x88
    uint8_t flash_cfg;
    uint8_t swap_cfg;
    uint32_t regval;

    flash_cfg = (flash_pin_cfg >> 2) & 0x07;
    swap_cfg = flash_pin_cfg & 0x03;

    if (flash_cfg == BFLB_FLASH_CFG_SF1_EXT_17_22) {
        bl_lp_sf_cfg_init_ext_flash_gpio(0);
        bflb_sf_ctrl_select_pad(SF_CTRL_SEL_SF1);
    } else {
        regval = getreg32(GLB_BASE_ADDR + GLB_GPIO_USE_PSRAM__IO_OFFSET_ADDR);

        /* Bit0~5 is sf io select */
        if ((regval & 0x3F) == 0x00) {
            bl_lp_sf_cfg_init_ext_flash_gpio(1);
        }

        regval = getreg32(GLB_BASE_ADDR + GLB_PARM_OFFSET_ADDR);
        if (swap_cfg == BFLB_SF2_SWAP_NONE) {
            /* Bit9 is flash2 swap cs with io2 */
            regval &= ~(1 << 9);
            /* Bit8 is flash2 swap io0 with io3 */
            regval &= ~(1 << 8);
        } else if (swap_cfg == BFLB_SF2_SWAP_CS_IO2) {
            /* Bit9 is flash2 swap cs with io2 */
            regval |= (1 << 9);
            /* Bit8 is flash2 swap io0 with io3 */
            regval &= ~(1 << 8);
        } else if (swap_cfg == BFLB_SF2_SWAP_IO0_IO3) {
            /* Bit9 is flash2 swap cs with io2 */
            regval &= ~(1 << 9);
            /* Bit8 is flash2 swap io0 with io3 */
            regval |= (1 << 8);
        } else {
            /* Bit9 is flash2 swap cs with io2 */
            regval |= (1 << 9);
            /* Bit8 is flash2 swap io0 with io3 */
            regval |= (1 << 8);
        }
        putreg32(regval, GLB_BASE_ADDR + GLB_PARM_OFFSET_ADDR);

        bflb_sf_ctrl_select_pad(SF_CTRL_SEL_SF2);
    }

    return 0;

}

void ATTR_TCM_SECTION bl_lp_xip_recovery(void)
{
    uint32_t encrypted = 0;
    uint32_t jdec_id = 0;
    uint32_t timeout = 0;
    uint32_t tmp_val;
    uint32_t ret = 0;
    uint32_t tmpVal = 0;
    uint8_t flash_pin = 0;
    uint8_t b_flash_cfg = 0;
    uint8_t b_swap_cfg = 0;
    spi_flash_cfg_type *pFlashCfg = (spi_flash_cfg_type *)iot2lp_para->flash_cfg;

    bl_lp_debug_record_time(iot2lp_para, "xip flash start");

    /* power on flash power */
    /* dcdc18 Always On */

    /* get flash gpio info */
    tmpVal = BL_RD_WORD(0x40007074);
    b_flash_cfg = ((tmpVal >> 26) & 7);
    // b_psram_cfg = ((tmpVal >> 24) & 3);
    b_swap_cfg = ((tmpVal >> 22) & 3);

    /* init flash gpio */
    flash_pin = ((b_flash_cfg << 2) | b_swap_cfg);
    // RomDriver_SF_Cfg_Init_Flash_Gpio((uint8_t)flash_pin, 0);
    // bflb_sf_cfg_init_flash_gpio((uint8_t)flash_pin, 0);
    bl_lp_sf_cfg_init_flash_gpio((uint8_t)flash_pin);


    /* flash_clk 0:144M, 1:XCLK(RC32M or XTAL), 2:57.6M, 3:72M, 4:BCLK, 5:96M */
    /* set flash clock */
    GLB_Set_SF_CLK(1, iot2lp_para->flash_clk, iot2lp_para->flash_clk_div);

    /* update flash controller */
    bl_lp_set_sf_ctrl(pFlashCfg);

    /* set flash cmds */
    // bl_lp_flash_set_cmds(pFlashCfg);
    /* do flash recovery */
    bflb_sflash_release_powerdown(pFlashCfg);
    arch_delay_us(120);
    timeout = 0;
    do {
        timeout++;
        if (timeout > 2000) {
            break;
        } else if (timeout > 0) {
            arch_delay_us(10);
        }
        bflb_sflash_reset_continue_read(pFlashCfg);
        /* Send software reset command(80bv has no this command)to deburst wrap for ISSI like */
        // bflb_sflash_software_reset(pFlashCfg);
        /* Disable burst may be removed(except for 80BV) and only work with winbond,but just for make sure */
        bflb_sflash_write_enable(pFlashCfg);
        /* For disable command that is setting register instaed of send command, we need write enable */
        /* Delete this line before the other flash_2_wires glitch */
        // bflb_sflash_disable_burst_wrap(pFlashCfg);

        bflb_sflash_set_spi_mode(SF_CTRL_SPI_MODE);
        bflb_sflash_get_jedecid((spi_flash_cfg_type *)pFlashCfg, (uint8_t *)&jdec_id);
        /* Dummy disable burstwrap for make sure */
        bflb_sflash_write_enable(pFlashCfg);

        /* Delete this line before the other flash_2_wires glitch */
        // bflb_sflash_disable_burst_wrap(pFlashCfg);

    } while (jdec_id != iot2lp_para->flash_jdec_id);

    bl_lp_debug_record_time(iot2lp_para, "read flash jdec_id");

    timeout = 0;
    do {
        timeout++;
        if (timeout > 50000) {
            break;
        } else if (timeout > 0) {
            arch_delay_us(10);
        }
        ret = bflb_sflash_busy(pFlashCfg);
    } while (1 == ret);


#if 0
    /* Enable QE again */
    if ((pFlashCfg->io_mode & 0x0f) == SF_CTRL_QO_MODE || (pFlashCfg->io_mode & 0x0f) == SF_CTRL_QIO_MODE) {
        SFlash_Qspi_Enable(pFlashCfg);
    }
#endif
    /* set burst wrap */
    if (((pFlashCfg->io_mode >> 4) & 0x01) == 1) {
        L1C_Set_Wrap(DISABLE); //useless, reserved for bootrom architecture
    } else {
        L1C_Set_Wrap(ENABLE); //useless, reserved for bootrom architecture
        /* For command that is setting register instead of send command, we need write enable */
        bflb_sflash_write_enable(pFlashCfg);
        if ((pFlashCfg->io_mode & 0x0f) == SF_CTRL_QO_MODE || (pFlashCfg->io_mode & 0x0f) == SF_CTRL_QIO_MODE) {
            bflb_sflash_set_burst_wrap(pFlashCfg);
        }
    }

    timeout = 0;
    do {
        timeout++;
        bflb_sflash_read(pFlashCfg, pFlashCfg->io_mode & 0xf, 0, 0x00000000, (uint8_t *)&tmp_val, 4);
        arch_delay_us(10);
    } while (timeout < 1000 && tmp_val != 0x504e4642);
    bflb_bootrom_sboot_set(0xf);

    encrypted = BL_RD_WORD(0x40007010);
    encrypted = (encrypted >>21) & 0x1;
    if (encrypted == 0) {
        bl_lp_xip_read_enable(pFlashCfg, 1 /* cont read*/, iot2lp_para->flash_offset);
    } else {
        bflb_bootrom_media_boot_set_encrypt();
        bl_lp_xip_read_enable(pFlashCfg, 0 /* not cont read*/, iot2lp_para->flash_offset);
    }

    /* set read mode */
    if ((pFlashCfg->io_mode & 0x0f) == SF_CTRL_QO_MODE || (pFlashCfg->io_mode & 0x0f) == SF_CTRL_QIO_MODE) {
        bflb_sflash_qspi_enable(pFlashCfg);
    }

    iot2lp_para->wakeup_flag = 1;

    bl_lp_debug_record_time(iot2lp_para, "xip flash end");

    void (*pFunc)(uint32_t);
    pFunc = (void (*)(uint32_t))iot2lp_para->app_entry;
    pFunc(iot2lp_para->args[0]);
}

