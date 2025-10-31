#include "bflb_reset.h"
#include "bflb_peri.h"
#include "bl702l_glb.h"

int bflb_peripheral_reset_by_id(uint8_t peri)
{
    uint8_t rst_num = 0;

    switch (peri) {
        case BFLB_PERIPHERAL_GLB:
            rst_num = 32;
            break;
        case BFLB_PERIPHERAL_MIX:
            rst_num = 33;
            break;
        case BFLB_PERIPHERAL_GPADC0:
            rst_num = 34;
            break;
        case BFLB_PERIPHERAL_GPDAC0:
            rst_num = 34;
            break;
        case BFLB_PERIPHERAL_SEC_DBG:
            rst_num = 35;
            break;
        case BFLB_PERIPHERAL_SEC_ENG:
            rst_num = 36;
            break;
        case BFLB_PERIPHERAL_TZ1:
            rst_num = 37;
            break;
        case BFLB_PERIPHERAL_TZ2:
            rst_num = 38;
            break;
        case BFLB_PERIPHERAL_EF_CTRL:
            rst_num = 39;
            break;
        case BFLB_PERIPHERAL_CCI:
            rst_num = 40;
            break;
        case BFLB_PERIPHERAL_L1C:
            rst_num = 41;
            break;
        case BFLB_PERIPHERAL_SF_CTRL:
            rst_num = 43;
            break;
        case BFLB_PERIPHERAL_DMA0:
            rst_num = 44;
            break;
        case BFLB_PERIPHERAL_PDS:
            rst_num = 46;
            break;
        case BFLB_PERIPHERAL_UART0:
            rst_num = 48;
            break;
        case BFLB_PERIPHERAL_SPI0:
            rst_num = 50;
            break;
        case BFLB_PERIPHERAL_I2C0:
            rst_num = 51;
            break;
        case BFLB_PERIPHERAL_PWM0:
            rst_num = 52;
            break;
        case BFLB_PERIPHERAL_TIMER0:
            rst_num = 53;
            break;
        case BFLB_PERIPHERAL_IR:
            rst_num = 54;
            break;
        case BFLB_PERIPHERAL_CHECKSUM:
            rst_num = 55;
            break;
        case BFLB_PERIPHERAL_KYS:
            rst_num = 57;
            break;
        case BFLB_PERIPHERAL_AUDIO:
            rst_num = 61;
            break;
        default:
            return -1;
    }

    return GLB_AHB_MCU_Software_Reset(rst_num);
}