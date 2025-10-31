#include "bflb_clock.h"
#include "bflb_peri.h"
#include "bl808_clock.h"

uint32_t bflb_clk_get_system_clock(uint8_t type)
{
    return 0;
}

uint32_t bflb_clk_get_peripheral_clock(uint8_t type, uint8_t idx)
{
    if (type == BFLB_DEVICE_TYPE_UART) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_UART0);
    } else if (type == BFLB_DEVICE_TYPE_SPI) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_SPI0);
    } else if (type == BFLB_DEVICE_TYPE_I2C) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_I2C0);
    } else if (type == BFLB_DEVICE_TYPE_SDH) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_SDH);
    }
    return 0;
}

uint32_t bflb_peripheral_clock_get_by_id(uint8_t peri)
{
    uint8_t dev_type = 0;
    uint8_t idx = 0;

    switch (peri) {
        case BFLB_PERIPHERAL_UART0:
            dev_type = BFLB_DEVICE_TYPE_UART;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_UART1:
            dev_type = BFLB_DEVICE_TYPE_UART;
            idx = 1;
            break;
        case BFLB_PERIPHERAL_UART2:
            dev_type = BFLB_DEVICE_TYPE_UART;
            idx = 2;
            break;
        case BFLB_PERIPHERAL_UART3:
            dev_type = BFLB_DEVICE_TYPE_UART;
            idx = 3;
            break;
#ifndef CONFIG_SUPPORT_UART_CLOCK_GET_ONLY
        case BFLB_PERIPHERAL_SPI0:
            dev_type = BFLB_DEVICE_TYPE_SPI;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_SPI1:
            dev_type = BFLB_DEVICE_TYPE_SPI;
            idx = 1;
            break;
        case BFLB_PERIPHERAL_I2C0:
            dev_type = BFLB_DEVICE_TYPE_I2C;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_I2C1:
            dev_type = BFLB_DEVICE_TYPE_I2C;
            idx = 1;
            break;
        case BFLB_PERIPHERAL_I2C2:
            dev_type = BFLB_DEVICE_TYPE_I2C;
            idx = 2;
            break;
        case BFLB_PERIPHERAL_I2C3:
            dev_type = BFLB_DEVICE_TYPE_I2C;
            idx = 3;
            break;
        case BFLB_PERIPHERAL_PSRAM0_CTRL:
            dev_type = BFLB_DEVICE_TYPE_PSRAM;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_PSRAM1_CTRL:
            dev_type = BFLB_DEVICE_TYPE_PSRAM;
            idx = 1;
            break;
        case BFLB_PERIPHERAL_SF_CTRL:
            dev_type = BFLB_DEVICE_TYPE_FLASH;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_I2S0:
            dev_type = BFLB_DEVICE_TYPE_I2S;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_IR:
            dev_type = BFLB_DEVICE_TYPE_IR;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_AUADC0:
            dev_type = BFLB_DEVICE_TYPE_AUDIOADC;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_AUDAC0:
            dev_type = BFLB_DEVICE_TYPE_AUDIODAC;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_GPADC0:
            dev_type = BFLB_DEVICE_TYPE_ADC;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_GPDAC0:
            dev_type = BFLB_DEVICE_TYPE_DAC;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_CAM0:
            dev_type = BFLB_DEVICE_TYPE_CAMERA;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_SDH0:
            dev_type = BFLB_DEVICE_TYPE_SDH;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_SEC_PKA:
            dev_type = BFLB_DEVICE_TYPE_SEC_PKA;
            idx = 0;
            break;
#endif
        default:
            return 0;
    }

    return bflb_clk_get_peripheral_clock(dev_type, idx);
}

static void ATTR_CLOCK_SECTION set_bit(uint32_t *value, int bit)
{
    *value |= (1 << bit);
}

static void ATTR_CLOCK_SECTION clear_bit(uint32_t *value, int bit)
{
    *value &= ~(1 << bit);
}

int ATTR_CLOCK_SECTION bflb_peripheral_clock_control_by_id(uint8_t peri, bool enable)
{
    uint32_t regval0 = getreg32(BFLB_GLB_CGEN0_BASE);
    uint32_t regval1 = getreg32(BFLB_GLB_CGEN1_BASE);
    uint32_t regval2 = getreg32(BFLB_GLB_CGEN2_BASE);

    void (*bitop)(uint32_t*, int) = enable ? set_bit : clear_bit;

    switch (peri) {
        case BFLB_PERIPHERAL_CPU:
            bitop(&regval0, 0);
            break;
        case BFLB_PERIPHERAL_SDU0:
            bitop(&regval0, 1);
            break;
        case BFLB_PERIPHERAL_SEC0:
            bitop(&regval0, 2);
            bitop(&regval1, 3);
            bitop(&regval1, 4);
            break;
        case BFLB_PERIPHERAL_DMA0:
            bitop(&regval0, 3);
            bitop(&regval1, 12);
            break;
        case BFLB_PERIPHERAL_DMA1:
            bitop(&regval0, 3);
            break;
        case BFLB_PERIPHERAL_DMA2:
            bitop(&regval0, 3);
            bitop(&regval2, 24);
            break;
        case BFLB_PERIPHERAL_CCI:
            bitop(&regval0, 4);
            break;
        case BFLB_PERIPHERAL_RF_TOP:
            bitop(&regval1, 1);
            break;
        case BFLB_PERIPHERAL_GPADC0:
            bitop(&regval1, 2);
            break;
        case BFLB_PERIPHERAL_GPDAC0:
            bitop(&regval1, 2);
            break;
        case BFLB_PERIPHERAL_TZ1:
            bitop(&regval1, 5);
            break;
        case BFLB_PERIPHERAL_TZ2:
            bitop(&regval1, 5);
            break;
        case BFLB_PERIPHERAL_EF_CTRL:
            bitop(&regval1, 7);
            break;
        case BFLB_PERIPHERAL_SF_CTRL:
            bitop(&regval1, 11);
            break;
        case BFLB_PERIPHERAL_EMAC0:
            bitop(&regval2, 23);
            break;
        case BFLB_PERIPHERAL_UART0:
            bitop(&regval1, 16);
            break;
        case BFLB_PERIPHERAL_UART1:
            bitop(&regval1, 17);
            break;
        case BFLB_PERIPHERAL_UART2:
            bitop(&regval1, 26);
            break;
        case BFLB_PERIPHERAL_SPI0:
            bitop(&regval1, 18);
            break;
        case BFLB_PERIPHERAL_I2C0:
            bitop(&regval1, 19);
            break;
        case BFLB_PERIPHERAL_I2C1:
            bitop(&regval1, 25);
            break;
        case BFLB_PERIPHERAL_PWM0:
            bitop(&regval1, 20);
            break;
        case BFLB_PERIPHERAL_TIMER0:
            bitop(&regval1, 21);
            break;
        case BFLB_PERIPHERAL_IR:
            bitop(&regval1, 22);
            break;
        case BFLB_PERIPHERAL_CHECKSUM:
            bitop(&regval1, 23);
            break;
        case BFLB_PERIPHERAL_I2S0:
            bitop(&regval1, 27);
            break;
        case BFLB_PERIPHERAL_EMI_MISC:
            bitop(&regval2, 16);
            break;
        case BFLB_PERIPHERAL_PSRAM0_CTRL:
            bitop(&regval2, 17);
            break;
        case BFLB_PERIPHERAL_PSRAM1_CTRL:
            bitop(&regval2, 18);
            break;
        case BFLB_PERIPHERAL_USB20:
            bitop(&regval1, 13);
            break;
        case BFLB_PERIPHERAL_MIX:
            bitop(&regval2, 20);
            break;
        case BFLB_PERIPHERAL_SDH0:
            bitop(&regval2, 22);
            break;
        case BFLB_PERIPHERAL_LZ4:
            bitop(&regval1, 29);
            break;
        case BFLB_PERIPHERAL_AUPDM:
            bitop(&regval1, 28);
            break;
        case BFLB_PERIPHERAL_GAUGE:
            bitop(&regval1, 0);
            break;
        default:
            return -1;
    }
    putreg32(regval0, BFLB_GLB_CGEN0_BASE);
    putreg32(regval1, BFLB_GLB_CGEN1_BASE);
    putreg32(regval2, BFLB_GLB_CGEN2_BASE);

    return 0;
}

int bflb_peripheral_clock_status_get_by_id(uint8_t peri)
{
    uint32_t regval0 = getreg32(BFLB_GLB_CGEN0_BASE);
    uint32_t regval1 = getreg32(BFLB_GLB_CGEN1_BASE);
    uint32_t regval2 = getreg32(BFLB_GLB_CGEN2_BASE);
    uint8_t tmpval = 0;

    switch (peri) {
        case BFLB_PERIPHERAL_CPU:
            tmpval = (regval0 >> 0) & 0x01;
            break;
        case BFLB_PERIPHERAL_SDU0:
            tmpval = (regval0 >> 1) & 0x01;
            break;
        case BFLB_PERIPHERAL_SEC0:
            tmpval = (regval0 >> 2) & 0x01;
            tmpval &= (regval1 >> 3) & 0x01;
            tmpval &= (regval1 >> 4) & 0x01;
            break;
        case BFLB_PERIPHERAL_DMA0:
            tmpval = (regval0 >> 3) & 0x01;
            tmpval &= (regval1 >> 12) & 0x01;
            break;
        case BFLB_PERIPHERAL_DMA1:
            tmpval = (regval0 >> 3) & 0x01;
            break;
        case BFLB_PERIPHERAL_DMA2:
            tmpval = (regval0 >> 3) & 0x01;
            tmpval = (regval2 >> 24) & 0x01;
            break;
        case BFLB_PERIPHERAL_CCI:
            tmpval = (regval0 >> 4) & 0x01;
            break;
        case BFLB_PERIPHERAL_RF_TOP:
            tmpval = (regval1 >> 1) & 0x01;
            break;
        case BFLB_PERIPHERAL_GPADC0:
            tmpval = (regval1 >> 2) & 0x01;
            break;
        case BFLB_PERIPHERAL_GPDAC0:
            tmpval = (regval1 >> 2) & 0x01;
            break;
        case BFLB_PERIPHERAL_TZ1:
            tmpval = (regval1 >> 5) & 0x01;
            break;
        case BFLB_PERIPHERAL_TZ2:
            tmpval = (regval1 >> 5) & 0x01;
            break;
        case BFLB_PERIPHERAL_EF_CTRL:
            tmpval = (regval1 >> 7) & 0x01;
            break;
        case BFLB_PERIPHERAL_SF_CTRL:
            tmpval = (regval1 >> 11) & 0x01;
            break;
        case BFLB_PERIPHERAL_EMAC0:
            tmpval = (regval2 >> 23) & 0x01;
            break;
        case BFLB_PERIPHERAL_UART0:
            tmpval = (regval1 >> 16) & 0x01;
            break;
        case BFLB_PERIPHERAL_UART1:
            tmpval = (regval1 >> 17) & 0x01;
            break;
        case BFLB_PERIPHERAL_UART2:
            tmpval = (regval1 >> 26) & 0x01;
            break;
        case BFLB_PERIPHERAL_SPI0:
            tmpval = (regval1 >> 18) & 0x01;
            break;
        case BFLB_PERIPHERAL_I2C0:
            tmpval = (regval1 >> 19) & 0x01;
            break;
        case BFLB_PERIPHERAL_I2C1:
            tmpval = (regval1 >> 25) & 0x01;
            break;
        case BFLB_PERIPHERAL_PWM0:
            tmpval = (regval1 >> 20) & 0x01;
            break;
        case BFLB_PERIPHERAL_TIMER0:
            tmpval = (regval1 >> 21) & 0x01;
            break;
        case BFLB_PERIPHERAL_IR:
            tmpval = (regval1 >> 22) & 0x01;
            break;
        case BFLB_PERIPHERAL_CHECKSUM:
            tmpval = (regval1 >> 23) & 0x01;
            break;
        case BFLB_PERIPHERAL_I2S0:
            tmpval = (regval1 >> 27) & 0x01;
            break;
        case BFLB_PERIPHERAL_EMI_MISC:
            tmpval = (regval2 >> 16) & 0x01;
            break;
        case BFLB_PERIPHERAL_PSRAM0_CTRL:
            tmpval = (regval2 >> 17) & 0x01;
            break;
        case BFLB_PERIPHERAL_PSRAM1_CTRL:
            tmpval = (regval2 >> 18) & 0x01;
            break;
        case BFLB_PERIPHERAL_USB20:
            tmpval = (regval1 >> 13) & 0x01;
            break;
        case BFLB_PERIPHERAL_MIX:
            tmpval = (regval2 >> 20) & 0x01;
            break;
        case BFLB_PERIPHERAL_SDH0:
            tmpval = (regval2 >> 22) & 0x01;
            break;
        case BFLB_PERIPHERAL_LZ4:
            tmpval = (regval1 >> 29) & 0x01;
            break;
        case BFLB_PERIPHERAL_AUPDM:
            tmpval = (regval1 >> 28) & 0x01;
            break;
        case BFLB_PERIPHERAL_GAUGE:
            tmpval = (regval1 >> 0) & 0x01;
            break;
        default:
            return -1;
    }

    return tmpval;
}
