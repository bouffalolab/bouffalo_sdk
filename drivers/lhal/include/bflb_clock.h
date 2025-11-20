#ifndef _BFLB_CLOCK_H
#define _BFLB_CLOCK_H

#include "bflb_core.h"

/** @addtogroup LHAL
  * @{
  */

/** @addtogroup CLOCK
  * @{
  */

/** @defgroup BFLB_SYSTEM_CLOCK system clock definition
  * @{
  */
#define BFLB_SYSTEM_ROOT_CLOCK 0
#define BFLB_SYSTEM_CPU_CLK    1
#define BFLB_SYSTEM_PBCLK      2
#define BFLB_SYSTEM_XCLK       3
#define BFLB_SYSTEM_32K_CLK    4
/**
  * @}
  */

#if defined(BL702) || defined(BL602) || defined(BL702L)
#define BFLB_GLB_CGEN0_BASE (0x40000000 + 0x20)
#define BFLB_GLB_CGEN1_BASE (0x40000000 + 0x24)
#define BFLB_GLB_CGEN2_BASE (0x40000000 + 0x28)
#elif defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL606P) || defined(BL808) || defined(BL628)
#define BFLB_GLB_CGEN0_BASE (0x20000000 + 0x580)
#define BFLB_GLB_CGEN1_BASE (0x20000000 + 0x584)
#define BFLB_GLB_CGEN2_BASE (0x20000000 + 0x588)
#endif

#if defined(BL616L)
#define PERIPHERAL_CLOCK_ADC_DAC_ENABLE()                         \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 2);                                       \
        regval |= (1 << 15);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)
#else
#define PERIPHERAL_CLOCK_ADC_DAC_ENABLE()                         \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 2);                                       \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)
#endif

#define PERIPHERAL_CLOCK_SEC_ENABLE()                             \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 3);                                       \
        regval |= (1 << 4);                                       \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)

#define PERIPHERAL_CLOCK_DMA0_ENABLE()                            \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 12);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)

#if defined(BL606P) || defined(BL808)
#define PERIPHERAL_CLOCK_DMA1_ENABLE()
#define PERIPHERAL_CLOCK_DMA2_ENABLE()
#endif

#define PERIPHERAL_CLOCK_UART0_ENABLE()                           \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 16);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)

#define PERIPHERAL_CLOCK_UART1_ENABLE()                           \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 17);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)

#if defined(BL606P) || defined(BL808) || defined(BL628)
#define PERIPHERAL_CLOCK_UART2_ENABLE()
#endif

#if defined(BL616L)
#define PERIPHERAL_CLOCK_UART3_ENABLE()                           \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 29);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)
#endif

#if defined(BL606P) || defined(BL808)
#define PERIPHERAL_CLOCK_SPI0_1_ENABLE()                          \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 18);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)
#elif defined(BL616D) && !defined(CPU_MODEL_A0)
#define PERIPHERAL_CLOCK_SPI0_2_ENABLE()                            \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 18);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)
#else
#define PERIPHERAL_CLOCK_SPI0_ENABLE()                            \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 18);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)
#endif

#if defined(BL616D)
#if defined(CPU_MODEL_A0)
#define PERIPHERAL_CLOCK_SPI1_ENABLE()                            \
    do {                                                          \
        *(volatile uint32_t *)0x20000548 &= ~(1 << 8);            \
        volatile uint32_t regval = getreg32(0x00080024);          \
        regval |= (1 << 1);                                       \
        putreg32(regval, 0x00080024);                             \
    } while (0)
#else
#define PERIPHERAL_CLOCK_SPI3_ENABLE()                            \
    do {                                                          \
        *(volatile uint32_t *)0x20000548 &= ~(1 << 8);            \
        volatile uint32_t regval = getreg32(0x00080024);          \
        regval |= (1 << 1);                                       \
        putreg32(regval, 0x00080024);                             \
    } while (0)
#endif
#endif

#define PERIPHERAL_CLOCK_I2C0_ENABLE()                            \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 19);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)

#if defined(BL606P) || defined(BL808) || defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL628)
#define PERIPHERAL_CLOCK_I2C1_ENABLE()                            \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 25);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)

#endif

#define PERIPHERAL_CLOCK_PWM0_ENABLE()                            \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 20);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)

#define PERIPHERAL_CLOCK_TIMER0_1_WDG_ENABLE()                    \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 21);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)

#define PERIPHERAL_CLOCK_IR_ENABLE()                              \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 22);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)

#define PERIPHERAL_CLOCK_CKS_ENABLE()                             \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 23);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)

#if defined(BL606P) || defined(BL808)
#define PERIPHERAL_CLOCK_CAN_UART2_ENABLE()                       \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 26);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)
#elif defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL628)
#define PERIPHERAL_CLOCK_CAN_ENABLE()                             \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 26);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)
#endif

#if defined(BL702)
#define PERIPHERAL_CLOCK_USB_ENABLE()                             \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 28);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)
#elif defined(BL616) || defined(BL606P) || defined(BL808) || defined(BL628)
#define PERIPHERAL_CLOCK_USB_ENABLE()                             \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 13);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)
#elif defined(BL616L)
#define PERIPHERAL_CLOCK_USB_ENABLE()                             \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 14);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)
#elif defined(BL616D)
#define PERIPHERAL_CLOCK_USB_ENABLE()                             \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN2_BASE); \
        regval |= (1 << 19);                                      \
        putreg32(regval, BFLB_GLB_CGEN2_BASE);                    \
    } while (0)
#endif


#if defined(BL702)
#define PERIPHERAL_CLOCK_I2S_ENABLE()                             \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 26);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)
#elif defined(BL616) || defined(BL616L) || defined(BL616D)  || defined(BL606P) || defined(BL808) || defined(BL628)
#define PERIPHERAL_CLOCK_I2S_ENABLE()                             \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 27);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)
#endif

#if defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL606P) || defined(BL808) || defined(BL628)
#define PERIPHERAL_CLOCK_SDH_ENABLE()                             \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN2_BASE); \
        regval |= (1 << 22);                                      \
        putreg32(regval, BFLB_GLB_CGEN2_BASE);                    \
    } while (0)
#endif

#if defined(BL702)
#define PERIPHERAL_CLOCK_EMAC_ENABLE()                            \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 13);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)
#elif defined(BL616) || defined(BL616L) || defined(BL616D) || defined(BL606P) || defined(BL808)
#define PERIPHERAL_CLOCK_EMAC_ENABLE()                            \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN2_BASE); \
        regval |= (1 << 23);                                      \
        putreg32(regval, BFLB_GLB_CGEN2_BASE);                    \
    } while (0)
#endif

#if defined(BL616) || defined(BL616L)
#define PERIPHERAL_CLOCK_AUDIO_ENABLE()                           \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN2_BASE); \
        regval |= (1 << 21);                                      \
        putreg32(regval, BFLB_GLB_CGEN2_BASE);                    \
    } while (0)
#endif

#if defined(BL616L)
#define PERIPHERAL_CLOCK_SDU_ENABLE()                             \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 13);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)
#endif

#if defined(BL616) || defined(BL616L)
#define PERIPHERAL_CLOCK_DBI_ENABLE()                             \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN1_BASE); \
        regval |= (1 << 24);                                      \
        putreg32(regval, BFLB_GLB_CGEN1_BASE);                    \
    } while (0)
#endif

#if defined(BL616D) || defined(BL616L)
#define PERIPHERAL_CLOCK_PEC_ENABLE()                             \
    do {                                                          \
        volatile uint32_t regval = getreg32(BFLB_GLB_CGEN2_BASE); \
        regval |= (1 << 25);                                      \
        putreg32(regval, BFLB_GLB_CGEN2_BASE);                    \
    } while (0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  enable/disable peri clock
 *
 * @param  peri: BFLB_PERIPHERAL_xxx
 * @param  enable: Boolean value to enable or disable the clock
 * @return int
 */
int bflb_peripheral_clock_control(uint8_t peri, bool enable);

/**
 * @brief  get peri clock value
 *
 * @param  peri: BFLB_PERIPHERAL_xxx
 * @return int
 */
uint32_t bflb_peripheral_clock_get(uint8_t peri);

/**
 * @brief  get peri clock status
 *
 * @param  peri: BFLB_PERIPHERAL_xxx
 * @return int
 */
int bflb_peripheral_clock_status_get(uint8_t peri);

/**
 * @brief Get system clock frequence
 *
 * @param [in] type system clock type
 * @return frequence
 */
uint32_t bflb_clk_get_system_clock(uint8_t type);

/**
 * @brief Get peripheral clock frequence
 *
 * @param [in] type peripheral type
 * @param [in] idx peripheral index
 * @return frequence
 */
uint32_t bflb_clk_get_peripheral_clock(uint8_t type, uint8_t idx);

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