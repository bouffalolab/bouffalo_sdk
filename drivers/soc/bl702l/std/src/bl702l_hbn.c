/**
  ******************************************************************************
  * @file    bl702l_hbn.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include "bl702l_hbn.h"
#include "bl702l_glb.h"
#include "bflb_acomp.h"
#include "bflb_xip_sflash.h"
#include "bl702l_ef_cfg.h"
#include <risc-v/e24/clic.h>

/** @addtogroup  BL702L_Peripheral_Driver
 *  @{
 */

/** @addtogroup  HBN
 *  @{
 */

/** @defgroup  HBN_Private_Macros
 *  @{
 */
#define HBN_CLK_SET_DUMMY_WAIT \
    {                          \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
        __NOP();               \
    }

/*@} end of group HBN_Private_Macros */

/** @defgroup  HBN_Private_Types
 *  @{
 */

/*@} end of group HBN_Private_Types */

/** @defgroup  HBN_Private_Variables
 *  @{
 */
#ifndef BFLB_USE_HAL_DRIVER
static intCallback_Type *hbnInt0CbfArra[HBN_OUT0_INT_MAX] = { NULL };
static intCallback_Type *hbnInt1CbfArra[HBN_OUT1_INT_MAX] = { NULL };
#endif

/*@} end of group HBN_Private_Variables */

/** @defgroup  HBN_Global_Variables
 *  @{
 */

/*@} end of group HBN_Global_Variables */

/** @defgroup  HBN_Private_Fun_Declaration
 *  @{
 */

/*@} end of group HBN_Private_Fun_Declaration */

/** @defgroup  HBN_Private_Functions
 *  @{
 */

/*@} end of group HBN_Private_Functions */

/** @defgroup  HBN_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  Enter HBN
 *
 * @param  cfg: HBN APP Config
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION HBN_Mode_Enter(HBN_APP_CFG_Type *cfg)
{
    uint32_t valLow = 0, valHigh = 0;
    uint64_t val;

    /* work clock select */
    if (cfg->useXtal32k) {
        HBN_32K_Sel(HBN_32K_XTAL);
    } else {
        HBN_32K_Sel(HBN_32K_RC);
        HBN_Power_Off_Xtal_32K();
    }

    /* turn off RC32K during HBN */
    if ((cfg->hbnLevel) >= HBN_LEVEL_2) {
        HBN_Power_Off_RC32K();
    } else {
        HBN_Power_On_RC32K();
    }

    /* always disable HBN pin pull up/down to reduce PDS/HBN current, 0x4000F014[16]=0 */
    HBN_Hw_Pu_Pd_Cfg(cfg->hw_pu_pd_en);

    /* HBN RTC config and enable */
    if (cfg->sleepTime != 0) {
        HBN_Get_RTC_Timer_Val(&valLow, &valHigh);
        val = valLow + ((uint64_t)valHigh << 32);
        val += cfg->sleepTime;
        HBN_Set_RTC_Timer(HBN_RTC_INT_DELAY_0T, val & 0xffffffff, val >> 32, HBN_RTC_COMP_BIT0_39);
        HBN_Enable_RTC_Counter();
    }

    HBN_Power_Down_Flash(cfg->flashCfg);

    GLB_Set_System_CLK(GLB_DLL_XTAL_NONE, GLB_SYS_CLK_RC32M);

    /* power off xtal */
    AON_Power_Off_XTAL();

    HBN_Enable(cfg->ldoLevel, cfg->hbnLevel);
}

/**
 * @brief Set Wakeup HBN or PDS by GPIOX
 *
 * @param gpio_wakeup_src PIN Wakeup PDS or HBN
 * @param gpio_trig_type HBN GPIO interrupt trigger type
 */
void ATTR_TCM_SECTION HBN_GPIO_Wakeup_Set(uint16_t gpio_wakeup_src, HBN_GPIO_INT_Trigger_Type gpio_trig_type)
{
    HBN_Pin_WakeUp_Mask(~(gpio_wakeup_src));

    if (gpio_wakeup_src != 0) {
        HBN_Aon_Pad_IeSmt_Cfg((uint8_t)(gpio_wakeup_src & 0x7f));
        HBN_Comm_Pad_Wakeup_En_Cfg((uint8_t)((gpio_wakeup_src & 0x380) >> 7));
        HBN_GPIO_INT_Enable(gpio_trig_type);
    } else {
        HBN_Aon_Pad_IeSmt_Cfg(0);
        HBN_Comm_Pad_Wakeup_En_Cfg(0);
    }
}

/****************************************************************************/ /**
 * @brief  power down and switch clock
 *
 * @param  flashCfg: None
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION HBN_Power_Down_Flash(spi_flash_cfg_type *flashCfg)
{
    spi_flash_cfg_type bhFlashCfg;

    if (flashCfg == NULL) {
        /* fix this some time */
        /* SFlash_Cache_Flush(); */
        bflb_xip_sflash_read_via_cache_need_lock(BL702L_FLASH_XIP_BASE + 8 + 4, (uint8_t *)(&bhFlashCfg), sizeof(spi_flash_cfg_type), 1, 1);
        /* fix this some time */
        /* SFlash_Cache_Flush(); */

        bflb_sf_ctrl_set_owner(SF_CTRL_OWNER_SAHB);
        bflb_sflash_reset_continue_read(&bhFlashCfg);
    } else {
        bflb_sf_ctrl_set_owner(SF_CTRL_OWNER_SAHB);
        bflb_sflash_reset_continue_read(flashCfg);
    }

    bflb_sflash_powerdown();
}

/****************************************************************************/ /**
 * @brief  Enable HBN mode
 *
 * @param  aGPIOIeCfg: AON GPIO IE config,Bit0->GPIO18. Bit(s) of Wakeup GPIO(s) must not be set to
 *                     0(s),say when use GPIO7 as wake up pin,aGPIOIeCfg should be 0x01.
 * @param  ldoLevel: LDO volatge level
 * @param  hbnLevel: HBN work level
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION HBN_Enable(HBN_LDO_LEVEL_Type ldoLevel, HBN_LEVEL_Type hbnLevel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO_LEVEL_TYPE(ldoLevel));
    CHECK_PARAM(IS_HBN_LEVEL_TYPE(hbnLevel));

    /* HBN mode LDO level */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO11_AON_VOUT_SEL, ldoLevel);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    /* Select RC32M */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL, 0);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);
    __NOP();
    __NOP();
    __NOP();
    __NOP();

    /* Set HBN flag */
    BL_WR_REG(HBN_BASE, HBN_RSV0, HBN_STATUS_ENTER_FLAG);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);

    /* Set HBN level, (HBN_PWRDN_HBN_RAM not use) */
    switch (hbnLevel) {
        case HBN_LEVEL_0:
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWRDN_HBN_RTC);
            break;

        case HBN_LEVEL_1:
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWRDN_HBN_RTC);
            break;

        case HBN_LEVEL_2:
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_RTC);
            break;

        case HBN_LEVEL_3:
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_CORE);
            tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PWRDN_HBN_RTC);
            break;

        default:
            break;
    }

    /* Set power on option:0 for por reset twice for robust 1 for reset only once*/
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PWR_ON_OPTION);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    *(volatile uint8_t *)(CLIC_HART0_BASE + CLIC_INTIP_OFFSET + HBN_OUT0_IRQn) = 0;
    *(volatile uint8_t *)(CLIC_HART0_BASE + CLIC_INTIP_OFFSET + HBN_OUT1_IRQn) = 0;

    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, 0xffffffff);
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, 0);

    /* Enable HBN mode */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_MODE);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    while (1) {
        BL702L_Delay_MS(1000);
    }
}
/****************************************************************************/ /**
 * @brief  Reset HBN mode
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_TCM_SECTION HBN_Reset(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    /* Reset HBN mode */
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_SW_RST);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_SW_RST);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_SW_RST);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  reset HBN by software
 *
 * @param  npXtalType: NP clock type
 * @param  bclkDiv: NP clock div
 * @param  apXtalType: AP clock type
 * @param  fclkDiv: AP clock div
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_App_Reset(uint8_t npXtalType, uint8_t bclkDiv, uint8_t apXtalType, uint8_t fclkDiv)
{
    uint32_t tmp[12];

    tmp[0] = BL_RD_REG(HBN_BASE, HBN_CTL);
    tmp[1] = BL_RD_REG(HBN_BASE, HBN_TIME_L);
    tmp[2] = BL_RD_REG(HBN_BASE, HBN_TIME_H);
    tmp[3] = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmp[4] = BL_RD_REG(HBN_BASE, HBN_IRQ_CLR);
    tmp[5] = BL_RD_REG(HBN_BASE, HBN_PIR_CFG);
    tmp[6] = BL_RD_REG(HBN_BASE, HBN_PIR_VTH);
    tmp[7] = BL_RD_REG(HBN_BASE, HBN_PIR_INTERVAL);
    tmp[8] = BL_RD_REG(HBN_BASE, HBN_SRAM);
    tmp[9] = BL_RD_REG(HBN_BASE, HBN_RSV0);
    tmp[10] = BL_RD_REG(HBN_BASE, HBN_RSV1);
    tmp[11] = BL_RD_REG(HBN_BASE, HBN_RSV2);
    /* DO HBN reset */
    HBN_Reset();
    /* HBN need 3 32k cyclce to recovery */
    BL702L_Delay_US(100);
    /* Recover HBN value */
    BL_WR_REG(HBN_BASE, HBN_TIME_L, tmp[1]);
    BL_WR_REG(HBN_BASE, HBN_TIME_H, tmp[2]);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmp[0]);

    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmp[3]);
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, tmp[4]);
    BL_WR_REG(HBN_BASE, HBN_PIR_CFG, tmp[5]);
    BL_WR_REG(HBN_BASE, HBN_PIR_VTH, tmp[6]);
    BL_WR_REG(HBN_BASE, HBN_PIR_INTERVAL, tmp[7]);
    BL_WR_REG(HBN_BASE, HBN_SRAM, tmp[8]);
    BL_WR_REG(HBN_BASE, HBN_RSV0, tmp[9]);
    BL_WR_REG(HBN_BASE, HBN_RSV1, tmp[10]);
    BL_WR_REG(HBN_BASE, HBN_RSV2, tmp[11]);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Disable HBN mode
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    /* Disable HBN mode */
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_MODE);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get HBN bor out state
 *
 * @param  None
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type HBN_Get_BOR_OUT_State(void)
{
    return BL_GET_REG_BITS_VAL(BL_RD_REG(HBN_BASE, HBN_MISC), HBN_R_BOD_OUT) ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  set HBN bor config
 *
 * @param  enable: ENABLE or DISABLE, if enable, Power up Brown Out Reset
 * @param  threshold: bor threshold
 * @param  mode: bor work mode with por
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_BOR_Config(uint8_t enable, HBN_BOR_THRES_Type threshold, HBN_BOR_MODE_Type mode)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_BOR_THRES_TYPE(threshold));
    CHECK_PARAM(IS_HBN_BOR_MODE_TYPE(mode));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_MISC);

    if (enable) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PU_BOD, 1);
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PU_BOD, 0);
    }

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_BOD_VTH, threshold);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_BOD_SEL, mode);
    BL_WR_REG(HBN_BASE, HBN_MISC, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  VDD11_AON voltage out select at PDS or HBN status
 *
 * @param  ldoLevel: LDO11 volatge level
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type ATTR_TCM_SECTION HBN_PDS_Set_Ldo11_Vout(HBN_LDO11_LEVEL_Type ldoLevel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO11_LEVEL_TYPE(ldoLevel));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_LDO11_AON_VOUT_SEL, ldoLevel);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN set ldo11aon voltage out
 *
 * @param  ldoLevel: LDO volatge level
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Ldo11_Aon_Vout(HBN_LDO_LEVEL_Type ldoLevel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO_LEVEL_TYPE(ldoLevel));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_SW_LDO11_AON_VOUT_SEL, ldoLevel);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN set ldo11soc voltage out
 *
 * @param  ldoLevel: LDO volatge level
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Ldo11_Soc_Vout(HBN_LDO_LEVEL_Type ldoLevel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO_LEVEL_TYPE(ldoLevel));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_SW_LDO11SOC_VOUT_SEL_AON, ldoLevel);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN set ldo11 all voltage out
 *
 * @param  ldoLevel: LDO volatge level
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Set_Ldo11_All_Vout(HBN_LDO_LEVEL_Type ldoLevel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_LDO_LEVEL_TYPE(ldoLevel));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_SW_LDO11_AON_VOUT_SEL, ldoLevel);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_SW_LDO11SOC_VOUT_SEL_AON, ldoLevel);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN select 32K
 *
 * @param  clkType: HBN 32k clock type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_32K_Sel(HBN_32K_CLK_Type clkType)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_HBN_32K_CLK_TYPE(clkType));

    HBN_Trim_RC32K();

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_F32K_SEL, clkType);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select uart clock source
 *
 * @param  clkSel: uart clock type selection
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_UART_CLK_Sel(HBN_UART_CLK_Type clkSel)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_UART_CLK_TYPE(clkSel));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    if (clkSel == HBN_UART_CLK_XCLK) {
        tmpVal = BL_SET_REG_BIT(tmpVal, HBN_UART_CLK_SEL2);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_UART_CLK_SEL2);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_UART_CLK_SEL, clkSel);
    }
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select xclk clock source
 *
 * @param  xClk: xclk clock type selection
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_Type xClk)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    CHECK_PARAM(IS_HBN_XCLK_CLK_TYPE(xClk));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL);

    switch (xClk) {
        case HBN_XCLK_CLK_RC32M:
            tmpVal2 &= (~(1 << 0));
            break;

        case HBN_XCLK_CLK_XTAL:
            tmpVal2 |= (1 << 0);
            break;

        default:
            break;
    }

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL, tmpVal2);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);
    HBN_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Select root clk source
 *
 * @param  rootClk: root clock type selection
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_CLOCK_SECTION HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_Type rootClk)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    CHECK_PARAM(IS_HBN_ROOT_CLK_TYPE(rootClk));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL);

    switch (rootClk) {
        case HBN_ROOT_CLK_RC32M:
            tmpVal2 = 0x0;
            break;

        case HBN_ROOT_CLK_XTAL:
            tmpVal2 = 0x1;
            break;

        case HBN_ROOT_CLK_DLL:
            tmpVal2 |= (1 << 1);
            break;

        default:
            break;
    }

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_ROOT_CLK_SEL, tmpVal2);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);
    HBN_CLK_SET_DUMMY_WAIT;

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  set HBN_RAM sleep mode
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_HRAM_slp(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_SRAM);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_RETRAM_SLP);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_RETRAM_RET);
    BL_WR_REG(HBN_BASE, HBN_SRAM, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  set HBN_RAM retension mode
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_HRAM_Ret(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_SRAM);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_RETRAM_SLP);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_RETRAM_RET);
    BL_WR_REG(HBN_BASE, HBN_SRAM, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Power on XTAL 32K
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Power_On_Xtal_32K(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_XTAL32K);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_XTAL32K_HIZ_EN);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_XTAL32K_INV_STRE, 3);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_XTAL32K_AC_CAP_SHORT);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PU_XTAL32K);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PU_XTAL32K_BUF);
    BL_WR_REG(HBN_BASE, HBN_XTAL32K, tmpVal);

    /* Delay >1s, but 1s is too long, so user should delay after this function */
    arch_delay_us(1100);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Power off XTAL 32K
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Power_Off_Xtal_32K(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_XTAL32K);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PU_XTAL32K);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PU_XTAL32K_BUF);
    BL_WR_REG(HBN_BASE, HBN_XTAL32K, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Power on RC32K
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Power_On_RC32K(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PU_RC32K);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    /* Delay >800us */
    arch_delay_us(880);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Power off RC3K
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_CLOCK_SECTION HBN_Power_Off_RC32K(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PU_RC32K);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Trim RC32K
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_CLOCK_SECTION HBN_Trim_RC32K(void)
{
    Efuse_Common_Trim_Type trim;
    int32_t tmpVal = 0;

    EF_Ctrl_Read_Common_Trim("rc32k", &trim);

    if (trim.en) {
        if (trim.parity == EF_Ctrl_Get_Trim_Parity(trim.value, trim.len)) {
            tmpVal = BL_RD_REG(HBN_BASE, HBN_RC32K_CTRL0);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_RC32K_CAP_SEL, trim.value);
            BL_WR_REG(HBN_BASE, HBN_RC32K_CTRL0, tmpVal);
            arch_delay_us(2);
            return SUCCESS;
        }
    }

    return ERROR;
}
#endif

/****************************************************************************/ /**
 * @brief  Get HBN status flag
 *
 * @param  None
 *
 * @return HBN status flag value
 *
*******************************************************************************/
uint32_t HBN_Get_Status_Flag(void)
{
    return BL_RD_REG(HBN_BASE, HBN_RSV0);
}

/****************************************************************************/ /**
 * @brief  Set HBN status flag
 *
 * @param  flag: Status Flag
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_Status_Flag(uint32_t flag)
{
    BL_WR_REG(HBN_BASE, HBN_RSV0, flag);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get HBN wakeup address
 *
 * @param  None
 *
 * @return HBN wakeup address
 *
*******************************************************************************/
uint32_t HBN_Get_Wakeup_Addr(void)
{
    return BL_RD_REG(HBN_BASE, HBN_RSV1);
}

/****************************************************************************/ /**
 * @brief  Set HBN wakeup address
 *
 * @param  addr: HBN wakeup address
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_Wakeup_Addr(uint32_t addr)
{
    BL_WR_REG(HBN_BASE, HBN_RSV1, addr);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get user boot config
 *
 * @param  None
 *
 * @return user boot config
 *
*******************************************************************************/
uint8_t HBN_Get_User_Boot_Config(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV2);
    if (HBN_RELEASE_CORE_FLAG == BL_GET_REG_BITS_VAL(tmpVal, HBN_RELEASE_CORE)) {
        return BL_GET_REG_BITS_VAL(tmpVal, HBN_USER_BOOT_SEL);
    }

    return 0x00;
}

/****************************************************************************/ /**
 * @brief  Set user boot config
 *
 * @param  ubCfg: user boot config
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_User_Boot_Config(uint8_t ubCfg)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_RSV2);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_RELEASE_CORE, HBN_RELEASE_CORE_FLAG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_USER_BOOT_SEL, ubCfg);
    BL_WR_REG(HBN_BASE, HBN_RSV2, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN clear RTC timer counter
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Clear_RTC_Counter(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    /* Clear RTC control bit0 */
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal & 0xfffffffe);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN clear RTC timer counter
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Enable_RTC_Counter(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    /* Set RTC control bit0 */
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal | 0x01);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN set RTC timer configuration
 *
 * @param  delay: RTC interrupt delay 32 clocks
 * @param  compValLow: RTC interrupt commpare value low 32 bits
 * @param  compValHigh: RTC interrupt commpare value high 32 bits
 * @param  compMode: RTC interrupt commpare
 *                   mode:HBN_RTC_COMP_BIT0_39,HBN_RTC_COMP_BIT0_23,HBN_RTC_COMP_BIT13_39
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_RTC_Timer(HBN_RTC_INT_Delay_Type delay, uint32_t compValLow, uint32_t compValHigh, uint8_t compMode)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_HBN_RTC_INT_DELAY_TYPE(delay));

    BL_WR_REG(HBN_BASE, HBN_TIME_L, compValLow);
    BL_WR_REG(HBN_BASE, HBN_TIME_H, compValHigh & 0xff);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    /* Set interrupt delay option */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_RTC_DLY_OPTION, delay);
    /* Set RTC compare mode */
    tmpVal |= (compMode << 1);
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN get RTC async timer count value
 *
 * @param  valLow: RTC count value pointer for low 32 bits
 * @param  valHigh: RTC count value pointer for high 8 bits
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
static BL_Err_Type HBN_Get_RTC_Timer_Async_Val(uint32_t *valLow, uint32_t *valHigh)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_NEW_RTC_TIME_H);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_RTC_TIMER_LATCH_EN);
    if (tmpVal == 0) {
        /* Tigger RTC val read */
        tmpVal = BL_RD_REG(HBN_BASE, HBN_RTC_TIME_H);
        tmpVal = BL_SET_REG_BIT(tmpVal, HBN_RTC_TIME_LATCH);
        BL_WR_REG(HBN_BASE, HBN_RTC_TIME_H, tmpVal);
        tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_RTC_TIME_LATCH);
        BL_WR_REG(HBN_BASE, HBN_RTC_TIME_H, tmpVal);

        /* Read RTC val */
        *valLow = BL_RD_REG(HBN_BASE, HBN_RTC_TIME_L);
        *valHigh = (BL_RD_REG(HBN_BASE, HBN_RTC_TIME_H) & 0xff);
    } else {
        /* Read RTC val */
        *valLow = BL_RD_REG(GLB_BASE, GLB_NEW_RTC_TIME_L);
        *valHigh = (BL_RD_REG(GLB_BASE, GLB_NEW_RTC_TIME_H) & 0xff);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN get RTC timer count value
 *
 * @param  valLow: RTC count value pointer for low 32 bits
 * @param  valHigh: RTC count value pointer for high 8 bits
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Get_RTC_Timer_Val(uint32_t *valLow, uint32_t *valHigh)
{
    uint32_t tmpValLow, tmpValHigh, tmpValLow1, tmpValHigh1;
    uint64_t val, val1;

    do {
        HBN_Get_RTC_Timer_Async_Val(&tmpValLow, &tmpValHigh);
        val = ((uint64_t)tmpValHigh << 32) | ((uint64_t)tmpValLow);
        HBN_Get_RTC_Timer_Async_Val(&tmpValLow1, &tmpValHigh1);
        val1 = ((uint64_t)tmpValHigh1 << 32) | ((uint64_t)tmpValLow1);
    } while (val1 < val);

    *valLow = tmpValLow1;
    *valHigh = tmpValHigh1;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN clear RTC timer interrupt,this function must be called to clear delayed rtc IRQ
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Clear_RTC_INT(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    /* Clear RTC commpare:bit1-3 for clearing Delayed RTC IRQ */
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal & 0xfffffff1);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN enable GPIO interrupt
 *
 * @param  gpioIntTrigType: HBN GPIO interrupt trigger type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_GPIO_INT_Enable(HBN_GPIO_INT_Trigger_Type gpioIntTrigType)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_HBN_GPIO_INT_TRIGGER_TYPE(gpioIntTrigType));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MODE, gpioIntTrigType);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN disable GPIO interrupt
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_GPIO_INT_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MASK, 0);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN get interrupt status
 *
 * @param  irqType: HBN interrupt type
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type HBN_Get_INT_State(HBN_INT_Type irqType)
{
    uint32_t tmpVal;

    /* Check the parameters */

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_STAT);

    if (tmpVal & (1 << irqType)) {
        return SET;
    } else {
        return RESET;
    }
}

/****************************************************************************/ /**
 * @brief  HBN get pin wakeup mode value
 *
 * @param  None
 *
 * @return HBN pin wakeup mode value
 *
*******************************************************************************/
uint8_t HBN_Get_Pin_Wakeup_Mode(void)
{
    return BL_GET_REG_BITS_VAL(BL_RD_REG(HBN_BASE, HBN_IRQ_MODE), HBN_PIN_WAKEUP_MODE);
}

/****************************************************************************/ /**
 * @brief  HBN clear interrupt status
 *
 * @param  irqType: HBN interrupt type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Clear_IRQ(HBN_INT_Type irqType)
{
    uint32_t tmpVal;

    CHECK_PARAM(IS_HBN_INT_TYPE(irqType));

    /* set clear bit */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_CLR);
    tmpVal |= (1 << irqType);
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, tmpVal);

    /* unset clear bit */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_CLR);
    tmpVal &= (~(1 << irqType));
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN hardware pullup or pulldown configuration
 *
 * @param  enable: ENABLE or DISABLE
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Hw_Pu_Pd_Cfg(uint8_t enable)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);

    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, HBN_REG_EN_HW_PU_PD);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_REG_EN_HW_PU_PD);
    }

    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN Config Common pad Wakeup enable
 *
 * @param  padCfg: GPIO Wakeup Enable for HBN
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Comm_Pad_Wakeup_En_Cfg(uint8_t padCfg)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_MISC);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_GPIO_WAKEUP_EN_AON, padCfg);
    BL_WR_REG(HBN_BASE, HBN_MISC, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN Config AON pad input and SMT
 *
 * @param  padCfg: AON pad config
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Aon_Pad_IeSmt_Cfg(uint8_t padCfg)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PAD_CTRL_0);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_REG_AON_GPIO_IE_SMT, padCfg);
    BL_WR_REG(HBN_BASE, HBN_PAD_CTRL_0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN Config PDS pad keep select
 *
 * @param  keepSel: PDS pad keep select
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
__WEAK
BL_Err_Type HBN_Enable_PDS_Pad_Keep(uint32_t keepSel)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_PAD_CTRL_0);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_REG_AON_GPIO_ISO_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_CR_GPIO_KEEP_EN,keepSel);
    BL_WR_REG(HBN_BASE, HBN_PAD_CTRL_0, tmpVal);

    return SUCCESS;
}
/****************************************************************************/ /**
 * @brief  HBN wakeup pin mask configuration
 *
 * @param  maskVal: mask value
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Pin_WakeUp_Mask(uint16_t maskVal)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_MASK, maskVal);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN aon pad cfg configuration
 *
 * @param  aonPadHwCtrlEn: hw control aon pad enable
 * @param  aonGpio: aon pad number
 * @param  aonPadCfg: aon pad configuration
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type ATTR_TCM_SECTION HBN_Aon_Pad_Cfg(uint8_t aonPadHwCtrlEn, HBN_AON_PAD_Type aonGpio, HBN_AON_PAD_CFG_Type *aonPadCfg)
{
    uint32_t tmpVal0;
    uint32_t tmpVal1;
    uint32_t enAonCtrlGpio;
    uint32_t aonPadIeSmt;
    uint32_t aonPadPu;
    uint32_t aonPadPd;
    uint32_t aonPadOe;

    CHECK_PARAM(IS_HBN_AON_PAD_TYPE(aonGpio));

    if (NULL == aonPadCfg) {
        return ERROR;
    }

    /* hbn_pad_ctrl_0 */
    tmpVal0 = BL_RD_REG(HBN_BASE, HBN_PAD_CTRL_0);
    enAonCtrlGpio = BL_GET_REG_BITS_VAL(tmpVal0, HBN_REG_EN_AON_CTRL_GPIO);
    if (aonPadCfg->ctrlEn) {
        enAonCtrlGpio |= (1 << aonGpio);
    } else {
        enAonCtrlGpio &= ~(1 << aonGpio);
    }
    tmpVal0 = BL_SET_REG_BITS_VAL(tmpVal0, HBN_REG_EN_AON_CTRL_GPIO, enAonCtrlGpio);
    aonPadIeSmt = BL_GET_REG_BITS_VAL(tmpVal0, HBN_REG_AON_GPIO_IE_SMT);
    if (aonPadCfg->ie) {
        aonPadIeSmt |= (1 << aonGpio);
    } else {
        aonPadIeSmt &= ~(1 << aonGpio);
    }
    tmpVal0 = BL_SET_REG_BITS_VAL(tmpVal0, HBN_REG_AON_GPIO_IE_SMT, aonPadIeSmt);
    BL_WR_REG(HBN_BASE, HBN_PAD_CTRL_0, tmpVal0);

    /* hbn_pad_ctrl_1 */
    tmpVal1 = BL_RD_REG(HBN_BASE, HBN_PAD_CTRL_1);
    aonPadPu = BL_GET_REG_BITS_VAL(tmpVal1, HBN_REG_AON_GPIO_PU);
    if (aonPadCfg->pullUp) {
        aonPadPu |= (1 << aonGpio);
    } else {
        aonPadPu &= ~(1 << aonGpio);
    }
    tmpVal1 = BL_SET_REG_BITS_VAL(tmpVal1, HBN_REG_AON_GPIO_PU, aonPadPu);
    aonPadPd = BL_GET_REG_BITS_VAL(tmpVal1, HBN_REG_AON_GPIO_PD);
    if (aonPadCfg->pullDown) {
        aonPadPd |= (1 << aonGpio);
    } else {
        aonPadPd &= ~(1 << aonGpio);
    }
    tmpVal1 = BL_SET_REG_BITS_VAL(tmpVal1, HBN_REG_AON_GPIO_PD, aonPadPd);
    aonPadOe = BL_GET_REG_BITS_VAL(tmpVal1, HBN_REG_AON_GPIO_OE);
    if (aonPadCfg->oe) {
        aonPadOe |= (1 << aonGpio);
    } else {
        aonPadOe &= ~(1 << aonGpio);
    }
    tmpVal1 = BL_SET_REG_BITS_VAL(tmpVal1, HBN_REG_AON_GPIO_OE, aonPadOe);
    BL_WR_REG(HBN_BASE, HBN_PAD_CTRL_1, tmpVal1);

    return SUCCESS;
}


/****************************************************************************/ /**
 * @brief  HBN enable ACOMP interrupt
 *
 * @param  acompId: HBN Acomp ID
 * @param  edge: HBN acomp interrupt edge type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Enable_AComp_IRQ(uint8_t acompId, HBN_ACOMP_INT_EDGE_Type edge)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    CHECK_PARAM(IS_AON_ACOMP_ID_TYPE(acompId));
    CHECK_PARAM(IS_HBN_ACOMP_INT_EDGE_TYPE(edge));

    if (acompId == AON_ACOMP0_ID) {
        tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
        tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP0_EN);
        tmpVal2 = tmpVal2 | edge;
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP0_EN, tmpVal2);
        BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);
    } else if (acompId == AON_ACOMP1_ID) {
        tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
        tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP1_EN);
        tmpVal2 = tmpVal2 | edge;
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP1_EN, tmpVal2);
        BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN disable ACOMP interrupt
 *
 * @param  acompId: HBN Acomp ID
 * @param  edge: HBN acomp interrupt edge type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Disable_AComp_IRQ(uint8_t acompId, HBN_ACOMP_INT_EDGE_Type edge)
{
    uint32_t tmpVal;
    uint32_t tmpVal2;

    CHECK_PARAM(IS_AON_ACOMP_ID_TYPE(acompId));
    CHECK_PARAM(IS_HBN_ACOMP_INT_EDGE_TYPE(edge));

    if (acompId == (uint8_t)AON_ACOMP0_ID) {
        tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
        tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP0_EN);
        tmpVal2 = tmpVal2 & (~edge);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP0_EN, tmpVal2);
        BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);
    } else if (acompId == (uint8_t)AON_ACOMP1_ID) {
        tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
        tmpVal2 = BL_GET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP1_EN);
        tmpVal2 = tmpVal2 & (~edge);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_IRQ_ACOMP1_EN, tmpVal2);
        BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN enable BOR interrupt
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Enable_BOR_IRQ(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_IRQ_BOD_EN);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN disable BOR interrupt
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Disable_BOR_IRQ(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_IRQ_BOD_EN);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  get HBN reset event status
 *
 * @param  event: HBN reset event type
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type HBN_Get_Reset_Event(HBN_RST_EVENT_Type event)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, HBN_RESET_EVENT);

    return (tmpVal & (1 << event)) ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  clear HBN reset event status
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Clear_Reset_Event(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_CLEAR_RESET_EVENT);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_SET_REG_BIT(tmpVal, HBN_CLEAR_RESET_EVENT);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_CLEAR_RESET_EVENT);
    BL_WR_REG(HBN_BASE, HBN_GLB, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN out0 install interrupt callback
 *
 * @param  intType: HBN out0 interrupt type
 * @param  cbFun: HBN out0 interrupt callback
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
BL_Err_Type HBN_Out0_Callback_Install(HBN_OUT0_INT_Type intType, intCallback_Type *cbFun)
{
    /* Check the parameters */
    CHECK_PARAM(IS_HBN_OUT0_INT_TYPE(intType));

    hbnInt0CbfArra[intType] = cbFun;

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  HBN out1 install interrupt callback
 *
 * @param  intType: HBN out1 interrupt type
 * @param  cbFun: HBN out1 interrupt callback
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
BL_Err_Type HBN_Out1_Callback_Install(HBN_OUT1_INT_Type intType, intCallback_Type *cbFun)
{
    /* Check the parameters */
    CHECK_PARAM(IS_HBN_OUT1_INT_TYPE(intType));

    hbnInt1CbfArra[intType] = cbFun;

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  HBN GPIO debug pull config
 *
 * @param  pupdEn: Enable or disable GPIO pull down and pull up
 * @param  dlyEn: Enable or disable GPIO wakeup delay function
 * @param  dlySec: GPIO wakeup delay sec 1 to 7
 * @param  gpioIrq: HBN GPIO num
 * @param  gpioMask: HBN GPIO MASK or UNMASK
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
BL_Err_Type ATTR_TCM_SECTION HBN_GPIO_Dbg_Pull_Cfg(BL_Fun_Type pupdEn, BL_Fun_Type dlyEn, uint8_t dlySec, HBN_INT_Type gpioIrq, BL_Mask_Type gpioMask)
{
    uint32_t tmpVal;

    CHECK_PARAM(((dlySec >= 1) && (dlySec <= 7)));
    CHECK_PARAM((gpioIrq >= HBN_INT_GPIO9) && (gpioIrq <= HBN_INT_GPIO13));

    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_DLY_EN, dlyEn);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_PIN_WAKEUP_DLY_SEL, dlySec);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_REG_EN_HW_PU_PD, pupdEn);

    if (gpioMask != UNMASK) {
        tmpVal = tmpVal | (1 << (gpioIrq + 8));
    } else {
        tmpVal = tmpVal & ~(1 << (gpioIrq + 8));
    }

    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    return SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  Set BOR config
 *
 * @param  cfg: Enable or disable
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type HBN_Set_BOR_Cfg(HBN_BOR_CFG_Type *cfg)
{
    uint32_t tmpVal = 0;

    if (cfg->enableBorInt) {
        HBN_Enable_BOR_IRQ();
    } else {
        HBN_Disable_BOR_IRQ();
    }

    tmpVal = BL_RD_REG(HBN_BASE, HBN_MISC);

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, HBN_BOD_VTH, cfg->borThreshold);

    if (cfg->enablePorInBor) {
        tmpVal = BL_SET_REG_BIT(tmpVal, HBN_BOD_SEL);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_BOD_SEL);
    }

    if (cfg->enableBor) {
        tmpVal = BL_SET_REG_BIT(tmpVal, HBN_PU_BOD);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_PU_BOD);
    }

    BL_WR_REG(HBN_BASE, HBN_MISC, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  HBN OUT0 interrupt handler
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
void HBN_OUT0_IRQHandler(void)
{
    if (SET == HBN_Get_INT_State(HBN_INT_GPIO9)) {
        HBN_Clear_IRQ(HBN_INT_GPIO9);

        if (hbnInt0CbfArra[HBN_OUT0_INT_GPIO9] != NULL) {
            hbnInt0CbfArra[HBN_OUT0_INT_GPIO9]();
        }
    }

    if (SET == HBN_Get_INT_State(HBN_INT_GPIO10)) {
        HBN_Clear_IRQ(HBN_INT_GPIO10);

        if (hbnInt0CbfArra[HBN_OUT0_INT_GPIO10] != NULL) {
            hbnInt0CbfArra[HBN_OUT0_INT_GPIO10]();
        }
    }

    if (SET == HBN_Get_INT_State(HBN_INT_GPIO11)) {
        HBN_Clear_IRQ(HBN_INT_GPIO11);

        if (hbnInt0CbfArra[HBN_OUT0_INT_GPIO11] != NULL) {
            hbnInt0CbfArra[HBN_OUT0_INT_GPIO11]();
        }
    }

    if (SET == HBN_Get_INT_State(HBN_INT_GPIO12)) {
        HBN_Clear_IRQ(HBN_INT_GPIO12);

        if (hbnInt0CbfArra[HBN_OUT0_INT_GPIO12] != NULL) {
            hbnInt0CbfArra[HBN_OUT0_INT_GPIO12]();
        }
    }

    if (SET == HBN_Get_INT_State(HBN_INT_GPIO13)) {
        HBN_Clear_IRQ(HBN_INT_GPIO13);

        if (hbnInt0CbfArra[HBN_OUT0_INT_GPIO13] != NULL) {
            hbnInt0CbfArra[HBN_OUT0_INT_GPIO13]();
        }
    }

    if (SET == HBN_Get_INT_State(HBN_INT_RTC)) {
        HBN_Clear_IRQ(HBN_INT_RTC);
        HBN_Clear_RTC_INT();

        if (hbnInt0CbfArra[HBN_OUT0_INT_RTC] != NULL) {
            hbnInt0CbfArra[HBN_OUT0_INT_RTC]();
        }
    }
}
#endif

/****************************************************************************/ /**
 * @brief  HBN OUT1 interrupt handler
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
void HBN_OUT1_IRQHandler(void)
{
    /* BOD */
    if (SET == HBN_Get_INT_State(HBN_INT_BOD)) {
        HBN_Clear_IRQ(HBN_INT_BOD);

        if (hbnInt1CbfArra[HBN_OUT1_INT_BOD] != NULL) {
            hbnInt1CbfArra[HBN_OUT1_INT_BOD]();
        }
    }

    /* ACOMP0 */
    if (SET == HBN_Get_INT_State(HBN_INT_ACOMP0)) {
        HBN_Clear_IRQ(HBN_INT_ACOMP0);

        if (hbnInt1CbfArra[HBN_OUT1_INT_ACOMP0] != NULL) {
            hbnInt1CbfArra[HBN_OUT1_INT_ACOMP0]();
        }
    }

    /* ACOMP1 */
    if (SET == HBN_Get_INT_State(HBN_INT_ACOMP1)) {
        HBN_Clear_IRQ(HBN_INT_ACOMP1);

        if (hbnInt1CbfArra[HBN_OUT1_INT_ACOMP1] != NULL) {
            hbnInt1CbfArra[HBN_OUT1_INT_ACOMP1]();
        }
    }
}
#endif

/****************************************************************************/ /**
 * @brief  HBN out0 IRQHandler install
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
BL_Err_Type HBN_Out0_IRQHandler_Install(void)
{
    Interrupt_Handler_Register(HBN_OUT0_IRQn, HBN_OUT0_IRQHandler);
    return SUCCESS;
}
#endif
/****************************************************************************/ /**
 * @brief  HBN out1 IRQHandler install
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
BL_Err_Type HBN_Out1_IRQHandler_Install(void)
{
    Interrupt_Handler_Register(HBN_OUT1_IRQn, HBN_OUT1_IRQHandler);
    return SUCCESS;
}
#endif
/*@} end of group HBN_Public_Functions */

/*@} end of group HBN */

/*@} end of group BL702L_Peripheral_Driver */
