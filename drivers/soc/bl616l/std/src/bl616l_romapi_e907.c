#include "bl616l_romdriver_e907.h"


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
BL_Err_Type AON_XTAL_Enter_LowPower(void) {
    return RomDriver_AON_XTAL_Enter_LowPower();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_XTAL_Exit_LowPower(void) {
    return RomDriver_AON_XTAL_Exit_LowPower();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_LDO18_IO_Switch_Efuse(uint8_t en) {
    return RomDriver_AON_LDO18_IO_Switch_Efuse(en);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_LDO18_IO_Switch_Psram(uint8_t en) {
    return RomDriver_AON_LDO18_IO_Switch_Psram(en);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_LDO18_IO_Switch_Flash(uint8_t en) {
    return RomDriver_AON_LDO18_IO_Switch_Flash(en);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Set_LDO18_IO_Sstart_Delay(uint8_t delay) {
    return RomDriver_AON_Set_LDO18_IO_Sstart_Delay(delay);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_SW_Set_Ldo18_IO_Vout(uint8_t ldoLevel) {
    return RomDriver_AON_SW_Set_Ldo18_IO_Vout(ldoLevel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_SW_Trim_Ldo18_IO_Vout(uint8_t trim) {
    return RomDriver_AON_SW_Trim_Ldo18_IO_Vout(trim);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type AON_Trim_Ldo18_IO_Vout(void) {
    return RomDriver_AON_Trim_Ldo18_IO_Vout();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t Clock_System_Clock_Get(BL_System_Clock_Type type) {
    return RomDriver_Clock_System_Clock_Get(type);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t Clock_Get_PSRAMB_Clk(void) {
    return RomDriver_Clock_Get_PSRAMB_Clk();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t Clock_Audio_ADC_Clock_Get(void) {
    return RomDriver_Clock_Audio_ADC_Clock_Get();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t Clock_PWM_Clk_Get(void) {
    return RomDriver_Clock_PWM_Clk_Get();
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
BL_Err_Type CPU_Set_MTimer_CLK(uint8_t enable, BL_MTimer_Source_Clock_Type mTimerSourceClockType, uint16_t div) {
    return RomDriver_CPU_Set_MTimer_CLK(enable, mTimerSourceClockType, div);
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
uint8_t bflb_efuse_is_mac_address_slot_empty(uint8_t slot, uint8_t reload) {
    return RomDriver_bflb_efuse_is_mac_address_slot_empty(slot, reload);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
int bflb_efuse_read_mac_address_opt(uint8_t slot, uint8_t mac[6], uint8_t reload) {
    return RomDriver_bflb_efuse_read_mac_address_opt(slot, mac, reload);
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
BL_Err_Type GLB_Set_MCU_Muxpll_160M_Sel(uint8_t clkSel) {
    return RomDriver_GLB_Set_MCU_Muxpll_160M_Sel(clkSel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t GLB_Get_MCU_Muxpll_160M_Sel(void) {
    return RomDriver_GLB_Get_MCU_Muxpll_160M_Sel();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_MCU_Muxpll_80M_Sel(uint8_t clkSel) {
    return RomDriver_GLB_Set_MCU_Muxpll_80M_Sel(clkSel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t GLB_Get_MCU_Muxpll_80M_Sel(void) {
    return RomDriver_GLB_Get_MCU_Muxpll_80M_Sel();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_ISP_Muxpll_80M_Sel(uint8_t clkSel) {
    return RomDriver_GLB_Set_ISP_Muxpll_80M_Sel(clkSel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t GLB_Get_ISP_Muxpll_80M_Sel(void) {
    return RomDriver_GLB_Get_ISP_Muxpll_80M_Sel();
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
BL_Err_Type GLB_Set_SPI0_CLK(uint8_t enable, uint8_t clkSel, uint8_t div) {
    return RomDriver_GLB_Set_SPI0_CLK(enable, clkSel, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_SPI1_CLK(uint8_t enable, uint8_t clkSel, uint8_t div) {
    return RomDriver_GLB_Set_SPI1_CLK(enable, clkSel, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_SPI0_Sig_Swap_Set(uint8_t group, uint8_t swap) {
    return RomDriver_GLB_SPI0_Sig_Swap_Set(group, swap);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_SPI1_Sig_Swap_Set(uint8_t group, uint8_t swap) {
    return RomDriver_GLB_SPI1_Sig_Swap_Set(group, swap);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_PWM1_IO_Sel(uint8_t ioSel) {
    return RomDriver_GLB_Set_PWM1_IO_Sel(ioSel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_PWM_CLK(uint8_t clkSel) {
    return RomDriver_GLB_Set_PWM_CLK(clkSel);
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
BL_Err_Type GLB_Set_Audio_SOLO_CLK(uint8_t enable, uint8_t div) {
    return RomDriver_GLB_Set_Audio_SOLO_CLK(enable, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_ISP_CORE_CLK(uint8_t enable, uint8_t div) {
    return RomDriver_GLB_Set_ISP_CORE_CLK(enable, div);
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
BL_Err_Type GLB_Set_PEC_CLK(uint8_t enable, uint8_t clkSel, uint8_t div) {
    return RomDriver_GLB_Set_PEC_CLK(enable, clkSel, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_AHB2QSPI_CLK(uint8_t enable, uint8_t div) {
    return RomDriver_GLB_Set_AHB2QSPI_CLK(enable, div);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Config_SDIO_Host_Cmd0_Reset(uint8_t enable) {
    return RomDriver_GLB_Config_SDIO_Host_Cmd0_Reset(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Config_SDIO_Host_Reset_System(uint8_t enable) {
    return RomDriver_GLB_Config_SDIO_Host_Reset_System(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Config_SDIO_Host_Reset_SDU(uint8_t enable) {
    return RomDriver_GLB_Config_SDIO_Host_Reset_SDU(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Config_SDIO_Host_Interrupt_CPU(uint8_t enable) {
    return RomDriver_GLB_Config_SDIO_Host_Interrupt_CPU(enable);
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
BL_Err_Type GLB_Swap_MCU_SPI_1_MOSI_With_MISO(BL_Fun_Type newState) {
    return RomDriver_GLB_Swap_MCU_SPI_1_MOSI_With_MISO(newState);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_MCU_SPI_0_ACT_MOD_Sel(uint8_t mod) {
    return RomDriver_GLB_Set_MCU_SPI_0_ACT_MOD_Sel(mod);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Set_MCU_SPI_1_ACT_MOD_Sel(uint8_t mod) {
    return RomDriver_GLB_Set_MCU_SPI_1_ACT_MOD_Sel(mod);
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
BL_Err_Type GLB_Set_Psram_Pad_For_Flash(uint8_t enable) {
    return RomDriver_GLB_Set_Psram_Pad_For_Flash(enable);
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
BL_Err_Type GLB_Trim_Ldo18ioVoutTrim(void) {
    return RomDriver_GLB_Trim_Ldo18ioVoutTrim();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void GLB_Power_Down_Ldo18ioVout(void) {
    return RomDriver_GLB_Power_Down_Ldo18ioVout();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void GLB_Power_Up_Ldo18ioVout(void) {
    return RomDriver_GLB_Power_Up_Ldo18ioVout();
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
BL_Err_Type GLB_Config_WIFIPLL(uint8_t xtalType, const GLB_WIFIPLL_Cfg_Type *pllCfgList) {
    return RomDriver_GLB_Config_WIFIPLL(xtalType, pllCfgList);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Simple_Set_MCU_System_CLK(uint8_t clkFreq, uint8_t mcuClkDiv, uint8_t mcuPBclkDiv) {
    return RomDriver_GLB_Simple_Set_MCU_System_CLK(clkFreq, mcuClkDiv, mcuPBclkDiv);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_Embedded_Flash_Pad_Enable(uint8_t swapIo2Cs) {
    return RomDriver_GLB_Embedded_Flash_Pad_Enable(swapIo2Cs);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_SF_Write_Protect_Enable(void) {
    return RomDriver_GLB_SF_Write_Protect_Enable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_SF_Write_Protect_Disable(void) {
    return RomDriver_GLB_SF_Write_Protect_Disable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_SF_Write_Protect_Set(uint8_t region, uint8_t cmd, uint32_t address, uint32_t length) {
    return RomDriver_GLB_SF_Write_Protect_Set(region, cmd, address, length);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void GLB_Write_SW_Reserved_Register(uint8_t index, uint32_t value) {
    return RomDriver_GLB_Write_SW_Reserved_Register(index, value);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t GLB_Read_SW_Reserved_Register(uint8_t index) {
    return RomDriver_GLB_Read_SW_Reserved_Register(index);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void GLB_Bus_Decoder_Err_Enable(void) {
    return RomDriver_GLB_Bus_Decoder_Err_Enable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void GLB_Bus_Decoder_Err_Disable(void) {
    return RomDriver_GLB_Bus_Decoder_Err_Disable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Ctrl_Ldo_Aon_Vout_in_HBN(uint8_t ldo_level, uint8_t ldo_trim) {
    return RomDriver_HBN_Ctrl_Ldo_Aon_Vout_in_HBN(ldo_level, ldo_trim);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Level_Select(uint8_t hbn_level) {
    return RomDriver_HBN_Level_Select(hbn_level);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void HBN_Power_Down_Flash(spi_flash_cfg_type *flashCfg) {
    return RomDriver_HBN_Power_Down_Flash(flashCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Reset(void) {
    return RomDriver_HBN_Reset();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type HBN_Get_POR_OUT_State(void) {
    return RomDriver_HBN_Get_POR_OUT_State();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type HBN_Get_BOD_OUT_State(void) {
    return RomDriver_HBN_Get_BOD_OUT_State();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_BOD_Config(uint8_t enable, uint8_t threshold, uint8_t mode) {
    return RomDriver_HBN_Set_BOD_Config(enable, threshold, mode);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_BOD_Cfg(HBN_BOD_CFG_Type *cfg) {
    return RomDriver_HBN_Set_BOD_Cfg(cfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void HBN_Get_Reset_Event(uint8_t *event) {
    return RomDriver_HBN_Get_Reset_Event(event);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void HBN_Clr_Reset_Event(void) {
    return RomDriver_HBN_Clr_Reset_Event();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_SW_Set_Ldo_Aon_Vout(uint8_t ldoLevel) {
    return RomDriver_HBN_SW_Set_Ldo_Aon_Vout(ldoLevel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_SW_Trim_Ldo_Aon_Vout(uint8_t trim) {
    return RomDriver_HBN_SW_Trim_Ldo_Aon_Vout(trim);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_RTC_CLK_Sel(uint8_t clkType) {
    return RomDriver_HBN_Set_RTC_CLK_Sel(clkType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Get_RTC_CLK_Sel(uint8_t *clkType) {
    return RomDriver_HBN_Get_RTC_CLK_Sel(clkType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Xtal_Divied_CLK(uint8_t enable, uint8_t div) {
    return RomDriver_HBN_Set_Xtal_Divied_CLK(enable, div);
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
BL_Err_Type HBN_Set_GPIO_INT_DET_CLK_Sel(uint8_t clkSel) {
    return RomDriver_HBN_Set_GPIO_INT_DET_CLK_Sel(clkSel);
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
BL_Err_Type HBN_SW_Set_Ldo_Soc_Vout(uint8_t ldoLevel) {
    return RomDriver_HBN_SW_Set_Ldo_Soc_Vout(ldoLevel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_SW_Trim_Ldo_Soc_Vout(uint8_t trim) {
    return RomDriver_HBN_SW_Trim_Ldo_Soc_Vout(trim);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Ldo_Soc_Vout_in_Lowpower(uint8_t ldoLevel) {
    return RomDriver_HBN_Set_Ldo_Soc_Vout_in_Lowpower(ldoLevel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Trim_Ldo_Soc_Vout_in_Lowpower(uint8_t trim) {
    return RomDriver_HBN_Trim_Ldo_Soc_Vout_in_Lowpower(trim);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Ctrl_Ldo_Soc_Mode_by_HW(uint8_t enable) {
    return RomDriver_HBN_Ctrl_Ldo_Soc_Mode_by_HW(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Ldo_Soc_Mode(uint8_t mode) {
    return RomDriver_HBN_Set_Ldo_Soc_Mode(mode);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_SW_Set_Ldo_Sys_Vout(uint8_t ldolevel) {
    return RomDriver_HBN_SW_Set_Ldo_Sys_Vout(ldolevel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_SW_Trim_Ldo_Sys_Vout(uint8_t trim) {
    return RomDriver_HBN_SW_Trim_Ldo_Sys_Vout(trim);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Ldo_Sys_Vout_in_Lowpower(uint8_t ldolevel) {
    return RomDriver_HBN_Set_Ldo_Sys_Vout_in_Lowpower(ldolevel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Ctrl_Ldo_Sys_Mode_by_HW(uint8_t enable) {
    return RomDriver_HBN_Ctrl_Ldo_Sys_Mode_by_HW(enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Ldo_Sys_Mode(uint8_t mode) {
    return RomDriver_HBN_Set_Ldo_Sys_Mode(mode);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Power_Off_LDO18_IO(void) {
    return RomDriver_HBN_Power_Off_LDO18_IO();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Power_On_LDO18_IO(void) {
    return RomDriver_HBN_Power_On_LDO18_IO();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Xtal_32K_Inverter_Amplify_Strength(uint8_t value) {
    return RomDriver_HBN_Set_Xtal_32K_Inverter_Amplify_Strength(value);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_Xtal_32K_Regulator(uint8_t level) {
    return RomDriver_HBN_Set_Xtal_32K_Regulator(level);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Start_Xtal_32K(void) {
    return RomDriver_HBN_Start_Xtal_32K();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Clear_Xtal_32K_Fast_Startup(void) {
    return RomDriver_HBN_Clear_Xtal_32K_Fast_Startup();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Sts_Type HBN_Get_Xtal_32K_Ready_State(void) {
    return RomDriver_HBN_Get_Xtal_32K_Ready_State();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Power_On_Xtal_32K(void) {
    return RomDriver_HBN_Power_On_Xtal_32K();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Power_Off_Xtal_32K(void) {
    return RomDriver_HBN_Power_Off_Xtal_32K();
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
BL_Err_Type HBN_Trim_Ldo_Soc_Vout(void) {
    return RomDriver_HBN_Trim_Ldo_Soc_Vout();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Trim_Ldo_Aon_Vout(void) {
    return RomDriver_HBN_Trim_Ldo_Aon_Vout();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Trim_Ldo_Sys_Vout(void) {
    return RomDriver_HBN_Trim_Ldo_Sys_Vout();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Set_RC32M_Speed(uint8_t rc32m_speed) {
    return RomDriver_HBN_Set_RC32M_Speed(rc32m_speed);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint8_t HBN_Get_RC32M_Speed(void) {
    return RomDriver_HBN_Get_RC32M_Speed();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type HBN_Trim_RC32M(void) {
    return RomDriver_HBN_Trim_RC32M();
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
void HBN_Wdg_Init(uint8_t mode, uint16_t comp_val) {
    return RomDriver_HBN_Wdg_Init(mode, comp_val);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void HBN_Wdg_Start(void) {
    return RomDriver_HBN_Wdg_Start();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void HBN_Wdg_Stop(void) {
    return RomDriver_HBN_Wdg_Stop();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint16_t HBN_Wdg_Get_Countervalue(void) {
    return RomDriver_HBN_Wdg_Get_Countervalue();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void HBN_Wdg_Set_Countervalue(uint16_t value) {
    return RomDriver_HBN_Wdg_Set_Countervalue(value);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void HBN_Wdg_Reset_Countervalue(void) {
    return RomDriver_HBN_Wdg_Reset_Countervalue();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void HBN_Wdg_Compint_Clear(void) {
    return RomDriver_HBN_Wdg_Compint_Clear();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
uint32_t HBN_Get_Version(void) {
    return RomDriver_HBN_Get_Version();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
void HBN_Set_Version(uint32_t version) {
    return RomDriver_HBN_Set_Version(version);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_GPIO_Pad_IntMask(uint8_t pad, BL_Mask_Type intMask) {
    return RomDriver_PDS_Set_GPIO_Pad_IntMask(pad, intMask);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_GPIO_Pad_IntMode(uint8_t pad, uint8_t trig) {
    return RomDriver_PDS_Set_GPIO_Pad_IntMode(pad, trig);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_GPIO_Pad_IntClr(uint8_t pad) {
    return RomDriver_PDS_Set_GPIO_Pad_IntClr(pad);
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
uint32_t PDS_Get_All_GPIO_Pad_IntStatus(void) {
    return RomDriver_PDS_Get_All_GPIO_Pad_IntStatus();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Disable_ALL_GPIO_Keep(void) {
    return RomDriver_PDS_Disable_ALL_GPIO_Keep();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Enable_GPIO_Keep(uint32_t pin) {
    return RomDriver_PDS_Enable_GPIO_Keep(pin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_PDS_GPIO_INT_DET_CLK_Sel(uint8_t clkSel) {
    return RomDriver_PDS_Set_PDS_GPIO_INT_DET_CLK_Sel(clkSel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Enable_WRAM_Access_Direct(void) {
    return RomDriver_PDS_Enable_WRAM_Access_Direct();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Disable_WRAM_Access_Direct(void) {
    return RomDriver_PDS_Disable_WRAM_Access_Direct();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_Ldo_Soc_Enter_lp_delay(uint16_t cnt) {
    return RomDriver_PDS_Set_Ldo_Soc_Enter_lp_delay(cnt);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_Ldo_Soc_Exit_lp_delay(uint16_t cnt) {
    return RomDriver_PDS_Set_Ldo_Soc_Exit_lp_delay(cnt);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_Ldo_Sys_Enter_lp_delay(uint16_t cnt) {
    return RomDriver_PDS_Set_Ldo_Sys_Enter_lp_delay(cnt);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_Ldo_Sys_Exit_lp_delay(uint16_t cnt) {
    return RomDriver_PDS_Set_Ldo_Sys_Exit_lp_delay(cnt);
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
BL_Err_Type PDS_Force_Config(PDS_CTL2_Type *cfg2, PDS_CTL3_Type *cfg3, PDS_CTL5_Type *cfg5, PDS_CTL6_Type *cfg6) {
    return RomDriver_PDS_Force_Config(cfg2, cfg3, cfg5, cfg6);
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
BL_Err_Type PDS_Power_Off_WB(void) {
    return RomDriver_PDS_Power_Off_WB();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Power_On_WB(void) {
    return RomDriver_PDS_Power_On_WB();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_USB_Suspend(void) {
    return RomDriver_PDS_Set_USB_Suspend();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Set_USB_Resume(void) {
    return RomDriver_PDS_Set_USB_Resume();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type PDS_Trim_USB20(void) {
    return RomDriver_PDS_Trim_USB20();
}
