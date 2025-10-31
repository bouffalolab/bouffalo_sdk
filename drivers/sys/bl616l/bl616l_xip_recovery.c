#include <assert.h>
#include "bflb_flash.h"
#include "bflb_sflash.h"
#include "bflb_sf_ctrl.h"
#include "bflb_sf_cfg.h"
#include "bl616l_clock.h"
#include "bl616l_xip_recovery.h"
#include "tzc_sec_reg.h"

ATTR_DTCM_SECTION sf_recovery_para_t g_flash_para;


static inline uint8_t Clock_Get_SF_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SF_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_SEL);
}

static inline uint8_t Clock_Get_SF_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SF_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_DIV);
}

static inline uint8_t Clock_Get_SF_Clk_Sel2_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SF_CFG0);

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
    uint8_t flash_clk, flash_clk_div;
    uint32_t flash_cfg_len;

    bflb_flash_get_cfg((uint8_t **)&g_flash_para.flash_cfg, &flash_cfg_len);
    g_flash_para.flash_jdec_id=bflb_flash_get_jedec_id();
    bl_lp_xip_get_flash_clock(&flash_clk, &flash_clk_div);
    g_flash_para.flash_clk = flash_clk;
    g_flash_para.flash_clk_div = flash_clk_div;
    g_flash_para.flash_offset = bflb_sf_ctrl_get_flash_image_offset(0, SF_CTRL_FLASH_BANK0);
    g_flash_para.flash_pin_cfg = (BL_RD_WORD(0x20056000 + 0x74) >> 5) & 0x3F;

    // printf("flash_offset: %d\r\n", g_flash_para.flash_offset);
    // printf("flash_jdec_id: %08x\r\n", g_flash_para.flash_jdec_id);
    // printf("flash_clk: %d\r\n", g_flash_para.flash_clk);
    // printf("flash_clk_div: %d\r\n", g_flash_para.flash_clk_div);
    // printf("flash_pin_cfg: %d\r\n", g_flash_para.flash_pin_cfg);

    g_flash_para.do_xip_recovery = 0;
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

    bflb_sflash_init(&sfCtrlCfg, NULL);
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

    bflb_sf_ctrl_set_flash_image_offset(image_offset, 0, SF_CTRL_FLASH_BANK0);
    bflb_sflash_xip_read_enable(pFlashCfg, pFlashCfg->io_mode & 0xf, cont_read, SF_CTRL_FLASH_BANK0);

    return 0;
}


void ATTR_TCM_SECTION bl_lp_xip_recovery(void)
{
    // uint32_t encrypted = 0;
    uint32_t jdec_id = 0;
    uint32_t timeout = 0;
    uint32_t tmp_val;
    uint32_t ret = 0;
    spi_flash_cfg_type *pFlashCfg = (spi_flash_cfg_type *)g_flash_para.flash_cfg;

    /* power on flash power */
    /* dcdc18 Always On */
    AON_LDO18_IO_Switch_Flash(1);

    /* init flash gpio */
    bflb_sf_cfg_init_flash_gpio(g_flash_para.flash_pin_cfg, 0);

    /* flash_clk 0:144M, 1:XCLK(RC32M or XTAL), 2:57.6M, 3:72M, 4:BCLK, 5:96M */
    /* set flash clock */
    GLB_Set_SF_CLK(1, g_flash_para.flash_clk, g_flash_para.flash_clk_div);

    /* update flash controller */
    bl_lp_set_sf_ctrl(pFlashCfg);

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
        /* Send 1 byte 0xff(4 line) to exit flash continue read in QPI mode */
        // bflb_spi_flash_send_qpi_exit();
        /* Exit form continous read for accepting command */
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

    } while (jdec_id != g_flash_para.flash_jdec_id);



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

    /* set burst wrap */
    if (((pFlashCfg->io_mode >> 4) & 0x01) == 1) {
        // L1C_Set_Wrap(DISABLE); //useless, reserved for bootrom architecture
    } else {
        // L1C_Set_Wrap(ENABLE); //useless, reserved for bootrom architecture
        /* For command that is setting register instead of send command, we need write enable */
        if ((pFlashCfg->io_mode & 0x0f) == SF_CTRL_QO_MODE || (pFlashCfg->io_mode & 0x0f) == SF_CTRL_QIO_MODE) {
            // bflb_sflash_qspi_enable(pFlashCfg);
            bflb_sflash_write_enable(pFlashCfg);
            bflb_sflash_set_burst_wrap(pFlashCfg);
        }
    }

    timeout = 0;
    do {
        timeout++;
        bflb_sflash_read(pFlashCfg, pFlashCfg->io_mode & 0xf, 0, 0x00000000, (uint8_t *)&tmp_val, 4);
        arch_delay_us(10);
    } while (timeout < 1000 && tmp_val != 0x504e4642); /* BFNP */

    /* protect bootrom */
    // tmpVal = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_ROM_CTRL);
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, TZC_SEC_TZC_SBOOT_DONE, val);
    // BL_WR_REG(TZC_SEC_BASE, TZC_SEC_TZC_ROM_CTRL, tmpVal);

    // encrypted = BL_RD_WORD(0x40007010);
    // encrypted = (encrypted >>21) & 0x1;
    // if (encrypted == 0) {
        bl_lp_xip_read_enable(pFlashCfg, 1 /* cont read*/, g_flash_para.flash_offset);
    // } else {
    //     bflb_bootrom_media_boot_set_encrypt();
        // bl_lp_xip_read_enable(pFlashCfg, 0 /* not cont read*/, g_flash_para.flash_offset);
    // }

    g_flash_para.do_xip_recovery = 1;

}