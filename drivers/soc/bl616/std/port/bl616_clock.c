#include "bflb_clock.h"
#include "bflb_peri.h"
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
    } else if (type == BFLB_DEVICE_TYPE_SEC_PKA) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_SEC_PKA);
    } else if (type == BFLB_DEVICE_TYPE_SDH) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_SDH);
    } else if (type == BFLB_DEVICE_TYPE_CAMERA) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_CAM);
    } else if (type == BFLB_DEVICE_TYPE_DBI) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_DBI);
    } else if (type == BFLB_DEVICE_TYPE_I2S) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_I2S);
    } else if (type == BFLB_DEVICE_TYPE_RTC) {
        return Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_RTC);
    }

    return 0;
}

uint32_t bflb_peripheral_clock_get_by_id(uint8_t peri)
{
    uint8_t dev_type = 0;
    uint8_t idx = 0;

    switch (peri) {
        case BFLB_PERIPHERAL_GPADC0:
            dev_type = BFLB_DEVICE_TYPE_ADC;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_GPDAC0:
            dev_type = BFLB_DEVICE_TYPE_DAC;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_SF_CTRL:
            dev_type = BFLB_DEVICE_TYPE_FLASH;
            idx = 0;
            break;
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
        case BFLB_PERIPHERAL_SPI0:
            dev_type = BFLB_DEVICE_TYPE_SPI;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_I2C0:
            dev_type = BFLB_DEVICE_TYPE_I2C;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_PWM0:
            dev_type = BFLB_DEVICE_TYPE_PWM;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_TIMER0:
            dev_type = BFLB_DEVICE_TYPE_TIMER;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_IR:
            dev_type = BFLB_DEVICE_TYPE_IR;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_I2S0:
            dev_type = BFLB_DEVICE_TYPE_I2S;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_PSRAM1_CTRL:
            dev_type = BFLB_DEVICE_TYPE_PSRAM;
            idx = 1;
            break;
        case BFLB_PERIPHERAL_SDH0:
            dev_type = BFLB_DEVICE_TYPE_SDH;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_I2C1:
            dev_type = BFLB_DEVICE_TYPE_I2C;
            idx = 1;
            break;
        case BFLB_PERIPHERAL_DBI:
            dev_type = BFLB_DEVICE_TYPE_DBI;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_AUADC0:
            dev_type = BFLB_DEVICE_TYPE_AUDIOADC;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_CAM0:
            dev_type = BFLB_DEVICE_TYPE_CAMERA;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_SEC_PKA:
            dev_type = BFLB_DEVICE_TYPE_SEC_PKA;
            idx = 0;
            break;
        case BFLB_PERIPHERAL_RTC:
            dev_type = BFLB_DEVICE_TYPE_RTC;
            idx = 0;
            break;
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
            bitop(&regval1, 13);
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
        case BFLB_PERIPHERAL_CCI:
            bitop(&regval0, 4);
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
        case BFLB_PERIPHERAL_PSRAM1_CTRL:
            bitop(&regval2, 18);
            break;
        case BFLB_PERIPHERAL_USB20:
            bitop(&regval1, 13);
            break;
        case BFLB_PERIPHERAL_AUDAC0:
            bitop(&regval2, 21);
            break;
        case BFLB_PERIPHERAL_SDH0:
            bitop(&regval2, 22);
            break;
        case BFLB_PERIPHERAL_DBI:
            bitop(&regval1, 24);
            break;
        case BFLB_PERIPHERAL_AUADC0:
            bitop(&regval1, 28);
            break;
        case BFLB_PERIPHERAL_DMA_GPIO:
            bitop(&regval1, 0);
            break;
        case BFLB_PERIPHERAL_MM_MISC:
            bitop(&regval2, 16);
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
            tmpval = (regval1 >> 13) & 0x01;
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
        case BFLB_PERIPHERAL_CCI:
            tmpval = (regval0 >> 4) & 0x01;
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
        case BFLB_PERIPHERAL_PSRAM1_CTRL:
            tmpval = (regval2 >> 18) & 0x01;
            break;
        case BFLB_PERIPHERAL_USB20:
            tmpval = (regval1 >> 13) & 0x01;
            break;
        case BFLB_PERIPHERAL_AUDAC0:
            tmpval = (regval2 >> 21) & 0x01;
            break;
        case BFLB_PERIPHERAL_SDH0:
            tmpval = (regval2 >> 22) & 0x01;
            break;
        case BFLB_PERIPHERAL_DBI:
            tmpval = (regval1 >> 24) & 0x01;
            break;
        case BFLB_PERIPHERAL_AUADC0:
            tmpval = (regval1 >> 28) & 0x01;
            break;
        case BFLB_PERIPHERAL_DMA_GPIO:
            tmpval = (regval1 >> 0) & 0x01;
            break;
        case BFLB_PERIPHERAL_MM_MISC:
            tmpval = (regval2 >> 16) & 0x01;
            break;
        default:
            return -1;
    }

    return tmpval;
}
