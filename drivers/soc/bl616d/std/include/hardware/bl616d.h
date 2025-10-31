#ifndef __BL616D_H__
#define __BL616D_H__

/** @addtogroup Configuration_section_for_RISCV
  * @{
  */

/**
  * @brief Configuration of the Processor and Core Peripherals
   */

#define CORE_ID_ADDRESS (0xF0000000)
#if defined(CPU_MODEL_A0)
#define CORE_ID_M0 (0xE9070000)
#define CORE_ID_M1 (0xE9078000)
#else
#define CORE_ID_M0 (0xA900A900)
#define CORE_ID_M1 (0xF900F900)
#endif
#define CORE_ID_LP (0xDEAD2010)

/**
  * @}
  */

/** @addtogroup Peripheral_interrupt_number_definition
  * @{
  */

#ifdef ARCH_RISCV
#if (__riscv_xlen == 64)
#define IRQ_NUM_BASE 16 /* PLIC ===> MEXT_IRQn */
#else
#if defined(CPU_MODEL_A0) || defined(CPU_LP)
#define IRQ_NUM_BASE 16
#else
#define IRQ_NUM_BASE 19
#endif
#endif
#endif
/**
 * @brief BL616D Interrupt Number Definition, according to the selected device
 *        in @ref Library_configuration_section
 */
typedef enum {
#ifdef ARCH_RISCV
    SSOFT_IRQn = 1,           /*!< 1 RISCV supervisor software Interrupt                             */
    MSOFT_IRQn = 3,           /*!< 3 RISCV machine software Interrupt                                */
    STIME_IRQn = 5,           /*!< 5 RISCV supervisor time Interrupt                                 */
    MTIME_IRQn = 7,           /*!< 7 RISCV machine time Interrupt                                    */
    SEXT_IRQn = 9,            /*!< 9 RISCV S-mode external  Interrupt                                */
    MEXT_IRQn = 11,           /*!< 11 RISCV M-mode external  Interrupt                               */
    CLIC_SOFT_PEND_IRQn = 12, /*!< 12 RISCV CLIC software pending  Interrupt                         */
#if (__riscv_xlen == 64)
    HPM_OVF_IRQn = 17, /*!< 17 RISCV HPM counter overflow Interrupt                           */
#endif
#endif
#if defined(CPU_LP)
    /******  BL616D specific Interrupt Numbers **********************************************************************/
    DMA2_INT0_IRQn = IRQ_NUM_BASE + 0,   /*!< DMA2_INT0 Interrupt                                            */
    DMA2_INT1_IRQn = IRQ_NUM_BASE + 1,   /*!< DMA2_INT1 Interrupt                                            */
    DMA2_INT2_IRQn = IRQ_NUM_BASE + 2,   /*!< DMA2_INT2 Interrupt                                            */
    DMA2_INT3_IRQn = IRQ_NUM_BASE + 3,   /*!< DMA2_INT3 Interrupt                                            */
    RESERVED4_IRQn = IRQ_NUM_BASE + 4,   /*!< Reserved Interrupt                                             */
    RESERVED5_IRQn = IRQ_NUM_BASE + 5,   /*!< Reserved Interrupt                                             */
    RESERVED6_IRQn = IRQ_NUM_BASE + 6,   /*!< Reserved Interrupt                                             */
    RESERVED7_IRQn = IRQ_NUM_BASE + 7,   /*!< Reserved Interrupt                                             */
    PWM1_IRQn = IRQ_NUM_BASE + 8,        /*!< PWM1 Interrupt                                                 */
    I2C2_IRQn = IRQ_NUM_BASE + 9,        /*!< I2C2 Interrupt                                                 */
    RESERVED10_IRQn = IRQ_NUM_BASE + 10, /*!< Reserved Interrupt                                             */
    TIMER2_IRQn = IRQ_NUM_BASE + 11,     /*!< Timer1 Channel 0 Interrupt                                     */
    TIMER3_IRQn = IRQ_NUM_BASE + 12,     /*!< Timer1 Channel 1 Interrupt                                     */
    WDG1_IRQn = IRQ_NUM_BASE + 13,       /*!< Timer1 Watch Dog Interrupt                                     */
    UART3_IRQn = IRQ_NUM_BASE + 14,      /*!< UART3 Interrupt                                                */
#if defined(CPU_MODEL_A0)
    SPI1_IRQn = IRQ_NUM_BASE + 15, /*!< SPI1 Interrupt                                                 */
#else
    SPI3_IRQn = IRQ_NUM_BASE + 15,   /*!< SPI3 Interrupt                                                 */
#endif
    WIFI_TO_CPU_PART2_IRQn = IRQ_NUM_BASE + 16, /*!< WIFI To CPU Part2 Interrupt                                    */
    RF_TOP_INT0_IRQn = IRQ_NUM_BASE + 17,       /*!< RF_TOP_INT0 Interrupt                                          */
    RF_TOP_INT1_IRQn = IRQ_NUM_BASE + 18,       /*!< RF_TOP_INT1 Interrupt                                          */
    WIFI_TBTT_SLEEP_IRQn = IRQ_NUM_BASE + 19,   /*!< WIFI TBTT Sleep Interrupt                                      */
    WIFI_TBTT_WAKEUP_IRQn = IRQ_NUM_BASE + 20,  /*!< WIFI TBTT Wakeup Interrupt                                     */
    WIFI_IPC_IRQn = IRQ_NUM_BASE + 21,          /*!< WIFI IPC Interrupt                                             */
    IPC0_AP2NP_IRQn = IRQ_NUM_BASE + 22,        /*!< IPC0 AP to NP Interrupt                                        */
    IPC0_NP2AP_IRQn = IRQ_NUM_BASE + 23,        /*!< IPC0 NP to AP Interrupt                                        */
    IPC1_AP2NP_IRQn = IRQ_NUM_BASE + 24,        /*!< IPC1 AP to NP  Interrupt                                       */
    IPC1_NP2AP_IRQn = IRQ_NUM_BASE + 25,        /*!< IPC1 NP to AP Interrupt                                        */
#if defined(CPU_MODEL_A0)
    RESERVED26_IRQn = IRQ_NUM_BASE + 26, /*!< Reserved Interrupt                                             */
    RESERVED27_IRQn = IRQ_NUM_BASE + 27, /*!< Reserved Interrupt                                             */
    RESERVED28_IRQn = IRQ_NUM_BASE + 28, /*!< Reserved Interrupt                                             */
#else
    CANFD_IRQn = IRQ_NUM_BASE + 26,  /*!< CANFD Interrupt                                              */
    CANFD1_IRQn = IRQ_NUM_BASE + 27, /*!< CANFD1 Interrupt                                             */
    CANFD2_IRQn = IRQ_NUM_BASE + 28, /*!< CANFD2 Interrupt                                             */
#endif
    RESERVED29_IRQn = IRQ_NUM_BASE + 29, /*!< Reserved Interrupt                                             */
    RESERVED30_IRQn = IRQ_NUM_BASE + 30, /*!< Reserved Interrupt                                             */
    NP2MINI_IRQn = IRQ_NUM_BASE + 31,    /*!< IPC0 NP to AP Interrupt                                        */
    IRQn_LAST,
#else
    /******  BL616D specific Interrupt Numbers **********************************************************************/
    BMX_MCU_BUS_ERR_IRQn = IRQ_NUM_BASE + 0,                    /*!< bmx mcu bus_err_int Interrupt                  */
    MJDEC_IRQn = IRQ_NUM_BASE + 1,                              /*!< MJDEC Interrupt                                */
    DISPLAY_IRQn = IRQ_NUM_BASE + 2,                            /*!< Display Interrupt                              */
    USB_IRQn = IRQ_NUM_BASE + 3,                                /*!< USB Interrupt                                  */
    WIFI_TO_CPU_IRQn = IRQ_NUM_BASE + 4,                        /*!< WIFI To CPU Interrupt                          */
    RF_TOP_INT0_IRQn = IRQ_NUM_BASE + 5,                        /*!< RF_TOP_INT0 Interrupt                          */
    RF_TOP_INT1_IRQn = IRQ_NUM_BASE + 6,                        /*!< RF_TOP_INT1 Interrupt                          */
    SDIO_IRQn = IRQ_NUM_BASE + 7,                               /*!< SDIO Interrupt                                 */
    WIFI_TBTT_SLEEP_IRQn = IRQ_NUM_BASE + 8,                    /*!< WIFI TBTT Sleep Interrupt                      */
    SEC_ENG_ID1_SHA_AES_TRNG_PKA_GMAC_IRQn = IRQ_NUM_BASE + 9,  /*!< Sec_eng_id1 Interrupt                          */
    SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC_IRQn = IRQ_NUM_BASE + 10, /*!< Sec_eng_id0 Interrupt                          */
    SEC_ENG_ID1_CDET_IRQn = IRQ_NUM_BASE + 11,                  /*!< Sec_eng_id1_cdet  Interrupt                    */
    SEC_ENG_ID0_CDET_IRQn = IRQ_NUM_BASE + 12,                  /*!< Sec_eng_id0_cdet Interrupt                     */
    SF_CTRL_ID1_IRQn = IRQ_NUM_BASE + 13,                       /*!< Sf_ctrl_id1 Interrupt                          */
    SF_CTRL_ID0_IRQn = IRQ_NUM_BASE + 14,                       /*!< Sf_ctrl_id0 Interrupt                          */
    DMA0_ALL_IRQn = IRQ_NUM_BASE + 15,                          /*!< DMA0_INTR_ALL Interrupt                        */
    DVP2BUS_INT0_IRQn = IRQ_NUM_BASE + 16,                      /*!< DVP2BUS_INT0 Interrupt                         */
    SDH_IRQn = IRQ_NUM_BASE + 17,                               /*!< SDH Interrupt                                  */
    DVP2BUS_INT1_IRQn = IRQ_NUM_BASE + 18,                      /*!< DVP2BUS_INT1 Interrupt                         */
    WIFI_TBTT_WAKEUP_IRQn = IRQ_NUM_BASE + 19,                  /*!< WIFI TBTT Wakeup Interrupt                     */
    IR_IRQn = IRQ_NUM_BASE + 20,                                /*!< IR Interrupt                                   */
    DMA1_ALL_IRQn = IRQ_NUM_BASE + 21,                          /*!< DMA1_INTR_ALL Interrupt                        */
    AUADC_IRQn = IRQ_NUM_BASE + 22,                             /*!< auadc_int Interrupt                            */
    MJPEG_IRQn = IRQ_NUM_BASE + 23,                             /*!< MJPEG Interrupt                                */
    EMAC_IRQn = IRQ_NUM_BASE + 24,                              /*!< EMAC  Interrupt                                */
    GPADC_DMA_IRQn = IRQ_NUM_BASE + 25,                         /*!< GPADC_DMA Interrupt                            */
    EFUSE_IRQn = IRQ_NUM_BASE + 26,                             /*!< Efuse Interrupt                                */
    SPI0_IRQn = IRQ_NUM_BASE + 27,                              /*!< SPI0 Interrupt                                 */
    UART0_IRQn = IRQ_NUM_BASE + 28,                             /*!< UART0 Interrupt                                */
    UART1_IRQn = IRQ_NUM_BASE + 29,                             /*!< UART1 Interrupt                                */
    UART2_IRQn = IRQ_NUM_BASE + 30,                             /*!< UART2 Interrupt                                */
    GPIO_DMA_IRQn = IRQ_NUM_BASE + 31,                          /*!< GPIO DMA Interrupt                             */
    I2C0_IRQn = IRQ_NUM_BASE + 32,                              /*!< I2C0 Interrupt                                 */
    PWM_IRQn = IRQ_NUM_BASE + 33,                               /*!< PWM Interrupt                                  */
    PEC_INT0_IRQn = IRQ_NUM_BASE + 34,                          /*!< PEC_INT0 Interrupt                             */
    PEC_INT1_IRQn = IRQ_NUM_BASE + 35,                          /*!< PEC_INT1 Interrupt                             */
    TIMER0_CH0_IRQn = IRQ_NUM_BASE + 36,                        /*!< Timer0 Channel 0 Interrupt                     */
    TIMER0_CH1_IRQn = IRQ_NUM_BASE + 37,                        /*!< Timer0 Channel 1 Interrupt                     */
    TIMER0_WDT_IRQn = IRQ_NUM_BASE + 38,                        /*!< Timer0 Watch Dog Interrupt                     */
    I2C1_IRQn = IRQ_NUM_BASE + 39,                              /*!< I2C1 Interrupt                                 */
    I2S_IRQn = IRQ_NUM_BASE + 40,                               /*!< I2S Interrupt                                  */
    ANA_OCP_OUT_TO_CPU_IRQn = IRQ_NUM_BASE + 41,                /*!< ana_ocp_out_to_cpu_irq Interrupt               */
    RESERVED42_IRQn = IRQ_NUM_BASE + 42,                        /*!< Reserved Interrupt                             */
    XTAL_RDY_SCAN_IRQn = IRQ_NUM_BASE + 43,                     /*!< Xtal Ready Scan Interrupt                      */
    GPIO_INT0_IRQn = IRQ_NUM_BASE + 44,                         /*!< GPIO Interrupt                                 */
    DM_IRQn = IRQ_NUM_BASE + 45,                                /*!< DM Interrupt                                   */
    BT_IRQn = IRQ_NUM_BASE + 46,                                /*!< BT Interrupt                                   */
    M154_REQ_ACK_IRQn = IRQ_NUM_BASE + 47,                      /*!< M154 req enh ack Interrupt                     */
    M154_INT_IRQn = IRQ_NUM_BASE + 48,                          /*!< M154 Interrupt                                 */
    M154_AES_IRQn = IRQ_NUM_BASE + 49,                          /*!< m154 aes Interrupt                             */
    PDS_WAKEUP_IRQn = IRQ_NUM_BASE + 50,                        /*!< PDS Wakeup Interrupt                           */
    HBN_OUT0_IRQn = IRQ_NUM_BASE + 51,                          /*!< Hibernate out 0 Interrupt                      */
    HBN_OUT1_IRQn = IRQ_NUM_BASE + 52,                          /*!< Hibernate out 1 Interrupt                      */
    BOD_IRQn = IRQ_NUM_BASE + 53,                               /*!< BOR Interrupt                                  */
#if defined(CPU_MODEL_A0)
    WIFI_IRQn = IRQ_NUM_BASE + 54,                              /*!< WIFI To CPU Interrupt                          */
#else
    DMA2D_IRQn = IRQ_NUM_BASE + 54,  /*!< DMA2D Interrupt                          */
#endif
    BZ_PHY_INT_IRQn = IRQ_NUM_BASE + 55,                        /*!< BZ phy Interrupt                               */
    BLE_IRQn = IRQ_NUM_BASE + 56,                               /*!< BLE Interrupt                                  */
    MAC_INT_TIMER_IRQn = IRQ_NUM_BASE + 57,                     /*!< MAC Tx Rx Timer Interrupt                      */
    MAC_INT_MISC_IRQn = IRQ_NUM_BASE + 58,                      /*!< MAC Tx Rx Misc Interrupt                       */
    MAC_INT_RX_TRIGGER_IRQn = IRQ_NUM_BASE + 59,                /*!< MAC Rx Trigger Interrupt                       */
    MAC_INT_TX_TRIGGER_IRQn = IRQ_NUM_BASE + 60,                /*!< MAC tx Trigger Interrupt                       */
    MAC_INT_GEN_IRQn = IRQ_NUM_BASE + 61,                       /*!< MAC Gen Interrupt                              */
    MAC_INT_PROT_TRIGGER_IRQn = IRQ_NUM_BASE + 62,              /*!< MAC Prot Trigger Interrupt                     */
    WIFI_IPC_IRQn = IRQ_NUM_BASE + 63,                          /*!< WIFI IPC Interrupt                             */
    IPC0_AP2NP_IRQn = IRQ_NUM_BASE + 64,                        /*!< IPC0 AP To NP Interrupt                        */
    IPC0_NP2AP_IRQn = IRQ_NUM_BASE + 65,                        /*!< IPC0 NP To AP Interrupt                        */
    IPC1_AP2NP_IRQn = IRQ_NUM_BASE + 66,                        /*!< IPC1 AP To NP Interrupt                        */
    IPC1_NP2AP_IRQn = IRQ_NUM_BASE + 67,                        /*!< IPC1 NP To AP Interrupt                        */
#if defined(CPU_MODEL_A0)
    RESERVED68_IRQn = IRQ_NUM_BASE + 68,                        /*!< Reserved Interrupt                             */
    RESERVED69_IRQn = IRQ_NUM_BASE + 69,                        /*!< Reserved Interrupt                             */
    RESERVED70_IRQn = IRQ_NUM_BASE + 70,                        /*!< Reserved Interrupt                             */
#else
    CANFD_IRQn = IRQ_NUM_BASE + 68,  /*!< CANFD Interrupt                              */
    CANFD1_IRQn = IRQ_NUM_BASE + 69, /*!< CANFD1 Interrupt                             */
    CANFD2_IRQn = IRQ_NUM_BASE + 70, /*!< CANFD2 Interrupt                             */
#endif
    DMA2_INT0_IRQn = IRQ_NUM_BASE + 71,                         /*!< DMA2_INT0 Interrupt                            */
    DMA2_INT1_IRQn = IRQ_NUM_BASE + 72,                         /*!< DMA2_INT1 Interrupt                            */
    DMA2_INT2_IRQn = IRQ_NUM_BASE + 73,                         /*!< DMA2_INT2 Interrupt                            */
    DMA2_INT3_IRQn = IRQ_NUM_BASE + 74,                         /*!< DMA2_INT3 Interrupt                            */
    RESERVED75_IRQn = IRQ_NUM_BASE + 75,                        /*!< Reserved Interrupt                             */
    RESERVED76_IRQn = IRQ_NUM_BASE + 76,                        /*!< Reserved Interrupt                             */
    RESERVED77_IRQn = IRQ_NUM_BASE + 77,                        /*!< Reserved Interrupt                             */
    RESERVED78_IRQn = IRQ_NUM_BASE + 78,                        /*!< Reserved Interrupt                             */
    PWM1_IRQn = IRQ_NUM_BASE + 79,                              /*!< PWM1 Interrupt                                 */
    I2C2_IRQn = IRQ_NUM_BASE + 80,                              /*!< I2C2 Interrupt                                 */
    RESERVED81_IRQn = IRQ_NUM_BASE + 81,                        /*!< Reserved Interrupt                             */
    TIMER2_IRQn = IRQ_NUM_BASE + 82,                            /*!< Timer1 Channel 0 Interrupt                     */
    TIMER3_IRQn = IRQ_NUM_BASE + 83,                            /*!< Timer1 Channel 1 Interrupt                     */
    WDG1_IRQn = IRQ_NUM_BASE + 84,                              /*!< Timer1 Watch Dog Interrupt                     */
    UART3_IRQn = IRQ_NUM_BASE + 85,                             /*!< UART3 Interrupt                                */
#if defined(CPU_MODEL_A0)
    SPI1_IRQn = IRQ_NUM_BASE + 86,                              /*!< SPI1 Interrupt                                 */
#else
    SPI3_IRQn = IRQ_NUM_BASE + 86,   /*!< SPI3 Interrupt                                 */
#endif
    RASTER2BLOCK_IRQn = IRQ_NUM_BASE + 87,                      /*!< Raster2block Interrupt                         */
    DTSRC_IRQn = IRQ_NUM_BASE + 88,                             /*!< DTSRC Interrupt                                */
    OSD0_IRQn = IRQ_NUM_BASE + 89,                              /*!< OSD0 Interrupt                                 */
    OSD1_IRQn = IRQ_NUM_BASE + 90,                              /*!< OSD1 Interrupt                                 */
#if !defined(CPU_MODEL_A0)
    MM_SEOF0_IRQn = IRQ_NUM_BASE + 91,                          /*!< MM SEOF0 Interrupt                             */
    MM_SEOF1_IRQn = IRQ_NUM_BASE + 92,                          /*!< MM SEOF1 Interrupt                             */
    DSI_IRQn = IRQ_NUM_BASE + 93,                               /*!< DSI Interrupt                                  */
    SPI1_IRQn = IRQ_NUM_BASE + 94,                              /*!< SPI1 Interrupt                                 */
    SPI2_IRQn = IRQ_NUM_BASE + 95,                              /*!< SPI2 Interrupt                                 */
    TOUCH_IRQn = IRQ_NUM_BASE + 96,                             /*!< Touch Interrupt                                */
#endif
    IRQn_LAST,
#endif
} IRQn_Type;

#if defined(CPU_LP)
#define BMX_MCU_BUS_ERR_IRQn                   (IRQn_LAST + 0)
#define MJDEC_IRQn                             (IRQn_LAST + 1)
#define DISPLAY_IRQn                           (IRQn_LAST + 2)
#define USB_IRQn                               (IRQn_LAST + 3)
// #define WIFI_TO_CPU_IRQn                       (IRQn_LAST + 4)
// #define RF_TOP_INT0_IRQn                       (IRQn_LAST + 5)
// #define RF_TOP_INT1_IRQn                       (IRQn_LAST + 6)
#define SDIO_IRQn                              (IRQn_LAST + 7)
// #define WIFI_TBTT_SLEEP_IRQn                   (IRQn_LAST + 8)
#define SEC_ENG_ID1_SHA_AES_TRNG_PKA_GMAC_IRQn (IRQn_LAST + 9)
#define SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC_IRQn (IRQn_LAST + 10)
#define SEC_ENG_ID1_CDET_IRQn                  (IRQn_LAST + 11)
#define SEC_ENG_ID0_CDET_IRQn                  (IRQn_LAST + 12)
#define SF_CTRL_ID1_IRQn                       (IRQn_LAST + 13)
#define SF_CTRL_ID0_IRQn                       (IRQn_LAST + 14)
#define DMA0_ALL_IRQn                          (IRQn_LAST + 15)
#define DVP2BUS_INT0_IRQn                      (IRQn_LAST + 16)
#define SDH_IRQn                               (IRQn_LAST + 17)
#define DVP2BUS_INT1_IRQn                      (IRQn_LAST + 18)
// #define WIFI_TBTT_WAKEUP_IRQn                  (IRQn_LAST + 19)
#define IR_IRQn                                (IRQn_LAST + 20)
#define DMA1_ALL_IRQn                          (IRQn_LAST + 21)
#define AUADC_IRQn                             (IRQn_LAST + 22)
#define MJPEG_IRQn                             (IRQn_LAST + 23)
#define EMAC_IRQn                              (IRQn_LAST + 24)
#define GPADC_DMA_IRQn                         (IRQn_LAST + 25)
#define EFUSE_IRQn                             (IRQn_LAST + 26)
#define SPI0_IRQn                              (IRQn_LAST + 27)
#define UART0_IRQn                             (IRQn_LAST + 28)
#define UART1_IRQn                             (IRQn_LAST + 29)
#define UART2_IRQn                             (IRQn_LAST + 30)
#define GPIO_DMA_IRQn                          (IRQn_LAST + 31)
#define I2C0_IRQn                              (IRQn_LAST + 32)
#define PWM_IRQn                               (IRQn_LAST + 33)
#define PEC_INT0_IRQn                          (IRQn_LAST + 34)
#define PEC_INT1_IRQn                          (IRQn_LAST + 34)
#define TIMER0_CH0_IRQn                        (IRQn_LAST + 36)
#define TIMER0_CH1_IRQn                        (IRQn_LAST + 37)
#define TIMER0_WDT_IRQn                        (IRQn_LAST + 38)
#define I2C1_IRQn                              (IRQn_LAST + 39)
#define I2S_IRQn                               (IRQn_LAST + 40)
#define ANA_OCP_OUT_TO_CPU_IRQn                (IRQn_LAST + 41)
#define RESERVED42_IRQn                        (IRQn_LAST + 42)
#define XTAL_RDY_SCAN_IRQn                     (IRQn_LAST + 43)
#define GPIO_INT0_IRQn                         (IRQn_LAST + 44)
#define DM_IRQn                                (IRQn_LAST + 45)
#define BT_IRQn                                (IRQn_LAST + 46)
#define M154_REQ_ACK_IRQn                      (IRQn_LAST + 47)
#define M154_INT_IRQn                          (IRQn_LAST + 48)
#define M154_AES_IRQn                          (IRQn_LAST + 49)
#define PDS_WAKEUP_IRQn                        (IRQn_LAST + 50)
#define HBN_OUT0_IRQn                          (IRQn_LAST + 51)
#define HBN_OUT1_IRQn                          (IRQn_LAST + 52)
#define BOD_IRQn                               (IRQn_LAST + 53)
#if defined(CPU_MODEL_A0)
#define WIFI_IRQn (IRQn_LAST + 54)
#else
#define DMA2D_IRQn (IRQn_LAST + 54)
#endif
#define BZ_PHY_INT_IRQn           (IRQn_LAST + 55)
#define BLE_IRQn                  (IRQn_LAST + 56)
#define MAC_INT_TIMER_IRQn        (IRQn_LAST + 57)
#define MAC_INT_MISC_IRQn         (IRQn_LAST + 58)
#define MAC_INT_RX_TRIGGER_IRQn   (IRQn_LAST + 59)
#define MAC_INT_TX_TRIGGER_IRQn   (IRQn_LAST + 60)
#define MAC_INT_GEN_IRQn          (IRQn_LAST + 61)
#define MAC_INT_PROT_TRIGGER_IRQn (IRQn_LAST + 62)
// #define WIFI_IPC_IRQn                          (IRQn_LAST + 63)
#endif

/**
 * @brief BL616D Memory Map Definitions
 */
#if defined(CPU_MODEL_A0)
#define BL616D_OCRAM_BASE           (0x21000000)
#define BL616D_OCRAM_END            (0x21000000 + 320 * 1024)
#define BL616D_OCRAM_CACHEABLE_BASE (0x61000000)
#define BL616D_OCRAM_CACHEABLE_END  (0x61000000 + 320 * 1024)
#else
#define BL616D_OCRAM_BASE           (0x61000000)
#define BL616D_OCRAM_END            (0x61000000 + 320 * 1024)
#define BL616D_OCRAM_CACHEABLE_BASE (0xA1000000)
#define BL616D_OCRAM_CACHEABLE_END  (0xA1000000 + 320 * 1024)
#endif
//#define BL616D_OCRAM_BUSREMAP_BASE            (0x23000000)
//#define BL616D_OCRAM_BUSREMAP_END             (0x23000000 + 320 * 1024)
//#define BL616D_OCRAM_BUSREMAP_CACHEABLE_BASE  (0x63000000)
//#define BL616D_OCRAM_BUSREMAP_CACHEABLE_END   (0x63000000 + 320 * 1024)

#if defined(CPU_MODEL_A0)
#define BL616D_WRAM_BASE           (0x21050000)
#define BL616D_WRAM_END            (0x21050000 + 320 * 1024)
#define BL616D_WRAM_CACHEABLE_BASE (0x61050000)
#define BL616D_WRAM_CACHEABLE_END  (0x61050000 + 320 * 1024)
#else
#define BL616D_WRAM_BASE           (0x61070000)
#define BL616D_WRAM_END            (0x61070000 + 320 * 1024)
#define BL616D_WRAM_CACHEABLE_BASE (0xA1070000)
#define BL616D_WRAM_CACHEABLE_END  (0xA1070000 + 320 * 1024)
#endif
//#define BL616D_WRAM_BUSREMAP_BASE             (0x23050000)
//#define BL616D_WRAM_BUSREMAP_END              (0x23050000 + 320 * 1024)
//#define BL616D_WRAM_BUSREMAP_CACHEABLE_BASE   (0x63050000)
//#define BL616D_WRAM_BUSREMAP_CACHEABLE_END    (0x63050000 + 320 * 1024)

#if defined(CPU_MODEL_A0)
#define BL616D_L2RAM_BASE           (0x210A0000)
#define BL616D_L2RAM_END            (0x210A0000 + 160 * 1024)
#define BL616D_L2RAM_CACHEABLE_BASE (0x610A0000)
#define BL616D_L2RAM_CACHEABLE_END  (0x610A0000 + 160 * 1024)
#else
#define BL616D_L2RAM_BASE           (0x61050000)
#define BL616D_L2RAM_END            (0x61050000 + 128 * 1024)
#define BL616D_L2RAM_CACHEABLE_BASE (0xA1050000)
#define BL616D_L2RAM_CACHEABLE_END  (0xA1050000 + 128 * 1024)
#endif
//#define BL616D_L2RAM_BUSREMAP_BASE            (0x230A0000)
//#define BL616D_L2RAM_BUSREMAP_END             (0x230A0000 + 160 * 1024)
//#define BL616D_L2RAM_BUSREMAP_CACHEABLE_BASE  (0x630A0000)
//#define BL616D_L2RAM_BUSREMAP_CACHEABLE_END   (0x630A0000 + 160 * 1024)

#if defined(CPU_MODEL_A0)
#define BL616D_XRAM_BASE           (0x210C8000)
#define BL616D_XRAM_END            (0x210C8000 + 16 * 1024)
#define BL616D_XRAM_CACHEABLE_BASE (0x610C8000)
#define BL616D_XRAM_CACHEABLE_END  (0x610C8000 + 16 * 1024)
#else
#define BL616D_XRAM_BASE           (0x610C0000)
#define BL616D_XRAM_END            (0x610C0000 + 16 * 1024)
#define BL616D_XRAM_CACHEABLE_BASE (0xA10C0000)
#define BL616D_XRAM_CACHEABLE_END  (0xA10C0000 + 16 * 1024)
#endif
//#define BL616D_XRAM_BUSREMAP_BASE             (0x230C8000)
//#define BL616D_XRAM_BUSREMAP_END              (0x230C8000 + 16 * 1024)
//#define BL616D_XRAM_BUSREMAP_CACHEABLE_BASE   (0x630C8000)
//#define BL616D_XRAM_BUSREMAP_CACHEABLE_END    (0x630C8000 + 16 * 1024)

#if defined(CPU_MODEL_A0)
#define BL616D_LPRAM_BASE (0x00000000)
#define BL616D_LPRAM_END  (0x00000000 + 64 * 1024)
#else
#define BL616D_LPRAM_BASE (0x00010000)
#define BL616D_LPRAM_END  (0x00010000 + 64 * 1024)
#endif

#if defined(CPU_MODEL_A0)
#define BL616D_FLASH_XIP_BASE        (0x80000000)
#define BL616D_FLASH_XIP_END         (0x80000000 + 64 * 1024 * 1024)
#define BL616D_FLASH_XIP_REMAP0_BASE (0xD0000000)
#define BL616D_FLASH_XIP_REMAP0_END  (0xD0000000 + 64 * 1024 * 1024)
#else
#define BL616D_FLASH_XIP_BASE        (0x30000000)
#define BL616D_FLASH_XIP_END         (0x30000000 + 64 * 1024 * 1024)
#define BL616D_FLASH_XIP_REMAP0_BASE (0xB0000000)
#define BL616D_FLASH_XIP_REMAP0_END  (0xB0000000 + 64 * 1024 * 1024)
#endif
//#define BL616D_FLASH_XIP_BUSREMAP_BASE        (0x10000000)
//#define BL616D_FLASH_XIP_BUSREMAP_END         (0x10000000 + 64 * 1024 * 1024)

#if defined(CPU_MODEL_A0)
#define BL616D_FLASH2_XIP_BASE        (0x84000000)
#define BL616D_FLASH2_XIP_END         (0x84000000 + 64 * 1024 * 1024)
#define BL616D_FLASH2_XIP_REMAP0_BASE (0xD4000000)
#define BL616D_FLASH2_XIP_REMAP0_END  (0xD4000000 + 64 * 1024 * 1024)
#else
#define BL616D_FLASH2_XIP_BASE        (0x34000000)
#define BL616D_FLASH2_XIP_END         (0x34000000 + 64 * 1024 * 1024)
#define BL616D_FLASH2_XIP_REMAP0_BASE (0xB4000000)
#define BL616D_FLASH2_XIP_REMAP0_END  (0xB4000000 + 64 * 1024 * 1024)
#endif
//#define BL616D_FLASH2_XIP_BUSREMAP_BASE       (0x14000000)
//#define BL616D_FLASH2_XIP_BUSREMAP_END        (0x14000000 + 64 * 1024 * 1024)

#if defined(CPU_MODEL_A0)
#define BL616D_PSRAM_BASE        (0x88000000)
#define BL616D_PSRAM_END         (0x88000000 + 128 * 1024 * 1024)
#define BL616D_PSRAM_REMAP0_BASE (0xD8000000)
#define BL616D_PSRAM_REMAP0_END  (0xD8000000 + 128 * 1024 * 1024)
#else
#define BL616D_PSRAM_BASE        (0x38000000)
#define BL616D_PSRAM_END         (0x38000000 + 128 * 1024 * 1024)
#define BL616D_PSRAM_REMAP0_BASE (0xB8000000)
#define BL616D_PSRAM_REMAP0_END  (0xB8000000 + 128 * 1024 * 1024)
#endif
//#define BL616D_PSRAM_BUSREMAP_BASE            (0xA8000000)
//#define BL616D_PSRAM_BUSREMAP_END             (0xA8000000 + 128 * 1024 * 1024)

#if defined(CPU_MODEL_A0)
#define BL616D_ALLRAM_BASE           (0x21000000)
#define BL616D_ALLRAM_END            (0x21000000 + 320 * 1024 + 320 * 1024 + 160 * 1024 + 16 * 1024)
#define BL616D_ALLRAM_CACHEABLE_BASE (0x61000000)
#define BL616D_ALLRAM_CACHEABLE_END  (0x61000000 + 320 * 1024 + 320 * 1024 + 160 * 1024 + 16 * 1024)
#else
#define BL616D_ALLRAM_BASE           (0x61000000)
#define BL616D_ALLRAM_END            (0x61000000 + 320 * 1024 + 128 * 1024 + 320 * 1024 + 16 * 1024)
#define BL616D_ALLRAM_CACHEABLE_BASE (0xA1000000)
#define BL616D_ALLRAM_CACHEABLE_END  (0xA1000000 + 320 * 1024 + 128 * 1024 + 320 * 1024 + 16 * 1024)
#endif
//#define BL616D_ALLRAM_BUSREMAP_BASE           (0x23000000)
//#define BL616D_ALLRAM_BUSREMAP_END            (0x23000000 + 320 * 1024 + 320 * 1024 + 160 * 1024 + 16 * 1024 )
//#define BL616D_ALLRAM_BUSREMAP_CACHEABLE_BASE (0x63000000)
//#define BL616D_ALLRAM_BUSREMAP_CACHEABLE_END  (0x63000000 + 320 * 1024 + 320 * 1024 + 160 * 1024 + 16 * 1024 )

/*@} end of group Memory_Map_Section */

/* BL616D peripherals base address */
/* WLSYS */
#define GLB_BASE     ((uint32_t)0x20000000)
#define DSI_PLL_BASE ((uint32_t)0x20000700)
#define RF_BASE      ((uint32_t)0x20001000)
#define GPIP_BASE    ((uint32_t)0x20002000)
#define SEC_DBG_BASE ((uint32_t)0x20003000)
#define RF2_BASE     ((uint32_t)0x20004000)
#if !defined(CPU_MODEL_A0)
#define DMA2D_BASE ((uint32_t)0x20005000)
#endif
#define TZ1_BASE      ((uint32_t)0x20006000)
#define TZC_SEC_BASE  ((uint32_t)0x20006000)
#define TZ2_BASE      ((uint32_t)0x20007000)
#define TZC_NSEC_BASE ((uint32_t)0x20007000)
#define CCI_BASE      ((uint32_t)0x20008000)
#define MCU_MISC_BASE ((uint32_t)0x20009000)
#define USB2_PHY_BASE ((uint32_t)0x2000A000)
#if !defined(CPU_MODEL_A0)
#define SPI_BASE ((uint32_t)0x2000B000)
#endif
#define EFUSE_BASE      ((uint32_t)0x2000C000)
#define EF_DATA_BASE    ((uint32_t)0x2000C000)
#define EF_CTRL_BASE    ((uint32_t)0x2000C000)
#define PSRAM_CTRL_BASE ((uint32_t)0x2000D000)
#if defined(CPU_MODEL_A0)
#define CANFD_BASE ((uint32_t)0x2000F000)
#else
#define CANFD_BASE  ((uint32_t)0x2000E000)
#define CANFD1_BASE ((uint32_t)0x2000E800)
#define CANFD2_BASE ((uint32_t)0x2000F000)
#endif
#define KYS_BASE   ((uint32_t)0x2000F800)
#define UART0_BASE ((uint32_t)0x20010000)
#define UART1_BASE ((uint32_t)0x20011000)
#define UART2_BASE ((uint32_t)0x20012000)
#define IPC0_BASE  ((uint32_t)0x20013000)
#define I2C0_BASE  ((uint32_t)0x20014000)
#define I2C1_BASE  ((uint32_t)0x20015000)
#define IPC1_BASE  ((uint32_t)0x20016000)
#if defined(CPU_MODEL_A0)
#define SPI_BASE ((uint32_t)0x20018000)
#else
#define AUADC_BASE ((uint32_t)0x20017000)
#define SPI1_BASE  ((uint32_t)0x20018000)
#endif
#define PWM_BASE   ((uint32_t)0x20019000)
#define TIMER_BASE ((uint32_t)0x2001A000)
#define IR_BASE    ((uint32_t)0x2001B000)
#define CKS_BASE   ((uint32_t)0x2001C000)
#define DBI_BASE   ((uint32_t)0x2001D000)
#define I2S_BASE   ((uint32_t)0x2001E000)
#define PEC_BASE   ((uint32_t)0x2001F000)
#if !defined(CPU_MODEL_A0)
#define DMA1_BASE  ((uint32_t)0x20021000)
#define AUDAC_BASE ((uint32_t)0x20022000)
#define SPI2_BASE  ((uint32_t)0x20023000)
#endif
#define SEC_ENG_BASE     ((uint32_t)0x20080000)
#define DMA_BASE         ((uint32_t)0x20081000)
#define QSPI_BASE        ((uint32_t)0x20082000)
#define SF_CTRL_BASE     ((uint32_t)0x20082000)
#define SF_CTRL_BUF_BASE ((uint32_t)0x20082600)
#define EMAC0_BASE       ((uint32_t)0x20083000)
#define SDH_BASE         ((uint32_t)0x20084000)
#define SDU_BASE         ((uint32_t)0x20085000)
#if defined(CPU_MODEL_A0)
#define DMA1_BASE ((uint32_t)0x20086000)
#endif
#define USB_BASE          ((uint32_t)0x20087000)
#define EMAC1_BASE        ((uint32_t)0x20089000)
#define GMAC0_BASE        ((uint32_t)0x2008A000)
#define GMAC1_BASE        ((uint32_t)0x2008C000)
#define PDS_BASE          ((uint32_t)0x2008E000)
#define HBN_BASE          ((uint32_t)0x2008F000)
#define AON_BASE          ((uint32_t)0x2008F000)
#define HBN_RAM_BASE      ((uint32_t)0x20090000)

/* MMSYS */
#define MM_MISC_BASE      ((uint32_t)0x20040000)
#define DVP2AXI0_BASE     ((uint32_t)0x20041000)
#define DVP2AXI1_BASE     ((uint32_t)0x20042000)
#define MJPEG_BASE        ((uint32_t)0x20043000)
#define MJDEC_BASE        ((uint32_t)0x20044000)
#define DTSRC_BASE        ((uint32_t)0x20045000)
#define RASTER2BLOCK_BASE ((uint32_t)0x20046000)
#define BLOCK2RASTER_BASE ((uint32_t)0x20047000)
#define DVP2SRAM0_BASE    ((uint32_t)0x20048000)
#define DVP2SRAM1_BASE    ((uint32_t)0x20049000)
#define OSD_BASE          ((uint32_t)0x2004A000)
#define GLB_WL_BASE       ((uint32_t)0x24000000)

/* LPSYS */
#define MINI_MISC         ((uint32_t)0x00080000)
#define PWM1_BASE         ((uint32_t)0x00081000)
#define I2C2_BASE         ((uint32_t)0x00082000)
#define TIMER1_BASE       ((uint32_t)0x00083000)
#define UART3_BASE        ((uint32_t)0x00084000)
#if defined(CPU_MODEL_A0)
#define SPI1_BASE ((uint32_t)0x00085000)
#else
#define SPI3_BASE ((uint32_t)0x00085000)
#endif
#define DMA2_BASE ((uint32_t)0x000C0000)

/**
  * @}
  */
#include <stdint.h>

/* RISCV CPU include files */
#ifdef __GNUC__
#if defined(CPU_MODEL_A0) || defined(CPU_LP)
#include <csi_core.h>
#else
#include <nmsis_core.h>
#endif
#endif

#endif
