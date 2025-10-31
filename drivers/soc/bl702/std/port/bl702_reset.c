#include "bflb_reset.h"
#include "bflb_peri.h"
#include "bl702_glb.h"

int bflb_peripheral_reset_by_id(uint8_t peri)
{
    uint8_t rst_num = 0;

    switch (peri) {
        case BFLB_PERIPHERAL_GLB:
            rst_num = 0;
            break;
        case BFLB_PERIPHERAL_MIX:
            rst_num = 1;
            break;
        case BFLB_PERIPHERAL_GPADC0:
            rst_num = 2;
            break;
        case BFLB_PERIPHERAL_GPDAC0:
            rst_num = 2;
            break;
        case BFLB_PERIPHERAL_SEC_DBG:
            rst_num = 3;
            break;
        case BFLB_PERIPHERAL_SEC_ENG:
            rst_num = 4;
            break;
        case BFLB_PERIPHERAL_TZ1:
            rst_num = 5;
            break;
        case BFLB_PERIPHERAL_TZ2:
            rst_num = 6;
            break;
        case BFLB_PERIPHERAL_EF_CTRL:
            rst_num = 7;
            break;
        case BFLB_PERIPHERAL_CCI:
            rst_num = 8;
            break;
        case BFLB_PERIPHERAL_L1C:
            rst_num = 9;
            break;
        case BFLB_PERIPHERAL_SF_CTRL:
            rst_num = 11;
            break;
        case BFLB_PERIPHERAL_DMA0:
            rst_num = 12;
            break;
        case BFLB_PERIPHERAL_EMAC0:
            rst_num = 13;
            break;
        case BFLB_PERIPHERAL_PDS:
            rst_num = 14;
            break;
        case BFLB_PERIPHERAL_UART0:
            rst_num = 16;
            break;
        case BFLB_PERIPHERAL_UART1:
            rst_num = 17;
            break;
        case BFLB_PERIPHERAL_SPI0:
            rst_num = 18;
            break;
        case BFLB_PERIPHERAL_I2C0:
            rst_num = 19;
            break;
        case BFLB_PERIPHERAL_PWM0:
            rst_num = 20;
            break;
        case BFLB_PERIPHERAL_TIMER0:
            rst_num = 21;
            break;
        case BFLB_PERIPHERAL_IR:
            rst_num = 22;
            break;
        case BFLB_PERIPHERAL_CHECKSUM:
            rst_num = 23;
            break;
        case BFLB_PERIPHERAL_QDEC:
            rst_num = 24;
            break;
        case BFLB_PERIPHERAL_KYS:
            rst_num = 25;
            break;
        case BFLB_PERIPHERAL_I2S0:
            rst_num = 26;
            break;
        case BFLB_PERIPHERAL_USB:
            rst_num = 28;
            break;
        case BFLB_PERIPHERAL_CAM0:
            rst_num = 29;
            break;
        case BFLB_PERIPHERAL_MJPEG:
            rst_num = 30;
            break;
        default:
            return -1;
    }

    return GLB_AHB_Slave1_Reset(rst_num);
}