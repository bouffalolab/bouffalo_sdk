#ifndef __BL616L_H__
#define __BL616L_H__

/** @addtogroup Configuration_section_for_RISCV
  * @{
  */

/**
  * @brief Configuration of the Processor and Core Peripherals
   */

#define CORE_ID_ADDRESS       (0xF0000000)
#define CORE_ID_M0            (0xE9070006)

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
#define IRQ_NUM_BASE 16
#endif
#endif
/**
 * @brief BL616LL Interrupt Number Definition, according to the selected device
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
    /******  BL616LL specific Interrupt Numbers **********************************************************************/
    BMX_MCU_BUS_ERR_IRQn = IRQ_NUM_BASE + 0,                    /*!< bmx mcu bus_err_int Interrupt                                     */
    BMX_MCU_TO_IRQn = IRQ_NUM_BASE + 1,                         /*!< bmx_timeout_int|mcu_timeout_int Interrupt                         */
    DBI_IRQn = IRQ_NUM_BASE + 2,                                /*!< DBI Interrupt                                                     */
    SDU_SOFT_RST_IRQn = IRQ_NUM_BASE + 3,                       /*!< SDU software reset Interrupt                                                */
    AUDIO_IRQn = IRQ_NUM_BASE + 4,                              /*!< Audio Interrupt                                                   */
    RF_TOP_INT0_IRQn = IRQ_NUM_BASE + 5,                        /*!< RF_TOP_INT0 Interrupt                                             */
    RF_TOP_INT1_IRQn = IRQ_NUM_BASE + 6,                        /*!< RF_TOP_INT1 Interrupt                                             */
    SDIO_IRQn = IRQ_NUM_BASE + 7,                               /*!< SDIO Interrupt                                        */
    WIFI_TBTT_SLEEP_IRQn = IRQ_NUM_BASE + 8,                    /*!< gauge_itf_int Interrupt                                           */
    SEC_ENG_ID1_SHA_AES_TRNG_PKA_GMAC_IRQn = IRQ_NUM_BASE + 9,  /*!< sec_eng_id1 Interrupt                                             */
    SEC_ENG_ID0_SHA_AES_TRNG_PKA_GMAC_IRQn = IRQ_NUM_BASE + 10, /*!< sec_eng_id0 Interrupt                                             */
    SEC_ENG_ID1_CDET_IRQn = IRQ_NUM_BASE + 11,                  /*!< sec_eng_id1_cdet  Interrupt                                       */
    SEC_ENG_ID0_CDET_IRQn = IRQ_NUM_BASE + 12,                  /*!< sec_eng_id0_cdet Interrupt                                        */
    SF_CTRL_ID1_IRQn = IRQ_NUM_BASE + 13,                       /*!< sf_ctrl_id1 Interrupt                                             */
    SF_CTRL_ID0_IRQn = IRQ_NUM_BASE + 14,                       /*!< sf_ctrl_id0 Interrupt                                             */
    DMA0_ALL_IRQn = IRQ_NUM_BASE + 15,                          /*!< DMA0_INTR_ALL Interrupt                                           */
    DVP2BUS_INT0_IRQn = IRQ_NUM_BASE + 16,                      /*!< DVP2BUS_INTR_ALL Interrupt                                           */
    SDH_IRQn = IRQ_NUM_BASE + 17,                               /*!< sdh Interrupt                                                     */
    DVP2BUS_INT1_IRQn = IRQ_NUM_BASE + 18,                      /*!< MM System All Interrupt                                           */
    WIFI_TBTT_WAKEUP_IRQn = IRQ_NUM_BASE + 19,                  /*!< IR TX Interrupt                                                   */
    IRRX_IRQn = IRQ_NUM_BASE + 20,                              /*!< IR RX Interrupt                                                   */
    USB_IRQn = IRQ_NUM_BASE + 21,                               /*!< USB  Interrupt                                                    */
    AUPDM_IRQn = IRQ_NUM_BASE + 22,                             /*!< aupdm_int Interrupt                                               */
    MJPEG_IRQn = IRQ_NUM_BASE + 23,                             /*!< reserved Interrupt                                                */
    EMAC_IRQn = IRQ_NUM_BASE + 24,                              /*!< EMAC  Interrupt                                                   */
    GPADC_DMA_IRQn = IRQ_NUM_BASE + 25,                         /*!< GPADC_DMA Interrupt                                               */
    EFUSE_IRQn = IRQ_NUM_BASE + 26,                             /*!< Efuse Interrupt                                                   */
    SPI0_IRQn = IRQ_NUM_BASE + 27,                              /*!< SPI0  Interrupt                                                   */
    UART0_IRQn = IRQ_NUM_BASE + 28,                             /*!< UART0 Interrupt                                                   */
    UART1_IRQn = IRQ_NUM_BASE + 29,                             /*!< UART1 Interrupt                                                   */
    RESERVED01_IRQn = IRQ_NUM_BASE + 30,                        /*!< RESERVED01 Interrupt                                                */
    GPIO_DMA_IRQn = IRQ_NUM_BASE + 31,                          /*!< GPIO DMA Interrupt                                                */
    I2C0_IRQn = IRQ_NUM_BASE + 32,                              /*!< I2C0  Interrupt                                                   */
    PWM_IRQn = IRQ_NUM_BASE + 33,                               /*!< PWM Interrupt                                                     */
    RESERVED0_IRQn = IRQ_NUM_BASE + 34,                         /*!< RESERVED Interrupt                                                */
    RESERVED1_IRQn = IRQ_NUM_BASE + 35,                         /*!< RESERVED Interrupt                                                */
    TIMER0_CH0_IRQn = IRQ_NUM_BASE + 36,                        /*!< Timer0 Channel 0 Interrupt                                        */
    TIMER0_CH1_IRQn = IRQ_NUM_BASE + 37,                        /*!< Timer0 Channel 1 Interrupt                                        */
    TIMER0_WDT_IRQn = IRQ_NUM_BASE + 38,                        /*!< Timer0 Watch Dog Interrupt                                        */
    I2C1_IRQn = IRQ_NUM_BASE + 39,                              /*!< I2C1  Interrupt                                                   */
    I2S_IRQn = IRQ_NUM_BASE + 40,                               /*!< I2S  Interrupt                                                    */
    ANA_OCP_OUT_TO_CPU_0_IRQn = IRQ_NUM_BASE + 41,              /*!< ana_ocp_out_to_cpu_irq0 Interrupt                                 */
    ANA_OCP_OUT_TO_CPU_1_IRQn = IRQ_NUM_BASE + 42,              /*!< ana_ocp_out_to_cpu_irq1 Interrupt                                 */
    XTAL_RDY_SCAN_IRQn = IRQ_NUM_BASE + 43,                     /*!< ana_ocp_out_to_cpu_irq2 Interrupt                                 */
    GPIO_INT0_IRQn = IRQ_NUM_BASE + 44,                         /*!< GPIO Interrupt                                                    */
    DM_IRQn = IRQ_NUM_BASE + 45,                                /*!< DM Interrupt                                                      */
    BT_IRQn = IRQ_NUM_BASE + 46,                                /*!< BT Interrupt                                                      */
    M154_REQ_ACK_IRQn = IRQ_NUM_BASE + 47,                      /*!< M154 req enh ack Interrupt                                        */
    M154_INT_IRQn = IRQ_NUM_BASE + 48,                          /*!< M154 Interrupt                                                    */
    M154_AES_IRQn = IRQ_NUM_BASE + 49,                          /*!< m154 aes Interrupt                                                */
    PDS_WAKEUP_IRQn = IRQ_NUM_BASE + 50,                        /*!< PDS Wakeup Interrupt                                              */
    HBN_OUT0_IRQn = IRQ_NUM_BASE + 51,                          /*!< Hibernate out 0 Interrupt                                         */
    HBN_OUT1_IRQn = IRQ_NUM_BASE + 52,                          /*!< Hibernate out 1 Interrupt                                         */
    BOD_IRQn = IRQ_NUM_BASE + 53,                               /*!< BOR Interrupt                                                     */
    WIFI_IRQn = IRQ_NUM_BASE + 54,                              /*!< WIFI To CPU Interrupt                                             */
    BZ_PHY_INT_IRQn = IRQ_NUM_BASE + 55,                        /*!< BZ phy Interrupt                                                  */
    BLE_IRQn = IRQ_NUM_BASE + 56,                               /*!< BLE Interrupt                                                     */
    MAC_INT_TIMER_IRQn = IRQ_NUM_BASE + 57,                     /*!< MAC Tx Rx Timer Interrupt                                         */
    MAC_INT_MISC_IRQn = IRQ_NUM_BASE + 58,                      /*!< MAC Tx Rx Misc Interrupt                                          */
    MAC_INT_RX_TRIGGER_IRQn = IRQ_NUM_BASE + 59,                /*!< MAC Rx Trigger Interrupt                                          */
    MAC_INT_TX_TRIGGER_IRQn = IRQ_NUM_BASE + 60,                /*!< MAC tx Trigger Interrupt                                          */
    MAC_INT_GEN_IRQn = IRQ_NUM_BASE + 61,                       /*!< MAC Gen Interrupt                                                 */
    MAC_INT_PROT_TRIGGER_IRQn = IRQ_NUM_BASE + 62,              /*!< MAC Prot Trigger Interrupt                                        */
    WIFI_IPC_IRQn = IRQ_NUM_BASE + 63,                          /*!< WIFI Ipc Interrupt                                                */
    EXP_PERI_IRQn = IRQ_NUM_BASE + 64,                          /*!< EXP PERI Interrupt                                                */
    LTMR_GPIO_LAT_IRQn = IRQ_NUM_BASE + 65,                     /*!< LTMR GPIO LAT POST Interrupt                                      */
    WDT1_IRQn = IRQ_NUM_BASE + 66,                              /*!< WDT1 Interrupt                                                    */

    IRQn_LAST,
} IRQn_Type;

/**
 * @brief BL616LL Memory Map Definitions
 */
#define BL616L_OCRAM_BASE                     (0x20FC0000)
#define BL616L_OCRAM_END                      (0x20FC0000 + 224 * 1024)
#define BL616L_OCRAM_CACHEABLE_BASE           (0x60FC0000)
#define BL616L_OCRAM_CACHEABLE_END            (0x60FC0000 + 224 * 1024)
//#define BL616L_OCRAM_BUSREMAP_BASE            (0x22FC0000)
//#define BL616L_OCRAM_BUSREMAP_END             (0x22FC0000 + 224 * 1024)
//#define BL616L_OCRAM_BUSREMAP_CACHEABLE_BASE  (0x62FC0000)
//#define BL616L_OCRAM_BUSREMAP_CACHEABLE_END   (0x62FC0000 + 224 * 1024)

#define BL616L_WRAM_BASE                      (0x20FF8000)
#define BL616L_WRAM_END                       (0x20FF8000 + 160 * 1024)
#define BL616L_WRAM_CACHEABLE_BASE            (0x60FF8000)
#define BL616L_WRAM_CACHEABLE_END             (0x60FF8000 + 160 * 1024)
//#define BL616L_WRAM_BUSREMAP_BASE             (0x23010000)
//#define BL616L_WRAM_BUSREMAP_END              (0x23010000 + 160 * 1024)
//#define BL616L_WRAM_BUSREMAP_CACHEABLE_BASE   (0x63010000)
//#define BL616L_WRAM_BUSREMAP_CACHEABLE_END    (0x63010000 + 160 * 1024)

#define BL616L_FLASH_XIP_BASE                 (0x80000000)
#define BL616L_FLASH_XIP_END                  (0x80000000 + 64 * 1024 * 1024)
//#define BL616L_FLASH_XIP_REMAP0_BASE          (0xD0000000)
//#define BL616L_FLASH_XIP_REMAP0_END           (0xD0000000 + 64 * 1024 * 1024)
//#define BL616L_FLASH_XIP_BUSREMAP_BASE        (0xA0000000)
//#define BL616L_FLASH_XIP_BUSREMAP_END         (0xA0000000 + 64 * 1024 * 1024)

#define BL616L_FLASH2_XIP_BASE                (0x84000000)
#define BL616L_FLASH2_XIP_END                 (0x84000000 + 64 * 1024 * 1024)
//#define BL616L_FLASH2_XIP_REMAP0_BASE         (0xD4000000)
//#define BL616L_FLASH2_XIP_REMAP0_END          (0xD4000000 + 64 * 1024 * 1024)
//#define BL616L_FLASH2_XIP_BUSREMAP_BASE       (0xA4000000)
//#define BL616L_FLASH2_XIP_BUSREMAP_END        (0xA4000000 + 64 * 1024 * 1024)

#define BL616L_PSRAM_BASE                     (0x88000000)
#define BL616L_PSRAM_END                      (0x88000000 + 128 * 1024 * 1024)
//#define BL616L_PSRAM_REMAP0_BASE              (0xD8000000)
//#define BL616L_PSRAM_REMAP0_END               (0xD8000000 + 128 * 1024 * 1024)
//#define BL616L_PSRAM_BUSREMAP_BASE            (0xA8000000)
//#define BL616L_PSRAM_BUSREMAP_END             (0xA8000000 + 128 * 1024 * 1024)

#define BL616L_ALLRAM_BASE                    (0x20FC0000)
#define BL616L_ALLRAM_END                     (0x20FC0000 + 224 * 1024 + 160 * 1024)
#define BL616L_ALLRAM_CACHEABLE_BASE          (0x60FC0000)
#define BL616L_ALLRAM_CACHEABLE_END           (0x60FC0000 + 224 * 1024 + 160 * 1024)
//#define BL616L_ALLRAM_BUSREMAP_BASE           (0x22FC0000)
//#define BL616L_ALLRAM_BUSREMAP_END            (0x22FC0000 + 224 * 1024 + 160 * 1024)
//#define BL616L_ALLRAM_BUSREMAP_CACHEABLE_BASE (0x62FC0000)
//#define BL616L_ALLRAM_BUSREMAP_CACHEABLE_END  (0x62FC0000 + 224 * 1024 + 160 * 1024)

/*@} end of group Memory_Map_Section */


/* BL616LL peripherals base address */
/* WLSYS */
#define GLB_BASE                             ((uint32_t)0x20000000)
#define MIX_BASE                             ((uint32_t)0x20001000)
#define GPIP_BASE                            ((uint32_t)0x20002000)
#define PHY_BASE                             ((uint32_t)0x20002800)
#define AGC_BASE                             ((uint32_t)0x20002c00)
#define SEC_DBG_BASE                         ((uint32_t)0x20003000)
#define SEC_ENG_BASE                         ((uint32_t)0x20004000)
#define TZ1_BASE                             ((uint32_t)0x20005000)
#define TZC_SEC_BASE                         ((uint32_t)0x20005000)
#define TZ2_BASE                             ((uint32_t)0x20006000)
#define TZC_NSEC_BASE                        ((uint32_t)0x20006000)
#define CCI_BASE                             ((uint32_t)0x20008000)
#define MCU_MISC_BASE                        ((uint32_t)0x20009000)
#define UART0_BASE                           ((uint32_t)0x2000a000)
#define UART1_BASE                           ((uint32_t)0x2000a100)
#define SPI0_BASE                            ((uint32_t)0x2000a200)
#define I2C0_BASE                            ((uint32_t)0x2000a300)
#define PWM_BASE                             ((uint32_t)0x2000a400)
#define TIMER_BASE                           ((uint32_t)0x2000a500)
#define IR_BASE                              ((uint32_t)0x2000a600)
#define CKS_BASE                             ((uint32_t)0x2000a700)
#define DBI_BASE                             ((uint32_t)0x2000a800)
#define I2C1_BASE                            ((uint32_t)0x2000a900)
#define I2S_BASE                             ((uint32_t)0x2000ab00)
#define AUADC_BASE                           ((uint32_t)0x2000ac00)
#define UART2_BASE                           ((uint32_t)0x2000ad00)
#define USPI1_BASE                           ((uint32_t)0x2000ae00)
#define SF_CTRL_BASE                         ((uint32_t)0x2000b000)
#define SF_CTRL_BUF_BASE                     ((uint32_t)0x2000b600)
#define DMA_BASE                             ((uint32_t)0x2000c000)
#define SDU_BASE                             ((uint32_t)0x2000d000)
#define PDS_BASE                             ((uint32_t)0x2000e000)
#define HBN_BASE                             ((uint32_t)0x2000f000)
#define AON_BASE                             ((uint32_t)0x2000f000)
#define MM_MISC_BASE                         ((uint32_t)0x20050000)
#define PSRAM_CTRL_BASE                      ((uint32_t)0x20052000)
#define AUDAC_BASE                           ((uint32_t)0x20055000)
#define EFUSE_BASE                           ((uint32_t)0x20056000)
#define EF_DATA_BASE                         ((uint32_t)0x20056000)
#define EF_CTRL_BASE                         ((uint32_t)0x20056000)
#define DVP2AXI0_BASE                        ((uint32_t)0x20057000)
#define DVP2AXI1_BASE                        ((uint32_t)0x20058000)
#define MJPEG_BASE                           ((uint32_t)0x20059000)
#define PEC_BASE                             ((uint32_t)0x2005A000)
#define SDH_BASE                             ((uint32_t)0x20060000)
#define EMAC_BASE                            ((uint32_t)0x20070000)
#define USB_BASE                             ((uint32_t)0x20072000)
#define HBN_RAM_BASE                         ((uint32_t)0x20010000)

/**
  * @}
  */
#include <stdint.h>

/* RISCV CPU include files */
#ifdef __GNUC__
#include <csi_core.h>
#endif

#endif
