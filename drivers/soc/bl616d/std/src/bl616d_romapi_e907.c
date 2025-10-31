#include "bl616d_romdriver_e907.h"


__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t bflb_clk_get_system_clock(uint8_t type) {
    return RomDriver_bflb_clk_get_system_clock(type);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t bflb_clk_get_peripheral_clock(uint8_t type, uint8_t idx) {
    return RomDriver_bflb_clk_get_peripheral_clock(type, idx);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t bflb_peripheral_clock_get_by_id(uint8_t peri) {
    return RomDriver_bflb_peripheral_clock_get_by_id(peri);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
int bflb_peripheral_clock_control_by_id(uint8_t peri, bool enable) {
    return RomDriver_bflb_peripheral_clock_control_by_id(peri, enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
int bflb_peripheral_clock_status_get_by_id(uint8_t peri) {
    return RomDriver_bflb_peripheral_clock_status_get_by_id(peri);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_On_MBG(void) {
    return RomDriver_AON_Power_On_MBG();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_Off_MBG(void) {
    return RomDriver_AON_Power_Off_MBG();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_On_SFReg(void) {
    return RomDriver_AON_Power_On_SFReg();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_Off_SFReg(void) {
    return RomDriver_AON_Power_Off_SFReg();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_On_RF(void) {
    return RomDriver_AON_Power_On_RF();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_Off_RF(void) {
    return RomDriver_AON_Power_Off_RF();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_On_XTAL(void) {
    return RomDriver_AON_Power_On_XTAL();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Set_Xtal_CapCode(uint8_t capIn, uint8_t capOut) {
    return RomDriver_AON_Set_Xtal_CapCode(capIn, capOut);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t AON_Get_Xtal_CapCode(void) {
    return RomDriver_AON_Get_Xtal_CapCode();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_Off_XTAL(void) {
    return RomDriver_AON_Power_Off_XTAL();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_On_BG(void) {
    return RomDriver_AON_Power_On_BG();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_Off_BG(void) {
    return RomDriver_AON_Power_Off_BG();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_On_LDO15_RF(void) {
    return RomDriver_AON_Power_On_LDO15_RF();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_Off_LDO15_RF(void) {
    return RomDriver_AON_Power_Off_LDO15_RF();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Output_Float_LDO15_RF(void) {
    return RomDriver_AON_Output_Float_LDO15_RF();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Output_Pulldown_LDO15_RF(void) {
    return RomDriver_AON_Output_Pulldown_LDO15_RF();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Output_LDO15_RF_Ctrl_by_Pd(void) {
    return RomDriver_AON_Output_LDO15_RF_Ctrl_by_Pd();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Set_Xtal_32K_Inverter_Amplify_Strength(uint8_t value) {
    return RomDriver_AON_Set_Xtal_32K_Inverter_Amplify_Strength(value);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Set_Xtal_32K_Regulator(uint8_t level) {
    return RomDriver_AON_Set_Xtal_32K_Regulator(level);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Start_Xtal_32K(void) {
    return RomDriver_AON_Start_Xtal_32K();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Clear_Xtal_32K_Fast_Startup(void) {
    return RomDriver_AON_Clear_Xtal_32K_Fast_Startup();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type AON_Get_Xtal_32K_Ready_State(void) {
    return RomDriver_AON_Get_Xtal_32K_Ready_State();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_On_Xtal_32K(void) {
    return RomDriver_AON_Power_On_Xtal_32K();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Power_Off_Xtal_32K(void) {
    return RomDriver_AON_Power_Off_Xtal_32K();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Set_Ldo18_Aon_Vout(uint8_t level) {
    return RomDriver_AON_Set_Ldo18_Aon_Vout(level);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_SW_Trim_Ldo18_Aon_Vout(uint8_t trim) {
    return RomDriver_AON_SW_Trim_Ldo18_Aon_Vout(trim);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Set_Ldo08_Aon_Vout(uint8_t level) {
    return RomDriver_AON_Set_Ldo08_Aon_Vout(level);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_SW_Trim_Ldo08_Aon_Vout(uint8_t trim) {
    return RomDriver_AON_SW_Trim_Ldo08_Aon_Vout(trim);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Set_Ldo_Soc_Vout(uint8_t level) {
    return RomDriver_AON_Set_Ldo_Soc_Vout(level);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_SW_Trim_Ldo_Soc_Vout(uint8_t trim) {
    return RomDriver_AON_SW_Trim_Ldo_Soc_Vout(trim);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Set_Switch_For_Efuse(uint8_t enable) {
    return RomDriver_AON_Set_Switch_For_Efuse(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Set_Ldo18_AON_Power_Switch_For_FLASH(uint8_t enable) {
    return RomDriver_AON_Set_Ldo18_AON_Power_Switch_For_FLASH(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Set_Ldo18_AON_Power_Switch_For_PSRAM(uint8_t enable) {
    return RomDriver_AON_Set_Ldo18_AON_Power_Switch_For_PSRAM(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Set_Dcdc_Sys_Vout(uint8_t level) {
    return RomDriver_AON_Set_Dcdc_Sys_Vout(level);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_SW_Trim_Dcdc_Sys_Vout(uint8_t trim) {
    return RomDriver_AON_SW_Trim_Dcdc_Sys_Vout(trim);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Set_Ldo18_Aon_Vout_in_Lowpower(uint8_t level) {
    return RomDriver_AON_Set_Ldo18_Aon_Vout_in_Lowpower(level);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Set_Dcdc_Sys_Vout_in_Lowpower(uint8_t level) {
    return RomDriver_AON_Set_Dcdc_Sys_Vout_in_Lowpower(level);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Set_Ldo_Soc_Vout_in_Lowpower(uint8_t level) {
    return RomDriver_AON_Set_Ldo_Soc_Vout_in_Lowpower(level);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Ctrl_Ldo18_Aon_Mode_by_HW(uint8_t enable) {
    return RomDriver_AON_Ctrl_Ldo18_Aon_Mode_by_HW(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Ctrl_Dcdc_Sys_Mode_by_HW(uint8_t enable) {
    return RomDriver_AON_Ctrl_Dcdc_Sys_Mode_by_HW(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Ctrl_Ldo_Soc_Mode_by_HW(uint8_t enable) {
    return RomDriver_AON_Ctrl_Ldo_Soc_Mode_by_HW(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Set_Ldo18_Aon_Mode(uint8_t mode) {
    return RomDriver_AON_Set_Ldo18_Aon_Mode(mode);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Set_Dcdc_Sys_Mode(uint8_t mode) {
    return RomDriver_AON_Set_Dcdc_Sys_Mode(mode);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Set_Ldo_Soc_Mode(uint8_t mode) {
    return RomDriver_AON_Set_Ldo_Soc_Mode(mode);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Set_Dcdc_Sys_Sstart_Delay(uint8_t delay) {
    return RomDriver_AON_Set_Dcdc_Sys_Sstart_Delay(delay);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type AON_Get_BOD_OUT_State(void) {
    return RomDriver_AON_Get_BOD_OUT_State();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Set_BOD_Config(uint8_t enable, uint8_t threshold, uint8_t mode) {
    return RomDriver_AON_Set_BOD_Config(enable, threshold, mode);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void AON_Wdg_Init(uint16_t comp_val) {
    return RomDriver_AON_Wdg_Init(comp_val);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void AON_Wdg_Start(void) {
    return RomDriver_AON_Wdg_Start();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type AON_Wdg_Status_Get(void) {
    return RomDriver_AON_Wdg_Status_Get();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void AON_Wdg_Status_Clear(void) {
    return RomDriver_AON_Wdg_Status_Clear();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void AON_Wdg_Kick(void) {
    return RomDriver_AON_Wdg_Kick();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void AON_Set_RC32M_Speed_As_8M(uint8_t enable) {
    return RomDriver_AON_Set_RC32M_Speed_As_8M(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void AON_Set_RC32M_Code_Fr_Ext(uint8_t code) {
    return RomDriver_AON_Set_RC32M_Code_Fr_Ext(code);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Trim_Ldo_Soc_Vout(void) {
    return RomDriver_AON_Trim_Ldo_Soc_Vout();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Trim_Ldo08_Aon_Vout(void) {
    return RomDriver_AON_Trim_Ldo08_Aon_Vout();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Trim_Ldo18_Aon_Vout(void) {
    return RomDriver_AON_Trim_Ldo18_Aon_Vout();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Trim_Dcdc12_Sys_Vout(void) {
    return RomDriver_HBN_Trim_Dcdc12_Sys_Vout();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t Clock_System_Clock_Get(BL_System_Clock_Type type) {
    return RomDriver_Clock_System_Clock_Get(type);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t Clock_Peripheral_Clock_Get(BL_Peripheral_Type type) {
    return RomDriver_Clock_Peripheral_Clock_Get(type);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t SystemCoreClockGet(void) {
    return RomDriver_SystemCoreClockGet();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type CPU_Set_MTimer_RST(uint8_t rstEn) {
    return RomDriver_CPU_Set_MTimer_RST(rstEn);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type CPU_Reset_MTimer(void) {
    return RomDriver_CPU_Reset_MTimer();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type CPU_Set_MTimer_CLK(uint8_t enable, uint8_t clock_src, uint16_t div) {
    return RomDriver_CPU_Set_MTimer_CLK(enable, clock_src, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t CPU_Get_MTimer_Source_Clock(void) {
    return RomDriver_CPU_Get_MTimer_Source_Clock();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t CPU_Get_MTimer_Clock(void) {
    return RomDriver_CPU_Get_MTimer_Clock();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint64_t CPU_Get_MTimer_Counter(void) {
    return RomDriver_CPU_Get_MTimer_Counter();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint64_t CPU_Get_CPU_Cycle(void) {
    return RomDriver_CPU_Get_CPU_Cycle();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint64_t CPU_Get_MTimer_US(void) {
    return RomDriver_CPU_Get_MTimer_US();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint64_t CPU_Get_MTimer_MS(void) {
    return RomDriver_CPU_Get_MTimer_MS();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type CPU_MTimer_Delay_US(uint32_t cnt) {
    return RomDriver_CPU_MTimer_Delay_US(cnt);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type CPU_MTimer_Delay_MS(uint32_t cnt) {
    return RomDriver_CPU_MTimer_Delay_MS(cnt);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void ASM_Delay_Us(uint32_t core, uint32_t cnt, uint32_t loopT) {
    return RomDriver_ASM_Delay_Us(core, cnt, loopT);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void arch_delay_us(uint32_t cnt) {
    return RomDriver_arch_delay_us(cnt);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void arch_delay_ms(uint32_t cnt) {
    return RomDriver_arch_delay_ms(cnt);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t bflb_ef_ctrl_get_common_trim_list(const bflb_ef_ctrl_com_trim_cfg_t **ptrim_list) {
    return RomDriver_bflb_ef_ctrl_get_common_trim_list(ptrim_list);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void bflb_efuse_get_device_info(bflb_efuse_device_info_type *device_info) {
    return RomDriver_bflb_efuse_get_device_info(device_info);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
int bflb_efuse_read_mac_address(uint8_t mac[6], uint8_t reload) {
    return RomDriver_bflb_efuse_read_mac_address(mac, reload);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
int bflb_efuse_get_chipid(uint8_t chipid[8]) {
    return RomDriver_bflb_efuse_get_chipid(chipid);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
float bflb_efuse_get_adc_trim(void) {
    return RomDriver_bflb_efuse_get_adc_trim();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t bflb_efuse_get_adc_tsen_trim(void) {
    return RomDriver_bflb_efuse_get_adc_tsen_trim();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void bflb_efuse_read_secure_boot(uint8_t *sign, uint8_t *aes) {
    return RomDriver_bflb_efuse_read_secure_boot(sign, aes);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
int bflb_efuse_enable_aes(uint8_t aes_type, uint8_t xts_mode) {
    return RomDriver_bflb_efuse_enable_aes(aes_type, xts_mode);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
int bflb_efuse_rw_lock_aes_key(uint8_t key_index, uint8_t rd_lock, uint8_t wr_lock) {
    return RomDriver_bflb_efuse_rw_lock_aes_key(key_index, rd_lock, wr_lock);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
int bflb_efuse_rw_lock_dbg_key(uint8_t rd_lock, uint8_t wr_lock) {
    return RomDriver_bflb_efuse_rw_lock_dbg_key(rd_lock, wr_lock);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
int bflb_efuse_write_lock_pk_hash(uint32_t pkhash_len) {
    return RomDriver_bflb_efuse_write_lock_pk_hash(pkhash_len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
int bflb_efuse_write_lock_usb_pid_vid(void) {
    return RomDriver_bflb_efuse_write_lock_usb_pid_vid();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Power_On_XTAL_And_PLL_CLK(uint8_t xtalType, uint8_t pllType) {
    return RomDriver_GLB_Power_On_XTAL_And_PLL_CLK(xtalType, pllType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Power_Off_WIFIPLL(void) {
    return RomDriver_GLB_Power_Off_WIFIPLL();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_WIFIPLL_Ref_Clk_Sel(uint8_t refClk) {
    return RomDriver_GLB_WIFIPLL_Ref_Clk_Sel(refClk);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Power_On_WIFIPLL(const GLB_WIFIPLL_Cfg_Type *const cfg, uint8_t waitStable) {
    return RomDriver_GLB_Power_On_WIFIPLL(cfg, waitStable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Power_Off_CPUPLL(void) {
    return RomDriver_GLB_Power_Off_CPUPLL();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_CPUPLL_Ref_Clk_Sel(uint8_t refClk) {
    return RomDriver_GLB_CPUPLL_Ref_Clk_Sel(refClk);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Power_On_CPUPLL(const GLB_CPUPLL_Cfg_Type *const cfg, uint8_t waitStable) {
    return RomDriver_GLB_Power_On_CPUPLL(cfg, waitStable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_CPUPLL_PostOut(uint8_t enable, uint8_t div) {
    return RomDriver_GLB_Set_CPUPLL_PostOut(enable, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_USB_CLK_From_WIFIPLL(uint8_t enable) {
    return RomDriver_GLB_Set_USB_CLK_From_WIFIPLL(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_SSC_CLK_From_WIFIPLL(uint8_t enable) {
    return RomDriver_GLB_Set_SSC_CLK_From_WIFIPLL(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_MCU_System_CLK_Div(uint8_t mcuClkDiv, uint8_t mcuPBclkDiv) {
    return RomDriver_GLB_Set_MCU_System_CLK_Div(mcuClkDiv, mcuPBclkDiv);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Get_MCU_System_CLK_Div(uint8_t *mcuClkDiv, uint8_t *mcuPBclkDiv) {
    return RomDriver_GLB_Get_MCU_System_CLK_Div(mcuClkDiv, mcuPBclkDiv);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_MCU_System_CLK(uint8_t clkFreq) {
    return RomDriver_GLB_Set_MCU_System_CLK(clkFreq);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_WL_MCU_System_CLK(uint8_t clkFreq, uint8_t cpuClkDiv, uint8_t BusClkDiv) {
    return RomDriver_GLB_Set_WL_MCU_System_CLK(clkFreq, cpuClkDiv, BusClkDiv);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_WL_XCLK_Sel(uint8_t clkSel) {
    return RomDriver_GLB_Set_WL_XCLK_Sel(clkSel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t GLB_Get_Core_Type(void) {
    return RomDriver_GLB_Get_Core_Type();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_CPU_Reset_Address(uint8_t coreID, uint32_t addr) {
    return RomDriver_GLB_Set_CPU_Reset_Address(coreID, addr);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Clr_EMI_Reset_Gate(void) {
    return RomDriver_GLB_Clr_EMI_Reset_Gate();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_ADC_CLK(uint8_t enable, uint8_t clkSel, uint8_t div) {
    return RomDriver_GLB_Set_ADC_CLK(enable, clkSel, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_DMA_CLK(uint8_t enable, uint8_t clk) {
    return RomDriver_GLB_Set_DMA_CLK(enable, clk);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_Peripheral_DMA_CN(uint8_t peri, uint8_t cn) {
    return RomDriver_GLB_Set_Peripheral_DMA_CN(peri, cn);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_IR_CLK(uint8_t enable, uint8_t clkSel, uint8_t div) {
    return RomDriver_GLB_Set_IR_CLK(enable, clkSel, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_IR_RX_GPIO_Sel(uint8_t gpio) {
    return RomDriver_GLB_IR_RX_GPIO_Sel(gpio);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_UART_CLK(uint8_t enable, uint8_t clkSel, uint8_t div) {
    return RomDriver_GLB_Set_UART_CLK(enable, clkSel, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_UART_Fun_Sel(uint8_t sig, uint8_t fun) {
    return RomDriver_GLB_UART_Fun_Sel(sig, fun);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_SF_CLK(uint8_t enable, uint8_t clkSel, uint8_t div) {
    return RomDriver_GLB_Set_SF_CLK(enable, clkSel, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_I2C_CLK(uint8_t enable, uint8_t clkSel, uint8_t div) {
    return RomDriver_GLB_Set_I2C_CLK(enable, clkSel, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_I2S_CLK(uint8_t refClkEn, uint8_t refClkDiv, uint8_t inRef, uint8_t outRef) {
    return RomDriver_GLB_Set_I2S_CLK(refClkEn, refClkDiv, inRef, outRef);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_SPI_CLK(uint8_t enable, uint8_t clkSel, uint8_t div) {
    return RomDriver_GLB_Set_SPI_CLK(enable, clkSel, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_SPI_Sig_Swap_Set(uint8_t group, uint8_t swap) {
    return RomDriver_GLB_SPI_Sig_Swap_Set(group, swap);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_PEC_CLK(uint8_t enable, uint8_t clkSel, uint8_t div) {
    return RomDriver_GLB_Set_PEC_CLK(enable, clkSel, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_PWM1_IO_Sel(uint8_t ioSel) {
    return RomDriver_GLB_Set_PWM1_IO_Sel(ioSel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_PDM_IO_Sel(uint8_t ioSel) {
    return RomDriver_GLB_Set_PDM_IO_Sel(ioSel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_DBI_CLK(uint8_t enable, uint8_t clkSel, uint8_t div) {
    return RomDriver_GLB_Set_DBI_CLK(enable, clkSel, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_DIG_CLK_Sel(uint8_t clkSel) {
    return RomDriver_GLB_Set_DIG_CLK_Sel(clkSel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_DIG_512K_CLK(uint8_t enable, uint8_t compensationEn, uint8_t div) {
    return RomDriver_GLB_Set_DIG_512K_CLK(enable, compensationEn, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_DIG_32K_CLK(uint8_t enable, uint8_t compensationEn, uint16_t div) {
    return RomDriver_GLB_Set_DIG_32K_CLK(enable, compensationEn, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Platform_Wakeup_PDS_Enable(uint8_t enable) {
    return RomDriver_GLB_Platform_Wakeup_PDS_Enable(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Sel_MCU_TMR_GPIO_Clock(uint8_t gpioPin) {
    return RomDriver_GLB_Sel_MCU_TMR_GPIO_Clock(gpioPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_Chip_Clock_Out0_Sel(uint8_t clkOutType) {
    return RomDriver_GLB_Set_Chip_Clock_Out0_Sel(clkOutType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_Chip_Clock_Out1_Sel(uint8_t clkOutType) {
    return RomDriver_GLB_Set_Chip_Clock_Out1_Sel(clkOutType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_Chip_Clock_Out2_Sel(uint8_t clkOutType) {
    return RomDriver_GLB_Set_Chip_Clock_Out2_Sel(clkOutType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_Chip_Clock_Out3_Sel(uint8_t clkOutType) {
    return RomDriver_GLB_Set_Chip_Clock_Out3_Sel(clkOutType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_Audio_AUTO_CLK(uint8_t divEn) {
    return RomDriver_GLB_Set_Audio_AUTO_CLK(divEn);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_Audio_ADC_CLK(uint8_t enable, uint8_t div) {
    return RomDriver_GLB_Set_Audio_ADC_CLK(enable, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_Audio_SOLO_CLK(uint8_t enable, uint8_t div) {
    return RomDriver_GLB_Set_Audio_SOLO_CLK(enable, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Invert_ETH_RX_CLK(uint8_t enable) {
    return RomDriver_GLB_Invert_ETH_RX_CLK(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Invert_ETH_TX_CLK(uint8_t enable) {
    return RomDriver_GLB_Invert_ETH_TX_CLK(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Invert_ETH_REF_O_CLK(uint8_t enable) {
    return RomDriver_GLB_Invert_ETH_REF_O_CLK(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_ETH_REF_O_CLK_Sel(uint8_t clkSel) {
    return RomDriver_GLB_Set_ETH_REF_O_CLK_Sel(clkSel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_CAM_CLK(uint8_t enable, uint8_t clkSel, uint8_t div) {
    return RomDriver_GLB_Set_CAM_CLK(enable, clkSel, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_SDH_CLK(uint8_t enable, uint8_t clkSel, uint8_t div) {
    return RomDriver_GLB_Set_SDH_CLK(enable, clkSel, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Config_SDIO_Host_Reset_System(uint8_t enable) {
    return RomDriver_GLB_Config_SDIO_Host_Reset_System(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_UART_Sig_Swap_Set(uint8_t group, uint8_t swap) {
    return RomDriver_GLB_UART_Sig_Swap_Set(group, swap);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Swap_MCU_SPI_0_MOSI_With_MISO(BL_Fun_Type newState) {
    return RomDriver_GLB_Swap_MCU_SPI_0_MOSI_With_MISO(newState);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_MCU_SPI_0_ACT_MOD_Sel(uint8_t mod) {
    return RomDriver_GLB_Set_MCU_SPI_0_ACT_MOD_Sel(mod);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_SFlash_IO_PARM(uint8_t selEmbedded, uint8_t swapIo3Io0, uint8_t swapIo2Cs) {
    return RomDriver_GLB_Set_SFlash_IO_PARM(selEmbedded, swapIo3Io0, swapIo2Cs);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_SFlash2_IO_PARM(uint8_t swapIo3Io0) {
    return RomDriver_GLB_Set_SFlash2_IO_PARM(swapIo3Io0);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Clr_Reset_Reason(void) {
    return RomDriver_GLB_Clr_Reset_Reason();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Get_Reset_Reason(GLB_RESET_RECORD_Type *reason) {
    return RomDriver_GLB_Get_Reset_Reason(reason);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_AHB_MCU_Software_Reset(uint8_t swrst) {
    return RomDriver_GLB_AHB_MCU_Software_Reset(swrst);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Disrst_Set(uint8_t enable, uint8_t disrst) {
    return RomDriver_GLB_Disrst_Set(enable, disrst);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_PER_Clock_Gate(uint64_t ips) {
    return RomDriver_GLB_PER_Clock_Gate(ips);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_PER_Clock_UnGate(uint64_t ips) {
    return RomDriver_GLB_PER_Clock_UnGate(ips);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_PLL_CGEN_Clock_Gate(uint8_t clk) {
    return RomDriver_GLB_PLL_CGEN_Clock_Gate(clk);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_PLL_CGEN_Clock_UnGate(uint8_t clk) {
    return RomDriver_GLB_PLL_CGEN_Clock_UnGate(clk);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_PKA_CLK_Sel(uint8_t clkSel) {
    return RomDriver_GLB_Set_PKA_CLK_Sel(clkSel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_MCU_SW_System_Reset(uint8_t sysPart) {
    return RomDriver_GLB_MCU_SW_System_Reset(sysPart);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_SW_System_Reset(void) {
    return RomDriver_GLB_SW_System_Reset();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_SW_CPU_Reset(void) {
    return RomDriver_GLB_SW_CPU_Reset();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_SW_POR_Reset(void) {
    return RomDriver_GLB_SW_POR_Reset();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_Auto_Calc_Xtal_Type(uint8_t calcXtalType) {
    return RomDriver_GLB_Set_Auto_Calc_Xtal_Type(calcXtalType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Get_Auto_Calc_Xtal_Type(uint8_t *calcXtalType) {
    return RomDriver_GLB_Get_Auto_Calc_Xtal_Type(calcXtalType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_Flash_Id_Value(uint32_t idValue) {
    return RomDriver_GLB_Set_Flash_Id_Value(idValue);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t GLB_Get_Flash_Id_Value(void) {
    return RomDriver_GLB_Get_Flash_Id_Value();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Trim_Ldo18ioVoutSel(void) {
    return RomDriver_GLB_Trim_Ldo18ioVoutSel();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Trim_Ldo18ioBypass(void) {
    return RomDriver_GLB_Trim_Ldo18ioBypass();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Trim_Ldo18ioVoutTrim(void) {
    return RomDriver_GLB_Trim_Ldo18ioVoutTrim();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void GLB_Power_Down_Ldo18ioVout(void) {
    return RomDriver_GLB_Power_Down_Ldo18ioVout();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_SRAM_RET(uint32_t value) {
    return RomDriver_GLB_Set_SRAM_RET(value);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t GLB_Get_SRAM_RET(void) {
    return RomDriver_GLB_Get_SRAM_RET();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_SRAM_SLP(uint32_t value) {
    return RomDriver_GLB_Set_SRAM_SLP(value);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t GLB_Get_SRAM_SLP(void) {
    return RomDriver_GLB_Get_SRAM_SLP();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_SRAM_PARM(uint32_t value) {
    return RomDriver_GLB_Set_SRAM_PARM(value);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t GLB_Get_SRAM_PARM(void) {
    return RomDriver_GLB_Get_SRAM_PARM();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_EM_Sel(uint8_t emType) {
    return RomDriver_GLB_Set_EM_Sel(emType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_PSRAMB_CLK_Sel(uint8_t enable, uint8_t clkSel, uint8_t div) {
    return RomDriver_GLB_Set_PSRAMB_CLK_Sel(enable, clkSel, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_Slave_Grp_0_CLK(uint8_t slave, uint8_t enable, GLB_SLAVE_GRP_0_CLK_SEL_Type clkSel, uint32_t div) {
    return RomDriver_GLB_Set_Slave_Grp_0_CLK(slave, enable, clkSel, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Config_WIFIPLL(uint8_t xtalType, const GLB_WIFIPLL_Cfg_Type *pllCfgList) {
    return RomDriver_GLB_Config_WIFIPLL(xtalType, pllCfgList);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Config_CPUPLL(uint8_t xtalType, const GLB_CPUPLL_Cfg_Type *pllCfgList) {
    return RomDriver_GLB_Config_CPUPLL(xtalType, pllCfgList);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Simple_Set_MCU_System_CLK(uint8_t clkFreq, uint8_t mcuClkDiv, uint8_t mcuPBclkDiv) {
    return RomDriver_GLB_Simple_Set_MCU_System_CLK(clkFreq, mcuClkDiv, mcuPBclkDiv);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void GLB_Set_Flash_Xip(uint8_t enable) {
    return RomDriver_GLB_Set_Flash_Xip(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Halt_Mini_Sys(void) {
    return RomDriver_GLB_Halt_Mini_Sys();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Release_Mini_Sys(void) {
    return RomDriver_GLB_Release_Mini_Sys();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_MINI_PER_Software_Reset(uint8_t swrst) {
    return RomDriver_GLB_MINI_PER_Software_Reset(swrst);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_SPI1_CLK(uint8_t enable, uint8_t clkSel, uint8_t div) {
    return RomDriver_GLB_Set_SPI1_CLK(enable, clkSel, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_MINI_FCLK(uint8_t enable, uint8_t clkSel, uint8_t div) {
    return RomDriver_GLB_Set_MINI_FCLK(enable, clkSel, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_MINI_PER_Clock_Gate(uint32_t clk) {
    return RomDriver_GLB_MINI_PER_Clock_Gate(clk);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_MINI_PER_Clock_UnGate(uint32_t clk) {
    return RomDriver_GLB_MINI_PER_Clock_UnGate(clk);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Select_LPCPU_Jtag(void) {
    return RomDriver_GLB_Select_LPCPU_Jtag();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Halt_CPU(uint8_t coreID) {
    return RomDriver_GLB_Halt_CPU(coreID);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Release_CPU(uint8_t coreID) {
    return RomDriver_GLB_Release_CPU(coreID);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void GLB_RC32M_Force_On(uint8_t enable) {
    return RomDriver_GLB_RC32M_Force_On(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void GLB_RC32M_Force_Off(uint8_t enable) {
    return RomDriver_GLB_RC32M_Force_Off(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint64_t GLB_Get_NP2MINI_Interrupt_Status(void) {
    return RomDriver_GLB_Get_NP2MINI_Interrupt_Status();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void GLB_Set_NP2MINI_Interrupt_Clear(uint8_t irq) {
    return RomDriver_GLB_Set_NP2MINI_Interrupt_Clear(irq);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void GLB_Set_NP2MINI_Interrupt_Mask(uint8_t irq, uint8_t mask) {
    return RomDriver_GLB_Set_NP2MINI_Interrupt_Mask(irq, mask);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint64_t GLB_Get_NP2MINI_Interrupt_Mask(void) {
    return RomDriver_GLB_Get_NP2MINI_Interrupt_Mask();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Embedded_Flash_Pad_Enable(uint8_t swapIo2Cs) {
    return RomDriver_GLB_Embedded_Flash_Pad_Enable(swapIo2Cs);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void HBN_Power_Down_Flash(spi_flash_cfg_type *flashCfg) {
    return RomDriver_HBN_Power_Down_Flash(flashCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Level_Select(uint8_t hbn_level) {
    return RomDriver_HBN_Level_Select(hbn_level);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Reset(void) {
    return RomDriver_HBN_Reset();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_GPADC_CLK_Sel(uint8_t clkSel) {
    return RomDriver_HBN_Set_GPADC_CLK_Sel(clkSel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_PIR_Enable(void) {
    return RomDriver_HBN_PIR_Enable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_PIR_Disable(void) {
    return RomDriver_HBN_PIR_Disable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_PIR_INT_Config(HBN_PIR_INT_CFG_Type *pirIntCfg) {
    return RomDriver_HBN_PIR_INT_Config(pirIntCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_PIR_LPF_Sel(uint8_t lpf) {
    return RomDriver_HBN_PIR_LPF_Sel(lpf);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_PIR_HPF_Sel(uint8_t hpf) {
    return RomDriver_HBN_PIR_HPF_Sel(hpf);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_PIR_Threshold(uint16_t threshold) {
    return RomDriver_HBN_Set_PIR_Threshold(threshold);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint16_t HBN_Get_PIR_Threshold(void) {
    return RomDriver_HBN_Get_PIR_Threshold();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_PIR_Interval(uint16_t interval) {
    return RomDriver_HBN_Set_PIR_Interval(interval);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint16_t HBN_Get_PIR_Interval(void) {
    return RomDriver_HBN_Get_PIR_Interval();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_32K_Sel(uint8_t clkType) {
    return RomDriver_HBN_32K_Sel(clkType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_UART_CLK_Sel(uint8_t clkSel) {
    return RomDriver_HBN_Set_UART_CLK_Sel(clkSel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t HBN_Get_MCU_XCLK_Sel(void) {
    return RomDriver_HBN_Get_MCU_XCLK_Sel();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_MCU_XCLK_Sel(uint8_t xclk) {
    return RomDriver_HBN_Set_MCU_XCLK_Sel(xclk);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t HBN_Get_MCU_Root_CLK_Sel(void) {
    return RomDriver_HBN_Get_MCU_Root_CLK_Sel();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_MCU_Root_CLK_Sel(uint8_t rootClk) {
    return RomDriver_HBN_Set_MCU_Root_CLK_Sel(rootClk);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_HRAM_slp(void) {
    return RomDriver_HBN_Set_HRAM_slp();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_HRAM_Ret(void) {
    return RomDriver_HBN_Set_HRAM_Ret();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Keep_On_RC32K(void) {
    return RomDriver_HBN_Keep_On_RC32K();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Power_Off_RC32K(void) {
    return RomDriver_HBN_Power_Off_RC32K();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Trim_RC32K(void) {
    return RomDriver_HBN_Trim_RC32K();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t HBN_Get_Status_Flag(void) {
    return RomDriver_HBN_Get_Status_Flag();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Status_Flag(uint32_t flag) {
    return RomDriver_HBN_Set_Status_Flag(flag);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t HBN_Get_Wakeup_Addr(void) {
    return RomDriver_HBN_Get_Wakeup_Addr();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Wakeup_Addr(uint32_t addr) {
    return RomDriver_HBN_Set_Wakeup_Addr(addr);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t HBN_Get_Core_Unhalt_Config(void) {
    return RomDriver_HBN_Get_Core_Unhalt_Config();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Core_Reboot_Config(uint8_t core, uint8_t hcfg) {
    return RomDriver_HBN_Set_Core_Reboot_Config(core, hcfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t HBN_Get_User_Boot_Config(void) {
    return RomDriver_HBN_Get_User_Boot_Config();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_User_Boot_Config(uint8_t ubCfg) {
    return RomDriver_HBN_Set_User_Boot_Config(ubCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Get_Ldo18io_Power_Delay_Config(uint16_t *pwrOffDly, uint16_t *pwrOnDly) {
    return RomDriver_HBN_Get_Ldo18io_Power_Delay_Config(pwrOffDly, pwrOnDly);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Ldo18io_Power_Delay_Config(uint16_t pwrOffDly, uint16_t pwrOnDly) {
    return RomDriver_HBN_Set_Ldo18io_Power_Delay_Config(pwrOffDly, pwrOnDly);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Xtal_Type(uint8_t xtalType) {
    return RomDriver_HBN_Set_Xtal_Type(xtalType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Get_Xtal_Type(uint8_t *xtalType) {
    return RomDriver_HBN_Get_Xtal_Type(xtalType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Get_Xtal_Value(uint32_t *xtalVal) {
    return RomDriver_HBN_Get_Xtal_Value(xtalVal);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Flash_Power_Delay(uint8_t flashPwrDly) {
    return RomDriver_HBN_Set_Flash_Power_Delay(flashPwrDly);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Get_Flash_Power_Delay(uint8_t *flashPwrDly) {
    return RomDriver_HBN_Get_Flash_Power_Delay(flashPwrDly);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Gpio_Keep(uint8_t gpioKeep) {
    return RomDriver_HBN_Set_Gpio_Keep(gpioKeep);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Get_Gpio_Keep(uint8_t *gpioKeep) {
    return RomDriver_HBN_Get_Gpio_Keep(gpioKeep);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Clear_Gpio_Keep(uint8_t gpioKeep) {
    return RomDriver_HBN_Clear_Gpio_Keep(gpioKeep);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_PDS_Gpio_Keep(uint8_t gpioKeep) {
    return RomDriver_HBN_Set_PDS_Gpio_Keep(gpioKeep);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Get_PDS_Gpio_Keep(uint8_t *gpioKeep) {
    return RomDriver_HBN_Get_PDS_Gpio_Keep(gpioKeep);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Clear_PDS_Gpio_Keep(uint8_t gpioKeep) {
    return RomDriver_HBN_Clear_PDS_Gpio_Keep(gpioKeep);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Disable_Gpio_Keep(int pin) {
    return RomDriver_HBN_Disable_Gpio_Keep(pin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Clear_RTC_Counter(void) {
    return RomDriver_HBN_Clear_RTC_Counter();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Enable_RTC_Counter(void) {
    return RomDriver_HBN_Enable_RTC_Counter();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_RTC_Timer(uint8_t delay, uint32_t compValLow, uint32_t compValHigh, uint8_t compMode) {
    return RomDriver_HBN_Set_RTC_Timer(delay, compValLow, compValHigh, compMode);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Get_RTC_Timer_Val(uint32_t *valLow, uint32_t *valHigh) {
    return RomDriver_HBN_Get_RTC_Timer_Val(valLow, valHigh);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Recal_RC32K(int32_t expected_counter, int32_t actual_counter) {
    return RomDriver_HBN_Recal_RC32K(expected_counter, actual_counter);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Clear_RTC_INT(void) {
    return RomDriver_HBN_Clear_RTC_INT();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_GPIO_INT_Enable(uint8_t gpioIntTrigType) {
    return RomDriver_HBN_GPIO_INT_Enable(gpioIntTrigType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_GPIO_INT_Disable(void) {
    return RomDriver_HBN_GPIO_INT_Disable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type HBN_Get_INT_State(uint8_t irqType) {
    return RomDriver_HBN_Get_INT_State(irqType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t HBN_Get_All_INT_State(void) {
    return RomDriver_HBN_Get_All_INT_State();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t HBN_Get_Pin_Wakeup_Mode(void) {
    return RomDriver_HBN_Get_Pin_Wakeup_Mode();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Clear_IRQ(uint8_t irqType) {
    return RomDriver_HBN_Clear_IRQ(irqType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Hw_Pu_Pd_Cfg(uint8_t enable) {
    return RomDriver_HBN_Hw_Pu_Pd_Cfg(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Aon_Pad_IntMode(uint8_t gpioIntTrigType) {
    return RomDriver_HBN_Set_Aon_Pad_IntMode(gpioIntTrigType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Pin_WakeUp_Mask(uint8_t maskVal) {
    return RomDriver_HBN_Pin_WakeUp_Mask(maskVal);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Aon_Pad_IntMask(uint8_t pad, BL_Mask_Type intMask) {
    return RomDriver_HBN_Set_Aon_Pad_IntMask(pad, intMask);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Aon_Pad_Ctrl(uint32_t aonPadCtl1, uint32_t aonPadCtl2) {
    return RomDriver_HBN_Aon_Pad_Ctrl(aonPadCtl1, aonPadCtl2);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Aon_Pad_Cfg(uint8_t aonGpio, HBN_AON_PAD_CFG_Type *aonPadCfg) {
    return RomDriver_HBN_Aon_Pad_Cfg(aonGpio, aonPadCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Aon_Pad_Cfg_Set(uint8_t aonPadHwCtrlEn, uint8_t aonGpio) {
    return RomDriver_HBN_Aon_Pad_Cfg_Set(aonPadHwCtrlEn, aonGpio);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Enable_AComp_IRQ(uint8_t acompId, uint8_t edge) {
    return RomDriver_HBN_Enable_AComp_IRQ(acompId, edge);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Disable_AComp_IRQ(uint8_t acompId, uint8_t edge) {
    return RomDriver_HBN_Disable_AComp_IRQ(acompId, edge);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Enable_BOD_IRQ(void) {
    return RomDriver_HBN_Enable_BOD_IRQ();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Disable_BOD_IRQ(void) {
    return RomDriver_HBN_Disable_BOD_IRQ();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Aon_Pad_WakeUpCfg(BL_Fun_Type puPdEn, uint8_t trigMode, uint32_t maskVal, BL_Fun_Type dlyEn, uint8_t dlySec) {
    return RomDriver_HBN_Aon_Pad_WakeUpCfg(puPdEn, trigMode, maskVal, dlyEn, dlySec);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie(uint8_t pad, uint8_t pu, uint8_t pd, uint8_t ie) {
    return RomDriver_PDS_Set_GPIO_Pad_Pn_Pu_Pd_Ie(pad, pu, pd, ie);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_GPIO_Pad_IntMask(uint8_t pad, BL_Mask_Type intMask) {
    return RomDriver_PDS_Set_GPIO_Pad_IntMask(pad, intMask);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_GPIO_Pad_IntMode(uint8_t pin, uint8_t trig) {
    return RomDriver_PDS_Set_GPIO_Pad_IntMode(pin, trig);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_GPIO_Pad_IntClr(uint8_t pin) {
    return RomDriver_PDS_Set_GPIO_Pad_IntClr(pin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type PDS_Get_GPIO_Pad_IntStatus(uint8_t pad) {
    return RomDriver_PDS_Get_GPIO_Pad_IntStatus(pad);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_All_GPIO_Pad_IntClr(void) {
    return RomDriver_PDS_Set_All_GPIO_Pad_IntClr();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint64_t PDS_Get_All_GPIO_Pad_IntStatus(void) {
    return RomDriver_PDS_Get_All_GPIO_Pad_IntStatus();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_Flash_Pad_Pull_None(uint8_t pinCfg) {
    return RomDriver_PDS_Set_Flash_Pad_Pull_None(pinCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_Flash_Pad_Pull_None_Fast(uint8_t pinCfg) {
    return RomDriver_PDS_Set_Flash_Pad_Pull_None_Fast(pinCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Disable_GPIO_Keep(void) {
    return RomDriver_PDS_Disable_GPIO_Keep();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_MCU0_Clock_Enable(void) {
    return RomDriver_PDS_Set_MCU0_Clock_Enable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_MCU0_Clock_Disable(void) {
    return RomDriver_PDS_Set_MCU0_Clock_Disable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_MCU1_Clock_Enable(void) {
    return RomDriver_PDS_Set_MCU1_Clock_Enable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_MCU1_Clock_Disable(void) {
    return RomDriver_PDS_Set_MCU1_Clock_Disable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_MCU0_Reset_Address(uint32_t addr) {
    return RomDriver_PDS_Set_MCU0_Reset_Address(addr);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Pu_PLL_Enable(void) {
    return RomDriver_PDS_Pu_PLL_Enable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Pu_PLL_Disable(void) {
    return RomDriver_PDS_Pu_PLL_Disable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Enable(PDS_CTL_Type *cfg, PDS_CTL4_Type *cfg4, uint32_t pdsSleepCnt) {
    return RomDriver_PDS_Enable(cfg, cfg4, pdsSleepCnt);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Force_Config(PDS_CTL2_Type *cfg2, PDS_CTL3_Type *cfg3, PDS_CTL5_Type *cfg5) {
    return RomDriver_PDS_Force_Config(cfg2, cfg3, cfg5);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_RAM_Config(PDS_CTRL_RAM1_Type *ram1Cfg, PDS_CTRL_RAM2_Type *ram2Cfg, PDS_CTRL_RAM3_Type *ram3Cfg, PDS_CTRL_RAM4_Type *ram4Cfg) {
    return RomDriver_PDS_RAM_Config(ram1Cfg, ram2Cfg, ram3Cfg, ram4Cfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_All_WRAM_Retention(void) {
    return RomDriver_PDS_Set_All_WRAM_Retention();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_All_OCRAM_Retention(void) {
    return RomDriver_PDS_Set_All_OCRAM_Retention();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_All_WRAM_Sleep(void) {
    return RomDriver_PDS_Set_All_WRAM_Sleep();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_All_OCRAM_Sleep(void) {
    return RomDriver_PDS_Set_All_OCRAM_Sleep();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Disable_Ctrl_Ram(void) {
    return RomDriver_PDS_Disable_Ctrl_Ram();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Default_Level_Config(PDS_DEFAULT_LV_CFG_Type *defaultLvCfg, uint32_t pdsSleepCnt) {
    return RomDriver_PDS_Default_Level_Config(defaultLvCfg, pdsSleepCnt);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_IntMask(uint8_t intType, BL_Mask_Type intMask) {
    return RomDriver_PDS_IntMask(intType, intMask);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type PDS_Get_IntStatus(uint8_t intType) {
    return RomDriver_PDS_Get_IntStatus(intType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_IntClear(void) {
    return RomDriver_PDS_IntClear();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_Wakeup_Src_IntMask(uint32_t WakeupType, BL_Mask_Type intMask) {
    return RomDriver_PDS_Set_Wakeup_Src_IntMask(WakeupType, intMask);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type PDS_Get_Wakeup_Src(uint32_t WakeupType) {
    return RomDriver_PDS_Get_Wakeup_Src(WakeupType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t PDS_Get_All_Wakeup_Src(void) {
    return RomDriver_PDS_Get_All_Wakeup_Src();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void PDS_Mask_All_Wakeup_Src(void) {
    return RomDriver_PDS_Mask_All_Wakeup_Src();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void PDS_Triger_SW_Wakeup_PDS(void) {
    return RomDriver_PDS_Triger_SW_Wakeup_PDS();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void PDS_Clear_SW_Wakeup_PDS(void) {
    return RomDriver_PDS_Clear_SW_Wakeup_PDS();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Trim_RC32M(void) {
    return RomDriver_PDS_Trim_RC32M();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Trim_USB20(void) {
    return RomDriver_PDS_Trim_USB20();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Power_Off_WB(void) {
    return RomDriver_PDS_Power_Off_WB();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Power_On_WB(void) {
    return RomDriver_PDS_Power_On_WB();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void PSram_Ctrl_Init(PSRAM_ID_Type PSRAM_ID, PSRAM_Ctrl_Cfg_Type *psramCtrlCfg) {
    return RomDriver_PSram_Ctrl_Init(PSRAM_ID, psramCtrlCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PSram_Ctrl_Winbond_Read_Reg(PSRAM_ID_Type PSRAM_ID, PSRAM_Ctrl_Winbond_Cfg_Reg_Type reg_addr, uint16_t *regVal) {
    return RomDriver_PSram_Ctrl_Winbond_Read_Reg(PSRAM_ID, reg_addr, regVal);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PSram_Ctrl_Winbond_Write_Reg(PSRAM_ID_Type PSRAM_ID, PSRAM_Ctrl_Winbond_Cfg_Reg_Type reg_addr, PSRAM_Winbond_Cfg_Type *reg_cfg) {
    return RomDriver_PSram_Ctrl_Winbond_Write_Reg(PSRAM_ID, reg_addr, reg_cfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PSram_Ctrl_ApMem_Read_Reg(PSRAM_ID_Type PSRAM_ID, PSRAM_Ctrl_ApMem_Cfg_Reg_Type reg_addr, uint16_t *regVal) {
    return RomDriver_PSram_Ctrl_ApMem_Read_Reg(PSRAM_ID, reg_addr, regVal);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PSram_Ctrl_ApMem_Write_Reg(PSRAM_ID_Type PSRAM_ID, PSRAM_Ctrl_ApMem_Cfg_Reg_Type reg_addr, PSRAM_APMemory_Cfg_Type *reg_cfg) {
    return RomDriver_PSram_Ctrl_ApMem_Write_Reg(PSRAM_ID, reg_addr, reg_cfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void PSram_Ctrl_ApMem_Reset(PSRAM_ID_Type PSRAM_ID) {
    return RomDriver_PSram_Ctrl_ApMem_Reset(PSRAM_ID);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void PSram_Ctrl_Winbond_Reset(PSRAM_ID_Type PSRAM_ID) {
    return RomDriver_PSram_Ctrl_Winbond_Reset(PSRAM_ID);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void PSram_Ctrl_CK_Sel(PSRAM_ID_Type PSRAM_ID, PSRAM_Clock_Type clkSel) {
    return RomDriver_PSram_Ctrl_CK_Sel(PSRAM_ID, clkSel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t PSram_Ctrl_Get_Timeout_Flag(PSRAM_ID_Type PSRAM_ID) {
    return RomDriver_PSram_Ctrl_Get_Timeout_Flag(PSRAM_ID);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void PSram_Ctrl_Clear_Timout_Flag(PSRAM_ID_Type PSRAM_ID) {
    return RomDriver_PSram_Ctrl_Clear_Timout_Flag(PSRAM_ID);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void PSram_Ctrl_Debug_Timout(PSRAM_ID_Type PSRAM_ID, uint8_t enable, uint32_t timeoutThr) {
    return RomDriver_PSram_Ctrl_Debug_Timout(PSRAM_ID, enable, timeoutThr);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Sec_Dbg_Read_Chip_ID(uint8_t id[8]) {
    return RomDriver_Sec_Dbg_Read_Chip_ID(id);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Sec_Dbg_Set_Dbg_Pwd(const uint32_t pwd[4]) {
    return RomDriver_Sec_Dbg_Set_Dbg_Pwd(pwd);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t Sec_Dbg_Read_Dbg_Mode(void) {
    return RomDriver_Sec_Dbg_Read_Dbg_Mode();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t Sec_Dbg_Read_Dbg_Enable(void) {
    return RomDriver_Sec_Dbg_Read_Dbg_Enable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Sec_Dbg_Set_Dbg_Trigger(void) {
    return RomDriver_Sec_Dbg_Set_Dbg_Trigger();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t Sec_Dbg_Wait_Ready(void) {
    return RomDriver_Sec_Dbg_Wait_Ready();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_Set_Sboot_Done(void) {
    return RomDriver_Tzc_Sec_Set_Sboot_Done();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_Set_Bus_Remap(uint8_t busRmpEn) {
    return RomDriver_Tzc_Sec_Set_Bus_Remap(busRmpEn);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_Set_Master_Group(TZC_SEC_Master_Type masterType, uint8_t group) {
    return RomDriver_Tzc_Sec_Set_Master_Group(masterType, group);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_Set_Slave_Group(TZC_SEC_Slave_Type slaveType, uint8_t group) {
    return RomDriver_Tzc_Sec_Set_Slave_Group(slaveType, group);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_Set_Codec_Group(uint8_t group) {
    return RomDriver_Tzc_Sec_Set_Codec_Group(group);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_Set_Glb_Ctrl_Group(TZC_SEC_GLB_Ctrl_Type slaveType, uint8_t group) {
    return RomDriver_Tzc_Sec_Set_Glb_Ctrl_Group(slaveType, group);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_Set_CPU_Group(uint8_t cpu, uint8_t group) {
    return RomDriver_Tzc_Sec_Set_CPU_Group(cpu, group);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_ROM_Access_Set(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group) {
    return RomDriver_Tzc_Sec_ROM_Access_Set(region, startAddr, length, group);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_ROM_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group) {
    return RomDriver_Tzc_Sec_ROM_Access_Set_Advance(region, startAddr, length, group);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_OCRAM_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group) {
    return RomDriver_Tzc_Sec_OCRAM_Access_Set_Advance(region, startAddr, length, group);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_OCRAM_Access_Set_Regionx(uint8_t group) {
    return RomDriver_Tzc_Sec_OCRAM_Access_Set_Regionx(group);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_WRAM_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group) {
    return RomDriver_Tzc_Sec_WRAM_Access_Set_Advance(region, startAddr, length, group);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_WRAM_Access_Set_Regionx(uint8_t group) {
    return RomDriver_Tzc_Sec_WRAM_Access_Set_Regionx(group);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_HBNRAM_Access_Set(uint32_t startAddr, uint32_t length) {
    return RomDriver_Tzc_Sec_HBNRAM_Access_Set(startAddr, length);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_Flash_Access_Set(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group) {
    return RomDriver_Tzc_Sec_Flash_Access_Set(region, startAddr, length, group);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_Flash_Access_Set_Regionx(uint8_t group) {
    return RomDriver_Tzc_Sec_Flash_Access_Set_Regionx(group);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_Flash_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group) {
    return RomDriver_Tzc_Sec_Flash_Access_Set_Advance(region, startAddr, length, group);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_PSRAMB_Access_Set(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group) {
    return RomDriver_Tzc_Sec_PSRAMB_Access_Set(region, startAddr, length, group);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_PSRAMB_Access_Release(void) {
    return RomDriver_Tzc_Sec_PSRAMB_Access_Release();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_PSRAMB_Access_Set_Advance(uint8_t region, uint32_t startAddr, uint32_t length, uint8_t group) {
    return RomDriver_Tzc_Sec_PSRAMB_Access_Set_Advance(region, startAddr, length, group);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_Set_Se_Ctrl_Mode(TZC_SEC_SE_Ctrl_Mode mode) {
    return RomDriver_Tzc_Sec_Set_Se_Ctrl_Mode(mode);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_Set_Sf_Ctrl_Mode(TZC_SEC_SF_Ctrl_Mode mode) {
    return RomDriver_Tzc_Sec_Set_Sf_Ctrl_Mode(mode);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_Set_Se_Group(TZC_SEC_SE_Ctrl_Type slaveType, uint8_t group) {
    return RomDriver_Tzc_Sec_Set_Se_Group(slaveType, group);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_Set_Sf_Group(TZC_SEC_SF_Ctrl_Type slaveType, uint8_t group) {
    return RomDriver_Tzc_Sec_Set_Sf_Group(slaveType, group);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void Tzc_Sec_Set_WTD_Rst_Delay(uint16_t delayValue) {
    return RomDriver_Tzc_Sec_Set_WTD_Rst_Delay(delayValue);
}
