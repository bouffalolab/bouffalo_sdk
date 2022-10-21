#include "bl616_romdriver_e907.h"


__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_Off_BG(void){
    return RomDriver_AON_Power_Off_BG();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_Off_LDO15_RF(void){
    return RomDriver_AON_Power_Off_LDO15_RF();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_Off_MBG(void){
    return RomDriver_AON_Power_Off_MBG();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_Off_SFReg(void){
    return RomDriver_AON_Power_Off_SFReg();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_Off_XTAL(void){
    return RomDriver_AON_Power_Off_XTAL();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_On_BG(void){
    return RomDriver_AON_Power_On_BG();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_On_LDO15_RF(void){
    return RomDriver_AON_Power_On_LDO15_RF();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_On_MBG(void){
    return RomDriver_AON_Power_On_MBG();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_On_SFReg(void){
    return RomDriver_AON_Power_On_SFReg();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_On_XTAL(void){
    return RomDriver_AON_Power_On_XTAL();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Set_Xtal_CapCode(uint8_t capIn, uint8_t capOut){
    return RomDriver_AON_Set_Xtal_CapCode(capIn,capOut);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Trim_DcdcDis(void){
    return RomDriver_AON_Trim_DcdcDis();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Trim_DcdcVoutSel(void){
    return RomDriver_AON_Trim_DcdcVoutSel();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Trim_DcdcVoutTrim(void){
    return RomDriver_AON_Trim_DcdcVoutTrim();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Trim_Ldo11socVoutTrim(void){
    return RomDriver_AON_Trim_Ldo11socVoutTrim();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Trim_Usb20RcalCode(void){
    return RomDriver_AON_Trim_Usb20RcalCode();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t AON_Get_Xtal_CapCode(void){
    return RomDriver_AON_Get_Xtal_CapCode();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type CPU_MTimer_Delay_MS(uint32_t cnt){
    return RomDriver_CPU_MTimer_Delay_MS(cnt);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type CPU_MTimer_Delay_US(uint32_t cnt){
    return RomDriver_CPU_MTimer_Delay_US(cnt);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type CPU_Reset_MTimer(void){
    return RomDriver_CPU_Reset_MTimer();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type CPU_Set_MTimer_CLK(uint8_t enable, BL_MTimer_Source_Clock_Type mTimerSourceClockType, uint16_t div){
    return RomDriver_CPU_Set_MTimer_CLK(enable,mTimerSourceClockType,div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type CPU_Set_MTimer_RST(uint8_t rstEn){
    return RomDriver_CPU_Set_MTimer_RST(rstEn);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t CPU_Get_MTimer_Clock(void){
    return RomDriver_CPU_Get_MTimer_Clock();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t CPU_Get_MTimer_Source_Clock(void){
    return RomDriver_CPU_Get_MTimer_Source_Clock();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t Clock_Audio_ADC_Clock_Get(void){
    return RomDriver_Clock_Audio_ADC_Clock_Get();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t Clock_Get_ISP_Clk(void){
    return RomDriver_Clock_Get_ISP_Clk();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t Clock_Get_PSRAMB_Clk(void){
    return RomDriver_Clock_Get_PSRAMB_Clk();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t Clock_Peripheral_Clock_Get(BL_Peripheral_Type type){
    return RomDriver_Clock_Peripheral_Clock_Get(type);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t Clock_System_Clock_Get(BL_System_Clock_Type type){
    return RomDriver_Clock_System_Clock_Get(type);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t SystemCoreClockGet(void){
    return RomDriver_SystemCoreClockGet();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint64_t CPU_Get_CPU_Cycle(void){
    return RomDriver_CPU_Get_CPU_Cycle();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint64_t CPU_Get_MTimer_Counter(void){
    return RomDriver_CPU_Get_MTimer_Counter();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint64_t CPU_Get_MTimer_MS(void){
    return RomDriver_CPU_Get_MTimer_MS();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint64_t CPU_Get_MTimer_US(void){
    return RomDriver_CPU_Get_MTimer_US();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type EF_Ctrl_Crc_Result(void){
    return RomDriver_EF_Ctrl_Crc_Result();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type EF_Ctrl_Get_Customer_PIDVID(uint16_t pid[1], uint16_t vid[1]){
    return RomDriver_EF_Ctrl_Get_Customer_PIDVID(pid,vid);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type EF_Ctrl_Read_Chip_ID(uint8_t chipID[8]){
    return RomDriver_EF_Ctrl_Read_Chip_ID(chipID);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type EF_Ctrl_Read_MAC_Address(uint8_t mac[6]){
    return RomDriver_EF_Ctrl_Read_MAC_Address(mac);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type EF_Ctrl_Read_MAC_Address_Raw(uint8_t mac[7]){
    return RomDriver_EF_Ctrl_Read_MAC_Address_Raw(mac);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type EF_Ctrl_AutoLoad_Done(void){
    return RomDriver_EF_Ctrl_AutoLoad_Done();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type EF_Ctrl_Busy(void){
    return RomDriver_EF_Ctrl_Busy();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type EF_Ctrl_Crc_Is_Busy(void){
    return RomDriver_EF_Ctrl_Crc_Is_Busy();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t EF_Ctrl_Get_Trim_Parity(uint32_t val, uint8_t len){
    return RomDriver_EF_Ctrl_Get_Trim_Parity(val,len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t EF_Ctrl_Is_All_Bits_Zero(uint32_t val, uint8_t start, uint8_t len){
    return RomDriver_EF_Ctrl_Is_All_Bits_Zero(val,start,len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t EF_Ctrl_Read_FlashDlyCoe(void){
    return RomDriver_EF_Ctrl_Read_FlashDlyCoe();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Clear(uint32_t index, uint32_t len){
    return RomDriver_EF_Ctrl_Clear(index,len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Crc_Enable(void){
    return RomDriver_EF_Ctrl_Crc_Enable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Crc_Set_Golden(uint32_t goldenValue){
    return RomDriver_EF_Ctrl_Crc_Set_Golden(goldenValue);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Load_Efuse_R0(void){
    return RomDriver_EF_Ctrl_Load_Efuse_R0();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Program_Direct_R0(uint32_t index, uint32_t *data, uint32_t len){
    return RomDriver_EF_Ctrl_Program_Direct_R0(index,data,len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Program_Efuse_0(void){
    return RomDriver_EF_Ctrl_Program_Efuse_0();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Read_ADC_Gain_Trim(Efuse_ADC_Gain_Coeff_Type *trim){
    return RomDriver_EF_Ctrl_Read_ADC_Gain_Trim(trim);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Read_AES_Key(uint8_t index, uint32_t *keyData, uint32_t len){
    return RomDriver_EF_Ctrl_Read_AES_Key(index,keyData,len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Read_Dbg_Pwd(uint8_t slot, uint32_t *passWdLow, uint32_t *passWdHigh){
    return RomDriver_EF_Ctrl_Read_Dbg_Pwd(slot,passWdLow,passWdHigh);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Read_DcdcDis_Trim(Efuse_Ana_DcdcDis_Type *trim){
    return RomDriver_EF_Ctrl_Read_DcdcDis_Trim(trim);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Read_DcdcVoutSel_Trim(Efuse_Ana_DcdcVoutSel_Type *trim){
    return RomDriver_EF_Ctrl_Read_DcdcVoutSel_Trim(trim);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Read_DcdcVoutTrim_Trim(Efuse_Ana_DcdcVoutTrim_Type *trim){
    return RomDriver_EF_Ctrl_Read_DcdcVoutTrim_Trim(trim);
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Read_Device_Info(Efuse_Device_Info_Type *deviceInfo){
    return RomDriver_EF_Ctrl_Read_Device_Info(deviceInfo);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Read_Direct_R0(uint32_t index, uint32_t *data, uint32_t len){
    return RomDriver_EF_Ctrl_Read_Direct_R0(index,data,len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Read_Ldo11socVoutTrim_Trim(Efuse_Ana_Ldo11socVoutTrim_Type *trim){
    return RomDriver_EF_Ctrl_Read_Ldo11socVoutTrim_Trim(trim);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Read_Ldo18ioBypass_Trim(Efuse_Ana_Ldo18ioBypass_Type *trim){
    return RomDriver_EF_Ctrl_Read_Ldo18ioBypass_Trim(trim);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Read_Ldo18ioVoutSel_Trim(Efuse_Ana_Ldo18ioVoutSel_Type *trim){
    return RomDriver_EF_Ctrl_Read_Ldo18ioVoutSel_Trim(trim);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Read_Ldo18ioVoutTrim_Trim(Efuse_Ana_Ldo18ioVoutTrim_Type *trim){
    return RomDriver_EF_Ctrl_Read_Ldo18ioVoutTrim_Trim(trim);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Read_Ldo33VoutTrim_Trim(Efuse_Ana_Ldo33VoutTrim_Type *trim){
    return RomDriver_EF_Ctrl_Read_Ldo33VoutTrim_Trim(trim);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Read_RC32K_Trim(Efuse_Ana_RC32K_Trim_Type *trim){
    return RomDriver_EF_Ctrl_Read_RC32K_Trim(trim);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Read_RC32M_Trim(Efuse_Ana_RC32M_Trim_Type *trim){
    return RomDriver_EF_Ctrl_Read_RC32M_Trim(trim);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Read_Secure_Boot(EF_Ctrl_SF_AES_Type aes[2]){
    return RomDriver_EF_Ctrl_Read_Secure_Boot(aes);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Read_Secure_Cfg(EF_Ctrl_Sec_Param_Type *cfg){
    return RomDriver_EF_Ctrl_Read_Secure_Cfg(cfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Read_Sw_Usage(uint32_t index, uint32_t *usage){
    return RomDriver_EF_Ctrl_Read_Sw_Usage(index,usage);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Read_TSEN_Trim(Efuse_TSEN_Refcode_Corner_Type *trim){
    return RomDriver_EF_Ctrl_Read_TSEN_Trim(trim);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Read_Usb20RcalCode_Trim(Efuse_Ana_Usb20RcalCode_Type *trim){
    return RomDriver_EF_Ctrl_Read_Usb20RcalCode_Trim(trim);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Readlock_AES_Key(uint8_t index, uint8_t program){
    return RomDriver_EF_Ctrl_Readlock_AES_Key(index,program);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Readlock_Dbg_Pwd(uint8_t program){
    return RomDriver_EF_Ctrl_Readlock_Dbg_Pwd(program);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Set_sf_key_re_sel(uint8_t ef_sf_key_re_sel){
    return RomDriver_EF_Ctrl_Set_sf_key_re_sel(ef_sf_key_re_sel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Sw_AHB_Clk_0(void){
    return RomDriver_EF_Ctrl_Sw_AHB_Clk_0();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Write_AES_Key(uint8_t index, uint32_t *keyData, uint32_t len, uint8_t program){
    return RomDriver_EF_Ctrl_Write_AES_Key(index,keyData,len,program);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Write_Dbg_Pwd(uint8_t slot, uint32_t passWdLow, uint32_t passWdHigh, uint8_t program){
    return RomDriver_EF_Ctrl_Write_Dbg_Pwd(slot,passWdLow,passWdHigh,program);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Write_MAC_Address(uint8_t mac[6], uint8_t program){
    return RomDriver_EF_Ctrl_Write_MAC_Address(mac,program);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Write_Secure_Boot(EF_Ctrl_Sign_Type sign[1], EF_Ctrl_SF_AES_Type aes[1], uint8_t program){
    return RomDriver_EF_Ctrl_Write_Secure_Boot(sign,aes,program);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Write_Secure_Cfg(EF_Ctrl_Sec_Param_Type *cfg, uint8_t program){
    return RomDriver_EF_Ctrl_Write_Secure_Cfg(cfg,program);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Write_Sw_Usage(uint32_t index, uint32_t usage, uint8_t program){
    return RomDriver_EF_Ctrl_Write_Sw_Usage(index,usage,program);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Writelock_AES_Key(uint8_t index, uint8_t program){
    return RomDriver_EF_Ctrl_Writelock_AES_Key(index,program);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Writelock_Dbg_Pwd(uint8_t program){
    return RomDriver_EF_Ctrl_Writelock_Dbg_Pwd(program);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Writelock_MAC_Address(uint8_t program){
    return RomDriver_EF_Ctrl_Writelock_MAC_Address(program);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void EF_Ctrl_Writelock_Sw_Usage(uint32_t index, uint8_t program){
    return RomDriver_EF_Ctrl_Writelock_Sw_Usage(index,program);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_Type swrst){
    return RomDriver_GLB_AHB_MCU_Software_Reset(swrst);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_AUPLL_Ref_Clk_Sel(GLB_PLL_REF_CLK_Type refClk){
    return RomDriver_GLB_AUPLL_Ref_Clk_Sel(refClk);
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_BMX_TO_Init(BMX_TO_Cfg_Type *BmxCfg){
    return RomDriver_GLB_BMX_TO_Init(BmxCfg);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Clr_BMX_TO_Status(void){
    return RomDriver_GLB_Clr_BMX_TO_Status();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Clr_EMI_Reset_Gate(void){
    return RomDriver_GLB_Clr_EMI_Reset_Gate();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Clr_Reset_Reason(void){
    return RomDriver_GLB_Clr_Reset_Reason();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Config_SDIO_Host_Interrupt_CPU(uint8_t enable){
    return RomDriver_GLB_Config_SDIO_Host_Interrupt_CPU(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Config_SDIO_Host_Reset_SDU(uint8_t enable){
    return RomDriver_GLB_Config_SDIO_Host_Reset_SDU(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Config_SDIO_Host_Reset_System(uint8_t enable){
    return RomDriver_GLB_Config_SDIO_Host_Reset_System(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Get_Auto_Calc_Xtal_Type(GLB_XTAL_Type *calcXtalType){
    return RomDriver_GLB_Get_Auto_Calc_Xtal_Type(calcXtalType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Get_MCU_System_CLK_Div(uint8_t *mcuClkDiv, uint8_t *mcuPBclkDiv){
    return RomDriver_GLB_Get_MCU_System_CLK_Div(mcuClkDiv,mcuPBclkDiv);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Get_Reset_Reason(GLB_RESET_RECORD_Type *reason){
    return RomDriver_GLB_Get_Reset_Reason(reason);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_IR_RX_GPIO_Sel(GLB_GPIO_Type gpio){
    return RomDriver_GLB_IR_RX_GPIO_Sel(gpio);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Invert_ETH_REF_O_CLK(uint8_t enable){
    return RomDriver_GLB_Invert_ETH_REF_O_CLK(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Invert_ETH_RX_CLK(uint8_t enable){
    return RomDriver_GLB_Invert_ETH_RX_CLK(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Invert_ETH_TX_CLK(uint8_t enable){
    return RomDriver_GLB_Invert_ETH_TX_CLK(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_MCU_SW_System_Reset(GLB_MCU_SW_SYSTEM_Type sysPart){
    return RomDriver_GLB_MCU_SW_System_Reset(sysPart);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_PER_Clock_Gate(uint64_t ips){
    return RomDriver_GLB_PER_Clock_Gate(ips);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_PER_Clock_UnGate(uint64_t ips){
    return RomDriver_GLB_PER_Clock_UnGate(ips);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_PLL_CGEN_Clock_Gate(GLB_PLL_CGEN_Type clk){
    return RomDriver_GLB_PLL_CGEN_Clock_Gate(clk);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_PLL_CGEN_Clock_UnGate(GLB_PLL_CGEN_Type clk){
    return RomDriver_GLB_PLL_CGEN_Clock_UnGate(clk);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Platform_Wakeup_PDS_Enable(uint8_t enable){
    return RomDriver_GLB_Platform_Wakeup_PDS_Enable(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Power_Off_AUPLL(void){
    return RomDriver_GLB_Power_Off_AUPLL();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Power_Off_WIFIPLL(void){
    return RomDriver_GLB_Power_Off_WIFIPLL();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Power_On_AUPLL(const GLB_WA_PLL_Cfg_Type *const cfg, uint8_t waitStable){
    return RomDriver_GLB_Power_On_AUPLL(cfg,waitStable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Power_On_WIFIPLL(const GLB_WA_PLL_Cfg_Type *const cfg, uint8_t waitStable){
    return RomDriver_GLB_Power_On_WIFIPLL(cfg,waitStable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Power_On_XTAL_And_PLL_CLK(GLB_XTAL_Type xtalType, GLB_PLL_Type pllType){
    return RomDriver_GLB_Power_On_XTAL_And_PLL_CLK(xtalType,pllType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_SPI_Sig_Swap_Set(GLB_SPI_SIG_SWAP_GRP_Type group, uint8_t swap){
    return RomDriver_GLB_SPI_Sig_Swap_Set(group,swap);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_SW_CPU_Reset(void){
    return RomDriver_GLB_SW_CPU_Reset();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_SW_POR_Reset(void){
    return RomDriver_GLB_SW_POR_Reset();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_SW_System_Reset(void){
    return RomDriver_GLB_SW_System_Reset();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Sel_MCU_TMR_GPIO_Clock(GLB_GPIO_Type gpioPin){
    return RomDriver_GLB_Sel_MCU_TMR_GPIO_Clock(gpioPin);
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_ADC_CLK(uint8_t enable, GLB_ADC_CLK_Type clkSel, uint8_t div){
    return RomDriver_GLB_Set_ADC_CLK(enable,clkSel,div);
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_Audio_ADC_CLK(uint8_t enable, uint8_t div){
    return RomDriver_GLB_Set_Audio_ADC_CLK(enable,div);
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_Audio_AUTO_CLK(uint8_t divEn){
    return RomDriver_GLB_Set_Audio_AUTO_CLK(divEn);
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_Audio_SOLO_CLK(uint8_t enable, uint8_t div){
    return RomDriver_GLB_Set_Audio_SOLO_CLK(enable,div);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_Auto_Calc_Xtal_Type(GLB_XTAL_Type calcXtalType){
    return RomDriver_GLB_Set_Auto_Calc_Xtal_Type(calcXtalType);
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_CAM_CLK(uint8_t enable, GLB_CAM_CLK_Type clkSel, uint8_t div){
    return RomDriver_GLB_Set_CAM_CLK(enable,clkSel,div);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_CPU_Reset_Address(GLB_CORE_ID_Type coreID, uint32_t addr){
    return RomDriver_GLB_Set_CPU_Reset_Address(coreID,addr);
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_DBI_CLK(uint8_t enable, GLB_DBI_CLK_Type clkSel, uint8_t div){
    return RomDriver_GLB_Set_DBI_CLK(enable,clkSel,div);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_DIG_32K_CLK(uint8_t enable, uint8_t compensationEn, uint16_t div){
    return RomDriver_GLB_Set_DIG_32K_CLK(enable,compensationEn,div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_DIG_512K_CLK(uint8_t enable, uint8_t compensationEn, uint8_t div){
    return RomDriver_GLB_Set_DIG_512K_CLK(enable,compensationEn,div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_DIG_CLK_Sel(GLB_DIG_CLK_Type clkSel){
    return RomDriver_GLB_Set_DIG_CLK_Sel(clkSel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_DMA_CLK(uint8_t enable, GLB_DMA_CLK_ID_Type clk){
    return RomDriver_GLB_Set_DMA_CLK(enable,clk);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_EM_Sel(GLB_EM_Type emType){
    return RomDriver_GLB_Set_EM_Sel(emType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_ETH_REF_O_CLK_Sel(GLB_ETH_REF_CLK_OUT_Type clkSel){
    return RomDriver_GLB_Set_ETH_REF_O_CLK_Sel(clkSel);
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_I2C_CLK(uint8_t enable, GLB_I2C_CLK_Type clkSel, uint8_t div){
    return RomDriver_GLB_Set_I2C_CLK(enable,clkSel,div);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_I2S_CLK(uint8_t refClkEn, uint8_t refClkDiv, GLB_I2S_DI_REF_CLK_Type inRef, GLB_I2S_DO_REF_CLK_Type outRef){
    return RomDriver_GLB_Set_I2S_CLK(refClkEn,refClkDiv,inRef,outRef);
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_IR_CLK(uint8_t enable, GLB_IR_CLK_SRC_Type clkSel, uint8_t div){
    return RomDriver_GLB_Set_IR_CLK(enable,clkSel,div);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_ISP_Muxpll_80M_Sel(GLB_ISP_MUXPLL_80M_CLK_SEL_Type clkSel){
    return RomDriver_GLB_Set_ISP_Muxpll_80M_Sel(clkSel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_MCU_Muxpll_160M_Sel(GLB_MCU_MUXPLL_160M_CLK_SEL_Type clkSel){
    return RomDriver_GLB_Set_MCU_Muxpll_160M_Sel(clkSel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_MCU_Muxpll_80M_Sel(GLB_MCU_MUXPLL_80M_CLK_SEL_Type clkSel){
    return RomDriver_GLB_Set_MCU_Muxpll_80M_Sel(clkSel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_MCU_SPI_0_ACT_MOD_Sel(GLB_SPI_PAD_ACT_AS_Type mod){
    return RomDriver_GLB_Set_MCU_SPI_0_ACT_MOD_Sel(mod);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_Type clkFreq){
    return RomDriver_GLB_Set_MCU_System_CLK(clkFreq);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_MCU_System_CLK_Div(uint8_t mcuClkDiv, uint8_t mcuPBclkDiv){
    return RomDriver_GLB_Set_MCU_System_CLK_Div(mcuClkDiv,mcuPBclkDiv);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_PDM_IO_Sel(GLB_PDM_IO_SEL_Type ioSel){
    return RomDriver_GLB_Set_PDM_IO_Sel(ioSel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_PKA_CLK_Sel(GLB_PKA_CLK_Type clkSel){
    return RomDriver_GLB_Set_PKA_CLK_Sel(clkSel);
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_PSRAMB_CLK_Sel(uint8_t enable, GLB_PSRAMB_PLL_Type clkSel, uint8_t div){
    return RomDriver_GLB_Set_PSRAMB_CLK_Sel(enable,clkSel,div);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_PWM1_IO_Sel(GLB_PWM1_IO_SEL_Type ioSel){
    return RomDriver_GLB_Set_PWM1_IO_Sel(ioSel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_Peripheral_DMA_CN(GLB_PERI_DMA_Type peri, GLB_PERI_DMA_CN_SEL_Type cn){
    return RomDriver_GLB_Set_Peripheral_DMA_CN(peri,cn);
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_SDH_CLK(uint8_t enable, GLB_SDH_CLK_Type clkSel, uint8_t div){
    return RomDriver_GLB_Set_SDH_CLK(enable,clkSel,div);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_SF_CLK(uint8_t enable, GLB_SFLASH_CLK_Type clkSel, uint8_t div){
    return RomDriver_GLB_Set_SF_CLK(enable,clkSel,div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_SFlash2_IO_PARM(uint8_t swapIo3Io0){
    return RomDriver_GLB_Set_SFlash2_IO_PARM(swapIo3Io0);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_SFlash_IO_PARM(uint8_t selEmbedded, uint8_t swapIo3Io0, uint8_t swapIo2Cs){
    return RomDriver_GLB_Set_SFlash_IO_PARM(selEmbedded,swapIo3Io0,swapIo2Cs);
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_SPI_CLK(uint8_t enable, GLB_SPI_CLK_Type clkSel, uint8_t div){
    return RomDriver_GLB_Set_SPI_CLK(enable,clkSel,div);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_SRAM_PARM(uint32_t value){
    return RomDriver_GLB_Set_SRAM_PARM(value);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_SRAM_RET(uint32_t value){
    return RomDriver_GLB_Set_SRAM_RET(value);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_SRAM_SLP(uint32_t value){
    return RomDriver_GLB_Set_SRAM_SLP(value);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_SSC_CLK_From_WIFIPLL(uint8_t enable){
    return RomDriver_GLB_Set_SSC_CLK_From_WIFIPLL(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_Slave_Grp_0_CLK(GLB_SLAVE_GRP_0_Type slave, uint8_t enable, GLB_SLAVE_GRP_0_CLK_SEL_Type clkSel, uint32_t div){
    return RomDriver_GLB_Set_Slave_Grp_0_CLK(slave,enable,clkSel,div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_UART_CLK(uint8_t enable, HBN_UART_CLK_Type clkSel, uint8_t div){
    return RomDriver_GLB_Set_UART_CLK(enable,clkSel,div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_USB_CLK_From_WIFIPLL(uint8_t enable){
    return RomDriver_GLB_Set_USB_CLK_From_WIFIPLL(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Swap_MCU_SPI_0_MOSI_With_MISO(BL_Fun_Type newState){
    return RomDriver_GLB_Swap_MCU_SPI_0_MOSI_With_MISO(newState);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Trim_Ldo18ioBypass(void){
    return RomDriver_GLB_Trim_Ldo18ioBypass();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Trim_Ldo18ioVoutSel(void){
    return RomDriver_GLB_Trim_Ldo18ioVoutSel();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Trim_Ldo18ioVoutTrim(void){
    return RomDriver_GLB_Trim_Ldo18ioVoutTrim();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_UART_Fun_Sel(GLB_UART_SIG_Type sig, GLB_UART_SIG_FUN_Type fun){
    return RomDriver_GLB_UART_Fun_Sel(sig,fun);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_UART_Sig_Swap_Set(GLB_UART_SIG_SWAP_GRP_Type group, uint8_t swap){
    return RomDriver_GLB_UART_Sig_Swap_Set(group,swap);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_WIFIPLL_Ref_Clk_Sel(GLB_PLL_REF_CLK_Type refClk){
    return RomDriver_GLB_WIFIPLL_Ref_Clk_Sel(refClk);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
GLB_CORE_ID_Type GLB_Get_Core_Type(void){
    return RomDriver_GLB_Get_Core_Type();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
GLB_MCU_MUXPLL_160M_CLK_SEL_Type GLB_Get_MCU_Muxpll_160M_Sel(void){
    return RomDriver_GLB_Get_MCU_Muxpll_160M_Sel();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
GLB_MCU_MUXPLL_80M_CLK_SEL_Type GLB_Get_MCU_Muxpll_80M_Sel(void){
    return RomDriver_GLB_Get_MCU_Muxpll_80M_Sel();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t GLB_Get_SRAM_PARM(void){
    return RomDriver_GLB_Get_SRAM_PARM();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t GLB_Get_SRAM_RET(void){
    return RomDriver_GLB_Get_SRAM_RET();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t GLB_Get_SRAM_SLP(void){
    return RomDriver_GLB_Get_SRAM_SLP();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t GLB_Get_BMX_TO_Status(void){
    return RomDriver_GLB_Get_BMX_TO_Status();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Clr_GPIO_IntStatus(GLB_GPIO_Type gpioPin){
    return RomDriver_GLB_Clr_GPIO_IntStatus(gpioPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Embedded_Flash_Pad_Enable(uint8_t swapIo2Cs){
    return RomDriver_GLB_Embedded_Flash_Pad_Enable(swapIo2Cs);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_Clr(GLB_GPIO_Type gpioPin){
    return RomDriver_GLB_GPIO_Clr(gpioPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_Fifo_Clear(void){
    return RomDriver_GLB_GPIO_Fifo_Clear();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_Fifo_Init(GLB_GPIO_FIFO_CFG_Type *cfg){
    return RomDriver_GLB_GPIO_Fifo_Init(cfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_Fifo_IntClear(GLB_GPIO_FIFO_INT_Type intType){
    return RomDriver_GLB_GPIO_Fifo_IntClear(intType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_Fifo_IntMask(GLB_GPIO_FIFO_INT_Type intType, BL_Mask_Type intMask){
    return RomDriver_GLB_GPIO_Fifo_IntMask(intType,intMask);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_Fifo_Push(uint16_t *data, uint16_t len){
    return RomDriver_GLB_GPIO_Fifo_Push(data,len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_Func_Init(GLB_GPIO_FUNC_Type gpioFun, GLB_GPIO_Type *pinList, uint8_t cnt){
    return RomDriver_GLB_GPIO_Func_Init(gpioFun,pinList,cnt);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_Init(GLB_GPIO_Cfg_Type *cfg){
    return RomDriver_GLB_GPIO_Init(cfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_Input_Disable(GLB_GPIO_Type gpioPin){
    return RomDriver_GLB_GPIO_Input_Disable(gpioPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_Input_Enable(GLB_GPIO_Type gpioPin){
    return RomDriver_GLB_GPIO_Input_Enable(gpioPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_IntMask(GLB_GPIO_Type gpioPin, BL_Mask_Type intMask){
    return RomDriver_GLB_GPIO_IntMask(gpioPin,intMask);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_Int_Init(GLB_GPIO_INT_Cfg_Type *intCfg){
    return RomDriver_GLB_GPIO_Int_Init(intCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_Output_Disable(GLB_GPIO_Type gpioPin){
    return RomDriver_GLB_GPIO_Output_Disable(gpioPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_Output_Enable(GLB_GPIO_Type gpioPin){
    return RomDriver_GLB_GPIO_Output_Enable(gpioPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_Set(GLB_GPIO_Type gpioPin){
    return RomDriver_GLB_GPIO_Set(gpioPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_Set_HZ(GLB_GPIO_Type gpioPin){
    return RomDriver_GLB_GPIO_Set_HZ(gpioPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_Write(GLB_GPIO_Type gpioPin, uint32_t val){
    return RomDriver_GLB_GPIO_Write(gpioPin,val);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_Fifo_Disable(void){
    return RomDriver_GLB_GPIO_Fifo_Disable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_Fifo_Enable(void){
    return RomDriver_GLB_GPIO_Fifo_Enable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type GLB_GPIO_Fifo_GetIntStatus(GLB_GPIO_FIFO_INT_Type intType){
    return RomDriver_GLB_GPIO_Fifo_GetIntStatus(intType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type GLB_Get_GPIO_IntStatus(GLB_GPIO_Type gpioPin){
    return RomDriver_GLB_Get_GPIO_IntStatus(gpioPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t GLB_GPIO_Fifo_GetCount(void){
    return RomDriver_GLB_GPIO_Fifo_GetCount();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t GLB_GPIO_Read(GLB_GPIO_Type gpioPin){
    return RomDriver_GLB_GPIO_Read(gpioPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t GLB_GPIO_Get_Fun(GLB_GPIO_Type gpioPin){
    return RomDriver_GLB_GPIO_Get_Fun(gpioPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_32K_Sel(HBN_32K_CLK_Type clkType){
    return RomDriver_HBN_32K_Sel(clkType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Aon_Pad_Cfg(uint8_t aonPadHwCtrlEn, HBN_AON_PAD_Type aonGpio, HBN_AON_PAD_CFG_Type *aonPadCfg){
    return RomDriver_HBN_Aon_Pad_Cfg(aonPadHwCtrlEn,aonGpio,aonPadCfg);
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Aon_Pad_Cfg_Set(uint8_t aonPadHwCtrlEn, HBN_AON_PAD_Type aonGpio){
    return RomDriver_HBN_Aon_Pad_Cfg_Set(aonPadHwCtrlEn,aonGpio);
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Aon_Pad_Ctrl(uint32_t aonPadCtl1, uint32_t aonPadCtl2){
    return RomDriver_HBN_Aon_Pad_Ctrl(aonPadCtl1,aonPadCtl2);
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Aon_Pad_WakeUpCfg(BL_Fun_Type puPdEn, HBN_GPIO_INT_Trigger_Type trigMode, uint32_t maskVal, BL_Fun_Type dlyEn, uint8_t dlySec){
    return RomDriver_HBN_Aon_Pad_WakeUpCfg(puPdEn,trigMode,maskVal,dlyEn,dlySec);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Clear_Gpio_Keep(uint8_t gpioKeep){
    return RomDriver_HBN_Clear_Gpio_Keep(gpioKeep);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Clear_IRQ(HBN_INT_Type irqType){
    return RomDriver_HBN_Clear_IRQ(irqType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Clear_PDS_Gpio_Keep(uint8_t gpioKeep){
    return RomDriver_HBN_Clear_PDS_Gpio_Keep(gpioKeep);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Clear_RTC_Counter(void){
    return RomDriver_HBN_Clear_RTC_Counter();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Clear_RTC_INT(void){
    return RomDriver_HBN_Clear_RTC_INT();
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Disable_AComp0_IRQ(void){
    return RomDriver_HBN_Disable_AComp0_IRQ();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Disable_AComp1_IRQ(void){
    return RomDriver_HBN_Disable_AComp1_IRQ();
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Disable_BOD_IRQ(void){
    return RomDriver_HBN_Disable_BOD_IRQ();
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Enable_AComp0_IRQ(void){
    return RomDriver_HBN_Enable_AComp0_IRQ();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Enable_AComp1_IRQ(void){
    return RomDriver_HBN_Enable_AComp1_IRQ();
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Enable_BOD_IRQ(void){
    return RomDriver_HBN_Enable_BOD_IRQ();
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Enable_RTC_Counter(void){
    return RomDriver_HBN_Enable_RTC_Counter();
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_GPIO_INT_Disable(void){
    return RomDriver_HBN_GPIO_INT_Disable();
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_GPIO_INT_Enable(HBN_GPIO_INT_Trigger_Type gpioIntTrigType){
    return RomDriver_HBN_GPIO_INT_Enable(gpioIntTrigType);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Get_Flash_Power_Delay(uint8_t *flashPwrDly){
    return RomDriver_HBN_Get_Flash_Power_Delay(flashPwrDly);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Get_Gpio_Keep(uint8_t *gpioKeep){
    return RomDriver_HBN_Get_Gpio_Keep(gpioKeep);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Get_Ldo18io_Power_Delay_Config(uint16_t *pwrOffDly, uint16_t *pwrOnDly){
    return RomDriver_HBN_Get_Ldo18io_Power_Delay_Config(pwrOffDly,pwrOnDly);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Get_PDS_Gpio_Keep(uint8_t *gpioKeep){
    return RomDriver_HBN_Get_PDS_Gpio_Keep(gpioKeep);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Get_RTC_Timer_Val(uint32_t *valLow, uint32_t *valHigh){
    return RomDriver_HBN_Get_RTC_Timer_Val(valLow,valHigh);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Get_Xtal_Type(uint8_t *xtalType){
    return RomDriver_HBN_Get_Xtal_Type(xtalType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Hw_Pu_Pd_Cfg(uint8_t enable){
    return RomDriver_HBN_Hw_Pu_Pd_Cfg(enable);
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Keep_On_RC32K(void){
    return RomDriver_HBN_Keep_On_RC32K();
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_PIR_Disable(void){
    return RomDriver_HBN_PIR_Disable();
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_PIR_Enable(void){
    return RomDriver_HBN_PIR_Enable();
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_PIR_HPF_Sel(HBN_PIR_HPF_Type hpf){
    return RomDriver_HBN_PIR_HPF_Sel(hpf);
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_PIR_INT_Config(HBN_PIR_INT_CFG_Type *pirIntCfg){
    return RomDriver_HBN_PIR_INT_Config(pirIntCfg);
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_PIR_LPF_Sel(HBN_PIR_LPF_Type lpf){
    return RomDriver_HBN_PIR_LPF_Sel(lpf);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Pin_WakeUp_Mask(uint8_t maskVal){
    return RomDriver_HBN_Pin_WakeUp_Mask(maskVal);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Power_Off_RC32K(void){
    return RomDriver_HBN_Power_Off_RC32K();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Power_Off_Xtal_32K(void){
    return RomDriver_HBN_Power_Off_Xtal_32K();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Power_On_Xtal_32K(void){
    return RomDriver_HBN_Power_On_Xtal_32K();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Reset(void){
    return RomDriver_HBN_Reset();
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_BOD_Cfg(HBN_BOD_CFG_Type *cfg){
    return RomDriver_HBN_Set_BOD_Cfg(cfg);
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_BOD_Config(uint8_t enable, HBN_BOD_THRES_Type threshold, HBN_BOD_MODE_Type mode){
    return RomDriver_HBN_Set_BOD_Config(enable,threshold,mode);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Core_Reboot_Config(uint8_t core, uint8_t hcfg){
    return RomDriver_HBN_Set_Core_Reboot_Config(core,hcfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Flash_Power_Delay(uint8_t flashPwrDly){
    return RomDriver_HBN_Set_Flash_Power_Delay(flashPwrDly);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_GPADC_CLK_Sel(HBN_GPADC_CLK_Type clkSel){
    return RomDriver_HBN_Set_GPADC_CLK_Sel(clkSel);
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Gpio_Keep(uint8_t gpioKeep){
    return RomDriver_HBN_Set_Gpio_Keep(gpioKeep);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_HRAM_Ret(void){
    return RomDriver_HBN_Set_HRAM_Ret();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_HRAM_slp(void){
    return RomDriver_HBN_Set_HRAM_slp();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Ldo11_All_Vout(HBN_LDO_LEVEL_Type ldoLevel){
    return RomDriver_HBN_Set_Ldo11_All_Vout(ldoLevel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Ldo11_Aon_Vout(HBN_LDO_LEVEL_Type ldoLevel){
    return RomDriver_HBN_Set_Ldo11_Aon_Vout(ldoLevel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Ldo11_Rt_Vout(HBN_LDO_LEVEL_Type ldoLevel){
    return RomDriver_HBN_Set_Ldo11_Rt_Vout(ldoLevel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Ldo11_Soc_Vout(HBN_LDO_LEVEL_Type ldoLevel){
    return RomDriver_HBN_Set_Ldo11_Soc_Vout(ldoLevel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Ldo18io_Power_Delay_Config(uint16_t pwrOffDly, uint16_t pwrOnDly){
    return RomDriver_HBN_Set_Ldo18io_Power_Delay_Config(pwrOffDly,pwrOnDly);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_MCU_Root_CLK_Sel(HBN_MCU_ROOT_CLK_Type rootClk){
    return RomDriver_HBN_Set_MCU_Root_CLK_Sel(rootClk);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_Type xclk){
    return RomDriver_HBN_Set_MCU_XCLK_Sel(xclk);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_PDS_Gpio_Keep(uint8_t gpioKeep){
    return RomDriver_HBN_Set_PDS_Gpio_Keep(gpioKeep);
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_PIR_Interval(uint16_t interval){
    return RomDriver_HBN_Set_PIR_Interval(interval);
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_PIR_Threshold(uint16_t threshold){
    return RomDriver_HBN_Set_PIR_Threshold(threshold);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_RTC_Timer(HBN_RTC_INT_Delay_Type delay, uint32_t compValLow, uint32_t compValHigh, uint8_t compMode){
    return RomDriver_HBN_Set_RTC_Timer(delay,compValLow,compValHigh,compMode);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Status_Flag(uint32_t flag){
    return RomDriver_HBN_Set_Status_Flag(flag);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_UART_CLK_Sel(HBN_UART_CLK_Type clkSel){
    return RomDriver_HBN_Set_UART_CLK_Sel(clkSel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_User_Boot_Config(uint8_t ubCfg){
    return RomDriver_HBN_Set_User_Boot_Config(ubCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Wakeup_Addr(uint32_t addr){
    return RomDriver_HBN_Set_Wakeup_Addr(addr);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Xtal_Type(uint8_t xtalType){
    return RomDriver_HBN_Set_Xtal_Type(xtalType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Trim_Ldo33VoutTrim(void){
    return RomDriver_HBN_Trim_Ldo33VoutTrim();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Trim_RC32K(void){
    return RomDriver_HBN_Trim_RC32K();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type HBN_Get_BOD_OUT_State(void){
    return RomDriver_HBN_Get_BOD_OUT_State();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type HBN_Get_INT_State(HBN_INT_Type irqType){
    return RomDriver_HBN_Get_INT_State(irqType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
HBN_MCU_ROOT_CLK_Type HBN_Get_MCU_Root_CLK_Sel(void){
    return RomDriver_HBN_Get_MCU_Root_CLK_Sel();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
HBN_MCU_XCLK_Type HBN_Get_MCU_XCLK_Sel(void){
    return RomDriver_HBN_Get_MCU_XCLK_Sel();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint16_t HBN_Get_PIR_Interval(void){
    return RomDriver_HBN_Get_PIR_Interval();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint16_t HBN_Get_PIR_Threshold(void){
    return RomDriver_HBN_Get_PIR_Threshold();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t HBN_Get_Status_Flag(void){
    return RomDriver_HBN_Get_Status_Flag();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t HBN_Get_Wakeup_Addr(void){
    return RomDriver_HBN_Get_Wakeup_Addr();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t HBN_Get_Core_Unhalt_Config(void){
    return RomDriver_HBN_Get_Core_Unhalt_Config();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t HBN_Get_Pin_Wakeup_Mode(void){
    return RomDriver_HBN_Get_Pin_Wakeup_Mode();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t HBN_Get_User_Boot_Config(void){
    return RomDriver_HBN_Get_User_Boot_Config();
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
void HBN_Enable(uint32_t aGPIOIeCfg, HBN_LDO_LEVEL_Type ldoLevel, HBN_LEVEL_Type hbnLevel, uint8_t dcdcPuSeq){
    return RomDriver_HBN_Enable(aGPIOIeCfg,ldoLevel,hbnLevel,dcdcPuSeq);
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
void HBN_Mode_Enter(HBN_APP_CFG_Type *cfg){
    return RomDriver_HBN_Mode_Enter(cfg);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
void HBN_Power_Down_Flash(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_HBN_Power_Down_Flash(flashCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type L1C_DCache_Clean_All(void){
    return RomDriver_L1C_DCache_Clean_All();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type L1C_DCache_Clean_By_Addr(uintptr_t addr, uint32_t len){
    return RomDriver_L1C_DCache_Clean_By_Addr(addr,len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type L1C_DCache_Clean_Invalid_All(void){
    return RomDriver_L1C_DCache_Clean_Invalid_All();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type L1C_DCache_Clean_Invalid_By_Addr(uintptr_t addr, uint32_t len){
    return RomDriver_L1C_DCache_Clean_Invalid_By_Addr(addr,len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type L1C_DCache_Disable(void){
    return RomDriver_L1C_DCache_Disable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type L1C_DCache_Enable(uint8_t wayDsiable){
    return RomDriver_L1C_DCache_Enable(wayDsiable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type L1C_DCache_Invalid_All(void){
    return RomDriver_L1C_DCache_Invalid_All();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type L1C_DCache_Invalid_By_Addr(uintptr_t addr, uint32_t len){
    return RomDriver_L1C_DCache_Invalid_By_Addr(addr,len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type L1C_ICache_Disable(void){
    return RomDriver_L1C_ICache_Disable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type L1C_ICache_Enable(uint8_t wayDsiable){
    return RomDriver_L1C_ICache_Enable(wayDsiable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type L1C_ICache_Invalid_All(void){
    return RomDriver_L1C_ICache_Invalid_All();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type L1C_ICache_Invalid_By_Addr(uintptr_t addr, uint32_t len){
    return RomDriver_L1C_ICache_Invalid_By_Addr(addr,len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type L1C_Set_Cache_Setting_By_ID(uint8_t core, L1C_CACHE_Cfg_Type *cacheSetting){
    return RomDriver_L1C_Set_Cache_Setting_By_ID(core,cacheSetting);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type L1C_Set_Wrap(uint8_t en){
    return RomDriver_L1C_Set_Wrap(en);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void L1C_DCache_Write_Set(BL_Fun_Type wtEn, BL_Fun_Type wbEn, BL_Fun_Type waEn){
    return RomDriver_L1C_DCache_Write_Set(wtEn,wbEn,waEn);
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Default_Level_Config(PDS_DEFAULT_LV_CFG_Type *defaultLvCfg, uint32_t pdsSleepCnt){
    return RomDriver_PDS_Default_Level_Config(defaultLvCfg,pdsSleepCnt);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Disable_GPIO_Keep(void){
    return RomDriver_PDS_Disable_GPIO_Keep();
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Enable(PDS_CTL_Type *cfg, PDS_CTL4_Type *cfg4, uint32_t pdsSleepCnt){
    return RomDriver_PDS_Enable(cfg,cfg4,pdsSleepCnt);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Force_Config(PDS_CTL2_Type *cfg2, PDS_CTL3_Type *cfg3, PDS_CTL5_Type *cfg5){
    return RomDriver_PDS_Force_Config(cfg2,cfg3,cfg5);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_IntClear(void){
    return RomDriver_PDS_IntClear();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_IntMask(PDS_INT_Type intType, BL_Mask_Type intMask){
    return RomDriver_PDS_IntMask(intType,intMask);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_RAM_Config(PDS_CTRL_RAM1_Type *ram1Cfg, PDS_CTRL_RAM2_Type *ram2Cfg, PDS_CTRL_RAM3_Type *ram3Cfg, PDS_CTRL_RAM4_Type *ram4Cfg){
    return RomDriver_PDS_RAM_Config(ram1Cfg,ram2Cfg,ram3Cfg,ram4Cfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_Flash_Pad_Pull_None(SF_Ctrl_Pin_Select pinCfg){
    return RomDriver_PDS_Set_Flash_Pad_Pull_None(pinCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_Flash_Pad_Pull_None_Fast(SF_Ctrl_Pin_Select pinCfg){
    return RomDriver_PDS_Set_Flash_Pad_Pull_None_Fast(pinCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_GPIO_Pad_IntClr(PDS_GPIO_INT_SET_Type set){
    return RomDriver_PDS_Set_GPIO_Pad_IntClr(set);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_GPIO_Pad_IntMask(GLB_GPIO_Type pad, BL_Mask_Type intMask){
    return RomDriver_PDS_Set_GPIO_Pad_IntMask(pad,intMask);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_GPIO_Pad_IntMode(PDS_GPIO_INT_SET_Type set, PDS_GPIO_INT_TRIG_Type trig){
    return RomDriver_PDS_Set_GPIO_Pad_IntMode(set,trig);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie(PDS_GPIO_GROUP_SET_Type grp, uint8_t pu, uint8_t pd, uint8_t ie){
    return RomDriver_PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie(grp,pu,pd,ie);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_MCU0_Clock_Disable(void){
    return RomDriver_PDS_Set_MCU0_Clock_Disable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_MCU0_Clock_Enable(void){
    return RomDriver_PDS_Set_MCU0_Clock_Enable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_MCU0_Reset_Address(uint32_t addr){
    return RomDriver_PDS_Set_MCU0_Reset_Address(addr);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_USB_Resume(void){
    return RomDriver_PDS_Set_USB_Resume();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_USB_Suspend(void){
    return RomDriver_PDS_Set_USB_Suspend();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Trim_RC32M(void){
    return RomDriver_PDS_Trim_RC32M();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Turn_Off_USB(void){
    return RomDriver_PDS_Turn_Off_USB();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Turn_On_USB(uint8_t waitReady){
    return RomDriver_PDS_Turn_On_USB(waitReady);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type PDS_Get_GPIO_Pad_IntStatus(GLB_GPIO_Type pad){
    return RomDriver_PDS_Get_GPIO_Pad_IntStatus(pad);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type PDS_Get_IntStatus(PDS_INT_Type intType){
    return RomDriver_PDS_Get_IntStatus(intType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SF_Cfg_Deinit_Ext_Flash_Gpio(uint8_t extFlashPin){
    return RomDriver_SF_Cfg_Deinit_Ext_Flash_Gpio(extFlashPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SF_Cfg_Flash_Init(SF_Ctrl_Pin_Select sel, const SF_Ctrl_Cfg_Type *pSfCtrlCfg, const SF_Ctrl_Bank2_Cfg *pBank2Cfg){
    return RomDriver_SF_Cfg_Flash_Init(sel,pSfCtrlCfg,pBank2Cfg);
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SF_Cfg_Get_Flash_Cfg_Need_Lock(uint32_t flashID, SPI_Flash_Cfg_Type *pFlashCfg){
    return RomDriver_SF_Cfg_Get_Flash_Cfg_Need_Lock(flashID,pFlashCfg);
}
#endif

__ALWAYS_INLINE
BL_Err_Type SF_Cfg_Init_Ext_Flash_Gpio(uint8_t extFlashPin){
    return RomDriver_SF_Cfg_Init_Ext_Flash_Gpio(extFlashPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SF_Cfg_Init_Flash2_Gpio(uint8_t swap){
    return RomDriver_SF_Cfg_Init_Flash2_Gpio(swap);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SF_Cfg_Init_Flash_Gpio(SF_Ctrl_Pin_Select flashPinCfg, uint8_t restoreDefault){
    return RomDriver_SF_Cfg_Init_Flash_Gpio(flashPinCfg,restoreDefault);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SF_Cfg_Sbus2_Flash_Init(SF_Ctrl_Pin_Select sel, const SF_Ctrl_Bank2_Cfg *pBank2Cfg){
    return RomDriver_SF_Cfg_Sbus2_Flash_Init(sel,pBank2Cfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t SF_Cfg_Flash_Identify(uint8_t callFromFlash, uint8_t flashPinCfg, uint8_t restoreDefault, SPI_Flash_Cfg_Type *pFlashCfg, uint8_t group, SF_Ctrl_Bank_Select bank){
    return RomDriver_SF_Cfg_Flash_Identify(callFromFlash,flashPinCfg,restoreDefault,pFlashCfg,group,bank);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type SF_Ctrl_GetBusyState(void){
    return RomDriver_SF_Ctrl_GetBusyState();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type SF_Ctrl_Is_AES_Enable(void){
    return RomDriver_SF_Ctrl_Is_AES_Enable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type SF_Ctrl_Is_Sbus2_Enable(void){
    return RomDriver_SF_Ctrl_Is_Sbus2_Enable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t SF_Ctrl_Get_Flash_Image_Offset(uint8_t group, SF_Ctrl_Bank_Select bank){
    return RomDriver_SF_Ctrl_Get_Flash_Image_Offset(group,bank);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t SF_Ctrl_Get_Clock_Delay(void){
    return RomDriver_SF_Ctrl_Get_Clock_Delay();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t SF_Ctrl_Get_Wrap_Queue_Value(void){
    return RomDriver_SF_Ctrl_Get_Wrap_Queue_Value();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_32bits_Addr_En(BL_Fun_Type en32BitsAddr){
    return RomDriver_SF_Ctrl_32bits_Addr_En(en32BitsAddr);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_AES_Disable(void){
    return RomDriver_SF_Ctrl_AES_Disable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_AES_Enable(void){
    return RomDriver_SF_Ctrl_AES_Enable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_AES_Enable_BE(void){
    return RomDriver_SF_Ctrl_AES_Enable_BE();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_AES_Enable_LE(void){
    return RomDriver_SF_Ctrl_AES_Enable_LE();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_AES_Set_IV(uint8_t region, uint8_t *iv, uint32_t addrOffset){
    return RomDriver_SF_Ctrl_AES_Set_IV(region,iv,addrOffset);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_AES_Set_IV_BE(uint8_t region, uint8_t *iv, uint32_t addrOffset){
    return RomDriver_SF_Ctrl_AES_Set_IV_BE(region,iv,addrOffset);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_AES_Set_Key(uint8_t region, uint8_t *key, SF_Ctrl_AES_Key_Type keyType){
    return RomDriver_SF_Ctrl_AES_Set_Key(region,key,keyType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_AES_Set_Key_BE(uint8_t region, uint8_t *key, SF_Ctrl_AES_Key_Type keyType){
    return RomDriver_SF_Ctrl_AES_Set_Key_BE(region,key,keyType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_AES_Set_Mode(SF_Ctrl_AES_Mode_Type mode){
    return RomDriver_SF_Ctrl_AES_Set_Mode(mode);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_AES_Set_Region(uint8_t region, uint8_t enable, uint8_t hwKey, uint32_t startAddr, uint32_t endAddr, uint8_t locked){
    return RomDriver_SF_Ctrl_AES_Set_Region(region,enable,hwKey,startAddr,endAddr,locked);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_AES_XTS_Set_IV(uint8_t region, uint8_t *iv, uint32_t addrOffset){
    return RomDriver_SF_Ctrl_AES_XTS_Set_IV(region,iv,addrOffset);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_AES_XTS_Set_IV_BE(uint8_t region, uint8_t *iv, uint32_t addrOffset){
    return RomDriver_SF_Ctrl_AES_XTS_Set_IV_BE(region,iv,addrOffset);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_AES_XTS_Set_Key(uint8_t region, uint8_t *key, SF_Ctrl_AES_Key_Type keyType){
    return RomDriver_SF_Ctrl_AES_XTS_Set_Key(region,key,keyType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_AES_XTS_Set_Key_BE(uint8_t region, uint8_t *key, SF_Ctrl_AES_Key_Type keyType){
    return RomDriver_SF_Ctrl_AES_XTS_Set_Key_BE(region,key,keyType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_Bank2_Enable(const SF_Ctrl_Bank2_Cfg *bank2Cfg){
    return RomDriver_SF_Ctrl_Bank2_Enable(bank2Cfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_Cmds_Set(SF_Ctrl_Cmds_Cfg *cmdsCfg, SF_Ctrl_Bank_Select bank){
    return RomDriver_SF_Ctrl_Cmds_Set(cmdsCfg,bank);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_Disable(void){
    return RomDriver_SF_Ctrl_Disable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_Disable_Wrap_Access(uint8_t disable){
    return RomDriver_SF_Ctrl_Disable_Wrap_Access(disable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_Enable(const SF_Ctrl_Cfg_Type *cfg){
    return RomDriver_SF_Ctrl_Enable(cfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_Flash2_Read_Icache_Set(SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid){
    return RomDriver_SF_Ctrl_Flash2_Read_Icache_Set(cfg,cmdValid);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_Flash_Read_Icache_Set(SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid){
    return RomDriver_SF_Ctrl_Flash_Read_Icache_Set(cfg,cmdValid);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_Lock_Flash_Image_Offset(uint8_t lock){
    return RomDriver_SF_Ctrl_Lock_Flash_Image_Offset(lock);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_Remap_Set(SF_Ctrl_Remap_Type remap, uint8_t lock){
    return RomDriver_SF_Ctrl_Remap_Set(remap,lock);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_Sbus2_Hold_Sram(void){
    return RomDriver_SF_Ctrl_Sbus2_Hold_Sram();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_Sbus2_Release_Sram(void){
    return RomDriver_SF_Ctrl_Sbus2_Release_Sram();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_Sbus2_Replace(SF_Ctrl_Pad_Type pad){
    return RomDriver_SF_Ctrl_Sbus2_Replace(pad);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_Sbus2_Revoke_replace(void){
    return RomDriver_SF_Ctrl_Sbus2_Revoke_replace();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_Sbus2_Set_Delay(uint8_t clkDelay, uint8_t rxClkInvert){
    return RomDriver_SF_Ctrl_Sbus2_Set_Delay(clkDelay,rxClkInvert);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_Sbus_Select_Bank(SF_Ctrl_Bank_Select bank){
    return RomDriver_SF_Ctrl_Sbus_Select_Bank(bank);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_Select_Pad(SF_Ctrl_Pin_Select sel){
    return RomDriver_SF_Ctrl_Select_Pad(sel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_SendCmd(SF_Ctrl_Cmd_Cfg_Type *cfg){
    return RomDriver_SF_Ctrl_SendCmd(cfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_Set_Clock_Delay(uint8_t delay){
    return RomDriver_SF_Ctrl_Set_Clock_Delay(delay);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_Set_Flash_Image_Offset(uint32_t addrOffset, uint8_t group, SF_Ctrl_Bank_Select bank){
    return RomDriver_SF_Ctrl_Set_Flash_Image_Offset(addrOffset,group,bank);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_Set_IO_Delay(SF_Ctrl_Pad_Type pad, uint8_t doDelay, uint8_t diDelay, uint8_t oeDelay){
    return RomDriver_SF_Ctrl_Set_IO_Delay(pad,doDelay,diDelay,oeDelay);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_Set_Owner(SF_Ctrl_Owner_Type owner){
    return RomDriver_SF_Ctrl_Set_Owner(owner);
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_Blk32_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t blkNum){
    return RomDriver_SFlash_Blk32_Erase(flashCfg,blkNum);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_Blk64_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t blkNum){
    return RomDriver_SFlash_Blk64_Erase(flashCfg,blkNum);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_Chip_Erase(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_Chip_Erase(flashCfg);
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t startaddr, uint32_t endaddr){
    return RomDriver_SFlash_Erase(flashCfg,startaddr,endaddr);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_Erase_Security_Register(SPI_Flash_Cfg_Type *pFlashCfg, SFlash_Sec_Reg_Cfg *pSecRegCfg){
    return RomDriver_SFlash_Erase_Security_Register(pFlashCfg,pSecRegCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_IDbus_Read_Enable(SPI_Flash_Cfg_Type *flashCfg, SF_Ctrl_IO_Type ioMode, uint8_t contRead, SF_Ctrl_Bank_Select bank){
    return RomDriver_SFlash_IDbus_Read_Enable(flashCfg,ioMode,contRead,bank);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_Program(SPI_Flash_Cfg_Type *flashCfg, SF_Ctrl_IO_Type ioMode, uint32_t addr, uint8_t *data, uint32_t len){
    return RomDriver_SFlash_Program(flashCfg,ioMode,addr,data,len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_Program_Security_Register(SPI_Flash_Cfg_Type *pFlashCfg, SFlash_Sec_Reg_Cfg *pSecRegCfg){
    return RomDriver_SFlash_Program_Security_Register(pFlashCfg,pSecRegCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_Qspi_Disable(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_Qspi_Disable(flashCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_Qspi_Enable(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_Qspi_Enable(flashCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_RCV_Enable(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t rCmd, uint8_t wCmd, uint8_t bitPos){
    return RomDriver_SFlash_RCV_Enable(pFlashCfg,rCmd,wCmd,bitPos);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_Read(SPI_Flash_Cfg_Type *flashCfg, SF_Ctrl_IO_Type ioMode, uint8_t contRead, uint32_t addr, uint8_t *data, uint32_t len){
    return RomDriver_SFlash_Read(flashCfg,ioMode,contRead,addr,data,len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_Read_Reg(SPI_Flash_Cfg_Type *flashCfg, uint8_t regIndex, uint8_t *regValue, uint8_t regLen){
    return RomDriver_SFlash_Read_Reg(flashCfg,regIndex,regValue,regLen);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_Read_Reg_With_Cmd(SPI_Flash_Cfg_Type *flashCfg, uint8_t readRegCmd, uint8_t *regValue, uint8_t regLen){
    return RomDriver_SFlash_Read_Reg_With_Cmd(flashCfg,readRegCmd,regValue,regLen);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_Read_Security_Register(SFlash_Sec_Reg_Cfg *pSecRegCfg){
    return RomDriver_SFlash_Read_Security_Register(pSecRegCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_Restore_From_Powerdown(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t flashContRead, SF_Ctrl_Bank_Select bank){
    return RomDriver_SFlash_Restore_From_Powerdown(pFlashCfg,flashContRead,bank);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_Sector_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t secNum){
    return RomDriver_SFlash_Sector_Erase(flashCfg,secNum);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_Set32BitsAddrMode(SPI_Flash_Cfg_Type *flashCfg, BL_Fun_Type en32BitsAddr){
    return RomDriver_SFlash_Set32BitsAddrMode(flashCfg,en32BitsAddr);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_SetSPIMode(SF_Ctrl_Mode_Type mode){
    return RomDriver_SFlash_SetSPIMode(mode);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_Set_IDbus_Cfg(SPI_Flash_Cfg_Type *flashCfg, SF_Ctrl_IO_Type ioMode, uint8_t contRead, uint32_t addr, uint32_t len, SF_Ctrl_Bank_Select bank){
    return RomDriver_SFlash_Set_IDbus_Cfg(flashCfg,ioMode,contRead,addr,len,bank);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_Software_Reset(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_Software_Reset(flashCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_Write_Enable(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_Write_Enable(flashCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_Write_Reg(SPI_Flash_Cfg_Type *flashCfg, uint8_t regIndex, uint8_t *regValue, uint8_t regLen){
    return RomDriver_SFlash_Write_Reg(flashCfg,regIndex,regValue,regLen);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type SFlash_Write_Reg_With_Cmd(SPI_Flash_Cfg_Type *flashCfg, uint8_t writeRegCmd, uint8_t *regValue, uint8_t regLen){
    return RomDriver_SFlash_Write_Reg_With_Cmd(flashCfg,writeRegCmd,regValue,regLen);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type SFlash_Busy(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_Busy(flashCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SFlash_DisableBurstWrap(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_DisableBurstWrap(flashCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SFlash_GetDeviceId(uint8_t *data, BL_Fun_Type is32BitsAddr){
    return RomDriver_SFlash_GetDeviceId(data,is32BitsAddr);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SFlash_GetJedecId(SPI_Flash_Cfg_Type *flashCfg, uint8_t *data){
    return RomDriver_SFlash_GetJedecId(flashCfg,data);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SFlash_GetUniqueId(uint8_t *data, uint8_t idLen){
    return RomDriver_SFlash_GetUniqueId(data,idLen);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SFlash_IDbus_Read_Disable(void){
    return RomDriver_SFlash_IDbus_Read_Disable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SFlash_Init(const SF_Ctrl_Cfg_Type *pSfCtrlCfg, const SF_Ctrl_Bank2_Cfg *pBank2Cfg){
    return RomDriver_SFlash_Init(pSfCtrlCfg,pBank2Cfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SFlash_Powerdown(void){
    return RomDriver_SFlash_Powerdown();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SFlash_Release_Powerdown(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_Release_Powerdown(flashCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SFlash_Reset_Continue_Read(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_Reset_Continue_Read(flashCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SFlash_SetBurstWrap(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_SetBurstWrap(flashCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void SFlash_Volatile_Reg_Write_Enable(SPI_Flash_Cfg_Type *flashCfg){
    return RomDriver_SFlash_Volatile_Reg_Write_Enable(flashCfg);
}

/*
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_AutoBaudDetection(UART_ID_Type uartId, BL_Fun_Type autoBaud){
    return RomDriver_UART_AutoBaudDetection(uartId,autoBaud);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_ClrRtsValue(UART_ID_Type uartId){
    return RomDriver_UART_ClrRtsValue(uartId);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_ClrTxValue(UART_ID_Type uartId){
    return RomDriver_UART_ClrTxValue(uartId);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_DeInit(UART_ID_Type uartId){
    return RomDriver_UART_DeInit(uartId);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_Disable(UART_ID_Type uartId, UART_Direction_Type direct){
    return RomDriver_UART_Disable(uartId,direct);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_Enable(UART_ID_Type uartId, UART_Direction_Type direct){
    return RomDriver_UART_Enable(uartId,direct);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_FifoConfig(UART_ID_Type uartId, UART_FifoCfg_Type *fifoCfg){
    return RomDriver_UART_FifoConfig(uartId,fifoCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_GetBitWidth0X55(UART_ID_Type uartId, uint16_t *width){
    return RomDriver_UART_GetBitWidth0X55(uartId,width);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_Init(UART_ID_Type uartId, UART_CFG_Type *uartCfg){
    return RomDriver_UART_Init(uartId,uartCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_IntClear(UART_ID_Type uartId, UART_INT_Type intType){
    return RomDriver_UART_IntClear(uartId,intType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_IntMask(UART_ID_Type uartId, UART_INT_Type intType, BL_Mask_Type intMask){
    return RomDriver_UART_IntMask(uartId,intType,intMask);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_IrConfig(UART_ID_Type uartId, UART_IrCfg_Type *irCfg){
    return RomDriver_UART_IrConfig(uartId,irCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_RxFifoClear(UART_ID_Type uartId){
    return RomDriver_UART_RxFifoClear(uartId);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_SendData(UART_ID_Type uartId, uint8_t *data, uint32_t len){
    return RomDriver_UART_SendData(uartId,data,len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_SendDataBlock(UART_ID_Type uartId, uint8_t *data, uint32_t len){
    return RomDriver_UART_SendDataBlock(uartId,data,len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_SetAllowableError0X55(UART_ID_Type uartId, uint8_t allowableError){
    return RomDriver_UART_SetAllowableError0X55(uartId,allowableError);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_ApplyAbrResult(UART_ID_Type uartId, UART_AutoBaudDetection_Type autoBaudDet){
    return RomDriver_UART_SetBaudrate(uartId,autoBaudDet);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_SetDeglitchCount(UART_ID_Type uartId, uint8_t deglitchCnt){
    return RomDriver_UART_SetDeglitchCount(uartId,deglitchCnt);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_SetRS485(UART_ID_Type uartId, BL_Fun_Type enable, UART_RS485Polarity_Type polarity){
    return RomDriver_UART_SetRS485(uartId,enable,polarity);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_SetRtsValue(UART_ID_Type uartId){
    return RomDriver_UART_SetRtsValue(uartId);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_SetRxByteCount(UART_ID_Type uartId, uint16_t count){
    return RomDriver_UART_SetRxByteCount(uartId,count);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_SetRxDataLength(UART_ID_Type uartId, uint16_t length){
    return RomDriver_UART_SetRxDataLength(uartId,length);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_SetRxTimeoutValue(UART_ID_Type uartId, uint8_t time){
    return RomDriver_UART_SetRxTimeoutValue(uartId,time);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_SetTxDataLength(UART_ID_Type uartId, uint16_t length){
    return RomDriver_UART_SetTxDataLength(uartId,length);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_SetTxValue(UART_ID_Type uartId){
    return RomDriver_UART_SetTxValue(uartId);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_TxFifoClear(UART_ID_Type uartId){
    return RomDriver_UART_TxFifoClear(uartId);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type UART_TxFreeRun(UART_ID_Type uartId, BL_Fun_Type txFreeRun){
    return RomDriver_UART_TxFreeRun(uartId,txFreeRun);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type UART_GetIntStatus(UART_ID_Type uartId, UART_INT_Type intType){
    return RomDriver_UART_GetIntStatus(uartId,intType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type UART_GetOverflowStatus(UART_ID_Type uartId, UART_Overflow_Type overflow){
    return RomDriver_UART_GetOverflowStatus(uartId,overflow);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type UART_GetRxBusBusyStatus(UART_ID_Type uartId){
    return RomDriver_UART_GetRxBusBusyStatus(uartId);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type UART_GetTxBusBusyStatus(UART_ID_Type uartId){
    return RomDriver_UART_GetTxBusBusyStatus(uartId);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint16_t UART_GetAutoBaudCount(UART_ID_Type uartId, UART_AutoBaudDetection_Type autoBaudDet){
    return RomDriver_UART_GetAutoBaudCount(uartId,autoBaudDet);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint16_t UART_GetRxByteCount(UART_ID_Type uartId){
    return RomDriver_UART_GetRxByteCount(uartId);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t UART_ReceiveData(UART_ID_Type uartId, uint8_t *data, uint32_t maxLen){
    return RomDriver_UART_ReceiveData(uartId,data,maxLen);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t UART_GetRxFifoCount(UART_ID_Type uartId){
    return RomDriver_UART_GetRxFifoCount(uartId);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t UART_GetTxFifoCount(UART_ID_Type uartId){
    return RomDriver_UART_GetTxFifoCount(uartId);
}
*/
#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type XIP_SFlash_Erase_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t startaddr, int len, uint8_t group, SF_Ctrl_Bank_Select bank){
    return RomDriver_XIP_SFlash_Erase_Need_Lock(pFlashCfg,startaddr,len,group,bank);
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type XIP_SFlash_GetDeviceId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, BL_Fun_Type is32BitsAddr, uint8_t *data, uint8_t group, SF_Ctrl_Bank_Select bank){
    return RomDriver_XIP_SFlash_GetDeviceId_Need_Lock(pFlashCfg,is32BitsAddr,data,group,bank);
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type XIP_SFlash_GetJedecId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t *data, uint8_t group, SF_Ctrl_Bank_Select bank){
    return RomDriver_XIP_SFlash_GetJedecId_Need_Lock(pFlashCfg,data,group,bank);
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type XIP_SFlash_GetUniqueId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t *data, uint8_t idLen, uint8_t group, SF_Ctrl_Bank_Select bank){
    return RomDriver_XIP_SFlash_GetUniqueId_Need_Lock(pFlashCfg,data,idLen,group,bank);
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type XIP_SFlash_Read_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t addr, uint8_t *data, uint32_t len, uint8_t group, SF_Ctrl_Bank_Select bank){
    return RomDriver_XIP_SFlash_Read_Need_Lock(pFlashCfg,addr,data,len,group,bank);
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type XIP_SFlash_Read_Via_Cache_Need_Lock(uint32_t addr, uint8_t *data, uint32_t len){
    return RomDriver_XIP_SFlash_Read_Via_Cache_Need_Lock(addr,data,len);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type XIP_SFlash_State_Restore(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t offset, uint8_t group, SF_Ctrl_Bank_Select bank){
    return RomDriver_XIP_SFlash_State_Restore(pFlashCfg,offset,group,bank);
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type XIP_SFlash_State_Save(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t *offset, uint8_t group, SF_Ctrl_Bank_Select bank){
    return RomDriver_XIP_SFlash_State_Save(pFlashCfg,offset,group,bank);
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type XIP_SFlash_Write_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t addr, uint8_t *data, uint32_t len, uint8_t group, SF_Ctrl_Bank_Select bank){
    return RomDriver_XIP_SFlash_Write_Need_Lock(pFlashCfg,addr,data,len,group,bank);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
void XIP_SFlash_Opt_Enter(uint8_t *aesEnable){
    return RomDriver_XIP_SFlash_Opt_Enter(aesEnable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void XIP_SFlash_Opt_Exit(uint8_t aesEnable){
    return RomDriver_XIP_SFlash_Opt_Exit(aesEnable);
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type MD_Clr_GPIO_IntStatus(MD_GPIO_Type gpioPin){
    return RomDriver_MD_Clr_GPIO_IntStatus(gpioPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type MD_Embedded_Flash_Pad_Enable(void){
    return RomDriver_MD_Embedded_Flash_Pad_Enable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type MD_GPIO_Clr(MD_GPIO_Type gpioPin){
    return RomDriver_MD_GPIO_Clr(gpioPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type MD_GPIO_Init(MD_GPIO_Cfg_Type *cfg){
    return RomDriver_MD_GPIO_Init(cfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type MD_GPIO_Input_Disable(MD_GPIO_Type gpioPin){
    return RomDriver_MD_GPIO_Input_Disable(gpioPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type MD_GPIO_Input_Enable(MD_GPIO_Type gpioPin){
    return RomDriver_MD_GPIO_Input_Enable(gpioPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type MD_GPIO_IntMask(MD_GPIO_Type gpioPin, BL_Mask_Type intMask){
    return RomDriver_MD_GPIO_IntMask(gpioPin,intMask);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type MD_GPIO_Int_Init(MD_GPIO_INT_Cfg_Type *intCfg){
    return RomDriver_MD_GPIO_Int_Init(intCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type MD_GPIO_Output_Disable(MD_GPIO_Type gpioPin){
    return RomDriver_MD_GPIO_Output_Disable(gpioPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type MD_GPIO_Output_Enable(MD_GPIO_Type gpioPin){
    return RomDriver_MD_GPIO_Output_Enable(gpioPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type MD_GPIO_Set(MD_GPIO_Type gpioPin){
    return RomDriver_MD_GPIO_Set(gpioPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type MD_GPIO_Set_HZ(MD_GPIO_Type gpioPin){
    return RomDriver_MD_GPIO_Set_HZ(gpioPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type MD_GPIO_Write(MD_GPIO_Type gpioPin, uint32_t val){
    return RomDriver_MD_GPIO_Write(gpioPin,val);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type MD_Get_GPIO_IntStatus(MD_GPIO_Type gpioPin){
    return RomDriver_MD_Get_GPIO_IntStatus(gpioPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t MD_GPIO_Read(MD_GPIO_Type gpioPin){
    return RomDriver_MD_GPIO_Read(gpioPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t MD_GPIO_Get_Fun(MD_GPIO_Type gpioPin){
    return RomDriver_MD_GPIO_Get_Fun(gpioPin);
}
#endif
