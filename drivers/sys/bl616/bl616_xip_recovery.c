#include <assert.h>
#include "bflb_flash.h"
#include "bflb_sflash.h"
#include "bflb_sf_ctrl.h"
#include "bflb_sf_cfg.h"
#include "bl616_clock.h"
#include "bl616_tzc_sec.h"
#include "bl616_xip_recovery.h"
#include "tzc_sec_reg.h"
#include "bflb_flash.h"
#include "bflb_sflash.h"
#include "bflb_sf_ctrl.h"
#include "sf_ctrl_reg.h"
#include "bl_lp.h"

#define SF_Ctrl_Get_AES_Region(addr, r) (addr + SF_CTRL_AES_REGION_OFFSET + (r) * 0x80)

static lp_fw_sf_sec_t sf_sec_info;
struct bflb_sf_ctrl_io_cs_clk_delay_cfg io_cs_delay_cfg;


static __inline uint8_t ATTR_CLOCK_SECTION Clock_Get_SF_Clk_Sel_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SF_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_SEL);
}

static __inline uint8_t ATTR_CLOCK_SECTION Clock_Get_SF_Div_Val(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_SF_CFG0);

    return BL_GET_REG_BITS_VAL(tmpVal, GLB_SF_CLK_DIV);
}

static __inline uint8_t ATTR_CLOCK_SECTION Clock_Get_SF_Clk_Sel2_Val(void)
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
            /* wifi pll 120m */
            *flash_clk = 0;
        } else if (sel2 == 1) {
            /* xtal */
            *flash_clk = 1;
        } else if (sel2 == 2) {
            /* wifi pll 120m */
            *flash_clk = 0;
        } else {
            /* aupll div5 clk */
            *flash_clk = 2;
        }
    } else if (sel == 1) {
        /* mux 80m */
        *flash_clk = 3;
    } else if (sel == 2) {
        /* mcu pbclk */
        *flash_clk = 4;
    } else {
        /* wifi 96m */
        *flash_clk = 5;
    }
    *flash_clk_div = Clock_Get_SF_Div_Val();
}

extern uint32_t __binary_length;
void bl_lp_xip_para_save(void)
{
    iot2lp_para->sec_cfg = &sf_sec_info;

    uint32_t tmpVal;
    uint32_t regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, 0);
    uint32_t *iv = iot2lp_para->sec_cfg->r0_aes_iv;
    //spi_flash_cfg_type *flash_cfg;
    uint8_t flash_clk, flash_clk_div;


    /* flash info save */
    uint32_t flash_cfg_len;
    bflb_flash_get_cfg((uint8_t **)&iot2lp_para->flash_cfg, &flash_cfg_len);
    iot2lp_para->flash_jdec_id = GLB_Get_Flash_Id_Value();

    /* flash io cs clk delay info save */
    bflb_sf_ctrl_get_flash_io_cs_clk_delay((struct bflb_sf_ctrl_io_cs_clk_delay_cfg *)&io_cs_delay_cfg);
    iot2lp_para->flash_io_cs_clk_delay_cfg = (void *)&io_cs_delay_cfg;

    tmpVal = getreg32(SF_CTRL_BASE + SF_CTRL_SF_AES_OFFSET);

    if (((tmpVal >> SF_CTRL_SF_AES_BLK_MODE_POS) & 0x1) == 1) {
        iot2lp_para->sec_cfg->xts_mode = 1;
    } else {
        iot2lp_para->sec_cfg->xts_mode = 0;
    }
    iot2lp_para->sec_cfg->encrypt_type = BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_MODE);

    // AES_Region0
    regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, 0);
    tmpVal = BL_RD_WORD(regionRegBase + SF_CTRL_SF_AES_START_OFFSET);
    iot2lp_para->sec_cfg->r0_aes_en = BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_REGION_EN);
    iot2lp_para->sec_cfg->r0_aes_hw_key_en = BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_REGION_HW_KEY_EN);
    iot2lp_para->sec_cfg->r0_aes_lock = BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_REGION_LOCK);
    iot2lp_para->sec_cfg->r0_aes_start = BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_REGION_START) * 1024;
    tmpVal = BL_RD_WORD(regionRegBase + SF_CTRL_SF_AES_END_OFFSET);
    iot2lp_para->sec_cfg->r0_aes_end = BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_REGION_END) * 1024;

    iv = iot2lp_para->sec_cfg->r0_aes_iv;
    iv[0] = BL_RD_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W0_OFFSET);
    iv[1] = BL_RD_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W1_OFFSET);
    iv[2] = BL_RD_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W2_OFFSET);
    iv[3] = BL_RD_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W3_OFFSET);

    // AES_Region1
    regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, 1);
    tmpVal = BL_RD_WORD(regionRegBase + SF_CTRL_SF_AES_START_OFFSET);
    iot2lp_para->sec_cfg->r1_aes_en = BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_REGION_EN);
    iot2lp_para->sec_cfg->r1_aes_hw_key_en = BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_REGION_HW_KEY_EN);
    iot2lp_para->sec_cfg->r1_aes_lock = BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_REGION_LOCK);
    iot2lp_para->sec_cfg->r1_aes_start = BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_REGION_START) * 1024;
    tmpVal = BL_RD_WORD(regionRegBase + SF_CTRL_SF_AES_END_OFFSET);
    iot2lp_para->sec_cfg->r1_aes_end = BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_REGION_END) * 1024;

    iv = iot2lp_para->sec_cfg->r1_aes_iv;
    iv[0] = BL_RD_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W0_OFFSET);
    iv[1] = BL_RD_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W1_OFFSET);
    iv[2] = BL_RD_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W2_OFFSET);
    iv[3] = BL_RD_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W3_OFFSET);

    // AES_Region2
    regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, 2);
    tmpVal = BL_RD_WORD(regionRegBase + SF_CTRL_SF_AES_START_OFFSET);
    iot2lp_para->sec_cfg->r2_aes_en = BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_REGION_EN);
    iot2lp_para->sec_cfg->r2_aes_hw_key_en = BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_REGION_HW_KEY_EN);
    iot2lp_para->sec_cfg->r2_aes_lock = BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_REGION_LOCK);
    iot2lp_para->sec_cfg->r2_aes_start = BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_REGION_START) * 1024;
    tmpVal = BL_RD_WORD(regionRegBase + SF_CTRL_SF_AES_END_OFFSET);
    iot2lp_para->sec_cfg->r2_aes_end = BL_GET_REG_BITS_VAL(tmpVal, SF_CTRL_SF_AES_REGION_END) * 1024;

    iv = iot2lp_para->sec_cfg->r2_aes_iv;
    iv[0] = BL_RD_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W0_OFFSET);
    iv[1] = BL_RD_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W1_OFFSET);
    iv[2] = BL_RD_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W2_OFFSET);
    iv[3] = BL_RD_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W3_OFFSET);

    uint32_t lpfw_size = *((uint32_t *)__lpfw_load_addr - 7);
    iot2lp_para->img_len = (int)(&__binary_length) + lpfw_size;
    printf("[LP] flash: app_len: %d, lp_len: %d, image_len: %d\r\n", (int)&__binary_length, (int)lpfw_size, (int)iot2lp_para->img_len);

    bl_lp_xip_get_flash_clock(&flash_clk, &flash_clk_div);
    iot2lp_para->flash_clk = flash_clk;
    iot2lp_para->flash_clk_div = flash_clk_div;
    printf("[LP] flash_clk: %d, flash_clk_div: %d\r\n", flash_clk, flash_clk_div);

    iot2lp_para->do_xip_recovery = 0;
}


static void bl_lp_xip_set_sf_ctrl(spi_flash_cfg_type *pFlashCfg)
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

static int bl_lp_xip_read_enable(spi_flash_cfg_type *pFlashCfg, uint8_t cont_read, uint32_t image_offset)
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


static void bflb_bootrom_media_boot_set_encrypt(void)
{
    uint8_t encrypt_type = iot2lp_para->sec_cfg->encrypt_type; /* the origin val */
    uint8_t xts_mode = iot2lp_para->sec_cfg->xts_mode;
    uint32_t regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, 0);

    if (xts_mode) {
        bflb_sf_ctrl_disable_wrap_access(0);
        bflb_sf_ctrl_aes_set_mode(SF_CTRL_AES_XTS_MODE);
    } else {
        bflb_sf_ctrl_disable_wrap_access(1);
        bflb_sf_ctrl_aes_set_mode(SF_CTRL_AES_CTR_MODE);
    }

    // AES_Region0
    if (iot2lp_para->sec_cfg->r0_aes_en) {
        if (xts_mode) {
            bflb_sf_ctrl_aes_xts_set_key_be(0, NULL, encrypt_type);
        } else {
            bflb_sf_ctrl_aes_set_key_be(0, NULL, encrypt_type);
        }

        regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, 0);
        BL_WR_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W0_OFFSET, iot2lp_para->sec_cfg->r0_aes_iv[0]);
        BL_WR_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W1_OFFSET, iot2lp_para->sec_cfg->r0_aes_iv[1]);
        BL_WR_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W2_OFFSET, iot2lp_para->sec_cfg->r0_aes_iv[2]);
        BL_WR_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W3_OFFSET, iot2lp_para->sec_cfg->r0_aes_iv[3]);

        bflb_sf_ctrl_aes_set_region(0, iot2lp_para->sec_cfg->r0_aes_en,
                                    iot2lp_para->sec_cfg->r0_aes_hw_key_en,
                                    iot2lp_para->sec_cfg->r0_aes_start,
                                    iot2lp_para->sec_cfg->r0_aes_end,
                                    iot2lp_para->sec_cfg->r0_aes_lock);
    }

    // AES_Region1
    if (iot2lp_para->sec_cfg->r1_aes_en) {
        if (xts_mode) {
            bflb_sf_ctrl_aes_xts_set_key_be(1, NULL, encrypt_type);
        } else {
            bflb_sf_ctrl_aes_set_key_be(1, NULL, encrypt_type);
        }

        regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, 1);
        BL_WR_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W0_OFFSET, iot2lp_para->sec_cfg->r1_aes_iv[0]);
        BL_WR_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W1_OFFSET, iot2lp_para->sec_cfg->r1_aes_iv[1]);
        BL_WR_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W2_OFFSET, iot2lp_para->sec_cfg->r1_aes_iv[2]);
        BL_WR_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W3_OFFSET, iot2lp_para->sec_cfg->r1_aes_iv[3]);

        bflb_sf_ctrl_aes_set_region(1, iot2lp_para->sec_cfg->r1_aes_en,
                                    iot2lp_para->sec_cfg->r1_aes_hw_key_en,
                                    iot2lp_para->sec_cfg->r1_aes_start,
                                    iot2lp_para->sec_cfg->r1_aes_end,
                                    iot2lp_para->sec_cfg->r1_aes_lock);
    }

    // AES_Region2
    if (iot2lp_para->sec_cfg->r2_aes_en) {
        if (xts_mode) {
            bflb_sf_ctrl_aes_xts_set_key_be(2, NULL, encrypt_type);
        } else {
            bflb_sf_ctrl_aes_set_key_be(2, NULL, encrypt_type);
        }

        regionRegBase = SF_Ctrl_Get_AES_Region(SF_CTRL_BASE, 2);
        BL_WR_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W0_OFFSET, iot2lp_para->sec_cfg->r2_aes_iv[0]);
        BL_WR_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W1_OFFSET, iot2lp_para->sec_cfg->r2_aes_iv[1]);
        BL_WR_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W2_OFFSET, iot2lp_para->sec_cfg->r2_aes_iv[2]);
        BL_WR_WORD(regionRegBase + SF_CTRL_SF_AES_IV_W3_OFFSET, iot2lp_para->sec_cfg->r2_aes_iv[3]);

        bflb_sf_ctrl_aes_set_region(2, iot2lp_para->sec_cfg->r2_aes_en,
                                    iot2lp_para->sec_cfg->r2_aes_hw_key_en,
                                    iot2lp_para->sec_cfg->r2_aes_start,
                                    iot2lp_para->sec_cfg->r2_aes_end,
                                    iot2lp_para->sec_cfg->r2_aes_lock);
    }

    bflb_sf_ctrl_aes_enable_be();
    bflb_sf_ctrl_aes_enable();
}

static void bl_lp_set_power(void)
{
    AON_Trim_DcdcDis();
    HBN_Trim_Ldo33VoutTrim();
    AON_Trim_DcdcVoutSel();
    AON_Trim_DcdcVoutTrim();
    AON_Trim_Ldo11socVoutTrim();
}


void bl_lp_flash_set_cmds(spi_flash_cfg_type *p_flash_cfg)
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
static void bflb_bootrom_sboot_set(uint8_t val)
{
    uint32_t tmpVal;
    tmpVal = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_ROM_TZSRG_CTRL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, TZC_SEC_TZC_SBOOT_DONE, val);
    BL_WR_REG(TZC_SEC_BASE, TZC_SEC_TZC_ROM_TZSRG_CTRL, tmpVal);
}

int format_printf(const char *format, ...);
void bl_lp_xip_recovery()
{
    uint32_t flash_pin = 0;
    uint32_t encrypted = 0;
    uint32_t jdec_id = 0;
    uint32_t timeout = 0;
    uint32_t tmp_val;
    uint32_t ret = 0;
    uint32_t power_trim_disable = 0;
    uint32_t ldo18io_cfg_dis = 0;

    bl_lp_debug_record_time(iot2lp_para, "xip flash start");

    spi_flash_cfg_type *pFlashCfg = (spi_flash_cfg_type *)iot2lp_para->flash_cfg;

    /* called from lowpower, open clock */
    *((volatile uint32_t *)0x20000584) = 0xffffffff;

    //EF_Ctrl_Load_Efuse_R0();

    /* system power set */
    power_trim_disable = BL_RD_WORD(0x2005605C);
    power_trim_disable = (power_trim_disable >> 22) & 0x1;
    if (!power_trim_disable) {
        bl_lp_set_power();
    }
    ldo18io_cfg_dis = BL_RD_WORD(0x20056060);
    ldo18io_cfg_dis = (ldo18io_cfg_dis >> 27) & 0x1;
    if (!ldo18io_cfg_dis) {
        GLB_Trim_Ldo18ioVoutSel();
        GLB_Trim_Ldo18ioVoutTrim();
        GLB_Trim_Ldo18ioBypass();
    }

    /* power on flash power */
    /* pu_ldo18flash=1 */
    ret = BL_RD_REG(GLB_BASE, GLB_LDO18IO);
    ret = BL_SET_REG_BIT(ret, GLB_PU_LDO18IO);
    BL_WR_REG(GLB_BASE, GLB_LDO18IO, ret);

    // arch_delay_us(200);
#ifdef SHARED_FUNC_EN
    shared_arch_delay_us(200);
#else
    arch_delay_us(200);
#endif
    /* get flash gpio info */
    flash_pin = BL_RD_WORD(0x2005605C);
    flash_pin = (flash_pin >> 14) & 0x3f;
    /* init flash gpio */
    bflb_sf_cfg_init_flash_gpio((uint8_t)flash_pin, 0);

    uint32_t REG_PLL_BASE_ADDRESS = 0;
    uint32_t tmpVal = 0;

    REG_PLL_BASE_ADDRESS = GLB_BASE + GLB_WIFI_PLL_CFG0_OFFSET;
    if (iot2lp_para->flash_clk == 0) {
        tmpVal = BL_RD_WORD(REG_PLL_BASE_ADDRESS + 4 * 8);
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV8); //120M
        BL_WR_WORD(REG_PLL_BASE_ADDRESS + 4 * 8, tmpVal);
    } else if (iot2lp_para->flash_clk == 2) {
        GLB_Config_AUDIO_PLL_To_491P52M();
    } else if (iot2lp_para->flash_clk == 3) {
        tmpVal = BL_RD_WORD(REG_PLL_BASE_ADDRESS + 4 * 8);
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV12); //80M
        BL_WR_WORD(REG_PLL_BASE_ADDRESS + 4 * 8, tmpVal);
    } else if (iot2lp_para->flash_clk == 5) {
        tmpVal = BL_RD_WORD(REG_PLL_BASE_ADDRESS + 4 * 8);
        tmpVal = BL_SET_REG_BIT(tmpVal, GLB_WIFIPLL_EN_DIV10); //96M
        BL_WR_WORD(REG_PLL_BASE_ADDRESS + 4 * 8, tmpVal);
    }

    /* set flash clock */
    GLB_Set_SF_CLK(1, iot2lp_para->flash_clk, iot2lp_para->flash_clk_div);

    /* update flash controller */
    bl_lp_xip_set_sf_ctrl(pFlashCfg);

    /* set flash io cs clk delay */
    bflb_sf_ctrl_set_flash_io_cs_clk_delay(*(struct bflb_sf_ctrl_io_cs_clk_delay_cfg *)iot2lp_para->flash_io_cs_clk_delay_cfg);

    /* set flash cmds */
    bl_lp_flash_set_cmds(pFlashCfg);

    /* do flash recovery */
    bflb_sflash_release_powerdown(pFlashCfg);
#ifdef SHARED_FUNC_EN
    shared_arch_delay_us(120);
#else
    arch_delay_us(120);
#endif
    timeout = 0;
    do {
        timeout++;
        if (timeout > 2000) {
            break;
        } else if (timeout > 0) {
#ifdef SHARED_FUNC_EN
            shared_arch_delay_us(10);
#else
            arch_delay_us(10);
#endif
        }
        bflb_sflash_reset_continue_read(pFlashCfg);
        /* Send software reset command(80bv has no this command)to deburst wrap for ISSI like */
        bflb_sflash_software_reset(pFlashCfg);
        /* Disable burst may be removed(except for 80BV) and only work with winbond,but just for make sure */
        bflb_sflash_write_enable(pFlashCfg);
        /* For disable command that is setting register instaed of send command, we need write enable */
        /* Delete this line before the other flash_2_wires glitch */
        //SFlash_DisableBurstWrap(pFlashCfg);

        bflb_sflash_set_spi_mode(SF_CTRL_SPI_MODE);
        bflb_sflash_get_jedecid(pFlashCfg, (uint8_t *)&jdec_id);
        /* Dummy disable burstwrap for make sure */
        bflb_sflash_write_enable(pFlashCfg);
    } while (jdec_id != iot2lp_para->flash_jdec_id);

    GLB_Set_Flash_Id_Value(jdec_id);

    timeout = 0;
    do {
        timeout++;
        if (timeout > 50000) {
            break;
        } else if (timeout > 0) {
#ifdef SHARED_FUNC_EN
            shared_arch_delay_us(10);
#else
            arch_delay_us(10);
#endif
        }
        ret = bflb_sflash_busy(pFlashCfg);
    } while (1 == ret);

    /* set flash into 32bits address mode,maybe flash is already in 32bits mode,but it's OK*/
    if (pFlashCfg->io_mode & 0x20) {
        bflb_sflash_set_32bits_addr_mode(pFlashCfg, ENABLE);
    }

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
#ifdef SHARED_FUNC_EN
        shared_arch_delay_us(10);
#else
        arch_delay_us(10);
#endif
    } while (timeout < 1000 && tmp_val != 0x504e4642);
    bflb_bootrom_sboot_set(0xf);

    encrypted = BL_RD_WORD(0x20056000);
    encrypted = encrypted & 0x03;
    if (encrypted == 0) {
        bl_lp_xip_read_enable(pFlashCfg, 1 /* cont read*/, iot2lp_para->flash_offset);
    } else {
        bflb_bootrom_media_boot_set_encrypt();
        bl_lp_xip_read_enable(pFlashCfg, 0 /* not cont read*/, iot2lp_para->flash_offset);
    }

    /* recovery tzc register */
    BL_WR_REG(TZC_SEC_BASE, TZC_SEC_TZC_OCRAM_TZSRG_R0, iot2lp_para->tzc_cfg->tzc_ocram_tzsrg_r0);
    BL_WR_REG(TZC_SEC_BASE, TZC_SEC_TZC_OCRAM_TZSRG_R1, iot2lp_para->tzc_cfg->tzc_ocram_tzsrg_r1);
    BL_WR_REG(TZC_SEC_BASE, TZC_SEC_TZC_OCRAM_TZSRG_R2, iot2lp_para->tzc_cfg->tzc_ocram_tzsrg_r2);
    BL_WR_REG(TZC_SEC_BASE, TZC_SEC_TZC_OCRAM_TZSRG_CTRL, iot2lp_para->tzc_cfg->tzc_ocram_tzsrg_ctrl);

    BL_WR_REG(TZC_SEC_BASE, TZC_SEC_TZC_SF_TZSRG_R0, iot2lp_para->tzc_cfg->tzc_sf_tzsrg_r0);
    BL_WR_REG(TZC_SEC_BASE, TZC_SEC_TZC_SF_TZSRG_R1, iot2lp_para->tzc_cfg->tzc_sf_tzsrg_r1);
    BL_WR_REG(TZC_SEC_BASE, TZC_SEC_TZC_SF_TZSRG_R2, iot2lp_para->tzc_cfg->tzc_sf_tzsrg_r2);
    BL_WR_REG(TZC_SEC_BASE, TZC_SEC_TZC_SF_TZSRG_MSB, iot2lp_para->tzc_cfg->tzc_sf_tzsrg_msb);
    BL_WR_REG(TZC_SEC_BASE, TZC_SEC_TZC_SF_TZSRG_CTRL, iot2lp_para->tzc_cfg->tzc_sf_tzsrg_ctrl);

    BL_WR_REG(GLB_BASE, GLB_UART_CFG1, 0xffffffff);
    BL_WR_REG(GLB_BASE, GLB_UART_CFG2, 0x0000ffff);

    iot2lp_para->wakeup_flag = 1;

    //csi_icache_enable();
    //csi_dcache_enable();

    csi_icache_invalid();
    csi_dcache_invalid();

    bl_lp_debug_record_time(iot2lp_para, "xip flash end");

    void (*pFunc)(uint32_t);
    pFunc = (void (*)(uint32_t))iot2lp_para->app_entry;
    pFunc(iot2lp_para->args[0]);
}