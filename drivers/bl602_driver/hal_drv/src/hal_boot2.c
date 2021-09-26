#include "hal_boot2.h"
#include "hal_gpio.h"
#include "hal_flash.h"
#include "bl602_glb.h"
#include "bl602_ef_ctrl.h"
#include "bl602_hbn.h"
#include "bl602_xip_sflash.h"
#include "bl602_sf_cfg.h"
#include "bl602_sf_cfg_ext.h"
#include "bl602_glb.h"
#include "bl602_xip_sflash_ext.h"
#include "tzc_sec_reg.h"

/**
 * @brief boot2 custom
 *
 * @param None
 * @return uint32
 */
uint32_t hal_boot2_custom(void)
{
    uint32_t sw_cfg, flash_pin_cfg;
    EF_Ctrl_Read_Sw_Usage(0, (uint32_t *)&sw_cfg);
    /* flash_pin_cfg
     * 0:internal flash with io switch,
     * 1:internal flash no io switch,
     * 2:GPIO 17-22
     * 3:GPIO 0-2 & 20-22
    */
    flash_pin_cfg = ((sw_cfg>>16)&0x3);
    if((flash_pin_cfg == 0)||(flash_pin_cfg == 1)){
        gpio_set_mode(GPIO_PIN_20, GPIO_OUTPUT_MODE);
        gpio_write(GPIO_PIN_20, 0);
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
void hal_boot2_get_efuse_cfg(boot2_efuse_hw_config *g_efuse_cfg)
{
    uint32_t tmp;
    uint32_t rootClk;
    uint8_t hdiv = 0, bdiv = 0;

    /* save bclk fclk div and root clock sel */
    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();
    rootClk = BL_RD_REG(HBN_BASE, HBN_GLB);

    /* change root clock to rc32m for efuse operation */
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_RC32M);

    /* Get sign and aes type*/
    EF_Ctrl_Read_Secure_Boot((EF_Ctrl_Sign_Type *)g_efuse_cfg->sign, (EF_Ctrl_SF_AES_Type *)g_efuse_cfg->encrypted);
    /* Get hash:aes key slot 0 and slot1*/
    EF_Ctrl_Read_AES_Key(0, (uint32_t *)g_efuse_cfg->pk_hash_cpu0, 8);
    EF_Ctrl_Read_Chip_ID(g_efuse_cfg->chip_id);
    /* Get HBN check sign config */
    EF_Ctrl_Read_Sw_Usage(0, &tmp);
    g_efuse_cfg->hbn_check_sign = (tmp >> 22) & 0x01;

    /* restore bclk fclk div and root clock sel */
    GLB_Set_System_CLK_Div(hdiv, bdiv);
    BL_WR_REG(HBN_BASE, HBN_GLB,rootClk);
    __NOP();__NOP();__NOP();__NOP();
}

/**
 * @brief reset sec eng clock
 *
 * @return
 */
void hal_boot2_reset_sec_eng(void)
{
    GLB_AHB_Slave1_Reset(BL_AHB_SLAVE1_SEC);
}

/**
 * @brief system soft reset
 *
 * @return
 */
void hal_boot2_sw_system_reset(void)
{
    GLB_SW_System_Reset();
}


/**
 * @brief
 *
 * @param flag
 * @param
 * @param
 * @return
 */
void hal_boot2_set_psmode_status(uint32_t flag)
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
uint32_t hal_boot2_get_psmode_status(void)
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
uint32_t hal_boot2_get_user_fw(void)
{
    return BL_RD_WORD(HBN_BASE + HBN_RSV0_OFFSET);
}

/**
 * @brief clr user define flag
 *
 * @param
 * @param
 * @param
 * @return
 */
void hal_boot2_clr_user_fw(void)
{
    uint32_t *p = (uint32_t *)(HBN_BASE + HBN_RSV0_OFFSET);
    *p = 0;
}

/**
 * @brief hal_boot2_sboot_finish
 *
 * @return 
 */
void ATTR_TCM_SECTION hal_boot2_sboot_finish(void) 
{
    uint32_t tmp_val;

    tmp_val = BL_RD_REG(TZC_SEC_BASE, TZC_SEC_TZC_ROM_CTRL);

    tmp_val = BL_SET_REG_BITS_VAL(tmp_val, TZC_SEC_TZC_SBOOT_DONE, 0xf);

    BL_WR_REG(TZC_SEC_BASE, TZC_SEC_TZC_ROM_CTRL, tmp_val);
}
/**
 * @brief hal_boot2_uart_gpio_init
 *
 * @return 
 */
void hal_boot2_uart_gpio_init(void)
{
    GLB_GPIO_Type gpios[]={GPIO_PIN_7,GPIO_PIN_16};

    GLB_GPIO_Func_Init(GPIO_FUN_UART,gpios,2);

    GLB_UART_Fun_Sel((GPIO_PIN_7 % 8), GLB_UART_SIG_FUN_UART0_RXD);
    GLB_UART_Fun_Sel((GPIO_PIN_16 % 8), GLB_UART_SIG_FUN_UART0_TXD);  
}

/**
 * @brief hal_boot2_debug_uart_gpio_init
 *
 * @return 
 */
void hal_boot2_debug_uart_gpio_init(void)
{
    GLB_GPIO_Type gpios[]={GPIO_PIN_8};

    GLB_GPIO_Func_Init(GPIO_FUN_UART,gpios,1);

    GLB_UART_Sig_Swap_Set(UART_SIG_SWAP_GPIO8_GPIO15);
    GLB_UART_Fun_Sel(GLB_UART_SIG_4, GLB_UART_SIG_FUN_UART1_TXD);
}
/**
 * @brief hal_boot2_debug_uart_gpio_deinit
 *
 * @return 
 */
void hal_boot2_debug_uart_gpio_deinit(void)
{
    GLB_AHB_Slave1_Reset(BL_AHB_SLAVE1_UART0);
    GLB_AHB_Slave1_Reset(BL_AHB_SLAVE1_UART1);
    GLB_UART_Sig_Swap_Set(UART_SIG_SWAP_NONE);
}
