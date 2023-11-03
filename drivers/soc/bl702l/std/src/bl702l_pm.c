#include "bl702l_pm.h"
#include "bl702l_glb.h"
#include "bl702l_pds.h"
#include "bl702l_hbn.h"
#include "bl702l_clock.h"
#include "bflb_acomp.h"
#include "bflb_flash.h"

#ifndef PM_PDS_FLASH_POWER_OFF
#define PM_PDS_FLASH_POWER_OFF 1
#endif

#ifndef PM_PDS_DLL_POWER_OFF
#define PM_PDS_DLL_POWER_OFF 1
#endif

#ifndef PM_PDS_PLL_POWER_OFF
#define PM_PDS_PLL_POWER_OFF 1
#endif

#ifndef PM_PDS_RF_POWER_OFF
#define PM_PDS_RF_POWER_OFF 1
#endif

#ifndef PM_PDS_LDO_LEVEL_DEFAULT
#define PM_PDS_LDO_LEVEL_DEFAULT HBN_LDO_LEVEL_1P10V
#endif

#ifndef PM_PDS_IO_KEEP_VAL
#define PM_PDS_IO_KEEP_VAL 0
#endif

#ifndef PM_HBN_LDO_LEVEL_DEFAULT
#define PM_HBN_LDO_LEVEL_DEFAULT HBN_LDO_LEVEL_1P10V
#endif

#ifndef PM_OCRAM_RET_VAL
#define PM_OCRAM_RET_VAL 0x1C1F1F
#endif


static PDS_DEFAULT_LV_CFG_Type pdsCfgLevel31 = {
    .pdsCtl = {
        .pdsStart = 1,
        .sleepForever = 0,
        .xtalForceOff = 0,
        .waitRC32mRdy = 0,
        .avdd14Off = 1,
        .bgSysOff = 1,
        .glbRstProtect = 0,
        .puFlash = 0,
        .clkOff = 1,
        .memStby = 1,
        .swPuFlash = 1,
        .isolation = 1,
        .waitXtalRdy = 0,
        .pdsPwrOff = 1,
        .xtalOff = 1,
        .socEnbForceOn = 1,
        .pdsRstSocEn = 1,
        .pdsRC32mOn = 0,
        .pdsLdoVselEn = 1,
        .xtalCntRC32kEn = 0,
        .cpu0WfiMask = 0,
        .ldo11Off = 1,
        .pdsCtlRfSel = 1,
        .xtalDegEn = 0,
        .bleWakeupReqEn = 0,
        .swGpioIsoMod = 0,
        .pdsClkOff = 0,
        .pdsGpioKeep = 0,
        .pdsGpioIsoMod = 0,
    },
    .pdsCtl2 = {
        .forceCpuPdsRst = 0,
        .forceBzPdsRst = 0,
        .forceCpuGateClk = 0,
        .forceBzGateClk = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff = 0,
        .forceMiscPdsRst = 0,
        .forceMiscGateClk = 0,
        .MiscIsoEn = 1,
    },
    .pdsCtl4 = {
        .cpuRst = 1,
        .cpuGateClk = 1,
        .BzRst = 1,
        .BzGateClk = 1,
        .MiscPwrOff = 1,
        .MiscRst = 1,
        .MiscGateClk = 1,
    }
};

/****************************************************************************/ /**
* @brief  pm power on flash
*
* @param  cfg: PDS app config
*
* @return None
*
*******************************************************************************/
void ATTR_TCM_SECTION pm_power_on_flash(uint32_t *cfg, uint8_t cont)
{
    /* Init flash gpio */
    bflb_sf_cfg_init_flash_gpio(0, 1);

    bflb_sf_ctrl_set_owner(SF_CTRL_OWNER_SAHB);

    /* Restore flash */
    bflb_sflash_restore_from_powerdown((spi_flash_cfg_type *)cfg, cont, 0);
}

/****************************************************************************/ /**
 * @brief  pm power on flash pad gpio
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION pm_power_on_flash_pad(void)
{
    /* Turn on Flash pad, GPIO23 - GPIO28 */
    bflb_sf_cfg_init_internal_flash_gpio();
}

/****************************************************************************/ /**
 * @brief  pm update flash_ctrl setting
 *
 * @param  fastClock: fast clock
 *
 * @return None
 *
*******************************************************************************/
static ATTR_TCM_SECTION void PDS_Update_Flash_Ctrl_Setting(uint8_t fastClock)
{
    if (fastClock) {
        GLB_Set_SF_CLK(1, GLB_SFLASH_CLK_64M, 0);
    } else {
        GLB_Set_SF_CLK(1, GLB_SFLASH_CLK_XCLK, 0);
    }

    bflb_sf_ctrl_set_clock_delay(fastClock);
}

/****************************************************************************/ /**
 * @brief  pm pds wakeup src enable
 *
 * @param  WakeupType
 *
 * @return BL_Err_Type
 *
*******************************************************************************/
BL_Err_Type pm_pds_wakeup_src_en(uint32_t WakeupType)
{
    return PDS_Wakeup_Src_En((PDS_WAKEUP_SRC_Type)WakeupType,ENABLE);
}

/****************************************************************************/ /**
 * @brief  pm pds get wakeup src
 *
 * @param  WakeupType
 *
 * @return BL_Sts_Type
 *
*******************************************************************************/
BL_Sts_Type pm_pds_get_wakeup_src(uint32_t WakeupType)
{
    return PDS_Get_Wakeup_Src(WakeupType);
}

/****************************************************************************/ /**
 * @brief  pm pds mask all wakeup src
 *
 * @param  none
 *
 * @return None
 *
*******************************************************************************/
void pm_pds_mask_all_wakeup_src(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_CLR_REG_BIT(tmpVal, PDS_CR_PDS_WAKEUP_SRC_EN);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal = BL_SET_REG_BIT(tmpVal, PDS_CR_PDS_RF_DONE_INT_MASK);
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);
}

/****************************************************************************/ /**
 * @brief  pm pds enable
 *
 * @param  cfg:
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION pm_pds_enable(uint32_t *cfg)
{
    uint32_t tmpVal = 0;
    PM_PDS_CFG_Type *p = (PM_PDS_CFG_Type *)cfg;
    PDS_DEFAULT_LV_CFG_Type *pPdsCfg = NULL;
    uintptr_t irq_flag;

    HBN_32K_Sel(HBN_32K_RC);
    HBN_Set_Ldo11_All_Vout(p->ldoLevel);

    /* To make it simple and safe*/
    irq_flag = bflb_irq_save();

    // PDS_WAKEUP_IRQHandler_Install();
    /* CLear HBN RTC INT Status */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_CTL);
    tmpVal &= ~0xE;
    BL_WR_REG(HBN_BASE, HBN_CTL, tmpVal);
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, 0xffffffff);
    BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, 0);

    tmpVal = BL_RD_REG(PDS_BASE, PDS_INT);
    tmpVal &= ~(1 << 8); //unmask pds wakeup
    BL_WR_REG(PDS_BASE, PDS_INT, tmpVal);

    if (p->sleepTime) {
        /* enable hbn(rtc\acomp\aonio) wakeup for pds */
        PDS_Wakeup_Src_En(PDS_WAKEUP_SRC_HBN_IRQ_OUT,ENABLE);
    }

    PDS_Set_All_GPIO_IntClear();
    PDS_IntClear();

    /* enable PDS interrupt to wakeup CPU (PDS1:CPU not powerdown, CPU __WFI) */
    CPU_Interrupt_Enable(PDS_WAKEUP_IRQn);

    switch (p->pdsLevel) {
        case PM_PDS_LEVEL_31:
            pPdsCfg = &pdsCfgLevel31;
            break;
        default:
            return;
    }

    if (p->powerDownFlash) {
        HBN_Power_Down_Flash((spi_flash_cfg_type *)p->flashCfg);
        /* turn_off_ext_flash_pin, GPIO23 - GPIO28 */
        // for (uint32_t pin = 23; pin <= 28; pin++) {
        //     GLB_GPIO_Set_HZ(pin);
        // }

        /* SF io select from efuse value */
#if 0 //TODO
        SF_Ctrl_Pin_Select sf_pin_select = 0;
        /* get sw uasge 0 */
        EF_Ctrl_Read_Sw_Usage(0, (uint32_t *)&tmpVal);
        /* get flash type */
        sf_pin_select = (tmpVal >> 14) & 0x3f;
#endif
    }

    if (p->turnoffDLL) {
        GLB_Set_System_CLK(GLB_DLL_XTAL_NONE, GLB_SYS_CLK_RC32M);
        AON_Power_Off_XTAL();
        GLB_Power_Off_DLL();
        PDS_Update_Flash_Ctrl_Setting(0);
    }

    if (p->pdsLdoEn) {
        pPdsCfg->pdsCtl.pdsLdoVselEn = 1;
        /* PDS mode LDO level, if cr_pds_ldo_vsel_en =1 */
        HBN_Set_Ldo11_Aon_Vout(p->ldoLevel);
    }

    if (p->ocramRetetion) {
        uint32_t pds_ram = PM_OCRAM_RET_VAL;
        PDS_RAM_CFG_Type *ramCfg = (PDS_RAM_CFG_Type *)&pds_ram;
        PDS_RAM_Config(ramCfg);
    }

    if (!(p->turnOffRF)) {
        pPdsCfg->pdsCtl.pdsCtlRfSel = 0;
    }

    if( p->ioKeepSel ) {
        pPdsCfg->pdsCtl.swGpioIsoMod = 1;
        pPdsCfg->pdsCtl.pdsGpioKeep = p->ioKeepSel;
        pPdsCfg->pdsCtl.pdsGpioIsoMod = 1;
    }


    /* config  ldo11soc_sstart_delay_aon =2 , cr_pds_pd_ldo11=0 to speedup ldo11soc_rdy_aon */
    AON_Set_LDO11_SOC_Sstart_Delay(0x2);

    PDS_Default_Level_Config(pPdsCfg, p->sleepTime);

    __WFI(); /* if(.wfiMask==0){CPU won't power down until PDS module had seen __wfi} */

    bflb_irq_restore(irq_flag);

    if (p->turnoffDLL) {
        HBN_Set_XCLK_CLK_Sel(1);
        GLB_Set_System_CLK(GLB_DLL_XTAL_32M, GLB_SYS_CLK_DLL128M);
        PDS_Update_Flash_Ctrl_Setting(1);
    }

    if (p->powerDownFlash) {
        pm_power_on_flash(p->flashCfg, p->flashContRead);
    }
}

void pm_pds_mode_enter(enum pm_pds_sleep_level pds_level, uint32_t sleep_time)
{
    PM_PDS_CFG_Type cfg = {
        .pdsLevel = pds_level,                    /*!< PDS level */
        .turnOffRF = PM_PDS_RF_POWER_OFF,         /*!< Wheather turn off RF */
        .powerDownFlash = PM_PDS_FLASH_POWER_OFF, /*!< Whether power down flash */
        .ocramRetetion = 1,                       /*!< Whether OCRAM Retention */
        .turnoffDLL = PM_PDS_DLL_POWER_OFF,       /*!< Whether trun off PLL */
        .pdsLdoEn = 1,                            /*!< Whether enable PDS control LDO */
        .flashContRead = 0,                       /*!< Whether enable flash continue read */
        .ioKeepSel = PM_PDS_IO_KEEP_VAL,          /*!< PDS io keep select */
        .sleepTime = sleep_time,                  /*!< PDS sleep time */
        .flashCfg = NULL,                         /*!< Flash config pointer, used when power down flash */
        .ldoLevel = PM_PDS_LDO_LEVEL_DEFAULT,     /*!< LDO level */
        .preCbFun = NULL,                         /*!< Pre callback function */
        .postCbFun = NULL,                        /*!< Post callback function */
    };

    uint32_t flash_cfg_len;
    bflb_flash_get_cfg((uint8_t **)&cfg.flashCfg, &flash_cfg_len);

    pm_pds_enable((uint32_t *)&cfg);
}

void pm_hbn_mode_enter(enum pm_hbn_sleep_level hbn_level, uint32_t sleep_time)
{
    HBN_APP_CFG_Type cfg = {
        .useXtal32k = 0,                      /*!< Whether use xtal 32K as 32K clock source,otherwise use rc32k */
        .sleepTime = sleep_time,              /*!< HBN sleep time */
        .hw_pu_pd_en = 1,                     /*!< Always disable HBN pin pull up/down to reduce PDS/HBN current */
        .flashCfg = NULL,                     /*!< Flash config pointer, used when power down flash */
        .hbnLevel = hbn_level,                /*!< HBN level */
        .ldoLevel = PM_HBN_LDO_LEVEL_DEFAULT, /*!< LDO level */
    };

    uint32_t flash_cfg_len;
    bflb_flash_get_cfg((uint8_t **)&cfg.flashCfg, &flash_cfg_len);

    HBN_Mode_Enter(&cfg);
}

void pm_pds_io_wakeup_en(uint32_t pin,int trigMode, int pu, int pd)
{
    PDS_GPIO_Cfg_Type cfg={
        .pu_en = pu,
        .pd_en = pd,
        .ie_en = 1,
        .oe_en = 0,
    };

    uint32_t tmpVal;

    PDS_GPIO_GROUP_SET_Type grp = (PDS_GPIO_GROUP_SET_Type)(pin/4);

    PDS_Set_GPIO_Pad_IntMask(grp,MASK);

    /* ENABL all pin's ie of GROUP */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_GPIO_IE_SET);
    tmpVal |= ((uint32_t)0xF << grp);
    BL_WR_REG(PDS_BASE, PDS_GPIO_IE_SET, tmpVal);

    PDS_Set_GPIO_Pad_Cfg((PDS_GPIO_Type)pin,&cfg);

    PDS_Set_GPIO_Pad_IntMode(grp,(PDS_GPIO_INT_TRIG_Type)trigMode);
    PDS_Set_GPIO_Pad_IntClr(grp);

    PDS_Set_GPIO_Pad_IntMask(grp,UNMASK);

    PDS_Wakeup_Src_En(PDS_WAKEUP_SRC_PDS_IO_INT,ENABLE);
}

void pm_hbn_io_wakeup_en(uint32_t pin,int trigMode, int pu, int pd)
{
    uint32_t mask = 0;
    uint32_t mask_bit = 0;

    HBN_AON_PAD_CFG_Type aonPadCfg={
        .ctrlEn = 1,
        .ie = 1,
        .pullUp = pu,
        .pullDown = pd,
        .oe = 0,
    };

    if( (pin>=9) && (pin<=13) ) {  /* AONIO 9~13*/
        HBN_Aon_Pad_Cfg(1,(HBN_AON_PAD_Type)(pin-9),&aonPadCfg);
        mask_bit = pin - 9;
    } else if( (pin>=30) && (pin<=31) ) {  /* AONIO 30~31*/
        HBN_Aon_Pad_Cfg(1,(HBN_AON_PAD_Type)(pin-25),&aonPadCfg);
        mask_bit = pin - 25;
    } else {
    }

    mask = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    mask = BL_GET_REG_BITS_VAL(mask,HBN_PIN_WAKEUP_MASK);
    mask = mask & ~(1 << mask_bit);

    /* set trigMode */
    HBN_GPIO_Wakeup_Set((~mask),(HBN_GPIO_INT_Trigger_Type)trigMode);

    /* UnMask Hbn_Irq Wakeup PDS*/
    PDS_Wakeup_Src_En(PDS_WAKEUP_SRC_HBN_IRQ_OUT,ENABLE);

}

BL_Err_Type pm_set_io_keep(uint32_t pin,uint32_t value)
{
    if( (pin>=4) && (pin<=6) ) { /* 702l don't have gpio4~6 */
        return ERROR;
    }

    PDS_GPIO_Cfg_Type pdsPadcfg={
        .pu_en = 0,
        .pd_en = 0,
        .ie_en = 1,
        .oe_en = 0,
    };

    HBN_AON_PAD_CFG_Type aonPadCfg={
        .ctrlEn = 1,
        .ie = 1,
        .pullUp = 0,
        .pullDown = 0,
        .oe = 0,
    };

    if( value ) {
        pdsPadcfg.pu_en = 1;
        aonPadCfg.pullUp = 1;
    } else {
        pdsPadcfg.pd_en = 1;
        aonPadCfg.pullDown = 1;
    }

    if( (pin>=9) && (pin<=13) ) {  /* AONIO 9~13*/
        HBN_Aon_Pad_Cfg(1,(HBN_AON_PAD_Type)(pin-9),&aonPadCfg);
    } else if( (pin>=30) && (pin<=31) ) {  /* AONIO 30~31*/
        HBN_Aon_Pad_Cfg(1,(HBN_AON_PAD_Type)(pin-25),&aonPadCfg);
    } else { /* PDSIO */
        PDS_Set_GPIO_Pad_Cfg((PDS_GPIO_Type)pin,&pdsPadcfg);

    }

    return SUCCESS;
}

uint32_t pm_acomp_wakeup_en(uint8_t acompNo, uint8_t pin, uint8_t pos_edge_en, uint8_t neg_edge_en)
{
    uint32_t chId=0;
    struct bflb_acomp_config_s acompCfg = {
        .mux_en = ENABLE,
        .pos_chan_sel = AON_ACOMP_CHAN_ADC0,
        .neg_chan_sel = AON_ACOMP_CHAN_VIO_X_SCALING_FACTOR_1,
        .vio_sel = 33, //1.65v
        .scaling_factor = AON_ACOMP_SCALING_FACTOR_1,
        .bias_prog = AON_ACOMP_BIAS_POWER_MODE1,
        .hysteresis_pos_volt = AON_ACOMP_HYSTERESIS_VOLT_NONE,
        .hysteresis_neg_volt = AON_ACOMP_HYSTERESIS_VOLT_NONE,
    };

    if( SUCCESS != bflb_acomp_gpio_2_chanid((uint32_t)pin,&chId) ) {
        return -1;
    } else {
        acompCfg.pos_chan_sel = chId;
    }

    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    /* Config Gpio as Analog */
    bflb_gpio_init(gpio, pin, GPIO_ANALOG | GPIO_PULL_NONE | GPIO_DRV_0);

    /* Config Comp0/1 */
    bflb_acomp_init(acompNo, &acompCfg);
    bflb_acomp_enable(acompNo);

    HBN_Clear_IRQ(HBN_INT_ACOMP0 + acompNo * 2);

    /* enable/disable POSEDGE */
    if (pos_edge_en) {
        HBN_Enable_AComp_IRQ(acompNo, HBN_ACOMP_INT_EDGE_POSEDGE);
    } else {
        HBN_Disable_AComp_IRQ(acompNo, HBN_ACOMP_INT_EDGE_POSEDGE);
    }

    /* enable/disable NEGEDGE */
    if (neg_edge_en) {
        HBN_Enable_AComp_IRQ(acompNo, HBN_ACOMP_INT_EDGE_NEGEDGE);
    } else {
        HBN_Disable_AComp_IRQ(acompNo, HBN_ACOMP_INT_EDGE_NEGEDGE);
    }

    /* UnMask Hbn_Irq Wakeup PDS*/
    PDS_Wakeup_Src_En(PDS_WAKEUP_SRC_HBN_IRQ_OUT,ENABLE);

    return 0;
}