#ifndef __BL616D_MEMORYMAP_H
#define __BL616D_MEMORYMAP_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
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
#define EMAC_V2_0_BASE    ((uint32_t)0x2008A000)
#define EMAC_V2_1_BASE    ((uint32_t)0x2008C000)
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
#define OSD0_BASE         ((uint32_t)0x2004A000)
#define OSD1_BASE         ((uint32_t)0x2004B000)

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

#endif