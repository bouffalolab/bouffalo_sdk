#include "hal_boot2.h"
#include "bl702_ef_ctrl.h"
#include "bl702_hbn.h"
#include "bl702_glb.h"
#include "bl702_xip_sflash.h"
/**
 * @brief boot2 custom
 *
 * @param None
 * @return uint32
 */
uint32_t hal_boot2_custom(void)
{
    return 0;
}

/**
 * @brief reset sec eng clock
 *
 * @return
 */
void hal_reset_sec_eng(void)
{
    GLB_AHB_Slave1_Reset(BL_AHB_SLAVE1_SEC);
}

/**
 * @brief system soft reset
 *
 * @return
 */
void hal_sw_system_reset(void)
{
    GLB_SW_System_Reset();
}
/**
 * @brief system soft reset
 *
 * @param pll_cfg
 * @return
 */
int32_t ATTR_TCM_SECTION hal_pll_init(hal_pll_config *pll_cfg)
{
    /* PLL config */
    if (pll_cfg->cfg.xtal_type > 0) {
        /* use xtal in aon to config PLL */
        GLB_Set_System_CLK(pll_cfg->cfg.xtal_type, pll_cfg->cfg.pll_clk);
        GLB_Set_System_CLK_Div(pll_cfg->cfg.hclk_div, pll_cfg->cfg.bclk_div);
        GLB_Set_SF_CLK(1, (GLB_SFLASH_CLK_Type)pll_cfg->cfg.flash_clk_type, pll_cfg->cfg.flash_clk_div);

    } else {
        return -1;
    }

    return 0;
}

/**
 * @brief get efuse Boot2 config
 *
 * @param g_efuse_cfg
 * @param
 * @param
 * @return None
 */
void efuse_get_boot2_cfg(boot_efuse_hw_config *g_efuse_cfg)
{
    uint32_t tmp;

    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    /* Get sign and aes type*/
    EF_Ctrl_Read_Secure_Boot((EF_Ctrl_Sign_Type *)g_efuse_cfg->sign, (EF_Ctrl_SF_AES_Type *)g_efuse_cfg->encrypted);
    /* Get hash:aes key slot 0 and slot1*/
    EF_Ctrl_Read_AES_Key(0, (uint32_t *)g_efuse_cfg->pk_hash_cpu0, 8);
    EF_Ctrl_Read_Chip_ID(g_efuse_cfg->chip_id);
    /* Get HBN check sign config */
    EF_Ctrl_Read_Sw_Usage(0, &tmp);
    g_efuse_cfg->hbn_check_sign = (tmp >> 22) & 0x01;

    GLB_Set_System_CLK_Div(0, 1);
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_DLL);
}

/**
 * @brief
 *
 * @param flag
 * @param
 * @param
 * @return
 */
void hal_hbn_set_status_flag(uint32_t flag)
{
    HBN_Set_Status_Flag(flag);
}

/**
 * @brief
 *
 * @param
 * @param
 * @param
 * @return flag
 */
uint32_t hal_hbn_get_status_flag(void)
{
    return HBN_Get_Status_Flag();
}

/**
 * @brief
 *
 * @param
 * @param
 * @param
 * @return user define flag
 */
uint8_t *hal_hbn_get_user_specified_fw(void)
{
    return (uint8_t *)(HBN_BASE + HBN_RSV0_OFFSET);
}

/**
 * @brief clr user define flag
 *
 * @param
 * @param
 * @param
 * @return
 */
void hal_hbn_clr_user_specified_fw(void)
{
    uint32_t *p = (uint32_t *)(HBN_BASE + HBN_RSV0_OFFSET);
    *p = 0;
}
/**
 * @brief flash_get_clk_cfg
 *
 * @return 0
 */
int32_t ATTR_TCM_SECTION flash_get_clk_cfg(hal_pll_config *cfg)
{
    L1C_Cache_Flush(0xf);
    XIP_SFlash_Read_Via_Cache_Need_Lock(8 + sizeof(hal_flash_config) + BL_FLASH_XIP_BASE, (uint8_t *)cfg, sizeof(hal_pll_config));
    L1C_Cache_Flush(0x0);

    return 0;
}
