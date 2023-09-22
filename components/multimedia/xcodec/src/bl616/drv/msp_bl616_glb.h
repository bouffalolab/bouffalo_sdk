
#ifndef __MSP_BL616_GLB_H__
#define __MSP_BL616_GLB_H__

#include "msp_glb_reg.h"

///////////////////////////////////////////////////////////////////////////////////////////
typedef enum {
    GLB_DMA0_CLK_CH0, /*!< DMA0 clock ID:channel 0 */
    GLB_DMA0_CLK_CH1, /*!< DMA0 clock ID:channel 1 */
    GLB_DMA0_CLK_CH2, /*!< DMA0 clock ID:channel 2 */
    GLB_DMA0_CLK_CH3, /*!< DMA0 clock ID:channel 3 */
} GLB_DMA_CLK_ID_Type;

typedef enum {
    GLB_AHB_CLOCK_IP_CPU,
    GLB_AHB_CLOCK_IP_RSVD,
    GLB_AHB_CLOCK_IP_SEC,
    GLB_AHB_CLOCK_IP_DMA_0,
    GLB_AHB_CLOCK_IP_DMA_1,
    GLB_AHB_CLOCK_IP_DMA_2,
    GLB_AHB_CLOCK_IP_CCI,
    GLB_AHB_CLOCK_IP_RF_TOP,
    GLB_AHB_CLOCK_IP_GPIP,
    GLB_AHB_CLOCK_IP_TZC,
    GLB_AHB_CLOCK_IP_EF_CTRL,
    GLB_AHB_CLOCK_IP_SF_CTRL,
    GLB_AHB_CLOCK_IP_EMAC,
    GLB_AHB_CLOCK_IP_UART0,
    GLB_AHB_CLOCK_IP_UART1,
    GLB_AHB_CLOCK_IP_UART2,
    GLB_AHB_CLOCK_IP_UART3,
    GLB_AHB_CLOCK_IP_UART4,
    GLB_AHB_CLOCK_IP_SPI,
    GLB_AHB_CLOCK_IP_I2C,
    GLB_AHB_CLOCK_IP_PWM,
    GLB_AHB_CLOCK_IP_TIMER,
    GLB_AHB_CLOCK_IP_IR,
    GLB_AHB_CLOCK_IP_CHECKSUM,
    GLB_AHB_CLOCK_IP_QDEC,
    GLB_AHB_CLOCK_IP_KYS,
    GLB_AHB_CLOCK_IP_I2S,
    GLB_AHB_CLOCK_IP_USB11,
    GLB_AHB_CLOCK_IP_CAM,
    GLB_AHB_CLOCK_IP_MJPEG,
    GLB_AHB_CLOCK_IP_BT_BLE_NORMAL,
    GLB_AHB_CLOCK_IP_BT_BLE_LP,
    GLB_AHB_CLOCK_IP_ZB_NORMAL,
    GLB_AHB_CLOCK_IP_ZB_LP,
    GLB_AHB_CLOCK_IP_WIFI_NORMAL,
    GLB_AHB_CLOCK_IP_WIFI_LP,
    GLB_AHB_CLOCK_IP_BT_BLE_2_NORMAL,
    GLB_AHB_CLOCK_IP_BT_BLE_2_LP,
    GLB_AHB_CLOCK_IP_EMI_MISC,
    GLB_AHB_CLOCK_IP_PSRAM0_CTRL,
    GLB_AHB_CLOCK_IP_PSRAM1_CTRL,
    GLB_AHB_CLOCK_IP_USB20_SDU,
    GLB_AHB_CLOCK_IP_MIX2,
    GLB_AHB_CLOCK_IP_AUDIO,
    GLB_AHB_CLOCK_IP_SDH,
    GLB_AHB_CLOCK_IP_ZB2_NORMAL,
    GLB_AHB_CLOCK_IP_ZB2_LP,
    GLB_AHB_CLOCK_IP_I2C1,
    GLB_AHB_CLOCK_IP_WIFI_PHY,
    GLB_AHB_CLOCK_IP_WIFI_MAC_PHY,
    GLB_AHB_CLOCK_IP_WIFI_PLATFORM,
    GLB_AHB_CLOCK_IP_LZ4,
    GLB_AHB_CLOCK_IP_AUPDM,
    GLB_AHB_CLOCK_IP_GAUGE,
    GLB_AHB_CLOCK_IP_DBI,
    GLB_AHB_CLOCK_IP_PEC,
    GLB_AHB_CLOCK_IP_ISO11898,
    GLB_AHB_CLOCK_IP_AUSOLO_TOP,
    GLB_AHB_CLOCK_IP_DMA_GPIO,
    GLB_AHB_CLOCK_IP_MM_MISC,
} GLB_AHB_CLOCK_IP_Type;

#define GLB_AHB_CLOCK_DMA_0             (1ULL<<GLB_AHB_CLOCK_IP_DMA_0          )
#define GLB_AHB_CLOCK_DMA_1             (1ULL<<GLB_AHB_CLOCK_IP_DMA_1          )
#define GLB_AHB_CLOCK_DMA_2             (1ULL<<GLB_AHB_CLOCK_IP_DMA_2          )

#define IS_GLB_PLL_TYPE(type) (((type) == GLB_PLL_NONE) ||    \
                               ((type) == GLB_PLL_WIFIPLL) || \
                               ((type) == GLB_PLL_AUPLL))



BL_Err_Type ms_GLB_PER_Clock_UnGate(uint64_t ips);
BL_Err_Type ms_GLB_Set_DMA_CLK(uint8_t enable, GLB_DMA_CLK_ID_Type clk);
//BL_Err_Type GLB_Config_AUDIO_PLL_To_451P58M(void);
BL_Err_Type GLB_Config_AUDIO_PLL_To_491P52M(void);
///////////////////////////////////////////////////////////////////////////////////////////

#endif /* __BL616_GLB_H__ */
