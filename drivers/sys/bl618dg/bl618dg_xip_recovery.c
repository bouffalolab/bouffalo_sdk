#include <assert.h>
#include "bflb_flash.h"
#include "bflb_sflash.h"
#include "bflb_sf_ctrl.h"
#include "bflb_sf_cfg.h"
#include "bl618dg_clock.h"
#include "bl618dg_pm.h"
#include "bl618dg_xip_recovery.h"
#include "tzc_sec_reg.h"
#include "bl618dg_lp.h"
#include "hardware/sf_ctrl_reg.h"

static ATTR_NOCACHE_RAM_SECTION sf_recovery_para_t g_flash_para = { 0 };
static ATTR_NOCACHE_RAM_SECTION lp_fw_sf_sec_t sf_sec_info = { 0 };
static ATTR_NOCACHE_RAM_SECTION lp_fw_tzc_t tzc_info = { 0 };

#define BL_LP_SF_AES_REGION(region) (SF_CTRL_BASE + SF_CTRL_AES_REGION_OFFSET + (region) * 0x80U)

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
        if ((sel2 == 0) || (sel2 == 2)) {
            /* 120m */
            *flash_clk = 0;
        } else if (sel2 == 1) {
            /* xtal */
            *flash_clk = 1;
        } else {
            /* sel2 == 3 */
            /* 48m */
            *flash_clk = 2;
        }
    } else if (sel == 1) {
        /* pll 80m */
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

static void bl_lp_tzc_para_save(void)
{
    iot2lp_para->tzc_cfg = &tzc_info;

    // tzc_info.tzc_ocram_tzsrg_ctrl = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_OCRAM_TZSRG_CTRL);
    // tzc_info.tzc_ocram_tzsrg_r0 = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_OCRAM_TZSRG_R0);
    // tzc_info.tzc_ocram_tzsrg_r1 = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_OCRAM_TZSRG_R1);
    // tzc_info.tzc_ocram_tzsrg_r2 = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_OCRAM_TZSRG_R2);

    // tzc_info.tzc_sf_tzsrg_ctrl = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_SF_TZSRG_CTRL);
    // tzc_info.tzc_sf_tzsrg_r0 = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_SF_TZSRG_R0);
    // tzc_info.tzc_sf_tzsrg_r1 = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_SF_TZSRG_R1);
    // tzc_info.tzc_sf_tzsrg_r2 = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_SF_TZSRG_R2);
    // tzc_info.tzc_sf_tzsrg_msb = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_SF_TZSRG_MSB);
}

static void bl_lp_sec_para_save(void)
{
    iot2lp_para->sec_cfg = &sf_sec_info;

    uint32_t tmpVal;
    uint8_t aes_enabled;
    uint32_t region_reg_base = BL_LP_SF_AES_REGION(0);
    uint32_t *iv = iot2lp_para->sec_cfg->r0_aes_iv;

    tmpVal = getreg32(SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);
    aes_enabled = !!(tmpVal & SF_CTRL_SF_AES_EN);

    /* Only snapshot active decryption regions; AES mode 0 is AES-128. */
    iot2lp_para->sec_cfg->xts_mode = !!(tmpVal & SF_CTRL_SF_AES_BLK_MODE);
    iot2lp_para->sec_cfg->encrypt_type = ((tmpVal & SF_CTRL_SF_AES_MODE_MASK) >> SF_CTRL_SF_AES_MODE_SHIFT);

    /* AES region 0 */
    region_reg_base = BL_LP_SF_AES_REGION(0);
    tmpVal = BL_RD_WORD(region_reg_base + SF_CTRL_SF_AES_START_OFFSET);
    iot2lp_para->sec_cfg->r0_aes_en = aes_enabled && !!(tmpVal & SF_CTRL_SF_AES_REGION_EN);
    iot2lp_para->sec_cfg->r0_aes_hw_key_en = !!(tmpVal & SF_CTRL_SF_AES_REGION_HW_KEY_EN);
    iot2lp_para->sec_cfg->r0_aes_lock = !!(tmpVal & SF_CTRL_SF_AES_REGION_LOCK);
    iot2lp_para->sec_cfg->r0_aes_start = ((tmpVal & SF_CTRL_SF_AES_REGION_START_MASK) >> SF_CTRL_SF_AES_REGION_START_SHIFT) * 1024;
    tmpVal = BL_RD_WORD(region_reg_base + SF_CTRL_SF_AES_END_OFFSET);
    iot2lp_para->sec_cfg->r0_aes_end = ((tmpVal & SF_CTRL_SF_AES_REGION_END_MASK) >> SF_CTRL_SF_AES_REGION_END_SHIFT) * 1024;

    iv = iot2lp_para->sec_cfg->r0_aes_iv;
    iv[0] = BL_RD_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W0_OFFSET);
    iv[1] = BL_RD_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W1_OFFSET);
    iv[2] = BL_RD_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W2_OFFSET);
    iv[3] = BL_RD_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W3_OFFSET);

    /* AES region 1 */
    region_reg_base = BL_LP_SF_AES_REGION(1);
    tmpVal = BL_RD_WORD(region_reg_base + SF_CTRL_SF_AES_START_OFFSET);
    iot2lp_para->sec_cfg->r1_aes_en = aes_enabled && !!(tmpVal & SF_CTRL_SF_AES_REGION_EN);
    iot2lp_para->sec_cfg->r1_aes_hw_key_en = !!(tmpVal & SF_CTRL_SF_AES_REGION_HW_KEY_EN);
    iot2lp_para->sec_cfg->r1_aes_lock = !!(tmpVal & SF_CTRL_SF_AES_REGION_LOCK);
    iot2lp_para->sec_cfg->r1_aes_start = ((tmpVal & SF_CTRL_SF_AES_REGION_START_MASK) >> SF_CTRL_SF_AES_REGION_START_SHIFT) * 1024;
    tmpVal = BL_RD_WORD(region_reg_base + SF_CTRL_SF_AES_END_OFFSET);
    iot2lp_para->sec_cfg->r1_aes_end = ((tmpVal & SF_CTRL_SF_AES_REGION_END_MASK) >> SF_CTRL_SF_AES_REGION_END_SHIFT) * 1024;

    iv = iot2lp_para->sec_cfg->r1_aes_iv;
    iv[0] = BL_RD_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W0_OFFSET);
    iv[1] = BL_RD_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W1_OFFSET);
    iv[2] = BL_RD_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W2_OFFSET);
    iv[3] = BL_RD_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W3_OFFSET);

    /* AES region 2 */
    region_reg_base = BL_LP_SF_AES_REGION(2);
    tmpVal = BL_RD_WORD(region_reg_base + SF_CTRL_SF_AES_START_OFFSET);
    iot2lp_para->sec_cfg->r2_aes_en = aes_enabled && !!(tmpVal & SF_CTRL_SF_AES_REGION_EN);
    iot2lp_para->sec_cfg->r2_aes_hw_key_en = !!(tmpVal & SF_CTRL_SF_AES_REGION_HW_KEY_EN);
    iot2lp_para->sec_cfg->r2_aes_lock = !!(tmpVal & SF_CTRL_SF_AES_REGION_LOCK);
    iot2lp_para->sec_cfg->r2_aes_start = ((tmpVal & SF_CTRL_SF_AES_REGION_START_MASK) >> SF_CTRL_SF_AES_REGION_START_SHIFT) * 1024;
    tmpVal = BL_RD_WORD(region_reg_base + SF_CTRL_SF_AES_END_OFFSET);
    iot2lp_para->sec_cfg->r2_aes_end = ((tmpVal & SF_CTRL_SF_AES_REGION_END_MASK) >> SF_CTRL_SF_AES_REGION_END_SHIFT) * 1024;

    iv = iot2lp_para->sec_cfg->r2_aes_iv;
    iv[0] = BL_RD_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W0_OFFSET);
    iv[1] = BL_RD_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W1_OFFSET);
    iv[2] = BL_RD_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W2_OFFSET);
    iv[3] = BL_RD_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W3_OFFSET);
}

void bl_lp_xip_para_save(void)
{
    uint8_t flash_clk, flash_clk_div;
    uint32_t flash_cfg_len;
    static struct bflb_sf_ctrl_io_cs_clk_delay_cfg io_cs_delay_cfg;

    /* Global iot2lp_para pointer is already initialized as const */

    bflb_flash_get_cfg((uint8_t **)&g_flash_para.flash_cfg, &flash_cfg_len);
    g_flash_para.flash_jdec_id = bflb_flash_get_jedec_id();
    bl_lp_xip_get_flash_clock(&flash_clk, &flash_clk_div);
    g_flash_para.flash_clk = flash_clk;
    g_flash_para.flash_clk_div = flash_clk_div;
    g_flash_para.flash_offset = bflb_sf_ctrl_get_flash_image_offset(0, SF_CTRL_FLASH_BANK0);
    g_flash_para.flash_pin_cfg = pm_get_sf_pin_select();

    /* flash io cs clk delay info save */
    bflb_sf_ctrl_get_flash_io_cs_clk_delay((struct bflb_sf_ctrl_io_cs_clk_delay_cfg *)&io_cs_delay_cfg);
    g_flash_para.flash_io_cs_clk_delay_cfg = (void *)&io_cs_delay_cfg;
    g_flash_para.do_xip_recovery = 0;

    iot2lp_para->flash_parameter = &g_flash_para;

    bl_lp_tzc_para_save();
    bl_lp_sec_para_save();
}

static void ATTR_TCM_SECTION bl_lp_sec_recovery(void)
{
    uint8_t encrypt_type = iot2lp_para->sec_cfg->encrypt_type;
    uint8_t xts_mode = iot2lp_para->sec_cfg->xts_mode;
    uint32_t region_reg_base = BL_LP_SF_AES_REGION(0);

    if (xts_mode) {
        bflb_sf_ctrl_disable_wrap_access(0);
        bflb_sf_ctrl_aes_set_mode(SF_CTRL_AES_XTS_MODE);
    } else {
        bflb_sf_ctrl_disable_wrap_access(1);
        bflb_sf_ctrl_aes_set_mode(SF_CTRL_AES_CTR_MODE);
    }

    /* Restore raw IV words (including image offset) before locking regions. */
    /* AES region 0 */
    if (iot2lp_para->sec_cfg->r0_aes_en) {
        if (xts_mode) {
            bflb_sf_ctrl_aes_xts_set_key_be(0, NULL, encrypt_type);
        } else {
            bflb_sf_ctrl_aes_set_key_be(0, NULL, encrypt_type);
        }

        region_reg_base = BL_LP_SF_AES_REGION(0);
        BL_WR_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W0_OFFSET, iot2lp_para->sec_cfg->r0_aes_iv[0]);
        BL_WR_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W1_OFFSET, iot2lp_para->sec_cfg->r0_aes_iv[1]);
        BL_WR_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W2_OFFSET, iot2lp_para->sec_cfg->r0_aes_iv[2]);
        BL_WR_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W3_OFFSET, iot2lp_para->sec_cfg->r0_aes_iv[3]);

        bflb_sf_ctrl_aes_set_region(0, iot2lp_para->sec_cfg->r0_aes_en,
                                   iot2lp_para->sec_cfg->r0_aes_hw_key_en,
                                   iot2lp_para->sec_cfg->r0_aes_start,
                                   iot2lp_para->sec_cfg->r0_aes_end,
                                   iot2lp_para->sec_cfg->r0_aes_lock);
    }

    /* AES region 1 */
    if (iot2lp_para->sec_cfg->r1_aes_en) {
        if (xts_mode) {
            bflb_sf_ctrl_aes_xts_set_key_be(1, NULL, encrypt_type);
        } else {
            bflb_sf_ctrl_aes_set_key_be(1, NULL, encrypt_type);
        }

        region_reg_base = BL_LP_SF_AES_REGION(1);
        BL_WR_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W0_OFFSET, iot2lp_para->sec_cfg->r1_aes_iv[0]);
        BL_WR_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W1_OFFSET, iot2lp_para->sec_cfg->r1_aes_iv[1]);
        BL_WR_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W2_OFFSET, iot2lp_para->sec_cfg->r1_aes_iv[2]);
        BL_WR_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W3_OFFSET, iot2lp_para->sec_cfg->r1_aes_iv[3]);

        bflb_sf_ctrl_aes_set_region(1, iot2lp_para->sec_cfg->r1_aes_en,
                                   iot2lp_para->sec_cfg->r1_aes_hw_key_en,
                                   iot2lp_para->sec_cfg->r1_aes_start,
                                   iot2lp_para->sec_cfg->r1_aes_end,
                                   iot2lp_para->sec_cfg->r1_aes_lock);
    }

    /* AES region 2 */
    if (iot2lp_para->sec_cfg->r2_aes_en) {
        if (xts_mode) {
            bflb_sf_ctrl_aes_xts_set_key_be(2, NULL, encrypt_type);
        } else {
            bflb_sf_ctrl_aes_set_key_be(2, NULL, encrypt_type);
        }

        region_reg_base = BL_LP_SF_AES_REGION(2);
        BL_WR_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W0_OFFSET, iot2lp_para->sec_cfg->r2_aes_iv[0]);
        BL_WR_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W1_OFFSET, iot2lp_para->sec_cfg->r2_aes_iv[1]);
        BL_WR_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W2_OFFSET, iot2lp_para->sec_cfg->r2_aes_iv[2]);
        BL_WR_WORD(region_reg_base + SF_CTRL_SF_AES_IV_W3_OFFSET, iot2lp_para->sec_cfg->r2_aes_iv[3]);

        bflb_sf_ctrl_aes_set_region(2, iot2lp_para->sec_cfg->r2_aes_en,
                                   iot2lp_para->sec_cfg->r2_aes_hw_key_en,
                                   iot2lp_para->sec_cfg->r2_aes_start,
                                   iot2lp_para->sec_cfg->r2_aes_end,
                                   iot2lp_para->sec_cfg->r2_aes_lock);
    }

    bflb_sf_ctrl_aes_enable_be();
    bflb_sf_ctrl_aes_enable();
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

static int ATTR_TCM_SECTION bl_lp_xip_read_enable(spi_flash_cfg_type *pFlashCfg, uint8_t cont_read,
                                                  uint32_t image_offset)
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

void ATTR_TCM_SECTION bl_lp_flash_set_cmds(spi_flash_cfg_type *p_flash_cfg)
{
    struct sf_ctrl_cmds_cfg cmds_cfg;

    cmds_cfg.ack_latency = 1;
    cmds_cfg.cmds_core_en = 1;
    cmds_cfg.cmds_en = 1;
    cmds_cfg.cmds_wrap_mode = 1;
    cmds_cfg.cmds_wrap_len = 9;

    if ((p_flash_cfg->io_mode & 0x1f) == SF_CTRL_QIO_MODE) {
        cmds_cfg.cmds_wrap_mode = 2;
        cmds_cfg.cmds_wrap_len = 2;
    }
    bflb_sf_ctrl_cmds_set(&cmds_cfg, 0);
}

void ATTR_TCM_SECTION bl_lp_xip_recovery(void)
{
    uint32_t jdec_id = 0;
    uint32_t timeout = 0;
    uint32_t tmp_val;
    uint32_t ret = 0;
    lp_fw_sf_sec_t *sec_cfg = iot2lp_para->sec_cfg;
    sf_recovery_para_t *p_flash_para = (sf_recovery_para_t *)iot2lp_para->flash_parameter;
    spi_flash_cfg_type *pFlashCfg = (spi_flash_cfg_type *)p_flash_para->flash_cfg;

    GLB_Set_Flash_Xip(1);

    /* power on flash power */
    /* dcdc18 Always On */
    AON_Set_Ldo18_AON_Power_Switch_For_FLASH(1);

    /* init flash gpio */
    bflb_sf_cfg_init_flash_gpio(p_flash_para->flash_pin_cfg, 0);

    /* flash_clk 0:120M, 1:XTAL, 2:48M, 3:80M, 4:BCLK, 5:96M */
    /* set flash clock */
    GLB_Set_SF_CLK(1, p_flash_para->flash_clk, p_flash_para->flash_clk_div);

    /* update flash controller */
    bl_lp_set_sf_ctrl(pFlashCfg);

    /* set flash io cs clk delay */
    bflb_sf_ctrl_set_flash_io_cs_clk_delay(
        *(struct bflb_sf_ctrl_io_cs_clk_delay_cfg *)p_flash_para->flash_io_cs_clk_delay_cfg);

    /* set flash cmds */
    bl_lp_flash_set_cmds(pFlashCfg);

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

    } while (jdec_id != p_flash_para->flash_jdec_id);

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

    /* Restore Boot2's hardware-key, big-endian AES setup before XIP fetches. */
    if (sec_cfg->r0_aes_en || sec_cfg->r1_aes_en || sec_cfg->r2_aes_en) {
        bl_lp_sec_recovery();
        bl_lp_xip_read_enable(pFlashCfg, 0 /* cont read */, p_flash_para->flash_offset);
    } else {
        bflb_sf_ctrl_aes_disable();
        bl_lp_xip_read_enable(pFlashCfg, 1 /* cont read */, p_flash_para->flash_offset);
    }
}
