#include "bflb_clock.h"
#include "bl616_clock.h"

uint32_t bflb_clk_get_system_clock(uint8_t type)
{
    switch (type) {
        case BFLB_SYSTEM_ROOT_CLOCK:
            return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_ROOT_CLK);
        case BFLB_SYSTEM_CPU_CLK:
            return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_CLK);
        case BFLB_SYSTEM_PBCLK:
            return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_MCU_PBCLK);
        case BFLB_SYSTEM_XCLK:
            return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK);
        case BFLB_SYSTEM_32K_CLK:
            return Clock_System_Clock_Get(BL_SYSTEM_CLOCK_F32K);
        default:
            return 0;
    }
    return 0;
}

uint32_t bflb_clk_get_peripheral_clock(uint8_t type, uint8_t idx)
{
    if (type == BFLB_DEVICE_TYPE_ADC) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_ADC);
    } else if (type == BFLB_DEVICE_TYPE_DAC) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_GPDAC);
    } else if (type == BFLB_DEVICE_TYPE_UART) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_UART0);
    } else if (type == BFLB_DEVICE_TYPE_SPI) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_SPI);
    } else if (type == BFLB_DEVICE_TYPE_I2C) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_I2C0);
    } else if (type == BFLB_DEVICE_TYPE_FLASH) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_FLASH);
    } else if (type == BFLB_DEVICE_TYPE_IR) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_IR);
    } else if (type == BFLB_DEVICE_TYPE_PKA) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_PKA);
    } else if (type == BFLB_DEVICE_TYPE_SDH) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_SDH);
    } else if (type == BFLB_DEVICE_TYPE_CAMERA) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_CAM);
    } else if (type == BFLB_DEVICE_TYPE_DBI) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_DBI);
    } else if (type == BFLB_DEVICE_TYPE_I2S) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_I2S);
    }
    return 0;
}
