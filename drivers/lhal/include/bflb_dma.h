#ifndef _BFLB_DMA_H
#define _BFLB_DMA_H

#include "bflb_core.h"

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup DMA
  * @{
  */

/** @defgroup DMA_DIRECTION dma transfer direction definition
  * @{
  */
#define DMA_MEMORY_TO_MEMORY       0
#define DMA_MEMORY_TO_PERIPH       1
#define DMA_PERIPH_TO_MEMORY       2
#define DMA_PERIPH_TO_PERIPH       3
/**
  * @}
  */

/** @defgroup DMA_ADDR_INCREMENT dma source and destination address increment definition
  * @{
  */
#define DMA_ADDR_INCREMENT_DISABLE 0
#define DMA_ADDR_INCREMENT_ENABLE  1
/**
  * @}
  */

/** @defgroup DMA_DATA_WIDTH dma data width definition
  * @{
  */
#define DMA_DATA_WIDTH_8BIT        0
#define DMA_DATA_WIDTH_16BIT       1
#define DMA_DATA_WIDTH_32BIT       2
/**
  * @}
  */

/** @defgroup DMA_BURST_COUNT dma burst increment count definition
  * @{
  */
#define DMA_BURST_INCR1            0
#define DMA_BURST_INCR4            1
#define DMA_BURST_INCR8            2
#define DMA_BURST_INCR16           3
/**
  * @}
  */

#if defined(BL702) || defined(BL602) || defined(BL702L)
/** @defgroup DMA_PERIPHERAL_REGBASE dma peripheral data register address definition
  * @{
  */
#define DMA_ADDR_UART0_TDR (0x4000A000 + 0x88)
#define DMA_ADDR_UART0_RDR (0x4000A000 + 0x8C)
#if !defined(BL702L)
#define DMA_ADDR_UART1_TDR (0x4000A100 + 0x88)
#define DMA_ADDR_UART1_RDR (0x4000A100 + 0x8C)
#endif
#define DMA_ADDR_I2C0_TDR (0x4000A300 + 0x88)
#define DMA_ADDR_I2C0_RDR (0x4000A300 + 0x8C)
#define DMA_ADDR_SPI0_TDR (0x4000A200 + 0x88)
#define DMA_ADDR_SPI0_RDR (0x4000A200 + 0x8C)
#if !defined(BL702L)
#define DMA_ADDR_I2S_TDR (0x4000AA00 + 0x88)
#define DMA_ADDR_I2S_RDR (0x4000AA00 + 0x8C)
#endif
#define DMA_ADDR_ADC_RDR (0x40002000 + 0x04)
#if !defined(BL702L)
#define DMA_ADDR_DAC_TDR (0x40002000 + 0x48)
#endif
#if defined(BL702L)
#define DMA_ADDR_IR_TDR    (0x4000A600 + 0x88)
#define DMA_ADDR_AUADC_RDR (0x4000AD00 + 0x88)
#endif
/**
  * @}
  */

/** @defgroup DMA_PERIPHERAL_REQUEST dma peripheral request definition
  * @{
  */
#define DMA_REQUEST_NONE     0x00000000
#define DMA_REQUEST_UART0_RX 0x00000000
#define DMA_REQUEST_UART0_TX 0x00000001
#if !defined(BL702L)
#define DMA_REQUEST_UART1_RX 0x00000002
#define DMA_REQUEST_UART1_TX 0x00000003
#endif
#define DMA_REQUEST_I2C0_RX 0x00000006
#define DMA_REQUEST_I2C0_TX 0x00000007
#if defined(BL702L)
#define DMA_REQUEST_IR_TX 0x00000008
#endif
#define DMA_REQUEST_SPI0_RX 0x0000000A
#define DMA_REQUEST_SPI0_TX 0x0000000B
#if defined(BL702L)
#define DMA_REQUEST_AUADC_RX 0x0000000D
#endif
#if !defined(BL702L)
#define DMA_REQUEST_I2S_RX 0x00000014
#define DMA_REQUEST_I2S_TX 0x00000015
#endif
#define DMA_REQUEST_ADC 0x00000016
#if !defined(BL702L)
#define DMA_REQUEST_DAC 0x00000017
#endif
/**
  * @}
  */

#elif defined(BL616)
/** @defgroup DMA_PERIPHERAL_REGBASE dma peripheral data register address definition
  * @{
  */
#define DMA_ADDR_UART0_TDR   (0x2000A000 + 0x88)
#define DMA_ADDR_UART0_RDR   (0x2000A000 + 0x8C)
#define DMA_ADDR_UART1_TDR   (0x2000A100 + 0x88)
#define DMA_ADDR_UART1_RDR   (0x2000A100 + 0x8C)
#define DMA_ADDR_I2C0_TDR    (0x2000A300 + 0x88)
#define DMA_ADDR_I2C0_RDR    (0x2000A300 + 0x8C)
#define DMA_ADDR_SPI0_TDR    (0x2000A200 + 0x88)
#define DMA_ADDR_SPI0_RDR    (0x2000A200 + 0x8C)
#define DMA_ADDR_I2S_TDR     (0x2000AB00 + 0x88)
#define DMA_ADDR_I2S_RDR     (0x2000AB00 + 0x8C)
#define DMA_ADDR_ADC_RDR     (0x20002000 + 0x04)
#define DMA_ADDR_DAC_TDR     (0x20002000 + 0x48)
#define DMA_ADDR_DBI_TDR     (0x2000A800 + 0x88)
#define DMA_ADDR_AUDAC_TDR   (0x20055000 + 0x94)
#define DMA_ADDR_AUADC_RDR   (0x2000A000 + 0xC88)
#define DMA_ADDR_WO_TDR      (0x20000000 + 0xB04)
/**
  * @}
  */

/** @defgroup DMA_PERIPHERAL_REQUEST dma peripheral request definition
  * @{
  */
#define DMA_REQUEST_NONE     0x00000000
#define DMA_REQUEST_UART0_RX 0x00000000
#define DMA_REQUEST_UART0_TX 0x00000001
#define DMA_REQUEST_UART1_RX 0x00000002
#define DMA_REQUEST_UART1_TX 0x00000003
#define DMA_REQUEST_I2C0_RX  0x00000006
#define DMA_REQUEST_I2C0_TX  0x00000007
#define DMA_REQUEST_WO       0x00000009
#define DMA_REQUEST_SPI0_RX  0x0000000A
#define DMA_REQUEST_SPI0_TX  0x0000000B
#define DMA_REQUEST_DBI_TX   0x00000014
#define DMA_REQUEST_AUADC_RX 0x00000015
#define DMA_REQUEST_AUDAC_TX 0x0000000D
#define DMA_REQUEST_I2S_RX   0x00000010
#define DMA_REQUEST_I2S_TX   0x00000011
#define DMA_REQUEST_ADC      0x00000016
#define DMA_REQUEST_DAC      0x00000017
/**
  * @}
  */

#elif defined(BL616L)
/** @defgroup DMA_PERIPHERAL_REGBASE dma peripheral data register address definition
  * @{
  */
#define DMA_ADDR_UART0_TDR      (0x2000A000 + 0x88)
#define DMA_ADDR_UART0_RDR      (0x2000A000 + 0x8C)
#define DMA_ADDR_UART1_TDR      (0x2000A100 + 0x88)
#define DMA_ADDR_UART1_RDR      (0x2000A100 + 0x8C)
#define DMA_ADDR_UART2_TDR      (0x2000AD00 + 0x88)
#define DMA_ADDR_UART2_RDR      (0x2000AD00 + 0x8C)
#define DMA_ADDR_I2C0_TDR       (0x2000A300 + 0x88)
#define DMA_ADDR_I2C0_RDR       (0x2000A300 + 0x8C)
#define DMA_ADDR_WO_TDR         (0x20000000 + 0xB04)
#define DMA_ADDR_SPI0_TDR       (0x2000A200 + 0x88)
#define DMA_ADDR_SPI0_RDR       (0x2000A200 + 0x8C)
#define DMA_ADDR_AUDAC_TDR      (0x20055000 + 0x94)
#define DMA_ADDR_I2C1_TDR       (0x2000A900 + 0x88)
#define DMA_ADDR_I2C1_RDR       (0x2000A900 + 0x8C)
#define DMA_ADDR_I2S_TDR        (0x2000AB00 + 0x88)
#define DMA_ADDR_I2S_RDR        (0x2000AB00 + 0x8C)
#define DMA_ADDR_DBI_TDR        (0x2000A800 + 0x88)
#define DMA_ADDR_AUADC_RDR      (0x2000AC00 + 0x88)
#define DMA_ADDR_ADC_RDR        (0x20002000 + 0x04)
#define DMA_ADDR_ADC_INJECT_RDR (0x20002400 + 0x04)
#define DMA_ADDR_SPI1_TDR       (0x2000AE00 + 0x88)
#define DMA_ADDR_SPI1_RDR       (0x2000AE00 + 0x8C)
#define DMA_ADDR_PEC_SM0_TDR    (0x2005A000 + 0x10)
#define DMA_ADDR_PEC_SM1_TDR    (0x2005A000 + 0x14)
#define DMA_ADDR_PEC_SM0_RDR    (0x2005A000 + 0x20)
#define DMA_ADDR_PEC_SM1_RDR    (0x2005A000 + 0x24)
/**
  * @}
  */

/** @defgroup DMA_PERIPHERAL_REQUEST dma peripheral request definition
  * @{
  */
#define DMA_REQUEST_NONE        0x00000000
#define DMA_REQUEST_UART0_RX    0x00000000
#define DMA_REQUEST_UART0_TX    0x00000001
#define DMA_REQUEST_UART1_RX    0x00000002
#define DMA_REQUEST_UART1_TX    0x00000003
#define DMA_REQUEST_UART2_RX    0x0000001C
#define DMA_REQUEST_UART2_TX    0x0000001D
#define DMA_REQUEST_I2C0_RX     0x00000006
#define DMA_REQUEST_I2C0_TX     0x00000007
#define DMA_REQUEST_TIMER0      0x00000008
#define DMA_REQUEST_WO          0x00000009
#define DMA_REQUEST_SPI0_RX     0x0000000A
#define DMA_REQUEST_SPI0_TX     0x0000000B
#define DMA_REQUEST_AUDAC_TX    0x0000000D
#define DMA_REQUEST_I2C1_RX     0x0000000E
#define DMA_REQUEST_I2C1_TX     0x0000000F
#define DMA_REQUEST_I2S_RX      0x00000010
#define DMA_REQUEST_I2S_TX      0x00000011
#define DMA_REQUEST_TIMER1      0x00000013
#define DMA_REQUEST_DBI_TX      0x00000014
#define DMA_REQUEST_AUADC_RX    0x00000015
#define DMA_REQUEST_ADC         0x00000016
#define DMA_REQUEST_ADC_INJECT  0x00000017
#define DMA_REQUEST_SPI1_RX     0x0000001E
#define DMA_REQUEST_SPI1_TX     0x0000001F
#define DMA_REQUEST_PEC_SM0_RX  0x00000018
#define DMA_REQUEST_PEC_SM1_RX  0x00000019
#define DMA_REQUEST_PEC_SM0_TX  0x0000001A
#define DMA_REQUEST_PEC_SM1_TX  0x0000001B
/**
  * @}
  */

#elif defined(BL808) || defined(BL606P)
/** @defgroup DMA_PERIPHERAL_REGBASE dma peripheral data register address definition
  * @{
  */
#define DMA_ADDR_UART0_TDR   (0x2000A000 + 0x88)
#define DMA_ADDR_UART0_RDR   (0x2000A000 + 0x8C)
#define DMA_ADDR_UART1_TDR   (0x2000A100 + 0x88)
#define DMA_ADDR_UART1_RDR   (0x2000A100 + 0x8C)
#define DMA_ADDR_UART2_TDR   (0x2000AA00 + 0x88)
#define DMA_ADDR_UART2_RDR   (0x2000AA00 + 0x8C)
#define DMA_ADDR_UART3_TDR   (0x30002000 + 0x88)
#define DMA_ADDR_UART3_RDR   (0x30002000 + 0x8C)
#define DMA_ADDR_I2C0_TDR    (0x2000A300 + 0x88)
#define DMA_ADDR_I2C0_RDR    (0x2000A300 + 0x8C)
#define DMA_ADDR_I2C1_TDR    (0x2000A900 + 0x88)
#define DMA_ADDR_I2C1_RDR    (0x2000A900 + 0x8C)
#define DMA_ADDR_I2C2_TDR    (0x30003000 + 0x88)
#define DMA_ADDR_I2C2_RDR    (0x30003000 + 0x8C)
#define DMA_ADDR_I2C3_TDR    (0x30004000 + 0x88)
#define DMA_ADDR_I2C3_RDR    (0x30004000 + 0x8C)
#define DMA_ADDR_SPI0_TDR    (0x2000A200 + 0x88)
#define DMA_ADDR_SPI0_RDR    (0x2000A200 + 0x8C)
#define DMA_ADDR_SPI1_TDR    (0x30008000 + 0x88)
#define DMA_ADDR_SPI1_RDR    (0x30008000 + 0x8C)
#define DMA_ADDR_DBI_TDR     (0x3001b000 + 0x88)
#define DMA_ADDR_I2S_TDR     (0x2000AB00 + 0x88)
#define DMA_ADDR_I2S_RDR     (0x2000AB00 + 0x8C)
#define DMA_ADDR_ADC_RDR     (0x20002000 + 0x04)
#define DMA_ADDR_DAC_TDR     (0x20002000 + 0x48)
#define DMA_ADDR_IR_TDR      (0x2000A600 + 0x88)
#define DMA_ADDR_WO_TDR      (0x20000000 + 0xB04)

/**
  * @}
  */

/** @defgroup DMA_PERIPHERAL_REQUEST dma peripheral request definition
  * @{
  */
#define DMA_REQUEST_NONE     0x00000000
#define DMA_REQUEST_UART0_RX 0x00000000
#define DMA_REQUEST_UART0_TX 0x00000001
#define DMA_REQUEST_UART1_RX 0x00000002
#define DMA_REQUEST_UART1_TX 0x00000003
#define DMA_REQUEST_UART2_RX 0x00000004
#define DMA_REQUEST_UART2_TX 0x00000005
#define DMA_REQUEST_I2C0_RX  0x00000006
#define DMA_REQUEST_I2C0_TX  0x00000007
#define DMA_REQUEST_IR_TX    0x00000008
#define DMA_REQUEST_WO       0x00000009
#define DMA_REQUEST_SPI0_RX  0x0000000A
#define DMA_REQUEST_SPI0_TX  0x0000000B
#define DMA_REQUEST_AUDIO_RX 0x0000000C
#define DMA_REQUEST_AUDIO_TX 0x0000000D
#define DMA_REQUEST_I2C1_RX  0x0000000E
#define DMA_REQUEST_I2C1_TX  0x0000000F
#define DMA_REQUEST_I2S_RX   0x00000010
#define DMA_REQUEST_I2S_TX   0x00000011
#define DMA_REQUEST_DBI_TX   0x00000014
#define DMA_REQUEST_ADC      0x00000016
#define DMA_REQUEST_DAC      0x00000017

/* Only support dma2 */
#define DMA_REQUEST_UART3_RX 0x00000000
#define DMA_REQUEST_UART3_TX 0x00000001
#define DMA_REQUEST_SPI1_RX  0x00000002
#define DMA_REQUEST_SPI1_TX  0x00000003
#define DMA_REQUEST_I2C2_RX  0x00000006
#define DMA_REQUEST_I2C2_TX  0x00000007
#define DMA_REQUEST_I2C3_RX  0x00000008
#define DMA_REQUEST_I2C3_TX  0x00000009
/**
  * @}
  */

#elif defined(BL628)
/** @defgroup DMA_PERIPHERAL_REGBASE dma peripheral data register address definition
  * @{
  */
#define DMA_ADDR_UART0_TDR   (0x20010000 + 0x88)
#define DMA_ADDR_UART0_RDR   (0x20010000 + 0x8C)
#define DMA_ADDR_UART1_TDR   (0x20011000 + 0x88)
#define DMA_ADDR_UART1_RDR   (0x20011000 + 0x8C)
#define DMA_ADDR_UART2_TDR   (0x20012000 + 0x88)
#define DMA_ADDR_UART2_RDR   (0x20012000 + 0x8C)
#define DMA_ADDR_I2C0_TDR    (0x20014000 + 0x88)
#define DMA_ADDR_I2C0_RDR    (0x20014000 + 0x8C)
#define DMA_ADDR_I2C1_TDR    (0x20015000 + 0x88)
#define DMA_ADDR_I2C1_RDR    (0x20015000 + 0x8C)
#define DMA_ADDR_SPI0_TDR    (0x20018000 + 0x88)
#define DMA_ADDR_SPI0_RDR    (0x20018000 + 0x8C)
#define DMA_ADDR_DBI_TDR     (0x2001D000 + 0x88)
#define DMA_ADDR_I2S_TDR     (0x2001E000 + 0x88)
#define DMA_ADDR_I2S_RDR     (0x2001E000 + 0x8C)
#define DMA_ADDR_ADC_RDR     (0x20002000 + 0x04)
#define DMA_ADDR_DAC_TDR     (0x20002000 + 0x48)
/**
  * @}
  */

/** @defgroup DMA_PERIPHERAL_REQUEST dma peripheral request definition
  * @{
  */
#define DMA_REQUEST_NONE     0x00000000
#define DMA_REQUEST_UART0_RX 0x00000000
#define DMA_REQUEST_UART0_TX 0x00000001
#define DMA_REQUEST_UART1_RX 0x00000002
#define DMA_REQUEST_UART1_TX 0x00000003
#define DMA_REQUEST_UART2_RX 0x00000004
#define DMA_REQUEST_UART2_TX 0x00000005
#define DMA_REQUEST_I2C0_RX  0x00000006
#define DMA_REQUEST_I2C0_TX  0x00000007
#define DMA_REQUEST_I2C1_RX  0x00000008
#define DMA_REQUEST_I2C1_TX  0x00000009
#define DMA_REQUEST_SPI0_RX  0x0000000A
#define DMA_REQUEST_SPI0_TX  0x0000000B
#define DMA_REQUEST_I2S_RX   0x00000010
#define DMA_REQUEST_I2S_TX   0x00000011
#define DMA_REQUEST_DBI_TX   0x00000014
#define DMA_REQUEST_ADC      0x00000016
#define DMA_REQUEST_DAC      0x00000017
/**
  * @}
  */

#elif defined(BL616D)
/** @defgroup DMA_PERIPHERAL_REGBASE dma peripheral data register address definition
  * @{
  */
#define DMA_ADDR_WO_TDR    (0x20000000 + 0xB04)
#define DMA_ADDR_ADC0_RDR  (0x20002000 + 0x10)
#define DMA_ADDR_ADC1_RDR  (0x20002000 + 0x44)
#define DMA_ADDR_DAC_TDR   (0x20002000 + 0x48)
#define DMA_ADDR_UART0_TDR (0x20010000 + 0x88)
#define DMA_ADDR_UART0_RDR (0x20010000 + 0x8C)
#define DMA_ADDR_UART1_TDR (0x20011000 + 0x88)
#define DMA_ADDR_UART1_RDR (0x20011000 + 0x8C)
#define DMA_ADDR_UART2_TDR (0x20012000 + 0x88)
#define DMA_ADDR_UART2_RDR (0x20012000 + 0x8C)
#define DMA_ADDR_I2C0_TDR  (0x20014000 + 0x88)
#define DMA_ADDR_I2C0_RDR  (0x20014000 + 0x8C)
#define DMA_ADDR_I2C1_TDR  (0x20015000 + 0x88)
#define DMA_ADDR_I2C1_RDR  (0x20015000 + 0x8C)
#if defined(CPU_MODEL_A0)
#define DMA_ADDR_SPI0_TDR (0x20018000 + 0x88)
#define DMA_ADDR_SPI0_RDR (0x20018000 + 0x8C)
#else
#define DMA_ADDR_SPI0_TDR (0x2000B000 + 0x88)
#define DMA_ADDR_SPI0_RDR (0x2000B000 + 0x8C)
#define DMA_ADDR_SPI1_TDR (0x20018000 + 0x88)
#define DMA_ADDR_SPI1_RDR (0x20018000 + 0x8C)
#define DMA_ADDR_SPI2_TDR (0x20023000 + 0x88)
#define DMA_ADDR_SPI2_RDR (0x20023000 + 0x8C)
#endif
#define DMA_ADDR_IR_TDR      (0x2001B000 + 0x88)
#define DMA_ADDR_DBI_TDR     (0x2001D000 + 0x88)
#define DMA_ADDR_I2S_TDR     (0x2001E000 + 0x88)
#define DMA_ADDR_I2S_RDR     (0x2001E000 + 0x8C)
#define DMA_ADDR_PEC_SM0_TDR (0x2001F000 + 0x10)
#define DMA_ADDR_PEC_SM1_TDR (0x2001F000 + 0x14)
#define DMA_ADDR_PEC_SM2_TDR (0x2001F000 + 0x18)
#define DMA_ADDR_PEC_SM3_TDR (0x2001F000 + 0x1C)
#define DMA_ADDR_PEC_SM0_RDR (0x2001F000 + 0x20)
#define DMA_ADDR_PEC_SM1_RDR (0x2001F000 + 0x24)
#define DMA_ADDR_PEC_SM2_RDR (0x2001F000 + 0x28)
#define DMA_ADDR_PEC_SM3_RDR (0x2001F000 + 0x2C)

#define DMA2_ADDR_I2C2_TDR   (0x00082000 + 0x88)
#define DMA2_ADDR_I2C2_RDR   (0x00082000 + 0x8C)
#define DMA2_ADDR_UART3_TDR  (0x00084000 + 0x88)
#define DMA2_ADDR_UART3_RDR  (0x00084000 + 0x8C)
#if defined(CPU_MODEL_A0)
#define DMA2_ADDR_SPI1_TDR (0x00085000 + 0x88)
#define DMA2_ADDR_SPI1_RDR (0x00085000 + 0x8C)
#else
#define DMA2_ADDR_SPI3_TDR (0x00085000 + 0x88)
#define DMA2_ADDR_SPI3_RDR (0x00085000 + 0x8C)
#endif
/**
  * @}
  */

/** @defgroup DMA_PERIPHERAL_REQUEST dma peripheral request definition
  * @{
  */
#define DMA_REQUEST_NONE     0x00000000
#define DMA_REQUEST_UART0_RX 0x00000000
#define DMA_REQUEST_UART0_TX 0x00000001
#define DMA_REQUEST_UART1_RX 0x00000002
#define DMA_REQUEST_UART1_TX 0x00000003
#define DMA_REQUEST_UART2_RX 0x00000004
#define DMA_REQUEST_UART2_TX 0x00000005
#define DMA_REQUEST_I2C0_RX  0x00000006
#define DMA_REQUEST_I2C0_TX  0x00000007
#define DMA_REQUEST_IR_TX    0x00000008
#define DMA_REQUEST_WO       0x00000009
#define DMA_REQUEST_SPI0_RX  0x0000000A
#define DMA_REQUEST_SPI0_TX  0x0000000B
#define DMA_REQUEST_AUDAC_TX 0x0000000D
#define DMA_REQUEST_I2C1_RX  0x0000000E
#define DMA_REQUEST_I2C1_TX  0x0000000F
#define DMA_REQUEST_I2S_RX   0x00000010
#define DMA_REQUEST_I2S_TX   0x00000011
#define DMA_REQUEST_ADC0     0x00000012
#define DMA_REQUEST_ADC1     0x00000013
#define DMA_REQUEST_DBI_TX   0x00000014
#define DMA_REQUEST_AUADC_RX 0x00000015
#if defined(CPU_MODEL_A0)
#define DMA_REQUEST_DAC        0x00000017
#define DMA_REQUEST_PEC_SM0_RX 0x00000018
#define DMA_REQUEST_PEC_SM1_RX 0x00000019
#define DMA_REQUEST_PEC_SM2_RX 0x0000001A
#define DMA_REQUEST_PEC_SM3_RX 0x0000001B
#define DMA_REQUEST_PEC_SM0_TX 0x0000001C
#define DMA_REQUEST_PEC_SM1_TX 0x0000001D
#define DMA_REQUEST_PEC_SM2_TX 0x0000001E
#define DMA_REQUEST_PEC_SM3_TX 0x0000001F
#else
#define DMA_REQUEST_DSI_TX     0x00000016
#define DMA_REQUEST_DSI_RX     0x00000017
#define DMA_REQUEST_SPI1_RX    0x00000018
#define DMA_REQUEST_SPI1_TX    0x00000019
#define DMA_REQUEST_SPI2_RX    0x0000001A
#define DMA_REQUEST_SPI2_TX    0x0000001B

#define DMA_REQUEST_PEC_SM0_TX 0x0000001C /* only for DMA0 */
#define DMA_REQUEST_PEC_SM1_TX 0x0000001D /* only for DMA0 */
#define DMA_REQUEST_PEC_SM2_TX 0x0000001C /* only for DMA1 */
#define DMA_REQUEST_PEC_SM3_TX 0x0000001D /* only for DMA1 */
#define DMA_REQUEST_PEC_SM0_RX 0x0000001E /* only for DMA0 */
#define DMA_REQUEST_PEC_SM1_RX 0x0000001F /* only for DMA0 */
#define DMA_REQUEST_PEC_SM2_RX 0x0000001E /* only for DMA1 */
#define DMA_REQUEST_PEC_SM3_RX 0x0000001F /* only for DMA1 */
#endif

#define DMA2_REQUEST_I2C2_TX  0x00000000
#define DMA2_REQUEST_I2C2_RX  0x00000001
#define DMA2_REQUEST_UART3_TX 0x00000002
#define DMA2_REQUEST_UART3_RX 0x00000003
#if defined(CPU_MODEL_A0)
#define DMA2_REQUEST_SPI1_TX 0x00000004
#define DMA2_REQUEST_SPI1_RX 0x00000005
#else
#define DMA2_REQUEST_SPI3_TX 0x00000004
#define DMA2_REQUEST_SPI3_RX 0x00000005
#endif
/**
  * @}
  */

#endif

#if defined(BL616L)
/** @defgroup DMA_LLI_MUTEX dma lli mutex definition
  * @{
  */
#define DMA_LLI_MUTEX_UNAVAILABLE   (0)
#define DMA_LLI_MUTEX_NOT_LAST_NODE (1)
#define DMA_LLI_MUTEX_LAST_NODE     (2)
/**
  * @}
  */
#endif

/** @defgroup DMA_CMD dma feature control cmd definition
  * @{
  */
#define DMA_CMD_SET_SRCADDR_INCREMENT (0x01)
#define DMA_CMD_SET_DSTADDR_INCREMENT (0x02)
#define DMA_CMD_SET_ADD_MODE          (0x03)
#define DMA_CMD_SET_REDUCE_MODE       (0x04)
#define DMA_CMD_SET_LLI_CONFIG        (0x05)
#define DMA_CMD_GET_LLI_SRCADDR       (0x06)
#define DMA_CMD_GET_LLI_DSTADDR       (0x07)
#define DMA_CMD_GET_LLI_CONTROL       (0x08)
#define DMA_CMD_GET_LLI_COUNT         (0x09)
#if defined(BL616L)
#define DMA_CMD_SET_LLI_MUTEX        (0x0a)
#define DMA_CMD_GET_LLI_MUTEX_STATUS (0x0b)
#define DMA_CMD_READ_HW_VERSION      (0x0c)
#define DMA_CMD_READ_SW_USAGE        (0x0d)
#define DMA_CMD_WRITE_SW_USAGE       (0x0e)
#endif
#define DMA_CMD_GET_TRANSFER_PENDING (0x0f)
/**
  * @}
  */

// clang-format off
#define IS_DMA_DIRECTION(type)   (((type) == DMA_MEMORY_TO_MEMORY) || \
                                  ((type) == DMA_MEMORY_TO_PERIPH) || \
                                  ((type) == DMA_PERIPH_TO_MEMORY) || \
                                  ((type) == DMA_PERIPH_TO_PERIPH))

#define IS_DMA_DATA_WIDTH(type)   (((type) == DMA_DATA_WIDTH_8BIT) || \
                                  ((type) == DMA_DATA_WIDTH_16BIT) || \
                                  ((type) == DMA_DATA_WIDTH_32BIT))

#define IS_DMA_BURST_COUNT(type)   (((type) == DMA_BURST_INCR1) || \
                                  ((type) == DMA_BURST_INCR4) || \
                                  ((type) == DMA_BURST_INCR8) || \
                                  ((type) == DMA_BURST_INCR16))

// clang-format on

union bflb_dma_lli_control_s {
    struct
    {
        uint32_t TransferSize : 12; /* [11: 0],        r/w,        0x0 */
        uint32_t SBSize       : 2;  /* [13:12],        r/w,        0x1 */
        uint32_t dst_min_mode : 1;  /* [   14],        r/w,        0x0 */
        uint32_t DBSize       : 2;  /* [16:15],        r/w,        0x1 */
        uint32_t dst_add_mode : 1;  /* [   17],        r/w,        0x0 */
        uint32_t SWidth       : 2;  /* [19:18],        r/w,        0x2 */
        uint32_t reserved_20  : 1;  /* [   20],       rsvd,        0x0 */
        uint32_t DWidth       : 2;  /* [22:21],        r/w,        0x2 */
        uint32_t fix_cnt      : 2;  /* [24:23],        r/w,        0x0 */
        uint32_t SLargerD     : 1;  /* [   25],        r/w,        0x0 */
        uint32_t SI           : 1;  /* [   26],        r/w,        0x1 */
        uint32_t DI           : 1;  /* [   27],        r/w,        0x1 */
        uint32_t Prot         : 3;  /* [30:28],        r/w,        0x0 */
        uint32_t I            : 1;  /* [   31],        r/w,        0x0 */
    } bits;
    uint32_t WORD;
};

/**
 * @brief DMA channel lli pool structure
 *
 * @param src_addr        DMA source address
 * @param dst_addr        DMA destination address
 * @param nextlli         DMA next lli address
 * @param control         DMA lli config
 */
struct bflb_dma_channel_lli_pool_s {
    uint32_t src_addr;
    uint32_t dst_addr;
    uint32_t nextlli;
    union bflb_dma_lli_control_s control;
};

/**
 * @brief DMA channel lli transfer structure
 *
 * @param src_addr        DMA source address
 * @param dst_addr        DMA destination address
 * @param nbytes          How many bytes should be transferred
 */
struct bflb_dma_channel_lli_transfer_s {
    uint32_t src_addr;
    uint32_t dst_addr;
    uint32_t nbytes;
};

/**
 * @brief DMA configuration structure
 *
 * @param direction        DMA transfer direction, use @ref DMA_DIRECTION
 * @param src_req          DMA source request, use @ref DMA_PERIPHERAL_REQUEST
 * @param dst_req          DMA destination request, use @ref DMA_PERIPHERAL_REQUEST
 * @param src_addr_inc     DMA source address increment, use @ref DMA_ADDR_INCREMENT
 * @param dst_addr_inc     DMA destination address increment, use @ref DMA_ADDR_INCREMENT
 * @param src_burst_count  DMA source burst count, use @ref DMA_BURST_COUNT
 * @param dst_burst_count  DMA destination burst count, use @ref DMA_BURST_COUNT
 * @param src_width        DMA source data width, use @ref DMA_DATA_WIDTH
 * @param dst_width        DMA destination data width, use @ref DMA_DATA_WIDTH
 */
struct bflb_dma_channel_config_s {
    uint8_t direction;
    uint32_t src_req;
    uint32_t dst_req;
    uint8_t src_addr_inc;
    uint8_t dst_addr_inc;
    uint8_t src_burst_count;
    uint8_t dst_burst_count;
    uint8_t src_width;
    uint8_t dst_width;
};

struct bflb_rx_cycle_dma {
    struct bflb_device_s *dma_ch;
    uint32_t src_addr;
    uint8_t *dst_buf;
    uint32_t dst_buf_size;
    uint8_t *read_ptr;
    uint32_t dma_last_lli_count;
    void (*copy)(uint8_t *data, uint32_t len);
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize dma channel.
 *
 * @param [in] dev device handle
 * @param [in] config pointer to save dma channel configuration
 */
void bflb_dma_channel_init(struct bflb_device_s *dev, const struct bflb_dma_channel_config_s *config);

/**
 * @brief Deinitialize dma channel.
 *
 * @param [in] dev device handle
 */
void bflb_dma_channel_deinit(struct bflb_device_s *dev);

/**
 * @brief Start dma channel transfer.
 *
 * @param [in] dev device handle
 */
void bflb_dma_channel_start(struct bflb_device_s *dev);

/**
 * @brief Stop dma channel transfer.
 *
 * @param [in] dev device handle
 */
void bflb_dma_channel_stop(struct bflb_device_s *dev);

/**
 * @brief Check if dma channel is in busy.
 *
 * @param [in] dev device handle
 * @return true means dma channel does not transfer completely, otherwise transfers completely.
 */
bool bflb_dma_channel_isbusy(struct bflb_device_s *dev);

/**
 * @brief Register dma channel transmission completion interrupt callback.
 *
 * @param [in] dev device handle
 * @param [in] callback interrupt callback
 * @param [in] arg user data
 */
void bflb_dma_channel_irq_attach(struct bflb_device_s *dev, void (*callback)(void *arg), void *arg);

/**
 * @brief Unregister dma channel transmission completion interrupt callback.
 *
 * @param [in] dev device handle
 */
void bflb_dma_channel_irq_detach(struct bflb_device_s *dev);

/**
 * @brief Config dma channel lli.
 *
 * @param [in] dev device handle
 * @param [in] lli_pool pointer to lli pool
 * @param [in] max_lli_count lli pool size
 * @param [in] transfer pointer to transfer structure
 * @param [in] count transfer count.
 * @return A negated errno value on failure, otherwise means number of used lli.
 */
int bflb_dma_channel_lli_reload(struct bflb_device_s *dev,
                                struct bflb_dma_channel_lli_pool_s *lli_pool, uint32_t max_lli_count,
                                struct bflb_dma_channel_lli_transfer_s *transfer, uint32_t count);

#if defined(BL616L)
/**
 * @brief Insert dma channel lli node.
 *
 * @param [in] dev device handle
 * @param [in] lli_pool pointer to lli pool
 * @param [in] max_lli_count lli pool size
 * @param [in] transfer pointer to transfer structure
 * @param [in] count transfer count.
 * @return A negated errno value on failure, otherwise means number of used lli.
 */
int bflb_dma_channel_lli_insert(struct bflb_device_s *dev,
                                struct bflb_dma_channel_lli_pool_s *lli_pool, uint32_t max_lli_count,
                                struct bflb_dma_channel_lli_transfer_s *transfer, uint32_t count);
#endif

/**
 * @brief Enable lli continueous mode.
 *
 * @param [in] dev device handle
 * @param [in] lli_pool pointer to lli pool
 * @param [in] used_lli_count number of used lli.
 */
void bflb_dma_channel_lli_link_head(struct bflb_device_s *dev,
                                    struct bflb_dma_channel_lli_pool_s *lli_pool,
                                    uint32_t used_lli_count);

/**
 * @brief Init rx cycle dma.
 *
 * @param [in] rx_dma rx dma handle
 * @param [in] dma_ch dma channel handle
 * @param [in] rx_llipool pointer to lli pool
 * @param [in] rx_llipool_size number of used lli
 * @param [in] src_addr dma source addr
 * @param [in] dst_buf dma dest addr
 * @param [in] dst_buf_size dma dest buffer size
 * @param [in] copy pointer to data copy
 */
int bflb_rx_cycle_dma_init(struct bflb_rx_cycle_dma *rx_dma,
                           struct bflb_device_s *dma_ch,
                           struct bflb_dma_channel_lli_pool_s *rx_llipool,
                           uint8_t rx_llipool_size,
                           uint32_t src_addr,
                           uint8_t *dst_buf,
                           uint32_t dst_buf_size,
                           void (*copy)(uint8_t *data, uint32_t len));

/**
 * @brief Rx cycle dma process
 *
 * @param [in] rx_dma rx dma handle
 * @param [in] in_dma_isr if called in dma isr
 */
void bflb_rx_cycle_dma_process(struct bflb_rx_cycle_dma *rx_dma, bool in_dma_isr);

/**
 * @brief Control dma feature.
 *
 * @param [in] dev device handle
 * @param [in] cmd feature command. use @ref DMA_CMD
 * @param [in] arg user data
 * @return A negated errno value on failure.
 */
int bflb_dma_feature_control(struct bflb_device_s *dev, int cmd, size_t arg);

/**
 * @brief Enable or disable dma channel transmission completion interrupt.
 *
 * @param [in] dev device handle
 * @param [in] mask true means disable, false means enable
 */
void bflb_dma_channel_tcint_mask(struct bflb_device_s *dev, bool mask);

/**
 * @brief Check if dma channel transfers completely.
 *
 * @param [in] dev device handle
 * @return true means yes, false means no.
 */
bool bflb_dma_channel_get_tcint_status(struct bflb_device_s *dev);

/**
 * @brief Clear dma channel transmission completion interrupt status.
 *
 * @param [in] dev device handle
 */
void bflb_dma_channel_tcint_clear(struct bflb_device_s *dev);

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

/**
  * @}
  */

#endif
