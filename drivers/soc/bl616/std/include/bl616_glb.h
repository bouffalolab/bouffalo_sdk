/**
  ******************************************************************************
  * @file    bl616_glb.h
  * @version V1.0
  * @date
  * @brief   This file is the standard driver header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#ifndef __BL616_GLB_H__
#define __BL616_GLB_H__

#include "glb_reg.h"
#include "cci_reg.h"
#include "mm_glb_reg.h"
#include "pds_reg.h"
#include "bl616_gpio.h"
#include "bl616_hbn.h"
#include "bl616_aon.h"
#include "bl616_pds.h"
#include "bl616_common.h"
#include "bflb_sf_ctrl.h"
#include "bflb_sf_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup  BL616_Peripheral_Driver
 *  @{
 */

/** @addtogroup  GLB
 *  @{
 */

/** @defgroup  GLB_Public_Types
 *  @{
 */

/** @defgroup GLB_CORE_ID definition
  * @{
  */
#define GLB_CORE_ID_M0                                   (0) /*!< M0 */
#define GLB_CORE_ID_MAX                                  (1) /*!< ID max */
#define GLB_CORE_ID_INVALID                              (2) /*!< ID invalid */
/**
  * @}
  */

/** @defgroup CCI_ID definition
  * @{
  */
#define CCI0_ID                                          (0) /*!< CCI0 port define */
#define CCI_ID_MAX                                       (1) /*!< CCI MAX ID define */
/**
  * @}
  */

/** @defgroup GLB MCU all interrupt type definition
  * @{
  */
#define GLB_MCU_ALL_INT_BMX_ERR_INT                      (0)  /*!< bmx_err_intmcu_dec_err_int */
#define GLB_MCU_ALL_INT_BMX_TIMEOUT_INT_MCU_TIMEOUT_INT  (1)  /*!< bmx_timeout_intmcu_timeout_int */
#define GLB_MCU_ALL_INT_IPC0_NP2AP_IRQ                   (2)  /*!< ipc0_np2ap_irq */
#define GLB_MCU_ALL_INT_IPC0_AP2NP_IRQ                   (3)  /*!< ipc0_ap2np_irq */
#define GLB_MCU_ALL_INT_AUDIO_INT                        (4)  /*!< audio_int */
#define GLB_MCU_ALL_INT_RF_TOP_INT_0                     (5)  /*!< rf_top_int[0] */
#define GLB_MCU_ALL_INT_RF_TOP_INT_1                     (6)  /*!< rf_top_int[1] */
#define GLB_MCU_ALL_INT_LZ4_INT                          (7)  /*!< lz4_int */
#define GLB_MCU_ALL_INT_RSV8                             (8)  /*!< rsv8 */
#define GLB_MCU_ALL_INT_SEC_ENG_ID0_GMAC_INT             (9)  /*!< sec_eng_id0_gmac_int */
#define GLB_MCU_ALL_INT_SEC_ENG_ID0_CDET_INT             (10) /*!< sec_eng_id0_cdet_int */
#define GLB_MCU_ALL_INT_SEC_ENG_ID0_PKA_INT              (11) /*!< sec_eng_id0_pka_int */
#define GLB_MCU_ALL_INT_SEC_ENG_ID0_TRNG_INT             (12) /*!< sec_eng_id0_trng_int */
#define GLB_MCU_ALL_INT_SEC_ENG_ID0_AES_INT              (13) /*!< sec_eng_id0_aes_int */
#define GLB_MCU_ALL_INT_SEC_ENG_ID0_SHA_INT              (14) /*!< sec_eng_id0_sha_int */
#define GLB_MCU_ALL_INT_DMA_INTR_ALL                     (15) /*!< DMA_INTR_ALL */
#define GLB_MCU_ALL_INT_DMA2_INTR_ALL                    (16) /*!< DMA2_INTR_ALL */
#define GLB_MCU_ALL_INT_SDH_MMC1_INT_SDH2PMU_WAKEUP_INT1 (17) /*!< sdh_mmc1_intsdh2pmu_wakeup_int1 */
#define GLB_MCU_ALL_INT_MM_IRQ_ALL                       (18) /*!< mm_irq_all */
#define GLB_MCU_ALL_INT_IRTX_INT                         (19) /*!< irtx_int */
#define GLB_MCU_ALL_INT_IRRX_INT                         (20) /*!< irrx_int */
#define GLB_MCU_ALL_INT_USB_INT                          (21) /*!< usb_int */
#define GLB_MCU_ALL_INT_AUPDM_INT                        (22) /*!< aupdm_int */
#define GLB_MCU_ALL_INT_SF_CTRL_ID0_INT                  (23) /*!< sf_ctrl_id0_int */
#define GLB_MCU_ALL_INT_EMAC_INT                         (24) /*!< emac_int */
#define GLB_MCU_ALL_INT_GPADC_DMA_INT                    (25) /*!< gpadc_dma_int */
#define GLB_MCU_ALL_INT_EFUSE_INT                        (26) /*!< efuse_int */
#define GLB_MCU_ALL_INT_SPI_0_INT                        (27) /*!< spi_0_int */
#define GLB_MCU_ALL_INT_UART_IRQ                         (28) /*!< uart_irq */
#define GLB_MCU_ALL_INT_UART1_IRQ                        (29) /*!< uart1_irq */
#define GLB_MCU_ALL_INT_UART2_IRQ                        (30) /*!< uart2_irq */
#define GLB_MCU_ALL_INT_GPIO_DMA_INT                     (31) /*!< gpio_dma_int */
#define GLB_MCU_ALL_INT_I2C_0_INT                        (32) /*!< i2c_0_int */
#define GLB_MCU_ALL_INT_PWM_INT                          (33) /*!< pwm_int */
#define GLB_MCU_ALL_INT_IPC1_NP2AP_IRQ                   (34) /*!< ipc1_np2ap_irq */
#define GLB_MCU_ALL_INT_IPC1_AP2NP_IRQ                   (35) /*!< ipc1_ap2np_irq */
#define GLB_MCU_ALL_INT_TIMER0_2_IRQ                     (36) /*!< timer0_2_irq */
#define GLB_MCU_ALL_INT_TIMER0_3_IRQ                     (37) /*!< timer0_3_irq */
#define GLB_MCU_ALL_INT_WDT0_IRQ                         (38) /*!< wdt0_irq */
#define GLB_MCU_ALL_INT_I2C_1_INT                        (39) /*!< i2c_1_int */
#define GLB_MCU_ALL_INT_I2S0_INT                         (40) /*!< i2s0_int */
#define GLB_MCU_ALL_INT_RSV41                            (41) /*!< rsv41 */
#define GLB_MCU_ALL_INT_RSV42                            (42) /*!< rsv42 */
#define GLB_MCU_ALL_INT_ANA_OCP_OUT_TO_CPU_IRQ           (43) /*!< ana_ocp_out_to_cpu_irq[2:0] */
#define GLB_MCU_ALL_INT_GPIO_IRQ                         (44) /*!< gpio_irq */
#define GLB_MCU_ALL_INT_DM_IRQ                           (45) /*!< dm_irq */
#define GLB_MCU_ALL_INT_BT_IRQ                           (46) /*!< bt_irq */
#define GLB_MCU_ALL_INT_M154_REQ_ENH_ACK_INT             (47) /*!< m154_req_enh_ack_int */
#define GLB_MCU_ALL_INT_M154_INT                         (48) /*!< m154_int */
#define GLB_MCU_ALL_INT_M154_AES_INT                     (49) /*!< m154_aes_int */
#define GLB_MCU_ALL_INT_PDS_INT                          (50) /*!< pds_int */
#define GLB_MCU_ALL_INT_HBN_IRQ_OUT_0                    (51) /*!< hbn_irq_out[0] */
#define GLB_MCU_ALL_INT_HBN_IRQ_OUT_1                    (52) /*!< hbn_irq_out[1] */
#define GLB_MCU_ALL_INT_BOR_OUT                          (53) /*!< bor_out */
#define GLB_MCU_ALL_INT_WIFI_TO_CPU_IRQ_N                (54) /*!< wifi_to_cpu_irq_n */
#define GLB_MCU_ALL_INT_BZ_PHY_INT                       (55) /*!< bz_phy_int */
#define GLB_MCU_ALL_INT_BLE_INT                          (56) /*!< ble_int */
#define GLB_MCU_ALL_INT_MAC_INT_TX_RX_TIMER              (57) /*!< mac_int_tx_rx_timer */
#define GLB_MCU_ALL_INT_MAC_INT_TX_RX_MISC               (58) /*!< mac_int_tx_rx_misc */
#define GLB_MCU_ALL_INT_MAC_INT_RX_TRIGGER               (59) /*!< mac_int_rx_trigger */
#define GLB_MCU_ALL_INT_MAC_INT_TX_TRIGGER               (60) /*!< mac_int_tx_trigger */
#define GLB_MCU_ALL_INT_MAC_INT_GEN                      (61) /*!< mac_int_gen */
#define GLB_MCU_ALL_INT_MAC_INT_PROT_TRIGGER             (62) /*!< mac_int_prot_trigger */
#define GLB_MCU_ALL_INT_WIFI_IPC                         (63) /*!< wifi_ipc */
/**
  * @}
  */

/** @defgroup GLB mcu muxpll 160M clock type definition
  * @{
  */
#define GLB_MCU_MUXPLL_SEL_WIFIPLL_160M                  (0) /*!< mcu muxpll select wifipll 160M */
#define GLB_MCU_MUXPLL_SEL_AUPLL_DIV3                    (1) /*!< mcu muxpll select aupll div3 */
#define GLB_MCU_MUXPLL_SEL_TOP_AUPLL_DIV2                (2) /*!< mcu muxpll select top aupll div2 */
#define GLB_MCU_MUXPLL_SEL_AUPLL_DIV2P5                  (3) /*!< mcu muxpll select aupll div2p5 */
/**
  * @}
  */

/** @defgroup GLB mcu muxpll 80M clock type definition
  * @{
  */
#define GLB_MCU_MUXPLL_SEL_WIFIPLL_80M                   (0) /*!< mcu muxpll select wifipll 80M */
#define GLB_MCU_MUXPLL_SEL_TOP_AUPLL_DIV5                (1) /*!< mcu muxpll select top aupll div5 */
#define GLB_MCU_MUXPLL_SEL_TOP_AUPLL_DIV6                (2) /*!< mcu muxpll select top aupll div6 */
/**
  * @}
  */

/** @defgroup GLB isp muxpll 80M clock type definition
  * @{
  */
#define GLB_ISP_MUXPLL_SEL_WIFIPLL_80M                   (0) /*!< isp muxpll select wifipll 80M */
#define GLB_ISP_MUXPLL_SEL_AUPLL_DIV5                    (1) /*!< isp muxpll select aupll div5 */
#define GLB_ISP_MUXPLL_SEL_AUPLL_DIV6                    (2) /*!< isp muxpll select aupll div6 */
/**
  * @}
  */

/** @defgroup GLB pll clock gate type definition
  * @{
  */
#define GLB_PLL_CGEN_ISP_WIFIPLL_80M                     (2)  /*!< CGEN for WIFIPLL 80M to ISP       */
#define GLB_PLL_CGEN_ISP_AUPLL_DIV5                      (3)  /*!< CGEN for AUPLL DIV5 to ISP        */
#define GLB_PLL_CGEN_ISP_AUPLL_DIV6                      (4)  /*!< CGEN for AUPLL DIV6 to ISP        */
#define GLB_PLL_CGEN_TOP_AUPLL_DIV5                      (5)  /*!< CGEN for AUPLL DIV5 to TOP        */
#define GLB_PLL_CGEN_TOP_AUPLL_DIV6                      (6)  /*!< CGEN for AUPLL DIV6 to TOP        */
#define GLB_PLL_CGEN_PSRAMB_WIFIPLL_320M                 (7)  /*!< CGEN for WIFIPLL 320MHz to psramB */
#define GLB_PLL_CGEN_PSRAMB_AUPLL_DIV1                   (8)  /*!< CGEN for AUPLL_DIV1 to psramB     */
#define GLB_PLL_CGEN_TOP_WIFIPLL_240M                    (13) /*!< CGEN for WIFIPLL 240MHz to TOP    */
#define GLB_PLL_CGEN_TOP_WIFIPLL_320M                    (14) /*!< CGEN for WIFIPLL 320MHz to TOP    */
#define GLB_PLL_CGEN_TOP_AUPLL_DIV2                      (15) /*!< CGEN for AUPLL_DIV2 to TOP        */
#define GLB_PLL_CGEN_TOP_AUPLL_DIV1                      (16) /*!< CGEN for AUPLL_DIV1 to TOP        */
/**
  * @}
  */

/** @defgroup GLB mcu system clock type definition
  * @{
  */
#define GLB_MCU_SYS_CLK_RC32M                            (0) /*!< use RC32M as system clock frequency */
#define GLB_MCU_SYS_CLK_XTAL                             (1) /*!< use XTAL as system clock */
#define GLB_MCU_SYS_CLK_TOP_AUPLL_DIV2                   (2) /*!< use TOP_AUPLL_DIV2 output as system clock */
#define GLB_MCU_SYS_CLK_TOP_AUPLL_DIV1                   (3) /*!< use TOP_AUPLL_DIV1 output as system clock */
#define GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M                 (4) /*!< use TOP_WIFIPLL_240M output as system clock */
#define GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M                 (5) /*!< use TOP_WIFIPLL_320M output as system clock */
/**
  * @}
  */

/** @defgroup GLB PLL reference clock type definition
  * @{
  */
#define GLB_PLL_REFCLK_XTAL                              (0) /*!< use XTAL as pll ref clock */
#define GLB_PLL_REFCLK_RC32M                             (3) /*!< use RC32M as pll ref clock */
/**
  * @}
  */

typedef struct {
    uint8_t clkpllRefdivRatio;  /*!< xxx pll_refdiv_ratio */
    uint8_t clkpllIntFracSw;    /*!< xxx pll_int_frac_sw */
    uint8_t clkpllIcp1u;        /*!< xxx pll_icp_1u */
    uint8_t clkpllIcp5u;        /*!< xxx pll_icp_5u */
    uint8_t clkpllRz;           /*!< xxx pll_rz */
    uint8_t clkpllCz;           /*!< xxx pll_cz */
    uint8_t clkpllC3;           /*!< xxx pll_c3 */
    uint8_t clkpllR4Short;      /*!< xxx pll_r4_short */
    uint8_t clkpllC4En;         /*!< xxx pll_r4_en */
    uint8_t clkpllSelSampleClk; /*!< xxx pll_sel_sample_clk */
    uint8_t clkpllVcoSpeed;     /*!< xxx pll_vco_speed */
    uint8_t clkpllSdmCtrlHw;    /*!< xxx pll_sdm_ctrl_hw */
    uint8_t clkpllSdmBypass;    /*!< xxx pll_sdm_bypass */
} GLB_WA_PLL_CFG_BASIC_Type;

typedef struct {
    const GLB_WA_PLL_CFG_BASIC_Type *const basicCfg; /*!< pll basic configuration */
    uint32_t clkpllSdmin;                            /*!< pll sdmin value */
    uint8_t clkpllPostDiv;                           /*!< aupll aupll_postdiv value */
} GLB_WA_PLL_Cfg_Type;

/** @defgroup GLB CAM clock type definition
  * @{
  */
#define GLB_CAM_CLK_XCLK                    (0) /*!< Select XCLK as CAM clock */
#define GLB_CAM_CLK_WIFIPLL_96M             (1) /*!< Select WIFIPLL_96M as CAM clock */
#define GLB_CAM_CLK_TOP_AUPLL_DIV5          (2) /*!< Select TOP_AUPLL_DIV5 as CAM clock */
/**
  * @}
  */

/** @defgroup GLB SDH clock type definition
  * @{
  */
#define GLB_SDH_CLK_WIFIPLL_96M             (0) /*!< Select WIFIPLL_96M as SDH clock */
#define GLB_SDH_CLK_TOP_AUPLL_DIV5          (1) /*!< Select TOP_AUPLL_DIV5 as SDH clock */
/**
  * @}
  */

/** @defgroup GLB UART sig swap group type definition
  * @{
  */
#define GLB_UART_SIG_SWAP_GRP_GPIO0_GPIO11  (0) /*!< SWAP UART SIG GROUP GPIO0  -  GPIO11 */
#define GLB_UART_SIG_SWAP_GRP_GPIO12_GPIO23 (1) /*!< SWAP UART SIG GROUP GPIO12 -  GPIO23 */
#define GLB_UART_SIG_SWAP_GRP_GPIO24_GPIO35 (2) /*!< SWAP UART SIG GROUP GPIO24 -  GPIO35 */
#define GLB_UART_SIG_SWAP_GRP_GPIO36_GPIO45 (3) /*!< SWAP UART SIG GROUP GPIO36 -  GPIO45 */
/**
  * @}
  */

/** @defgroup GLB I2S output ref clock type definition
  * @{
  */
#define GLB_I2S_OUT_REF_CLK_NONE            (0) /*!< no output reference clock on I2S_0 ref_clock port */
#define GLB_I2S_OUT_REF_CLK_SRC             (1) /*!< output reference clock on I2S_0 ref_clock port */
/**
  * @}
  */

/** @defgroup GLB DMA clock ID type definition
  * @{
  */
#define GLB_DMA0_CLK_CH0                    (0) /*!< DMA0 clock ID:channel 0 */
#define GLB_DMA0_CLK_CH1                    (1) /*!< DMA0 clock ID:channel 1 */
#define GLB_DMA0_CLK_CH2                    (2) /*!< DMA0 clock ID:channel 2 */
#define GLB_DMA0_CLK_CH3                    (3) /*!< DMA0 clock ID:channel 3 */
/**
  * @}
  */

/** @defgroup GLB peripheral DMA type definition
  * @{
  */
#define GLB_PERI_DMA_UART0_RX               (0)  /*!< uart_rx  */
#define GLB_PERI_DMA_UART0_TX               (1)  /*!< uart_tx  */
#define GLB_PERI_DMA_UART1_RX               (2)  /*!< uart1_rx */
#define GLB_PERI_DMA_UART1_TX               (3)  /*!< uart1_tx */
#define GLB_PERI_DMA_UART2_RX               (4)  /*!< uart2_rx */
#define GLB_PERI_DMA_UART2_TX               (5)  /*!< uart2_tx */
#define GLB_PERI_DMA_I2C_0_RX               (6)  /*!< i2c_0_rx */
#define GLB_PERI_DMA_I2C_0_TX               (7)  /*!< i2c_0_tx */
#define GLB_PERI_DMA_IRTX_TX                (8)  /*!< irtx_tx  */
#define GLB_PERI_DMA_GPIO_TX                (9)  /*!< gpio_tx  */
#define GLB_PERI_DMA_SPI_RX                 (10) /*!< spi_rx   */
#define GLB_PERI_DMA_SPI_TX                 (11) /*!< spi_tx   */
#define GLB_PERI_DMA_AUDIO_RX               (12) /*!< audio_rx */
#define GLB_PERI_DMA_AUDIO_TX               (13) /*!< audio_tx */
#define GLB_PERI_DMA_I2C_1_RX               (14) /*!< i2c_1_rx */
#define GLB_PERI_DMA_I2C_1_TX               (15) /*!< i2c_1_tx */
#define GLB_PERI_DMA_I2S_0_RX               (16) /*!< i2s_0_rx */
#define GLB_PERI_DMA_I2S_0_TX               (17) /*!< i2s_0_tx */
#define GLB_PERI_DMA_PDM_RX                 (18) /*!< pdm_rx   */
#define GLB_PERI_DMA_PADC                   (19) /*!< padc     */
#define GLB_PERI_DMA_GAUGE                  (20) /*!< gauge    */
#define GLB_PERI_DMA_GPADC                  (22) /*!< gpadc    */
#define GLB_PERI_DMA_GPDAC_TX               (23) /*!< gpdac_tx */
/**
  * @}
  */

/** @defgroup GLB DMA CN selection type definition
  * @{
  */
#define GLB_PERI_DMA_CN_SEL_DMA0            (0) /*!< peripheral DMA channel select DMA0 */
/**
  * @}
  */

/** @defgroup GLB IR clock source type definition
  * @{
  */
#define GLB_IR_CLK_SRC_XCLK                 (0) /*!< IR clock source select XCLK */
/**
  * @}
  */

/** @defgroup GLB flash clock type definition
  * @{
  */
#define GLB_SFLASH_CLK_WIFIPLL_120M         (0) /*!< Select WIFIPLL 120M as flash clock */
#define GLB_SFLASH_CLK_XTAL                 (1) /*!< Select XTAL as flash clock */
#define GLB_SFLASH_CLK_TOP_AUPLL_DIV5       (2) /*!< Select AUPLL DIV5 as flash clock */
#define GLB_SFLASH_CLK_MUXPLL_80M           (3) /*!< Select MUXPLL 80M as flash clock */
#define GLB_SFLASH_CLK_BCLK                 (4) /*!< Select BCLK as flash clock */
#define GLB_SFLASH_CLK_WIFIPLL_96M          (5) /*!< Select WIFIPLL 96M as flash clock */
/**
  * @}
  */

/** @defgroup GLB I2C clock type definition
  * @{
  */
#define GLB_I2C_CLK_BCLK                    (0) /*!< Select bus clk as I2C clock */
#define GLB_I2C_CLK_XCLK                    (1) /*!< Select xclk as I2C clock */
/**
  * @}
  */

/** @defgroup GLB SPI clock type definition
  * @{
  */
#define GLB_SPI_CLK_MCU_MUXPLL_160M         (0) /*!< Select MCU MUXPLL 160M as SPI clock */
#define GLB_SPI_CLK_XCLK                    (1) /*!< Select xclk as SPI clock */
/**
  * @}
  */

/** @defgroup GLB PWM1 io type definition
  * @{
  */
#define GLB_PWM1_IO_SINGLE_END              (0) /*!< Select pwm1 io single end */
#define GLB_PWM1_IO_DIFF_END                (1) /*!< Select pwm1 io differential end for BLDC */
/**
  * @}
  */

/** @defgroup GLB PDM io type definition
  * @{
  */
#define GLB_PDM_IO_SEL_AUDIO_TOP            (0) /*!< Select audio_top PDM */
#define GLB_PDM_IO_SEL_AUPDM_TOP            (1) /*!< Select aupdm_top PDM */
/**
  * @}
  */

/** @defgroup GLB DBI clock type definition
  * @{
  */
#define GLB_DBI_CLK_MCU_MUXPLL_160M         (0) /*!< Select MCU MUXPLL 160M as DBI clock */
#define GLB_DBI_CLK_XCLK                    (1) /*!< Select xclk as DBI clock */
/**
  * @}
  */

/** @defgroup GLB Audio auto clock type definition
  * @{
  */
#define GLB_AUDIO_AUTO_AUPLL                (0) /*!< Select aupll as audio auto clock */
/**
  * @}
  */

/** @defgroup GLB Audio ADC clock type definition
  * @{
  */
#define GLB_AUDIO_ADC_AUPLL                 (0) /*!< Select aupll as audio adc clock */
/**
  * @}
  */

/** @defgroup GLB Audio solo clock type definition
  * @{
  */
#define GLB_AUDIO_SOLO_AUPLL                (0) /*!< Select aupll as audio solo clock */
/**
  * @}
  */

/** @defgroup GLB SPI pad action type definition
  * @{
  */
#define GLB_SPI_PAD_ACT_AS_SLAVE            (0) /*!< SPI pad act as slave */
#define GLB_SPI_PAD_ACT_AS_MASTER           (1) /*!< SPI pad act as master */
/**
  * @}
  */

/**
 *  @brief GLB reset recorder type definition
 */
typedef struct {
    uint8_t reset_recorder_ana_powb         : 1; /* reset record bit [0] */
    uint8_t reset_recorder_ext_rst_n        : 1; /* reset record bit [1] */
    uint8_t reset_recorder_pds_reset        : 1; /* reset record bit [2] */
    uint8_t reset_recorder_wdt_rst_n        : 1; /* reset record bit [3] */
    uint8_t reset_recorder_cpu_porst_n      : 1; /* reset record bit [4] */
    uint8_t reset_recorder_sys_reset_n      : 1; /* reset record bit [5] */
    uint8_t reset_recorder_cpu_sys_rstreq_n : 1; /* reset record bit [6] */
    uint8_t reset_recorder_rsvd             : 1; /* reset record bit [7] */
} GLB_RESET_RECORD_Type;

/** @defgroup GLB AHB software type definition
  * @{
  */
#define GLB_AHB_MCU_SW_RSV0         (0)
#define GLB_AHB_MCU_SW_RSV1         (1)
#define GLB_AHB_MCU_SW_WIFI         (4)
#define GLB_AHB_MCU_SW_BTDM         (8)
#define GLB_AHB_MCU_SW_ZIGBEE       (9)
#define GLB_AHB_MCU_SW_BLE2         (10)
#define GLB_AHB_MCU_SW_ZIGBEE2      (11)
#define GLB_AHB_MCU_SW_EMI_MISC     (16)
#define GLB_AHB_MCU_SW_MM_MISC      (17)
#define GLB_AHB_MCU_SW_PSRAM1_CTRL  (18)
#define GLB_AHB_MCU_SW_EXT_USB      (19)
#define GLB_AHB_MCU_SW_EXT_AUPWM    (21)
#define GLB_AHB_MCU_SW_EXT_SDH      (22)
#define GLB_AHB_MCU_SW_EXT_EMAC     (23)
#define GLB_AHB_MCU_SW_D2XA         (25)
#define GLB_AHB_MCU_SW_D2XB         (26)
#define GLB_AHB_MCU_SW_JENC         (27)
#define GLB_AHB_MCU_SW_GLB          (32)
#define GLB_AHB_MCU_SW_MIX          (33)
#define GLB_AHB_MCU_SW_GPIP         (34)
#define GLB_AHB_MCU_SW_SEC_DBG      (35)
#define GLB_AHB_MCU_SW_SEC_ENG      (36)
#define GLB_AHB_MCU_SW_TZ1          (37)
#define GLB_AHB_MCU_SW_RSV38        (38)
#define GLB_AHB_MCU_SW_EFUSE        (39)
#define GLB_AHB_MCU_SW_RSV40        (40)
#define GLB_AHB_MCU_SW_PERI         (41)
#define GLB_AHB_MCU_SW_RSV42        (42)
#define GLB_AHB_MCU_SW_SF           (43)
#define GLB_AHB_MCU_SW_DMA          (44)
#define GLB_AHB_MCU_SW_SDU          (45)
#define GLB_AHB_MCU_SW_PDS          (46)
#define GLB_AHB_MCU_SW_RSV47        (47)
#define GLB_AHB_MCU_SW_UART0        (48)
#define GLB_AHB_MCU_SW_UART1        (49)
#define GLB_AHB_MCU_SW_SPI          (50)
#define GLB_AHB_MCU_SW_I2C0         (51)
#define GLB_AHB_MCU_SW_PWM          (52)
#define GLB_AHB_MCU_SW_TIMER        (53)
#define GLB_AHB_MCU_SW_IR_REMOTE    (54)
#define GLB_AHB_MCU_SW_CHECKSUM     (55)
#define GLB_AHB_MCU_SW_DBI          (56)
#define GLB_AHB_MCU_SW_I2C1         (57)
#define GLB_AHB_MCU_SW_I2S          (59)
#define GLB_AHB_MCU_SW_AUSOLO       (60)
#define GLB_AHB_MCU_SW_RSV61        (61)
#define GLB_AHB_MCU_SW_RSV62        (62)
#define GLB_AHB_MCU_SW_RSV63        (63)
#define GLB_AHB_MCU_SW_PWRON_RST    (64)
#define GLB_AHB_MCU_SW_CPU_RESET    (65)
#define GLB_AHB_MCU_SW_SYS_RESET    (66)
#define GLB_AHB_MCU_SW_CHIP_RESET   (69)
/**
  * @}
  */

/** @defgroup GLB dis reset type definition
  * @{
  */
#define GLB_DISRST_GPIP             (2)
#define GLB_DISRST_SEC_ENG          (4)
#define GLB_DISRST_CCI              (8)
#define GLB_DISRST_SF               (11)
#define GLB_DISRST_UART0            (16)
#define GLB_DISRST_UART1            (17)
#define GLB_DISRST_SPI              (18)
#define GLB_DISRST_I2C0             (19)
#define GLB_DISRST_PWM              (20)
#define GLB_DISRST_TIMER            (21)
#define GLB_DISRST_IR_REMOTE        (22)
#define GLB_DISRST_CHECKSUM         (23)
#define GLB_DISRST_DBI              (24)
#define GLB_DISRST_I2C1             (25)
/**
  * @}
  */

/** @defgroup GLB PKA clock type definition
  * @{
  */
#define GLB_PKA_CLK_MCU_BCLK        (0) /*!< Select MCU_BCLK as PKA clock */
#define GLB_PKA_CLK_MCU_MUXPLL_160M (1) /*!< Select MCU MUXPLL 160M as PKA clock */
/**
  * @}
  */

/** @defgroup GLB MCU software system reset type definition
  * @{
  */
#define GLB_MCU_SW_SYSTEM_CTRL_MCU  (1) /*!< mcu reset */
/**
  * @}
  */

/** @defgroup BMX arb mode type definition
  * @{
  */
#define BMX_ARB_ROUND_ROBIN         (0) /*!< 0->round-robin */
#define BMX_ARB_FIX                 (1) /*!< 1->fix */
/**
  * @}
  */

/** @defgroup BMX latch type definition
  * @{
  */
#define BMX_LATCH_FIRST_ERROR       (0) /*!< 0->Latch first error */
#define BMX_LATCH_LAST_ERROR        (1) /*!< 1->Latch last error */
/**
  * @}
  */

/**
 *  @brief BMX configuration structure type definition
 */
typedef struct {
    uint8_t arbMod;    /*!< 0->fix, 2->round-robin, 3->random */
    uint8_t timeoutEn; /*!< Bus timeout enable: detect slave no reaponse in 1024 cycles */
} BMX_TO_Cfg_Type;

/** @defgroup BMX bus err type definition
  * @{
  */
#define BMX_BUS_ERR_TRUSTZONE_DECODE       (0) /*!< Bus trustzone decode error */
#define BMX_BUS_ERR_ADDR_DECODE            (1) /*!< Bus addr decode error */
/**
  * @}
  */

/** @defgroup BMX bus err interrupt type definition
  * @{
  */
#define BMX_ERR_INT_ERR                    (0) /*!< BMX bus err interrupt */
#define BMX_ERR_INT_ALL                    (1) /*!< BMX bus err interrupt max num */
/**
  * @}
  */

/** @defgroup BMX time out interrupt type definition
  * @{
  */
#define BMX_TO_INT_TIMEOUT                 (0) /*!< BMX timeout interrupt */
#define BMX_TO_INT_ALL                     (1) /*!< BMX timeout interrupt max num */
/**
  * @}
  */

/** @defgroup GLB eth ref clock out type definition
  * @{
  */
#define GLB_ETH_REF_CLK_OUT_OUTSIDE_50M    (0) /*!< select outside 50MHz RMII ref clock */
#define GLB_ETH_REF_CLK_OUT_INSIDE_50M     (1) /*!< select inside 50MHz RMII ref clock */
/**
  * @}
  */

/** @defgroup GLB EM type definition
  * @{
  */
#define GLB_WRAM160KB_EM0KB                (0) /*!< WRAM_160KB EM_0KB */
#define GLB_WRAM128KB_EM32KB               (1) /*!< WRAM_128KB EM_32KB */
#define GLB_WRAM96KB_EM64KB                (2) /*!< WRAM_96KB EM_64KB */

/**
 *  @brief GLB Package Type type definition
 */
#define GLB_PACKAGE_TYPE_QFN40             (0) /*!< Package Type is QFN40 */
#define GLB_PACKAGE_TYPE_QFN40M            (1) /*!< Package Type is QFN40M */
#define GLB_PACKAGE_TYPE_QFN56             (2) /*!< Package Type is QFN56 */


/** @defgroup GLB ADC clock type definition
  * @{
  */
#define GLB_ADC_CLK_AUPLL                  (0) /*!< use AUPLL as ADC clock */
#define GLB_ADC_CLK_XCLK                   (1) /*!< use XCLK as ADC clock */
/**
  * @}
  */

/** @defgroup GLB DAC clock type definition
  * @{
  */
#define GLB_DAC_CLK_32M                    (0) /*!< use 32M as DAC clock */
#define GLB_DAC_CLK_XCLK                   (1) /*!< use XCLK as DAC clock */
/**
  * @}
  */

/** @defgroup GLB chip clock out 0 type definition
  * @{
  */
#define GLB_CHIP_CLK_OUT_0_CAM_REF_CLK     (0) /*!< cam_ref_clk */
#define GLB_CHIP_CLK_OUT_0_I2S_REF_CLK     (1) /*!< i2s_ref_clk out */
#define GLB_CHIP_CLK_OUT_0_NONE            (2) /*!< no clock out */
#define GLB_CHIP_CLK_OUT_0_SOLO_IN_128FS   (3) /*!< clk_solo_in_128fs */
/**
  * @}
  */

/** @defgroup GLB chip clock out 1 type definition
  * @{
  */
#define GLB_CHIP_CLK_OUT_1_CAM_REF_CLK     (0) /*!< no chip clock out */
#define GLB_CHIP_CLK_OUT_1_I2S_REF_CLK     (1) /*!< i2s_ref_clk out */
#define GLB_CHIP_CLK_OUT_1_NONE            (2) /*!< no clock out */
#define GLB_CHIP_CLK_OUT_1_SOLO_IN_128FS   (3) /*!< clk_solo_in_128fs */
/**
  * @}
  */

/** @defgroup GLB chip clock out 2 type definition
  * @{
  */
#define GLB_CHIP_CLK_OUT_2_CAM_REF_CLK     (0) /*!< cam_ref_clk */
#define GLB_CHIP_CLK_OUT_2_I2S_REF_CLK     (1) /*!< i2s_ref_clk */
#define GLB_CHIP_CLK_OUT_2_ANA_XTAL_CLK    (2) /*!< ana_xtal_clk */
#define GLB_CHIP_CLK_OUT_2_WIFIPLL_32M     (3) /*!< wifipll_32m_clk */
/**
  * @}
  */

/** @defgroup GLB chip clock out 3 type definition
  * @{
  */
#define GLB_CHIP_CLK_OUT_3_CAM_REF_CLK     (0) /*!< no chip clock out */
#define GLB_CHIP_CLK_OUT_3_I2S_REF_CLK     (1) /*!< i2s_ref_clk out */
#define GLB_CHIP_CLK_OUT_3_NONE            (2) /*!< no clock out */
#define GLB_CHIP_CLK_OUT_3_WIFIPLL_48M     (3) /*!< wifipll_48m_clk */
/**
  * @}
  */

/** @defgroup GLB DIG clock source select type definition
  * @{
  */
#define GLB_DIG_CLK_WIFIPLL_32M            (0) /*!< select WIFIPLL 32M as DIG clock source */
#define GLB_DIG_CLK_XCLK                   (1) /*!< select XCLK as DIG clock source */
#define GLB_DIG_CLK_AUPLL                  (2) /*!< select AUPLL as DIG clock source */
/**
  * @}
  */

/** @defgroup GLB 512K clock out select type definition
  * @{
  */
#define GLB_512K_CLK_OUT_512K              (0) /*!< select 512K clock out */
#define GLB_512K_CLK_OUT_256K              (1) /*!< select 256K clock out */
#define GLB_512K_CLK_OUT_128K              (2) /*!< select 128K clock out */
/**
  * @}
  */

/** @defgroup GLB BT bandwidth type definition
  * @{
  */
#define GLB_BT_BANDWIDTH_1M                (0) /*!< BT bandwidth 1MHz */
#define GLB_BT_BANDWIDTH_2M                (1) /*!< BT bandwidth 2MHz */
/**
  * @}
  */

/** @defgroup GLB UART signal type definition
  * @{
  */
#define GLB_UART_SIG_0                     (0)  /*!< UART signal 0 */
#define GLB_UART_SIG_1                     (1)  /*!< UART signal 1 */
#define GLB_UART_SIG_2                     (2)  /*!< UART signal 2 */
#define GLB_UART_SIG_3                     (3)  /*!< UART signal 3 */
#define GLB_UART_SIG_4                     (4)  /*!< UART signal 4 */
#define GLB_UART_SIG_5                     (5)  /*!< UART signal 5 */
#define GLB_UART_SIG_6                     (6)  /*!< UART signal 6 */
#define GLB_UART_SIG_7                     (7)  /*!< UART signal 7 */
#define GLB_UART_SIG_8                     (8)  /*!< UART signal 8 */
#define GLB_UART_SIG_9                     (9)  /*!< UART signal 9 */
#define GLB_UART_SIG_10                    (10) /*!< UART signal 10 */
#define GLB_UART_SIG_11                    (11) /*!< UART signal 11 */
/**
  * @}
  */

/** @defgroup GLB UART signal function type definition
  * @{
  */
#define GLB_UART_SIG_FUN_UART0_RTS         (0) /*!< UART funtion: UART 0 RTS */
#define GLB_UART_SIG_FUN_UART0_CTS         (1) /*!< UART funtion: UART 0 CTS */
#define GLB_UART_SIG_FUN_UART0_TXD         (2) /*!< UART funtion: UART 0 TXD */
#define GLB_UART_SIG_FUN_UART0_RXD         (3) /*!< UART funtion: UART 0 RXD */
#define GLB_UART_SIG_FUN_UART1_RTS         (4) /*!< UART funtion: UART 1 RTS */
#define GLB_UART_SIG_FUN_UART1_CTS         (5) /*!< UART funtion: UART 1 CTS */
#define GLB_UART_SIG_FUN_UART1_TXD         (6) /*!< UART funtion: UART 1 TXD */
#define GLB_UART_SIG_FUN_UART1_RXD         (7) /*!< UART funtion: UART 1 RXD */
#define GLB_UART_SIG_FUN_RESERVED1         (8) /*!< UART funtion: UART 2 RTS */
#define GLB_UART_SIG_FUN_RESERVED2         (9) /*!< UART funtion: UART 2 CTS */
/**
  * @}
  */

/** @defgroup XTAL type definition
  * @{
  */
#define GLB_XTAL_NONE                      (0) /*!< XTAL is none */
#define GLB_XTAL_24M                       (1) /*!< XTAL is 24M */
#define GLB_XTAL_32M                       (2) /*!< XTAL is 32M */
#define GLB_XTAL_38P4M                     (3) /*!< XTAL is 38.4M */
#define GLB_XTAL_40M                       (4) /*!< XTAL is 40M */
#define GLB_XTAL_26M                       (5) /*!< XTAL is 26M */
#define GLB_XTAL_RC32M                     (6) /*!< XTAL is RC32M */
#define GLB_XTAL_MAX                       (7) /*!< type max num */
/**
  * @}
  */

/** @defgroup PLL power on type definition
  * @{
  */
#define GLB_PLL_NONE                       (0) /*!< power on xtal and pll */
#define GLB_PLL_WIFIPLL                    (1) /*!< power on WIFIPLL */
#define GLB_PLL_AUPLL                      (2) /*!< power on AUPLL */
/**
  * @}
  */

/** @defgroup WA PLL XTAL type definition
  * @{
  */
#define GLB_WA_PLL_WIFIPLL                 (0) /*!< wifi pll */
#define GLB_WA_PLL_AUPLL                   (1) /*!< audio pll */
/**
  * @}
  */

/** @defgroup PSRAMB PLL type definition
  * @{
  */
#define GLB_PSRAMB_EMI_WIFIPLL_320M        (0) /*!< emi_wifipll_320m_clk */
#define GLB_PSRAMB_EMI_AUPLL_DIV1          (1) /*!< emi_aupll_div1_clk */
/**
  * @}
  */

/** @defgroup GLB UART clock type definition
  * @{
  */
#define GLB_UART_CLK_BCLK                  (0) /*!< Select bclk clock as UART clock */
#define GLB_UART_CLK_PLL_160M              (1) /*!< Select PLL 160M as UART clock */
/**
  * @}
  */

/** @defgroup GLB I2S di ref clock type definition
  * @{
  */
#define GLB_I2S_DI_SEL_I2S_DI_INPUT        (0) /*!< Select i2s_di input */
#define GLB_I2S_DI_SEL_I2S_REF_OUTPUT      (1) /*!< Select i2s_ref_clk output */
/**
  * @}
  */

/** @defgroup GLB SPI sig swap group type definition
  * @{
  */
#define GLB_SPI_SIG_SWAP_GRP_GPIO0_GPIO11  (0) /*!< SWAP SPI SIG GROUP GPIO0  -  GPIO11 */
#define GLB_SPI_SIG_SWAP_GRP_GPIO12_GPIO23 (1) /*!< SWAP SPI SIG GROUP GPIO12 -  GPIO23 */
#define GLB_SPI_SIG_SWAP_GRP_GPIO24_GPIO35 (2) /*!< SWAP SPI SIG GROUP GPIO24 -  GPIO35 */
#define GLB_SPI_SIG_SWAP_GRP_GPIO36_GPIO45 (3) /*!< SWAP SPI SIG GROUP GPIO36 -  GPIO45 */
/**
  * @}
  */

/** @defgroup GLB I2S do ref clock type definition
  * @{
  */
#define GLB_I2S_DO_SEL_I2S_DO_OUTPT        (0) /*!< Select i2s_do output */
#define GLB_I2S_DO_SEL_I2S_REF_OUTPUT      (1) /*!< Select i2s_ref_clk output */
/**
  * @}
  */

/** @defgroup GLB AHB clock IP type definition
  * @{
  */
#define GLB_AHB_CLOCK_IP_CPU               (0)
#define GLB_AHB_CLOCK_IP_RSVD              (1)
#define GLB_AHB_CLOCK_IP_SEC               (2)
#define GLB_AHB_CLOCK_IP_DMA_0             (3)
#define GLB_AHB_CLOCK_IP_DMA_1             (4)
#define GLB_AHB_CLOCK_IP_DMA_2             (5)
#define GLB_AHB_CLOCK_IP_CCI               (6)
#define GLB_AHB_CLOCK_IP_RF_TOP            (7)
#define GLB_AHB_CLOCK_IP_GPIP              (8)
#define GLB_AHB_CLOCK_IP_TZC               (9)
#define GLB_AHB_CLOCK_IP_EF_CTRL           (10)
#define GLB_AHB_CLOCK_IP_SF_CTRL           (11)
#define GLB_AHB_CLOCK_IP_EMAC              (12)
#define GLB_AHB_CLOCK_IP_UART0             (13)
#define GLB_AHB_CLOCK_IP_UART1             (14)
#define GLB_AHB_CLOCK_IP_UART2             (15)
#define GLB_AHB_CLOCK_IP_UART3             (16)
#define GLB_AHB_CLOCK_IP_UART4             (17)
#define GLB_AHB_CLOCK_IP_SPI               (18)
#define GLB_AHB_CLOCK_IP_I2C               (19)
#define GLB_AHB_CLOCK_IP_PWM               (20)
#define GLB_AHB_CLOCK_IP_TIMER             (21)
#define GLB_AHB_CLOCK_IP_IR                (22)
#define GLB_AHB_CLOCK_IP_CHECKSUM          (23)
#define GLB_AHB_CLOCK_IP_QDEC              (24)
#define GLB_AHB_CLOCK_IP_KYS               (25)
#define GLB_AHB_CLOCK_IP_I2S               (26)
#define GLB_AHB_CLOCK_IP_USB11             (27)
#define GLB_AHB_CLOCK_IP_CAM               (28)
#define GLB_AHB_CLOCK_IP_MJPEG             (29)
#define GLB_AHB_CLOCK_IP_BT_BLE_NORMAL     (30)
#define GLB_AHB_CLOCK_IP_BT_BLE_LP         (31)
#define GLB_AHB_CLOCK_IP_ZB_NORMAL         (32)
#define GLB_AHB_CLOCK_IP_ZB_LP             (33)
#define GLB_AHB_CLOCK_IP_WIFI_NORMAL       (34)
#define GLB_AHB_CLOCK_IP_WIFI_LP           (35)
#define GLB_AHB_CLOCK_IP_BT_BLE_2_NORMAL   (36)
#define GLB_AHB_CLOCK_IP_BT_BLE_2_LP       (37)
#define GLB_AHB_CLOCK_IP_EMI_MISC          (38)
#define GLB_AHB_CLOCK_IP_PSRAM0_CTRL       (39)
#define GLB_AHB_CLOCK_IP_PSRAM1_CTRL       (40)
#define GLB_AHB_CLOCK_IP_USB20_SDU         (41)
#define GLB_AHB_CLOCK_IP_MIX2              (42)
#define GLB_AHB_CLOCK_IP_AUDIO             (43)
#define GLB_AHB_CLOCK_IP_SDH               (44)
#define GLB_AHB_CLOCK_IP_ZB2_NORMAL        (45)
#define GLB_AHB_CLOCK_IP_ZB2_LP            (46)
#define GLB_AHB_CLOCK_IP_I2C1              (47)
#define GLB_AHB_CLOCK_IP_WIFI_PHY          (48)
#define GLB_AHB_CLOCK_IP_WIFI_MAC_PHY      (49)
#define GLB_AHB_CLOCK_IP_WIFI_PLATFORM     (50)
#define GLB_AHB_CLOCK_IP_LZ4               (51)
#define GLB_AHB_CLOCK_IP_AUPDM             (52)
#define GLB_AHB_CLOCK_IP_GAUGE             (53)
#define GLB_AHB_CLOCK_IP_DBI               (54)
#define GLB_AHB_CLOCK_IP_AUSOLO_TOP        (55)
#define GLB_AHB_CLOCK_IP_DMA_GPIO          (56)
#define GLB_AHB_CLOCK_IP_MM_MISC           (57)
/**
  * @}
  */

/** @defgroup GLB slave grp0 type definition
  * @{
  */
#define GLB_SLAVE_GRP_0_ADC                (0)
#define GLB_SLAVE_GRP_0_IR                 (1)
#define GLB_SLAVE_GRP_0_I2C                (2)
#define GLB_SLAVE_GRP_0_SPI                (3)
#define GLB_SLAVE_GRP_0_DBI                (4)
#define GLB_SLAVE_GRP_0_AUDIO_AUTO         (5)
#define GLB_SLAVE_GRP_0_AUDIO_ADC          (6)
#define GLB_SLAVE_GRP_0_AUDIO_SOLO         (7)
#define GLB_SLAVE_GRP_0_CAM                (8)
#define GLB_SLAVE_GRP_0_SDH                (9)
#define GLB_SLAVE_GRP_0_PSRAMB             (10)
#define GLB_SLAVE_GRP_0_MAX                (11)
/**
  * @}
  */

typedef struct {
    uint32_t clkOffSetAddr;
    uint8_t clkEnPos;
    uint8_t clkSelPos;
    uint8_t clkDivPos;
    uint8_t clkEnLen;
    uint8_t clkSelLen;
    uint8_t clkDivLen;
} GLB_SLAVE_GRP_0_TBL_Type;

typedef union {
    uint8_t adcClkSel;
    uint8_t irClkSel;
    uint8_t i2cClkSel;
    uint8_t spiClkSel;
    uint8_t dbiClkSel;
    uint8_t audioAutoClkSel;
    uint8_t audioAdcClkSel;
    uint8_t audioSoloClkSel;
    uint8_t camClkSel;
    uint8_t sdhClkSel;
    uint8_t psrambClkSel;
    uint32_t clkSel;
} GLB_SLAVE_GRP_0_CLK_SEL_Type;

/*@} end of group GLB_Public_Types */

/** @defgroup  GLB_Public_Constants
 *  @{
 */

/** @defgroup  GLB_CORE_ID_TYPE
 *  @{
 */
#define IS_GLB_CORE_ID_TYPE(type) (((type) == GLB_CORE_ID_M0) ||  \
                                   ((type) == GLB_CORE_ID_MAX) || \
                                   ((type) == GLB_CORE_ID_INVALID))

/** @defgroup  CCI_ID_TYPE
 *  @{
 */
#define IS_CCI_ID_TYPE(type) (((type) == CCI0_ID) || \
                              ((type) == CCI_ID_MAX))

/** @defgroup  GLB_MCU_ALL_INT_TYPE
 *  @{
 */
#define IS_GLB_MCU_ALL_INT_TYPE(type) (((type) == GLB_MCU_ALL_INT_BMX_ERR_INT) ||                      \
                                       ((type) == GLB_MCU_ALL_INT_BMX_TIMEOUT_INT_MCU_TIMEOUT_INT) ||  \
                                       ((type) == GLB_MCU_ALL_INT_IPC0_NP2AP_IRQ) ||                   \
                                       ((type) == GLB_MCU_ALL_INT_IPC0_AP2NP_IRQ) ||                   \
                                       ((type) == GLB_MCU_ALL_INT_AUDIO_INT) ||                        \
                                       ((type) == GLB_MCU_ALL_INT_RF_TOP_INT_0) ||                     \
                                       ((type) == GLB_MCU_ALL_INT_RF_TOP_INT_1) ||                     \
                                       ((type) == GLB_MCU_ALL_INT_LZ4_INT) ||                          \
                                       ((type) == GLB_MCU_ALL_INT_RSV8) ||                             \
                                       ((type) == GLB_MCU_ALL_INT_SEC_ENG_ID0_GMAC_INT) ||             \
                                       ((type) == GLB_MCU_ALL_INT_SEC_ENG_ID0_CDET_INT) ||             \
                                       ((type) == GLB_MCU_ALL_INT_SEC_ENG_ID0_PKA_INT) ||              \
                                       ((type) == GLB_MCU_ALL_INT_SEC_ENG_ID0_TRNG_INT) ||             \
                                       ((type) == GLB_MCU_ALL_INT_SEC_ENG_ID0_AES_INT) ||              \
                                       ((type) == GLB_MCU_ALL_INT_SEC_ENG_ID0_SHA_INT) ||              \
                                       ((type) == GLB_MCU_ALL_INT_DMA_INTR_ALL) ||                     \
                                       ((type) == GLB_MCU_ALL_INT_DMA2_INTR_ALL) ||                    \
                                       ((type) == GLB_MCU_ALL_INT_SDH_MMC1_INT_SDH2PMU_WAKEUP_INT1) || \
                                       ((type) == GLB_MCU_ALL_INT_MM_IRQ_ALL) ||                       \
                                       ((type) == GLB_MCU_ALL_INT_IRTX_INT) ||                         \
                                       ((type) == GLB_MCU_ALL_INT_IRRX_INT) ||                         \
                                       ((type) == GLB_MCU_ALL_INT_USB_INT) ||                          \
                                       ((type) == GLB_MCU_ALL_INT_AUPDM_INT) ||                        \
                                       ((type) == GLB_MCU_ALL_INT_SF_CTRL_ID0_INT) ||                  \
                                       ((type) == GLB_MCU_ALL_INT_EMAC_INT) ||                         \
                                       ((type) == GLB_MCU_ALL_INT_GPADC_DMA_INT) ||                    \
                                       ((type) == GLB_MCU_ALL_INT_EFUSE_INT) ||                        \
                                       ((type) == GLB_MCU_ALL_INT_SPI_0_INT) ||                        \
                                       ((type) == GLB_MCU_ALL_INT_UART_IRQ) ||                         \
                                       ((type) == GLB_MCU_ALL_INT_UART1_IRQ) ||                        \
                                       ((type) == GLB_MCU_ALL_INT_UART2_IRQ) ||                        \
                                       ((type) == GLB_MCU_ALL_INT_GPIO_DMA_INT) ||                     \
                                       ((type) == GLB_MCU_ALL_INT_I2C_0_INT) ||                        \
                                       ((type) == GLB_MCU_ALL_INT_PWM_INT) ||                          \
                                       ((type) == GLB_MCU_ALL_INT_IPC1_NP2AP_IRQ) ||                   \
                                       ((type) == GLB_MCU_ALL_INT_IPC1_AP2NP_IRQ) ||                   \
                                       ((type) == GLB_MCU_ALL_INT_TIMER0_2_IRQ) ||                     \
                                       ((type) == GLB_MCU_ALL_INT_TIMER0_3_IRQ) ||                     \
                                       ((type) == GLB_MCU_ALL_INT_WDT0_IRQ) ||                         \
                                       ((type) == GLB_MCU_ALL_INT_I2C_1_INT) ||                        \
                                       ((type) == GLB_MCU_ALL_INT_I2S0_INT) ||                         \
                                       ((type) == GLB_MCU_ALL_INT_RSV41) ||                            \
                                       ((type) == GLB_MCU_ALL_INT_RSV42) ||                            \
                                       ((type) == GLB_MCU_ALL_INT_ANA_OCP_OUT_TO_CPU_IRQ) ||           \
                                       ((type) == GLB_MCU_ALL_INT_GPIO_IRQ) ||                         \
                                       ((type) == GLB_MCU_ALL_INT_DM_IRQ) ||                           \
                                       ((type) == GLB_MCU_ALL_INT_BT_IRQ) ||                           \
                                       ((type) == GLB_MCU_ALL_INT_M154_REQ_ENH_ACK_INT) ||             \
                                       ((type) == GLB_MCU_ALL_INT_M154_INT) ||                         \
                                       ((type) == GLB_MCU_ALL_INT_M154_AES_INT) ||                     \
                                       ((type) == GLB_MCU_ALL_INT_PDS_INT) ||                          \
                                       ((type) == GLB_MCU_ALL_INT_HBN_IRQ_OUT_0) ||                    \
                                       ((type) == GLB_MCU_ALL_INT_HBN_IRQ_OUT_1) ||                    \
                                       ((type) == GLB_MCU_ALL_INT_BOR_OUT) ||                          \
                                       ((type) == GLB_MCU_ALL_INT_WIFI_TO_CPU_IRQ_N) ||                \
                                       ((type) == GLB_MCU_ALL_INT_BZ_PHY_INT) ||                       \
                                       ((type) == GLB_MCU_ALL_INT_BLE_INT) ||                          \
                                       ((type) == GLB_MCU_ALL_INT_MAC_INT_TX_RX_TIMER) ||              \
                                       ((type) == GLB_MCU_ALL_INT_MAC_INT_TX_RX_MISC) ||               \
                                       ((type) == GLB_MCU_ALL_INT_MAC_INT_RX_TRIGGER) ||               \
                                       ((type) == GLB_MCU_ALL_INT_MAC_INT_TX_TRIGGER) ||               \
                                       ((type) == GLB_MCU_ALL_INT_MAC_INT_GEN) ||                      \
                                       ((type) == GLB_MCU_ALL_INT_MAC_INT_PROT_TRIGGER) ||             \
                                       ((type) == GLB_MCU_ALL_INT_WIFI_IPC))

/** @defgroup  GLB_MCU_MUXPLL_160M_CLK_SEL_TYPE
 *  @{
 */
#define IS_GLB_MCU_MUXPLL_160M_CLK_SEL_TYPE(type) (((type) == GLB_MCU_MUXPLL_SEL_WIFIPLL_160M) ||   \
                                                   ((type) == GLB_MCU_MUXPLL_SEL_AUPLL_DIV3) ||     \
                                                   ((type) == GLB_MCU_MUXPLL_SEL_TOP_AUPLL_DIV2) || \
                                                   ((type) == GLB_MCU_MUXPLL_SEL_AUPLL_DIV2P5))

/** @defgroup  GLB_MCU_MUXPLL_80M_CLK_SEL_TYPE
 *  @{
 */
#define IS_GLB_MCU_MUXPLL_80M_CLK_SEL_TYPE(type) (((type) == GLB_MCU_MUXPLL_SEL_WIFIPLL_80M) ||    \
                                                  ((type) == GLB_MCU_MUXPLL_SEL_TOP_AUPLL_DIV5) || \
                                                  ((type) == GLB_MCU_MUXPLL_SEL_TOP_AUPLL_DIV6))

/** @defgroup  GLB_ISP_MUXPLL_80M_CLK_SEL_TYPE
 *  @{
 */
#define IS_GLB_ISP_MUXPLL_80M_CLK_SEL_TYPE(type) (((type) == GLB_ISP_MUXPLL_SEL_WIFIPLL_80M) || \
                                                  ((type) == GLB_ISP_MUXPLL_SEL_AUPLL_DIV5) ||  \
                                                  ((type) == GLB_ISP_MUXPLL_SEL_AUPLL_DIV6))

/** @defgroup  GLB_PLL_CGEN_TYPE
 *  @{
 */
#define IS_GLB_PLL_CGEN_TYPE(type) (((type) == GLB_PLL_CGEN_ISP_WIFIPLL_80M) ||     \
                                    ((type) == GLB_PLL_CGEN_ISP_AUPLL_DIV5) ||      \
                                    ((type) == GLB_PLL_CGEN_ISP_AUPLL_DIV6) ||      \
                                    ((type) == GLB_PLL_CGEN_TOP_AUPLL_DIV5) ||      \
                                    ((type) == GLB_PLL_CGEN_TOP_AUPLL_DIV6) ||      \
                                    ((type) == GLB_PLL_CGEN_PSRAMB_WIFIPLL_320M) || \
                                    ((type) == GLB_PLL_CGEN_PSRAMB_AUPLL_DIV1) ||   \
                                    ((type) == GLB_PLL_CGEN_TOP_WIFIPLL_240M) ||    \
                                    ((type) == GLB_PLL_CGEN_TOP_WIFIPLL_320M) ||    \
                                    ((type) == GLB_PLL_CGEN_TOP_AUPLL_DIV2) ||      \
                                    ((type) == GLB_PLL_CGEN_TOP_AUPLL_DIV1))

/** @defgroup  GLB_MCU_SYS_CLK_TYPE
 *  @{
 */
#define IS_GLB_MCU_SYS_CLK_TYPE(type) (((type) == GLB_MCU_SYS_CLK_RC32M) ||            \
                                       ((type) == GLB_MCU_SYS_CLK_XTAL) ||             \
                                       ((type) == GLB_MCU_SYS_CLK_TOP_AUPLL_DIV2) ||   \
                                       ((type) == GLB_MCU_SYS_CLK_TOP_AUPLL_DIV1) ||   \
                                       ((type) == GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M) || \
                                       ((type) == GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M))

/** @defgroup  GLB_PLL_REF_CLK_TYPE
 *  @{
 */
#define IS_GLB_PLL_REF_CLK_TYPE(type) (((type) == GLB_PLL_REFCLK_XTAL) || \
                                       ((type) == GLB_PLL_REFCLK_RC32M))

/** @defgroup  GLB_WIFIPLL_REF_CLK_Type
 *  @{
 */
#define IS_GLB_WIFIPLL_REF_CLK_TYPE(type) (((type) == GLB_WIFIPLL_REF_XTAL_SOC_CLK) || \
                                           ((type) == GLB_WIFIPLL_REF_XTAL_CLK) ||     \
                                           ((type) == GLB_WIFIPLL_REF_XTAL_LO_CLK) ||  \
                                           ((type) == GLB_WIFIPLL_REF_RC32M_CLK))

/** @defgroup  GLB_AUPLL_REF_CLK_Type
 *  @{
 */
#define IS_GLB_AUPLL_REF_CLK_TYPE(type) (((type) == GLB_AUPLL_REF_XTAL_SOC_CLK) || \
                                         ((type) == GLB_AUPLL_REF_RC32M_CLK))

/** @defgroup  GLB_CPUPLL_REF_CLK_Type
 *  @{
 */
#define IS_GLB_CPUPLL_REF_CLK_TYPE(type) (((type) == GLB_CPUPLL_REF_XTAL_SOC_CLK) || \
                                          ((type) == GLB_CPUPLL_REF_RC32M_CLK))

/** @defgroup  GLB_UHSPLL_REF_CLK_Type
 *  @{
 */
#define IS_GLB_UHSPLL_REF_CLK_TYPE(type) (((type) == GLB_UHSPLL_REF_XTAL_SOC_CLK) || \
                                          ((type) == GLB_UHSPLL_REF_RC32M_CLK))

/** @defgroup  GLB_CAM_CLK_TYPE
 *  @{
 */
#define IS_GLB_CAM_CLK_TYPE(type) (((type) == GLB_CAM_CLK_XCLK) ||        \
                                   ((type) == GLB_CAM_CLK_WIFIPLL_96M) || \
                                   ((type) == GLB_CAM_CLK_TOP_AUPLL_DIV5))

/** @defgroup  GLB_SDH_CLK_TYPE
 *  @{
 */
#define IS_GLB_SDH_CLK_TYPE(type) (((type) == GLB_SDH_CLK_WIFIPLL_96M) || \
                                   ((type) == GLB_SDH_CLK_TOP_AUPLL_DIV5))

/** @defgroup  GLB_GLB_UART_SIG_SWAP_SET_TYPE
 *  @{
 */
#define IS_GLB_UART_SIG_SWAP_GRP_TYPE(type) (((type) == GLB_UART_SIG_SWAP_GRP_GPIO0_GPIO11) ||  \
                                             ((type) == GLB_UART_SIG_SWAP_GRP_GPIO12_GPIO23) || \
                                             ((type) == GLB_UART_SIG_SWAP_GRP_GPIO24_GPIO35) || \
                                             ((type) == GLB_UART_SIG_SWAP_GRP_GPIO36_GPIO45))

/** @defgroup  GLB_I2S_OUT_REF_CLK_TYPE
 *  @{
 */
#define IS_GLB_I2S_OUT_REF_CLK_TYPE(type) (((type) == GLB_I2S_OUT_REF_CLK_NONE) || \
                                           ((type) == GLB_I2S_OUT_REF_CLK_SRC))

/** @defgroup  GLB_DMA_CLK_ID_TYPE
 *  @{
 */
#define IS_GLB_DMA_CLK_ID_TYPE(type) (((type) == GLB_DMA0_CLK_CH0) || \
                                      ((type) == GLB_DMA0_CLK_CH1) || \
                                      ((type) == GLB_DMA0_CLK_CH2) || \
                                      ((type) == GLB_DMA0_CLK_CH3))

/** @defgroup  GLB_PERI_DMA_TYPE
 *  @{
 */
#define IS_GLB_PERI_DMA_TYPE(type) (((type) == GLB_PERI_DMA_UART0_RX) || \
                                    ((type) == GLB_PERI_DMA_UART0_TX) || \
                                    ((type) == GLB_PERI_DMA_UART1_RX) || \
                                    ((type) == GLB_PERI_DMA_UART1_TX) || \
                                    ((type) == GLB_PERI_DMA_UART2_RX) || \
                                    ((type) == GLB_PERI_DMA_UART2_TX) || \
                                    ((type) == GLB_PERI_DMA_I2C_0_RX) || \
                                    ((type) == GLB_PERI_DMA_I2C_0_TX) || \
                                    ((type) == GLB_PERI_DMA_IRTX_TX) ||  \
                                    ((type) == GLB_PERI_DMA_GPIO_TX) ||  \
                                    ((type) == GLB_PERI_DMA_SPI_RX) ||   \
                                    ((type) == GLB_PERI_DMA_SPI_TX) ||   \
                                    ((type) == GLB_PERI_DMA_AUDIO_RX) || \
                                    ((type) == GLB_PERI_DMA_AUDIO_TX) || \
                                    ((type) == GLB_PERI_DMA_I2C_1_RX) || \
                                    ((type) == GLB_PERI_DMA_I2C_1_TX) || \
                                    ((type) == GLB_PERI_DMA_I2S_0_RX) || \
                                    ((type) == GLB_PERI_DMA_I2S_0_TX) || \
                                    ((type) == GLB_PERI_DMA_PDM_RX) ||   \
                                    ((type) == GLB_PERI_DMA_PADC) ||     \
                                    ((type) == GLB_PERI_DMA_GAUGE) ||    \
                                    ((type) == GLB_PERI_DMA_GPADC) ||    \
                                    ((type) == GLB_PERI_DMA_GPDAC_TX))

/** @defgroup  GLB_PERI_DMA_CN_SEL_TYPE
 *  @{
 */
#define IS_GLB_PERI_DMA_CN_SEL_TYPE(type) (((type) == GLB_PERI_DMA_CN_SEL_DMA0))

/** @defgroup  GLB_IR_CLK_SRC_TYPE
 *  @{
 */
#define IS_GLB_IR_CLK_SRC_TYPE(type)      (((type) == GLB_IR_CLK_SRC_XCLK))

/** @defgroup  GLB_SFLASH_CLK_TYPE
 *  @{
 */
#define IS_GLB_SFLASH_CLK_TYPE(type)      (((type) == GLB_SFLASH_CLK_WIFIPLL_120M) || \
                                      ((type) == GLB_SFLASH_CLK_XTAL) ||              \
                                      ((type) == GLB_SFLASH_CLK_TOP_AUPLL_DIV5) ||    \
                                      ((type) == GLB_SFLASH_CLK_MUXPLL_80M) ||        \
                                      ((type) == GLB_SFLASH_CLK_BCLK) ||              \
                                      ((type) == GLB_SFLASH_CLK_WIFIPLL_96M))

/** @defgroup  GLB_I2C_CLK_TYPE
 *  @{
 */
#define IS_GLB_I2C_CLK_TYPE(type) (((type) == GLB_I2C_CLK_BCLK) || \
                                   ((type) == GLB_I2C_CLK_XCLK))

/** @defgroup  GLB_SPI_CLK_TYPE
 *  @{
 */
#define IS_GLB_SPI_CLK_TYPE(type) (((type) == GLB_SPI_CLK_PLL160M) || \
                                   ((type) == GLB_SPI_CLK_XCLK))

/** @defgroup  GLB_PWM1_IO_SEL_TYPE
 *  @{
 */
#define IS_GLB_PWM1_IO_SEL_TYPE(type) (((type) == GLB_PWM1_IO_SINGLE_END) || \
                                       ((type) == GLB_PWM1_IO_DIFF_END))

/** @defgroup  GLB_PWM2_IO_SEL_TYPE
 *  @{
 */
#define IS_GLB_PWM2_IO_SEL_TYPE(type) (((type) == GLB_PWM2_IO_SINGLE_END) || \
                                       ((type) == GLB_PWM2_IO_SINGLE_END_BLDC))

/** @defgroup  GLB_PDM_IO_SEL_TYPE
 *  @{
 */
#define IS_GLB_PDM_IO_SEL_TYPE(type) (((type) == GLB_PDM_IO_SEL_AUDIO_TOP) || \
                                      ((type) == GLB_PDM_IO_SEL_AUPDM_TOP))

/** @defgroup  GLB_DBI_CLK_TYPE
 *  @{
 */
#define IS_GLB_DBI_CLK_TYPE(type) (((type) == GLB_DBI_CLK_PLL160M) || \
                                   ((type) == GLB_DBI_CLK_XCLK))

/** @defgroup  GLB_AUDIO_AUTO_CLK_TYPE
 *  @{
 */
#define IS_GLB_AUDIO_AUTO_CLK_TYPE(type) (((type) == GLB_AUDIO_AUTO_AUPLL))

/** @defgroup  GLB_AUDIO_ADC_CLK_TYPE
 *  @{
 */
#define IS_GLB_AUDIO_ADC_CLK_TYPE(type)  (((type) == GLB_AUDIO_ADC_AUPLL))

/** @defgroup  GLB_AUDIO_SOLO_CLK_TYPE
 *  @{
 */
#define IS_GLB_AUDIO_SOLO_CLK_TYPE(type) (((type) == GLB_AUDIO_SOLO_AUPLL))

/** @defgroup  GLB_SPI_PAD_ACT_AS_TYPE
 *  @{
 */
#define IS_GLB_SPI_PAD_ACT_AS_TYPE(type) (((type) == GLB_SPI_PAD_ACT_AS_SLAVE) || \
                                          ((type) == GLB_SPI_PAD_ACT_AS_MASTER))

/** @defgroup  GLB_AHB_SW_TYPE
 *  @{
 */
#define IS_GLB_AHB_MCU_SW_TYPE(type) (((type) == GLB_AHB_MCU_SW_RSV0) ||        \
                                      ((type) == GLB_AHB_MCU_SW_RSV1) ||        \
                                      ((type) == GLB_AHB_MCU_SW_WIFI) ||        \
                                      ((type) == GLB_AHB_MCU_SW_BTDM) ||        \
                                      ((type) == GLB_AHB_MCU_SW_ZIGBEE) ||      \
                                      ((type) == GLB_AHB_MCU_SW_BLE2) ||        \
                                      ((type) == GLB_AHB_MCU_SW_ZIGBEE2) ||     \
                                      ((type) == GLB_AHB_MCU_SW_EMI_MISC) ||    \
                                      ((type) == GLB_AHB_MCU_SW_MM_MISC) ||     \
                                      ((type) == GLB_AHB_MCU_SW_PSRAM1_CTRL) || \
                                      ((type) == GLB_AHB_MCU_SW_EXT_USB) ||     \
                                      ((type) == GLB_AHB_MCU_SW_EXT_AUPWM) ||   \
                                      ((type) == GLB_AHB_MCU_SW_EXT_SDH) ||     \
                                      ((type) == GLB_AHB_MCU_SW_EXT_EMAC) ||    \
                                      ((type) == GLB_AHB_MCU_SW_D2XA) ||        \
                                      ((type) == GLB_AHB_MCU_SW_D2XB) ||        \
                                      ((type) == GLB_AHB_MCU_SW_JENC) ||        \
                                      ((type) == GLB_AHB_MCU_SW_GLB) ||         \
                                      ((type) == GLB_AHB_MCU_SW_MIX) ||         \
                                      ((type) == GLB_AHB_MCU_SW_GPIP) ||        \
                                      ((type) == GLB_AHB_MCU_SW_SEC_DBG) ||     \
                                      ((type) == GLB_AHB_MCU_SW_SEC_ENG) ||     \
                                      ((type) == GLB_AHB_MCU_SW_TZ1) ||         \
                                      ((type) == GLB_AHB_MCU_SW_RSV38) ||       \
                                      ((type) == GLB_AHB_MCU_SW_EFUSE) ||       \
                                      ((type) == GLB_AHB_MCU_SW_RSV40) ||       \
                                      ((type) == GLB_AHB_MCU_SW_PERI) ||        \
                                      ((type) == GLB_AHB_MCU_SW_RSV42) ||       \
                                      ((type) == GLB_AHB_MCU_SW_SF) ||          \
                                      ((type) == GLB_AHB_MCU_SW_DMA) ||         \
                                      ((type) == GLB_AHB_MCU_SW_SDU) ||         \
                                      ((type) == GLB_AHB_MCU_SW_PDS) ||         \
                                      ((type) == GLB_AHB_MCU_SW_RSV47) ||       \
                                      ((type) == GLB_AHB_MCU_SW_UART0) ||       \
                                      ((type) == GLB_AHB_MCU_SW_UART1) ||       \
                                      ((type) == GLB_AHB_MCU_SW_SPI) ||         \
                                      ((type) == GLB_AHB_MCU_SW_I2C0) ||        \
                                      ((type) == GLB_AHB_MCU_SW_PWM) ||         \
                                      ((type) == GLB_AHB_MCU_SW_TIMER) ||       \
                                      ((type) == GLB_AHB_MCU_SW_IR_REMOTE) ||   \
                                      ((type) == GLB_AHB_MCU_SW_CHECKSUM) ||    \
                                      ((type) == GLB_AHB_MCU_SW_DBI) ||         \
                                      ((type) == GLB_AHB_MCU_SW_I2C1) ||        \
                                      ((type) == GLB_AHB_MCU_SW_I2S) ||         \
                                      ((type) == GLB_AHB_MCU_SW_AUSOLO) ||      \
                                      ((type) == GLB_AHB_MCU_SW_RSV61) ||       \
                                      ((type) == GLB_AHB_MCU_SW_RSV62) ||       \
                                      ((type) == GLB_AHB_MCU_SW_RSV63) ||       \
                                      ((type) == GLB_AHB_MCU_SW_PWRON_RST) ||   \
                                      ((type) == GLB_AHB_MCU_SW_CPU_RESET) ||   \
                                      ((type) == GLB_AHB_MCU_SW_SYS_RESET) ||   \
                                      ((type) == GLB_AHB_MCU_SW_CHIP_RESET))

/** @defgroup  GLB DISRST_TYPE
 *  @{
 */
#define IS_GLB_DISRST_TYPE(type) (((type) == GLB_DISRST_GPIP) ||      \
                                  ((type) == GLB_DISRST_SEC_ENG) ||   \
                                  ((type) == GLB_DISRST_CCI) ||       \
                                  ((type) == GLB_DISRST_SF) ||        \
                                  ((type) == GLB_DISRST_UART0) ||     \
                                  ((type) == GLB_DISRST_UART1) ||     \
                                  ((type) == GLB_DISRST_SPI) ||       \
                                  ((type) == GLB_DISRST_I2C0) ||      \
                                  ((type) == GLB_DISRST_PWM) ||       \
                                  ((type) == GLB_DISRST_TIMER) ||     \
                                  ((type) == GLB_DISRST_IR_REMOTE) || \
                                  ((type) == GLB_DISRST_CHECKSUM) ||  \
                                  ((type) == GLB_DISRST_DBI) ||       \
                                  ((type) == GLB_DISRST_I2C1))

/** @defgroup  GLB_PKA_CLK_TYPE
 *  @{
 */
#define IS_GLB_PKA_CLK_TYPE(type) (((type) == GLB_PKA_CLK_MCU_BCLK) || \
                                   ((type) == GLB_PKA_CLK_MCU_MUXPLL_160M))

/** @defgroup  GLB_MCU_SW_SYSTEM_TYPE
 *  @{
 */
#define IS_GLB_MCU_SW_SYSTEM_TYPE(type) (((type) == GLB_MCU_SW_SYSTEM_CTRL_MCU))

/** @defgroup  BMX_ARB_TYPE
 *  @{
 */
#define IS_BMX_ARB_TYPE(type)           (((type) == BMX_ARB_ROUND_ROBIN) || \
                               ((type) == BMX_ARB_FIX))

/** @defgroup  BMX_LATCH_TYPE
 *  @{
 */
#define IS_BMX_LATCH_TYPE(type) (((type) == BMX_LATCH_FIRST_ERROR) || \
                                 ((type) == BMX_LATCH_LAST_ERROR))

/** @defgroup  BMX_BUS_ERR_TYPE
 *  @{
 */
#define IS_BMX_BUS_ERR_TYPE(type) (((type) == BMX_BUS_ERR_TRUSTZONE_DECODE) || \
                                   ((type) == BMX_BUS_ERR_ADDR_DECODE))

/** @defgroup  BMX_ERR_INT_TYPE
 *  @{
 */
#define IS_BMX_ERR_INT_TYPE(type) (((type) == BMX_ERR_INT_ERR) || \
                                   ((type) == BMX_ERR_INT_ALL))

/** @defgroup  BMX_TO_INT_TYPE
 *  @{
 */
#define IS_BMX_TO_INT_TYPE(type) (((type) == BMX_TO_INT_TIMEOUT) || \
                                  ((type) == BMX_TO_INT_ALL))

/** @defgroup  GLB_ETH_REF_CLK_OUT_TYPE
 *  @{
 */
#define IS_GLB_ETH_REF_CLK_OUT_TYPE(type) (((type) == GLB_ETH_REF_CLK_OUT_OUTSIDE_50M) || \
                                           ((type) == GLB_ETH_REF_CLK_OUT_INSIDE_50M))

/** @defgroup  GLB_EM_TYPE
 *  @{
 */
#define IS_GLB_EM_TYPE(type) (((type) == GLB_WRAM160KB_EM0KB) ||  \
                              ((type) == GLB_WRAM128KB_EM32KB) || \
                              ((type) == GLB_WRAM96KB_EM64KB))

/** @defgroup  GLB_ADC_CLK_TYPE
 *  @{
 */
#define IS_GLB_ADC_CLK_TYPE(type) (((type) == GLB_ADC_CLK_AUPLL) || \
                                   ((type) == GLB_ADC_CLK_XCLK))

/** @defgroup  GLB_DAC_CLK_TYPE
 *  @{
 */
#define IS_GLB_DAC_CLK_TYPE(type) (((type) == GLB_DAC_CLK_32M) || \
                                   ((type) == GLB_DAC_CLK_XCLK))

/** @defgroup  GLB_CHIP_CLK_INOUT_TYPE
 *  @{
 */
#define IS_GLB_CHIP_CLK_INOUT_TYPE(type) (((type) == GLB_CHIP_CLK_INOUT_INPUT) || \
                                          ((type) == GLB_CHIP_CLK_INOUT_OUTPUT))

/** @defgroup  GLB_CHIP_CLK_OUT_0_TYPE
 *  @{
 */
#define IS_GLB_CHIP_CLK_OUT_0_TYPE(type) (((type) == GLB_CHIP_CLK_OUT_0_CAM_REF_CLK) || \
                                          ((type) == GLB_CHIP_CLK_OUT_0_I2S_REF_CLK) || \
                                          ((type) == GLB_CHIP_CLK_OUT_0_NONE) ||        \
                                          ((type) == GLB_CHIP_CLK_OUT_0_SOLO_IN_128FS))

/** @defgroup  GLB_CHIP_CLK_OUT_1_TYPE
 *  @{
 */
#define IS_GLB_CHIP_CLK_OUT_1_TYPE(type) (((type) == GLB_CHIP_CLK_OUT_1_CAM_REF_CLK) || \
                                          ((type) == GLB_CHIP_CLK_OUT_1_I2S_REF_CLK) || \
                                          ((type) == GLB_CHIP_CLK_OUT_1_NONE) ||        \
                                          ((type) == GLB_CHIP_CLK_OUT_1_SOLO_IN_128FS))

/** @defgroup  GLB_CHIP_CLK_OUT_2_TYPE
 *  @{
 */
#define IS_GLB_CHIP_CLK_OUT_2_TYPE(type) (((type) == GLB_CHIP_CLK_OUT_2_CAM_REF_CLK) ||  \
                                          ((type) == GLB_CHIP_CLK_OUT_2_I2S_REF_CLK) ||  \
                                          ((type) == GLB_CHIP_CLK_OUT_2_ANA_XTAL_CLK) || \
                                          ((type) == GLB_CHIP_CLK_OUT_2_WIFIPLL_32M))

/** @defgroup  GLB_CHIP_CLK_OUT_3_TYPE
 *  @{
 */
#define IS_GLB_CHIP_CLK_OUT_3_TYPE(type) (((type) == GLB_CHIP_CLK_OUT_3_CAM_REF_CLK) || \
                                          ((type) == GLB_CHIP_CLK_OUT_3_I2S_REF_CLK) || \
                                          ((type) == GLB_CHIP_CLK_OUT_3_NONE) ||        \
                                          ((type) == GLB_CHIP_CLK_OUT_3_WIFIPLL_48M))

/** @defgroup  GLB_DIG_CLK_TYPE
 *  @{
 */
#define IS_GLB_DIG_CLK_TYPE(type) (((type) == GLB_DIG_CLK_WIFIPLL_32M) || \
                                   ((type) == GLB_DIG_CLK_XCLK) ||        \
                                   ((type) == GLB_DIG_CLK_AUPLL))

/** @defgroup  GLB_512K_CLK_OUT_TYPE
 *  @{
 */
#define IS_GLB_512K_CLK_OUT_TYPE(type) (((type) == GLB_512K_CLK_OUT_512K) || \
                                        ((type) == GLB_512K_CLK_OUT_256K) || \
                                        ((type) == GLB_512K_CLK_OUT_128K))

/** @defgroup  GLB_BT_BANDWIDTH_TYPE
 *  @{
 */
#define IS_GLB_BT_BANDWIDTH_TYPE(type) (((type) == GLB_BT_BANDWIDTH_1M) || \
                                        ((type) == GLB_BT_BANDWIDTH_2M))

/** @defgroup  GLB_UART_SIG_TYPE
 *  @{
 */
#define IS_GLB_UART_SIG_TYPE(type) (((type) == GLB_UART_SIG_0) ||  \
                                    ((type) == GLB_UART_SIG_1) ||  \
                                    ((type) == GLB_UART_SIG_2) ||  \
                                    ((type) == GLB_UART_SIG_3) ||  \
                                    ((type) == GLB_UART_SIG_4) ||  \
                                    ((type) == GLB_UART_SIG_5) ||  \
                                    ((type) == GLB_UART_SIG_6) ||  \
                                    ((type) == GLB_UART_SIG_7) ||  \
                                    ((type) == GLB_UART_SIG_8) ||  \
                                    ((type) == GLB_UART_SIG_9) ||  \
                                    ((type) == GLB_UART_SIG_10) || \
                                    ((type) == GLB_UART_SIG_11))

/** @defgroup  GLB_UART_SIG_FUN_TYPE
 *  @{
 */
#define IS_GLB_UART_SIG_FUN_TYPE(type) (((type) == GLB_UART_SIG_FUN_UART0_RTS) || \
                                        ((type) == GLB_UART_SIG_FUN_UART0_CTS) || \
                                        ((type) == GLB_UART_SIG_FUN_UART0_TXD) || \
                                        ((type) == GLB_UART_SIG_FUN_UART0_RXD) || \
                                        ((type) == GLB_UART_SIG_FUN_UART1_RTS) || \
                                        ((type) == GLB_UART_SIG_FUN_UART1_CTS) || \
                                        ((type) == GLB_UART_SIG_FUN_UART1_TXD) || \
                                        ((type) == GLB_UART_SIG_FUN_UART1_RXD) || \
                                        ((type) == GLB_UART_SIG_FUN_RESERVED))

/** @defgroup  GLB_XTAL_TYPE
 *  @{
 */
#define IS_GLB_XTAL_TYPE(type) (((type) == GLB_XTAL_NONE) ||  \
                                ((type) == GLB_XTAL_24M) ||   \
                                ((type) == GLB_XTAL_32M) ||   \
                                ((type) == GLB_XTAL_38P4M) || \
                                ((type) == GLB_XTAL_40M) ||   \
                                ((type) == GLB_XTAL_26M) ||   \
                                ((type) == GLB_XTAL_RC32M) || \
                                ((type) == GLB_XTAL_MAX))

/** @defgroup  GLB_PLL_TYPE
 *  @{
 */
#define IS_GLB_PLL_TYPE(type) (((type) == GLB_PLL_NONE) ||    \
                               ((type) == GLB_PLL_WIFIPLL) || \
                               ((type) == GLB_PLL_AUPLL))

/** @defgroup  GLB_WA_PLL_TYPE
 *  @{
 */
#define IS_GLB_WA_PLL_TYPE(type) (((type) == GLB_WA_PLL_WIFIPLL) || \
                                  ((type) == GLB_WA_PLL_AUPLL))

/** @defgroup  GLB_PSRAMB_PLL_TYPE
 *  @{
 */
#define IS_GLB_PSRAMB_PLL_TYPE(type) (((type) == GLB_PSRAMB_EMI_WIFIPLL_320M) || \
                                      ((type) == GLB_PSRAMB_EMI_AUPLL_DIV1))

/** @defgroup  GLB_UART_CLK_TYPE
 *  @{
 */
#define IS_GLB_UART_CLK_TYPE(type) (((type) == GLB_UART_CLK_BCLK) || \
                                    ((type) == GLB_UART_CLK_PLL_160M))

/** @defgroup  GLB_I2S_DI_REF_CLK_TYPE
 *  @{
 */
#define IS_GLB_I2S_DI_REF_CLK_TYPE(type) (((type) == GLB_I2S_DI_SEL_I2S_DI_INPUT) || \
                                          ((type) == GLB_I2S_DI_SEL_I2S_REF_OUTPUT))

/** @defgroup  GLB_SPI_SIG_SWAP_GRP_TYPE
 *  @{
 */
#define IS_GLB_SPI_SIG_SWAP_GRP_TYPE(type) (((type) == GLB_SPI_SIG_SWAP_GRP_GPIO0_GPIO11) ||  \
                                            ((type) == GLB_SPI_SIG_SWAP_GRP_GPIO12_GPIO23) || \
                                            ((type) == GLB_SPI_SIG_SWAP_GRP_GPIO24_GPIO35) || \
                                            ((type) == GLB_SPI_SIG_SWAP_GRP_GPIO36_GPIO45))

/** @defgroup  GLB_I2S_DO_REF_CLK_TYPE
 *  @{
 */
#define IS_GLB_I2S_DO_REF_CLK_TYPE(type) (((type) == GLB_I2S_DO_SEL_I2S_DO_OUTPT) || \
                                          ((type) == GLB_I2S_DO_SEL_I2S_REF_OUTPUT))

#define IS_GLB_SLAVE_GRP_0_TYPE(type) (((type) == GLB_SLAVE_GRP_0_ADC) ||        \
                                       ((type) == GLB_SLAVE_GRP_0_IR) ||         \
                                       ((type) == GLB_SLAVE_GRP_0_I2C) ||        \
                                       ((type) == GLB_SLAVE_GRP_0_SPI) ||        \
                                       ((type) == GLB_SLAVE_GRP_0_DBI) ||        \
                                       ((type) == GLB_SLAVE_GRP_0_AUDIO_AUTO) || \
                                       ((type) == GLB_SLAVE_GRP_0_AUDIO_ADC) ||  \
                                       ((type) == GLB_SLAVE_GRP_0_AUDIO_SOLO) || \
                                       ((type) == GLB_SLAVE_GRP_0_CAM) ||        \
                                       ((type) == GLB_SLAVE_GRP_0_SDH) ||        \
                                       ((type) == GLB_SLAVE_GRP_0_PSRAMB) ||     \
                                       ((type) == GLB_SLAVE_GRP_0_MAX))

/*@} end of group GLB_Public_Constants */

/** @defgroup  GLB_Public_Macros
 *  @{
 */
#define UART_SIG_SWAP_NONE            (0x00) /* uart_sig[0:11] -> uart_sig[0:11] */
#define UART_SIG_SWAP_GPIO0_GPIO11    (0x01) /* GPIO0-11  uart_sig[ 0:11] -> uart_sig[ 6:11], uart_sig[ 0: 5] */
#define UART_SIG_SWAP_GPIO12_GPIO23   (0x02) /* GPIO12-23 uart_sig[12:23] -> uart_sig[18:23], uart_sig[12:17] */
#define UART_SIG_SWAP_GPIO24_GPIO35   (0x04) /* GPIO24-35 uart_sig[24:35] -> uart_sig[30:35], uart_sig[24:29] */
#define UART_SIG_SWAP_GPIO36_GPIO47   (0x08) /* GPIO36-47 uart_sig[36:47] -> uart_sig[42:47], uart_sig[36:41] */

#define JTAG_SIG_SWAP_NONE            0x00 /* GPIO0-22  E21_TMS/E21_TDI/E21_TCK/E21_TDO <- E21_TCK/E21_TDO/E21_TMS/E21_TDI */

#define GLB_AHB_CLOCK_CPU             (1ULL << GLB_AHB_CLOCK_IP_CPU)
#define GLB_AHB_CLOCK_RSVD            (1ULL << GLB_AHB_CLOCK_IP_RSVD)
#define GLB_AHB_CLOCK_SEC             (1ULL << GLB_AHB_CLOCK_IP_SEC)
#define GLB_AHB_CLOCK_DMA_0           (1ULL << GLB_AHB_CLOCK_IP_DMA_0)
#define GLB_AHB_CLOCK_DMA_1           (1ULL << GLB_AHB_CLOCK_IP_DMA_1)
#define GLB_AHB_CLOCK_DMA_2           (1ULL << GLB_AHB_CLOCK_IP_DMA_2)
#define GLB_AHB_CLOCK_CCI             (1ULL << GLB_AHB_CLOCK_IP_CCI)
#define GLB_AHB_CLOCK_RF_TOP          (1ULL << GLB_AHB_CLOCK_IP_RF_TOP)
#define GLB_AHB_CLOCK_GPIP            (1ULL << GLB_AHB_CLOCK_IP_GPIP)
#define GLB_AHB_CLOCK_TZC             (1ULL << GLB_AHB_CLOCK_IP_TZC)
#define GLB_AHB_CLOCK_EF_CTRL         (1ULL << GLB_AHB_CLOCK_IP_EF_CTRL)
#define GLB_AHB_CLOCK_SF_CTRL         (1ULL << GLB_AHB_CLOCK_IP_SF_CTRL)
#define GLB_AHB_CLOCK_EMAC            (1ULL << GLB_AHB_CLOCK_IP_EMAC)
#define GLB_AHB_CLOCK_UART0           (1ULL << GLB_AHB_CLOCK_IP_UART0)
#define GLB_AHB_CLOCK_UART1           (1ULL << GLB_AHB_CLOCK_IP_UART1)
#define GLB_AHB_CLOCK_UART2           (1ULL << GLB_AHB_CLOCK_IP_UART2)
#define GLB_AHB_CLOCK_UART3           (1ULL << GLB_AHB_CLOCK_IP_UART3)
#define GLB_AHB_CLOCK_UART4           (1ULL << GLB_AHB_CLOCK_IP_UART4)
#define GLB_AHB_CLOCK_SPI             (1ULL << GLB_AHB_CLOCK_IP_SPI)
#define GLB_AHB_CLOCK_I2C             (1ULL << GLB_AHB_CLOCK_IP_I2C)
#define GLB_AHB_CLOCK_PWM             (1ULL << GLB_AHB_CLOCK_IP_PWM)
#define GLB_AHB_CLOCK_TIMER           (1ULL << GLB_AHB_CLOCK_IP_TIMER)
#define GLB_AHB_CLOCK_IR              (1ULL << GLB_AHB_CLOCK_IP_IR)
#define GLB_AHB_CLOCK_CHECKSUM        (1ULL << GLB_AHB_CLOCK_IP_CHECKSUM)
#define GLB_AHB_CLOCK_QDEC            (1ULL << GLB_AHB_CLOCK_IP_QDEC)
#define GLB_AHB_CLOCK_KYS             (1ULL << GLB_AHB_CLOCK_IP_KYS)
#define GLB_AHB_CLOCK_I2S             (1ULL << GLB_AHB_CLOCK_IP_I2S)
#define GLB_AHB_CLOCK_USB11           (1ULL << GLB_AHB_CLOCK_IP_USB11)
#define GLB_AHB_CLOCK_CAM             (1ULL << GLB_AHB_CLOCK_IP_CAM)
#define GLB_AHB_CLOCK_MJPEG           (1ULL << GLB_AHB_CLOCK_IP_MJPEG)
#define GLB_AHB_CLOCK_BT_BLE_NORMAL   (1ULL << GLB_AHB_CLOCK_IP_BT_BLE_NORMAL)
#define GLB_AHB_CLOCK_BT_BLE_LP       (1ULL << GLB_AHB_CLOCK_IP_BT_BLE_LP)
#define GLB_AHB_CLOCK_ZB_NORMAL       (1ULL << GLB_AHB_CLOCK_IP_ZB_NORMAL)
#define GLB_AHB_CLOCK_ZB_LP           (1ULL << GLB_AHB_CLOCK_IP_ZB_LP)
#define GLB_AHB_CLOCK_WIFI_NORMAL     (1ULL << GLB_AHB_CLOCK_IP_WIFI_NORMAL)
#define GLB_AHB_CLOCK_WIFI_LP         (1ULL << GLB_AHB_CLOCK_IP_WIFI_LP)
#define GLB_AHB_CLOCK_BT_BLE_2_NORMAL (1ULL << GLB_AHB_CLOCK_IP_BT_BLE_2_NORMAL)
#define GLB_AHB_CLOCK_BT_BLE_2_LP     (1ULL << GLB_AHB_CLOCK_IP_BT_BLE_2_LP)
#define GLB_AHB_CLOCK_EMI_MISC        (1ULL << GLB_AHB_CLOCK_IP_EMI_MISC)
#define GLB_AHB_CLOCK_PSRAM0_CTRL     (1ULL << GLB_AHB_CLOCK_IP_PSRAM0_CTRL)
#define GLB_AHB_CLOCK_PSRAM1_CTRL     (1ULL << GLB_AHB_CLOCK_IP_PSRAM1_CTRL)
#define GLB_AHB_CLOCK_USB20_SDU       (1ULL << GLB_AHB_CLOCK_IP_USB20_SDU)
#define GLB_AHB_CLOCK_MIX2            (1ULL << GLB_AHB_CLOCK_IP_MIX2)
#define GLB_AHB_CLOCK_AUDIO           (1ULL << GLB_AHB_CLOCK_IP_AUDIO)
#define GLB_AHB_CLOCK_SDH             (1ULL << GLB_AHB_CLOCK_IP_SDH)
#define GLB_AHB_CLOCK_ZB_2_NORMAL     (1ULL << GLB_AHB_CLOCK_IP_ZB2_NORMAL)
#define GLB_AHB_CLOCK_ZB_2_LP         (1ULL << GLB_AHB_CLOCK_IP_ZB2_LP)
#define GLB_AHB_CLOCK_I2C1            (1ULL << GLB_AHB_CLOCK_IP_I2C1)
#define GLB_AHB_CLOCK_WIFI_PHY        (1ULL << GLB_AHB_CLOCK_IP_WIFI_PHY)
#define GLB_AHB_CLOCK_WIFI_MAC_PHY    (1ULL << GLB_AHB_CLOCK_IP_WIFI_MAC_PHY)
#define GLB_AHB_CLOCK_WIFI_PLATFORM   (1ULL << GLB_AHB_CLOCK_IP_WIFI_PLATFORM)
#define GLB_AHB_CLOCK_LZ4             (1ULL << GLB_AHB_CLOCK_IP_LZ4)
#define GLB_AHB_CLOCK_AUPDM           (1ULL << GLB_AHB_CLOCK_IP_AUPDM)
#define GLB_AHB_CLOCK_GAUGE           (1ULL << GLB_AHB_CLOCK_IP_GAUGE)
#define GLB_AHB_CLOCK_DBI             (1ULL << GLB_AHB_CLOCK_IP_DBI)
#define GLB_AHB_CLOCK_AUSOLO_TOP      (1ULL << GLB_AHB_CLOCK_IP_AUSOLO_TOP)
#define GLB_AHB_CLOCK_DMA_GPIO        (1ULL << GLB_AHB_CLOCK_IP_DMA_GPIO)
#define GLB_AHB_CLOCK_MM_MISC         (1ULL << GLB_AHB_CLOCK_IP_MM_MISC)

#define GLB_AUTO_CALC_XTAL_FLAG_MASK  0x0000ff00
#define GLB_AUTO_CALC_XTAL_FLAG_VALUE 0x5800

/*@} end of group GLB_Public_Macros */

extern const GLB_WA_PLL_Cfg_Type wifiPllCfg_960M[GLB_XTAL_MAX];
extern const GLB_WA_PLL_Cfg_Type audioPllCfg_491P52M[GLB_XTAL_MAX];
extern const GLB_WA_PLL_Cfg_Type audioPllCfg_451P58M[GLB_XTAL_MAX];
extern const GLB_WA_PLL_Cfg_Type audioPllCfg_400M[GLB_XTAL_MAX];
extern const GLB_WA_PLL_Cfg_Type audioPllCfg_384M[GLB_XTAL_MAX];
extern const GLB_SLAVE_GRP_0_TBL_Type glb_slave_grp_0_table[GLB_SLAVE_GRP_0_MAX];

/** @defgroup  GLB_Public_Functions
 *  @{
 */
/*----------*/
#ifndef BFLB_USE_HAL_DRIVER
void BMX_ERR_IRQHandler(void);
void BMX_TO_IRQHandler(void);
#endif
/*----------*/
BL_Err_Type GLB_Set_MCU_Muxpll_160M_Sel(uint8_t clkSel);
uint8_t GLB_Get_MCU_Muxpll_160M_Sel(void);
BL_Err_Type GLB_Set_MCU_Muxpll_80M_Sel(uint8_t clkSel);
uint8_t GLB_Get_MCU_Muxpll_80M_Sel(void);
BL_Err_Type GLB_Set_ISP_Muxpll_80M_Sel(uint8_t clkSel);
uint8_t GLB_Get_ISP_Muxpll_80M_Sel(void);
/*----------*/
BL_Err_Type GLB_Power_On_XTAL_And_PLL_CLK(uint8_t xtalType, uint8_t pllPuType);
BL_Err_Type GLB_Power_Off_WIFIPLL(void);
BL_Err_Type GLB_WIFIPLL_Ref_Clk_Sel(uint8_t refClk);
BL_Err_Type GLB_Power_On_WIFIPLL(const GLB_WA_PLL_Cfg_Type *const cfg, uint8_t waitStable);
BL_Err_Type GLB_Power_Off_AUPLL(void);
BL_Err_Type GLB_AUPLL_Ref_Clk_Sel(uint8_t refClk);
BL_Err_Type GLB_Power_On_AUPLL(const GLB_WA_PLL_Cfg_Type *const cfg, uint8_t waitStable);
BL_Err_Type GLB_Set_USB_CLK_From_WIFIPLL(uint8_t enable);
BL_Err_Type GLB_Set_SSC_CLK_From_WIFIPLL(uint8_t enable);
/*----------*/
BL_Err_Type GLB_Set_MCU_System_CLK_Div(uint8_t mcuClkDiv, uint8_t mcuPBclkDiv);
BL_Err_Type GLB_Get_MCU_System_CLK_Div(uint8_t *mcuClkDiv, uint8_t *mcuPBclkDiv);
BL_Err_Type GLB_Set_MCU_System_CLK(uint8_t clkFreq);
/*----------*/
uint8_t GLB_Get_Core_Type(void);
BL_Err_Type GLB_Set_CPU_Reset_Address(uint8_t coreID, uint32_t addr);
/*----------*/
BL_Err_Type GLB_Clr_EMI_Reset_Gate(void);
BL_Err_Type GLB_Set_MCU_MTimer_CLK(uint8_t enable, uint16_t div, uint8_t rst);
BL_Err_Type GLB_Set_ADC_CLK(uint8_t enable, uint8_t clkSel, uint8_t div);
BL_Err_Type GLB_Set_DMA_CLK(uint8_t enable, uint8_t clk);
BL_Err_Type GLB_Set_Peripheral_DMA_CN(uint8_t peri, uint8_t cn);
BL_Err_Type GLB_Set_IR_CLK(uint8_t enable, uint8_t clkSel, uint8_t div);
BL_Err_Type GLB_IR_RX_GPIO_Sel(uint8_t gpio);
BL_Err_Type GLB_Set_UART_CLK(uint8_t enable, uint8_t clkSel, uint8_t div);
BL_Err_Type GLB_UART_Fun_Sel(uint8_t sig, uint8_t fun);
BL_Err_Type GLB_Set_SF_CLK(uint8_t enable, uint8_t clkSel, uint8_t div);
BL_Err_Type GLB_Set_I2C_CLK(uint8_t enable, uint8_t clkSel, uint8_t div);
BL_Err_Type GLB_Set_I2S_CLK(uint8_t refClkEn, uint8_t refClkDiv, uint8_t inRef, uint8_t outRef);
BL_Err_Type GLB_Set_SPI_CLK(uint8_t enable, uint8_t clkSel, uint8_t div);
BL_Err_Type GLB_SPI_Sig_Swap_Set(uint8_t group, uint8_t swap);
BL_Err_Type GLB_Set_PWM1_IO_Sel(uint8_t ioSel);
BL_Err_Type GLB_Set_PDM_IO_Sel(uint8_t ioSel);
BL_Err_Type GLB_Set_DBI_CLK(uint8_t enable, uint8_t clkSel, uint8_t div);
/*----------*/
BL_Err_Type GLB_Set_DIG_CLK_Sel(uint8_t clkSel);
BL_Err_Type GLB_Set_DIG_512K_CLK(uint8_t enable, uint8_t compensationEn, uint8_t div);
BL_Err_Type GLB_Set_DIG_32K_CLK(uint8_t enable, uint8_t compensationEn, uint16_t div);
BL_Err_Type GLB_Platform_Wakeup_PDS_Enable(uint8_t enable);
/*----------*/
BL_Err_Type GLB_Sel_MCU_TMR_GPIO_Clock(uint8_t gpioPin);
BL_Err_Type GLB_Set_Chip_Clock_Out0_Sel(uint8_t clkOutType);
BL_Err_Type GLB_Set_Chip_Clock_Out1_Sel(uint8_t clkOutType);
BL_Err_Type GLB_Set_Chip_Clock_Out2_Sel(uint8_t clkOutType);
BL_Err_Type GLB_Set_Chip_Clock_Out3_Sel(uint8_t clkOutType);

/*----------*/
BL_Err_Type GLB_BMX_TO_Init(BMX_TO_Cfg_Type *BmxCfg);
uint8_t GLB_Get_BMX_TO_Status(void);
BL_Err_Type GLB_Clr_BMX_TO_Status(void);
/*----------*/
BL_Err_Type GLB_Set_Audio_AUTO_CLK(uint8_t divEn);
BL_Err_Type GLB_Set_Audio_ADC_CLK(uint8_t enable, uint8_t div);
BL_Err_Type GLB_Set_Audio_SOLO_CLK(uint8_t enable, uint8_t div);
/*----------*/
BL_Err_Type GLB_Invert_ETH_RX_CLK(uint8_t enable);
BL_Err_Type GLB_Invert_ETH_TX_CLK(uint8_t enable);
BL_Err_Type GLB_Invert_ETH_REF_O_CLK(uint8_t enable);
BL_Err_Type GLB_Set_ETH_REF_O_CLK_Sel(uint8_t clkSel);
/*----------*/
BL_Err_Type GLB_Set_CAM_CLK(uint8_t enable, uint8_t clkSel, uint8_t div);
BL_Err_Type GLB_Set_SDH_CLK(uint8_t enable, uint8_t clkSel, uint8_t div);
/*----------*/
BL_Err_Type GLB_Config_SDIO_Host_Reset_System(uint8_t enable);
BL_Err_Type GLB_Config_SDIO_Host_Reset_SDU(uint8_t enable);
BL_Err_Type GLB_Config_SDIO_Host_Interrupt_CPU(uint8_t enable);
/*----------*/
BL_Err_Type GLB_UART_Sig_Swap_Set(uint8_t group, uint8_t swap);
BL_Err_Type GLB_Swap_MCU_SPI_0_MOSI_With_MISO(BL_Fun_Type newState);
BL_Err_Type GLB_Set_MCU_SPI_0_ACT_MOD_Sel(uint8_t mod);
BL_Err_Type GLB_Set_SFlash_IO_PARM(uint8_t selEmbedded, uint8_t swapIo3Io0, uint8_t swapIo2Cs);
BL_Err_Type GLB_Set_SFlash2_IO_PARM(uint8_t swapIo3Io0);
/*----------*/
BL_Err_Type GLB_Clr_Reset_Reason(void);
BL_Err_Type GLB_Get_Reset_Reason(GLB_RESET_RECORD_Type *reason);
/*----------*/
BL_Err_Type GLB_AHB_MCU_Software_Reset(uint8_t swrst);
BL_Err_Type GLB_PER_Clock_Gate(uint64_t ips);
BL_Err_Type GLB_PER_Clock_UnGate(uint64_t ips);
BL_Err_Type GLB_PLL_CGEN_Clock_Gate(uint8_t clk);
BL_Err_Type GLB_PLL_CGEN_Clock_UnGate(uint8_t clk);
BL_Err_Type GLB_Set_PKA_CLK_Sel(uint8_t clkSel);
BL_Err_Type GLB_MCU_SW_System_Reset(uint8_t sysPart);
BL_Err_Type GLB_SW_System_Reset(void);
BL_Err_Type GLB_SW_CPU_Reset(void);
BL_Err_Type GLB_SW_POR_Reset(void);
BL_Err_Type GLB_Disrst_Set(uint8_t enable, uint8_t disrst);
/*----------*/
BL_Err_Type GLB_Set_Auto_Calc_Xtal_Type(uint8_t calcXtalType);
BL_Err_Type GLB_Get_Auto_Calc_Xtal_Type(uint8_t *calcXtalType);
BL_Err_Type GLB_Set_Flash_Id_Value(uint32_t idValue);
uint32_t GLB_Get_Flash_Id_Value(void);
/*----------*/
BL_Err_Type GLB_Trim_Ldo18ioVoutSel(void);
BL_Err_Type GLB_Trim_Ldo18ioBypass(void);
BL_Err_Type GLB_Trim_Ldo18ioVoutTrim(void);
void GLB_Power_Down_Ldo18ioVout(void);
/*----------*/
BL_Err_Type GLB_Set_SRAM_RET(uint32_t value);
uint32_t GLB_Get_SRAM_RET(void);
BL_Err_Type GLB_Set_SRAM_SLP(uint32_t value);
uint32_t GLB_Get_SRAM_SLP(void);
BL_Err_Type GLB_Set_SRAM_PARM(uint32_t value);
uint32_t GLB_Get_SRAM_PARM(void);
BL_Err_Type GLB_Set_EM_Sel(uint8_t emType);
BL_Err_Type GLB_Set_PSRAMB_CLK_Sel(uint8_t enable, uint8_t clkSel, uint8_t div);
/*----------*/
BL_Err_Type GLB_Set_Slave_Grp_0_CLK(uint8_t slave, uint8_t enable, GLB_SLAVE_GRP_0_CLK_SEL_Type clkSel, uint32_t div);
/*----------*/
BL_Err_Type GLB_Config_WIFI_PLL(uint8_t xtalType, const GLB_WA_PLL_Cfg_Type *pllCfgList);
BL_Err_Type GLB_Config_AUDIO_PLL(uint8_t xtalType, const GLB_WA_PLL_Cfg_Type *pllCfgList);
BL_Err_Type GLB_Config_AUDIO_PLL_To_384M(void);
BL_Err_Type GLB_Config_AUDIO_PLL_To_400M(void);

BL_Err_Type GLB_Config_AUDIO_PLL_To_451P58M(void);
BL_Err_Type GLB_Config_AUDIO_PLL_To_491P52M(void);

/*----------*/
uint8_t GLB_Get_Package_Type(void);
BL_Sts_Type GLB_Get_PAD_Bonging_to_GND_Sts(void);
/*----------*/

/*@} end of group GLB_Public_Functions */

/*@} end of group GLB */

/*@} end of group BL616_Peripheral_Driver */

#ifdef __cplusplus
}
#endif

#endif /* __BL616_GLB_H__ */
