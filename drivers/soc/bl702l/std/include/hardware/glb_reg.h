/**
  ******************************************************************************
  * @file    glb_reg.h
  * @version V1.0
  * @date    2022-07-14
  * @brief   This file is the description of.IP register
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
#ifndef  __GLB_REG_H__
#define  __GLB_REG_H__

#include "bl702l.h"

/* 0x0 : clk_cfg0 */
#define GLB_CLK_CFG0_OFFSET                                     (0x0)
#define GLB_REG_PLL_EN                                          GLB_REG_PLL_EN
#define GLB_REG_PLL_EN_POS                                      (0U)
#define GLB_REG_PLL_EN_LEN                                      (1U)
#define GLB_REG_PLL_EN_MSK                                      (((1U<<GLB_REG_PLL_EN_LEN)-1)<<GLB_REG_PLL_EN_POS)
#define GLB_REG_PLL_EN_UMSK                                     (~(((1U<<GLB_REG_PLL_EN_LEN)-1)<<GLB_REG_PLL_EN_POS))
#define GLB_REG_FCLK_EN                                         GLB_REG_FCLK_EN
#define GLB_REG_FCLK_EN_POS                                     (1U)
#define GLB_REG_FCLK_EN_LEN                                     (1U)
#define GLB_REG_FCLK_EN_MSK                                     (((1U<<GLB_REG_FCLK_EN_LEN)-1)<<GLB_REG_FCLK_EN_POS)
#define GLB_REG_FCLK_EN_UMSK                                    (~(((1U<<GLB_REG_FCLK_EN_LEN)-1)<<GLB_REG_FCLK_EN_POS))
#define GLB_REG_HCLK_EN                                         GLB_REG_HCLK_EN
#define GLB_REG_HCLK_EN_POS                                     (2U)
#define GLB_REG_HCLK_EN_LEN                                     (1U)
#define GLB_REG_HCLK_EN_MSK                                     (((1U<<GLB_REG_HCLK_EN_LEN)-1)<<GLB_REG_HCLK_EN_POS)
#define GLB_REG_HCLK_EN_UMSK                                    (~(((1U<<GLB_REG_HCLK_EN_LEN)-1)<<GLB_REG_HCLK_EN_POS))
#define GLB_REG_BCLK_EN                                         GLB_REG_BCLK_EN
#define GLB_REG_BCLK_EN_POS                                     (3U)
#define GLB_REG_BCLK_EN_LEN                                     (1U)
#define GLB_REG_BCLK_EN_MSK                                     (((1U<<GLB_REG_BCLK_EN_LEN)-1)<<GLB_REG_BCLK_EN_POS)
#define GLB_REG_BCLK_EN_UMSK                                    (~(((1U<<GLB_REG_BCLK_EN_LEN)-1)<<GLB_REG_BCLK_EN_POS))
#define GLB_REG_PLL_SEL                                         GLB_REG_PLL_SEL
#define GLB_REG_PLL_SEL_POS                                     (4U)
#define GLB_REG_PLL_SEL_LEN                                     (2U)
#define GLB_REG_PLL_SEL_MSK                                     (((1U<<GLB_REG_PLL_SEL_LEN)-1)<<GLB_REG_PLL_SEL_POS)
#define GLB_REG_PLL_SEL_UMSK                                    (~(((1U<<GLB_REG_PLL_SEL_LEN)-1)<<GLB_REG_PLL_SEL_POS))
#define GLB_HBN_ROOT_CLK_SEL                                    GLB_HBN_ROOT_CLK_SEL
#define GLB_HBN_ROOT_CLK_SEL_POS                                (6U)
#define GLB_HBN_ROOT_CLK_SEL_LEN                                (2U)
#define GLB_HBN_ROOT_CLK_SEL_MSK                                (((1U<<GLB_HBN_ROOT_CLK_SEL_LEN)-1)<<GLB_HBN_ROOT_CLK_SEL_POS)
#define GLB_HBN_ROOT_CLK_SEL_UMSK                               (~(((1U<<GLB_HBN_ROOT_CLK_SEL_LEN)-1)<<GLB_HBN_ROOT_CLK_SEL_POS))
#define GLB_REG_HCLK_DIV                                        GLB_REG_HCLK_DIV
#define GLB_REG_HCLK_DIV_POS                                    (8U)
#define GLB_REG_HCLK_DIV_LEN                                    (8U)
#define GLB_REG_HCLK_DIV_MSK                                    (((1U<<GLB_REG_HCLK_DIV_LEN)-1)<<GLB_REG_HCLK_DIV_POS)
#define GLB_REG_HCLK_DIV_UMSK                                   (~(((1U<<GLB_REG_HCLK_DIV_LEN)-1)<<GLB_REG_HCLK_DIV_POS))
#define GLB_REG_BCLK_DIV                                        GLB_REG_BCLK_DIV
#define GLB_REG_BCLK_DIV_POS                                    (16U)
#define GLB_REG_BCLK_DIV_LEN                                    (8U)
#define GLB_REG_BCLK_DIV_MSK                                    (((1U<<GLB_REG_BCLK_DIV_LEN)-1)<<GLB_REG_BCLK_DIV_POS)
#define GLB_REG_BCLK_DIV_UMSK                                   (~(((1U<<GLB_REG_BCLK_DIV_LEN)-1)<<GLB_REG_BCLK_DIV_POS))
#define GLB_FCLK_SW_STATE                                       GLB_FCLK_SW_STATE
#define GLB_FCLK_SW_STATE_POS                                   (24U)
#define GLB_FCLK_SW_STATE_LEN                                   (3U)
#define GLB_FCLK_SW_STATE_MSK                                   (((1U<<GLB_FCLK_SW_STATE_LEN)-1)<<GLB_FCLK_SW_STATE_POS)
#define GLB_FCLK_SW_STATE_UMSK                                  (~(((1U<<GLB_FCLK_SW_STATE_LEN)-1)<<GLB_FCLK_SW_STATE_POS))
#define GLB_CHIP_RDY                                            GLB_CHIP_RDY
#define GLB_CHIP_RDY_POS                                        (27U)
#define GLB_CHIP_RDY_LEN                                        (1U)
#define GLB_CHIP_RDY_MSK                                        (((1U<<GLB_CHIP_RDY_LEN)-1)<<GLB_CHIP_RDY_POS)
#define GLB_CHIP_RDY_UMSK                                       (~(((1U<<GLB_CHIP_RDY_LEN)-1)<<GLB_CHIP_RDY_POS))
#define GLB_ID                                                  GLB_ID
#define GLB_ID_POS                                              (28U)
#define GLB_ID_LEN                                              (4U)
#define GLB_ID_MSK                                              (((1U<<GLB_ID_LEN)-1)<<GLB_ID_POS)
#define GLB_ID_UMSK                                             (~(((1U<<GLB_ID_LEN)-1)<<GLB_ID_POS))

/* 0x4 : clk_cfg1 */
#define GLB_CLK_CFG1_OFFSET                                     (0x4)
#define GLB_KYS_1M_CLK_DIV                                      GLB_KYS_1M_CLK_DIV
#define GLB_KYS_1M_CLK_DIV_POS                                  (0U)
#define GLB_KYS_1M_CLK_DIV_LEN                                  (5U)
#define GLB_KYS_1M_CLK_DIV_MSK                                  (((1U<<GLB_KYS_1M_CLK_DIV_LEN)-1)<<GLB_KYS_1M_CLK_DIV_POS)
#define GLB_KYS_1M_CLK_DIV_UMSK                                 (~(((1U<<GLB_KYS_1M_CLK_DIV_LEN)-1)<<GLB_KYS_1M_CLK_DIV_POS))
#define GLB_KYS_1M_CLK_SEL                                      GLB_KYS_1M_CLK_SEL
#define GLB_KYS_1M_CLK_SEL_POS                                  (7U)
#define GLB_KYS_1M_CLK_SEL_LEN                                  (1U)
#define GLB_KYS_1M_CLK_SEL_MSK                                  (((1U<<GLB_KYS_1M_CLK_SEL_LEN)-1)<<GLB_KYS_1M_CLK_SEL_POS)
#define GLB_KYS_1M_CLK_SEL_UMSK                                 (~(((1U<<GLB_KYS_1M_CLK_SEL_LEN)-1)<<GLB_KYS_1M_CLK_SEL_POS))
#define GLB_REG_AUDIO_CLK_DIV                                   GLB_REG_AUDIO_CLK_DIV
#define GLB_REG_AUDIO_CLK_DIV_POS                               (8U)
#define GLB_REG_AUDIO_CLK_DIV_LEN                               (4U)
#define GLB_REG_AUDIO_CLK_DIV_MSK                               (((1U<<GLB_REG_AUDIO_CLK_DIV_LEN)-1)<<GLB_REG_AUDIO_CLK_DIV_POS)
#define GLB_REG_AUDIO_CLK_DIV_UMSK                              (~(((1U<<GLB_REG_AUDIO_CLK_DIV_LEN)-1)<<GLB_REG_AUDIO_CLK_DIV_POS))
#define GLB_REG_AUDIO_CLK_DIV_EN                                GLB_REG_AUDIO_CLK_DIV_EN
#define GLB_REG_AUDIO_CLK_DIV_EN_POS                            (12U)
#define GLB_REG_AUDIO_CLK_DIV_EN_LEN                            (1U)
#define GLB_REG_AUDIO_CLK_DIV_EN_MSK                            (((1U<<GLB_REG_AUDIO_CLK_DIV_EN_LEN)-1)<<GLB_REG_AUDIO_CLK_DIV_EN_POS)
#define GLB_REG_AUDIO_CLK_DIV_EN_UMSK                           (~(((1U<<GLB_REG_AUDIO_CLK_DIV_EN_LEN)-1)<<GLB_REG_AUDIO_CLK_DIV_EN_POS))
#define GLB_REG_AUDIO_AUTO_DIV_EN                               GLB_REG_AUDIO_AUTO_DIV_EN
#define GLB_REG_AUDIO_AUTO_DIV_EN_POS                           (13U)
#define GLB_REG_AUDIO_AUTO_DIV_EN_LEN                           (1U)
#define GLB_REG_AUDIO_AUTO_DIV_EN_MSK                           (((1U<<GLB_REG_AUDIO_AUTO_DIV_EN_LEN)-1)<<GLB_REG_AUDIO_AUTO_DIV_EN_POS)
#define GLB_REG_AUDIO_AUTO_DIV_EN_UMSK                          (~(((1U<<GLB_REG_AUDIO_AUTO_DIV_EN_LEN)-1)<<GLB_REG_AUDIO_AUTO_DIV_EN_POS))
#define GLB_REG_AUDIO_CLK_SRC_SEL                               GLB_REG_AUDIO_CLK_SRC_SEL
#define GLB_REG_AUDIO_CLK_SRC_SEL_POS                           (14U)
#define GLB_REG_AUDIO_CLK_SRC_SEL_LEN                           (1U)
#define GLB_REG_AUDIO_CLK_SRC_SEL_MSK                           (((1U<<GLB_REG_AUDIO_CLK_SRC_SEL_LEN)-1)<<GLB_REG_AUDIO_CLK_SRC_SEL_POS)
#define GLB_REG_AUDIO_CLK_SRC_SEL_UMSK                          (~(((1U<<GLB_REG_AUDIO_CLK_SRC_SEL_LEN)-1)<<GLB_REG_AUDIO_CLK_SRC_SEL_POS))
#define GLB_BLE_CLK_SEL                                         GLB_BLE_CLK_SEL
#define GLB_BLE_CLK_SEL_POS                                     (16U)
#define GLB_BLE_CLK_SEL_LEN                                     (6U)
#define GLB_BLE_CLK_SEL_MSK                                     (((1U<<GLB_BLE_CLK_SEL_LEN)-1)<<GLB_BLE_CLK_SEL_POS)
#define GLB_BLE_CLK_SEL_UMSK                                    (~(((1U<<GLB_BLE_CLK_SEL_LEN)-1)<<GLB_BLE_CLK_SEL_POS))
#define GLB_BLE_EN                                              GLB_BLE_EN
#define GLB_BLE_EN_POS                                          (24U)
#define GLB_BLE_EN_LEN                                          (1U)
#define GLB_BLE_EN_MSK                                          (((1U<<GLB_BLE_EN_LEN)-1)<<GLB_BLE_EN_POS)
#define GLB_BLE_EN_UMSK                                         (~(((1U<<GLB_BLE_EN_LEN)-1)<<GLB_BLE_EN_POS))
#define GLB_M154_ZBEN                                           GLB_M154_ZBEN
#define GLB_M154_ZBEN_POS                                       (25U)
#define GLB_M154_ZBEN_LEN                                       (1U)
#define GLB_M154_ZBEN_MSK                                       (((1U<<GLB_M154_ZBEN_LEN)-1)<<GLB_M154_ZBEN_POS)
#define GLB_M154_ZBEN_UMSK                                      (~(((1U<<GLB_M154_ZBEN_LEN)-1)<<GLB_M154_ZBEN_POS))

/* 0x8 : clk_cfg2 */
#define GLB_CLK_CFG2_OFFSET                                     (0x8)
#define GLB_UART_CLK_DIV                                        GLB_UART_CLK_DIV
#define GLB_UART_CLK_DIV_POS                                    (0U)
#define GLB_UART_CLK_DIV_LEN                                    (3U)
#define GLB_UART_CLK_DIV_MSK                                    (((1U<<GLB_UART_CLK_DIV_LEN)-1)<<GLB_UART_CLK_DIV_POS)
#define GLB_UART_CLK_DIV_UMSK                                   (~(((1U<<GLB_UART_CLK_DIV_LEN)-1)<<GLB_UART_CLK_DIV_POS))
#define GLB_UART_CLK_EN                                         GLB_UART_CLK_EN
#define GLB_UART_CLK_EN_POS                                     (4U)
#define GLB_UART_CLK_EN_LEN                                     (1U)
#define GLB_UART_CLK_EN_MSK                                     (((1U<<GLB_UART_CLK_EN_LEN)-1)<<GLB_UART_CLK_EN_POS)
#define GLB_UART_CLK_EN_UMSK                                    (~(((1U<<GLB_UART_CLK_EN_LEN)-1)<<GLB_UART_CLK_EN_POS))
#define GLB_HBN_UART_CLK_SEL                                    GLB_HBN_UART_CLK_SEL
#define GLB_HBN_UART_CLK_SEL_POS                                (7U)
#define GLB_HBN_UART_CLK_SEL_LEN                                (1U)
#define GLB_HBN_UART_CLK_SEL_MSK                                (((1U<<GLB_HBN_UART_CLK_SEL_LEN)-1)<<GLB_HBN_UART_CLK_SEL_POS)
#define GLB_HBN_UART_CLK_SEL_UMSK                               (~(((1U<<GLB_HBN_UART_CLK_SEL_LEN)-1)<<GLB_HBN_UART_CLK_SEL_POS))
#define GLB_SF_CLK_DIV                                          GLB_SF_CLK_DIV
#define GLB_SF_CLK_DIV_POS                                      (8U)
#define GLB_SF_CLK_DIV_LEN                                      (3U)
#define GLB_SF_CLK_DIV_MSK                                      (((1U<<GLB_SF_CLK_DIV_LEN)-1)<<GLB_SF_CLK_DIV_POS)
#define GLB_SF_CLK_DIV_UMSK                                     (~(((1U<<GLB_SF_CLK_DIV_LEN)-1)<<GLB_SF_CLK_DIV_POS))
#define GLB_SF_CLK_EN                                           GLB_SF_CLK_EN
#define GLB_SF_CLK_EN_POS                                       (11U)
#define GLB_SF_CLK_EN_LEN                                       (1U)
#define GLB_SF_CLK_EN_MSK                                       (((1U<<GLB_SF_CLK_EN_LEN)-1)<<GLB_SF_CLK_EN_POS)
#define GLB_SF_CLK_EN_UMSK                                      (~(((1U<<GLB_SF_CLK_EN_LEN)-1)<<GLB_SF_CLK_EN_POS))
#define GLB_SF_CLK_SEL                                          GLB_SF_CLK_SEL
#define GLB_SF_CLK_SEL_POS                                      (12U)
#define GLB_SF_CLK_SEL_LEN                                      (2U)
#define GLB_SF_CLK_SEL_MSK                                      (((1U<<GLB_SF_CLK_SEL_LEN)-1)<<GLB_SF_CLK_SEL_POS)
#define GLB_SF_CLK_SEL_UMSK                                     (~(((1U<<GLB_SF_CLK_SEL_LEN)-1)<<GLB_SF_CLK_SEL_POS))
#define GLB_IR_CLK_DIV                                          GLB_IR_CLK_DIV
#define GLB_IR_CLK_DIV_POS                                      (16U)
#define GLB_IR_CLK_DIV_LEN                                      (6U)
#define GLB_IR_CLK_DIV_MSK                                      (((1U<<GLB_IR_CLK_DIV_LEN)-1)<<GLB_IR_CLK_DIV_POS)
#define GLB_IR_CLK_DIV_UMSK                                     (~(((1U<<GLB_IR_CLK_DIV_LEN)-1)<<GLB_IR_CLK_DIV_POS))
#define GLB_HBN_UART_CLK_SEL2                                   GLB_HBN_UART_CLK_SEL2
#define GLB_HBN_UART_CLK_SEL2_POS                               (22U)
#define GLB_HBN_UART_CLK_SEL2_LEN                               (1U)
#define GLB_HBN_UART_CLK_SEL2_MSK                               (((1U<<GLB_HBN_UART_CLK_SEL2_LEN)-1)<<GLB_HBN_UART_CLK_SEL2_POS)
#define GLB_HBN_UART_CLK_SEL2_UMSK                              (~(((1U<<GLB_HBN_UART_CLK_SEL2_LEN)-1)<<GLB_HBN_UART_CLK_SEL2_POS))
#define GLB_IR_CLK_EN                                           GLB_IR_CLK_EN
#define GLB_IR_CLK_EN_POS                                       (23U)
#define GLB_IR_CLK_EN_LEN                                       (1U)
#define GLB_IR_CLK_EN_MSK                                       (((1U<<GLB_IR_CLK_EN_LEN)-1)<<GLB_IR_CLK_EN_POS)
#define GLB_IR_CLK_EN_UMSK                                      (~(((1U<<GLB_IR_CLK_EN_LEN)-1)<<GLB_IR_CLK_EN_POS))
#define GLB_DMA_CLK_EN                                          GLB_DMA_CLK_EN
#define GLB_DMA_CLK_EN_POS                                      (24U)
#define GLB_DMA_CLK_EN_LEN                                      (4U)
#define GLB_DMA_CLK_EN_MSK                                      (((1U<<GLB_DMA_CLK_EN_LEN)-1)<<GLB_DMA_CLK_EN_POS)
#define GLB_DMA_CLK_EN_UMSK                                     (~(((1U<<GLB_DMA_CLK_EN_LEN)-1)<<GLB_DMA_CLK_EN_POS))

/* 0xC : clk_cfg3 */
#define GLB_CLK_CFG3_OFFSET                                     (0xC)
#define GLB_SPI_CLK_DIV                                         GLB_SPI_CLK_DIV
#define GLB_SPI_CLK_DIV_POS                                     (0U)
#define GLB_SPI_CLK_DIV_LEN                                     (5U)
#define GLB_SPI_CLK_DIV_MSK                                     (((1U<<GLB_SPI_CLK_DIV_LEN)-1)<<GLB_SPI_CLK_DIV_POS)
#define GLB_SPI_CLK_DIV_UMSK                                    (~(((1U<<GLB_SPI_CLK_DIV_LEN)-1)<<GLB_SPI_CLK_DIV_POS))
#define GLB_SPI_CLK_EN                                          GLB_SPI_CLK_EN
#define GLB_SPI_CLK_EN_POS                                      (8U)
#define GLB_SPI_CLK_EN_LEN                                      (1U)
#define GLB_SPI_CLK_EN_MSK                                      (((1U<<GLB_SPI_CLK_EN_LEN)-1)<<GLB_SPI_CLK_EN_POS)
#define GLB_SPI_CLK_EN_UMSK                                     (~(((1U<<GLB_SPI_CLK_EN_LEN)-1)<<GLB_SPI_CLK_EN_POS))
#define GLB_SPI_CLK_SEL                                         GLB_SPI_CLK_SEL
#define GLB_SPI_CLK_SEL_POS                                     (9U)
#define GLB_SPI_CLK_SEL_LEN                                     (1U)
#define GLB_SPI_CLK_SEL_MSK                                     (((1U<<GLB_SPI_CLK_SEL_LEN)-1)<<GLB_SPI_CLK_SEL_POS)
#define GLB_SPI_CLK_SEL_UMSK                                    (~(((1U<<GLB_SPI_CLK_SEL_LEN)-1)<<GLB_SPI_CLK_SEL_POS))
#define GLB_CHIP_CLK_OUT_EN                                     GLB_CHIP_CLK_OUT_EN
#define GLB_CHIP_CLK_OUT_EN_POS                                 (10U)
#define GLB_CHIP_CLK_OUT_EN_LEN                                 (4U)
#define GLB_CHIP_CLK_OUT_EN_MSK                                 (((1U<<GLB_CHIP_CLK_OUT_EN_LEN)-1)<<GLB_CHIP_CLK_OUT_EN_POS)
#define GLB_CHIP_CLK_OUT_EN_UMSK                                (~(((1U<<GLB_CHIP_CLK_OUT_EN_LEN)-1)<<GLB_CHIP_CLK_OUT_EN_POS))
#define GLB_GPIO_TMR_CLK_SEL                                    GLB_GPIO_TMR_CLK_SEL
#define GLB_GPIO_TMR_CLK_SEL_POS                                (14U)
#define GLB_GPIO_TMR_CLK_SEL_LEN                                (2U)
#define GLB_GPIO_TMR_CLK_SEL_MSK                                (((1U<<GLB_GPIO_TMR_CLK_SEL_LEN)-1)<<GLB_GPIO_TMR_CLK_SEL_POS)
#define GLB_GPIO_TMR_CLK_SEL_UMSK                               (~(((1U<<GLB_GPIO_TMR_CLK_SEL_LEN)-1)<<GLB_GPIO_TMR_CLK_SEL_POS))
#define GLB_I2C_CLK_DIV                                         GLB_I2C_CLK_DIV
#define GLB_I2C_CLK_DIV_POS                                     (16U)
#define GLB_I2C_CLK_DIV_LEN                                     (8U)
#define GLB_I2C_CLK_DIV_MSK                                     (((1U<<GLB_I2C_CLK_DIV_LEN)-1)<<GLB_I2C_CLK_DIV_POS)
#define GLB_I2C_CLK_DIV_UMSK                                    (~(((1U<<GLB_I2C_CLK_DIV_LEN)-1)<<GLB_I2C_CLK_DIV_POS))
#define GLB_I2C_CLK_EN                                          GLB_I2C_CLK_EN
#define GLB_I2C_CLK_EN_POS                                      (24U)
#define GLB_I2C_CLK_EN_LEN                                      (1U)
#define GLB_I2C_CLK_EN_MSK                                      (((1U<<GLB_I2C_CLK_EN_LEN)-1)<<GLB_I2C_CLK_EN_POS)
#define GLB_I2C_CLK_EN_UMSK                                     (~(((1U<<GLB_I2C_CLK_EN_LEN)-1)<<GLB_I2C_CLK_EN_POS))
#define GLB_I2C_CLK_SEL                                         GLB_I2C_CLK_SEL
#define GLB_I2C_CLK_SEL_POS                                     (25U)
#define GLB_I2C_CLK_SEL_LEN                                     (1U)
#define GLB_I2C_CLK_SEL_MSK                                     (((1U<<GLB_I2C_CLK_SEL_LEN)-1)<<GLB_I2C_CLK_SEL_POS)
#define GLB_I2C_CLK_SEL_UMSK                                    (~(((1U<<GLB_I2C_CLK_SEL_LEN)-1)<<GLB_I2C_CLK_SEL_POS))
#define GLB_CHIP_CLK_OUT_0_SEL                                  GLB_CHIP_CLK_OUT_0_SEL
#define GLB_CHIP_CLK_OUT_0_SEL_POS                              (28U)
#define GLB_CHIP_CLK_OUT_0_SEL_LEN                              (2U)
#define GLB_CHIP_CLK_OUT_0_SEL_MSK                              (((1U<<GLB_CHIP_CLK_OUT_0_SEL_LEN)-1)<<GLB_CHIP_CLK_OUT_0_SEL_POS)
#define GLB_CHIP_CLK_OUT_0_SEL_UMSK                             (~(((1U<<GLB_CHIP_CLK_OUT_0_SEL_LEN)-1)<<GLB_CHIP_CLK_OUT_0_SEL_POS))
#define GLB_CHIP_CLK_OUT_1_SEL                                  GLB_CHIP_CLK_OUT_1_SEL
#define GLB_CHIP_CLK_OUT_1_SEL_POS                              (30U)
#define GLB_CHIP_CLK_OUT_1_SEL_LEN                              (2U)
#define GLB_CHIP_CLK_OUT_1_SEL_MSK                              (((1U<<GLB_CHIP_CLK_OUT_1_SEL_LEN)-1)<<GLB_CHIP_CLK_OUT_1_SEL_POS)
#define GLB_CHIP_CLK_OUT_1_SEL_UMSK                             (~(((1U<<GLB_CHIP_CLK_OUT_1_SEL_LEN)-1)<<GLB_CHIP_CLK_OUT_1_SEL_POS))

/* 0x10 : swrst_cfg0 */
#define GLB_SWRST_CFG0_OFFSET                                   (0x10)
#define GLB_SWRST_S300                                          GLB_SWRST_S300
#define GLB_SWRST_S300_POS                                      (4U)
#define GLB_SWRST_S300_LEN                                      (1U)
#define GLB_SWRST_S300_MSK                                      (((1U<<GLB_SWRST_S300_LEN)-1)<<GLB_SWRST_S300_POS)
#define GLB_SWRST_S300_UMSK                                     (~(((1U<<GLB_SWRST_S300_LEN)-1)<<GLB_SWRST_S300_POS))
#define GLB_SWRST_S301                                          GLB_SWRST_S301
#define GLB_SWRST_S301_POS                                      (8U)
#define GLB_SWRST_S301_LEN                                      (1U)
#define GLB_SWRST_S301_MSK                                      (((1U<<GLB_SWRST_S301_LEN)-1)<<GLB_SWRST_S301_POS)
#define GLB_SWRST_S301_UMSK                                     (~(((1U<<GLB_SWRST_S301_LEN)-1)<<GLB_SWRST_S301_POS))

/* 0x14 : swrst_cfg1 */
#define GLB_SWRST_CFG1_OFFSET                                   (0x14)
#define GLB_SWRST_S10                                           GLB_SWRST_S10
#define GLB_SWRST_S10_POS                                       (0U)
#define GLB_SWRST_S10_LEN                                       (1U)
#define GLB_SWRST_S10_MSK                                       (((1U<<GLB_SWRST_S10_LEN)-1)<<GLB_SWRST_S10_POS)
#define GLB_SWRST_S10_UMSK                                      (~(((1U<<GLB_SWRST_S10_LEN)-1)<<GLB_SWRST_S10_POS))
#define GLB_SWRST_S11                                           GLB_SWRST_S11
#define GLB_SWRST_S11_POS                                       (1U)
#define GLB_SWRST_S11_LEN                                       (1U)
#define GLB_SWRST_S11_MSK                                       (((1U<<GLB_SWRST_S11_LEN)-1)<<GLB_SWRST_S11_POS)
#define GLB_SWRST_S11_UMSK                                      (~(((1U<<GLB_SWRST_S11_LEN)-1)<<GLB_SWRST_S11_POS))
#define GLB_SWRST_S12                                           GLB_SWRST_S12
#define GLB_SWRST_S12_POS                                       (2U)
#define GLB_SWRST_S12_LEN                                       (1U)
#define GLB_SWRST_S12_MSK                                       (((1U<<GLB_SWRST_S12_LEN)-1)<<GLB_SWRST_S12_POS)
#define GLB_SWRST_S12_UMSK                                      (~(((1U<<GLB_SWRST_S12_LEN)-1)<<GLB_SWRST_S12_POS))
#define GLB_SWRST_S13                                           GLB_SWRST_S13
#define GLB_SWRST_S13_POS                                       (3U)
#define GLB_SWRST_S13_LEN                                       (1U)
#define GLB_SWRST_S13_MSK                                       (((1U<<GLB_SWRST_S13_LEN)-1)<<GLB_SWRST_S13_POS)
#define GLB_SWRST_S13_UMSK                                      (~(((1U<<GLB_SWRST_S13_LEN)-1)<<GLB_SWRST_S13_POS))
#define GLB_SWRST_S14                                           GLB_SWRST_S14
#define GLB_SWRST_S14_POS                                       (4U)
#define GLB_SWRST_S14_LEN                                       (1U)
#define GLB_SWRST_S14_MSK                                       (((1U<<GLB_SWRST_S14_LEN)-1)<<GLB_SWRST_S14_POS)
#define GLB_SWRST_S14_UMSK                                      (~(((1U<<GLB_SWRST_S14_LEN)-1)<<GLB_SWRST_S14_POS))
#define GLB_SWRST_S15                                           GLB_SWRST_S15
#define GLB_SWRST_S15_POS                                       (5U)
#define GLB_SWRST_S15_LEN                                       (1U)
#define GLB_SWRST_S15_MSK                                       (((1U<<GLB_SWRST_S15_LEN)-1)<<GLB_SWRST_S15_POS)
#define GLB_SWRST_S15_UMSK                                      (~(((1U<<GLB_SWRST_S15_LEN)-1)<<GLB_SWRST_S15_POS))
#define GLB_SWRST_S16                                           GLB_SWRST_S16
#define GLB_SWRST_S16_POS                                       (6U)
#define GLB_SWRST_S16_LEN                                       (1U)
#define GLB_SWRST_S16_MSK                                       (((1U<<GLB_SWRST_S16_LEN)-1)<<GLB_SWRST_S16_POS)
#define GLB_SWRST_S16_UMSK                                      (~(((1U<<GLB_SWRST_S16_LEN)-1)<<GLB_SWRST_S16_POS))
#define GLB_SWRST_S17                                           GLB_SWRST_S17
#define GLB_SWRST_S17_POS                                       (7U)
#define GLB_SWRST_S17_LEN                                       (1U)
#define GLB_SWRST_S17_MSK                                       (((1U<<GLB_SWRST_S17_LEN)-1)<<GLB_SWRST_S17_POS)
#define GLB_SWRST_S17_UMSK                                      (~(((1U<<GLB_SWRST_S17_LEN)-1)<<GLB_SWRST_S17_POS))
#define GLB_SWRST_S18                                           GLB_SWRST_S18
#define GLB_SWRST_S18_POS                                       (8U)
#define GLB_SWRST_S18_LEN                                       (1U)
#define GLB_SWRST_S18_MSK                                       (((1U<<GLB_SWRST_S18_LEN)-1)<<GLB_SWRST_S18_POS)
#define GLB_SWRST_S18_UMSK                                      (~(((1U<<GLB_SWRST_S18_LEN)-1)<<GLB_SWRST_S18_POS))
#define GLB_SWRST_S19                                           GLB_SWRST_S19
#define GLB_SWRST_S19_POS                                       (9U)
#define GLB_SWRST_S19_LEN                                       (1U)
#define GLB_SWRST_S19_MSK                                       (((1U<<GLB_SWRST_S19_LEN)-1)<<GLB_SWRST_S19_POS)
#define GLB_SWRST_S19_UMSK                                      (~(((1U<<GLB_SWRST_S19_LEN)-1)<<GLB_SWRST_S19_POS))
#define GLB_SWRST_S1B                                           GLB_SWRST_S1B
#define GLB_SWRST_S1B_POS                                       (11U)
#define GLB_SWRST_S1B_LEN                                       (1U)
#define GLB_SWRST_S1B_MSK                                       (((1U<<GLB_SWRST_S1B_LEN)-1)<<GLB_SWRST_S1B_POS)
#define GLB_SWRST_S1B_UMSK                                      (~(((1U<<GLB_SWRST_S1B_LEN)-1)<<GLB_SWRST_S1B_POS))
#define GLB_SWRST_S1C                                           GLB_SWRST_S1C
#define GLB_SWRST_S1C_POS                                       (12U)
#define GLB_SWRST_S1C_LEN                                       (1U)
#define GLB_SWRST_S1C_MSK                                       (((1U<<GLB_SWRST_S1C_LEN)-1)<<GLB_SWRST_S1C_POS)
#define GLB_SWRST_S1C_UMSK                                      (~(((1U<<GLB_SWRST_S1C_LEN)-1)<<GLB_SWRST_S1C_POS))
#define GLB_SWRST_S1E                                           GLB_SWRST_S1E
#define GLB_SWRST_S1E_POS                                       (14U)
#define GLB_SWRST_S1E_LEN                                       (1U)
#define GLB_SWRST_S1E_MSK                                       (((1U<<GLB_SWRST_S1E_LEN)-1)<<GLB_SWRST_S1E_POS)
#define GLB_SWRST_S1E_UMSK                                      (~(((1U<<GLB_SWRST_S1E_LEN)-1)<<GLB_SWRST_S1E_POS))
#define GLB_SWRST_S1A0                                          GLB_SWRST_S1A0
#define GLB_SWRST_S1A0_POS                                      (16U)
#define GLB_SWRST_S1A0_LEN                                      (1U)
#define GLB_SWRST_S1A0_MSK                                      (((1U<<GLB_SWRST_S1A0_LEN)-1)<<GLB_SWRST_S1A0_POS)
#define GLB_SWRST_S1A0_UMSK                                     (~(((1U<<GLB_SWRST_S1A0_LEN)-1)<<GLB_SWRST_S1A0_POS))
#define GLB_SWRST_S1A2                                          GLB_SWRST_S1A2
#define GLB_SWRST_S1A2_POS                                      (18U)
#define GLB_SWRST_S1A2_LEN                                      (1U)
#define GLB_SWRST_S1A2_MSK                                      (((1U<<GLB_SWRST_S1A2_LEN)-1)<<GLB_SWRST_S1A2_POS)
#define GLB_SWRST_S1A2_UMSK                                     (~(((1U<<GLB_SWRST_S1A2_LEN)-1)<<GLB_SWRST_S1A2_POS))
#define GLB_SWRST_S1A3                                          GLB_SWRST_S1A3
#define GLB_SWRST_S1A3_POS                                      (19U)
#define GLB_SWRST_S1A3_LEN                                      (1U)
#define GLB_SWRST_S1A3_MSK                                      (((1U<<GLB_SWRST_S1A3_LEN)-1)<<GLB_SWRST_S1A3_POS)
#define GLB_SWRST_S1A3_UMSK                                     (~(((1U<<GLB_SWRST_S1A3_LEN)-1)<<GLB_SWRST_S1A3_POS))
#define GLB_SWRST_S1A4                                          GLB_SWRST_S1A4
#define GLB_SWRST_S1A4_POS                                      (20U)
#define GLB_SWRST_S1A4_LEN                                      (1U)
#define GLB_SWRST_S1A4_MSK                                      (((1U<<GLB_SWRST_S1A4_LEN)-1)<<GLB_SWRST_S1A4_POS)
#define GLB_SWRST_S1A4_UMSK                                     (~(((1U<<GLB_SWRST_S1A4_LEN)-1)<<GLB_SWRST_S1A4_POS))
#define GLB_SWRST_S1A5                                          GLB_SWRST_S1A5
#define GLB_SWRST_S1A5_POS                                      (21U)
#define GLB_SWRST_S1A5_LEN                                      (1U)
#define GLB_SWRST_S1A5_MSK                                      (((1U<<GLB_SWRST_S1A5_LEN)-1)<<GLB_SWRST_S1A5_POS)
#define GLB_SWRST_S1A5_UMSK                                     (~(((1U<<GLB_SWRST_S1A5_LEN)-1)<<GLB_SWRST_S1A5_POS))
#define GLB_SWRST_S1A6                                          GLB_SWRST_S1A6
#define GLB_SWRST_S1A6_POS                                      (22U)
#define GLB_SWRST_S1A6_LEN                                      (1U)
#define GLB_SWRST_S1A6_MSK                                      (((1U<<GLB_SWRST_S1A6_LEN)-1)<<GLB_SWRST_S1A6_POS)
#define GLB_SWRST_S1A6_UMSK                                     (~(((1U<<GLB_SWRST_S1A6_LEN)-1)<<GLB_SWRST_S1A6_POS))
#define GLB_SWRST_S1A7                                          GLB_SWRST_S1A7
#define GLB_SWRST_S1A7_POS                                      (23U)
#define GLB_SWRST_S1A7_LEN                                      (1U)
#define GLB_SWRST_S1A7_MSK                                      (((1U<<GLB_SWRST_S1A7_LEN)-1)<<GLB_SWRST_S1A7_POS)
#define GLB_SWRST_S1A7_UMSK                                     (~(((1U<<GLB_SWRST_S1A7_LEN)-1)<<GLB_SWRST_S1A7_POS))
#define GLB_SWRST_S1A9                                          GLB_SWRST_S1A9
#define GLB_SWRST_S1A9_POS                                      (25U)
#define GLB_SWRST_S1A9_LEN                                      (1U)
#define GLB_SWRST_S1A9_MSK                                      (((1U<<GLB_SWRST_S1A9_LEN)-1)<<GLB_SWRST_S1A9_POS)
#define GLB_SWRST_S1A9_UMSK                                     (~(((1U<<GLB_SWRST_S1A9_LEN)-1)<<GLB_SWRST_S1A9_POS))
#define GLB_SWRST_S1AD                                          GLB_SWRST_S1AD
#define GLB_SWRST_S1AD_POS                                      (29U)
#define GLB_SWRST_S1AD_LEN                                      (1U)
#define GLB_SWRST_S1AD_MSK                                      (((1U<<GLB_SWRST_S1AD_LEN)-1)<<GLB_SWRST_S1AD_POS)
#define GLB_SWRST_S1AD_UMSK                                     (~(((1U<<GLB_SWRST_S1AD_LEN)-1)<<GLB_SWRST_S1AD_POS))

/* 0x18 : swrst_cfg2 */
#define GLB_SWRST_CFG2_OFFSET                                   (0x18)
#define GLB_REG_CTRL_PWRON_RST                                  GLB_REG_CTRL_PWRON_RST
#define GLB_REG_CTRL_PWRON_RST_POS                              (0U)
#define GLB_REG_CTRL_PWRON_RST_LEN                              (1U)
#define GLB_REG_CTRL_PWRON_RST_MSK                              (((1U<<GLB_REG_CTRL_PWRON_RST_LEN)-1)<<GLB_REG_CTRL_PWRON_RST_POS)
#define GLB_REG_CTRL_PWRON_RST_UMSK                             (~(((1U<<GLB_REG_CTRL_PWRON_RST_LEN)-1)<<GLB_REG_CTRL_PWRON_RST_POS))
#define GLB_REG_CTRL_CPU_RESET                                  GLB_REG_CTRL_CPU_RESET
#define GLB_REG_CTRL_CPU_RESET_POS                              (1U)
#define GLB_REG_CTRL_CPU_RESET_LEN                              (1U)
#define GLB_REG_CTRL_CPU_RESET_MSK                              (((1U<<GLB_REG_CTRL_CPU_RESET_LEN)-1)<<GLB_REG_CTRL_CPU_RESET_POS)
#define GLB_REG_CTRL_CPU_RESET_UMSK                             (~(((1U<<GLB_REG_CTRL_CPU_RESET_LEN)-1)<<GLB_REG_CTRL_CPU_RESET_POS))
#define GLB_REG_CTRL_SYS_RESET                                  GLB_REG_CTRL_SYS_RESET
#define GLB_REG_CTRL_SYS_RESET_POS                              (2U)
#define GLB_REG_CTRL_SYS_RESET_LEN                              (1U)
#define GLB_REG_CTRL_SYS_RESET_MSK                              (((1U<<GLB_REG_CTRL_SYS_RESET_LEN)-1)<<GLB_REG_CTRL_SYS_RESET_POS)
#define GLB_REG_CTRL_SYS_RESET_UMSK                             (~(((1U<<GLB_REG_CTRL_SYS_RESET_LEN)-1)<<GLB_REG_CTRL_SYS_RESET_POS))
#define GLB_REG_CTRL_RESET_DUMMY                                GLB_REG_CTRL_RESET_DUMMY
#define GLB_REG_CTRL_RESET_DUMMY_POS                            (4U)
#define GLB_REG_CTRL_RESET_DUMMY_LEN                            (4U)
#define GLB_REG_CTRL_RESET_DUMMY_MSK                            (((1U<<GLB_REG_CTRL_RESET_DUMMY_LEN)-1)<<GLB_REG_CTRL_RESET_DUMMY_POS)
#define GLB_REG_CTRL_RESET_DUMMY_UMSK                           (~(((1U<<GLB_REG_CTRL_RESET_DUMMY_LEN)-1)<<GLB_REG_CTRL_RESET_DUMMY_POS))
#define GLB_PKA_CLK_SEL                                         GLB_PKA_CLK_SEL
#define GLB_PKA_CLK_SEL_POS                                     (24U)
#define GLB_PKA_CLK_SEL_LEN                                     (2U)
#define GLB_PKA_CLK_SEL_MSK                                     (((1U<<GLB_PKA_CLK_SEL_LEN)-1)<<GLB_PKA_CLK_SEL_POS)
#define GLB_PKA_CLK_SEL_UMSK                                    (~(((1U<<GLB_PKA_CLK_SEL_LEN)-1)<<GLB_PKA_CLK_SEL_POS))

/* 0x1C : swrst_cfg3 */
#define GLB_SWRST_CFG3_OFFSET                                   (0x1C)
#define GLB_DISRST_S11                                          GLB_DISRST_S11
#define GLB_DISRST_S11_POS                                      (1U)
#define GLB_DISRST_S11_LEN                                      (1U)
#define GLB_DISRST_S11_MSK                                      (((1U<<GLB_DISRST_S11_LEN)-1)<<GLB_DISRST_S11_POS)
#define GLB_DISRST_S11_UMSK                                     (~(((1U<<GLB_DISRST_S11_LEN)-1)<<GLB_DISRST_S11_POS))
#define GLB_DISRST_S12                                          GLB_DISRST_S12
#define GLB_DISRST_S12_POS                                      (2U)
#define GLB_DISRST_S12_LEN                                      (1U)
#define GLB_DISRST_S12_MSK                                      (((1U<<GLB_DISRST_S12_LEN)-1)<<GLB_DISRST_S12_POS)
#define GLB_DISRST_S12_UMSK                                     (~(((1U<<GLB_DISRST_S12_LEN)-1)<<GLB_DISRST_S12_POS))
#define GLB_DISRST_S18                                          GLB_DISRST_S18
#define GLB_DISRST_S18_POS                                      (8U)
#define GLB_DISRST_S18_LEN                                      (1U)
#define GLB_DISRST_S18_MSK                                      (((1U<<GLB_DISRST_S18_LEN)-1)<<GLB_DISRST_S18_POS)
#define GLB_DISRST_S18_UMSK                                     (~(((1U<<GLB_DISRST_S18_LEN)-1)<<GLB_DISRST_S18_POS))
#define GLB_DISRST_S1B                                          GLB_DISRST_S1B
#define GLB_DISRST_S1B_POS                                      (11U)
#define GLB_DISRST_S1B_LEN                                      (1U)
#define GLB_DISRST_S1B_MSK                                      (((1U<<GLB_DISRST_S1B_LEN)-1)<<GLB_DISRST_S1B_POS)
#define GLB_DISRST_S1B_UMSK                                     (~(((1U<<GLB_DISRST_S1B_LEN)-1)<<GLB_DISRST_S1B_POS))
#define GLB_DISRST_S1C                                          GLB_DISRST_S1C
#define GLB_DISRST_S1C_POS                                      (12U)
#define GLB_DISRST_S1C_LEN                                      (1U)
#define GLB_DISRST_S1C_MSK                                      (((1U<<GLB_DISRST_S1C_LEN)-1)<<GLB_DISRST_S1C_POS)
#define GLB_DISRST_S1C_UMSK                                     (~(((1U<<GLB_DISRST_S1C_LEN)-1)<<GLB_DISRST_S1C_POS))
#define GLB_DISRST_S1A0                                         GLB_DISRST_S1A0
#define GLB_DISRST_S1A0_POS                                     (16U)
#define GLB_DISRST_S1A0_LEN                                     (1U)
#define GLB_DISRST_S1A0_MSK                                     (((1U<<GLB_DISRST_S1A0_LEN)-1)<<GLB_DISRST_S1A0_POS)
#define GLB_DISRST_S1A0_UMSK                                    (~(((1U<<GLB_DISRST_S1A0_LEN)-1)<<GLB_DISRST_S1A0_POS))
#define GLB_DISRST_S1A2                                         GLB_DISRST_S1A2
#define GLB_DISRST_S1A2_POS                                     (18U)
#define GLB_DISRST_S1A2_LEN                                     (1U)
#define GLB_DISRST_S1A2_MSK                                     (((1U<<GLB_DISRST_S1A2_LEN)-1)<<GLB_DISRST_S1A2_POS)
#define GLB_DISRST_S1A2_UMSK                                    (~(((1U<<GLB_DISRST_S1A2_LEN)-1)<<GLB_DISRST_S1A2_POS))
#define GLB_DISRST_S1A3                                         GLB_DISRST_S1A3
#define GLB_DISRST_S1A3_POS                                     (19U)
#define GLB_DISRST_S1A3_LEN                                     (1U)
#define GLB_DISRST_S1A3_MSK                                     (((1U<<GLB_DISRST_S1A3_LEN)-1)<<GLB_DISRST_S1A3_POS)
#define GLB_DISRST_S1A3_UMSK                                    (~(((1U<<GLB_DISRST_S1A3_LEN)-1)<<GLB_DISRST_S1A3_POS))
#define GLB_DISRST_S1A4                                         GLB_DISRST_S1A4
#define GLB_DISRST_S1A4_POS                                     (20U)
#define GLB_DISRST_S1A4_LEN                                     (1U)
#define GLB_DISRST_S1A4_MSK                                     (((1U<<GLB_DISRST_S1A4_LEN)-1)<<GLB_DISRST_S1A4_POS)
#define GLB_DISRST_S1A4_UMSK                                    (~(((1U<<GLB_DISRST_S1A4_LEN)-1)<<GLB_DISRST_S1A4_POS))
#define GLB_DISRST_S1A5                                         GLB_DISRST_S1A5
#define GLB_DISRST_S1A5_POS                                     (21U)
#define GLB_DISRST_S1A5_LEN                                     (1U)
#define GLB_DISRST_S1A5_MSK                                     (((1U<<GLB_DISRST_S1A5_LEN)-1)<<GLB_DISRST_S1A5_POS)
#define GLB_DISRST_S1A5_UMSK                                    (~(((1U<<GLB_DISRST_S1A5_LEN)-1)<<GLB_DISRST_S1A5_POS))
#define GLB_DISRST_S1A6                                         GLB_DISRST_S1A6
#define GLB_DISRST_S1A6_POS                                     (22U)
#define GLB_DISRST_S1A6_LEN                                     (1U)
#define GLB_DISRST_S1A6_MSK                                     (((1U<<GLB_DISRST_S1A6_LEN)-1)<<GLB_DISRST_S1A6_POS)
#define GLB_DISRST_S1A6_UMSK                                    (~(((1U<<GLB_DISRST_S1A6_LEN)-1)<<GLB_DISRST_S1A6_POS))
#define GLB_DISRST_S1A7                                         GLB_DISRST_S1A7
#define GLB_DISRST_S1A7_POS                                     (23U)
#define GLB_DISRST_S1A7_LEN                                     (1U)
#define GLB_DISRST_S1A7_MSK                                     (((1U<<GLB_DISRST_S1A7_LEN)-1)<<GLB_DISRST_S1A7_POS)
#define GLB_DISRST_S1A7_UMSK                                    (~(((1U<<GLB_DISRST_S1A7_LEN)-1)<<GLB_DISRST_S1A7_POS))
#define GLB_DISRST_S1A9                                         GLB_DISRST_S1A9
#define GLB_DISRST_S1A9_POS                                     (25U)
#define GLB_DISRST_S1A9_LEN                                     (1U)
#define GLB_DISRST_S1A9_MSK                                     (((1U<<GLB_DISRST_S1A9_LEN)-1)<<GLB_DISRST_S1A9_POS)
#define GLB_DISRST_S1A9_UMSK                                    (~(((1U<<GLB_DISRST_S1A9_LEN)-1)<<GLB_DISRST_S1A9_POS))
#define GLB_DISRST_S1AD                                         GLB_DISRST_S1AD
#define GLB_DISRST_S1AD_POS                                     (29U)
#define GLB_DISRST_S1AD_LEN                                     (1U)
#define GLB_DISRST_S1AD_MSK                                     (((1U<<GLB_DISRST_S1AD_LEN)-1)<<GLB_DISRST_S1AD_POS)
#define GLB_DISRST_S1AD_UMSK                                    (~(((1U<<GLB_DISRST_S1AD_LEN)-1)<<GLB_DISRST_S1AD_POS))

/* 0x20 : cgen_cfg0 */
#define GLB_CGEN_CFG0_OFFSET                                    (0x20)
#define GLB_CGEN_M0                                             GLB_CGEN_M0
#define GLB_CGEN_M0_POS                                         (0U)
#define GLB_CGEN_M0_LEN                                         (1U)
#define GLB_CGEN_M0_MSK                                         (((1U<<GLB_CGEN_M0_LEN)-1)<<GLB_CGEN_M0_POS)
#define GLB_CGEN_M0_UMSK                                        (~(((1U<<GLB_CGEN_M0_LEN)-1)<<GLB_CGEN_M0_POS))
#define GLB_CGEN_M2                                             GLB_CGEN_M2
#define GLB_CGEN_M2_POS                                         (2U)
#define GLB_CGEN_M2_LEN                                         (1U)
#define GLB_CGEN_M2_MSK                                         (((1U<<GLB_CGEN_M2_LEN)-1)<<GLB_CGEN_M2_POS)
#define GLB_CGEN_M2_UMSK                                        (~(((1U<<GLB_CGEN_M2_LEN)-1)<<GLB_CGEN_M2_POS))
#define GLB_CGEN_M3                                             GLB_CGEN_M3
#define GLB_CGEN_M3_POS                                         (3U)
#define GLB_CGEN_M3_LEN                                         (1U)
#define GLB_CGEN_M3_MSK                                         (((1U<<GLB_CGEN_M3_LEN)-1)<<GLB_CGEN_M3_POS)
#define GLB_CGEN_M3_UMSK                                        (~(((1U<<GLB_CGEN_M3_LEN)-1)<<GLB_CGEN_M3_POS))
#define GLB_CGEN_M4                                             GLB_CGEN_M4
#define GLB_CGEN_M4_POS                                         (4U)
#define GLB_CGEN_M4_LEN                                         (1U)
#define GLB_CGEN_M4_MSK                                         (((1U<<GLB_CGEN_M4_LEN)-1)<<GLB_CGEN_M4_POS)
#define GLB_CGEN_M4_UMSK                                        (~(((1U<<GLB_CGEN_M4_LEN)-1)<<GLB_CGEN_M4_POS))

/* 0x24 : cgen_cfg1 */
#define GLB_CGEN_CFG1_OFFSET                                    (0x24)
#define GLB_CGEN_S12                                            GLB_CGEN_S12
#define GLB_CGEN_S12_POS                                        (2U)
#define GLB_CGEN_S12_LEN                                        (1U)
#define GLB_CGEN_S12_MSK                                        (((1U<<GLB_CGEN_S12_LEN)-1)<<GLB_CGEN_S12_POS)
#define GLB_CGEN_S12_UMSK                                       (~(((1U<<GLB_CGEN_S12_LEN)-1)<<GLB_CGEN_S12_POS))
#define GLB_CGEN_S13                                            GLB_CGEN_S13
#define GLB_CGEN_S13_POS                                        (3U)
#define GLB_CGEN_S13_LEN                                        (1U)
#define GLB_CGEN_S13_MSK                                        (((1U<<GLB_CGEN_S13_LEN)-1)<<GLB_CGEN_S13_POS)
#define GLB_CGEN_S13_UMSK                                       (~(((1U<<GLB_CGEN_S13_LEN)-1)<<GLB_CGEN_S13_POS))
#define GLB_CGEN_S14                                            GLB_CGEN_S14
#define GLB_CGEN_S14_POS                                        (4U)
#define GLB_CGEN_S14_LEN                                        (1U)
#define GLB_CGEN_S14_MSK                                        (((1U<<GLB_CGEN_S14_LEN)-1)<<GLB_CGEN_S14_POS)
#define GLB_CGEN_S14_UMSK                                       (~(((1U<<GLB_CGEN_S14_LEN)-1)<<GLB_CGEN_S14_POS))
#define GLB_CGEN_S15                                            GLB_CGEN_S15
#define GLB_CGEN_S15_POS                                        (5U)
#define GLB_CGEN_S15_LEN                                        (1U)
#define GLB_CGEN_S15_MSK                                        (((1U<<GLB_CGEN_S15_LEN)-1)<<GLB_CGEN_S15_POS)
#define GLB_CGEN_S15_UMSK                                       (~(((1U<<GLB_CGEN_S15_LEN)-1)<<GLB_CGEN_S15_POS))
#define GLB_CGEN_S17                                            GLB_CGEN_S17
#define GLB_CGEN_S17_POS                                        (7U)
#define GLB_CGEN_S17_LEN                                        (1U)
#define GLB_CGEN_S17_MSK                                        (((1U<<GLB_CGEN_S17_LEN)-1)<<GLB_CGEN_S17_POS)
#define GLB_CGEN_S17_UMSK                                       (~(((1U<<GLB_CGEN_S17_LEN)-1)<<GLB_CGEN_S17_POS))
#define GLB_CGEN_S1B                                            GLB_CGEN_S1B
#define GLB_CGEN_S1B_POS                                        (11U)
#define GLB_CGEN_S1B_LEN                                        (1U)
#define GLB_CGEN_S1B_MSK                                        (((1U<<GLB_CGEN_S1B_LEN)-1)<<GLB_CGEN_S1B_POS)
#define GLB_CGEN_S1B_UMSK                                       (~(((1U<<GLB_CGEN_S1B_LEN)-1)<<GLB_CGEN_S1B_POS))
#define GLB_CGEN_S1C                                            GLB_CGEN_S1C
#define GLB_CGEN_S1C_POS                                        (12U)
#define GLB_CGEN_S1C_LEN                                        (1U)
#define GLB_CGEN_S1C_MSK                                        (((1U<<GLB_CGEN_S1C_LEN)-1)<<GLB_CGEN_S1C_POS)
#define GLB_CGEN_S1C_UMSK                                       (~(((1U<<GLB_CGEN_S1C_LEN)-1)<<GLB_CGEN_S1C_POS))
#define GLB_CGEN_S1A0                                           GLB_CGEN_S1A0
#define GLB_CGEN_S1A0_POS                                       (16U)
#define GLB_CGEN_S1A0_LEN                                       (1U)
#define GLB_CGEN_S1A0_MSK                                       (((1U<<GLB_CGEN_S1A0_LEN)-1)<<GLB_CGEN_S1A0_POS)
#define GLB_CGEN_S1A0_UMSK                                      (~(((1U<<GLB_CGEN_S1A0_LEN)-1)<<GLB_CGEN_S1A0_POS))
#define GLB_CGEN_S1A2                                           GLB_CGEN_S1A2
#define GLB_CGEN_S1A2_POS                                       (18U)
#define GLB_CGEN_S1A2_LEN                                       (1U)
#define GLB_CGEN_S1A2_MSK                                       (((1U<<GLB_CGEN_S1A2_LEN)-1)<<GLB_CGEN_S1A2_POS)
#define GLB_CGEN_S1A2_UMSK                                      (~(((1U<<GLB_CGEN_S1A2_LEN)-1)<<GLB_CGEN_S1A2_POS))
#define GLB_CGEN_S1A3                                           GLB_CGEN_S1A3
#define GLB_CGEN_S1A3_POS                                       (19U)
#define GLB_CGEN_S1A3_LEN                                       (1U)
#define GLB_CGEN_S1A3_MSK                                       (((1U<<GLB_CGEN_S1A3_LEN)-1)<<GLB_CGEN_S1A3_POS)
#define GLB_CGEN_S1A3_UMSK                                      (~(((1U<<GLB_CGEN_S1A3_LEN)-1)<<GLB_CGEN_S1A3_POS))
#define GLB_CGEN_S1A4                                           GLB_CGEN_S1A4
#define GLB_CGEN_S1A4_POS                                       (20U)
#define GLB_CGEN_S1A4_LEN                                       (1U)
#define GLB_CGEN_S1A4_MSK                                       (((1U<<GLB_CGEN_S1A4_LEN)-1)<<GLB_CGEN_S1A4_POS)
#define GLB_CGEN_S1A4_UMSK                                      (~(((1U<<GLB_CGEN_S1A4_LEN)-1)<<GLB_CGEN_S1A4_POS))
#define GLB_CGEN_S1A5                                           GLB_CGEN_S1A5
#define GLB_CGEN_S1A5_POS                                       (21U)
#define GLB_CGEN_S1A5_LEN                                       (1U)
#define GLB_CGEN_S1A5_MSK                                       (((1U<<GLB_CGEN_S1A5_LEN)-1)<<GLB_CGEN_S1A5_POS)
#define GLB_CGEN_S1A5_UMSK                                      (~(((1U<<GLB_CGEN_S1A5_LEN)-1)<<GLB_CGEN_S1A5_POS))
#define GLB_CGEN_S1A6                                           GLB_CGEN_S1A6
#define GLB_CGEN_S1A6_POS                                       (22U)
#define GLB_CGEN_S1A6_LEN                                       (1U)
#define GLB_CGEN_S1A6_MSK                                       (((1U<<GLB_CGEN_S1A6_LEN)-1)<<GLB_CGEN_S1A6_POS)
#define GLB_CGEN_S1A6_UMSK                                      (~(((1U<<GLB_CGEN_S1A6_LEN)-1)<<GLB_CGEN_S1A6_POS))
#define GLB_CGEN_S1A7                                           GLB_CGEN_S1A7
#define GLB_CGEN_S1A7_POS                                       (23U)
#define GLB_CGEN_S1A7_LEN                                       (1U)
#define GLB_CGEN_S1A7_MSK                                       (((1U<<GLB_CGEN_S1A7_LEN)-1)<<GLB_CGEN_S1A7_POS)
#define GLB_CGEN_S1A7_UMSK                                      (~(((1U<<GLB_CGEN_S1A7_LEN)-1)<<GLB_CGEN_S1A7_POS))
#define GLB_CGEN_S1A9                                           GLB_CGEN_S1A9
#define GLB_CGEN_S1A9_POS                                       (25U)
#define GLB_CGEN_S1A9_LEN                                       (1U)
#define GLB_CGEN_S1A9_MSK                                       (((1U<<GLB_CGEN_S1A9_LEN)-1)<<GLB_CGEN_S1A9_POS)
#define GLB_CGEN_S1A9_UMSK                                      (~(((1U<<GLB_CGEN_S1A9_LEN)-1)<<GLB_CGEN_S1A9_POS))
#define GLB_CGEN_S1AD                                           GLB_CGEN_S1AD
#define GLB_CGEN_S1AD_POS                                       (29U)
#define GLB_CGEN_S1AD_LEN                                       (1U)
#define GLB_CGEN_S1AD_MSK                                       (((1U<<GLB_CGEN_S1AD_LEN)-1)<<GLB_CGEN_S1AD_POS)
#define GLB_CGEN_S1AD_UMSK                                      (~(((1U<<GLB_CGEN_S1AD_LEN)-1)<<GLB_CGEN_S1AD_POS))

/* 0x28 : cgen_cfg2 */
#define GLB_CGEN_CFG2_OFFSET                                    (0x28)
#define GLB_CGEN_S300                                           GLB_CGEN_S300
#define GLB_CGEN_S300_POS                                       (0U)
#define GLB_CGEN_S300_LEN                                       (1U)
#define GLB_CGEN_S300_MSK                                       (((1U<<GLB_CGEN_S300_LEN)-1)<<GLB_CGEN_S300_POS)
#define GLB_CGEN_S300_UMSK                                      (~(((1U<<GLB_CGEN_S300_LEN)-1)<<GLB_CGEN_S300_POS))
#define GLB_CGEN_S301                                           GLB_CGEN_S301
#define GLB_CGEN_S301_POS                                       (4U)
#define GLB_CGEN_S301_LEN                                       (1U)
#define GLB_CGEN_S301_MSK                                       (((1U<<GLB_CGEN_S301_LEN)-1)<<GLB_CGEN_S301_POS)
#define GLB_CGEN_S301_UMSK                                      (~(((1U<<GLB_CGEN_S301_LEN)-1)<<GLB_CGEN_S301_POS))

/* 0x2C : cgen_cfg3 */
#define GLB_CGEN_CFG3_OFFSET                                    (0x2C)

/* 0x30 : MBIST_CTL */
#define GLB_MBIST_CTL_OFFSET                                    (0x30)
#define GLB_IROM_MBIST_MODE                                     GLB_IROM_MBIST_MODE
#define GLB_IROM_MBIST_MODE_POS                                 (0U)
#define GLB_IROM_MBIST_MODE_LEN                                 (1U)
#define GLB_IROM_MBIST_MODE_MSK                                 (((1U<<GLB_IROM_MBIST_MODE_LEN)-1)<<GLB_IROM_MBIST_MODE_POS)
#define GLB_IROM_MBIST_MODE_UMSK                                (~(((1U<<GLB_IROM_MBIST_MODE_LEN)-1)<<GLB_IROM_MBIST_MODE_POS))
#define GLB_HSRAM_CACHE_MBIST_MODE                              GLB_HSRAM_CACHE_MBIST_MODE
#define GLB_HSRAM_CACHE_MBIST_MODE_POS                          (2U)
#define GLB_HSRAM_CACHE_MBIST_MODE_LEN                          (1U)
#define GLB_HSRAM_CACHE_MBIST_MODE_MSK                          (((1U<<GLB_HSRAM_CACHE_MBIST_MODE_LEN)-1)<<GLB_HSRAM_CACHE_MBIST_MODE_POS)
#define GLB_HSRAM_CACHE_MBIST_MODE_UMSK                         (~(((1U<<GLB_HSRAM_CACHE_MBIST_MODE_LEN)-1)<<GLB_HSRAM_CACHE_MBIST_MODE_POS))
#define GLB_TAG_MBIST_MODE                                      GLB_TAG_MBIST_MODE
#define GLB_TAG_MBIST_MODE_POS                                  (3U)
#define GLB_TAG_MBIST_MODE_LEN                                  (1U)
#define GLB_TAG_MBIST_MODE_MSK                                  (((1U<<GLB_TAG_MBIST_MODE_LEN)-1)<<GLB_TAG_MBIST_MODE_POS)
#define GLB_TAG_MBIST_MODE_UMSK                                 (~(((1U<<GLB_TAG_MBIST_MODE_LEN)-1)<<GLB_TAG_MBIST_MODE_POS))
#define GLB_OCRAM_MBIST_MODE                                    GLB_OCRAM_MBIST_MODE
#define GLB_OCRAM_MBIST_MODE_POS                                (4U)
#define GLB_OCRAM_MBIST_MODE_LEN                                (1U)
#define GLB_OCRAM_MBIST_MODE_MSK                                (((1U<<GLB_OCRAM_MBIST_MODE_LEN)-1)<<GLB_OCRAM_MBIST_MODE_POS)
#define GLB_OCRAM_MBIST_MODE_UMSK                               (~(((1U<<GLB_OCRAM_MBIST_MODE_LEN)-1)<<GLB_OCRAM_MBIST_MODE_POS))
#define GLB_EM_RAM_MBIST_MODE                                   GLB_EM_RAM_MBIST_MODE
#define GLB_EM_RAM_MBIST_MODE_POS                               (5U)
#define GLB_EM_RAM_MBIST_MODE_LEN                               (1U)
#define GLB_EM_RAM_MBIST_MODE_MSK                               (((1U<<GLB_EM_RAM_MBIST_MODE_LEN)-1)<<GLB_EM_RAM_MBIST_MODE_POS)
#define GLB_EM_RAM_MBIST_MODE_UMSK                              (~(((1U<<GLB_EM_RAM_MBIST_MODE_LEN)-1)<<GLB_EM_RAM_MBIST_MODE_POS))
#define GLB_M154_MBIST_MODE                                     GLB_M154_MBIST_MODE
#define GLB_M154_MBIST_MODE_POS                                 (6U)
#define GLB_M154_MBIST_MODE_LEN                                 (1U)
#define GLB_M154_MBIST_MODE_MSK                                 (((1U<<GLB_M154_MBIST_MODE_LEN)-1)<<GLB_M154_MBIST_MODE_POS)
#define GLB_M154_MBIST_MODE_UMSK                                (~(((1U<<GLB_M154_MBIST_MODE_LEN)-1)<<GLB_M154_MBIST_MODE_POS))
#define GLB_SF_MBIST_MODE                                       GLB_SF_MBIST_MODE
#define GLB_SF_MBIST_MODE_POS                                   (7U)
#define GLB_SF_MBIST_MODE_LEN                                   (1U)
#define GLB_SF_MBIST_MODE_MSK                                   (((1U<<GLB_SF_MBIST_MODE_LEN)-1)<<GLB_SF_MBIST_MODE_POS)
#define GLB_SF_MBIST_MODE_UMSK                                  (~(((1U<<GLB_SF_MBIST_MODE_LEN)-1)<<GLB_SF_MBIST_MODE_POS))
#define GLB_SEC_MBIST_MODE                                      GLB_SEC_MBIST_MODE
#define GLB_SEC_MBIST_MODE_POS                                  (8U)
#define GLB_SEC_MBIST_MODE_LEN                                  (1U)
#define GLB_SEC_MBIST_MODE_MSK                                  (((1U<<GLB_SEC_MBIST_MODE_LEN)-1)<<GLB_SEC_MBIST_MODE_POS)
#define GLB_SEC_MBIST_MODE_UMSK                                 (~(((1U<<GLB_SEC_MBIST_MODE_LEN)-1)<<GLB_SEC_MBIST_MODE_POS))
#define GLB_REG_MBIST_RST_N                                     GLB_REG_MBIST_RST_N
#define GLB_REG_MBIST_RST_N_POS                                 (31U)
#define GLB_REG_MBIST_RST_N_LEN                                 (1U)
#define GLB_REG_MBIST_RST_N_MSK                                 (((1U<<GLB_REG_MBIST_RST_N_LEN)-1)<<GLB_REG_MBIST_RST_N_POS)
#define GLB_REG_MBIST_RST_N_UMSK                                (~(((1U<<GLB_REG_MBIST_RST_N_LEN)-1)<<GLB_REG_MBIST_RST_N_POS))

/* 0x34 : MBIST_STAT */
#define GLB_MBIST_STAT_OFFSET                                   (0x34)
#define GLB_IROM_MBIST_DONE                                     GLB_IROM_MBIST_DONE
#define GLB_IROM_MBIST_DONE_POS                                 (0U)
#define GLB_IROM_MBIST_DONE_LEN                                 (1U)
#define GLB_IROM_MBIST_DONE_MSK                                 (((1U<<GLB_IROM_MBIST_DONE_LEN)-1)<<GLB_IROM_MBIST_DONE_POS)
#define GLB_IROM_MBIST_DONE_UMSK                                (~(((1U<<GLB_IROM_MBIST_DONE_LEN)-1)<<GLB_IROM_MBIST_DONE_POS))
#define GLB_HSRAM_CACHE_MBIST_DONE                              GLB_HSRAM_CACHE_MBIST_DONE
#define GLB_HSRAM_CACHE_MBIST_DONE_POS                          (2U)
#define GLB_HSRAM_CACHE_MBIST_DONE_LEN                          (1U)
#define GLB_HSRAM_CACHE_MBIST_DONE_MSK                          (((1U<<GLB_HSRAM_CACHE_MBIST_DONE_LEN)-1)<<GLB_HSRAM_CACHE_MBIST_DONE_POS)
#define GLB_HSRAM_CACHE_MBIST_DONE_UMSK                         (~(((1U<<GLB_HSRAM_CACHE_MBIST_DONE_LEN)-1)<<GLB_HSRAM_CACHE_MBIST_DONE_POS))
#define GLB_TAG_MBIST_DONE                                      GLB_TAG_MBIST_DONE
#define GLB_TAG_MBIST_DONE_POS                                  (3U)
#define GLB_TAG_MBIST_DONE_LEN                                  (1U)
#define GLB_TAG_MBIST_DONE_MSK                                  (((1U<<GLB_TAG_MBIST_DONE_LEN)-1)<<GLB_TAG_MBIST_DONE_POS)
#define GLB_TAG_MBIST_DONE_UMSK                                 (~(((1U<<GLB_TAG_MBIST_DONE_LEN)-1)<<GLB_TAG_MBIST_DONE_POS))
#define GLB_OCRAM_MBIST_DONE                                    GLB_OCRAM_MBIST_DONE
#define GLB_OCRAM_MBIST_DONE_POS                                (4U)
#define GLB_OCRAM_MBIST_DONE_LEN                                (1U)
#define GLB_OCRAM_MBIST_DONE_MSK                                (((1U<<GLB_OCRAM_MBIST_DONE_LEN)-1)<<GLB_OCRAM_MBIST_DONE_POS)
#define GLB_OCRAM_MBIST_DONE_UMSK                               (~(((1U<<GLB_OCRAM_MBIST_DONE_LEN)-1)<<GLB_OCRAM_MBIST_DONE_POS))
#define GLB_EM_RAM_MBIST_DONE                                   GLB_EM_RAM_MBIST_DONE
#define GLB_EM_RAM_MBIST_DONE_POS                               (5U)
#define GLB_EM_RAM_MBIST_DONE_LEN                               (1U)
#define GLB_EM_RAM_MBIST_DONE_MSK                               (((1U<<GLB_EM_RAM_MBIST_DONE_LEN)-1)<<GLB_EM_RAM_MBIST_DONE_POS)
#define GLB_EM_RAM_MBIST_DONE_UMSK                              (~(((1U<<GLB_EM_RAM_MBIST_DONE_LEN)-1)<<GLB_EM_RAM_MBIST_DONE_POS))
#define GLB_M154_MBIST_DONE                                     GLB_M154_MBIST_DONE
#define GLB_M154_MBIST_DONE_POS                                 (6U)
#define GLB_M154_MBIST_DONE_LEN                                 (1U)
#define GLB_M154_MBIST_DONE_MSK                                 (((1U<<GLB_M154_MBIST_DONE_LEN)-1)<<GLB_M154_MBIST_DONE_POS)
#define GLB_M154_MBIST_DONE_UMSK                                (~(((1U<<GLB_M154_MBIST_DONE_LEN)-1)<<GLB_M154_MBIST_DONE_POS))
#define GLB_SF_MBIST_DONE                                       GLB_SF_MBIST_DONE
#define GLB_SF_MBIST_DONE_POS                                   (7U)
#define GLB_SF_MBIST_DONE_LEN                                   (1U)
#define GLB_SF_MBIST_DONE_MSK                                   (((1U<<GLB_SF_MBIST_DONE_LEN)-1)<<GLB_SF_MBIST_DONE_POS)
#define GLB_SF_MBIST_DONE_UMSK                                  (~(((1U<<GLB_SF_MBIST_DONE_LEN)-1)<<GLB_SF_MBIST_DONE_POS))
#define GLB_SEC_MBIST_DONE                                      GLB_SEC_MBIST_DONE
#define GLB_SEC_MBIST_DONE_POS                                  (8U)
#define GLB_SEC_MBIST_DONE_LEN                                  (1U)
#define GLB_SEC_MBIST_DONE_MSK                                  (((1U<<GLB_SEC_MBIST_DONE_LEN)-1)<<GLB_SEC_MBIST_DONE_POS)
#define GLB_SEC_MBIST_DONE_UMSK                                 (~(((1U<<GLB_SEC_MBIST_DONE_LEN)-1)<<GLB_SEC_MBIST_DONE_POS))
#define GLB_IROM_MBIST_FAIL                                     GLB_IROM_MBIST_FAIL
#define GLB_IROM_MBIST_FAIL_POS                                 (16U)
#define GLB_IROM_MBIST_FAIL_LEN                                 (1U)
#define GLB_IROM_MBIST_FAIL_MSK                                 (((1U<<GLB_IROM_MBIST_FAIL_LEN)-1)<<GLB_IROM_MBIST_FAIL_POS)
#define GLB_IROM_MBIST_FAIL_UMSK                                (~(((1U<<GLB_IROM_MBIST_FAIL_LEN)-1)<<GLB_IROM_MBIST_FAIL_POS))
#define GLB_HSRAM_CACHE_MBIST_FAIL                              GLB_HSRAM_CACHE_MBIST_FAIL
#define GLB_HSRAM_CACHE_MBIST_FAIL_POS                          (18U)
#define GLB_HSRAM_CACHE_MBIST_FAIL_LEN                          (1U)
#define GLB_HSRAM_CACHE_MBIST_FAIL_MSK                          (((1U<<GLB_HSRAM_CACHE_MBIST_FAIL_LEN)-1)<<GLB_HSRAM_CACHE_MBIST_FAIL_POS)
#define GLB_HSRAM_CACHE_MBIST_FAIL_UMSK                         (~(((1U<<GLB_HSRAM_CACHE_MBIST_FAIL_LEN)-1)<<GLB_HSRAM_CACHE_MBIST_FAIL_POS))
#define GLB_TAG_MBIST_FAIL                                      GLB_TAG_MBIST_FAIL
#define GLB_TAG_MBIST_FAIL_POS                                  (19U)
#define GLB_TAG_MBIST_FAIL_LEN                                  (1U)
#define GLB_TAG_MBIST_FAIL_MSK                                  (((1U<<GLB_TAG_MBIST_FAIL_LEN)-1)<<GLB_TAG_MBIST_FAIL_POS)
#define GLB_TAG_MBIST_FAIL_UMSK                                 (~(((1U<<GLB_TAG_MBIST_FAIL_LEN)-1)<<GLB_TAG_MBIST_FAIL_POS))
#define GLB_OCRAM_MBIST_FAIL                                    GLB_OCRAM_MBIST_FAIL
#define GLB_OCRAM_MBIST_FAIL_POS                                (20U)
#define GLB_OCRAM_MBIST_FAIL_LEN                                (1U)
#define GLB_OCRAM_MBIST_FAIL_MSK                                (((1U<<GLB_OCRAM_MBIST_FAIL_LEN)-1)<<GLB_OCRAM_MBIST_FAIL_POS)
#define GLB_OCRAM_MBIST_FAIL_UMSK                               (~(((1U<<GLB_OCRAM_MBIST_FAIL_LEN)-1)<<GLB_OCRAM_MBIST_FAIL_POS))
#define GLB_EM_RAM_MBIST_FAIL                                   GLB_EM_RAM_MBIST_FAIL
#define GLB_EM_RAM_MBIST_FAIL_POS                               (21U)
#define GLB_EM_RAM_MBIST_FAIL_LEN                               (1U)
#define GLB_EM_RAM_MBIST_FAIL_MSK                               (((1U<<GLB_EM_RAM_MBIST_FAIL_LEN)-1)<<GLB_EM_RAM_MBIST_FAIL_POS)
#define GLB_EM_RAM_MBIST_FAIL_UMSK                              (~(((1U<<GLB_EM_RAM_MBIST_FAIL_LEN)-1)<<GLB_EM_RAM_MBIST_FAIL_POS))
#define GLB_M154_MBIST_FAIL                                     GLB_M154_MBIST_FAIL
#define GLB_M154_MBIST_FAIL_POS                                 (22U)
#define GLB_M154_MBIST_FAIL_LEN                                 (1U)
#define GLB_M154_MBIST_FAIL_MSK                                 (((1U<<GLB_M154_MBIST_FAIL_LEN)-1)<<GLB_M154_MBIST_FAIL_POS)
#define GLB_M154_MBIST_FAIL_UMSK                                (~(((1U<<GLB_M154_MBIST_FAIL_LEN)-1)<<GLB_M154_MBIST_FAIL_POS))
#define GLB_SF_MBIST_FAIL                                       GLB_SF_MBIST_FAIL
#define GLB_SF_MBIST_FAIL_POS                                   (23U)
#define GLB_SF_MBIST_FAIL_LEN                                   (1U)
#define GLB_SF_MBIST_FAIL_MSK                                   (((1U<<GLB_SF_MBIST_FAIL_LEN)-1)<<GLB_SF_MBIST_FAIL_POS)
#define GLB_SF_MBIST_FAIL_UMSK                                  (~(((1U<<GLB_SF_MBIST_FAIL_LEN)-1)<<GLB_SF_MBIST_FAIL_POS))
#define GLB_SEC_MBIST_FAIL                                      GLB_SEC_MBIST_FAIL
#define GLB_SEC_MBIST_FAIL_POS                                  (24U)
#define GLB_SEC_MBIST_FAIL_LEN                                  (1U)
#define GLB_SEC_MBIST_FAIL_MSK                                  (((1U<<GLB_SEC_MBIST_FAIL_LEN)-1)<<GLB_SEC_MBIST_FAIL_POS)
#define GLB_SEC_MBIST_FAIL_UMSK                                 (~(((1U<<GLB_SEC_MBIST_FAIL_LEN)-1)<<GLB_SEC_MBIST_FAIL_POS))

/* 0x40 : NEW_RTC_TIME_L */
#define GLB_NEW_RTC_TIME_L_OFFSET                               (0x40)
#define GLB_RTC_TIMER_LATCH_L                                   GLB_RTC_TIMER_LATCH_L
#define GLB_RTC_TIMER_LATCH_L_POS                               (0U)
#define GLB_RTC_TIMER_LATCH_L_LEN                               (32U)
#define GLB_RTC_TIMER_LATCH_L_MSK                               (((1U<<GLB_RTC_TIMER_LATCH_L_LEN)-1)<<GLB_RTC_TIMER_LATCH_L_POS)
#define GLB_RTC_TIMER_LATCH_L_UMSK                              (~(((1U<<GLB_RTC_TIMER_LATCH_L_LEN)-1)<<GLB_RTC_TIMER_LATCH_L_POS))

/* 0x44 : NEW_RTC_TIME_H */
#define GLB_NEW_RTC_TIME_H_OFFSET                               (0x44)
#define GLB_RTC_TIMER_LATCH_H                                   GLB_RTC_TIMER_LATCH_H
#define GLB_RTC_TIMER_LATCH_H_POS                               (0U)
#define GLB_RTC_TIMER_LATCH_H_LEN                               (8U)
#define GLB_RTC_TIMER_LATCH_H_MSK                               (((1U<<GLB_RTC_TIMER_LATCH_H_LEN)-1)<<GLB_RTC_TIMER_LATCH_H_POS)
#define GLB_RTC_TIMER_LATCH_H_UMSK                              (~(((1U<<GLB_RTC_TIMER_LATCH_H_LEN)-1)<<GLB_RTC_TIMER_LATCH_H_POS))
#define GLB_REG_RTC_TIMER_LATCH_EN                              GLB_REG_RTC_TIMER_LATCH_EN
#define GLB_REG_RTC_TIMER_LATCH_EN_POS                          (24U)
#define GLB_REG_RTC_TIMER_LATCH_EN_LEN                          (1U)
#define GLB_REG_RTC_TIMER_LATCH_EN_MSK                          (((1U<<GLB_REG_RTC_TIMER_LATCH_EN_LEN)-1)<<GLB_REG_RTC_TIMER_LATCH_EN_POS)
#define GLB_REG_RTC_TIMER_LATCH_EN_UMSK                         (~(((1U<<GLB_REG_RTC_TIMER_LATCH_EN_LEN)-1)<<GLB_REG_RTC_TIMER_LATCH_EN_POS))

/* 0x4C : XTAL_DEG_32K */
#define GLB_XTAL_DEG_32K_OFFSET                                 (0x4C)
#define GLB_XTAL_DEG_CNT_LIMIT                                  GLB_XTAL_DEG_CNT_LIMIT
#define GLB_XTAL_DEG_CNT_LIMIT_POS                              (0U)
#define GLB_XTAL_DEG_CNT_LIMIT_LEN                              (8U)
#define GLB_XTAL_DEG_CNT_LIMIT_MSK                              (((1U<<GLB_XTAL_DEG_CNT_LIMIT_LEN)-1)<<GLB_XTAL_DEG_CNT_LIMIT_POS)
#define GLB_XTAL_DEG_CNT_LIMIT_UMSK                             (~(((1U<<GLB_XTAL_DEG_CNT_LIMIT_LEN)-1)<<GLB_XTAL_DEG_CNT_LIMIT_POS))
#define GLB_REG_RC32K_DEG_EN                                    GLB_REG_RC32K_DEG_EN
#define GLB_REG_RC32K_DEG_EN_POS                                (13U)
#define GLB_REG_RC32K_DEG_EN_LEN                                (1U)
#define GLB_REG_RC32K_DEG_EN_MSK                                (((1U<<GLB_REG_RC32K_DEG_EN_LEN)-1)<<GLB_REG_RC32K_DEG_EN_POS)
#define GLB_REG_RC32K_DEG_EN_UMSK                               (~(((1U<<GLB_REG_RC32K_DEG_EN_LEN)-1)<<GLB_REG_RC32K_DEG_EN_POS))
#define GLB_RC32K_DEG_END_PS                                    GLB_RC32K_DEG_END_PS
#define GLB_RC32K_DEG_END_PS_POS                                (14U)
#define GLB_RC32K_DEG_END_PS_LEN                                (1U)
#define GLB_RC32K_DEG_END_PS_MSK                                (((1U<<GLB_RC32K_DEG_END_PS_LEN)-1)<<GLB_RC32K_DEG_END_PS_POS)
#define GLB_RC32K_DEG_END_PS_UMSK                               (~(((1U<<GLB_RC32K_DEG_END_PS_LEN)-1)<<GLB_RC32K_DEG_END_PS_POS))
#define GLB_RC32K_DEG_START_PS                                  GLB_RC32K_DEG_START_PS
#define GLB_RC32K_DEG_START_PS_POS                              (15U)
#define GLB_RC32K_DEG_START_PS_LEN                              (1U)
#define GLB_RC32K_DEG_START_PS_MSK                              (((1U<<GLB_RC32K_DEG_START_PS_LEN)-1)<<GLB_RC32K_DEG_START_PS_POS)
#define GLB_RC32K_DEG_START_PS_UMSK                             (~(((1U<<GLB_RC32K_DEG_START_PS_LEN)-1)<<GLB_RC32K_DEG_START_PS_POS))
#define GLB_XTAL_CNT_32K_CGEN                                   GLB_XTAL_CNT_32K_CGEN
#define GLB_XTAL_CNT_32K_CGEN_POS                               (27U)
#define GLB_XTAL_CNT_32K_CGEN_LEN                               (1U)
#define GLB_XTAL_CNT_32K_CGEN_MSK                               (((1U<<GLB_XTAL_CNT_32K_CGEN_LEN)-1)<<GLB_XTAL_CNT_32K_CGEN_POS)
#define GLB_XTAL_CNT_32K_CGEN_UMSK                              (~(((1U<<GLB_XTAL_CNT_32K_CGEN_LEN)-1)<<GLB_XTAL_CNT_32K_CGEN_POS))
#define GLB_CLR_XTAL_CNT_32K_DONE                               GLB_CLR_XTAL_CNT_32K_DONE
#define GLB_CLR_XTAL_CNT_32K_DONE_POS                           (28U)
#define GLB_CLR_XTAL_CNT_32K_DONE_LEN                           (1U)
#define GLB_CLR_XTAL_CNT_32K_DONE_MSK                           (((1U<<GLB_CLR_XTAL_CNT_32K_DONE_LEN)-1)<<GLB_CLR_XTAL_CNT_32K_DONE_POS)
#define GLB_CLR_XTAL_CNT_32K_DONE_UMSK                          (~(((1U<<GLB_CLR_XTAL_CNT_32K_DONE_LEN)-1)<<GLB_CLR_XTAL_CNT_32K_DONE_POS))
#define GLB_XTAL_CNT_32K_SW_TRIG_PS                             GLB_XTAL_CNT_32K_SW_TRIG_PS
#define GLB_XTAL_CNT_32K_SW_TRIG_PS_POS                         (31U)
#define GLB_XTAL_CNT_32K_SW_TRIG_PS_LEN                         (1U)
#define GLB_XTAL_CNT_32K_SW_TRIG_PS_MSK                         (((1U<<GLB_XTAL_CNT_32K_SW_TRIG_PS_LEN)-1)<<GLB_XTAL_CNT_32K_SW_TRIG_PS_POS)
#define GLB_XTAL_CNT_32K_SW_TRIG_PS_UMSK                        (~(((1U<<GLB_XTAL_CNT_32K_SW_TRIG_PS_LEN)-1)<<GLB_XTAL_CNT_32K_SW_TRIG_PS_POS))

/* 0x50 : bmx_cfg1 */
#define GLB_BMX_CFG1_OFFSET                                     (0x50)
#define GLB_BMX_TIMEOUT_EN                                      GLB_BMX_TIMEOUT_EN
#define GLB_BMX_TIMEOUT_EN_POS                                  (0U)
#define GLB_BMX_TIMEOUT_EN_LEN                                  (4U)
#define GLB_BMX_TIMEOUT_EN_MSK                                  (((1U<<GLB_BMX_TIMEOUT_EN_LEN)-1)<<GLB_BMX_TIMEOUT_EN_POS)
#define GLB_BMX_TIMEOUT_EN_UMSK                                 (~(((1U<<GLB_BMX_TIMEOUT_EN_LEN)-1)<<GLB_BMX_TIMEOUT_EN_POS))
#define GLB_BMX_ARB_MODE                                        GLB_BMX_ARB_MODE
#define GLB_BMX_ARB_MODE_POS                                    (4U)
#define GLB_BMX_ARB_MODE_LEN                                    (2U)
#define GLB_BMX_ARB_MODE_MSK                                    (((1U<<GLB_BMX_ARB_MODE_LEN)-1)<<GLB_BMX_ARB_MODE_POS)
#define GLB_BMX_ARB_MODE_UMSK                                   (~(((1U<<GLB_BMX_ARB_MODE_LEN)-1)<<GLB_BMX_ARB_MODE_POS))
#define GLB_BMX_ERR_EN                                          GLB_BMX_ERR_EN
#define GLB_BMX_ERR_EN_POS                                      (8U)
#define GLB_BMX_ERR_EN_LEN                                      (1U)
#define GLB_BMX_ERR_EN_MSK                                      (((1U<<GLB_BMX_ERR_EN_LEN)-1)<<GLB_BMX_ERR_EN_POS)
#define GLB_BMX_ERR_EN_UMSK                                     (~(((1U<<GLB_BMX_ERR_EN_LEN)-1)<<GLB_BMX_ERR_EN_POS))
#define GLB_BMX_BUSY_OPTION_DIS                                 GLB_BMX_BUSY_OPTION_DIS
#define GLB_BMX_BUSY_OPTION_DIS_POS                             (9U)
#define GLB_BMX_BUSY_OPTION_DIS_LEN                             (1U)
#define GLB_BMX_BUSY_OPTION_DIS_MSK                             (((1U<<GLB_BMX_BUSY_OPTION_DIS_LEN)-1)<<GLB_BMX_BUSY_OPTION_DIS_POS)
#define GLB_BMX_BUSY_OPTION_DIS_UMSK                            (~(((1U<<GLB_BMX_BUSY_OPTION_DIS_LEN)-1)<<GLB_BMX_BUSY_OPTION_DIS_POS))
#define GLB_BMX_GATING_DIS                                      GLB_BMX_GATING_DIS
#define GLB_BMX_GATING_DIS_POS                                  (10U)
#define GLB_BMX_GATING_DIS_LEN                                  (1U)
#define GLB_BMX_GATING_DIS_MSK                                  (((1U<<GLB_BMX_GATING_DIS_LEN)-1)<<GLB_BMX_GATING_DIS_POS)
#define GLB_BMX_GATING_DIS_UMSK                                 (~(((1U<<GLB_BMX_GATING_DIS_LEN)-1)<<GLB_BMX_GATING_DIS_POS))
#define GLB_HSEL_OPTION                                         GLB_HSEL_OPTION
#define GLB_HSEL_OPTION_POS                                     (12U)
#define GLB_HSEL_OPTION_LEN                                     (4U)
#define GLB_HSEL_OPTION_MSK                                     (((1U<<GLB_HSEL_OPTION_LEN)-1)<<GLB_HSEL_OPTION_POS)
#define GLB_HSEL_OPTION_UMSK                                    (~(((1U<<GLB_HSEL_OPTION_LEN)-1)<<GLB_HSEL_OPTION_POS))
#define GLB_HBN_APB_CFG                                         GLB_HBN_APB_CFG
#define GLB_HBN_APB_CFG_POS                                     (24U)
#define GLB_HBN_APB_CFG_LEN                                     (8U)
#define GLB_HBN_APB_CFG_MSK                                     (((1U<<GLB_HBN_APB_CFG_LEN)-1)<<GLB_HBN_APB_CFG_POS)
#define GLB_HBN_APB_CFG_UMSK                                    (~(((1U<<GLB_HBN_APB_CFG_LEN)-1)<<GLB_HBN_APB_CFG_POS))

/* 0x54 : bmx_cfg2 */
#define GLB_BMX_CFG2_OFFSET                                     (0x54)
#define GLB_BMX_ERR_ADDR_DIS                                    GLB_BMX_ERR_ADDR_DIS
#define GLB_BMX_ERR_ADDR_DIS_POS                                (0U)
#define GLB_BMX_ERR_ADDR_DIS_LEN                                (1U)
#define GLB_BMX_ERR_ADDR_DIS_MSK                                (((1U<<GLB_BMX_ERR_ADDR_DIS_LEN)-1)<<GLB_BMX_ERR_ADDR_DIS_POS)
#define GLB_BMX_ERR_ADDR_DIS_UMSK                               (~(((1U<<GLB_BMX_ERR_ADDR_DIS_LEN)-1)<<GLB_BMX_ERR_ADDR_DIS_POS))
#define GLB_BMX_ERR_CLR                                         GLB_BMX_ERR_CLR
#define GLB_BMX_ERR_CLR_POS                                     (1U)
#define GLB_BMX_ERR_CLR_LEN                                     (1U)
#define GLB_BMX_ERR_CLR_MSK                                     (((1U<<GLB_BMX_ERR_CLR_LEN)-1)<<GLB_BMX_ERR_CLR_POS)
#define GLB_BMX_ERR_CLR_UMSK                                    (~(((1U<<GLB_BMX_ERR_CLR_LEN)-1)<<GLB_BMX_ERR_CLR_POS))
#define GLB_BMX_ERR_DEC                                         GLB_BMX_ERR_DEC
#define GLB_BMX_ERR_DEC_POS                                     (4U)
#define GLB_BMX_ERR_DEC_LEN                                     (1U)
#define GLB_BMX_ERR_DEC_MSK                                     (((1U<<GLB_BMX_ERR_DEC_LEN)-1)<<GLB_BMX_ERR_DEC_POS)
#define GLB_BMX_ERR_DEC_UMSK                                    (~(((1U<<GLB_BMX_ERR_DEC_LEN)-1)<<GLB_BMX_ERR_DEC_POS))
#define GLB_BMX_ERR_TZ                                          GLB_BMX_ERR_TZ
#define GLB_BMX_ERR_TZ_POS                                      (5U)
#define GLB_BMX_ERR_TZ_LEN                                      (1U)
#define GLB_BMX_ERR_TZ_MSK                                      (((1U<<GLB_BMX_ERR_TZ_LEN)-1)<<GLB_BMX_ERR_TZ_POS)
#define GLB_BMX_ERR_TZ_UMSK                                     (~(((1U<<GLB_BMX_ERR_TZ_LEN)-1)<<GLB_BMX_ERR_TZ_POS))
#define GLB_REG_W_THRE_BMX                                      GLB_REG_W_THRE_BMX
#define GLB_REG_W_THRE_BMX_POS                                  (8U)
#define GLB_REG_W_THRE_BMX_LEN                                  (2U)
#define GLB_REG_W_THRE_BMX_MSK                                  (((1U<<GLB_REG_W_THRE_BMX_LEN)-1)<<GLB_REG_W_THRE_BMX_POS)
#define GLB_REG_W_THRE_BMX_UMSK                                 (~(((1U<<GLB_REG_W_THRE_BMX_LEN)-1)<<GLB_REG_W_THRE_BMX_POS))
#define GLB_REG_W_THRE_L1C                                      GLB_REG_W_THRE_L1C
#define GLB_REG_W_THRE_L1C_POS                                  (10U)
#define GLB_REG_W_THRE_L1C_LEN                                  (2U)
#define GLB_REG_W_THRE_L1C_MSK                                  (((1U<<GLB_REG_W_THRE_L1C_LEN)-1)<<GLB_REG_W_THRE_L1C_POS)
#define GLB_REG_W_THRE_L1C_UMSK                                 (~(((1U<<GLB_REG_W_THRE_L1C_LEN)-1)<<GLB_REG_W_THRE_L1C_POS))
#define GLB_BMX_DBG_SEL                                         GLB_BMX_DBG_SEL
#define GLB_BMX_DBG_SEL_POS                                     (28U)
#define GLB_BMX_DBG_SEL_LEN                                     (4U)
#define GLB_BMX_DBG_SEL_MSK                                     (((1U<<GLB_BMX_DBG_SEL_LEN)-1)<<GLB_BMX_DBG_SEL_POS)
#define GLB_BMX_DBG_SEL_UMSK                                    (~(((1U<<GLB_BMX_DBG_SEL_LEN)-1)<<GLB_BMX_DBG_SEL_POS))

/* 0x58 : bmx_err_addr */
#define GLB_BMX_ERR_ADDR_OFFSET                                 (0x58)
#define GLB_BMX_ERR_ADDR                                        GLB_BMX_ERR_ADDR
#define GLB_BMX_ERR_ADDR_POS                                    (0U)
#define GLB_BMX_ERR_ADDR_LEN                                    (32U)
#define GLB_BMX_ERR_ADDR_MSK                                    (((1U<<GLB_BMX_ERR_ADDR_LEN)-1)<<GLB_BMX_ERR_ADDR_POS)
#define GLB_BMX_ERR_ADDR_UMSK                                   (~(((1U<<GLB_BMX_ERR_ADDR_LEN)-1)<<GLB_BMX_ERR_ADDR_POS))

/* 0x5C : bmx_dbg_out */
#define GLB_BMX_DBG_OUT_OFFSET                                  (0x5C)
#define GLB_BMX_DBG_OUT                                         GLB_BMX_DBG_OUT
#define GLB_BMX_DBG_OUT_POS                                     (0U)
#define GLB_BMX_DBG_OUT_LEN                                     (32U)
#define GLB_BMX_DBG_OUT_MSK                                     (((1U<<GLB_BMX_DBG_OUT_LEN)-1)<<GLB_BMX_DBG_OUT_POS)
#define GLB_BMX_DBG_OUT_UMSK                                    (~(((1U<<GLB_BMX_DBG_OUT_LEN)-1)<<GLB_BMX_DBG_OUT_POS))

/* 0x60 : rsv0 */
#define GLB_RSV0_OFFSET                                         (0x60)

/* 0x64 : rsv1 */
#define GLB_RSV1_OFFSET                                         (0x64)

/* 0x68 : rsv2 */
#define GLB_RSV2_OFFSET                                         (0x68)

/* 0x6C : rsv3 */
#define GLB_RSV3_OFFSET                                         (0x6C)

/* 0x78 : sram_parm */
#define GLB_SRAM_PARM_OFFSET                                    (0x78)
#define GLB_REG_SRAM_PARM                                       GLB_REG_SRAM_PARM
#define GLB_REG_SRAM_PARM_POS                                   (0U)
#define GLB_REG_SRAM_PARM_LEN                                   (18U)
#define GLB_REG_SRAM_PARM_MSK                                   (((1U<<GLB_REG_SRAM_PARM_LEN)-1)<<GLB_REG_SRAM_PARM_POS)
#define GLB_REG_SRAM_PARM_UMSK                                  (~(((1U<<GLB_REG_SRAM_PARM_LEN)-1)<<GLB_REG_SRAM_PARM_POS))
#define GLB_REG_OCRAM_PARM                                      GLB_REG_OCRAM_PARM
#define GLB_REG_OCRAM_PARM_POS                                  (20U)
#define GLB_REG_OCRAM_PARM_LEN                                  (11U)
#define GLB_REG_OCRAM_PARM_MSK                                  (((1U<<GLB_REG_OCRAM_PARM_LEN)-1)<<GLB_REG_OCRAM_PARM_POS)
#define GLB_REG_OCRAM_PARM_UMSK                                 (~(((1U<<GLB_REG_OCRAM_PARM_LEN)-1)<<GLB_REG_OCRAM_PARM_POS))

/* 0x7C : seam_misc */
#define GLB_SEAM_MISC_OFFSET                                    (0x7C)
#define GLB_EM_SEL                                              GLB_EM_SEL
#define GLB_EM_SEL_POS                                          (0U)
#define GLB_EM_SEL_LEN                                          (4U)
#define GLB_EM_SEL_MSK                                          (((1U<<GLB_EM_SEL_LEN)-1)<<GLB_EM_SEL_POS)
#define GLB_EM_SEL_UMSK                                         (~(((1U<<GLB_EM_SEL_LEN)-1)<<GLB_EM_SEL_POS))

/* 0x80 : glb_parm */
#define GLB_PARM_OFFSET                                         (0x80)
#define GLB_JTAG_SWAP_SET                                       GLB_JTAG_SWAP_SET
#define GLB_JTAG_SWAP_SET_POS                                   (0U)
#define GLB_JTAG_SWAP_SET_LEN                                   (8U)
#define GLB_JTAG_SWAP_SET_MSK                                   (((1U<<GLB_JTAG_SWAP_SET_LEN)-1)<<GLB_JTAG_SWAP_SET_POS)
#define GLB_JTAG_SWAP_SET_UMSK                                  (~(((1U<<GLB_JTAG_SWAP_SET_LEN)-1)<<GLB_JTAG_SWAP_SET_POS))
#define GLB_CFG_SFLASH_SWAP_IO0_IO3                             GLB_CFG_SFLASH_SWAP_IO0_IO3
#define GLB_CFG_SFLASH_SWAP_IO0_IO3_POS                         (8U)
#define GLB_CFG_SFLASH_SWAP_IO0_IO3_LEN                         (1U)
#define GLB_CFG_SFLASH_SWAP_IO0_IO3_MSK                         (((1U<<GLB_CFG_SFLASH_SWAP_IO0_IO3_LEN)-1)<<GLB_CFG_SFLASH_SWAP_IO0_IO3_POS)
#define GLB_CFG_SFLASH_SWAP_IO0_IO3_UMSK                        (~(((1U<<GLB_CFG_SFLASH_SWAP_IO0_IO3_LEN)-1)<<GLB_CFG_SFLASH_SWAP_IO0_IO3_POS))
#define GLB_CFG_SFLASH_SWAP_CS_IO2                              GLB_CFG_SFLASH_SWAP_CS_IO2
#define GLB_CFG_SFLASH_SWAP_CS_IO2_POS                          (9U)
#define GLB_CFG_SFLASH_SWAP_CS_IO2_LEN                          (1U)
#define GLB_CFG_SFLASH_SWAP_CS_IO2_MSK                          (((1U<<GLB_CFG_SFLASH_SWAP_CS_IO2_LEN)-1)<<GLB_CFG_SFLASH_SWAP_CS_IO2_POS)
#define GLB_CFG_SFLASH_SWAP_CS_IO2_UMSK                         (~(((1U<<GLB_CFG_SFLASH_SWAP_CS_IO2_LEN)-1)<<GLB_CFG_SFLASH_SWAP_CS_IO2_POS))
#define GLB_CFG_SFLASH_REVERSE                                  GLB_CFG_SFLASH_REVERSE
#define GLB_CFG_SFLASH_REVERSE_POS                              (10U)
#define GLB_CFG_SFLASH_REVERSE_LEN                              (1U)
#define GLB_CFG_SFLASH_REVERSE_MSK                              (((1U<<GLB_CFG_SFLASH_REVERSE_LEN)-1)<<GLB_CFG_SFLASH_REVERSE_POS)
#define GLB_CFG_SFLASH_REVERSE_UMSK                             (~(((1U<<GLB_CFG_SFLASH_REVERSE_LEN)-1)<<GLB_CFG_SFLASH_REVERSE_POS))
#define GLB_CFG_FLASH2_SCENARIO                                 GLB_CFG_FLASH2_SCENARIO
#define GLB_CFG_FLASH2_SCENARIO_POS                             (11U)
#define GLB_CFG_FLASH2_SCENARIO_LEN                             (1U)
#define GLB_CFG_FLASH2_SCENARIO_MSK                             (((1U<<GLB_CFG_FLASH2_SCENARIO_LEN)-1)<<GLB_CFG_FLASH2_SCENARIO_POS)
#define GLB_CFG_FLASH2_SCENARIO_UMSK                            (~(((1U<<GLB_CFG_FLASH2_SCENARIO_LEN)-1)<<GLB_CFG_FLASH2_SCENARIO_POS))
#define GLB_REG_SPI_0_MASTER_MODE                               GLB_REG_SPI_0_MASTER_MODE
#define GLB_REG_SPI_0_MASTER_MODE_POS                           (12U)
#define GLB_REG_SPI_0_MASTER_MODE_LEN                           (1U)
#define GLB_REG_SPI_0_MASTER_MODE_MSK                           (((1U<<GLB_REG_SPI_0_MASTER_MODE_LEN)-1)<<GLB_REG_SPI_0_MASTER_MODE_POS)
#define GLB_REG_SPI_0_MASTER_MODE_UMSK                          (~(((1U<<GLB_REG_SPI_0_MASTER_MODE_LEN)-1)<<GLB_REG_SPI_0_MASTER_MODE_POS))
#define GLB_REG_SPI_0_SWAP                                      GLB_REG_SPI_0_SWAP
#define GLB_REG_SPI_0_SWAP_POS                                  (13U)
#define GLB_REG_SPI_0_SWAP_LEN                                  (1U)
#define GLB_REG_SPI_0_SWAP_MSK                                  (((1U<<GLB_REG_SPI_0_SWAP_LEN)-1)<<GLB_REG_SPI_0_SWAP_POS)
#define GLB_REG_SPI_0_SWAP_UMSK                                 (~(((1U<<GLB_REG_SPI_0_SWAP_LEN)-1)<<GLB_REG_SPI_0_SWAP_POS))
#define GLB_REG_CCI_USE_JTAG_PIN                                GLB_REG_CCI_USE_JTAG_PIN
#define GLB_REG_CCI_USE_JTAG_PIN_POS                            (16U)
#define GLB_REG_CCI_USE_JTAG_PIN_LEN                            (1U)
#define GLB_REG_CCI_USE_JTAG_PIN_MSK                            (((1U<<GLB_REG_CCI_USE_JTAG_PIN_LEN)-1)<<GLB_REG_CCI_USE_JTAG_PIN_POS)
#define GLB_REG_CCI_USE_JTAG_PIN_UMSK                           (~(((1U<<GLB_REG_CCI_USE_JTAG_PIN_LEN)-1)<<GLB_REG_CCI_USE_JTAG_PIN_POS))
#define GLB_P1_ADC_TEST_WITH_CCI                                GLB_P1_ADC_TEST_WITH_CCI
#define GLB_P1_ADC_TEST_WITH_CCI_POS                            (17U)
#define GLB_P1_ADC_TEST_WITH_CCI_LEN                            (1U)
#define GLB_P1_ADC_TEST_WITH_CCI_MSK                            (((1U<<GLB_P1_ADC_TEST_WITH_CCI_LEN)-1)<<GLB_P1_ADC_TEST_WITH_CCI_POS)
#define GLB_P1_ADC_TEST_WITH_CCI_UMSK                           (~(((1U<<GLB_P1_ADC_TEST_WITH_CCI_LEN)-1)<<GLB_P1_ADC_TEST_WITH_CCI_POS))
#define GLB_P2_DAC_TEST_WITH_CCI                                GLB_P2_DAC_TEST_WITH_CCI
#define GLB_P2_DAC_TEST_WITH_CCI_POS                            (18U)
#define GLB_P2_DAC_TEST_WITH_CCI_LEN                            (1U)
#define GLB_P2_DAC_TEST_WITH_CCI_MSK                            (((1U<<GLB_P2_DAC_TEST_WITH_CCI_LEN)-1)<<GLB_P2_DAC_TEST_WITH_CCI_POS)
#define GLB_P2_DAC_TEST_WITH_CCI_UMSK                           (~(((1U<<GLB_P2_DAC_TEST_WITH_CCI_LEN)-1)<<GLB_P2_DAC_TEST_WITH_CCI_POS))
#define GLB_P3_CCI_USE_IO_0_2_7                                 GLB_P3_CCI_USE_IO_0_2_7
#define GLB_P3_CCI_USE_IO_0_2_7_POS                             (19U)
#define GLB_P3_CCI_USE_IO_0_2_7_LEN                             (1U)
#define GLB_P3_CCI_USE_IO_0_2_7_MSK                             (((1U<<GLB_P3_CCI_USE_IO_0_2_7_LEN)-1)<<GLB_P3_CCI_USE_IO_0_2_7_POS)
#define GLB_P3_CCI_USE_IO_0_2_7_UMSK                            (~(((1U<<GLB_P3_CCI_USE_IO_0_2_7_LEN)-1)<<GLB_P3_CCI_USE_IO_0_2_7_POS))
#define GLB_P4_ADC_TEST_WITH_JTAG                               GLB_P4_ADC_TEST_WITH_JTAG
#define GLB_P4_ADC_TEST_WITH_JTAG_POS                           (20U)
#define GLB_P4_ADC_TEST_WITH_JTAG_LEN                           (1U)
#define GLB_P4_ADC_TEST_WITH_JTAG_MSK                           (((1U<<GLB_P4_ADC_TEST_WITH_JTAG_LEN)-1)<<GLB_P4_ADC_TEST_WITH_JTAG_POS)
#define GLB_P4_ADC_TEST_WITH_JTAG_UMSK                          (~(((1U<<GLB_P4_ADC_TEST_WITH_JTAG_LEN)-1)<<GLB_P4_ADC_TEST_WITH_JTAG_POS))
#define GLB_P5_DAC_TEST_WITH_JTAG                               GLB_P5_DAC_TEST_WITH_JTAG
#define GLB_P5_DAC_TEST_WITH_JTAG_POS                           (21U)
#define GLB_P5_DAC_TEST_WITH_JTAG_LEN                           (1U)
#define GLB_P5_DAC_TEST_WITH_JTAG_MSK                           (((1U<<GLB_P5_DAC_TEST_WITH_JTAG_LEN)-1)<<GLB_P5_DAC_TEST_WITH_JTAG_POS)
#define GLB_P5_DAC_TEST_WITH_JTAG_UMSK                          (~(((1U<<GLB_P5_DAC_TEST_WITH_JTAG_LEN)-1)<<GLB_P5_DAC_TEST_WITH_JTAG_POS))
#define GLB_P6_JTAG_USE_IO_0_2_7                                GLB_P6_JTAG_USE_IO_0_2_7
#define GLB_P6_JTAG_USE_IO_0_2_7_POS                            (23U)
#define GLB_P6_JTAG_USE_IO_0_2_7_LEN                            (1U)
#define GLB_P6_JTAG_USE_IO_0_2_7_MSK                            (((1U<<GLB_P6_JTAG_USE_IO_0_2_7_LEN)-1)<<GLB_P6_JTAG_USE_IO_0_2_7_POS)
#define GLB_P6_JTAG_USE_IO_0_2_7_UMSK                           (~(((1U<<GLB_P6_JTAG_USE_IO_0_2_7_LEN)-1)<<GLB_P6_JTAG_USE_IO_0_2_7_POS))
#define GLB_REG_EN_GPIO_O_LATCH_MODE                            GLB_REG_EN_GPIO_O_LATCH_MODE
#define GLB_REG_EN_GPIO_O_LATCH_MODE_POS                        (28U)
#define GLB_REG_EN_GPIO_O_LATCH_MODE_LEN                        (1U)
#define GLB_REG_EN_GPIO_O_LATCH_MODE_MSK                        (((1U<<GLB_REG_EN_GPIO_O_LATCH_MODE_LEN)-1)<<GLB_REG_EN_GPIO_O_LATCH_MODE_POS)
#define GLB_REG_EN_GPIO_O_LATCH_MODE_UMSK                       (~(((1U<<GLB_REG_EN_GPIO_O_LATCH_MODE_LEN)-1)<<GLB_REG_EN_GPIO_O_LATCH_MODE_POS))
#define GLB_REG_KYS_DRV_VAL                                     GLB_REG_KYS_DRV_VAL
#define GLB_REG_KYS_DRV_VAL_POS                                 (29U)
#define GLB_REG_KYS_DRV_VAL_LEN                                 (1U)
#define GLB_REG_KYS_DRV_VAL_MSK                                 (((1U<<GLB_REG_KYS_DRV_VAL_LEN)-1)<<GLB_REG_KYS_DRV_VAL_POS)
#define GLB_REG_KYS_DRV_VAL_UMSK                                (~(((1U<<GLB_REG_KYS_DRV_VAL_LEN)-1)<<GLB_REG_KYS_DRV_VAL_POS))

/* 0x84 : PDM_CLK_CTRL */
#define GLB_PDM_CLK_CTRL_OFFSET                                 (0x84)

/* 0x88 : GPIO_USE_PSRAM__IO */
#define GLB_GPIO_USE_PSRAM__IO_OFFSET                           (0x88)

/* 0x90 : CPU_CLK_CFG */
#define GLB_CPU_CLK_CFG_OFFSET                                  (0x90)
#define GLB_CPU_RTC_DIV                                         GLB_CPU_RTC_DIV
#define GLB_CPU_RTC_DIV_POS                                     (0U)
#define GLB_CPU_RTC_DIV_LEN                                     (5U)
#define GLB_CPU_RTC_DIV_MSK                                     (((1U<<GLB_CPU_RTC_DIV_LEN)-1)<<GLB_CPU_RTC_DIV_POS)
#define GLB_CPU_RTC_DIV_UMSK                                    (~(((1U<<GLB_CPU_RTC_DIV_LEN)-1)<<GLB_CPU_RTC_DIV_POS))
#define GLB_CPU_RTC_EN                                          GLB_CPU_RTC_EN
#define GLB_CPU_RTC_EN_POS                                      (18U)
#define GLB_CPU_RTC_EN_LEN                                      (1U)
#define GLB_CPU_RTC_EN_MSK                                      (((1U<<GLB_CPU_RTC_EN_LEN)-1)<<GLB_CPU_RTC_EN_POS)
#define GLB_CPU_RTC_EN_UMSK                                     (~(((1U<<GLB_CPU_RTC_EN_LEN)-1)<<GLB_CPU_RTC_EN_POS))
#define GLB_CPU_RTC_SEL                                         GLB_CPU_RTC_SEL
#define GLB_CPU_RTC_SEL_POS                                     (19U)
#define GLB_CPU_RTC_SEL_LEN                                     (1U)
#define GLB_CPU_RTC_SEL_MSK                                     (((1U<<GLB_CPU_RTC_SEL_LEN)-1)<<GLB_CPU_RTC_SEL_POS)
#define GLB_CPU_RTC_SEL_UMSK                                    (~(((1U<<GLB_CPU_RTC_SEL_LEN)-1)<<GLB_CPU_RTC_SEL_POS))
#define GLB_DEBUG_NDRESET_GATE                                  GLB_DEBUG_NDRESET_GATE
#define GLB_DEBUG_NDRESET_GATE_POS                              (20U)
#define GLB_DEBUG_NDRESET_GATE_LEN                              (1U)
#define GLB_DEBUG_NDRESET_GATE_MSK                              (((1U<<GLB_DEBUG_NDRESET_GATE_LEN)-1)<<GLB_DEBUG_NDRESET_GATE_POS)
#define GLB_DEBUG_NDRESET_GATE_UMSK                             (~(((1U<<GLB_DEBUG_NDRESET_GATE_LEN)-1)<<GLB_DEBUG_NDRESET_GATE_POS))

/* 0x94 : CLK_DESPUR_ROOT_CLK_CFG */
#define GLB_CLK_DESPUR_ROOT_CLK_CFG_OFFSET                      (0x94)
#define GLB_CFG_DESPUR_BMAP_ROOT_CLK                            GLB_CFG_DESPUR_BMAP_ROOT_CLK
#define GLB_CFG_DESPUR_BMAP_ROOT_CLK_POS                        (0U)
#define GLB_CFG_DESPUR_BMAP_ROOT_CLK_LEN                        (28U)
#define GLB_CFG_DESPUR_BMAP_ROOT_CLK_MSK                        (((1U<<GLB_CFG_DESPUR_BMAP_ROOT_CLK_LEN)-1)<<GLB_CFG_DESPUR_BMAP_ROOT_CLK_POS)
#define GLB_CFG_DESPUR_BMAP_ROOT_CLK_UMSK                       (~(((1U<<GLB_CFG_DESPUR_BMAP_ROOT_CLK_LEN)-1)<<GLB_CFG_DESPUR_BMAP_ROOT_CLK_POS))
#define GLB_CFG_DESPUR_CGEN_ROOT_CLK                            GLB_CFG_DESPUR_CGEN_ROOT_CLK
#define GLB_CFG_DESPUR_CGEN_ROOT_CLK_POS                        (28U)
#define GLB_CFG_DESPUR_CGEN_ROOT_CLK_LEN                        (1U)
#define GLB_CFG_DESPUR_CGEN_ROOT_CLK_MSK                        (((1U<<GLB_CFG_DESPUR_CGEN_ROOT_CLK_LEN)-1)<<GLB_CFG_DESPUR_CGEN_ROOT_CLK_POS)
#define GLB_CFG_DESPUR_CGEN_ROOT_CLK_UMSK                       (~(((1U<<GLB_CFG_DESPUR_CGEN_ROOT_CLK_LEN)-1)<<GLB_CFG_DESPUR_CGEN_ROOT_CLK_POS))
#define GLB_CFG_DESPUR_ENABLE_ROOT_CLK                          GLB_CFG_DESPUR_ENABLE_ROOT_CLK
#define GLB_CFG_DESPUR_ENABLE_ROOT_CLK_POS                      (29U)
#define GLB_CFG_DESPUR_ENABLE_ROOT_CLK_LEN                      (1U)
#define GLB_CFG_DESPUR_ENABLE_ROOT_CLK_MSK                      (((1U<<GLB_CFG_DESPUR_ENABLE_ROOT_CLK_LEN)-1)<<GLB_CFG_DESPUR_ENABLE_ROOT_CLK_POS)
#define GLB_CFG_DESPUR_ENABLE_ROOT_CLK_UMSK                     (~(((1U<<GLB_CFG_DESPUR_ENABLE_ROOT_CLK_LEN)-1)<<GLB_CFG_DESPUR_ENABLE_ROOT_CLK_POS))
#define GLB_CFG_DESPUR_CLK_SEL_ROOT_CLK                         GLB_CFG_DESPUR_CLK_SEL_ROOT_CLK
#define GLB_CFG_DESPUR_CLK_SEL_ROOT_CLK_POS                     (30U)
#define GLB_CFG_DESPUR_CLK_SEL_ROOT_CLK_LEN                     (1U)
#define GLB_CFG_DESPUR_CLK_SEL_ROOT_CLK_MSK                     (((1U<<GLB_CFG_DESPUR_CLK_SEL_ROOT_CLK_LEN)-1)<<GLB_CFG_DESPUR_CLK_SEL_ROOT_CLK_POS)
#define GLB_CFG_DESPUR_CLK_SEL_ROOT_CLK_UMSK                    (~(((1U<<GLB_CFG_DESPUR_CLK_SEL_ROOT_CLK_LEN)-1)<<GLB_CFG_DESPUR_CLK_SEL_ROOT_CLK_POS))

/* 0x98 : CLK_DESPUR_CLK_SF_CFG */
#define GLB_CLK_DESPUR_CLK_SF_CFG_OFFSET                        (0x98)
#define GLB_CFG_DESPUR_BMAP_CLK_SF                              GLB_CFG_DESPUR_BMAP_CLK_SF
#define GLB_CFG_DESPUR_BMAP_CLK_SF_POS                          (0U)
#define GLB_CFG_DESPUR_BMAP_CLK_SF_LEN                          (28U)
#define GLB_CFG_DESPUR_BMAP_CLK_SF_MSK                          (((1U<<GLB_CFG_DESPUR_BMAP_CLK_SF_LEN)-1)<<GLB_CFG_DESPUR_BMAP_CLK_SF_POS)
#define GLB_CFG_DESPUR_BMAP_CLK_SF_UMSK                         (~(((1U<<GLB_CFG_DESPUR_BMAP_CLK_SF_LEN)-1)<<GLB_CFG_DESPUR_BMAP_CLK_SF_POS))
#define GLB_CFG_DESPUR_CGEN_CLK_SF                              GLB_CFG_DESPUR_CGEN_CLK_SF
#define GLB_CFG_DESPUR_CGEN_CLK_SF_POS                          (28U)
#define GLB_CFG_DESPUR_CGEN_CLK_SF_LEN                          (1U)
#define GLB_CFG_DESPUR_CGEN_CLK_SF_MSK                          (((1U<<GLB_CFG_DESPUR_CGEN_CLK_SF_LEN)-1)<<GLB_CFG_DESPUR_CGEN_CLK_SF_POS)
#define GLB_CFG_DESPUR_CGEN_CLK_SF_UMSK                         (~(((1U<<GLB_CFG_DESPUR_CGEN_CLK_SF_LEN)-1)<<GLB_CFG_DESPUR_CGEN_CLK_SF_POS))
#define GLB_CFG_DESPUR_ENABLE_CLK_SF                            GLB_CFG_DESPUR_ENABLE_CLK_SF
#define GLB_CFG_DESPUR_ENABLE_CLK_SF_POS                        (29U)
#define GLB_CFG_DESPUR_ENABLE_CLK_SF_LEN                        (1U)
#define GLB_CFG_DESPUR_ENABLE_CLK_SF_MSK                        (((1U<<GLB_CFG_DESPUR_ENABLE_CLK_SF_LEN)-1)<<GLB_CFG_DESPUR_ENABLE_CLK_SF_POS)
#define GLB_CFG_DESPUR_ENABLE_CLK_SF_UMSK                       (~(((1U<<GLB_CFG_DESPUR_ENABLE_CLK_SF_LEN)-1)<<GLB_CFG_DESPUR_ENABLE_CLK_SF_POS))
#define GLB_CFG_DESPUR_CLK_SEL_CLK_SF                           GLB_CFG_DESPUR_CLK_SEL_CLK_SF
#define GLB_CFG_DESPUR_CLK_SEL_CLK_SF_POS                       (30U)
#define GLB_CFG_DESPUR_CLK_SEL_CLK_SF_LEN                       (1U)
#define GLB_CFG_DESPUR_CLK_SEL_CLK_SF_MSK                       (((1U<<GLB_CFG_DESPUR_CLK_SEL_CLK_SF_LEN)-1)<<GLB_CFG_DESPUR_CLK_SEL_CLK_SF_POS)
#define GLB_CFG_DESPUR_CLK_SEL_CLK_SF_UMSK                      (~(((1U<<GLB_CFG_DESPUR_CLK_SEL_CLK_SF_LEN)-1)<<GLB_CFG_DESPUR_CLK_SEL_CLK_SF_POS))

/* 0xA4 : GPADC_32M_SRC_CTRL */
#define GLB_GPADC_32M_SRC_CTRL_OFFSET                           (0xA4)
#define GLB_GPADC_32M_CLK_DIV                                   GLB_GPADC_32M_CLK_DIV
#define GLB_GPADC_32M_CLK_DIV_POS                               (0U)
#define GLB_GPADC_32M_CLK_DIV_LEN                               (6U)
#define GLB_GPADC_32M_CLK_DIV_MSK                               (((1U<<GLB_GPADC_32M_CLK_DIV_LEN)-1)<<GLB_GPADC_32M_CLK_DIV_POS)
#define GLB_GPADC_32M_CLK_DIV_UMSK                              (~(((1U<<GLB_GPADC_32M_CLK_DIV_LEN)-1)<<GLB_GPADC_32M_CLK_DIV_POS))
#define GLB_GPADC_32M_CLK_SEL                                   GLB_GPADC_32M_CLK_SEL
#define GLB_GPADC_32M_CLK_SEL_POS                               (7U)
#define GLB_GPADC_32M_CLK_SEL_LEN                               (1U)
#define GLB_GPADC_32M_CLK_SEL_MSK                               (((1U<<GLB_GPADC_32M_CLK_SEL_LEN)-1)<<GLB_GPADC_32M_CLK_SEL_POS)
#define GLB_GPADC_32M_CLK_SEL_UMSK                              (~(((1U<<GLB_GPADC_32M_CLK_SEL_LEN)-1)<<GLB_GPADC_32M_CLK_SEL_POS))
#define GLB_GPADC_32M_DIV_EN                                    GLB_GPADC_32M_DIV_EN
#define GLB_GPADC_32M_DIV_EN_POS                                (8U)
#define GLB_GPADC_32M_DIV_EN_LEN                                (1U)
#define GLB_GPADC_32M_DIV_EN_MSK                                (((1U<<GLB_GPADC_32M_DIV_EN_LEN)-1)<<GLB_GPADC_32M_DIV_EN_POS)
#define GLB_GPADC_32M_DIV_EN_UMSK                               (~(((1U<<GLB_GPADC_32M_DIV_EN_LEN)-1)<<GLB_GPADC_32M_DIV_EN_POS))

/* 0xA8 : DIG32K_WAKEUP_CTRL */
#define GLB_DIG32K_WAKEUP_CTRL_OFFSET                           (0xA8)
#define GLB_DIG_32K_DIV                                         GLB_DIG_32K_DIV
#define GLB_DIG_32K_DIV_POS                                     (0U)
#define GLB_DIG_32K_DIV_LEN                                     (11U)
#define GLB_DIG_32K_DIV_MSK                                     (((1U<<GLB_DIG_32K_DIV_LEN)-1)<<GLB_DIG_32K_DIV_POS)
#define GLB_DIG_32K_DIV_UMSK                                    (~(((1U<<GLB_DIG_32K_DIV_LEN)-1)<<GLB_DIG_32K_DIV_POS))
#define GLB_DIG_32K_EN                                          GLB_DIG_32K_EN
#define GLB_DIG_32K_EN_POS                                      (12U)
#define GLB_DIG_32K_EN_LEN                                      (1U)
#define GLB_DIG_32K_EN_MSK                                      (((1U<<GLB_DIG_32K_EN_LEN)-1)<<GLB_DIG_32K_EN_POS)
#define GLB_DIG_32K_EN_UMSK                                     (~(((1U<<GLB_DIG_32K_EN_LEN)-1)<<GLB_DIG_32K_EN_POS))
#define GLB_DIG_32K_COMP                                        GLB_DIG_32K_COMP
#define GLB_DIG_32K_COMP_POS                                    (13U)
#define GLB_DIG_32K_COMP_LEN                                    (1U)
#define GLB_DIG_32K_COMP_MSK                                    (((1U<<GLB_DIG_32K_COMP_LEN)-1)<<GLB_DIG_32K_COMP_POS)
#define GLB_DIG_32K_COMP_UMSK                                   (~(((1U<<GLB_DIG_32K_COMP_LEN)-1)<<GLB_DIG_32K_COMP_POS))
#define GLB_DIG_CLK_SRC_SEL                                     GLB_DIG_CLK_SRC_SEL
#define GLB_DIG_CLK_SRC_SEL_POS                                 (28U)
#define GLB_DIG_CLK_SRC_SEL_LEN                                 (1U)
#define GLB_DIG_CLK_SRC_SEL_MSK                                 (((1U<<GLB_DIG_CLK_SRC_SEL_LEN)-1)<<GLB_DIG_CLK_SRC_SEL_POS)
#define GLB_DIG_CLK_SRC_SEL_UMSK                                (~(((1U<<GLB_DIG_CLK_SRC_SEL_LEN)-1)<<GLB_DIG_CLK_SRC_SEL_POS))
#define GLB_SW_BLE_WAKEUP_REQ                                   GLB_SW_BLE_WAKEUP_REQ
#define GLB_SW_BLE_WAKEUP_REQ_POS                               (31U)
#define GLB_SW_BLE_WAKEUP_REQ_LEN                               (1U)
#define GLB_SW_BLE_WAKEUP_REQ_MSK                               (((1U<<GLB_SW_BLE_WAKEUP_REQ_LEN)-1)<<GLB_SW_BLE_WAKEUP_REQ_POS)
#define GLB_SW_BLE_WAKEUP_REQ_UMSK                              (~(((1U<<GLB_SW_BLE_WAKEUP_REQ_LEN)-1)<<GLB_SW_BLE_WAKEUP_REQ_POS))

/* 0xAC : WIFI_BT_COEX_CTRL */
#define GLB_WIFI_BT_COEX_CTRL_OFFSET                            (0xAC)

/* 0xB0 : BZ_COEX_CTRL */
#define GLB_BZ_COEX_CTRL_OFFSET                                 (0xB0)
#define GLB_COEX_EN                                             GLB_COEX_EN
#define GLB_COEX_EN_POS                                         (0U)
#define GLB_COEX_EN_LEN                                         (1U)
#define GLB_COEX_EN_MSK                                         (((1U<<GLB_COEX_EN_LEN)-1)<<GLB_COEX_EN_POS)
#define GLB_COEX_EN_UMSK                                        (~(((1U<<GLB_COEX_EN_LEN)-1)<<GLB_COEX_EN_POS))
#define GLB_WLAN_EN                                             GLB_WLAN_EN
#define GLB_WLAN_EN_POS                                         (1U)
#define GLB_WLAN_EN_LEN                                         (1U)
#define GLB_WLAN_EN_MSK                                         (((1U<<GLB_WLAN_EN_LEN)-1)<<GLB_WLAN_EN_POS)
#define GLB_WLAN_EN_UMSK                                        (~(((1U<<GLB_WLAN_EN_LEN)-1)<<GLB_WLAN_EN_POS))
#define GLB_BLE_RX_IGNORE                                       GLB_BLE_RX_IGNORE
#define GLB_BLE_RX_IGNORE_POS                                   (2U)
#define GLB_BLE_RX_IGNORE_LEN                                   (1U)
#define GLB_BLE_RX_IGNORE_MSK                                   (((1U<<GLB_BLE_RX_IGNORE_LEN)-1)<<GLB_BLE_RX_IGNORE_POS)
#define GLB_BLE_RX_IGNORE_UMSK                                  (~(((1U<<GLB_BLE_RX_IGNORE_LEN)-1)<<GLB_BLE_RX_IGNORE_POS))
#define GLB_M154_RX_IGNORE                                      GLB_M154_RX_IGNORE
#define GLB_M154_RX_IGNORE_POS                                  (3U)
#define GLB_M154_RX_IGNORE_LEN                                  (1U)
#define GLB_M154_RX_IGNORE_MSK                                  (((1U<<GLB_M154_RX_IGNORE_LEN)-1)<<GLB_M154_RX_IGNORE_POS)
#define GLB_M154_RX_IGNORE_UMSK                                 (~(((1U<<GLB_M154_RX_IGNORE_LEN)-1)<<GLB_M154_RX_IGNORE_POS))
#define GLB_BZ_PRI_THR                                          GLB_BZ_PRI_THR
#define GLB_BZ_PRI_THR_POS                                      (4U)
#define GLB_BZ_PRI_THR_LEN                                      (4U)
#define GLB_BZ_PRI_THR_MSK                                      (((1U<<GLB_BZ_PRI_THR_LEN)-1)<<GLB_BZ_PRI_THR_POS)
#define GLB_BZ_PRI_THR_UMSK                                     (~(((1U<<GLB_BZ_PRI_THR_LEN)-1)<<GLB_BZ_PRI_THR_POS))
#define GLB_BZ_PRI_EN                                           GLB_BZ_PRI_EN
#define GLB_BZ_PRI_EN_POS                                       (8U)
#define GLB_BZ_PRI_EN_LEN                                       (1U)
#define GLB_BZ_PRI_EN_MSK                                       (((1U<<GLB_BZ_PRI_EN_LEN)-1)<<GLB_BZ_PRI_EN_POS)
#define GLB_BZ_PRI_EN_UMSK                                      (~(((1U<<GLB_BZ_PRI_EN_LEN)-1)<<GLB_BZ_PRI_EN_POS))
#define GLB_BZ_PRI_POL                                          GLB_BZ_PRI_POL
#define GLB_BZ_PRI_POL_POS                                      (9U)
#define GLB_BZ_PRI_POL_LEN                                      (1U)
#define GLB_BZ_PRI_POL_MSK                                      (((1U<<GLB_BZ_PRI_POL_LEN)-1)<<GLB_BZ_PRI_POL_POS)
#define GLB_BZ_PRI_POL_UMSK                                     (~(((1U<<GLB_BZ_PRI_POL_LEN)-1)<<GLB_BZ_PRI_POL_POS))
#define GLB_BZ_ACTIVE_POL                                       GLB_BZ_ACTIVE_POL
#define GLB_BZ_ACTIVE_POL_POS                                   (10U)
#define GLB_BZ_ACTIVE_POL_LEN                                   (1U)
#define GLB_BZ_ACTIVE_POL_MSK                                   (((1U<<GLB_BZ_ACTIVE_POL_LEN)-1)<<GLB_BZ_ACTIVE_POL_POS)
#define GLB_BZ_ACTIVE_POL_UMSK                                  (~(((1U<<GLB_BZ_ACTIVE_POL_LEN)-1)<<GLB_BZ_ACTIVE_POL_POS))
#define GLB_BZ_ABORT_POL                                        GLB_BZ_ABORT_POL
#define GLB_BZ_ABORT_POL_POS                                    (11U)
#define GLB_BZ_ABORT_POL_LEN                                    (1U)
#define GLB_BZ_ABORT_POL_MSK                                    (((1U<<GLB_BZ_ABORT_POL_LEN)-1)<<GLB_BZ_ABORT_POL_POS)
#define GLB_BZ_ABORT_POL_UMSK                                   (~(((1U<<GLB_BZ_ABORT_POL_LEN)-1)<<GLB_BZ_ABORT_POL_POS))
#define GLB_COEX_PRI                                            GLB_COEX_PRI
#define GLB_COEX_PRI_POS                                        (12U)
#define GLB_COEX_PRI_LEN                                        (1U)
#define GLB_COEX_PRI_MSK                                        (((1U<<GLB_COEX_PRI_LEN)-1)<<GLB_COEX_PRI_POS)
#define GLB_COEX_PRI_UMSK                                       (~(((1U<<GLB_COEX_PRI_LEN)-1)<<GLB_COEX_PRI_POS))
#define GLB_FORCE_M154_WIN                                      GLB_FORCE_M154_WIN
#define GLB_FORCE_M154_WIN_POS                                  (13U)
#define GLB_FORCE_M154_WIN_LEN                                  (1U)
#define GLB_FORCE_M154_WIN_MSK                                  (((1U<<GLB_FORCE_M154_WIN_LEN)-1)<<GLB_FORCE_M154_WIN_POS)
#define GLB_FORCE_M154_WIN_UMSK                                 (~(((1U<<GLB_FORCE_M154_WIN_LEN)-1)<<GLB_FORCE_M154_WIN_POS))
#define GLB_FORCE_BLE_WIN                                       GLB_FORCE_BLE_WIN
#define GLB_FORCE_BLE_WIN_POS                                   (14U)
#define GLB_FORCE_BLE_WIN_LEN                                   (1U)
#define GLB_FORCE_BLE_WIN_MSK                                   (((1U<<GLB_FORCE_BLE_WIN_LEN)-1)<<GLB_FORCE_BLE_WIN_POS)
#define GLB_FORCE_BLE_WIN_UMSK                                  (~(((1U<<GLB_FORCE_BLE_WIN_LEN)-1)<<GLB_FORCE_BLE_WIN_POS))
#define GLB_COEX_OPTION                                         GLB_COEX_OPTION
#define GLB_COEX_OPTION_POS                                     (15U)
#define GLB_COEX_OPTION_LEN                                     (1U)
#define GLB_COEX_OPTION_MSK                                     (((1U<<GLB_COEX_OPTION_LEN)-1)<<GLB_COEX_OPTION_POS)
#define GLB_COEX_OPTION_UMSK                                    (~(((1U<<GLB_COEX_OPTION_LEN)-1)<<GLB_COEX_OPTION_POS))
#define GLB_COEX_FORCE_CH                                       GLB_COEX_FORCE_CH
#define GLB_COEX_FORCE_CH_POS                                   (16U)
#define GLB_COEX_FORCE_CH_LEN                                   (7U)
#define GLB_COEX_FORCE_CH_MSK                                   (((1U<<GLB_COEX_FORCE_CH_LEN)-1)<<GLB_COEX_FORCE_CH_POS)
#define GLB_COEX_FORCE_CH_UMSK                                  (~(((1U<<GLB_COEX_FORCE_CH_LEN)-1)<<GLB_COEX_FORCE_CH_POS))
#define GLB_M154_RX_ABORT_DIS                                   GLB_M154_RX_ABORT_DIS
#define GLB_M154_RX_ABORT_DIS_POS                               (24U)
#define GLB_M154_RX_ABORT_DIS_LEN                               (1U)
#define GLB_M154_RX_ABORT_DIS_MSK                               (((1U<<GLB_M154_RX_ABORT_DIS_LEN)-1)<<GLB_M154_RX_ABORT_DIS_POS)
#define GLB_M154_RX_ABORT_DIS_UMSK                              (~(((1U<<GLB_M154_RX_ABORT_DIS_LEN)-1)<<GLB_M154_RX_ABORT_DIS_POS))
#define GLB_M154_TX_ABORT_DIS                                   GLB_M154_TX_ABORT_DIS
#define GLB_M154_TX_ABORT_DIS_POS                               (25U)
#define GLB_M154_TX_ABORT_DIS_LEN                               (1U)
#define GLB_M154_TX_ABORT_DIS_MSK                               (((1U<<GLB_M154_TX_ABORT_DIS_LEN)-1)<<GLB_M154_TX_ABORT_DIS_POS)
#define GLB_M154_TX_ABORT_DIS_UMSK                              (~(((1U<<GLB_M154_TX_ABORT_DIS_LEN)-1)<<GLB_M154_TX_ABORT_DIS_POS))
#define GLB_BLE_RX_ABORT_DIS                                    GLB_BLE_RX_ABORT_DIS
#define GLB_BLE_RX_ABORT_DIS_POS                                (26U)
#define GLB_BLE_RX_ABORT_DIS_LEN                                (1U)
#define GLB_BLE_RX_ABORT_DIS_MSK                                (((1U<<GLB_BLE_RX_ABORT_DIS_LEN)-1)<<GLB_BLE_RX_ABORT_DIS_POS)
#define GLB_BLE_RX_ABORT_DIS_UMSK                               (~(((1U<<GLB_BLE_RX_ABORT_DIS_LEN)-1)<<GLB_BLE_RX_ABORT_DIS_POS))
#define GLB_BLE_TX_ABORT_DIS                                    GLB_BLE_TX_ABORT_DIS
#define GLB_BLE_TX_ABORT_DIS_POS                                (27U)
#define GLB_BLE_TX_ABORT_DIS_LEN                                (1U)
#define GLB_BLE_TX_ABORT_DIS_MSK                                (((1U<<GLB_BLE_TX_ABORT_DIS_LEN)-1)<<GLB_BLE_TX_ABORT_DIS_POS)
#define GLB_BLE_TX_ABORT_DIS_UMSK                               (~(((1U<<GLB_BLE_TX_ABORT_DIS_LEN)-1)<<GLB_BLE_TX_ABORT_DIS_POS))
#define GLB_COEX_ARB                                            GLB_COEX_ARB
#define GLB_COEX_ARB_POS                                        (28U)
#define GLB_COEX_ARB_LEN                                        (4U)
#define GLB_COEX_ARB_MSK                                        (((1U<<GLB_COEX_ARB_LEN)-1)<<GLB_COEX_ARB_POS)
#define GLB_COEX_ARB_UMSK                                       (~(((1U<<GLB_COEX_ARB_LEN)-1)<<GLB_COEX_ARB_POS))

/* 0xC0 : UART_SIG_SEL_0 */
#define GLB_UART_SIG_SEL_0_OFFSET                               (0xC0)
#define GLB_UART_SIG_0_SEL                                      GLB_UART_SIG_0_SEL
#define GLB_UART_SIG_0_SEL_POS                                  (0U)
#define GLB_UART_SIG_0_SEL_LEN                                  (4U)
#define GLB_UART_SIG_0_SEL_MSK                                  (((1U<<GLB_UART_SIG_0_SEL_LEN)-1)<<GLB_UART_SIG_0_SEL_POS)
#define GLB_UART_SIG_0_SEL_UMSK                                 (~(((1U<<GLB_UART_SIG_0_SEL_LEN)-1)<<GLB_UART_SIG_0_SEL_POS))
#define GLB_UART_SIG_1_SEL                                      GLB_UART_SIG_1_SEL
#define GLB_UART_SIG_1_SEL_POS                                  (4U)
#define GLB_UART_SIG_1_SEL_LEN                                  (4U)
#define GLB_UART_SIG_1_SEL_MSK                                  (((1U<<GLB_UART_SIG_1_SEL_LEN)-1)<<GLB_UART_SIG_1_SEL_POS)
#define GLB_UART_SIG_1_SEL_UMSK                                 (~(((1U<<GLB_UART_SIG_1_SEL_LEN)-1)<<GLB_UART_SIG_1_SEL_POS))
#define GLB_UART_SIG_2_SEL                                      GLB_UART_SIG_2_SEL
#define GLB_UART_SIG_2_SEL_POS                                  (8U)
#define GLB_UART_SIG_2_SEL_LEN                                  (4U)
#define GLB_UART_SIG_2_SEL_MSK                                  (((1U<<GLB_UART_SIG_2_SEL_LEN)-1)<<GLB_UART_SIG_2_SEL_POS)
#define GLB_UART_SIG_2_SEL_UMSK                                 (~(((1U<<GLB_UART_SIG_2_SEL_LEN)-1)<<GLB_UART_SIG_2_SEL_POS))
#define GLB_UART_SIG_3_SEL                                      GLB_UART_SIG_3_SEL
#define GLB_UART_SIG_3_SEL_POS                                  (12U)
#define GLB_UART_SIG_3_SEL_LEN                                  (4U)
#define GLB_UART_SIG_3_SEL_MSK                                  (((1U<<GLB_UART_SIG_3_SEL_LEN)-1)<<GLB_UART_SIG_3_SEL_POS)
#define GLB_UART_SIG_3_SEL_UMSK                                 (~(((1U<<GLB_UART_SIG_3_SEL_LEN)-1)<<GLB_UART_SIG_3_SEL_POS))

/* 0xD0 : DBG_SEL_LL */
#define GLB_DBG_SEL_LL_OFFSET                                   (0xD0)
#define GLB_REG_DBG_LL_CTRL                                     GLB_REG_DBG_LL_CTRL
#define GLB_REG_DBG_LL_CTRL_POS                                 (0U)
#define GLB_REG_DBG_LL_CTRL_LEN                                 (32U)
#define GLB_REG_DBG_LL_CTRL_MSK                                 (((1U<<GLB_REG_DBG_LL_CTRL_LEN)-1)<<GLB_REG_DBG_LL_CTRL_POS)
#define GLB_REG_DBG_LL_CTRL_UMSK                                (~(((1U<<GLB_REG_DBG_LL_CTRL_LEN)-1)<<GLB_REG_DBG_LL_CTRL_POS))

/* 0xD4 : DBG_SEL_LH */
#define GLB_DBG_SEL_LH_OFFSET                                   (0xD4)
#define GLB_REG_DBG_LH_CTRL                                     GLB_REG_DBG_LH_CTRL
#define GLB_REG_DBG_LH_CTRL_POS                                 (0U)
#define GLB_REG_DBG_LH_CTRL_LEN                                 (32U)
#define GLB_REG_DBG_LH_CTRL_MSK                                 (((1U<<GLB_REG_DBG_LH_CTRL_LEN)-1)<<GLB_REG_DBG_LH_CTRL_POS)
#define GLB_REG_DBG_LH_CTRL_UMSK                                (~(((1U<<GLB_REG_DBG_LH_CTRL_LEN)-1)<<GLB_REG_DBG_LH_CTRL_POS))

/* 0xD8 : DBG_SEL_HL */
#define GLB_DBG_SEL_HL_OFFSET                                   (0xD8)
#define GLB_REG_DBG_HL_CTRL                                     GLB_REG_DBG_HL_CTRL
#define GLB_REG_DBG_HL_CTRL_POS                                 (0U)
#define GLB_REG_DBG_HL_CTRL_LEN                                 (32U)
#define GLB_REG_DBG_HL_CTRL_MSK                                 (((1U<<GLB_REG_DBG_HL_CTRL_LEN)-1)<<GLB_REG_DBG_HL_CTRL_POS)
#define GLB_REG_DBG_HL_CTRL_UMSK                                (~(((1U<<GLB_REG_DBG_HL_CTRL_LEN)-1)<<GLB_REG_DBG_HL_CTRL_POS))

/* 0xDC : DBG_SEL_HH */
#define GLB_DBG_SEL_HH_OFFSET                                   (0xDC)
#define GLB_REG_DBG_HH_CTRL                                     GLB_REG_DBG_HH_CTRL
#define GLB_REG_DBG_HH_CTRL_POS                                 (0U)
#define GLB_REG_DBG_HH_CTRL_LEN                                 (32U)
#define GLB_REG_DBG_HH_CTRL_MSK                                 (((1U<<GLB_REG_DBG_HH_CTRL_LEN)-1)<<GLB_REG_DBG_HH_CTRL_POS)
#define GLB_REG_DBG_HH_CTRL_UMSK                                (~(((1U<<GLB_REG_DBG_HH_CTRL_LEN)-1)<<GLB_REG_DBG_HH_CTRL_POS))

/* 0xE0 : debug */
#define GLB_DEBUG_OFFSET                                        (0xE0)
#define GLB_DEBUG_OE                                            GLB_DEBUG_OE
#define GLB_DEBUG_OE_POS                                        (0U)
#define GLB_DEBUG_OE_LEN                                        (1U)
#define GLB_DEBUG_OE_MSK                                        (((1U<<GLB_DEBUG_OE_LEN)-1)<<GLB_DEBUG_OE_POS)
#define GLB_DEBUG_OE_UMSK                                       (~(((1U<<GLB_DEBUG_OE_LEN)-1)<<GLB_DEBUG_OE_POS))
#define GLB_DEBUG_I                                             GLB_DEBUG_I
#define GLB_DEBUG_I_POS                                         (1U)
#define GLB_DEBUG_I_LEN                                         (31U)
#define GLB_DEBUG_I_MSK                                         (((1U<<GLB_DEBUG_I_LEN)-1)<<GLB_DEBUG_I_POS)
#define GLB_DEBUG_I_UMSK                                        (~(((1U<<GLB_DEBUG_I_LEN)-1)<<GLB_DEBUG_I_POS))

/* 0x100 : GPIO_CFGCTL0 */
#define GLB_GPIO_CFGCTL0_OFFSET                                 (0x100)
#define GLB_REG_GPIO_0_IE                                       GLB_REG_GPIO_0_IE
#define GLB_REG_GPIO_0_IE_POS                                   (0U)
#define GLB_REG_GPIO_0_IE_LEN                                   (1U)
#define GLB_REG_GPIO_0_IE_MSK                                   (((1U<<GLB_REG_GPIO_0_IE_LEN)-1)<<GLB_REG_GPIO_0_IE_POS)
#define GLB_REG_GPIO_0_IE_UMSK                                  (~(((1U<<GLB_REG_GPIO_0_IE_LEN)-1)<<GLB_REG_GPIO_0_IE_POS))
#define GLB_REG_GPIO_0_SMT                                      GLB_REG_GPIO_0_SMT
#define GLB_REG_GPIO_0_SMT_POS                                  (1U)
#define GLB_REG_GPIO_0_SMT_LEN                                  (1U)
#define GLB_REG_GPIO_0_SMT_MSK                                  (((1U<<GLB_REG_GPIO_0_SMT_LEN)-1)<<GLB_REG_GPIO_0_SMT_POS)
#define GLB_REG_GPIO_0_SMT_UMSK                                 (~(((1U<<GLB_REG_GPIO_0_SMT_LEN)-1)<<GLB_REG_GPIO_0_SMT_POS))
#define GLB_REG_GPIO_0_DRV                                      GLB_REG_GPIO_0_DRV
#define GLB_REG_GPIO_0_DRV_POS                                  (2U)
#define GLB_REG_GPIO_0_DRV_LEN                                  (2U)
#define GLB_REG_GPIO_0_DRV_MSK                                  (((1U<<GLB_REG_GPIO_0_DRV_LEN)-1)<<GLB_REG_GPIO_0_DRV_POS)
#define GLB_REG_GPIO_0_DRV_UMSK                                 (~(((1U<<GLB_REG_GPIO_0_DRV_LEN)-1)<<GLB_REG_GPIO_0_DRV_POS))
#define GLB_REG_GPIO_0_PU                                       GLB_REG_GPIO_0_PU
#define GLB_REG_GPIO_0_PU_POS                                   (4U)
#define GLB_REG_GPIO_0_PU_LEN                                   (1U)
#define GLB_REG_GPIO_0_PU_MSK                                   (((1U<<GLB_REG_GPIO_0_PU_LEN)-1)<<GLB_REG_GPIO_0_PU_POS)
#define GLB_REG_GPIO_0_PU_UMSK                                  (~(((1U<<GLB_REG_GPIO_0_PU_LEN)-1)<<GLB_REG_GPIO_0_PU_POS))
#define GLB_REG_GPIO_0_PD                                       GLB_REG_GPIO_0_PD
#define GLB_REG_GPIO_0_PD_POS                                   (5U)
#define GLB_REG_GPIO_0_PD_LEN                                   (1U)
#define GLB_REG_GPIO_0_PD_MSK                                   (((1U<<GLB_REG_GPIO_0_PD_LEN)-1)<<GLB_REG_GPIO_0_PD_POS)
#define GLB_REG_GPIO_0_PD_UMSK                                  (~(((1U<<GLB_REG_GPIO_0_PD_LEN)-1)<<GLB_REG_GPIO_0_PD_POS))
#define GLB_REG_GPIO_0_FUNC_SEL                                 GLB_REG_GPIO_0_FUNC_SEL
#define GLB_REG_GPIO_0_FUNC_SEL_POS                             (8U)
#define GLB_REG_GPIO_0_FUNC_SEL_LEN                             (5U)
#define GLB_REG_GPIO_0_FUNC_SEL_MSK                             (((1U<<GLB_REG_GPIO_0_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_0_FUNC_SEL_POS)
#define GLB_REG_GPIO_0_FUNC_SEL_UMSK                            (~(((1U<<GLB_REG_GPIO_0_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_0_FUNC_SEL_POS))
#define GLB_REG_GPIO_0_MODE                                     GLB_REG_GPIO_0_MODE
#define GLB_REG_GPIO_0_MODE_POS                                 (15U)
#define GLB_REG_GPIO_0_MODE_LEN                                 (1U)
#define GLB_REG_GPIO_0_MODE_MSK                                 (((1U<<GLB_REG_GPIO_0_MODE_LEN)-1)<<GLB_REG_GPIO_0_MODE_POS)
#define GLB_REG_GPIO_0_MODE_UMSK                                (~(((1U<<GLB_REG_GPIO_0_MODE_LEN)-1)<<GLB_REG_GPIO_0_MODE_POS))
#define GLB_REG_GPIO_1_IE                                       GLB_REG_GPIO_1_IE
#define GLB_REG_GPIO_1_IE_POS                                   (16U)
#define GLB_REG_GPIO_1_IE_LEN                                   (1U)
#define GLB_REG_GPIO_1_IE_MSK                                   (((1U<<GLB_REG_GPIO_1_IE_LEN)-1)<<GLB_REG_GPIO_1_IE_POS)
#define GLB_REG_GPIO_1_IE_UMSK                                  (~(((1U<<GLB_REG_GPIO_1_IE_LEN)-1)<<GLB_REG_GPIO_1_IE_POS))
#define GLB_REG_GPIO_1_SMT                                      GLB_REG_GPIO_1_SMT
#define GLB_REG_GPIO_1_SMT_POS                                  (17U)
#define GLB_REG_GPIO_1_SMT_LEN                                  (1U)
#define GLB_REG_GPIO_1_SMT_MSK                                  (((1U<<GLB_REG_GPIO_1_SMT_LEN)-1)<<GLB_REG_GPIO_1_SMT_POS)
#define GLB_REG_GPIO_1_SMT_UMSK                                 (~(((1U<<GLB_REG_GPIO_1_SMT_LEN)-1)<<GLB_REG_GPIO_1_SMT_POS))
#define GLB_REG_GPIO_1_DRV                                      GLB_REG_GPIO_1_DRV
#define GLB_REG_GPIO_1_DRV_POS                                  (18U)
#define GLB_REG_GPIO_1_DRV_LEN                                  (2U)
#define GLB_REG_GPIO_1_DRV_MSK                                  (((1U<<GLB_REG_GPIO_1_DRV_LEN)-1)<<GLB_REG_GPIO_1_DRV_POS)
#define GLB_REG_GPIO_1_DRV_UMSK                                 (~(((1U<<GLB_REG_GPIO_1_DRV_LEN)-1)<<GLB_REG_GPIO_1_DRV_POS))
#define GLB_REG_GPIO_1_PU                                       GLB_REG_GPIO_1_PU
#define GLB_REG_GPIO_1_PU_POS                                   (20U)
#define GLB_REG_GPIO_1_PU_LEN                                   (1U)
#define GLB_REG_GPIO_1_PU_MSK                                   (((1U<<GLB_REG_GPIO_1_PU_LEN)-1)<<GLB_REG_GPIO_1_PU_POS)
#define GLB_REG_GPIO_1_PU_UMSK                                  (~(((1U<<GLB_REG_GPIO_1_PU_LEN)-1)<<GLB_REG_GPIO_1_PU_POS))
#define GLB_REG_GPIO_1_PD                                       GLB_REG_GPIO_1_PD
#define GLB_REG_GPIO_1_PD_POS                                   (21U)
#define GLB_REG_GPIO_1_PD_LEN                                   (1U)
#define GLB_REG_GPIO_1_PD_MSK                                   (((1U<<GLB_REG_GPIO_1_PD_LEN)-1)<<GLB_REG_GPIO_1_PD_POS)
#define GLB_REG_GPIO_1_PD_UMSK                                  (~(((1U<<GLB_REG_GPIO_1_PD_LEN)-1)<<GLB_REG_GPIO_1_PD_POS))
#define GLB_REG_GPIO_1_FUNC_SEL                                 GLB_REG_GPIO_1_FUNC_SEL
#define GLB_REG_GPIO_1_FUNC_SEL_POS                             (24U)
#define GLB_REG_GPIO_1_FUNC_SEL_LEN                             (5U)
#define GLB_REG_GPIO_1_FUNC_SEL_MSK                             (((1U<<GLB_REG_GPIO_1_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_1_FUNC_SEL_POS)
#define GLB_REG_GPIO_1_FUNC_SEL_UMSK                            (~(((1U<<GLB_REG_GPIO_1_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_1_FUNC_SEL_POS))
#define GLB_REG_GPIO_1_MODE                                     GLB_REG_GPIO_1_MODE
#define GLB_REG_GPIO_1_MODE_POS                                 (31U)
#define GLB_REG_GPIO_1_MODE_LEN                                 (1U)
#define GLB_REG_GPIO_1_MODE_MSK                                 (((1U<<GLB_REG_GPIO_1_MODE_LEN)-1)<<GLB_REG_GPIO_1_MODE_POS)
#define GLB_REG_GPIO_1_MODE_UMSK                                (~(((1U<<GLB_REG_GPIO_1_MODE_LEN)-1)<<GLB_REG_GPIO_1_MODE_POS))

/* 0x104 : GPIO_CFGCTL1 */
#define GLB_GPIO_CFGCTL1_OFFSET                                 (0x104)
#define GLB_REG_GPIO_2_IE                                       GLB_REG_GPIO_2_IE
#define GLB_REG_GPIO_2_IE_POS                                   (0U)
#define GLB_REG_GPIO_2_IE_LEN                                   (1U)
#define GLB_REG_GPIO_2_IE_MSK                                   (((1U<<GLB_REG_GPIO_2_IE_LEN)-1)<<GLB_REG_GPIO_2_IE_POS)
#define GLB_REG_GPIO_2_IE_UMSK                                  (~(((1U<<GLB_REG_GPIO_2_IE_LEN)-1)<<GLB_REG_GPIO_2_IE_POS))
#define GLB_REG_GPIO_2_SMT                                      GLB_REG_GPIO_2_SMT
#define GLB_REG_GPIO_2_SMT_POS                                  (1U)
#define GLB_REG_GPIO_2_SMT_LEN                                  (1U)
#define GLB_REG_GPIO_2_SMT_MSK                                  (((1U<<GLB_REG_GPIO_2_SMT_LEN)-1)<<GLB_REG_GPIO_2_SMT_POS)
#define GLB_REG_GPIO_2_SMT_UMSK                                 (~(((1U<<GLB_REG_GPIO_2_SMT_LEN)-1)<<GLB_REG_GPIO_2_SMT_POS))
#define GLB_REG_GPIO_2_DRV                                      GLB_REG_GPIO_2_DRV
#define GLB_REG_GPIO_2_DRV_POS                                  (2U)
#define GLB_REG_GPIO_2_DRV_LEN                                  (2U)
#define GLB_REG_GPIO_2_DRV_MSK                                  (((1U<<GLB_REG_GPIO_2_DRV_LEN)-1)<<GLB_REG_GPIO_2_DRV_POS)
#define GLB_REG_GPIO_2_DRV_UMSK                                 (~(((1U<<GLB_REG_GPIO_2_DRV_LEN)-1)<<GLB_REG_GPIO_2_DRV_POS))
#define GLB_REG_GPIO_2_PU                                       GLB_REG_GPIO_2_PU
#define GLB_REG_GPIO_2_PU_POS                                   (4U)
#define GLB_REG_GPIO_2_PU_LEN                                   (1U)
#define GLB_REG_GPIO_2_PU_MSK                                   (((1U<<GLB_REG_GPIO_2_PU_LEN)-1)<<GLB_REG_GPIO_2_PU_POS)
#define GLB_REG_GPIO_2_PU_UMSK                                  (~(((1U<<GLB_REG_GPIO_2_PU_LEN)-1)<<GLB_REG_GPIO_2_PU_POS))
#define GLB_REG_GPIO_2_PD                                       GLB_REG_GPIO_2_PD
#define GLB_REG_GPIO_2_PD_POS                                   (5U)
#define GLB_REG_GPIO_2_PD_LEN                                   (1U)
#define GLB_REG_GPIO_2_PD_MSK                                   (((1U<<GLB_REG_GPIO_2_PD_LEN)-1)<<GLB_REG_GPIO_2_PD_POS)
#define GLB_REG_GPIO_2_PD_UMSK                                  (~(((1U<<GLB_REG_GPIO_2_PD_LEN)-1)<<GLB_REG_GPIO_2_PD_POS))
#define GLB_REG_GPIO_2_FUNC_SEL                                 GLB_REG_GPIO_2_FUNC_SEL
#define GLB_REG_GPIO_2_FUNC_SEL_POS                             (8U)
#define GLB_REG_GPIO_2_FUNC_SEL_LEN                             (5U)
#define GLB_REG_GPIO_2_FUNC_SEL_MSK                             (((1U<<GLB_REG_GPIO_2_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_2_FUNC_SEL_POS)
#define GLB_REG_GPIO_2_FUNC_SEL_UMSK                            (~(((1U<<GLB_REG_GPIO_2_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_2_FUNC_SEL_POS))
#define GLB_REG_GPIO_2_MODE                                     GLB_REG_GPIO_2_MODE
#define GLB_REG_GPIO_2_MODE_POS                                 (15U)
#define GLB_REG_GPIO_2_MODE_LEN                                 (1U)
#define GLB_REG_GPIO_2_MODE_MSK                                 (((1U<<GLB_REG_GPIO_2_MODE_LEN)-1)<<GLB_REG_GPIO_2_MODE_POS)
#define GLB_REG_GPIO_2_MODE_UMSK                                (~(((1U<<GLB_REG_GPIO_2_MODE_LEN)-1)<<GLB_REG_GPIO_2_MODE_POS))
#define GLB_REG_GPIO_3_IE                                       GLB_REG_GPIO_3_IE
#define GLB_REG_GPIO_3_IE_POS                                   (16U)
#define GLB_REG_GPIO_3_IE_LEN                                   (1U)
#define GLB_REG_GPIO_3_IE_MSK                                   (((1U<<GLB_REG_GPIO_3_IE_LEN)-1)<<GLB_REG_GPIO_3_IE_POS)
#define GLB_REG_GPIO_3_IE_UMSK                                  (~(((1U<<GLB_REG_GPIO_3_IE_LEN)-1)<<GLB_REG_GPIO_3_IE_POS))
#define GLB_REG_GPIO_3_SMT                                      GLB_REG_GPIO_3_SMT
#define GLB_REG_GPIO_3_SMT_POS                                  (17U)
#define GLB_REG_GPIO_3_SMT_LEN                                  (1U)
#define GLB_REG_GPIO_3_SMT_MSK                                  (((1U<<GLB_REG_GPIO_3_SMT_LEN)-1)<<GLB_REG_GPIO_3_SMT_POS)
#define GLB_REG_GPIO_3_SMT_UMSK                                 (~(((1U<<GLB_REG_GPIO_3_SMT_LEN)-1)<<GLB_REG_GPIO_3_SMT_POS))
#define GLB_REG_GPIO_3_DRV                                      GLB_REG_GPIO_3_DRV
#define GLB_REG_GPIO_3_DRV_POS                                  (18U)
#define GLB_REG_GPIO_3_DRV_LEN                                  (2U)
#define GLB_REG_GPIO_3_DRV_MSK                                  (((1U<<GLB_REG_GPIO_3_DRV_LEN)-1)<<GLB_REG_GPIO_3_DRV_POS)
#define GLB_REG_GPIO_3_DRV_UMSK                                 (~(((1U<<GLB_REG_GPIO_3_DRV_LEN)-1)<<GLB_REG_GPIO_3_DRV_POS))
#define GLB_REG_GPIO_3_PU                                       GLB_REG_GPIO_3_PU
#define GLB_REG_GPIO_3_PU_POS                                   (20U)
#define GLB_REG_GPIO_3_PU_LEN                                   (1U)
#define GLB_REG_GPIO_3_PU_MSK                                   (((1U<<GLB_REG_GPIO_3_PU_LEN)-1)<<GLB_REG_GPIO_3_PU_POS)
#define GLB_REG_GPIO_3_PU_UMSK                                  (~(((1U<<GLB_REG_GPIO_3_PU_LEN)-1)<<GLB_REG_GPIO_3_PU_POS))
#define GLB_REG_GPIO_3_PD                                       GLB_REG_GPIO_3_PD
#define GLB_REG_GPIO_3_PD_POS                                   (21U)
#define GLB_REG_GPIO_3_PD_LEN                                   (1U)
#define GLB_REG_GPIO_3_PD_MSK                                   (((1U<<GLB_REG_GPIO_3_PD_LEN)-1)<<GLB_REG_GPIO_3_PD_POS)
#define GLB_REG_GPIO_3_PD_UMSK                                  (~(((1U<<GLB_REG_GPIO_3_PD_LEN)-1)<<GLB_REG_GPIO_3_PD_POS))
#define GLB_REG_GPIO_3_FUNC_SEL                                 GLB_REG_GPIO_3_FUNC_SEL
#define GLB_REG_GPIO_3_FUNC_SEL_POS                             (24U)
#define GLB_REG_GPIO_3_FUNC_SEL_LEN                             (5U)
#define GLB_REG_GPIO_3_FUNC_SEL_MSK                             (((1U<<GLB_REG_GPIO_3_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_3_FUNC_SEL_POS)
#define GLB_REG_GPIO_3_FUNC_SEL_UMSK                            (~(((1U<<GLB_REG_GPIO_3_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_3_FUNC_SEL_POS))
#define GLB_REG_GPIO_3_MODE                                     GLB_REG_GPIO_3_MODE
#define GLB_REG_GPIO_3_MODE_POS                                 (31U)
#define GLB_REG_GPIO_3_MODE_LEN                                 (1U)
#define GLB_REG_GPIO_3_MODE_MSK                                 (((1U<<GLB_REG_GPIO_3_MODE_LEN)-1)<<GLB_REG_GPIO_3_MODE_POS)
#define GLB_REG_GPIO_3_MODE_UMSK                                (~(((1U<<GLB_REG_GPIO_3_MODE_LEN)-1)<<GLB_REG_GPIO_3_MODE_POS))

/* 0x108 : GPIO_CFGCTL2 */
#define GLB_GPIO_CFGCTL2_OFFSET                                 (0x108)

/* 0x10C : GPIO_CFGCTL3 */
#define GLB_GPIO_CFGCTL3_OFFSET                                 (0x10C)
#define GLB_REG_GPIO_7_IE                                       GLB_REG_GPIO_7_IE
#define GLB_REG_GPIO_7_IE_POS                                   (16U)
#define GLB_REG_GPIO_7_IE_LEN                                   (1U)
#define GLB_REG_GPIO_7_IE_MSK                                   (((1U<<GLB_REG_GPIO_7_IE_LEN)-1)<<GLB_REG_GPIO_7_IE_POS)
#define GLB_REG_GPIO_7_IE_UMSK                                  (~(((1U<<GLB_REG_GPIO_7_IE_LEN)-1)<<GLB_REG_GPIO_7_IE_POS))
#define GLB_REG_GPIO_7_SMT                                      GLB_REG_GPIO_7_SMT
#define GLB_REG_GPIO_7_SMT_POS                                  (17U)
#define GLB_REG_GPIO_7_SMT_LEN                                  (1U)
#define GLB_REG_GPIO_7_SMT_MSK                                  (((1U<<GLB_REG_GPIO_7_SMT_LEN)-1)<<GLB_REG_GPIO_7_SMT_POS)
#define GLB_REG_GPIO_7_SMT_UMSK                                 (~(((1U<<GLB_REG_GPIO_7_SMT_LEN)-1)<<GLB_REG_GPIO_7_SMT_POS))
#define GLB_REG_GPIO_7_DRV                                      GLB_REG_GPIO_7_DRV
#define GLB_REG_GPIO_7_DRV_POS                                  (18U)
#define GLB_REG_GPIO_7_DRV_LEN                                  (2U)
#define GLB_REG_GPIO_7_DRV_MSK                                  (((1U<<GLB_REG_GPIO_7_DRV_LEN)-1)<<GLB_REG_GPIO_7_DRV_POS)
#define GLB_REG_GPIO_7_DRV_UMSK                                 (~(((1U<<GLB_REG_GPIO_7_DRV_LEN)-1)<<GLB_REG_GPIO_7_DRV_POS))
#define GLB_REG_GPIO_7_PU                                       GLB_REG_GPIO_7_PU
#define GLB_REG_GPIO_7_PU_POS                                   (20U)
#define GLB_REG_GPIO_7_PU_LEN                                   (1U)
#define GLB_REG_GPIO_7_PU_MSK                                   (((1U<<GLB_REG_GPIO_7_PU_LEN)-1)<<GLB_REG_GPIO_7_PU_POS)
#define GLB_REG_GPIO_7_PU_UMSK                                  (~(((1U<<GLB_REG_GPIO_7_PU_LEN)-1)<<GLB_REG_GPIO_7_PU_POS))
#define GLB_REG_GPIO_7_PD                                       GLB_REG_GPIO_7_PD
#define GLB_REG_GPIO_7_PD_POS                                   (21U)
#define GLB_REG_GPIO_7_PD_LEN                                   (1U)
#define GLB_REG_GPIO_7_PD_MSK                                   (((1U<<GLB_REG_GPIO_7_PD_LEN)-1)<<GLB_REG_GPIO_7_PD_POS)
#define GLB_REG_GPIO_7_PD_UMSK                                  (~(((1U<<GLB_REG_GPIO_7_PD_LEN)-1)<<GLB_REG_GPIO_7_PD_POS))
#define GLB_REG_GPIO_7_FUNC_SEL                                 GLB_REG_GPIO_7_FUNC_SEL
#define GLB_REG_GPIO_7_FUNC_SEL_POS                             (24U)
#define GLB_REG_GPIO_7_FUNC_SEL_LEN                             (5U)
#define GLB_REG_GPIO_7_FUNC_SEL_MSK                             (((1U<<GLB_REG_GPIO_7_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_7_FUNC_SEL_POS)
#define GLB_REG_GPIO_7_FUNC_SEL_UMSK                            (~(((1U<<GLB_REG_GPIO_7_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_7_FUNC_SEL_POS))
#define GLB_REG_GPIO_7_MODE                                     GLB_REG_GPIO_7_MODE
#define GLB_REG_GPIO_7_MODE_POS                                 (31U)
#define GLB_REG_GPIO_7_MODE_LEN                                 (1U)
#define GLB_REG_GPIO_7_MODE_MSK                                 (((1U<<GLB_REG_GPIO_7_MODE_LEN)-1)<<GLB_REG_GPIO_7_MODE_POS)
#define GLB_REG_GPIO_7_MODE_UMSK                                (~(((1U<<GLB_REG_GPIO_7_MODE_LEN)-1)<<GLB_REG_GPIO_7_MODE_POS))

/* 0x110 : GPIO_CFGCTL4 */
#define GLB_GPIO_CFGCTL4_OFFSET                                 (0x110)
#define GLB_REG_GPIO_8_IE                                       GLB_REG_GPIO_8_IE
#define GLB_REG_GPIO_8_IE_POS                                   (0U)
#define GLB_REG_GPIO_8_IE_LEN                                   (1U)
#define GLB_REG_GPIO_8_IE_MSK                                   (((1U<<GLB_REG_GPIO_8_IE_LEN)-1)<<GLB_REG_GPIO_8_IE_POS)
#define GLB_REG_GPIO_8_IE_UMSK                                  (~(((1U<<GLB_REG_GPIO_8_IE_LEN)-1)<<GLB_REG_GPIO_8_IE_POS))
#define GLB_REG_GPIO_8_SMT                                      GLB_REG_GPIO_8_SMT
#define GLB_REG_GPIO_8_SMT_POS                                  (1U)
#define GLB_REG_GPIO_8_SMT_LEN                                  (1U)
#define GLB_REG_GPIO_8_SMT_MSK                                  (((1U<<GLB_REG_GPIO_8_SMT_LEN)-1)<<GLB_REG_GPIO_8_SMT_POS)
#define GLB_REG_GPIO_8_SMT_UMSK                                 (~(((1U<<GLB_REG_GPIO_8_SMT_LEN)-1)<<GLB_REG_GPIO_8_SMT_POS))
#define GLB_REG_GPIO_8_DRV                                      GLB_REG_GPIO_8_DRV
#define GLB_REG_GPIO_8_DRV_POS                                  (2U)
#define GLB_REG_GPIO_8_DRV_LEN                                  (2U)
#define GLB_REG_GPIO_8_DRV_MSK                                  (((1U<<GLB_REG_GPIO_8_DRV_LEN)-1)<<GLB_REG_GPIO_8_DRV_POS)
#define GLB_REG_GPIO_8_DRV_UMSK                                 (~(((1U<<GLB_REG_GPIO_8_DRV_LEN)-1)<<GLB_REG_GPIO_8_DRV_POS))
#define GLB_REG_GPIO_8_PU                                       GLB_REG_GPIO_8_PU
#define GLB_REG_GPIO_8_PU_POS                                   (4U)
#define GLB_REG_GPIO_8_PU_LEN                                   (1U)
#define GLB_REG_GPIO_8_PU_MSK                                   (((1U<<GLB_REG_GPIO_8_PU_LEN)-1)<<GLB_REG_GPIO_8_PU_POS)
#define GLB_REG_GPIO_8_PU_UMSK                                  (~(((1U<<GLB_REG_GPIO_8_PU_LEN)-1)<<GLB_REG_GPIO_8_PU_POS))
#define GLB_REG_GPIO_8_PD                                       GLB_REG_GPIO_8_PD
#define GLB_REG_GPIO_8_PD_POS                                   (5U)
#define GLB_REG_GPIO_8_PD_LEN                                   (1U)
#define GLB_REG_GPIO_8_PD_MSK                                   (((1U<<GLB_REG_GPIO_8_PD_LEN)-1)<<GLB_REG_GPIO_8_PD_POS)
#define GLB_REG_GPIO_8_PD_UMSK                                  (~(((1U<<GLB_REG_GPIO_8_PD_LEN)-1)<<GLB_REG_GPIO_8_PD_POS))
#define GLB_REG_GPIO_8_FUNC_SEL                                 GLB_REG_GPIO_8_FUNC_SEL
#define GLB_REG_GPIO_8_FUNC_SEL_POS                             (8U)
#define GLB_REG_GPIO_8_FUNC_SEL_LEN                             (5U)
#define GLB_REG_GPIO_8_FUNC_SEL_MSK                             (((1U<<GLB_REG_GPIO_8_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_8_FUNC_SEL_POS)
#define GLB_REG_GPIO_8_FUNC_SEL_UMSK                            (~(((1U<<GLB_REG_GPIO_8_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_8_FUNC_SEL_POS))
#define GLB_REG_GPIO_8_MODE                                     GLB_REG_GPIO_8_MODE
#define GLB_REG_GPIO_8_MODE_POS                                 (15U)
#define GLB_REG_GPIO_8_MODE_LEN                                 (1U)
#define GLB_REG_GPIO_8_MODE_MSK                                 (((1U<<GLB_REG_GPIO_8_MODE_LEN)-1)<<GLB_REG_GPIO_8_MODE_POS)
#define GLB_REG_GPIO_8_MODE_UMSK                                (~(((1U<<GLB_REG_GPIO_8_MODE_LEN)-1)<<GLB_REG_GPIO_8_MODE_POS))
#define GLB_REG_GPIO_9_IE                                       GLB_REG_GPIO_9_IE
#define GLB_REG_GPIO_9_IE_POS                                   (16U)
#define GLB_REG_GPIO_9_IE_LEN                                   (1U)
#define GLB_REG_GPIO_9_IE_MSK                                   (((1U<<GLB_REG_GPIO_9_IE_LEN)-1)<<GLB_REG_GPIO_9_IE_POS)
#define GLB_REG_GPIO_9_IE_UMSK                                  (~(((1U<<GLB_REG_GPIO_9_IE_LEN)-1)<<GLB_REG_GPIO_9_IE_POS))
#define GLB_REG_GPIO_9_SMT                                      GLB_REG_GPIO_9_SMT
#define GLB_REG_GPIO_9_SMT_POS                                  (17U)
#define GLB_REG_GPIO_9_SMT_LEN                                  (1U)
#define GLB_REG_GPIO_9_SMT_MSK                                  (((1U<<GLB_REG_GPIO_9_SMT_LEN)-1)<<GLB_REG_GPIO_9_SMT_POS)
#define GLB_REG_GPIO_9_SMT_UMSK                                 (~(((1U<<GLB_REG_GPIO_9_SMT_LEN)-1)<<GLB_REG_GPIO_9_SMT_POS))
#define GLB_REG_GPIO_9_DRV                                      GLB_REG_GPIO_9_DRV
#define GLB_REG_GPIO_9_DRV_POS                                  (18U)
#define GLB_REG_GPIO_9_DRV_LEN                                  (2U)
#define GLB_REG_GPIO_9_DRV_MSK                                  (((1U<<GLB_REG_GPIO_9_DRV_LEN)-1)<<GLB_REG_GPIO_9_DRV_POS)
#define GLB_REG_GPIO_9_DRV_UMSK                                 (~(((1U<<GLB_REG_GPIO_9_DRV_LEN)-1)<<GLB_REG_GPIO_9_DRV_POS))
#define GLB_REG_GPIO_9_PU                                       GLB_REG_GPIO_9_PU
#define GLB_REG_GPIO_9_PU_POS                                   (20U)
#define GLB_REG_GPIO_9_PU_LEN                                   (1U)
#define GLB_REG_GPIO_9_PU_MSK                                   (((1U<<GLB_REG_GPIO_9_PU_LEN)-1)<<GLB_REG_GPIO_9_PU_POS)
#define GLB_REG_GPIO_9_PU_UMSK                                  (~(((1U<<GLB_REG_GPIO_9_PU_LEN)-1)<<GLB_REG_GPIO_9_PU_POS))
#define GLB_REG_GPIO_9_PD                                       GLB_REG_GPIO_9_PD
#define GLB_REG_GPIO_9_PD_POS                                   (21U)
#define GLB_REG_GPIO_9_PD_LEN                                   (1U)
#define GLB_REG_GPIO_9_PD_MSK                                   (((1U<<GLB_REG_GPIO_9_PD_LEN)-1)<<GLB_REG_GPIO_9_PD_POS)
#define GLB_REG_GPIO_9_PD_UMSK                                  (~(((1U<<GLB_REG_GPIO_9_PD_LEN)-1)<<GLB_REG_GPIO_9_PD_POS))
#define GLB_REG_GPIO_9_FUNC_SEL                                 GLB_REG_GPIO_9_FUNC_SEL
#define GLB_REG_GPIO_9_FUNC_SEL_POS                             (24U)
#define GLB_REG_GPIO_9_FUNC_SEL_LEN                             (5U)
#define GLB_REG_GPIO_9_FUNC_SEL_MSK                             (((1U<<GLB_REG_GPIO_9_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_9_FUNC_SEL_POS)
#define GLB_REG_GPIO_9_FUNC_SEL_UMSK                            (~(((1U<<GLB_REG_GPIO_9_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_9_FUNC_SEL_POS))
#define GLB_REG_GPIO_9_MODE                                     GLB_REG_GPIO_9_MODE
#define GLB_REG_GPIO_9_MODE_POS                                 (31U)
#define GLB_REG_GPIO_9_MODE_LEN                                 (1U)
#define GLB_REG_GPIO_9_MODE_MSK                                 (((1U<<GLB_REG_GPIO_9_MODE_LEN)-1)<<GLB_REG_GPIO_9_MODE_POS)
#define GLB_REG_GPIO_9_MODE_UMSK                                (~(((1U<<GLB_REG_GPIO_9_MODE_LEN)-1)<<GLB_REG_GPIO_9_MODE_POS))

/* 0x114 : GPIO_CFGCTL5 */
#define GLB_GPIO_CFGCTL5_OFFSET                                 (0x114)
#define GLB_REG_GPIO_10_IE                                      GLB_REG_GPIO_10_IE
#define GLB_REG_GPIO_10_IE_POS                                  (0U)
#define GLB_REG_GPIO_10_IE_LEN                                  (1U)
#define GLB_REG_GPIO_10_IE_MSK                                  (((1U<<GLB_REG_GPIO_10_IE_LEN)-1)<<GLB_REG_GPIO_10_IE_POS)
#define GLB_REG_GPIO_10_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_10_IE_LEN)-1)<<GLB_REG_GPIO_10_IE_POS))
#define GLB_REG_GPIO_10_SMT                                     GLB_REG_GPIO_10_SMT
#define GLB_REG_GPIO_10_SMT_POS                                 (1U)
#define GLB_REG_GPIO_10_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_10_SMT_MSK                                 (((1U<<GLB_REG_GPIO_10_SMT_LEN)-1)<<GLB_REG_GPIO_10_SMT_POS)
#define GLB_REG_GPIO_10_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_10_SMT_LEN)-1)<<GLB_REG_GPIO_10_SMT_POS))
#define GLB_REG_GPIO_10_DRV                                     GLB_REG_GPIO_10_DRV
#define GLB_REG_GPIO_10_DRV_POS                                 (2U)
#define GLB_REG_GPIO_10_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_10_DRV_MSK                                 (((1U<<GLB_REG_GPIO_10_DRV_LEN)-1)<<GLB_REG_GPIO_10_DRV_POS)
#define GLB_REG_GPIO_10_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_10_DRV_LEN)-1)<<GLB_REG_GPIO_10_DRV_POS))
#define GLB_REG_GPIO_10_PU                                      GLB_REG_GPIO_10_PU
#define GLB_REG_GPIO_10_PU_POS                                  (4U)
#define GLB_REG_GPIO_10_PU_LEN                                  (1U)
#define GLB_REG_GPIO_10_PU_MSK                                  (((1U<<GLB_REG_GPIO_10_PU_LEN)-1)<<GLB_REG_GPIO_10_PU_POS)
#define GLB_REG_GPIO_10_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_10_PU_LEN)-1)<<GLB_REG_GPIO_10_PU_POS))
#define GLB_REG_GPIO_10_PD                                      GLB_REG_GPIO_10_PD
#define GLB_REG_GPIO_10_PD_POS                                  (5U)
#define GLB_REG_GPIO_10_PD_LEN                                  (1U)
#define GLB_REG_GPIO_10_PD_MSK                                  (((1U<<GLB_REG_GPIO_10_PD_LEN)-1)<<GLB_REG_GPIO_10_PD_POS)
#define GLB_REG_GPIO_10_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_10_PD_LEN)-1)<<GLB_REG_GPIO_10_PD_POS))
#define GLB_REG_GPIO_10_FUNC_SEL                                GLB_REG_GPIO_10_FUNC_SEL
#define GLB_REG_GPIO_10_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_10_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_10_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_10_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_10_FUNC_SEL_POS)
#define GLB_REG_GPIO_10_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_10_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_10_FUNC_SEL_POS))
#define GLB_REG_GPIO_10_MODE                                    GLB_REG_GPIO_10_MODE
#define GLB_REG_GPIO_10_MODE_POS                                (15U)
#define GLB_REG_GPIO_10_MODE_LEN                                (1U)
#define GLB_REG_GPIO_10_MODE_MSK                                (((1U<<GLB_REG_GPIO_10_MODE_LEN)-1)<<GLB_REG_GPIO_10_MODE_POS)
#define GLB_REG_GPIO_10_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_10_MODE_LEN)-1)<<GLB_REG_GPIO_10_MODE_POS))
#define GLB_REG_GPIO_11_IE                                      GLB_REG_GPIO_11_IE
#define GLB_REG_GPIO_11_IE_POS                                  (16U)
#define GLB_REG_GPIO_11_IE_LEN                                  (1U)
#define GLB_REG_GPIO_11_IE_MSK                                  (((1U<<GLB_REG_GPIO_11_IE_LEN)-1)<<GLB_REG_GPIO_11_IE_POS)
#define GLB_REG_GPIO_11_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_11_IE_LEN)-1)<<GLB_REG_GPIO_11_IE_POS))
#define GLB_REG_GPIO_11_SMT                                     GLB_REG_GPIO_11_SMT
#define GLB_REG_GPIO_11_SMT_POS                                 (17U)
#define GLB_REG_GPIO_11_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_11_SMT_MSK                                 (((1U<<GLB_REG_GPIO_11_SMT_LEN)-1)<<GLB_REG_GPIO_11_SMT_POS)
#define GLB_REG_GPIO_11_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_11_SMT_LEN)-1)<<GLB_REG_GPIO_11_SMT_POS))
#define GLB_REG_GPIO_11_DRV                                     GLB_REG_GPIO_11_DRV
#define GLB_REG_GPIO_11_DRV_POS                                 (18U)
#define GLB_REG_GPIO_11_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_11_DRV_MSK                                 (((1U<<GLB_REG_GPIO_11_DRV_LEN)-1)<<GLB_REG_GPIO_11_DRV_POS)
#define GLB_REG_GPIO_11_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_11_DRV_LEN)-1)<<GLB_REG_GPIO_11_DRV_POS))
#define GLB_REG_GPIO_11_PU                                      GLB_REG_GPIO_11_PU
#define GLB_REG_GPIO_11_PU_POS                                  (20U)
#define GLB_REG_GPIO_11_PU_LEN                                  (1U)
#define GLB_REG_GPIO_11_PU_MSK                                  (((1U<<GLB_REG_GPIO_11_PU_LEN)-1)<<GLB_REG_GPIO_11_PU_POS)
#define GLB_REG_GPIO_11_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_11_PU_LEN)-1)<<GLB_REG_GPIO_11_PU_POS))
#define GLB_REG_GPIO_11_PD                                      GLB_REG_GPIO_11_PD
#define GLB_REG_GPIO_11_PD_POS                                  (21U)
#define GLB_REG_GPIO_11_PD_LEN                                  (1U)
#define GLB_REG_GPIO_11_PD_MSK                                  (((1U<<GLB_REG_GPIO_11_PD_LEN)-1)<<GLB_REG_GPIO_11_PD_POS)
#define GLB_REG_GPIO_11_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_11_PD_LEN)-1)<<GLB_REG_GPIO_11_PD_POS))
#define GLB_REG_GPIO_11_FUNC_SEL                                GLB_REG_GPIO_11_FUNC_SEL
#define GLB_REG_GPIO_11_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_11_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_11_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_11_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_11_FUNC_SEL_POS)
#define GLB_REG_GPIO_11_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_11_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_11_FUNC_SEL_POS))
#define GLB_REG_GPIO_11_MODE                                    GLB_REG_GPIO_11_MODE
#define GLB_REG_GPIO_11_MODE_POS                                (31U)
#define GLB_REG_GPIO_11_MODE_LEN                                (1U)
#define GLB_REG_GPIO_11_MODE_MSK                                (((1U<<GLB_REG_GPIO_11_MODE_LEN)-1)<<GLB_REG_GPIO_11_MODE_POS)
#define GLB_REG_GPIO_11_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_11_MODE_LEN)-1)<<GLB_REG_GPIO_11_MODE_POS))

/* 0x118 : GPIO_CFGCTL6 */
#define GLB_GPIO_CFGCTL6_OFFSET                                 (0x118)
#define GLB_REG_GPIO_12_IE                                      GLB_REG_GPIO_12_IE
#define GLB_REG_GPIO_12_IE_POS                                  (0U)
#define GLB_REG_GPIO_12_IE_LEN                                  (1U)
#define GLB_REG_GPIO_12_IE_MSK                                  (((1U<<GLB_REG_GPIO_12_IE_LEN)-1)<<GLB_REG_GPIO_12_IE_POS)
#define GLB_REG_GPIO_12_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_12_IE_LEN)-1)<<GLB_REG_GPIO_12_IE_POS))
#define GLB_REG_GPIO_12_SMT                                     GLB_REG_GPIO_12_SMT
#define GLB_REG_GPIO_12_SMT_POS                                 (1U)
#define GLB_REG_GPIO_12_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_12_SMT_MSK                                 (((1U<<GLB_REG_GPIO_12_SMT_LEN)-1)<<GLB_REG_GPIO_12_SMT_POS)
#define GLB_REG_GPIO_12_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_12_SMT_LEN)-1)<<GLB_REG_GPIO_12_SMT_POS))
#define GLB_REG_GPIO_12_DRV                                     GLB_REG_GPIO_12_DRV
#define GLB_REG_GPIO_12_DRV_POS                                 (2U)
#define GLB_REG_GPIO_12_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_12_DRV_MSK                                 (((1U<<GLB_REG_GPIO_12_DRV_LEN)-1)<<GLB_REG_GPIO_12_DRV_POS)
#define GLB_REG_GPIO_12_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_12_DRV_LEN)-1)<<GLB_REG_GPIO_12_DRV_POS))
#define GLB_REG_GPIO_12_PU                                      GLB_REG_GPIO_12_PU
#define GLB_REG_GPIO_12_PU_POS                                  (4U)
#define GLB_REG_GPIO_12_PU_LEN                                  (1U)
#define GLB_REG_GPIO_12_PU_MSK                                  (((1U<<GLB_REG_GPIO_12_PU_LEN)-1)<<GLB_REG_GPIO_12_PU_POS)
#define GLB_REG_GPIO_12_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_12_PU_LEN)-1)<<GLB_REG_GPIO_12_PU_POS))
#define GLB_REG_GPIO_12_PD                                      GLB_REG_GPIO_12_PD
#define GLB_REG_GPIO_12_PD_POS                                  (5U)
#define GLB_REG_GPIO_12_PD_LEN                                  (1U)
#define GLB_REG_GPIO_12_PD_MSK                                  (((1U<<GLB_REG_GPIO_12_PD_LEN)-1)<<GLB_REG_GPIO_12_PD_POS)
#define GLB_REG_GPIO_12_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_12_PD_LEN)-1)<<GLB_REG_GPIO_12_PD_POS))
#define GLB_REG_GPIO_12_FUNC_SEL                                GLB_REG_GPIO_12_FUNC_SEL
#define GLB_REG_GPIO_12_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_12_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_12_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_12_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_12_FUNC_SEL_POS)
#define GLB_REG_GPIO_12_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_12_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_12_FUNC_SEL_POS))
#define GLB_REG_GPIO_12_MODE                                    GLB_REG_GPIO_12_MODE
#define GLB_REG_GPIO_12_MODE_POS                                (15U)
#define GLB_REG_GPIO_12_MODE_LEN                                (1U)
#define GLB_REG_GPIO_12_MODE_MSK                                (((1U<<GLB_REG_GPIO_12_MODE_LEN)-1)<<GLB_REG_GPIO_12_MODE_POS)
#define GLB_REG_GPIO_12_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_12_MODE_LEN)-1)<<GLB_REG_GPIO_12_MODE_POS))
#define GLB_REG_GPIO_13_IE                                      GLB_REG_GPIO_13_IE
#define GLB_REG_GPIO_13_IE_POS                                  (16U)
#define GLB_REG_GPIO_13_IE_LEN                                  (1U)
#define GLB_REG_GPIO_13_IE_MSK                                  (((1U<<GLB_REG_GPIO_13_IE_LEN)-1)<<GLB_REG_GPIO_13_IE_POS)
#define GLB_REG_GPIO_13_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_13_IE_LEN)-1)<<GLB_REG_GPIO_13_IE_POS))
#define GLB_REG_GPIO_13_SMT                                     GLB_REG_GPIO_13_SMT
#define GLB_REG_GPIO_13_SMT_POS                                 (17U)
#define GLB_REG_GPIO_13_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_13_SMT_MSK                                 (((1U<<GLB_REG_GPIO_13_SMT_LEN)-1)<<GLB_REG_GPIO_13_SMT_POS)
#define GLB_REG_GPIO_13_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_13_SMT_LEN)-1)<<GLB_REG_GPIO_13_SMT_POS))
#define GLB_REG_GPIO_13_DRV                                     GLB_REG_GPIO_13_DRV
#define GLB_REG_GPIO_13_DRV_POS                                 (18U)
#define GLB_REG_GPIO_13_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_13_DRV_MSK                                 (((1U<<GLB_REG_GPIO_13_DRV_LEN)-1)<<GLB_REG_GPIO_13_DRV_POS)
#define GLB_REG_GPIO_13_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_13_DRV_LEN)-1)<<GLB_REG_GPIO_13_DRV_POS))
#define GLB_REG_GPIO_13_PU                                      GLB_REG_GPIO_13_PU
#define GLB_REG_GPIO_13_PU_POS                                  (20U)
#define GLB_REG_GPIO_13_PU_LEN                                  (1U)
#define GLB_REG_GPIO_13_PU_MSK                                  (((1U<<GLB_REG_GPIO_13_PU_LEN)-1)<<GLB_REG_GPIO_13_PU_POS)
#define GLB_REG_GPIO_13_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_13_PU_LEN)-1)<<GLB_REG_GPIO_13_PU_POS))
#define GLB_REG_GPIO_13_PD                                      GLB_REG_GPIO_13_PD
#define GLB_REG_GPIO_13_PD_POS                                  (21U)
#define GLB_REG_GPIO_13_PD_LEN                                  (1U)
#define GLB_REG_GPIO_13_PD_MSK                                  (((1U<<GLB_REG_GPIO_13_PD_LEN)-1)<<GLB_REG_GPIO_13_PD_POS)
#define GLB_REG_GPIO_13_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_13_PD_LEN)-1)<<GLB_REG_GPIO_13_PD_POS))
#define GLB_REG_GPIO_13_FUNC_SEL                                GLB_REG_GPIO_13_FUNC_SEL
#define GLB_REG_GPIO_13_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_13_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_13_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_13_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_13_FUNC_SEL_POS)
#define GLB_REG_GPIO_13_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_13_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_13_FUNC_SEL_POS))
#define GLB_REG_GPIO_13_MODE                                    GLB_REG_GPIO_13_MODE
#define GLB_REG_GPIO_13_MODE_POS                                (31U)
#define GLB_REG_GPIO_13_MODE_LEN                                (1U)
#define GLB_REG_GPIO_13_MODE_MSK                                (((1U<<GLB_REG_GPIO_13_MODE_LEN)-1)<<GLB_REG_GPIO_13_MODE_POS)
#define GLB_REG_GPIO_13_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_13_MODE_LEN)-1)<<GLB_REG_GPIO_13_MODE_POS))

/* 0x11C : GPIO_CFGCTL7 */
#define GLB_GPIO_CFGCTL7_OFFSET                                 (0x11C)
#define GLB_REG_GPIO_14_IE                                      GLB_REG_GPIO_14_IE
#define GLB_REG_GPIO_14_IE_POS                                  (0U)
#define GLB_REG_GPIO_14_IE_LEN                                  (1U)
#define GLB_REG_GPIO_14_IE_MSK                                  (((1U<<GLB_REG_GPIO_14_IE_LEN)-1)<<GLB_REG_GPIO_14_IE_POS)
#define GLB_REG_GPIO_14_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_14_IE_LEN)-1)<<GLB_REG_GPIO_14_IE_POS))
#define GLB_REG_GPIO_14_SMT                                     GLB_REG_GPIO_14_SMT
#define GLB_REG_GPIO_14_SMT_POS                                 (1U)
#define GLB_REG_GPIO_14_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_14_SMT_MSK                                 (((1U<<GLB_REG_GPIO_14_SMT_LEN)-1)<<GLB_REG_GPIO_14_SMT_POS)
#define GLB_REG_GPIO_14_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_14_SMT_LEN)-1)<<GLB_REG_GPIO_14_SMT_POS))
#define GLB_REG_GPIO_14_DRV                                     GLB_REG_GPIO_14_DRV
#define GLB_REG_GPIO_14_DRV_POS                                 (2U)
#define GLB_REG_GPIO_14_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_14_DRV_MSK                                 (((1U<<GLB_REG_GPIO_14_DRV_LEN)-1)<<GLB_REG_GPIO_14_DRV_POS)
#define GLB_REG_GPIO_14_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_14_DRV_LEN)-1)<<GLB_REG_GPIO_14_DRV_POS))
#define GLB_REG_GPIO_14_PU                                      GLB_REG_GPIO_14_PU
#define GLB_REG_GPIO_14_PU_POS                                  (4U)
#define GLB_REG_GPIO_14_PU_LEN                                  (1U)
#define GLB_REG_GPIO_14_PU_MSK                                  (((1U<<GLB_REG_GPIO_14_PU_LEN)-1)<<GLB_REG_GPIO_14_PU_POS)
#define GLB_REG_GPIO_14_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_14_PU_LEN)-1)<<GLB_REG_GPIO_14_PU_POS))
#define GLB_REG_GPIO_14_PD                                      GLB_REG_GPIO_14_PD
#define GLB_REG_GPIO_14_PD_POS                                  (5U)
#define GLB_REG_GPIO_14_PD_LEN                                  (1U)
#define GLB_REG_GPIO_14_PD_MSK                                  (((1U<<GLB_REG_GPIO_14_PD_LEN)-1)<<GLB_REG_GPIO_14_PD_POS)
#define GLB_REG_GPIO_14_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_14_PD_LEN)-1)<<GLB_REG_GPIO_14_PD_POS))
#define GLB_REG_GPIO_14_FUNC_SEL                                GLB_REG_GPIO_14_FUNC_SEL
#define GLB_REG_GPIO_14_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_14_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_14_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_14_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_14_FUNC_SEL_POS)
#define GLB_REG_GPIO_14_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_14_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_14_FUNC_SEL_POS))
#define GLB_REG_GPIO_14_MODE                                    GLB_REG_GPIO_14_MODE
#define GLB_REG_GPIO_14_MODE_POS                                (15U)
#define GLB_REG_GPIO_14_MODE_LEN                                (1U)
#define GLB_REG_GPIO_14_MODE_MSK                                (((1U<<GLB_REG_GPIO_14_MODE_LEN)-1)<<GLB_REG_GPIO_14_MODE_POS)
#define GLB_REG_GPIO_14_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_14_MODE_LEN)-1)<<GLB_REG_GPIO_14_MODE_POS))
#define GLB_REG_GPIO_15_IE                                      GLB_REG_GPIO_15_IE
#define GLB_REG_GPIO_15_IE_POS                                  (16U)
#define GLB_REG_GPIO_15_IE_LEN                                  (1U)
#define GLB_REG_GPIO_15_IE_MSK                                  (((1U<<GLB_REG_GPIO_15_IE_LEN)-1)<<GLB_REG_GPIO_15_IE_POS)
#define GLB_REG_GPIO_15_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_15_IE_LEN)-1)<<GLB_REG_GPIO_15_IE_POS))
#define GLB_REG_GPIO_15_SMT                                     GLB_REG_GPIO_15_SMT
#define GLB_REG_GPIO_15_SMT_POS                                 (17U)
#define GLB_REG_GPIO_15_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_15_SMT_MSK                                 (((1U<<GLB_REG_GPIO_15_SMT_LEN)-1)<<GLB_REG_GPIO_15_SMT_POS)
#define GLB_REG_GPIO_15_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_15_SMT_LEN)-1)<<GLB_REG_GPIO_15_SMT_POS))
#define GLB_REG_GPIO_15_DRV                                     GLB_REG_GPIO_15_DRV
#define GLB_REG_GPIO_15_DRV_POS                                 (18U)
#define GLB_REG_GPIO_15_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_15_DRV_MSK                                 (((1U<<GLB_REG_GPIO_15_DRV_LEN)-1)<<GLB_REG_GPIO_15_DRV_POS)
#define GLB_REG_GPIO_15_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_15_DRV_LEN)-1)<<GLB_REG_GPIO_15_DRV_POS))
#define GLB_REG_GPIO_15_PU                                      GLB_REG_GPIO_15_PU
#define GLB_REG_GPIO_15_PU_POS                                  (20U)
#define GLB_REG_GPIO_15_PU_LEN                                  (1U)
#define GLB_REG_GPIO_15_PU_MSK                                  (((1U<<GLB_REG_GPIO_15_PU_LEN)-1)<<GLB_REG_GPIO_15_PU_POS)
#define GLB_REG_GPIO_15_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_15_PU_LEN)-1)<<GLB_REG_GPIO_15_PU_POS))
#define GLB_REG_GPIO_15_PD                                      GLB_REG_GPIO_15_PD
#define GLB_REG_GPIO_15_PD_POS                                  (21U)
#define GLB_REG_GPIO_15_PD_LEN                                  (1U)
#define GLB_REG_GPIO_15_PD_MSK                                  (((1U<<GLB_REG_GPIO_15_PD_LEN)-1)<<GLB_REG_GPIO_15_PD_POS)
#define GLB_REG_GPIO_15_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_15_PD_LEN)-1)<<GLB_REG_GPIO_15_PD_POS))
#define GLB_REG_GPIO_15_FUNC_SEL                                GLB_REG_GPIO_15_FUNC_SEL
#define GLB_REG_GPIO_15_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_15_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_15_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_15_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_15_FUNC_SEL_POS)
#define GLB_REG_GPIO_15_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_15_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_15_FUNC_SEL_POS))
#define GLB_REG_GPIO_15_MODE                                    GLB_REG_GPIO_15_MODE
#define GLB_REG_GPIO_15_MODE_POS                                (31U)
#define GLB_REG_GPIO_15_MODE_LEN                                (1U)
#define GLB_REG_GPIO_15_MODE_MSK                                (((1U<<GLB_REG_GPIO_15_MODE_LEN)-1)<<GLB_REG_GPIO_15_MODE_POS)
#define GLB_REG_GPIO_15_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_15_MODE_LEN)-1)<<GLB_REG_GPIO_15_MODE_POS))

/* 0x120 : GPIO_CFGCTL8 */
#define GLB_GPIO_CFGCTL8_OFFSET                                 (0x120)
#define GLB_REG_GPIO_16_IE                                      GLB_REG_GPIO_16_IE
#define GLB_REG_GPIO_16_IE_POS                                  (0U)
#define GLB_REG_GPIO_16_IE_LEN                                  (1U)
#define GLB_REG_GPIO_16_IE_MSK                                  (((1U<<GLB_REG_GPIO_16_IE_LEN)-1)<<GLB_REG_GPIO_16_IE_POS)
#define GLB_REG_GPIO_16_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_16_IE_LEN)-1)<<GLB_REG_GPIO_16_IE_POS))
#define GLB_REG_GPIO_16_SMT                                     GLB_REG_GPIO_16_SMT
#define GLB_REG_GPIO_16_SMT_POS                                 (1U)
#define GLB_REG_GPIO_16_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_16_SMT_MSK                                 (((1U<<GLB_REG_GPIO_16_SMT_LEN)-1)<<GLB_REG_GPIO_16_SMT_POS)
#define GLB_REG_GPIO_16_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_16_SMT_LEN)-1)<<GLB_REG_GPIO_16_SMT_POS))
#define GLB_REG_GPIO_16_DRV                                     GLB_REG_GPIO_16_DRV
#define GLB_REG_GPIO_16_DRV_POS                                 (2U)
#define GLB_REG_GPIO_16_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_16_DRV_MSK                                 (((1U<<GLB_REG_GPIO_16_DRV_LEN)-1)<<GLB_REG_GPIO_16_DRV_POS)
#define GLB_REG_GPIO_16_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_16_DRV_LEN)-1)<<GLB_REG_GPIO_16_DRV_POS))
#define GLB_REG_GPIO_16_PU                                      GLB_REG_GPIO_16_PU
#define GLB_REG_GPIO_16_PU_POS                                  (4U)
#define GLB_REG_GPIO_16_PU_LEN                                  (1U)
#define GLB_REG_GPIO_16_PU_MSK                                  (((1U<<GLB_REG_GPIO_16_PU_LEN)-1)<<GLB_REG_GPIO_16_PU_POS)
#define GLB_REG_GPIO_16_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_16_PU_LEN)-1)<<GLB_REG_GPIO_16_PU_POS))
#define GLB_REG_GPIO_16_PD                                      GLB_REG_GPIO_16_PD
#define GLB_REG_GPIO_16_PD_POS                                  (5U)
#define GLB_REG_GPIO_16_PD_LEN                                  (1U)
#define GLB_REG_GPIO_16_PD_MSK                                  (((1U<<GLB_REG_GPIO_16_PD_LEN)-1)<<GLB_REG_GPIO_16_PD_POS)
#define GLB_REG_GPIO_16_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_16_PD_LEN)-1)<<GLB_REG_GPIO_16_PD_POS))
#define GLB_REG_GPIO_16_FUNC_SEL                                GLB_REG_GPIO_16_FUNC_SEL
#define GLB_REG_GPIO_16_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_16_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_16_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_16_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_16_FUNC_SEL_POS)
#define GLB_REG_GPIO_16_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_16_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_16_FUNC_SEL_POS))
#define GLB_REG_GPIO_16_MODE                                    GLB_REG_GPIO_16_MODE
#define GLB_REG_GPIO_16_MODE_POS                                (15U)
#define GLB_REG_GPIO_16_MODE_LEN                                (1U)
#define GLB_REG_GPIO_16_MODE_MSK                                (((1U<<GLB_REG_GPIO_16_MODE_LEN)-1)<<GLB_REG_GPIO_16_MODE_POS)
#define GLB_REG_GPIO_16_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_16_MODE_LEN)-1)<<GLB_REG_GPIO_16_MODE_POS))
#define GLB_REG_GPIO_17_IE                                      GLB_REG_GPIO_17_IE
#define GLB_REG_GPIO_17_IE_POS                                  (16U)
#define GLB_REG_GPIO_17_IE_LEN                                  (1U)
#define GLB_REG_GPIO_17_IE_MSK                                  (((1U<<GLB_REG_GPIO_17_IE_LEN)-1)<<GLB_REG_GPIO_17_IE_POS)
#define GLB_REG_GPIO_17_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_17_IE_LEN)-1)<<GLB_REG_GPIO_17_IE_POS))
#define GLB_REG_GPIO_17_SMT                                     GLB_REG_GPIO_17_SMT
#define GLB_REG_GPIO_17_SMT_POS                                 (17U)
#define GLB_REG_GPIO_17_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_17_SMT_MSK                                 (((1U<<GLB_REG_GPIO_17_SMT_LEN)-1)<<GLB_REG_GPIO_17_SMT_POS)
#define GLB_REG_GPIO_17_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_17_SMT_LEN)-1)<<GLB_REG_GPIO_17_SMT_POS))
#define GLB_REG_GPIO_17_DRV                                     GLB_REG_GPIO_17_DRV
#define GLB_REG_GPIO_17_DRV_POS                                 (18U)
#define GLB_REG_GPIO_17_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_17_DRV_MSK                                 (((1U<<GLB_REG_GPIO_17_DRV_LEN)-1)<<GLB_REG_GPIO_17_DRV_POS)
#define GLB_REG_GPIO_17_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_17_DRV_LEN)-1)<<GLB_REG_GPIO_17_DRV_POS))
#define GLB_REG_GPIO_17_PU                                      GLB_REG_GPIO_17_PU
#define GLB_REG_GPIO_17_PU_POS                                  (20U)
#define GLB_REG_GPIO_17_PU_LEN                                  (1U)
#define GLB_REG_GPIO_17_PU_MSK                                  (((1U<<GLB_REG_GPIO_17_PU_LEN)-1)<<GLB_REG_GPIO_17_PU_POS)
#define GLB_REG_GPIO_17_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_17_PU_LEN)-1)<<GLB_REG_GPIO_17_PU_POS))
#define GLB_REG_GPIO_17_PD                                      GLB_REG_GPIO_17_PD
#define GLB_REG_GPIO_17_PD_POS                                  (21U)
#define GLB_REG_GPIO_17_PD_LEN                                  (1U)
#define GLB_REG_GPIO_17_PD_MSK                                  (((1U<<GLB_REG_GPIO_17_PD_LEN)-1)<<GLB_REG_GPIO_17_PD_POS)
#define GLB_REG_GPIO_17_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_17_PD_LEN)-1)<<GLB_REG_GPIO_17_PD_POS))
#define GLB_REG_GPIO_17_FUNC_SEL                                GLB_REG_GPIO_17_FUNC_SEL
#define GLB_REG_GPIO_17_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_17_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_17_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_17_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_17_FUNC_SEL_POS)
#define GLB_REG_GPIO_17_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_17_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_17_FUNC_SEL_POS))
#define GLB_REG_GPIO_17_MODE                                    GLB_REG_GPIO_17_MODE
#define GLB_REG_GPIO_17_MODE_POS                                (31U)
#define GLB_REG_GPIO_17_MODE_LEN                                (1U)
#define GLB_REG_GPIO_17_MODE_MSK                                (((1U<<GLB_REG_GPIO_17_MODE_LEN)-1)<<GLB_REG_GPIO_17_MODE_POS)
#define GLB_REG_GPIO_17_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_17_MODE_LEN)-1)<<GLB_REG_GPIO_17_MODE_POS))

/* 0x124 : GPIO_CFGCTL9 */
#define GLB_GPIO_CFGCTL9_OFFSET                                 (0x124)
#define GLB_REG_GPIO_18_IE                                      GLB_REG_GPIO_18_IE
#define GLB_REG_GPIO_18_IE_POS                                  (0U)
#define GLB_REG_GPIO_18_IE_LEN                                  (1U)
#define GLB_REG_GPIO_18_IE_MSK                                  (((1U<<GLB_REG_GPIO_18_IE_LEN)-1)<<GLB_REG_GPIO_18_IE_POS)
#define GLB_REG_GPIO_18_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_18_IE_LEN)-1)<<GLB_REG_GPIO_18_IE_POS))
#define GLB_REG_GPIO_18_SMT                                     GLB_REG_GPIO_18_SMT
#define GLB_REG_GPIO_18_SMT_POS                                 (1U)
#define GLB_REG_GPIO_18_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_18_SMT_MSK                                 (((1U<<GLB_REG_GPIO_18_SMT_LEN)-1)<<GLB_REG_GPIO_18_SMT_POS)
#define GLB_REG_GPIO_18_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_18_SMT_LEN)-1)<<GLB_REG_GPIO_18_SMT_POS))
#define GLB_REG_GPIO_18_DRV                                     GLB_REG_GPIO_18_DRV
#define GLB_REG_GPIO_18_DRV_POS                                 (2U)
#define GLB_REG_GPIO_18_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_18_DRV_MSK                                 (((1U<<GLB_REG_GPIO_18_DRV_LEN)-1)<<GLB_REG_GPIO_18_DRV_POS)
#define GLB_REG_GPIO_18_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_18_DRV_LEN)-1)<<GLB_REG_GPIO_18_DRV_POS))
#define GLB_REG_GPIO_18_PU                                      GLB_REG_GPIO_18_PU
#define GLB_REG_GPIO_18_PU_POS                                  (4U)
#define GLB_REG_GPIO_18_PU_LEN                                  (1U)
#define GLB_REG_GPIO_18_PU_MSK                                  (((1U<<GLB_REG_GPIO_18_PU_LEN)-1)<<GLB_REG_GPIO_18_PU_POS)
#define GLB_REG_GPIO_18_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_18_PU_LEN)-1)<<GLB_REG_GPIO_18_PU_POS))
#define GLB_REG_GPIO_18_PD                                      GLB_REG_GPIO_18_PD
#define GLB_REG_GPIO_18_PD_POS                                  (5U)
#define GLB_REG_GPIO_18_PD_LEN                                  (1U)
#define GLB_REG_GPIO_18_PD_MSK                                  (((1U<<GLB_REG_GPIO_18_PD_LEN)-1)<<GLB_REG_GPIO_18_PD_POS)
#define GLB_REG_GPIO_18_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_18_PD_LEN)-1)<<GLB_REG_GPIO_18_PD_POS))
#define GLB_REG_GPIO_18_FUNC_SEL                                GLB_REG_GPIO_18_FUNC_SEL
#define GLB_REG_GPIO_18_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_18_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_18_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_18_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_18_FUNC_SEL_POS)
#define GLB_REG_GPIO_18_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_18_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_18_FUNC_SEL_POS))
#define GLB_REG_GPIO_18_MODE                                    GLB_REG_GPIO_18_MODE
#define GLB_REG_GPIO_18_MODE_POS                                (15U)
#define GLB_REG_GPIO_18_MODE_LEN                                (1U)
#define GLB_REG_GPIO_18_MODE_MSK                                (((1U<<GLB_REG_GPIO_18_MODE_LEN)-1)<<GLB_REG_GPIO_18_MODE_POS)
#define GLB_REG_GPIO_18_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_18_MODE_LEN)-1)<<GLB_REG_GPIO_18_MODE_POS))
#define GLB_REG_GPIO_19_IE                                      GLB_REG_GPIO_19_IE
#define GLB_REG_GPIO_19_IE_POS                                  (16U)
#define GLB_REG_GPIO_19_IE_LEN                                  (1U)
#define GLB_REG_GPIO_19_IE_MSK                                  (((1U<<GLB_REG_GPIO_19_IE_LEN)-1)<<GLB_REG_GPIO_19_IE_POS)
#define GLB_REG_GPIO_19_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_19_IE_LEN)-1)<<GLB_REG_GPIO_19_IE_POS))
#define GLB_REG_GPIO_19_SMT                                     GLB_REG_GPIO_19_SMT
#define GLB_REG_GPIO_19_SMT_POS                                 (17U)
#define GLB_REG_GPIO_19_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_19_SMT_MSK                                 (((1U<<GLB_REG_GPIO_19_SMT_LEN)-1)<<GLB_REG_GPIO_19_SMT_POS)
#define GLB_REG_GPIO_19_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_19_SMT_LEN)-1)<<GLB_REG_GPIO_19_SMT_POS))
#define GLB_REG_GPIO_19_DRV                                     GLB_REG_GPIO_19_DRV
#define GLB_REG_GPIO_19_DRV_POS                                 (18U)
#define GLB_REG_GPIO_19_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_19_DRV_MSK                                 (((1U<<GLB_REG_GPIO_19_DRV_LEN)-1)<<GLB_REG_GPIO_19_DRV_POS)
#define GLB_REG_GPIO_19_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_19_DRV_LEN)-1)<<GLB_REG_GPIO_19_DRV_POS))
#define GLB_REG_GPIO_19_PU                                      GLB_REG_GPIO_19_PU
#define GLB_REG_GPIO_19_PU_POS                                  (20U)
#define GLB_REG_GPIO_19_PU_LEN                                  (1U)
#define GLB_REG_GPIO_19_PU_MSK                                  (((1U<<GLB_REG_GPIO_19_PU_LEN)-1)<<GLB_REG_GPIO_19_PU_POS)
#define GLB_REG_GPIO_19_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_19_PU_LEN)-1)<<GLB_REG_GPIO_19_PU_POS))
#define GLB_REG_GPIO_19_PD                                      GLB_REG_GPIO_19_PD
#define GLB_REG_GPIO_19_PD_POS                                  (21U)
#define GLB_REG_GPIO_19_PD_LEN                                  (1U)
#define GLB_REG_GPIO_19_PD_MSK                                  (((1U<<GLB_REG_GPIO_19_PD_LEN)-1)<<GLB_REG_GPIO_19_PD_POS)
#define GLB_REG_GPIO_19_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_19_PD_LEN)-1)<<GLB_REG_GPIO_19_PD_POS))
#define GLB_REG_GPIO_19_FUNC_SEL                                GLB_REG_GPIO_19_FUNC_SEL
#define GLB_REG_GPIO_19_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_19_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_19_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_19_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_19_FUNC_SEL_POS)
#define GLB_REG_GPIO_19_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_19_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_19_FUNC_SEL_POS))
#define GLB_REG_GPIO_19_MODE                                    GLB_REG_GPIO_19_MODE
#define GLB_REG_GPIO_19_MODE_POS                                (31U)
#define GLB_REG_GPIO_19_MODE_LEN                                (1U)
#define GLB_REG_GPIO_19_MODE_MSK                                (((1U<<GLB_REG_GPIO_19_MODE_LEN)-1)<<GLB_REG_GPIO_19_MODE_POS)
#define GLB_REG_GPIO_19_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_19_MODE_LEN)-1)<<GLB_REG_GPIO_19_MODE_POS))

/* 0x128 : GPIO_CFGCTL10 */
#define GLB_GPIO_CFGCTL10_OFFSET                                (0x128)
#define GLB_REG_GPIO_20_IE                                      GLB_REG_GPIO_20_IE
#define GLB_REG_GPIO_20_IE_POS                                  (0U)
#define GLB_REG_GPIO_20_IE_LEN                                  (1U)
#define GLB_REG_GPIO_20_IE_MSK                                  (((1U<<GLB_REG_GPIO_20_IE_LEN)-1)<<GLB_REG_GPIO_20_IE_POS)
#define GLB_REG_GPIO_20_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_20_IE_LEN)-1)<<GLB_REG_GPIO_20_IE_POS))
#define GLB_REG_GPIO_20_SMT                                     GLB_REG_GPIO_20_SMT
#define GLB_REG_GPIO_20_SMT_POS                                 (1U)
#define GLB_REG_GPIO_20_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_20_SMT_MSK                                 (((1U<<GLB_REG_GPIO_20_SMT_LEN)-1)<<GLB_REG_GPIO_20_SMT_POS)
#define GLB_REG_GPIO_20_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_20_SMT_LEN)-1)<<GLB_REG_GPIO_20_SMT_POS))
#define GLB_REG_GPIO_20_DRV                                     GLB_REG_GPIO_20_DRV
#define GLB_REG_GPIO_20_DRV_POS                                 (2U)
#define GLB_REG_GPIO_20_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_20_DRV_MSK                                 (((1U<<GLB_REG_GPIO_20_DRV_LEN)-1)<<GLB_REG_GPIO_20_DRV_POS)
#define GLB_REG_GPIO_20_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_20_DRV_LEN)-1)<<GLB_REG_GPIO_20_DRV_POS))
#define GLB_REG_GPIO_20_PU                                      GLB_REG_GPIO_20_PU
#define GLB_REG_GPIO_20_PU_POS                                  (4U)
#define GLB_REG_GPIO_20_PU_LEN                                  (1U)
#define GLB_REG_GPIO_20_PU_MSK                                  (((1U<<GLB_REG_GPIO_20_PU_LEN)-1)<<GLB_REG_GPIO_20_PU_POS)
#define GLB_REG_GPIO_20_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_20_PU_LEN)-1)<<GLB_REG_GPIO_20_PU_POS))
#define GLB_REG_GPIO_20_PD                                      GLB_REG_GPIO_20_PD
#define GLB_REG_GPIO_20_PD_POS                                  (5U)
#define GLB_REG_GPIO_20_PD_LEN                                  (1U)
#define GLB_REG_GPIO_20_PD_MSK                                  (((1U<<GLB_REG_GPIO_20_PD_LEN)-1)<<GLB_REG_GPIO_20_PD_POS)
#define GLB_REG_GPIO_20_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_20_PD_LEN)-1)<<GLB_REG_GPIO_20_PD_POS))
#define GLB_REG_GPIO_20_FUNC_SEL                                GLB_REG_GPIO_20_FUNC_SEL
#define GLB_REG_GPIO_20_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_20_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_20_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_20_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_20_FUNC_SEL_POS)
#define GLB_REG_GPIO_20_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_20_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_20_FUNC_SEL_POS))
#define GLB_REG_GPIO_20_MODE                                    GLB_REG_GPIO_20_MODE
#define GLB_REG_GPIO_20_MODE_POS                                (15U)
#define GLB_REG_GPIO_20_MODE_LEN                                (1U)
#define GLB_REG_GPIO_20_MODE_MSK                                (((1U<<GLB_REG_GPIO_20_MODE_LEN)-1)<<GLB_REG_GPIO_20_MODE_POS)
#define GLB_REG_GPIO_20_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_20_MODE_LEN)-1)<<GLB_REG_GPIO_20_MODE_POS))
#define GLB_REG_GPIO_21_IE                                      GLB_REG_GPIO_21_IE
#define GLB_REG_GPIO_21_IE_POS                                  (16U)
#define GLB_REG_GPIO_21_IE_LEN                                  (1U)
#define GLB_REG_GPIO_21_IE_MSK                                  (((1U<<GLB_REG_GPIO_21_IE_LEN)-1)<<GLB_REG_GPIO_21_IE_POS)
#define GLB_REG_GPIO_21_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_21_IE_LEN)-1)<<GLB_REG_GPIO_21_IE_POS))
#define GLB_REG_GPIO_21_SMT                                     GLB_REG_GPIO_21_SMT
#define GLB_REG_GPIO_21_SMT_POS                                 (17U)
#define GLB_REG_GPIO_21_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_21_SMT_MSK                                 (((1U<<GLB_REG_GPIO_21_SMT_LEN)-1)<<GLB_REG_GPIO_21_SMT_POS)
#define GLB_REG_GPIO_21_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_21_SMT_LEN)-1)<<GLB_REG_GPIO_21_SMT_POS))
#define GLB_REG_GPIO_21_DRV                                     GLB_REG_GPIO_21_DRV
#define GLB_REG_GPIO_21_DRV_POS                                 (18U)
#define GLB_REG_GPIO_21_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_21_DRV_MSK                                 (((1U<<GLB_REG_GPIO_21_DRV_LEN)-1)<<GLB_REG_GPIO_21_DRV_POS)
#define GLB_REG_GPIO_21_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_21_DRV_LEN)-1)<<GLB_REG_GPIO_21_DRV_POS))
#define GLB_REG_GPIO_21_PU                                      GLB_REG_GPIO_21_PU
#define GLB_REG_GPIO_21_PU_POS                                  (20U)
#define GLB_REG_GPIO_21_PU_LEN                                  (1U)
#define GLB_REG_GPIO_21_PU_MSK                                  (((1U<<GLB_REG_GPIO_21_PU_LEN)-1)<<GLB_REG_GPIO_21_PU_POS)
#define GLB_REG_GPIO_21_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_21_PU_LEN)-1)<<GLB_REG_GPIO_21_PU_POS))
#define GLB_REG_GPIO_21_PD                                      GLB_REG_GPIO_21_PD
#define GLB_REG_GPIO_21_PD_POS                                  (21U)
#define GLB_REG_GPIO_21_PD_LEN                                  (1U)
#define GLB_REG_GPIO_21_PD_MSK                                  (((1U<<GLB_REG_GPIO_21_PD_LEN)-1)<<GLB_REG_GPIO_21_PD_POS)
#define GLB_REG_GPIO_21_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_21_PD_LEN)-1)<<GLB_REG_GPIO_21_PD_POS))
#define GLB_REG_GPIO_21_FUNC_SEL                                GLB_REG_GPIO_21_FUNC_SEL
#define GLB_REG_GPIO_21_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_21_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_21_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_21_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_21_FUNC_SEL_POS)
#define GLB_REG_GPIO_21_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_21_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_21_FUNC_SEL_POS))
#define GLB_REG_GPIO_21_MODE                                    GLB_REG_GPIO_21_MODE
#define GLB_REG_GPIO_21_MODE_POS                                (31U)
#define GLB_REG_GPIO_21_MODE_LEN                                (1U)
#define GLB_REG_GPIO_21_MODE_MSK                                (((1U<<GLB_REG_GPIO_21_MODE_LEN)-1)<<GLB_REG_GPIO_21_MODE_POS)
#define GLB_REG_GPIO_21_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_21_MODE_LEN)-1)<<GLB_REG_GPIO_21_MODE_POS))

/* 0x12C : GPIO_CFGCTL11 */
#define GLB_GPIO_CFGCTL11_OFFSET                                (0x12C)
#define GLB_REG_GPIO_22_IE                                      GLB_REG_GPIO_22_IE
#define GLB_REG_GPIO_22_IE_POS                                  (0U)
#define GLB_REG_GPIO_22_IE_LEN                                  (1U)
#define GLB_REG_GPIO_22_IE_MSK                                  (((1U<<GLB_REG_GPIO_22_IE_LEN)-1)<<GLB_REG_GPIO_22_IE_POS)
#define GLB_REG_GPIO_22_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_22_IE_LEN)-1)<<GLB_REG_GPIO_22_IE_POS))
#define GLB_REG_GPIO_22_SMT                                     GLB_REG_GPIO_22_SMT
#define GLB_REG_GPIO_22_SMT_POS                                 (1U)
#define GLB_REG_GPIO_22_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_22_SMT_MSK                                 (((1U<<GLB_REG_GPIO_22_SMT_LEN)-1)<<GLB_REG_GPIO_22_SMT_POS)
#define GLB_REG_GPIO_22_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_22_SMT_LEN)-1)<<GLB_REG_GPIO_22_SMT_POS))
#define GLB_REG_GPIO_22_DRV                                     GLB_REG_GPIO_22_DRV
#define GLB_REG_GPIO_22_DRV_POS                                 (2U)
#define GLB_REG_GPIO_22_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_22_DRV_MSK                                 (((1U<<GLB_REG_GPIO_22_DRV_LEN)-1)<<GLB_REG_GPIO_22_DRV_POS)
#define GLB_REG_GPIO_22_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_22_DRV_LEN)-1)<<GLB_REG_GPIO_22_DRV_POS))
#define GLB_REG_GPIO_22_PU                                      GLB_REG_GPIO_22_PU
#define GLB_REG_GPIO_22_PU_POS                                  (4U)
#define GLB_REG_GPIO_22_PU_LEN                                  (1U)
#define GLB_REG_GPIO_22_PU_MSK                                  (((1U<<GLB_REG_GPIO_22_PU_LEN)-1)<<GLB_REG_GPIO_22_PU_POS)
#define GLB_REG_GPIO_22_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_22_PU_LEN)-1)<<GLB_REG_GPIO_22_PU_POS))
#define GLB_REG_GPIO_22_PD                                      GLB_REG_GPIO_22_PD
#define GLB_REG_GPIO_22_PD_POS                                  (5U)
#define GLB_REG_GPIO_22_PD_LEN                                  (1U)
#define GLB_REG_GPIO_22_PD_MSK                                  (((1U<<GLB_REG_GPIO_22_PD_LEN)-1)<<GLB_REG_GPIO_22_PD_POS)
#define GLB_REG_GPIO_22_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_22_PD_LEN)-1)<<GLB_REG_GPIO_22_PD_POS))
#define GLB_REG_GPIO_22_FUNC_SEL                                GLB_REG_GPIO_22_FUNC_SEL
#define GLB_REG_GPIO_22_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_22_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_22_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_22_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_22_FUNC_SEL_POS)
#define GLB_REG_GPIO_22_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_22_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_22_FUNC_SEL_POS))
#define GLB_REG_GPIO_22_MODE                                    GLB_REG_GPIO_22_MODE
#define GLB_REG_GPIO_22_MODE_POS                                (15U)
#define GLB_REG_GPIO_22_MODE_LEN                                (1U)
#define GLB_REG_GPIO_22_MODE_MSK                                (((1U<<GLB_REG_GPIO_22_MODE_LEN)-1)<<GLB_REG_GPIO_22_MODE_POS)
#define GLB_REG_GPIO_22_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_22_MODE_LEN)-1)<<GLB_REG_GPIO_22_MODE_POS))
#define GLB_REG_GPIO_23_IE                                      GLB_REG_GPIO_23_IE
#define GLB_REG_GPIO_23_IE_POS                                  (16U)
#define GLB_REG_GPIO_23_IE_LEN                                  (1U)
#define GLB_REG_GPIO_23_IE_MSK                                  (((1U<<GLB_REG_GPIO_23_IE_LEN)-1)<<GLB_REG_GPIO_23_IE_POS)
#define GLB_REG_GPIO_23_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_23_IE_LEN)-1)<<GLB_REG_GPIO_23_IE_POS))
#define GLB_REG_GPIO_23_SMT                                     GLB_REG_GPIO_23_SMT
#define GLB_REG_GPIO_23_SMT_POS                                 (17U)
#define GLB_REG_GPIO_23_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_23_SMT_MSK                                 (((1U<<GLB_REG_GPIO_23_SMT_LEN)-1)<<GLB_REG_GPIO_23_SMT_POS)
#define GLB_REG_GPIO_23_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_23_SMT_LEN)-1)<<GLB_REG_GPIO_23_SMT_POS))
#define GLB_REG_GPIO_23_DRV                                     GLB_REG_GPIO_23_DRV
#define GLB_REG_GPIO_23_DRV_POS                                 (18U)
#define GLB_REG_GPIO_23_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_23_DRV_MSK                                 (((1U<<GLB_REG_GPIO_23_DRV_LEN)-1)<<GLB_REG_GPIO_23_DRV_POS)
#define GLB_REG_GPIO_23_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_23_DRV_LEN)-1)<<GLB_REG_GPIO_23_DRV_POS))
#define GLB_REG_GPIO_23_PU                                      GLB_REG_GPIO_23_PU
#define GLB_REG_GPIO_23_PU_POS                                  (20U)
#define GLB_REG_GPIO_23_PU_LEN                                  (1U)
#define GLB_REG_GPIO_23_PU_MSK                                  (((1U<<GLB_REG_GPIO_23_PU_LEN)-1)<<GLB_REG_GPIO_23_PU_POS)
#define GLB_REG_GPIO_23_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_23_PU_LEN)-1)<<GLB_REG_GPIO_23_PU_POS))
#define GLB_REG_GPIO_23_PD                                      GLB_REG_GPIO_23_PD
#define GLB_REG_GPIO_23_PD_POS                                  (21U)
#define GLB_REG_GPIO_23_PD_LEN                                  (1U)
#define GLB_REG_GPIO_23_PD_MSK                                  (((1U<<GLB_REG_GPIO_23_PD_LEN)-1)<<GLB_REG_GPIO_23_PD_POS)
#define GLB_REG_GPIO_23_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_23_PD_LEN)-1)<<GLB_REG_GPIO_23_PD_POS))
#define GLB_REG_GPIO_23_FUNC_SEL                                GLB_REG_GPIO_23_FUNC_SEL
#define GLB_REG_GPIO_23_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_23_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_23_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_23_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_23_FUNC_SEL_POS)
#define GLB_REG_GPIO_23_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_23_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_23_FUNC_SEL_POS))
#define GLB_REG_GPIO_23_MODE                                    GLB_REG_GPIO_23_MODE
#define GLB_REG_GPIO_23_MODE_POS                                (31U)
#define GLB_REG_GPIO_23_MODE_LEN                                (1U)
#define GLB_REG_GPIO_23_MODE_MSK                                (((1U<<GLB_REG_GPIO_23_MODE_LEN)-1)<<GLB_REG_GPIO_23_MODE_POS)
#define GLB_REG_GPIO_23_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_23_MODE_LEN)-1)<<GLB_REG_GPIO_23_MODE_POS))

/* 0x130 : GPIO_CFGCTL12 */
#define GLB_GPIO_CFGCTL12_OFFSET                                (0x130)
#define GLB_REG_GPIO_24_IE                                      GLB_REG_GPIO_24_IE
#define GLB_REG_GPIO_24_IE_POS                                  (0U)
#define GLB_REG_GPIO_24_IE_LEN                                  (1U)
#define GLB_REG_GPIO_24_IE_MSK                                  (((1U<<GLB_REG_GPIO_24_IE_LEN)-1)<<GLB_REG_GPIO_24_IE_POS)
#define GLB_REG_GPIO_24_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_24_IE_LEN)-1)<<GLB_REG_GPIO_24_IE_POS))
#define GLB_REG_GPIO_24_SMT                                     GLB_REG_GPIO_24_SMT
#define GLB_REG_GPIO_24_SMT_POS                                 (1U)
#define GLB_REG_GPIO_24_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_24_SMT_MSK                                 (((1U<<GLB_REG_GPIO_24_SMT_LEN)-1)<<GLB_REG_GPIO_24_SMT_POS)
#define GLB_REG_GPIO_24_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_24_SMT_LEN)-1)<<GLB_REG_GPIO_24_SMT_POS))
#define GLB_REG_GPIO_24_DRV                                     GLB_REG_GPIO_24_DRV
#define GLB_REG_GPIO_24_DRV_POS                                 (2U)
#define GLB_REG_GPIO_24_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_24_DRV_MSK                                 (((1U<<GLB_REG_GPIO_24_DRV_LEN)-1)<<GLB_REG_GPIO_24_DRV_POS)
#define GLB_REG_GPIO_24_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_24_DRV_LEN)-1)<<GLB_REG_GPIO_24_DRV_POS))
#define GLB_REG_GPIO_24_PU                                      GLB_REG_GPIO_24_PU
#define GLB_REG_GPIO_24_PU_POS                                  (4U)
#define GLB_REG_GPIO_24_PU_LEN                                  (1U)
#define GLB_REG_GPIO_24_PU_MSK                                  (((1U<<GLB_REG_GPIO_24_PU_LEN)-1)<<GLB_REG_GPIO_24_PU_POS)
#define GLB_REG_GPIO_24_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_24_PU_LEN)-1)<<GLB_REG_GPIO_24_PU_POS))
#define GLB_REG_GPIO_24_PD                                      GLB_REG_GPIO_24_PD
#define GLB_REG_GPIO_24_PD_POS                                  (5U)
#define GLB_REG_GPIO_24_PD_LEN                                  (1U)
#define GLB_REG_GPIO_24_PD_MSK                                  (((1U<<GLB_REG_GPIO_24_PD_LEN)-1)<<GLB_REG_GPIO_24_PD_POS)
#define GLB_REG_GPIO_24_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_24_PD_LEN)-1)<<GLB_REG_GPIO_24_PD_POS))
#define GLB_REG_GPIO_24_FUNC_SEL                                GLB_REG_GPIO_24_FUNC_SEL
#define GLB_REG_GPIO_24_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_24_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_24_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_24_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_24_FUNC_SEL_POS)
#define GLB_REG_GPIO_24_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_24_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_24_FUNC_SEL_POS))
#define GLB_REG_GPIO_24_MODE                                    GLB_REG_GPIO_24_MODE
#define GLB_REG_GPIO_24_MODE_POS                                (15U)
#define GLB_REG_GPIO_24_MODE_LEN                                (1U)
#define GLB_REG_GPIO_24_MODE_MSK                                (((1U<<GLB_REG_GPIO_24_MODE_LEN)-1)<<GLB_REG_GPIO_24_MODE_POS)
#define GLB_REG_GPIO_24_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_24_MODE_LEN)-1)<<GLB_REG_GPIO_24_MODE_POS))
#define GLB_REG_GPIO_25_IE                                      GLB_REG_GPIO_25_IE
#define GLB_REG_GPIO_25_IE_POS                                  (16U)
#define GLB_REG_GPIO_25_IE_LEN                                  (1U)
#define GLB_REG_GPIO_25_IE_MSK                                  (((1U<<GLB_REG_GPIO_25_IE_LEN)-1)<<GLB_REG_GPIO_25_IE_POS)
#define GLB_REG_GPIO_25_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_25_IE_LEN)-1)<<GLB_REG_GPIO_25_IE_POS))
#define GLB_REG_GPIO_25_SMT                                     GLB_REG_GPIO_25_SMT
#define GLB_REG_GPIO_25_SMT_POS                                 (17U)
#define GLB_REG_GPIO_25_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_25_SMT_MSK                                 (((1U<<GLB_REG_GPIO_25_SMT_LEN)-1)<<GLB_REG_GPIO_25_SMT_POS)
#define GLB_REG_GPIO_25_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_25_SMT_LEN)-1)<<GLB_REG_GPIO_25_SMT_POS))
#define GLB_REG_GPIO_25_DRV                                     GLB_REG_GPIO_25_DRV
#define GLB_REG_GPIO_25_DRV_POS                                 (18U)
#define GLB_REG_GPIO_25_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_25_DRV_MSK                                 (((1U<<GLB_REG_GPIO_25_DRV_LEN)-1)<<GLB_REG_GPIO_25_DRV_POS)
#define GLB_REG_GPIO_25_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_25_DRV_LEN)-1)<<GLB_REG_GPIO_25_DRV_POS))
#define GLB_REG_GPIO_25_PU                                      GLB_REG_GPIO_25_PU
#define GLB_REG_GPIO_25_PU_POS                                  (20U)
#define GLB_REG_GPIO_25_PU_LEN                                  (1U)
#define GLB_REG_GPIO_25_PU_MSK                                  (((1U<<GLB_REG_GPIO_25_PU_LEN)-1)<<GLB_REG_GPIO_25_PU_POS)
#define GLB_REG_GPIO_25_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_25_PU_LEN)-1)<<GLB_REG_GPIO_25_PU_POS))
#define GLB_REG_GPIO_25_PD                                      GLB_REG_GPIO_25_PD
#define GLB_REG_GPIO_25_PD_POS                                  (21U)
#define GLB_REG_GPIO_25_PD_LEN                                  (1U)
#define GLB_REG_GPIO_25_PD_MSK                                  (((1U<<GLB_REG_GPIO_25_PD_LEN)-1)<<GLB_REG_GPIO_25_PD_POS)
#define GLB_REG_GPIO_25_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_25_PD_LEN)-1)<<GLB_REG_GPIO_25_PD_POS))
#define GLB_REG_GPIO_25_FUNC_SEL                                GLB_REG_GPIO_25_FUNC_SEL
#define GLB_REG_GPIO_25_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_25_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_25_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_25_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_25_FUNC_SEL_POS)
#define GLB_REG_GPIO_25_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_25_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_25_FUNC_SEL_POS))
#define GLB_REG_GPIO_25_MODE                                    GLB_REG_GPIO_25_MODE
#define GLB_REG_GPIO_25_MODE_POS                                (31U)
#define GLB_REG_GPIO_25_MODE_LEN                                (1U)
#define GLB_REG_GPIO_25_MODE_MSK                                (((1U<<GLB_REG_GPIO_25_MODE_LEN)-1)<<GLB_REG_GPIO_25_MODE_POS)
#define GLB_REG_GPIO_25_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_25_MODE_LEN)-1)<<GLB_REG_GPIO_25_MODE_POS))

/* 0x134 : GPIO_CFGCTL13 */
#define GLB_GPIO_CFGCTL13_OFFSET                                (0x134)
#define GLB_REG_GPIO_26_IE                                      GLB_REG_GPIO_26_IE
#define GLB_REG_GPIO_26_IE_POS                                  (0U)
#define GLB_REG_GPIO_26_IE_LEN                                  (1U)
#define GLB_REG_GPIO_26_IE_MSK                                  (((1U<<GLB_REG_GPIO_26_IE_LEN)-1)<<GLB_REG_GPIO_26_IE_POS)
#define GLB_REG_GPIO_26_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_26_IE_LEN)-1)<<GLB_REG_GPIO_26_IE_POS))
#define GLB_REG_GPIO_26_SMT                                     GLB_REG_GPIO_26_SMT
#define GLB_REG_GPIO_26_SMT_POS                                 (1U)
#define GLB_REG_GPIO_26_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_26_SMT_MSK                                 (((1U<<GLB_REG_GPIO_26_SMT_LEN)-1)<<GLB_REG_GPIO_26_SMT_POS)
#define GLB_REG_GPIO_26_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_26_SMT_LEN)-1)<<GLB_REG_GPIO_26_SMT_POS))
#define GLB_REG_GPIO_26_DRV                                     GLB_REG_GPIO_26_DRV
#define GLB_REG_GPIO_26_DRV_POS                                 (2U)
#define GLB_REG_GPIO_26_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_26_DRV_MSK                                 (((1U<<GLB_REG_GPIO_26_DRV_LEN)-1)<<GLB_REG_GPIO_26_DRV_POS)
#define GLB_REG_GPIO_26_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_26_DRV_LEN)-1)<<GLB_REG_GPIO_26_DRV_POS))
#define GLB_REG_GPIO_26_PU                                      GLB_REG_GPIO_26_PU
#define GLB_REG_GPIO_26_PU_POS                                  (4U)
#define GLB_REG_GPIO_26_PU_LEN                                  (1U)
#define GLB_REG_GPIO_26_PU_MSK                                  (((1U<<GLB_REG_GPIO_26_PU_LEN)-1)<<GLB_REG_GPIO_26_PU_POS)
#define GLB_REG_GPIO_26_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_26_PU_LEN)-1)<<GLB_REG_GPIO_26_PU_POS))
#define GLB_REG_GPIO_26_PD                                      GLB_REG_GPIO_26_PD
#define GLB_REG_GPIO_26_PD_POS                                  (5U)
#define GLB_REG_GPIO_26_PD_LEN                                  (1U)
#define GLB_REG_GPIO_26_PD_MSK                                  (((1U<<GLB_REG_GPIO_26_PD_LEN)-1)<<GLB_REG_GPIO_26_PD_POS)
#define GLB_REG_GPIO_26_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_26_PD_LEN)-1)<<GLB_REG_GPIO_26_PD_POS))
#define GLB_REG_GPIO_26_FUNC_SEL                                GLB_REG_GPIO_26_FUNC_SEL
#define GLB_REG_GPIO_26_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_26_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_26_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_26_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_26_FUNC_SEL_POS)
#define GLB_REG_GPIO_26_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_26_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_26_FUNC_SEL_POS))
#define GLB_REG_GPIO_26_MODE                                    GLB_REG_GPIO_26_MODE
#define GLB_REG_GPIO_26_MODE_POS                                (15U)
#define GLB_REG_GPIO_26_MODE_LEN                                (1U)
#define GLB_REG_GPIO_26_MODE_MSK                                (((1U<<GLB_REG_GPIO_26_MODE_LEN)-1)<<GLB_REG_GPIO_26_MODE_POS)
#define GLB_REG_GPIO_26_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_26_MODE_LEN)-1)<<GLB_REG_GPIO_26_MODE_POS))
#define GLB_REG_GPIO_27_IE                                      GLB_REG_GPIO_27_IE
#define GLB_REG_GPIO_27_IE_POS                                  (16U)
#define GLB_REG_GPIO_27_IE_LEN                                  (1U)
#define GLB_REG_GPIO_27_IE_MSK                                  (((1U<<GLB_REG_GPIO_27_IE_LEN)-1)<<GLB_REG_GPIO_27_IE_POS)
#define GLB_REG_GPIO_27_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_27_IE_LEN)-1)<<GLB_REG_GPIO_27_IE_POS))
#define GLB_REG_GPIO_27_SMT                                     GLB_REG_GPIO_27_SMT
#define GLB_REG_GPIO_27_SMT_POS                                 (17U)
#define GLB_REG_GPIO_27_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_27_SMT_MSK                                 (((1U<<GLB_REG_GPIO_27_SMT_LEN)-1)<<GLB_REG_GPIO_27_SMT_POS)
#define GLB_REG_GPIO_27_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_27_SMT_LEN)-1)<<GLB_REG_GPIO_27_SMT_POS))
#define GLB_REG_GPIO_27_DRV                                     GLB_REG_GPIO_27_DRV
#define GLB_REG_GPIO_27_DRV_POS                                 (18U)
#define GLB_REG_GPIO_27_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_27_DRV_MSK                                 (((1U<<GLB_REG_GPIO_27_DRV_LEN)-1)<<GLB_REG_GPIO_27_DRV_POS)
#define GLB_REG_GPIO_27_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_27_DRV_LEN)-1)<<GLB_REG_GPIO_27_DRV_POS))
#define GLB_REG_GPIO_27_PU                                      GLB_REG_GPIO_27_PU
#define GLB_REG_GPIO_27_PU_POS                                  (20U)
#define GLB_REG_GPIO_27_PU_LEN                                  (1U)
#define GLB_REG_GPIO_27_PU_MSK                                  (((1U<<GLB_REG_GPIO_27_PU_LEN)-1)<<GLB_REG_GPIO_27_PU_POS)
#define GLB_REG_GPIO_27_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_27_PU_LEN)-1)<<GLB_REG_GPIO_27_PU_POS))
#define GLB_REG_GPIO_27_PD                                      GLB_REG_GPIO_27_PD
#define GLB_REG_GPIO_27_PD_POS                                  (21U)
#define GLB_REG_GPIO_27_PD_LEN                                  (1U)
#define GLB_REG_GPIO_27_PD_MSK                                  (((1U<<GLB_REG_GPIO_27_PD_LEN)-1)<<GLB_REG_GPIO_27_PD_POS)
#define GLB_REG_GPIO_27_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_27_PD_LEN)-1)<<GLB_REG_GPIO_27_PD_POS))
#define GLB_REG_GPIO_27_FUNC_SEL                                GLB_REG_GPIO_27_FUNC_SEL
#define GLB_REG_GPIO_27_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_27_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_27_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_27_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_27_FUNC_SEL_POS)
#define GLB_REG_GPIO_27_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_27_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_27_FUNC_SEL_POS))
#define GLB_REG_GPIO_27_MODE                                    GLB_REG_GPIO_27_MODE
#define GLB_REG_GPIO_27_MODE_POS                                (31U)
#define GLB_REG_GPIO_27_MODE_LEN                                (1U)
#define GLB_REG_GPIO_27_MODE_MSK                                (((1U<<GLB_REG_GPIO_27_MODE_LEN)-1)<<GLB_REG_GPIO_27_MODE_POS)
#define GLB_REG_GPIO_27_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_27_MODE_LEN)-1)<<GLB_REG_GPIO_27_MODE_POS))

/* 0x138 : GPIO_CFGCTL14 */
#define GLB_GPIO_CFGCTL14_OFFSET                                (0x138)
#define GLB_REG_GPIO_28_IE                                      GLB_REG_GPIO_28_IE
#define GLB_REG_GPIO_28_IE_POS                                  (0U)
#define GLB_REG_GPIO_28_IE_LEN                                  (1U)
#define GLB_REG_GPIO_28_IE_MSK                                  (((1U<<GLB_REG_GPIO_28_IE_LEN)-1)<<GLB_REG_GPIO_28_IE_POS)
#define GLB_REG_GPIO_28_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_28_IE_LEN)-1)<<GLB_REG_GPIO_28_IE_POS))
#define GLB_REG_GPIO_28_SMT                                     GLB_REG_GPIO_28_SMT
#define GLB_REG_GPIO_28_SMT_POS                                 (1U)
#define GLB_REG_GPIO_28_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_28_SMT_MSK                                 (((1U<<GLB_REG_GPIO_28_SMT_LEN)-1)<<GLB_REG_GPIO_28_SMT_POS)
#define GLB_REG_GPIO_28_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_28_SMT_LEN)-1)<<GLB_REG_GPIO_28_SMT_POS))
#define GLB_REG_GPIO_28_DRV                                     GLB_REG_GPIO_28_DRV
#define GLB_REG_GPIO_28_DRV_POS                                 (2U)
#define GLB_REG_GPIO_28_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_28_DRV_MSK                                 (((1U<<GLB_REG_GPIO_28_DRV_LEN)-1)<<GLB_REG_GPIO_28_DRV_POS)
#define GLB_REG_GPIO_28_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_28_DRV_LEN)-1)<<GLB_REG_GPIO_28_DRV_POS))
#define GLB_REG_GPIO_28_PU                                      GLB_REG_GPIO_28_PU
#define GLB_REG_GPIO_28_PU_POS                                  (4U)
#define GLB_REG_GPIO_28_PU_LEN                                  (1U)
#define GLB_REG_GPIO_28_PU_MSK                                  (((1U<<GLB_REG_GPIO_28_PU_LEN)-1)<<GLB_REG_GPIO_28_PU_POS)
#define GLB_REG_GPIO_28_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_28_PU_LEN)-1)<<GLB_REG_GPIO_28_PU_POS))
#define GLB_REG_GPIO_28_PD                                      GLB_REG_GPIO_28_PD
#define GLB_REG_GPIO_28_PD_POS                                  (5U)
#define GLB_REG_GPIO_28_PD_LEN                                  (1U)
#define GLB_REG_GPIO_28_PD_MSK                                  (((1U<<GLB_REG_GPIO_28_PD_LEN)-1)<<GLB_REG_GPIO_28_PD_POS)
#define GLB_REG_GPIO_28_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_28_PD_LEN)-1)<<GLB_REG_GPIO_28_PD_POS))
#define GLB_REG_GPIO_28_FUNC_SEL                                GLB_REG_GPIO_28_FUNC_SEL
#define GLB_REG_GPIO_28_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_28_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_28_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_28_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_28_FUNC_SEL_POS)
#define GLB_REG_GPIO_28_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_28_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_28_FUNC_SEL_POS))
#define GLB_REG_GPIO_28_MODE                                    GLB_REG_GPIO_28_MODE
#define GLB_REG_GPIO_28_MODE_POS                                (15U)
#define GLB_REG_GPIO_28_MODE_LEN                                (1U)
#define GLB_REG_GPIO_28_MODE_MSK                                (((1U<<GLB_REG_GPIO_28_MODE_LEN)-1)<<GLB_REG_GPIO_28_MODE_POS)
#define GLB_REG_GPIO_28_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_28_MODE_LEN)-1)<<GLB_REG_GPIO_28_MODE_POS))
#define GLB_REG_GPIO_29_IE                                      GLB_REG_GPIO_29_IE
#define GLB_REG_GPIO_29_IE_POS                                  (16U)
#define GLB_REG_GPIO_29_IE_LEN                                  (1U)
#define GLB_REG_GPIO_29_IE_MSK                                  (((1U<<GLB_REG_GPIO_29_IE_LEN)-1)<<GLB_REG_GPIO_29_IE_POS)
#define GLB_REG_GPIO_29_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_29_IE_LEN)-1)<<GLB_REG_GPIO_29_IE_POS))
#define GLB_REG_GPIO_29_SMT                                     GLB_REG_GPIO_29_SMT
#define GLB_REG_GPIO_29_SMT_POS                                 (17U)
#define GLB_REG_GPIO_29_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_29_SMT_MSK                                 (((1U<<GLB_REG_GPIO_29_SMT_LEN)-1)<<GLB_REG_GPIO_29_SMT_POS)
#define GLB_REG_GPIO_29_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_29_SMT_LEN)-1)<<GLB_REG_GPIO_29_SMT_POS))
#define GLB_REG_GPIO_29_DRV                                     GLB_REG_GPIO_29_DRV
#define GLB_REG_GPIO_29_DRV_POS                                 (18U)
#define GLB_REG_GPIO_29_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_29_DRV_MSK                                 (((1U<<GLB_REG_GPIO_29_DRV_LEN)-1)<<GLB_REG_GPIO_29_DRV_POS)
#define GLB_REG_GPIO_29_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_29_DRV_LEN)-1)<<GLB_REG_GPIO_29_DRV_POS))
#define GLB_REG_GPIO_29_PU                                      GLB_REG_GPIO_29_PU
#define GLB_REG_GPIO_29_PU_POS                                  (20U)
#define GLB_REG_GPIO_29_PU_LEN                                  (1U)
#define GLB_REG_GPIO_29_PU_MSK                                  (((1U<<GLB_REG_GPIO_29_PU_LEN)-1)<<GLB_REG_GPIO_29_PU_POS)
#define GLB_REG_GPIO_29_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_29_PU_LEN)-1)<<GLB_REG_GPIO_29_PU_POS))
#define GLB_REG_GPIO_29_PD                                      GLB_REG_GPIO_29_PD
#define GLB_REG_GPIO_29_PD_POS                                  (21U)
#define GLB_REG_GPIO_29_PD_LEN                                  (1U)
#define GLB_REG_GPIO_29_PD_MSK                                  (((1U<<GLB_REG_GPIO_29_PD_LEN)-1)<<GLB_REG_GPIO_29_PD_POS)
#define GLB_REG_GPIO_29_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_29_PD_LEN)-1)<<GLB_REG_GPIO_29_PD_POS))
#define GLB_REG_GPIO_29_FUNC_SEL                                GLB_REG_GPIO_29_FUNC_SEL
#define GLB_REG_GPIO_29_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_29_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_29_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_29_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_29_FUNC_SEL_POS)
#define GLB_REG_GPIO_29_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_29_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_29_FUNC_SEL_POS))
#define GLB_REG_GPIO_29_MODE                                    GLB_REG_GPIO_29_MODE
#define GLB_REG_GPIO_29_MODE_POS                                (31U)
#define GLB_REG_GPIO_29_MODE_LEN                                (1U)
#define GLB_REG_GPIO_29_MODE_MSK                                (((1U<<GLB_REG_GPIO_29_MODE_LEN)-1)<<GLB_REG_GPIO_29_MODE_POS)
#define GLB_REG_GPIO_29_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_29_MODE_LEN)-1)<<GLB_REG_GPIO_29_MODE_POS))

/* 0x13C : GPIO_CFGCTL15 */
#define GLB_GPIO_CFGCTL15_OFFSET                                (0x13C)
#define GLB_REG_GPIO_30_IE                                      GLB_REG_GPIO_30_IE
#define GLB_REG_GPIO_30_IE_POS                                  (0U)
#define GLB_REG_GPIO_30_IE_LEN                                  (1U)
#define GLB_REG_GPIO_30_IE_MSK                                  (((1U<<GLB_REG_GPIO_30_IE_LEN)-1)<<GLB_REG_GPIO_30_IE_POS)
#define GLB_REG_GPIO_30_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_30_IE_LEN)-1)<<GLB_REG_GPIO_30_IE_POS))
#define GLB_REG_GPIO_30_SMT                                     GLB_REG_GPIO_30_SMT
#define GLB_REG_GPIO_30_SMT_POS                                 (1U)
#define GLB_REG_GPIO_30_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_30_SMT_MSK                                 (((1U<<GLB_REG_GPIO_30_SMT_LEN)-1)<<GLB_REG_GPIO_30_SMT_POS)
#define GLB_REG_GPIO_30_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_30_SMT_LEN)-1)<<GLB_REG_GPIO_30_SMT_POS))
#define GLB_REG_GPIO_30_DRV                                     GLB_REG_GPIO_30_DRV
#define GLB_REG_GPIO_30_DRV_POS                                 (2U)
#define GLB_REG_GPIO_30_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_30_DRV_MSK                                 (((1U<<GLB_REG_GPIO_30_DRV_LEN)-1)<<GLB_REG_GPIO_30_DRV_POS)
#define GLB_REG_GPIO_30_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_30_DRV_LEN)-1)<<GLB_REG_GPIO_30_DRV_POS))
#define GLB_REG_GPIO_30_PU                                      GLB_REG_GPIO_30_PU
#define GLB_REG_GPIO_30_PU_POS                                  (4U)
#define GLB_REG_GPIO_30_PU_LEN                                  (1U)
#define GLB_REG_GPIO_30_PU_MSK                                  (((1U<<GLB_REG_GPIO_30_PU_LEN)-1)<<GLB_REG_GPIO_30_PU_POS)
#define GLB_REG_GPIO_30_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_30_PU_LEN)-1)<<GLB_REG_GPIO_30_PU_POS))
#define GLB_REG_GPIO_30_PD                                      GLB_REG_GPIO_30_PD
#define GLB_REG_GPIO_30_PD_POS                                  (5U)
#define GLB_REG_GPIO_30_PD_LEN                                  (1U)
#define GLB_REG_GPIO_30_PD_MSK                                  (((1U<<GLB_REG_GPIO_30_PD_LEN)-1)<<GLB_REG_GPIO_30_PD_POS)
#define GLB_REG_GPIO_30_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_30_PD_LEN)-1)<<GLB_REG_GPIO_30_PD_POS))
#define GLB_REG_GPIO_30_FUNC_SEL                                GLB_REG_GPIO_30_FUNC_SEL
#define GLB_REG_GPIO_30_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_30_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_30_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_30_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_30_FUNC_SEL_POS)
#define GLB_REG_GPIO_30_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_30_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_30_FUNC_SEL_POS))
#define GLB_REG_GPIO_30_MODE                                    GLB_REG_GPIO_30_MODE
#define GLB_REG_GPIO_30_MODE_POS                                (15U)
#define GLB_REG_GPIO_30_MODE_LEN                                (1U)
#define GLB_REG_GPIO_30_MODE_MSK                                (((1U<<GLB_REG_GPIO_30_MODE_LEN)-1)<<GLB_REG_GPIO_30_MODE_POS)
#define GLB_REG_GPIO_30_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_30_MODE_LEN)-1)<<GLB_REG_GPIO_30_MODE_POS))
#define GLB_REG_GPIO_31_IE                                      GLB_REG_GPIO_31_IE
#define GLB_REG_GPIO_31_IE_POS                                  (16U)
#define GLB_REG_GPIO_31_IE_LEN                                  (1U)
#define GLB_REG_GPIO_31_IE_MSK                                  (((1U<<GLB_REG_GPIO_31_IE_LEN)-1)<<GLB_REG_GPIO_31_IE_POS)
#define GLB_REG_GPIO_31_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_31_IE_LEN)-1)<<GLB_REG_GPIO_31_IE_POS))
#define GLB_REG_GPIO_31_SMT                                     GLB_REG_GPIO_31_SMT
#define GLB_REG_GPIO_31_SMT_POS                                 (17U)
#define GLB_REG_GPIO_31_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_31_SMT_MSK                                 (((1U<<GLB_REG_GPIO_31_SMT_LEN)-1)<<GLB_REG_GPIO_31_SMT_POS)
#define GLB_REG_GPIO_31_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_31_SMT_LEN)-1)<<GLB_REG_GPIO_31_SMT_POS))
#define GLB_REG_GPIO_31_DRV                                     GLB_REG_GPIO_31_DRV
#define GLB_REG_GPIO_31_DRV_POS                                 (18U)
#define GLB_REG_GPIO_31_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_31_DRV_MSK                                 (((1U<<GLB_REG_GPIO_31_DRV_LEN)-1)<<GLB_REG_GPIO_31_DRV_POS)
#define GLB_REG_GPIO_31_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_31_DRV_LEN)-1)<<GLB_REG_GPIO_31_DRV_POS))
#define GLB_REG_GPIO_31_PU                                      GLB_REG_GPIO_31_PU
#define GLB_REG_GPIO_31_PU_POS                                  (20U)
#define GLB_REG_GPIO_31_PU_LEN                                  (1U)
#define GLB_REG_GPIO_31_PU_MSK                                  (((1U<<GLB_REG_GPIO_31_PU_LEN)-1)<<GLB_REG_GPIO_31_PU_POS)
#define GLB_REG_GPIO_31_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_31_PU_LEN)-1)<<GLB_REG_GPIO_31_PU_POS))
#define GLB_REG_GPIO_31_PD                                      GLB_REG_GPIO_31_PD
#define GLB_REG_GPIO_31_PD_POS                                  (21U)
#define GLB_REG_GPIO_31_PD_LEN                                  (1U)
#define GLB_REG_GPIO_31_PD_MSK                                  (((1U<<GLB_REG_GPIO_31_PD_LEN)-1)<<GLB_REG_GPIO_31_PD_POS)
#define GLB_REG_GPIO_31_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_31_PD_LEN)-1)<<GLB_REG_GPIO_31_PD_POS))
#define GLB_REG_GPIO_31_FUNC_SEL                                GLB_REG_GPIO_31_FUNC_SEL
#define GLB_REG_GPIO_31_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_31_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_31_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_31_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_31_FUNC_SEL_POS)
#define GLB_REG_GPIO_31_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_31_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_31_FUNC_SEL_POS))
#define GLB_REG_GPIO_31_MODE                                    GLB_REG_GPIO_31_MODE
#define GLB_REG_GPIO_31_MODE_POS                                (31U)
#define GLB_REG_GPIO_31_MODE_LEN                                (1U)
#define GLB_REG_GPIO_31_MODE_MSK                                (((1U<<GLB_REG_GPIO_31_MODE_LEN)-1)<<GLB_REG_GPIO_31_MODE_POS)
#define GLB_REG_GPIO_31_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_31_MODE_LEN)-1)<<GLB_REG_GPIO_31_MODE_POS))

/* 0x140 : GPIO_CFGCTL16 */
#define GLB_GPIO_CFGCTL16_OFFSET                                (0x140)
#define GLB_REG_GPIO_32_IE                                      GLB_REG_GPIO_32_IE
#define GLB_REG_GPIO_32_IE_POS                                  (0U)
#define GLB_REG_GPIO_32_IE_LEN                                  (1U)
#define GLB_REG_GPIO_32_IE_MSK                                  (((1U<<GLB_REG_GPIO_32_IE_LEN)-1)<<GLB_REG_GPIO_32_IE_POS)
#define GLB_REG_GPIO_32_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_32_IE_LEN)-1)<<GLB_REG_GPIO_32_IE_POS))
#define GLB_REG_GPIO_32_SMT                                     GLB_REG_GPIO_32_SMT
#define GLB_REG_GPIO_32_SMT_POS                                 (1U)
#define GLB_REG_GPIO_32_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_32_SMT_MSK                                 (((1U<<GLB_REG_GPIO_32_SMT_LEN)-1)<<GLB_REG_GPIO_32_SMT_POS)
#define GLB_REG_GPIO_32_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_32_SMT_LEN)-1)<<GLB_REG_GPIO_32_SMT_POS))
#define GLB_REG_GPIO_32_DRV                                     GLB_REG_GPIO_32_DRV
#define GLB_REG_GPIO_32_DRV_POS                                 (2U)
#define GLB_REG_GPIO_32_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_32_DRV_MSK                                 (((1U<<GLB_REG_GPIO_32_DRV_LEN)-1)<<GLB_REG_GPIO_32_DRV_POS)
#define GLB_REG_GPIO_32_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_32_DRV_LEN)-1)<<GLB_REG_GPIO_32_DRV_POS))
#define GLB_REG_GPIO_32_PU                                      GLB_REG_GPIO_32_PU
#define GLB_REG_GPIO_32_PU_POS                                  (4U)
#define GLB_REG_GPIO_32_PU_LEN                                  (1U)
#define GLB_REG_GPIO_32_PU_MSK                                  (((1U<<GLB_REG_GPIO_32_PU_LEN)-1)<<GLB_REG_GPIO_32_PU_POS)
#define GLB_REG_GPIO_32_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_32_PU_LEN)-1)<<GLB_REG_GPIO_32_PU_POS))
#define GLB_REG_GPIO_32_PD                                      GLB_REG_GPIO_32_PD
#define GLB_REG_GPIO_32_PD_POS                                  (5U)
#define GLB_REG_GPIO_32_PD_LEN                                  (1U)
#define GLB_REG_GPIO_32_PD_MSK                                  (((1U<<GLB_REG_GPIO_32_PD_LEN)-1)<<GLB_REG_GPIO_32_PD_POS)
#define GLB_REG_GPIO_32_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_32_PD_LEN)-1)<<GLB_REG_GPIO_32_PD_POS))
#define GLB_REG_GPIO_33_IE                                      GLB_REG_GPIO_33_IE
#define GLB_REG_GPIO_33_IE_POS                                  (16U)
#define GLB_REG_GPIO_33_IE_LEN                                  (1U)
#define GLB_REG_GPIO_33_IE_MSK                                  (((1U<<GLB_REG_GPIO_33_IE_LEN)-1)<<GLB_REG_GPIO_33_IE_POS)
#define GLB_REG_GPIO_33_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_33_IE_LEN)-1)<<GLB_REG_GPIO_33_IE_POS))
#define GLB_REG_GPIO_33_SMT                                     GLB_REG_GPIO_33_SMT
#define GLB_REG_GPIO_33_SMT_POS                                 (17U)
#define GLB_REG_GPIO_33_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_33_SMT_MSK                                 (((1U<<GLB_REG_GPIO_33_SMT_LEN)-1)<<GLB_REG_GPIO_33_SMT_POS)
#define GLB_REG_GPIO_33_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_33_SMT_LEN)-1)<<GLB_REG_GPIO_33_SMT_POS))
#define GLB_REG_GPIO_33_DRV                                     GLB_REG_GPIO_33_DRV
#define GLB_REG_GPIO_33_DRV_POS                                 (18U)
#define GLB_REG_GPIO_33_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_33_DRV_MSK                                 (((1U<<GLB_REG_GPIO_33_DRV_LEN)-1)<<GLB_REG_GPIO_33_DRV_POS)
#define GLB_REG_GPIO_33_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_33_DRV_LEN)-1)<<GLB_REG_GPIO_33_DRV_POS))
#define GLB_REG_GPIO_33_PU                                      GLB_REG_GPIO_33_PU
#define GLB_REG_GPIO_33_PU_POS                                  (20U)
#define GLB_REG_GPIO_33_PU_LEN                                  (1U)
#define GLB_REG_GPIO_33_PU_MSK                                  (((1U<<GLB_REG_GPIO_33_PU_LEN)-1)<<GLB_REG_GPIO_33_PU_POS)
#define GLB_REG_GPIO_33_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_33_PU_LEN)-1)<<GLB_REG_GPIO_33_PU_POS))
#define GLB_REG_GPIO_33_PD                                      GLB_REG_GPIO_33_PD
#define GLB_REG_GPIO_33_PD_POS                                  (21U)
#define GLB_REG_GPIO_33_PD_LEN                                  (1U)
#define GLB_REG_GPIO_33_PD_MSK                                  (((1U<<GLB_REG_GPIO_33_PD_LEN)-1)<<GLB_REG_GPIO_33_PD_POS)
#define GLB_REG_GPIO_33_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_33_PD_LEN)-1)<<GLB_REG_GPIO_33_PD_POS))

/* 0x144 : GPIO_CFGCTL17 */
#define GLB_GPIO_CFGCTL17_OFFSET                                (0x144)
#define GLB_REG_GPIO_34_IE                                      GLB_REG_GPIO_34_IE
#define GLB_REG_GPIO_34_IE_POS                                  (0U)
#define GLB_REG_GPIO_34_IE_LEN                                  (1U)
#define GLB_REG_GPIO_34_IE_MSK                                  (((1U<<GLB_REG_GPIO_34_IE_LEN)-1)<<GLB_REG_GPIO_34_IE_POS)
#define GLB_REG_GPIO_34_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_34_IE_LEN)-1)<<GLB_REG_GPIO_34_IE_POS))
#define GLB_REG_GPIO_34_SMT                                     GLB_REG_GPIO_34_SMT
#define GLB_REG_GPIO_34_SMT_POS                                 (1U)
#define GLB_REG_GPIO_34_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_34_SMT_MSK                                 (((1U<<GLB_REG_GPIO_34_SMT_LEN)-1)<<GLB_REG_GPIO_34_SMT_POS)
#define GLB_REG_GPIO_34_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_34_SMT_LEN)-1)<<GLB_REG_GPIO_34_SMT_POS))
#define GLB_REG_GPIO_34_DRV                                     GLB_REG_GPIO_34_DRV
#define GLB_REG_GPIO_34_DRV_POS                                 (2U)
#define GLB_REG_GPIO_34_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_34_DRV_MSK                                 (((1U<<GLB_REG_GPIO_34_DRV_LEN)-1)<<GLB_REG_GPIO_34_DRV_POS)
#define GLB_REG_GPIO_34_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_34_DRV_LEN)-1)<<GLB_REG_GPIO_34_DRV_POS))
#define GLB_REG_GPIO_34_PU                                      GLB_REG_GPIO_34_PU
#define GLB_REG_GPIO_34_PU_POS                                  (4U)
#define GLB_REG_GPIO_34_PU_LEN                                  (1U)
#define GLB_REG_GPIO_34_PU_MSK                                  (((1U<<GLB_REG_GPIO_34_PU_LEN)-1)<<GLB_REG_GPIO_34_PU_POS)
#define GLB_REG_GPIO_34_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_34_PU_LEN)-1)<<GLB_REG_GPIO_34_PU_POS))
#define GLB_REG_GPIO_34_PD                                      GLB_REG_GPIO_34_PD
#define GLB_REG_GPIO_34_PD_POS                                  (5U)
#define GLB_REG_GPIO_34_PD_LEN                                  (1U)
#define GLB_REG_GPIO_34_PD_MSK                                  (((1U<<GLB_REG_GPIO_34_PD_LEN)-1)<<GLB_REG_GPIO_34_PD_POS)
#define GLB_REG_GPIO_34_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_34_PD_LEN)-1)<<GLB_REG_GPIO_34_PD_POS))
#define GLB_REG_GPIO_35_IE                                      GLB_REG_GPIO_35_IE
#define GLB_REG_GPIO_35_IE_POS                                  (16U)
#define GLB_REG_GPIO_35_IE_LEN                                  (1U)
#define GLB_REG_GPIO_35_IE_MSK                                  (((1U<<GLB_REG_GPIO_35_IE_LEN)-1)<<GLB_REG_GPIO_35_IE_POS)
#define GLB_REG_GPIO_35_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_35_IE_LEN)-1)<<GLB_REG_GPIO_35_IE_POS))
#define GLB_REG_GPIO_35_SMT                                     GLB_REG_GPIO_35_SMT
#define GLB_REG_GPIO_35_SMT_POS                                 (17U)
#define GLB_REG_GPIO_35_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_35_SMT_MSK                                 (((1U<<GLB_REG_GPIO_35_SMT_LEN)-1)<<GLB_REG_GPIO_35_SMT_POS)
#define GLB_REG_GPIO_35_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_35_SMT_LEN)-1)<<GLB_REG_GPIO_35_SMT_POS))
#define GLB_REG_GPIO_35_DRV                                     GLB_REG_GPIO_35_DRV
#define GLB_REG_GPIO_35_DRV_POS                                 (18U)
#define GLB_REG_GPIO_35_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_35_DRV_MSK                                 (((1U<<GLB_REG_GPIO_35_DRV_LEN)-1)<<GLB_REG_GPIO_35_DRV_POS)
#define GLB_REG_GPIO_35_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_35_DRV_LEN)-1)<<GLB_REG_GPIO_35_DRV_POS))
#define GLB_REG_GPIO_35_PU                                      GLB_REG_GPIO_35_PU
#define GLB_REG_GPIO_35_PU_POS                                  (20U)
#define GLB_REG_GPIO_35_PU_LEN                                  (1U)
#define GLB_REG_GPIO_35_PU_MSK                                  (((1U<<GLB_REG_GPIO_35_PU_LEN)-1)<<GLB_REG_GPIO_35_PU_POS)
#define GLB_REG_GPIO_35_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_35_PU_LEN)-1)<<GLB_REG_GPIO_35_PU_POS))
#define GLB_REG_GPIO_35_PD                                      GLB_REG_GPIO_35_PD
#define GLB_REG_GPIO_35_PD_POS                                  (21U)
#define GLB_REG_GPIO_35_PD_LEN                                  (1U)
#define GLB_REG_GPIO_35_PD_MSK                                  (((1U<<GLB_REG_GPIO_35_PD_LEN)-1)<<GLB_REG_GPIO_35_PD_POS)
#define GLB_REG_GPIO_35_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_35_PD_LEN)-1)<<GLB_REG_GPIO_35_PD_POS))

/* 0x148 : GPIO_CFGCTL18 */
#define GLB_GPIO_CFGCTL18_OFFSET                                (0x148)
#define GLB_REG_GPIO_36_IE                                      GLB_REG_GPIO_36_IE
#define GLB_REG_GPIO_36_IE_POS                                  (0U)
#define GLB_REG_GPIO_36_IE_LEN                                  (1U)
#define GLB_REG_GPIO_36_IE_MSK                                  (((1U<<GLB_REG_GPIO_36_IE_LEN)-1)<<GLB_REG_GPIO_36_IE_POS)
#define GLB_REG_GPIO_36_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_36_IE_LEN)-1)<<GLB_REG_GPIO_36_IE_POS))
#define GLB_REG_GPIO_36_SMT                                     GLB_REG_GPIO_36_SMT
#define GLB_REG_GPIO_36_SMT_POS                                 (1U)
#define GLB_REG_GPIO_36_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_36_SMT_MSK                                 (((1U<<GLB_REG_GPIO_36_SMT_LEN)-1)<<GLB_REG_GPIO_36_SMT_POS)
#define GLB_REG_GPIO_36_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_36_SMT_LEN)-1)<<GLB_REG_GPIO_36_SMT_POS))
#define GLB_REG_GPIO_36_DRV                                     GLB_REG_GPIO_36_DRV
#define GLB_REG_GPIO_36_DRV_POS                                 (2U)
#define GLB_REG_GPIO_36_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_36_DRV_MSK                                 (((1U<<GLB_REG_GPIO_36_DRV_LEN)-1)<<GLB_REG_GPIO_36_DRV_POS)
#define GLB_REG_GPIO_36_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_36_DRV_LEN)-1)<<GLB_REG_GPIO_36_DRV_POS))
#define GLB_REG_GPIO_36_PU                                      GLB_REG_GPIO_36_PU
#define GLB_REG_GPIO_36_PU_POS                                  (4U)
#define GLB_REG_GPIO_36_PU_LEN                                  (1U)
#define GLB_REG_GPIO_36_PU_MSK                                  (((1U<<GLB_REG_GPIO_36_PU_LEN)-1)<<GLB_REG_GPIO_36_PU_POS)
#define GLB_REG_GPIO_36_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_36_PU_LEN)-1)<<GLB_REG_GPIO_36_PU_POS))
#define GLB_REG_GPIO_36_PD                                      GLB_REG_GPIO_36_PD
#define GLB_REG_GPIO_36_PD_POS                                  (5U)
#define GLB_REG_GPIO_36_PD_LEN                                  (1U)
#define GLB_REG_GPIO_36_PD_MSK                                  (((1U<<GLB_REG_GPIO_36_PD_LEN)-1)<<GLB_REG_GPIO_36_PD_POS)
#define GLB_REG_GPIO_36_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_36_PD_LEN)-1)<<GLB_REG_GPIO_36_PD_POS))
#define GLB_REG_GPIO_37_IE                                      GLB_REG_GPIO_37_IE
#define GLB_REG_GPIO_37_IE_POS                                  (16U)
#define GLB_REG_GPIO_37_IE_LEN                                  (1U)
#define GLB_REG_GPIO_37_IE_MSK                                  (((1U<<GLB_REG_GPIO_37_IE_LEN)-1)<<GLB_REG_GPIO_37_IE_POS)
#define GLB_REG_GPIO_37_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_37_IE_LEN)-1)<<GLB_REG_GPIO_37_IE_POS))
#define GLB_REG_GPIO_37_SMT                                     GLB_REG_GPIO_37_SMT
#define GLB_REG_GPIO_37_SMT_POS                                 (17U)
#define GLB_REG_GPIO_37_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_37_SMT_MSK                                 (((1U<<GLB_REG_GPIO_37_SMT_LEN)-1)<<GLB_REG_GPIO_37_SMT_POS)
#define GLB_REG_GPIO_37_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_37_SMT_LEN)-1)<<GLB_REG_GPIO_37_SMT_POS))
#define GLB_REG_GPIO_37_DRV                                     GLB_REG_GPIO_37_DRV
#define GLB_REG_GPIO_37_DRV_POS                                 (18U)
#define GLB_REG_GPIO_37_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_37_DRV_MSK                                 (((1U<<GLB_REG_GPIO_37_DRV_LEN)-1)<<GLB_REG_GPIO_37_DRV_POS)
#define GLB_REG_GPIO_37_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_37_DRV_LEN)-1)<<GLB_REG_GPIO_37_DRV_POS))
#define GLB_REG_GPIO_37_PU                                      GLB_REG_GPIO_37_PU
#define GLB_REG_GPIO_37_PU_POS                                  (20U)
#define GLB_REG_GPIO_37_PU_LEN                                  (1U)
#define GLB_REG_GPIO_37_PU_MSK                                  (((1U<<GLB_REG_GPIO_37_PU_LEN)-1)<<GLB_REG_GPIO_37_PU_POS)
#define GLB_REG_GPIO_37_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_37_PU_LEN)-1)<<GLB_REG_GPIO_37_PU_POS))
#define GLB_REG_GPIO_37_PD                                      GLB_REG_GPIO_37_PD
#define GLB_REG_GPIO_37_PD_POS                                  (21U)
#define GLB_REG_GPIO_37_PD_LEN                                  (1U)
#define GLB_REG_GPIO_37_PD_MSK                                  (((1U<<GLB_REG_GPIO_37_PD_LEN)-1)<<GLB_REG_GPIO_37_PD_POS)
#define GLB_REG_GPIO_37_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_37_PD_LEN)-1)<<GLB_REG_GPIO_37_PD_POS))

/* 0x180 : GPIO_CFGCTL30 */
#define GLB_GPIO_CFGCTL30_OFFSET                                (0x180)
#define GLB_REG_GPIO_0_I                                        GLB_REG_GPIO_0_I
#define GLB_REG_GPIO_0_I_POS                                    (0U)
#define GLB_REG_GPIO_0_I_LEN                                    (1U)
#define GLB_REG_GPIO_0_I_MSK                                    (((1U<<GLB_REG_GPIO_0_I_LEN)-1)<<GLB_REG_GPIO_0_I_POS)
#define GLB_REG_GPIO_0_I_UMSK                                   (~(((1U<<GLB_REG_GPIO_0_I_LEN)-1)<<GLB_REG_GPIO_0_I_POS))
#define GLB_REG_GPIO_1_I                                        GLB_REG_GPIO_1_I
#define GLB_REG_GPIO_1_I_POS                                    (1U)
#define GLB_REG_GPIO_1_I_LEN                                    (1U)
#define GLB_REG_GPIO_1_I_MSK                                    (((1U<<GLB_REG_GPIO_1_I_LEN)-1)<<GLB_REG_GPIO_1_I_POS)
#define GLB_REG_GPIO_1_I_UMSK                                   (~(((1U<<GLB_REG_GPIO_1_I_LEN)-1)<<GLB_REG_GPIO_1_I_POS))
#define GLB_REG_GPIO_2_I                                        GLB_REG_GPIO_2_I
#define GLB_REG_GPIO_2_I_POS                                    (2U)
#define GLB_REG_GPIO_2_I_LEN                                    (1U)
#define GLB_REG_GPIO_2_I_MSK                                    (((1U<<GLB_REG_GPIO_2_I_LEN)-1)<<GLB_REG_GPIO_2_I_POS)
#define GLB_REG_GPIO_2_I_UMSK                                   (~(((1U<<GLB_REG_GPIO_2_I_LEN)-1)<<GLB_REG_GPIO_2_I_POS))
#define GLB_REG_GPIO_3_I                                        GLB_REG_GPIO_3_I
#define GLB_REG_GPIO_3_I_POS                                    (3U)
#define GLB_REG_GPIO_3_I_LEN                                    (1U)
#define GLB_REG_GPIO_3_I_MSK                                    (((1U<<GLB_REG_GPIO_3_I_LEN)-1)<<GLB_REG_GPIO_3_I_POS)
#define GLB_REG_GPIO_3_I_UMSK                                   (~(((1U<<GLB_REG_GPIO_3_I_LEN)-1)<<GLB_REG_GPIO_3_I_POS))
#define GLB_REG_GPIO_7_I                                        GLB_REG_GPIO_7_I
#define GLB_REG_GPIO_7_I_POS                                    (7U)
#define GLB_REG_GPIO_7_I_LEN                                    (1U)
#define GLB_REG_GPIO_7_I_MSK                                    (((1U<<GLB_REG_GPIO_7_I_LEN)-1)<<GLB_REG_GPIO_7_I_POS)
#define GLB_REG_GPIO_7_I_UMSK                                   (~(((1U<<GLB_REG_GPIO_7_I_LEN)-1)<<GLB_REG_GPIO_7_I_POS))
#define GLB_REG_GPIO_8_I                                        GLB_REG_GPIO_8_I
#define GLB_REG_GPIO_8_I_POS                                    (8U)
#define GLB_REG_GPIO_8_I_LEN                                    (1U)
#define GLB_REG_GPIO_8_I_MSK                                    (((1U<<GLB_REG_GPIO_8_I_LEN)-1)<<GLB_REG_GPIO_8_I_POS)
#define GLB_REG_GPIO_8_I_UMSK                                   (~(((1U<<GLB_REG_GPIO_8_I_LEN)-1)<<GLB_REG_GPIO_8_I_POS))
#define GLB_REG_GPIO_9_I                                        GLB_REG_GPIO_9_I
#define GLB_REG_GPIO_9_I_POS                                    (9U)
#define GLB_REG_GPIO_9_I_LEN                                    (1U)
#define GLB_REG_GPIO_9_I_MSK                                    (((1U<<GLB_REG_GPIO_9_I_LEN)-1)<<GLB_REG_GPIO_9_I_POS)
#define GLB_REG_GPIO_9_I_UMSK                                   (~(((1U<<GLB_REG_GPIO_9_I_LEN)-1)<<GLB_REG_GPIO_9_I_POS))
#define GLB_REG_GPIO_10_I                                       GLB_REG_GPIO_10_I
#define GLB_REG_GPIO_10_I_POS                                   (10U)
#define GLB_REG_GPIO_10_I_LEN                                   (1U)
#define GLB_REG_GPIO_10_I_MSK                                   (((1U<<GLB_REG_GPIO_10_I_LEN)-1)<<GLB_REG_GPIO_10_I_POS)
#define GLB_REG_GPIO_10_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_10_I_LEN)-1)<<GLB_REG_GPIO_10_I_POS))
#define GLB_REG_GPIO_11_I                                       GLB_REG_GPIO_11_I
#define GLB_REG_GPIO_11_I_POS                                   (11U)
#define GLB_REG_GPIO_11_I_LEN                                   (1U)
#define GLB_REG_GPIO_11_I_MSK                                   (((1U<<GLB_REG_GPIO_11_I_LEN)-1)<<GLB_REG_GPIO_11_I_POS)
#define GLB_REG_GPIO_11_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_11_I_LEN)-1)<<GLB_REG_GPIO_11_I_POS))
#define GLB_REG_GPIO_12_I                                       GLB_REG_GPIO_12_I
#define GLB_REG_GPIO_12_I_POS                                   (12U)
#define GLB_REG_GPIO_12_I_LEN                                   (1U)
#define GLB_REG_GPIO_12_I_MSK                                   (((1U<<GLB_REG_GPIO_12_I_LEN)-1)<<GLB_REG_GPIO_12_I_POS)
#define GLB_REG_GPIO_12_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_12_I_LEN)-1)<<GLB_REG_GPIO_12_I_POS))
#define GLB_REG_GPIO_13_I                                       GLB_REG_GPIO_13_I
#define GLB_REG_GPIO_13_I_POS                                   (13U)
#define GLB_REG_GPIO_13_I_LEN                                   (1U)
#define GLB_REG_GPIO_13_I_MSK                                   (((1U<<GLB_REG_GPIO_13_I_LEN)-1)<<GLB_REG_GPIO_13_I_POS)
#define GLB_REG_GPIO_13_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_13_I_LEN)-1)<<GLB_REG_GPIO_13_I_POS))
#define GLB_REG_GPIO_14_I                                       GLB_REG_GPIO_14_I
#define GLB_REG_GPIO_14_I_POS                                   (14U)
#define GLB_REG_GPIO_14_I_LEN                                   (1U)
#define GLB_REG_GPIO_14_I_MSK                                   (((1U<<GLB_REG_GPIO_14_I_LEN)-1)<<GLB_REG_GPIO_14_I_POS)
#define GLB_REG_GPIO_14_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_14_I_LEN)-1)<<GLB_REG_GPIO_14_I_POS))
#define GLB_REG_GPIO_15_I                                       GLB_REG_GPIO_15_I
#define GLB_REG_GPIO_15_I_POS                                   (15U)
#define GLB_REG_GPIO_15_I_LEN                                   (1U)
#define GLB_REG_GPIO_15_I_MSK                                   (((1U<<GLB_REG_GPIO_15_I_LEN)-1)<<GLB_REG_GPIO_15_I_POS)
#define GLB_REG_GPIO_15_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_15_I_LEN)-1)<<GLB_REG_GPIO_15_I_POS))
#define GLB_REG_GPIO_16_I                                       GLB_REG_GPIO_16_I
#define GLB_REG_GPIO_16_I_POS                                   (16U)
#define GLB_REG_GPIO_16_I_LEN                                   (1U)
#define GLB_REG_GPIO_16_I_MSK                                   (((1U<<GLB_REG_GPIO_16_I_LEN)-1)<<GLB_REG_GPIO_16_I_POS)
#define GLB_REG_GPIO_16_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_16_I_LEN)-1)<<GLB_REG_GPIO_16_I_POS))
#define GLB_REG_GPIO_17_I                                       GLB_REG_GPIO_17_I
#define GLB_REG_GPIO_17_I_POS                                   (17U)
#define GLB_REG_GPIO_17_I_LEN                                   (1U)
#define GLB_REG_GPIO_17_I_MSK                                   (((1U<<GLB_REG_GPIO_17_I_LEN)-1)<<GLB_REG_GPIO_17_I_POS)
#define GLB_REG_GPIO_17_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_17_I_LEN)-1)<<GLB_REG_GPIO_17_I_POS))
#define GLB_REG_GPIO_18_I                                       GLB_REG_GPIO_18_I
#define GLB_REG_GPIO_18_I_POS                                   (18U)
#define GLB_REG_GPIO_18_I_LEN                                   (1U)
#define GLB_REG_GPIO_18_I_MSK                                   (((1U<<GLB_REG_GPIO_18_I_LEN)-1)<<GLB_REG_GPIO_18_I_POS)
#define GLB_REG_GPIO_18_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_18_I_LEN)-1)<<GLB_REG_GPIO_18_I_POS))
#define GLB_REG_GPIO_19_I                                       GLB_REG_GPIO_19_I
#define GLB_REG_GPIO_19_I_POS                                   (19U)
#define GLB_REG_GPIO_19_I_LEN                                   (1U)
#define GLB_REG_GPIO_19_I_MSK                                   (((1U<<GLB_REG_GPIO_19_I_LEN)-1)<<GLB_REG_GPIO_19_I_POS)
#define GLB_REG_GPIO_19_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_19_I_LEN)-1)<<GLB_REG_GPIO_19_I_POS))
#define GLB_REG_GPIO_20_I                                       GLB_REG_GPIO_20_I
#define GLB_REG_GPIO_20_I_POS                                   (20U)
#define GLB_REG_GPIO_20_I_LEN                                   (1U)
#define GLB_REG_GPIO_20_I_MSK                                   (((1U<<GLB_REG_GPIO_20_I_LEN)-1)<<GLB_REG_GPIO_20_I_POS)
#define GLB_REG_GPIO_20_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_20_I_LEN)-1)<<GLB_REG_GPIO_20_I_POS))
#define GLB_REG_GPIO_21_I                                       GLB_REG_GPIO_21_I
#define GLB_REG_GPIO_21_I_POS                                   (21U)
#define GLB_REG_GPIO_21_I_LEN                                   (1U)
#define GLB_REG_GPIO_21_I_MSK                                   (((1U<<GLB_REG_GPIO_21_I_LEN)-1)<<GLB_REG_GPIO_21_I_POS)
#define GLB_REG_GPIO_21_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_21_I_LEN)-1)<<GLB_REG_GPIO_21_I_POS))
#define GLB_REG_GPIO_22_I                                       GLB_REG_GPIO_22_I
#define GLB_REG_GPIO_22_I_POS                                   (22U)
#define GLB_REG_GPIO_22_I_LEN                                   (1U)
#define GLB_REG_GPIO_22_I_MSK                                   (((1U<<GLB_REG_GPIO_22_I_LEN)-1)<<GLB_REG_GPIO_22_I_POS)
#define GLB_REG_GPIO_22_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_22_I_LEN)-1)<<GLB_REG_GPIO_22_I_POS))
#define GLB_REG_GPIO_23_I                                       GLB_REG_GPIO_23_I
#define GLB_REG_GPIO_23_I_POS                                   (23U)
#define GLB_REG_GPIO_23_I_LEN                                   (1U)
#define GLB_REG_GPIO_23_I_MSK                                   (((1U<<GLB_REG_GPIO_23_I_LEN)-1)<<GLB_REG_GPIO_23_I_POS)
#define GLB_REG_GPIO_23_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_23_I_LEN)-1)<<GLB_REG_GPIO_23_I_POS))
#define GLB_REG_GPIO_24_I                                       GLB_REG_GPIO_24_I
#define GLB_REG_GPIO_24_I_POS                                   (24U)
#define GLB_REG_GPIO_24_I_LEN                                   (1U)
#define GLB_REG_GPIO_24_I_MSK                                   (((1U<<GLB_REG_GPIO_24_I_LEN)-1)<<GLB_REG_GPIO_24_I_POS)
#define GLB_REG_GPIO_24_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_24_I_LEN)-1)<<GLB_REG_GPIO_24_I_POS))
#define GLB_REG_GPIO_25_I                                       GLB_REG_GPIO_25_I
#define GLB_REG_GPIO_25_I_POS                                   (25U)
#define GLB_REG_GPIO_25_I_LEN                                   (1U)
#define GLB_REG_GPIO_25_I_MSK                                   (((1U<<GLB_REG_GPIO_25_I_LEN)-1)<<GLB_REG_GPIO_25_I_POS)
#define GLB_REG_GPIO_25_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_25_I_LEN)-1)<<GLB_REG_GPIO_25_I_POS))
#define GLB_REG_GPIO_26_I                                       GLB_REG_GPIO_26_I
#define GLB_REG_GPIO_26_I_POS                                   (26U)
#define GLB_REG_GPIO_26_I_LEN                                   (1U)
#define GLB_REG_GPIO_26_I_MSK                                   (((1U<<GLB_REG_GPIO_26_I_LEN)-1)<<GLB_REG_GPIO_26_I_POS)
#define GLB_REG_GPIO_26_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_26_I_LEN)-1)<<GLB_REG_GPIO_26_I_POS))
#define GLB_REG_GPIO_27_I                                       GLB_REG_GPIO_27_I
#define GLB_REG_GPIO_27_I_POS                                   (27U)
#define GLB_REG_GPIO_27_I_LEN                                   (1U)
#define GLB_REG_GPIO_27_I_MSK                                   (((1U<<GLB_REG_GPIO_27_I_LEN)-1)<<GLB_REG_GPIO_27_I_POS)
#define GLB_REG_GPIO_27_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_27_I_LEN)-1)<<GLB_REG_GPIO_27_I_POS))
#define GLB_REG_GPIO_28_I                                       GLB_REG_GPIO_28_I
#define GLB_REG_GPIO_28_I_POS                                   (28U)
#define GLB_REG_GPIO_28_I_LEN                                   (1U)
#define GLB_REG_GPIO_28_I_MSK                                   (((1U<<GLB_REG_GPIO_28_I_LEN)-1)<<GLB_REG_GPIO_28_I_POS)
#define GLB_REG_GPIO_28_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_28_I_LEN)-1)<<GLB_REG_GPIO_28_I_POS))
#define GLB_REG_GPIO_29_I                                       GLB_REG_GPIO_29_I
#define GLB_REG_GPIO_29_I_POS                                   (29U)
#define GLB_REG_GPIO_29_I_LEN                                   (1U)
#define GLB_REG_GPIO_29_I_MSK                                   (((1U<<GLB_REG_GPIO_29_I_LEN)-1)<<GLB_REG_GPIO_29_I_POS)
#define GLB_REG_GPIO_29_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_29_I_LEN)-1)<<GLB_REG_GPIO_29_I_POS))
#define GLB_REG_GPIO_30_I                                       GLB_REG_GPIO_30_I
#define GLB_REG_GPIO_30_I_POS                                   (30U)
#define GLB_REG_GPIO_30_I_LEN                                   (1U)
#define GLB_REG_GPIO_30_I_MSK                                   (((1U<<GLB_REG_GPIO_30_I_LEN)-1)<<GLB_REG_GPIO_30_I_POS)
#define GLB_REG_GPIO_30_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_30_I_LEN)-1)<<GLB_REG_GPIO_30_I_POS))
#define GLB_REG_GPIO_31_I                                       GLB_REG_GPIO_31_I
#define GLB_REG_GPIO_31_I_POS                                   (31U)
#define GLB_REG_GPIO_31_I_LEN                                   (1U)
#define GLB_REG_GPIO_31_I_MSK                                   (((1U<<GLB_REG_GPIO_31_I_LEN)-1)<<GLB_REG_GPIO_31_I_POS)
#define GLB_REG_GPIO_31_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_31_I_LEN)-1)<<GLB_REG_GPIO_31_I_POS))

/* 0x184 : GPIO_CFGCTL31 */
#define GLB_GPIO_CFGCTL31_OFFSET                                (0x184)

/* 0x188 : GPIO_CFGCTL32 */
#define GLB_GPIO_CFGCTL32_OFFSET                                (0x188)
#define GLB_REG_GPIO_0_O                                        GLB_REG_GPIO_0_O
#define GLB_REG_GPIO_0_O_POS                                    (0U)
#define GLB_REG_GPIO_0_O_LEN                                    (1U)
#define GLB_REG_GPIO_0_O_MSK                                    (((1U<<GLB_REG_GPIO_0_O_LEN)-1)<<GLB_REG_GPIO_0_O_POS)
#define GLB_REG_GPIO_0_O_UMSK                                   (~(((1U<<GLB_REG_GPIO_0_O_LEN)-1)<<GLB_REG_GPIO_0_O_POS))
#define GLB_REG_GPIO_1_O                                        GLB_REG_GPIO_1_O
#define GLB_REG_GPIO_1_O_POS                                    (1U)
#define GLB_REG_GPIO_1_O_LEN                                    (1U)
#define GLB_REG_GPIO_1_O_MSK                                    (((1U<<GLB_REG_GPIO_1_O_LEN)-1)<<GLB_REG_GPIO_1_O_POS)
#define GLB_REG_GPIO_1_O_UMSK                                   (~(((1U<<GLB_REG_GPIO_1_O_LEN)-1)<<GLB_REG_GPIO_1_O_POS))
#define GLB_REG_GPIO_2_O                                        GLB_REG_GPIO_2_O
#define GLB_REG_GPIO_2_O_POS                                    (2U)
#define GLB_REG_GPIO_2_O_LEN                                    (1U)
#define GLB_REG_GPIO_2_O_MSK                                    (((1U<<GLB_REG_GPIO_2_O_LEN)-1)<<GLB_REG_GPIO_2_O_POS)
#define GLB_REG_GPIO_2_O_UMSK                                   (~(((1U<<GLB_REG_GPIO_2_O_LEN)-1)<<GLB_REG_GPIO_2_O_POS))
#define GLB_REG_GPIO_3_O                                        GLB_REG_GPIO_3_O
#define GLB_REG_GPIO_3_O_POS                                    (3U)
#define GLB_REG_GPIO_3_O_LEN                                    (1U)
#define GLB_REG_GPIO_3_O_MSK                                    (((1U<<GLB_REG_GPIO_3_O_LEN)-1)<<GLB_REG_GPIO_3_O_POS)
#define GLB_REG_GPIO_3_O_UMSK                                   (~(((1U<<GLB_REG_GPIO_3_O_LEN)-1)<<GLB_REG_GPIO_3_O_POS))
#define GLB_REG_GPIO_7_O                                        GLB_REG_GPIO_7_O
#define GLB_REG_GPIO_7_O_POS                                    (7U)
#define GLB_REG_GPIO_7_O_LEN                                    (1U)
#define GLB_REG_GPIO_7_O_MSK                                    (((1U<<GLB_REG_GPIO_7_O_LEN)-1)<<GLB_REG_GPIO_7_O_POS)
#define GLB_REG_GPIO_7_O_UMSK                                   (~(((1U<<GLB_REG_GPIO_7_O_LEN)-1)<<GLB_REG_GPIO_7_O_POS))
#define GLB_REG_GPIO_8_O                                        GLB_REG_GPIO_8_O
#define GLB_REG_GPIO_8_O_POS                                    (8U)
#define GLB_REG_GPIO_8_O_LEN                                    (1U)
#define GLB_REG_GPIO_8_O_MSK                                    (((1U<<GLB_REG_GPIO_8_O_LEN)-1)<<GLB_REG_GPIO_8_O_POS)
#define GLB_REG_GPIO_8_O_UMSK                                   (~(((1U<<GLB_REG_GPIO_8_O_LEN)-1)<<GLB_REG_GPIO_8_O_POS))
#define GLB_REG_GPIO_9_O                                        GLB_REG_GPIO_9_O
#define GLB_REG_GPIO_9_O_POS                                    (9U)
#define GLB_REG_GPIO_9_O_LEN                                    (1U)
#define GLB_REG_GPIO_9_O_MSK                                    (((1U<<GLB_REG_GPIO_9_O_LEN)-1)<<GLB_REG_GPIO_9_O_POS)
#define GLB_REG_GPIO_9_O_UMSK                                   (~(((1U<<GLB_REG_GPIO_9_O_LEN)-1)<<GLB_REG_GPIO_9_O_POS))
#define GLB_REG_GPIO_10_O                                       GLB_REG_GPIO_10_O
#define GLB_REG_GPIO_10_O_POS                                   (10U)
#define GLB_REG_GPIO_10_O_LEN                                   (1U)
#define GLB_REG_GPIO_10_O_MSK                                   (((1U<<GLB_REG_GPIO_10_O_LEN)-1)<<GLB_REG_GPIO_10_O_POS)
#define GLB_REG_GPIO_10_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_10_O_LEN)-1)<<GLB_REG_GPIO_10_O_POS))
#define GLB_REG_GPIO_11_O                                       GLB_REG_GPIO_11_O
#define GLB_REG_GPIO_11_O_POS                                   (11U)
#define GLB_REG_GPIO_11_O_LEN                                   (1U)
#define GLB_REG_GPIO_11_O_MSK                                   (((1U<<GLB_REG_GPIO_11_O_LEN)-1)<<GLB_REG_GPIO_11_O_POS)
#define GLB_REG_GPIO_11_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_11_O_LEN)-1)<<GLB_REG_GPIO_11_O_POS))
#define GLB_REG_GPIO_12_O                                       GLB_REG_GPIO_12_O
#define GLB_REG_GPIO_12_O_POS                                   (12U)
#define GLB_REG_GPIO_12_O_LEN                                   (1U)
#define GLB_REG_GPIO_12_O_MSK                                   (((1U<<GLB_REG_GPIO_12_O_LEN)-1)<<GLB_REG_GPIO_12_O_POS)
#define GLB_REG_GPIO_12_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_12_O_LEN)-1)<<GLB_REG_GPIO_12_O_POS))
#define GLB_REG_GPIO_13_O                                       GLB_REG_GPIO_13_O
#define GLB_REG_GPIO_13_O_POS                                   (13U)
#define GLB_REG_GPIO_13_O_LEN                                   (1U)
#define GLB_REG_GPIO_13_O_MSK                                   (((1U<<GLB_REG_GPIO_13_O_LEN)-1)<<GLB_REG_GPIO_13_O_POS)
#define GLB_REG_GPIO_13_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_13_O_LEN)-1)<<GLB_REG_GPIO_13_O_POS))
#define GLB_REG_GPIO_14_O                                       GLB_REG_GPIO_14_O
#define GLB_REG_GPIO_14_O_POS                                   (14U)
#define GLB_REG_GPIO_14_O_LEN                                   (1U)
#define GLB_REG_GPIO_14_O_MSK                                   (((1U<<GLB_REG_GPIO_14_O_LEN)-1)<<GLB_REG_GPIO_14_O_POS)
#define GLB_REG_GPIO_14_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_14_O_LEN)-1)<<GLB_REG_GPIO_14_O_POS))
#define GLB_REG_GPIO_15_O                                       GLB_REG_GPIO_15_O
#define GLB_REG_GPIO_15_O_POS                                   (15U)
#define GLB_REG_GPIO_15_O_LEN                                   (1U)
#define GLB_REG_GPIO_15_O_MSK                                   (((1U<<GLB_REG_GPIO_15_O_LEN)-1)<<GLB_REG_GPIO_15_O_POS)
#define GLB_REG_GPIO_15_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_15_O_LEN)-1)<<GLB_REG_GPIO_15_O_POS))
#define GLB_REG_GPIO_16_O                                       GLB_REG_GPIO_16_O
#define GLB_REG_GPIO_16_O_POS                                   (16U)
#define GLB_REG_GPIO_16_O_LEN                                   (1U)
#define GLB_REG_GPIO_16_O_MSK                                   (((1U<<GLB_REG_GPIO_16_O_LEN)-1)<<GLB_REG_GPIO_16_O_POS)
#define GLB_REG_GPIO_16_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_16_O_LEN)-1)<<GLB_REG_GPIO_16_O_POS))
#define GLB_REG_GPIO_17_O                                       GLB_REG_GPIO_17_O
#define GLB_REG_GPIO_17_O_POS                                   (17U)
#define GLB_REG_GPIO_17_O_LEN                                   (1U)
#define GLB_REG_GPIO_17_O_MSK                                   (((1U<<GLB_REG_GPIO_17_O_LEN)-1)<<GLB_REG_GPIO_17_O_POS)
#define GLB_REG_GPIO_17_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_17_O_LEN)-1)<<GLB_REG_GPIO_17_O_POS))
#define GLB_REG_GPIO_18_O                                       GLB_REG_GPIO_18_O
#define GLB_REG_GPIO_18_O_POS                                   (18U)
#define GLB_REG_GPIO_18_O_LEN                                   (1U)
#define GLB_REG_GPIO_18_O_MSK                                   (((1U<<GLB_REG_GPIO_18_O_LEN)-1)<<GLB_REG_GPIO_18_O_POS)
#define GLB_REG_GPIO_18_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_18_O_LEN)-1)<<GLB_REG_GPIO_18_O_POS))
#define GLB_REG_GPIO_19_O                                       GLB_REG_GPIO_19_O
#define GLB_REG_GPIO_19_O_POS                                   (19U)
#define GLB_REG_GPIO_19_O_LEN                                   (1U)
#define GLB_REG_GPIO_19_O_MSK                                   (((1U<<GLB_REG_GPIO_19_O_LEN)-1)<<GLB_REG_GPIO_19_O_POS)
#define GLB_REG_GPIO_19_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_19_O_LEN)-1)<<GLB_REG_GPIO_19_O_POS))
#define GLB_REG_GPIO_20_O                                       GLB_REG_GPIO_20_O
#define GLB_REG_GPIO_20_O_POS                                   (20U)
#define GLB_REG_GPIO_20_O_LEN                                   (1U)
#define GLB_REG_GPIO_20_O_MSK                                   (((1U<<GLB_REG_GPIO_20_O_LEN)-1)<<GLB_REG_GPIO_20_O_POS)
#define GLB_REG_GPIO_20_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_20_O_LEN)-1)<<GLB_REG_GPIO_20_O_POS))
#define GLB_REG_GPIO_21_O                                       GLB_REG_GPIO_21_O
#define GLB_REG_GPIO_21_O_POS                                   (21U)
#define GLB_REG_GPIO_21_O_LEN                                   (1U)
#define GLB_REG_GPIO_21_O_MSK                                   (((1U<<GLB_REG_GPIO_21_O_LEN)-1)<<GLB_REG_GPIO_21_O_POS)
#define GLB_REG_GPIO_21_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_21_O_LEN)-1)<<GLB_REG_GPIO_21_O_POS))
#define GLB_REG_GPIO_22_O                                       GLB_REG_GPIO_22_O
#define GLB_REG_GPIO_22_O_POS                                   (22U)
#define GLB_REG_GPIO_22_O_LEN                                   (1U)
#define GLB_REG_GPIO_22_O_MSK                                   (((1U<<GLB_REG_GPIO_22_O_LEN)-1)<<GLB_REG_GPIO_22_O_POS)
#define GLB_REG_GPIO_22_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_22_O_LEN)-1)<<GLB_REG_GPIO_22_O_POS))
#define GLB_REG_GPIO_23_O                                       GLB_REG_GPIO_23_O
#define GLB_REG_GPIO_23_O_POS                                   (23U)
#define GLB_REG_GPIO_23_O_LEN                                   (1U)
#define GLB_REG_GPIO_23_O_MSK                                   (((1U<<GLB_REG_GPIO_23_O_LEN)-1)<<GLB_REG_GPIO_23_O_POS)
#define GLB_REG_GPIO_23_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_23_O_LEN)-1)<<GLB_REG_GPIO_23_O_POS))
#define GLB_REG_GPIO_24_O                                       GLB_REG_GPIO_24_O
#define GLB_REG_GPIO_24_O_POS                                   (24U)
#define GLB_REG_GPIO_24_O_LEN                                   (1U)
#define GLB_REG_GPIO_24_O_MSK                                   (((1U<<GLB_REG_GPIO_24_O_LEN)-1)<<GLB_REG_GPIO_24_O_POS)
#define GLB_REG_GPIO_24_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_24_O_LEN)-1)<<GLB_REG_GPIO_24_O_POS))
#define GLB_REG_GPIO_25_O                                       GLB_REG_GPIO_25_O
#define GLB_REG_GPIO_25_O_POS                                   (25U)
#define GLB_REG_GPIO_25_O_LEN                                   (1U)
#define GLB_REG_GPIO_25_O_MSK                                   (((1U<<GLB_REG_GPIO_25_O_LEN)-1)<<GLB_REG_GPIO_25_O_POS)
#define GLB_REG_GPIO_25_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_25_O_LEN)-1)<<GLB_REG_GPIO_25_O_POS))
#define GLB_REG_GPIO_26_O                                       GLB_REG_GPIO_26_O
#define GLB_REG_GPIO_26_O_POS                                   (26U)
#define GLB_REG_GPIO_26_O_LEN                                   (1U)
#define GLB_REG_GPIO_26_O_MSK                                   (((1U<<GLB_REG_GPIO_26_O_LEN)-1)<<GLB_REG_GPIO_26_O_POS)
#define GLB_REG_GPIO_26_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_26_O_LEN)-1)<<GLB_REG_GPIO_26_O_POS))
#define GLB_REG_GPIO_27_O                                       GLB_REG_GPIO_27_O
#define GLB_REG_GPIO_27_O_POS                                   (27U)
#define GLB_REG_GPIO_27_O_LEN                                   (1U)
#define GLB_REG_GPIO_27_O_MSK                                   (((1U<<GLB_REG_GPIO_27_O_LEN)-1)<<GLB_REG_GPIO_27_O_POS)
#define GLB_REG_GPIO_27_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_27_O_LEN)-1)<<GLB_REG_GPIO_27_O_POS))
#define GLB_REG_GPIO_28_O                                       GLB_REG_GPIO_28_O
#define GLB_REG_GPIO_28_O_POS                                   (28U)
#define GLB_REG_GPIO_28_O_LEN                                   (1U)
#define GLB_REG_GPIO_28_O_MSK                                   (((1U<<GLB_REG_GPIO_28_O_LEN)-1)<<GLB_REG_GPIO_28_O_POS)
#define GLB_REG_GPIO_28_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_28_O_LEN)-1)<<GLB_REG_GPIO_28_O_POS))
#define GLB_REG_GPIO_29_O                                       GLB_REG_GPIO_29_O
#define GLB_REG_GPIO_29_O_POS                                   (29U)
#define GLB_REG_GPIO_29_O_LEN                                   (1U)
#define GLB_REG_GPIO_29_O_MSK                                   (((1U<<GLB_REG_GPIO_29_O_LEN)-1)<<GLB_REG_GPIO_29_O_POS)
#define GLB_REG_GPIO_29_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_29_O_LEN)-1)<<GLB_REG_GPIO_29_O_POS))
#define GLB_REG_GPIO_30_O                                       GLB_REG_GPIO_30_O
#define GLB_REG_GPIO_30_O_POS                                   (30U)
#define GLB_REG_GPIO_30_O_LEN                                   (1U)
#define GLB_REG_GPIO_30_O_MSK                                   (((1U<<GLB_REG_GPIO_30_O_LEN)-1)<<GLB_REG_GPIO_30_O_POS)
#define GLB_REG_GPIO_30_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_30_O_LEN)-1)<<GLB_REG_GPIO_30_O_POS))
#define GLB_REG_GPIO_31_O                                       GLB_REG_GPIO_31_O
#define GLB_REG_GPIO_31_O_POS                                   (31U)
#define GLB_REG_GPIO_31_O_LEN                                   (1U)
#define GLB_REG_GPIO_31_O_MSK                                   (((1U<<GLB_REG_GPIO_31_O_LEN)-1)<<GLB_REG_GPIO_31_O_POS)
#define GLB_REG_GPIO_31_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_31_O_LEN)-1)<<GLB_REG_GPIO_31_O_POS))

/* 0x18C : GPIO_CFGCTL33 */
#define GLB_GPIO_CFGCTL33_OFFSET                                (0x18C)

/* 0x190 : GPIO_CFGCTL34 */
#define GLB_GPIO_CFGCTL34_OFFSET                                (0x190)
#define GLB_REG_GPIO_0_OE                                       GLB_REG_GPIO_0_OE
#define GLB_REG_GPIO_0_OE_POS                                   (0U)
#define GLB_REG_GPIO_0_OE_LEN                                   (1U)
#define GLB_REG_GPIO_0_OE_MSK                                   (((1U<<GLB_REG_GPIO_0_OE_LEN)-1)<<GLB_REG_GPIO_0_OE_POS)
#define GLB_REG_GPIO_0_OE_UMSK                                  (~(((1U<<GLB_REG_GPIO_0_OE_LEN)-1)<<GLB_REG_GPIO_0_OE_POS))
#define GLB_REG_GPIO_1_OE                                       GLB_REG_GPIO_1_OE
#define GLB_REG_GPIO_1_OE_POS                                   (1U)
#define GLB_REG_GPIO_1_OE_LEN                                   (1U)
#define GLB_REG_GPIO_1_OE_MSK                                   (((1U<<GLB_REG_GPIO_1_OE_LEN)-1)<<GLB_REG_GPIO_1_OE_POS)
#define GLB_REG_GPIO_1_OE_UMSK                                  (~(((1U<<GLB_REG_GPIO_1_OE_LEN)-1)<<GLB_REG_GPIO_1_OE_POS))
#define GLB_REG_GPIO_2_OE                                       GLB_REG_GPIO_2_OE
#define GLB_REG_GPIO_2_OE_POS                                   (2U)
#define GLB_REG_GPIO_2_OE_LEN                                   (1U)
#define GLB_REG_GPIO_2_OE_MSK                                   (((1U<<GLB_REG_GPIO_2_OE_LEN)-1)<<GLB_REG_GPIO_2_OE_POS)
#define GLB_REG_GPIO_2_OE_UMSK                                  (~(((1U<<GLB_REG_GPIO_2_OE_LEN)-1)<<GLB_REG_GPIO_2_OE_POS))
#define GLB_REG_GPIO_3_OE                                       GLB_REG_GPIO_3_OE
#define GLB_REG_GPIO_3_OE_POS                                   (3U)
#define GLB_REG_GPIO_3_OE_LEN                                   (1U)
#define GLB_REG_GPIO_3_OE_MSK                                   (((1U<<GLB_REG_GPIO_3_OE_LEN)-1)<<GLB_REG_GPIO_3_OE_POS)
#define GLB_REG_GPIO_3_OE_UMSK                                  (~(((1U<<GLB_REG_GPIO_3_OE_LEN)-1)<<GLB_REG_GPIO_3_OE_POS))
#define GLB_REG_GPIO_7_OE                                       GLB_REG_GPIO_7_OE
#define GLB_REG_GPIO_7_OE_POS                                   (7U)
#define GLB_REG_GPIO_7_OE_LEN                                   (1U)
#define GLB_REG_GPIO_7_OE_MSK                                   (((1U<<GLB_REG_GPIO_7_OE_LEN)-1)<<GLB_REG_GPIO_7_OE_POS)
#define GLB_REG_GPIO_7_OE_UMSK                                  (~(((1U<<GLB_REG_GPIO_7_OE_LEN)-1)<<GLB_REG_GPIO_7_OE_POS))
#define GLB_REG_GPIO_8_OE                                       GLB_REG_GPIO_8_OE
#define GLB_REG_GPIO_8_OE_POS                                   (8U)
#define GLB_REG_GPIO_8_OE_LEN                                   (1U)
#define GLB_REG_GPIO_8_OE_MSK                                   (((1U<<GLB_REG_GPIO_8_OE_LEN)-1)<<GLB_REG_GPIO_8_OE_POS)
#define GLB_REG_GPIO_8_OE_UMSK                                  (~(((1U<<GLB_REG_GPIO_8_OE_LEN)-1)<<GLB_REG_GPIO_8_OE_POS))
#define GLB_REG_GPIO_9_OE                                       GLB_REG_GPIO_9_OE
#define GLB_REG_GPIO_9_OE_POS                                   (9U)
#define GLB_REG_GPIO_9_OE_LEN                                   (1U)
#define GLB_REG_GPIO_9_OE_MSK                                   (((1U<<GLB_REG_GPIO_9_OE_LEN)-1)<<GLB_REG_GPIO_9_OE_POS)
#define GLB_REG_GPIO_9_OE_UMSK                                  (~(((1U<<GLB_REG_GPIO_9_OE_LEN)-1)<<GLB_REG_GPIO_9_OE_POS))
#define GLB_REG_GPIO_10_OE                                      GLB_REG_GPIO_10_OE
#define GLB_REG_GPIO_10_OE_POS                                  (10U)
#define GLB_REG_GPIO_10_OE_LEN                                  (1U)
#define GLB_REG_GPIO_10_OE_MSK                                  (((1U<<GLB_REG_GPIO_10_OE_LEN)-1)<<GLB_REG_GPIO_10_OE_POS)
#define GLB_REG_GPIO_10_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_10_OE_LEN)-1)<<GLB_REG_GPIO_10_OE_POS))
#define GLB_REG_GPIO_11_OE                                      GLB_REG_GPIO_11_OE
#define GLB_REG_GPIO_11_OE_POS                                  (11U)
#define GLB_REG_GPIO_11_OE_LEN                                  (1U)
#define GLB_REG_GPIO_11_OE_MSK                                  (((1U<<GLB_REG_GPIO_11_OE_LEN)-1)<<GLB_REG_GPIO_11_OE_POS)
#define GLB_REG_GPIO_11_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_11_OE_LEN)-1)<<GLB_REG_GPIO_11_OE_POS))
#define GLB_REG_GPIO_12_OE                                      GLB_REG_GPIO_12_OE
#define GLB_REG_GPIO_12_OE_POS                                  (12U)
#define GLB_REG_GPIO_12_OE_LEN                                  (1U)
#define GLB_REG_GPIO_12_OE_MSK                                  (((1U<<GLB_REG_GPIO_12_OE_LEN)-1)<<GLB_REG_GPIO_12_OE_POS)
#define GLB_REG_GPIO_12_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_12_OE_LEN)-1)<<GLB_REG_GPIO_12_OE_POS))
#define GLB_REG_GPIO_13_OE                                      GLB_REG_GPIO_13_OE
#define GLB_REG_GPIO_13_OE_POS                                  (13U)
#define GLB_REG_GPIO_13_OE_LEN                                  (1U)
#define GLB_REG_GPIO_13_OE_MSK                                  (((1U<<GLB_REG_GPIO_13_OE_LEN)-1)<<GLB_REG_GPIO_13_OE_POS)
#define GLB_REG_GPIO_13_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_13_OE_LEN)-1)<<GLB_REG_GPIO_13_OE_POS))
#define GLB_REG_GPIO_14_OE                                      GLB_REG_GPIO_14_OE
#define GLB_REG_GPIO_14_OE_POS                                  (14U)
#define GLB_REG_GPIO_14_OE_LEN                                  (1U)
#define GLB_REG_GPIO_14_OE_MSK                                  (((1U<<GLB_REG_GPIO_14_OE_LEN)-1)<<GLB_REG_GPIO_14_OE_POS)
#define GLB_REG_GPIO_14_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_14_OE_LEN)-1)<<GLB_REG_GPIO_14_OE_POS))
#define GLB_REG_GPIO_15_OE                                      GLB_REG_GPIO_15_OE
#define GLB_REG_GPIO_15_OE_POS                                  (15U)
#define GLB_REG_GPIO_15_OE_LEN                                  (1U)
#define GLB_REG_GPIO_15_OE_MSK                                  (((1U<<GLB_REG_GPIO_15_OE_LEN)-1)<<GLB_REG_GPIO_15_OE_POS)
#define GLB_REG_GPIO_15_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_15_OE_LEN)-1)<<GLB_REG_GPIO_15_OE_POS))
#define GLB_REG_GPIO_16_OE                                      GLB_REG_GPIO_16_OE
#define GLB_REG_GPIO_16_OE_POS                                  (16U)
#define GLB_REG_GPIO_16_OE_LEN                                  (1U)
#define GLB_REG_GPIO_16_OE_MSK                                  (((1U<<GLB_REG_GPIO_16_OE_LEN)-1)<<GLB_REG_GPIO_16_OE_POS)
#define GLB_REG_GPIO_16_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_16_OE_LEN)-1)<<GLB_REG_GPIO_16_OE_POS))
#define GLB_REG_GPIO_17_OE                                      GLB_REG_GPIO_17_OE
#define GLB_REG_GPIO_17_OE_POS                                  (17U)
#define GLB_REG_GPIO_17_OE_LEN                                  (1U)
#define GLB_REG_GPIO_17_OE_MSK                                  (((1U<<GLB_REG_GPIO_17_OE_LEN)-1)<<GLB_REG_GPIO_17_OE_POS)
#define GLB_REG_GPIO_17_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_17_OE_LEN)-1)<<GLB_REG_GPIO_17_OE_POS))
#define GLB_REG_GPIO_18_OE                                      GLB_REG_GPIO_18_OE
#define GLB_REG_GPIO_18_OE_POS                                  (18U)
#define GLB_REG_GPIO_18_OE_LEN                                  (1U)
#define GLB_REG_GPIO_18_OE_MSK                                  (((1U<<GLB_REG_GPIO_18_OE_LEN)-1)<<GLB_REG_GPIO_18_OE_POS)
#define GLB_REG_GPIO_18_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_18_OE_LEN)-1)<<GLB_REG_GPIO_18_OE_POS))
#define GLB_REG_GPIO_19_OE                                      GLB_REG_GPIO_19_OE
#define GLB_REG_GPIO_19_OE_POS                                  (19U)
#define GLB_REG_GPIO_19_OE_LEN                                  (1U)
#define GLB_REG_GPIO_19_OE_MSK                                  (((1U<<GLB_REG_GPIO_19_OE_LEN)-1)<<GLB_REG_GPIO_19_OE_POS)
#define GLB_REG_GPIO_19_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_19_OE_LEN)-1)<<GLB_REG_GPIO_19_OE_POS))
#define GLB_REG_GPIO_20_OE                                      GLB_REG_GPIO_20_OE
#define GLB_REG_GPIO_20_OE_POS                                  (20U)
#define GLB_REG_GPIO_20_OE_LEN                                  (1U)
#define GLB_REG_GPIO_20_OE_MSK                                  (((1U<<GLB_REG_GPIO_20_OE_LEN)-1)<<GLB_REG_GPIO_20_OE_POS)
#define GLB_REG_GPIO_20_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_20_OE_LEN)-1)<<GLB_REG_GPIO_20_OE_POS))
#define GLB_REG_GPIO_21_OE                                      GLB_REG_GPIO_21_OE
#define GLB_REG_GPIO_21_OE_POS                                  (21U)
#define GLB_REG_GPIO_21_OE_LEN                                  (1U)
#define GLB_REG_GPIO_21_OE_MSK                                  (((1U<<GLB_REG_GPIO_21_OE_LEN)-1)<<GLB_REG_GPIO_21_OE_POS)
#define GLB_REG_GPIO_21_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_21_OE_LEN)-1)<<GLB_REG_GPIO_21_OE_POS))
#define GLB_REG_GPIO_22_OE                                      GLB_REG_GPIO_22_OE
#define GLB_REG_GPIO_22_OE_POS                                  (22U)
#define GLB_REG_GPIO_22_OE_LEN                                  (1U)
#define GLB_REG_GPIO_22_OE_MSK                                  (((1U<<GLB_REG_GPIO_22_OE_LEN)-1)<<GLB_REG_GPIO_22_OE_POS)
#define GLB_REG_GPIO_22_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_22_OE_LEN)-1)<<GLB_REG_GPIO_22_OE_POS))
#define GLB_REG_GPIO_23_OE                                      GLB_REG_GPIO_23_OE
#define GLB_REG_GPIO_23_OE_POS                                  (23U)
#define GLB_REG_GPIO_23_OE_LEN                                  (1U)
#define GLB_REG_GPIO_23_OE_MSK                                  (((1U<<GLB_REG_GPIO_23_OE_LEN)-1)<<GLB_REG_GPIO_23_OE_POS)
#define GLB_REG_GPIO_23_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_23_OE_LEN)-1)<<GLB_REG_GPIO_23_OE_POS))
#define GLB_REG_GPIO_24_OE                                      GLB_REG_GPIO_24_OE
#define GLB_REG_GPIO_24_OE_POS                                  (24U)
#define GLB_REG_GPIO_24_OE_LEN                                  (1U)
#define GLB_REG_GPIO_24_OE_MSK                                  (((1U<<GLB_REG_GPIO_24_OE_LEN)-1)<<GLB_REG_GPIO_24_OE_POS)
#define GLB_REG_GPIO_24_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_24_OE_LEN)-1)<<GLB_REG_GPIO_24_OE_POS))
#define GLB_REG_GPIO_25_OE                                      GLB_REG_GPIO_25_OE
#define GLB_REG_GPIO_25_OE_POS                                  (25U)
#define GLB_REG_GPIO_25_OE_LEN                                  (1U)
#define GLB_REG_GPIO_25_OE_MSK                                  (((1U<<GLB_REG_GPIO_25_OE_LEN)-1)<<GLB_REG_GPIO_25_OE_POS)
#define GLB_REG_GPIO_25_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_25_OE_LEN)-1)<<GLB_REG_GPIO_25_OE_POS))
#define GLB_REG_GPIO_26_OE                                      GLB_REG_GPIO_26_OE
#define GLB_REG_GPIO_26_OE_POS                                  (26U)
#define GLB_REG_GPIO_26_OE_LEN                                  (1U)
#define GLB_REG_GPIO_26_OE_MSK                                  (((1U<<GLB_REG_GPIO_26_OE_LEN)-1)<<GLB_REG_GPIO_26_OE_POS)
#define GLB_REG_GPIO_26_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_26_OE_LEN)-1)<<GLB_REG_GPIO_26_OE_POS))
#define GLB_REG_GPIO_27_OE                                      GLB_REG_GPIO_27_OE
#define GLB_REG_GPIO_27_OE_POS                                  (27U)
#define GLB_REG_GPIO_27_OE_LEN                                  (1U)
#define GLB_REG_GPIO_27_OE_MSK                                  (((1U<<GLB_REG_GPIO_27_OE_LEN)-1)<<GLB_REG_GPIO_27_OE_POS)
#define GLB_REG_GPIO_27_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_27_OE_LEN)-1)<<GLB_REG_GPIO_27_OE_POS))
#define GLB_REG_GPIO_28_OE                                      GLB_REG_GPIO_28_OE
#define GLB_REG_GPIO_28_OE_POS                                  (28U)
#define GLB_REG_GPIO_28_OE_LEN                                  (1U)
#define GLB_REG_GPIO_28_OE_MSK                                  (((1U<<GLB_REG_GPIO_28_OE_LEN)-1)<<GLB_REG_GPIO_28_OE_POS)
#define GLB_REG_GPIO_28_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_28_OE_LEN)-1)<<GLB_REG_GPIO_28_OE_POS))
#define GLB_REG_GPIO_29_OE                                      GLB_REG_GPIO_29_OE
#define GLB_REG_GPIO_29_OE_POS                                  (29U)
#define GLB_REG_GPIO_29_OE_LEN                                  (1U)
#define GLB_REG_GPIO_29_OE_MSK                                  (((1U<<GLB_REG_GPIO_29_OE_LEN)-1)<<GLB_REG_GPIO_29_OE_POS)
#define GLB_REG_GPIO_29_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_29_OE_LEN)-1)<<GLB_REG_GPIO_29_OE_POS))
#define GLB_REG_GPIO_30_OE                                      GLB_REG_GPIO_30_OE
#define GLB_REG_GPIO_30_OE_POS                                  (30U)
#define GLB_REG_GPIO_30_OE_LEN                                  (1U)
#define GLB_REG_GPIO_30_OE_MSK                                  (((1U<<GLB_REG_GPIO_30_OE_LEN)-1)<<GLB_REG_GPIO_30_OE_POS)
#define GLB_REG_GPIO_30_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_30_OE_LEN)-1)<<GLB_REG_GPIO_30_OE_POS))
#define GLB_REG_GPIO_31_OE                                      GLB_REG_GPIO_31_OE
#define GLB_REG_GPIO_31_OE_POS                                  (31U)
#define GLB_REG_GPIO_31_OE_LEN                                  (1U)
#define GLB_REG_GPIO_31_OE_MSK                                  (((1U<<GLB_REG_GPIO_31_OE_LEN)-1)<<GLB_REG_GPIO_31_OE_POS)
#define GLB_REG_GPIO_31_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_31_OE_LEN)-1)<<GLB_REG_GPIO_31_OE_POS))

/* 0x194 : GPIO_CFGCTL35 */
#define GLB_GPIO_CFGCTL35_OFFSET                                (0x194)
#define GLB_REG_GPIO_0_SET                                      GLB_REG_GPIO_0_SET
#define GLB_REG_GPIO_0_SET_POS                                  (0U)
#define GLB_REG_GPIO_0_SET_LEN                                  (1U)
#define GLB_REG_GPIO_0_SET_MSK                                  (((1U<<GLB_REG_GPIO_0_SET_LEN)-1)<<GLB_REG_GPIO_0_SET_POS)
#define GLB_REG_GPIO_0_SET_UMSK                                 (~(((1U<<GLB_REG_GPIO_0_SET_LEN)-1)<<GLB_REG_GPIO_0_SET_POS))
#define GLB_REG_GPIO_1_SET                                      GLB_REG_GPIO_1_SET
#define GLB_REG_GPIO_1_SET_POS                                  (1U)
#define GLB_REG_GPIO_1_SET_LEN                                  (1U)
#define GLB_REG_GPIO_1_SET_MSK                                  (((1U<<GLB_REG_GPIO_1_SET_LEN)-1)<<GLB_REG_GPIO_1_SET_POS)
#define GLB_REG_GPIO_1_SET_UMSK                                 (~(((1U<<GLB_REG_GPIO_1_SET_LEN)-1)<<GLB_REG_GPIO_1_SET_POS))
#define GLB_REG_GPIO_2_SET                                      GLB_REG_GPIO_2_SET
#define GLB_REG_GPIO_2_SET_POS                                  (2U)
#define GLB_REG_GPIO_2_SET_LEN                                  (1U)
#define GLB_REG_GPIO_2_SET_MSK                                  (((1U<<GLB_REG_GPIO_2_SET_LEN)-1)<<GLB_REG_GPIO_2_SET_POS)
#define GLB_REG_GPIO_2_SET_UMSK                                 (~(((1U<<GLB_REG_GPIO_2_SET_LEN)-1)<<GLB_REG_GPIO_2_SET_POS))
#define GLB_REG_GPIO_3_SET                                      GLB_REG_GPIO_3_SET
#define GLB_REG_GPIO_3_SET_POS                                  (3U)
#define GLB_REG_GPIO_3_SET_LEN                                  (1U)
#define GLB_REG_GPIO_3_SET_MSK                                  (((1U<<GLB_REG_GPIO_3_SET_LEN)-1)<<GLB_REG_GPIO_3_SET_POS)
#define GLB_REG_GPIO_3_SET_UMSK                                 (~(((1U<<GLB_REG_GPIO_3_SET_LEN)-1)<<GLB_REG_GPIO_3_SET_POS))
#define GLB_REG_GPIO_7_SET                                      GLB_REG_GPIO_7_SET
#define GLB_REG_GPIO_7_SET_POS                                  (7U)
#define GLB_REG_GPIO_7_SET_LEN                                  (1U)
#define GLB_REG_GPIO_7_SET_MSK                                  (((1U<<GLB_REG_GPIO_7_SET_LEN)-1)<<GLB_REG_GPIO_7_SET_POS)
#define GLB_REG_GPIO_7_SET_UMSK                                 (~(((1U<<GLB_REG_GPIO_7_SET_LEN)-1)<<GLB_REG_GPIO_7_SET_POS))
#define GLB_REG_GPIO_8_SET                                      GLB_REG_GPIO_8_SET
#define GLB_REG_GPIO_8_SET_POS                                  (8U)
#define GLB_REG_GPIO_8_SET_LEN                                  (1U)
#define GLB_REG_GPIO_8_SET_MSK                                  (((1U<<GLB_REG_GPIO_8_SET_LEN)-1)<<GLB_REG_GPIO_8_SET_POS)
#define GLB_REG_GPIO_8_SET_UMSK                                 (~(((1U<<GLB_REG_GPIO_8_SET_LEN)-1)<<GLB_REG_GPIO_8_SET_POS))
#define GLB_REG_GPIO_9_SET                                      GLB_REG_GPIO_9_SET
#define GLB_REG_GPIO_9_SET_POS                                  (9U)
#define GLB_REG_GPIO_9_SET_LEN                                  (1U)
#define GLB_REG_GPIO_9_SET_MSK                                  (((1U<<GLB_REG_GPIO_9_SET_LEN)-1)<<GLB_REG_GPIO_9_SET_POS)
#define GLB_REG_GPIO_9_SET_UMSK                                 (~(((1U<<GLB_REG_GPIO_9_SET_LEN)-1)<<GLB_REG_GPIO_9_SET_POS))
#define GLB_REG_GPIO_10_SET                                     GLB_REG_GPIO_10_SET
#define GLB_REG_GPIO_10_SET_POS                                 (10U)
#define GLB_REG_GPIO_10_SET_LEN                                 (1U)
#define GLB_REG_GPIO_10_SET_MSK                                 (((1U<<GLB_REG_GPIO_10_SET_LEN)-1)<<GLB_REG_GPIO_10_SET_POS)
#define GLB_REG_GPIO_10_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_10_SET_LEN)-1)<<GLB_REG_GPIO_10_SET_POS))
#define GLB_REG_GPIO_11_SET                                     GLB_REG_GPIO_11_SET
#define GLB_REG_GPIO_11_SET_POS                                 (11U)
#define GLB_REG_GPIO_11_SET_LEN                                 (1U)
#define GLB_REG_GPIO_11_SET_MSK                                 (((1U<<GLB_REG_GPIO_11_SET_LEN)-1)<<GLB_REG_GPIO_11_SET_POS)
#define GLB_REG_GPIO_11_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_11_SET_LEN)-1)<<GLB_REG_GPIO_11_SET_POS))
#define GLB_REG_GPIO_12_SET                                     GLB_REG_GPIO_12_SET
#define GLB_REG_GPIO_12_SET_POS                                 (12U)
#define GLB_REG_GPIO_12_SET_LEN                                 (1U)
#define GLB_REG_GPIO_12_SET_MSK                                 (((1U<<GLB_REG_GPIO_12_SET_LEN)-1)<<GLB_REG_GPIO_12_SET_POS)
#define GLB_REG_GPIO_12_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_12_SET_LEN)-1)<<GLB_REG_GPIO_12_SET_POS))
#define GLB_REG_GPIO_13_SET                                     GLB_REG_GPIO_13_SET
#define GLB_REG_GPIO_13_SET_POS                                 (13U)
#define GLB_REG_GPIO_13_SET_LEN                                 (1U)
#define GLB_REG_GPIO_13_SET_MSK                                 (((1U<<GLB_REG_GPIO_13_SET_LEN)-1)<<GLB_REG_GPIO_13_SET_POS)
#define GLB_REG_GPIO_13_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_13_SET_LEN)-1)<<GLB_REG_GPIO_13_SET_POS))
#define GLB_REG_GPIO_14_SET                                     GLB_REG_GPIO_14_SET
#define GLB_REG_GPIO_14_SET_POS                                 (14U)
#define GLB_REG_GPIO_14_SET_LEN                                 (1U)
#define GLB_REG_GPIO_14_SET_MSK                                 (((1U<<GLB_REG_GPIO_14_SET_LEN)-1)<<GLB_REG_GPIO_14_SET_POS)
#define GLB_REG_GPIO_14_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_14_SET_LEN)-1)<<GLB_REG_GPIO_14_SET_POS))
#define GLB_REG_GPIO_15_SET                                     GLB_REG_GPIO_15_SET
#define GLB_REG_GPIO_15_SET_POS                                 (15U)
#define GLB_REG_GPIO_15_SET_LEN                                 (1U)
#define GLB_REG_GPIO_15_SET_MSK                                 (((1U<<GLB_REG_GPIO_15_SET_LEN)-1)<<GLB_REG_GPIO_15_SET_POS)
#define GLB_REG_GPIO_15_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_15_SET_LEN)-1)<<GLB_REG_GPIO_15_SET_POS))
#define GLB_REG_GPIO_16_SET                                     GLB_REG_GPIO_16_SET
#define GLB_REG_GPIO_16_SET_POS                                 (16U)
#define GLB_REG_GPIO_16_SET_LEN                                 (1U)
#define GLB_REG_GPIO_16_SET_MSK                                 (((1U<<GLB_REG_GPIO_16_SET_LEN)-1)<<GLB_REG_GPIO_16_SET_POS)
#define GLB_REG_GPIO_16_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_16_SET_LEN)-1)<<GLB_REG_GPIO_16_SET_POS))
#define GLB_REG_GPIO_17_SET                                     GLB_REG_GPIO_17_SET
#define GLB_REG_GPIO_17_SET_POS                                 (17U)
#define GLB_REG_GPIO_17_SET_LEN                                 (1U)
#define GLB_REG_GPIO_17_SET_MSK                                 (((1U<<GLB_REG_GPIO_17_SET_LEN)-1)<<GLB_REG_GPIO_17_SET_POS)
#define GLB_REG_GPIO_17_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_17_SET_LEN)-1)<<GLB_REG_GPIO_17_SET_POS))
#define GLB_REG_GPIO_18_SET                                     GLB_REG_GPIO_18_SET
#define GLB_REG_GPIO_18_SET_POS                                 (18U)
#define GLB_REG_GPIO_18_SET_LEN                                 (1U)
#define GLB_REG_GPIO_18_SET_MSK                                 (((1U<<GLB_REG_GPIO_18_SET_LEN)-1)<<GLB_REG_GPIO_18_SET_POS)
#define GLB_REG_GPIO_18_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_18_SET_LEN)-1)<<GLB_REG_GPIO_18_SET_POS))
#define GLB_REG_GPIO_19_SET                                     GLB_REG_GPIO_19_SET
#define GLB_REG_GPIO_19_SET_POS                                 (19U)
#define GLB_REG_GPIO_19_SET_LEN                                 (1U)
#define GLB_REG_GPIO_19_SET_MSK                                 (((1U<<GLB_REG_GPIO_19_SET_LEN)-1)<<GLB_REG_GPIO_19_SET_POS)
#define GLB_REG_GPIO_19_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_19_SET_LEN)-1)<<GLB_REG_GPIO_19_SET_POS))
#define GLB_REG_GPIO_20_SET                                     GLB_REG_GPIO_20_SET
#define GLB_REG_GPIO_20_SET_POS                                 (20U)
#define GLB_REG_GPIO_20_SET_LEN                                 (1U)
#define GLB_REG_GPIO_20_SET_MSK                                 (((1U<<GLB_REG_GPIO_20_SET_LEN)-1)<<GLB_REG_GPIO_20_SET_POS)
#define GLB_REG_GPIO_20_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_20_SET_LEN)-1)<<GLB_REG_GPIO_20_SET_POS))
#define GLB_REG_GPIO_21_SET                                     GLB_REG_GPIO_21_SET
#define GLB_REG_GPIO_21_SET_POS                                 (21U)
#define GLB_REG_GPIO_21_SET_LEN                                 (1U)
#define GLB_REG_GPIO_21_SET_MSK                                 (((1U<<GLB_REG_GPIO_21_SET_LEN)-1)<<GLB_REG_GPIO_21_SET_POS)
#define GLB_REG_GPIO_21_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_21_SET_LEN)-1)<<GLB_REG_GPIO_21_SET_POS))
#define GLB_REG_GPIO_22_SET                                     GLB_REG_GPIO_22_SET
#define GLB_REG_GPIO_22_SET_POS                                 (22U)
#define GLB_REG_GPIO_22_SET_LEN                                 (1U)
#define GLB_REG_GPIO_22_SET_MSK                                 (((1U<<GLB_REG_GPIO_22_SET_LEN)-1)<<GLB_REG_GPIO_22_SET_POS)
#define GLB_REG_GPIO_22_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_22_SET_LEN)-1)<<GLB_REG_GPIO_22_SET_POS))
#define GLB_REG_GPIO_23_SET                                     GLB_REG_GPIO_23_SET
#define GLB_REG_GPIO_23_SET_POS                                 (23U)
#define GLB_REG_GPIO_23_SET_LEN                                 (1U)
#define GLB_REG_GPIO_23_SET_MSK                                 (((1U<<GLB_REG_GPIO_23_SET_LEN)-1)<<GLB_REG_GPIO_23_SET_POS)
#define GLB_REG_GPIO_23_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_23_SET_LEN)-1)<<GLB_REG_GPIO_23_SET_POS))
#define GLB_REG_GPIO_24_SET                                     GLB_REG_GPIO_24_SET
#define GLB_REG_GPIO_24_SET_POS                                 (24U)
#define GLB_REG_GPIO_24_SET_LEN                                 (1U)
#define GLB_REG_GPIO_24_SET_MSK                                 (((1U<<GLB_REG_GPIO_24_SET_LEN)-1)<<GLB_REG_GPIO_24_SET_POS)
#define GLB_REG_GPIO_24_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_24_SET_LEN)-1)<<GLB_REG_GPIO_24_SET_POS))
#define GLB_REG_GPIO_25_SET                                     GLB_REG_GPIO_25_SET
#define GLB_REG_GPIO_25_SET_POS                                 (25U)
#define GLB_REG_GPIO_25_SET_LEN                                 (1U)
#define GLB_REG_GPIO_25_SET_MSK                                 (((1U<<GLB_REG_GPIO_25_SET_LEN)-1)<<GLB_REG_GPIO_25_SET_POS)
#define GLB_REG_GPIO_25_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_25_SET_LEN)-1)<<GLB_REG_GPIO_25_SET_POS))
#define GLB_REG_GPIO_26_SET                                     GLB_REG_GPIO_26_SET
#define GLB_REG_GPIO_26_SET_POS                                 (26U)
#define GLB_REG_GPIO_26_SET_LEN                                 (1U)
#define GLB_REG_GPIO_26_SET_MSK                                 (((1U<<GLB_REG_GPIO_26_SET_LEN)-1)<<GLB_REG_GPIO_26_SET_POS)
#define GLB_REG_GPIO_26_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_26_SET_LEN)-1)<<GLB_REG_GPIO_26_SET_POS))
#define GLB_REG_GPIO_27_SET                                     GLB_REG_GPIO_27_SET
#define GLB_REG_GPIO_27_SET_POS                                 (27U)
#define GLB_REG_GPIO_27_SET_LEN                                 (1U)
#define GLB_REG_GPIO_27_SET_MSK                                 (((1U<<GLB_REG_GPIO_27_SET_LEN)-1)<<GLB_REG_GPIO_27_SET_POS)
#define GLB_REG_GPIO_27_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_27_SET_LEN)-1)<<GLB_REG_GPIO_27_SET_POS))
#define GLB_REG_GPIO_28_SET                                     GLB_REG_GPIO_28_SET
#define GLB_REG_GPIO_28_SET_POS                                 (28U)
#define GLB_REG_GPIO_28_SET_LEN                                 (1U)
#define GLB_REG_GPIO_28_SET_MSK                                 (((1U<<GLB_REG_GPIO_28_SET_LEN)-1)<<GLB_REG_GPIO_28_SET_POS)
#define GLB_REG_GPIO_28_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_28_SET_LEN)-1)<<GLB_REG_GPIO_28_SET_POS))
#define GLB_REG_GPIO_29_SET                                     GLB_REG_GPIO_29_SET
#define GLB_REG_GPIO_29_SET_POS                                 (29U)
#define GLB_REG_GPIO_29_SET_LEN                                 (1U)
#define GLB_REG_GPIO_29_SET_MSK                                 (((1U<<GLB_REG_GPIO_29_SET_LEN)-1)<<GLB_REG_GPIO_29_SET_POS)
#define GLB_REG_GPIO_29_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_29_SET_LEN)-1)<<GLB_REG_GPIO_29_SET_POS))
#define GLB_REG_GPIO_30_SET                                     GLB_REG_GPIO_30_SET
#define GLB_REG_GPIO_30_SET_POS                                 (30U)
#define GLB_REG_GPIO_30_SET_LEN                                 (1U)
#define GLB_REG_GPIO_30_SET_MSK                                 (((1U<<GLB_REG_GPIO_30_SET_LEN)-1)<<GLB_REG_GPIO_30_SET_POS)
#define GLB_REG_GPIO_30_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_30_SET_LEN)-1)<<GLB_REG_GPIO_30_SET_POS))
#define GLB_REG_GPIO_31_SET                                     GLB_REG_GPIO_31_SET
#define GLB_REG_GPIO_31_SET_POS                                 (31U)
#define GLB_REG_GPIO_31_SET_LEN                                 (1U)
#define GLB_REG_GPIO_31_SET_MSK                                 (((1U<<GLB_REG_GPIO_31_SET_LEN)-1)<<GLB_REG_GPIO_31_SET_POS)
#define GLB_REG_GPIO_31_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_31_SET_LEN)-1)<<GLB_REG_GPIO_31_SET_POS))

/* 0x198 : GPIO_CFGCTL36 */
#define GLB_GPIO_CFGCTL36_OFFSET                                (0x198)
#define GLB_REG_GPIO_0_CLR                                      GLB_REG_GPIO_0_CLR
#define GLB_REG_GPIO_0_CLR_POS                                  (0U)
#define GLB_REG_GPIO_0_CLR_LEN                                  (1U)
#define GLB_REG_GPIO_0_CLR_MSK                                  (((1U<<GLB_REG_GPIO_0_CLR_LEN)-1)<<GLB_REG_GPIO_0_CLR_POS)
#define GLB_REG_GPIO_0_CLR_UMSK                                 (~(((1U<<GLB_REG_GPIO_0_CLR_LEN)-1)<<GLB_REG_GPIO_0_CLR_POS))
#define GLB_REG_GPIO_1_CLR                                      GLB_REG_GPIO_1_CLR
#define GLB_REG_GPIO_1_CLR_POS                                  (1U)
#define GLB_REG_GPIO_1_CLR_LEN                                  (1U)
#define GLB_REG_GPIO_1_CLR_MSK                                  (((1U<<GLB_REG_GPIO_1_CLR_LEN)-1)<<GLB_REG_GPIO_1_CLR_POS)
#define GLB_REG_GPIO_1_CLR_UMSK                                 (~(((1U<<GLB_REG_GPIO_1_CLR_LEN)-1)<<GLB_REG_GPIO_1_CLR_POS))
#define GLB_REG_GPIO_2_CLR                                      GLB_REG_GPIO_2_CLR
#define GLB_REG_GPIO_2_CLR_POS                                  (2U)
#define GLB_REG_GPIO_2_CLR_LEN                                  (1U)
#define GLB_REG_GPIO_2_CLR_MSK                                  (((1U<<GLB_REG_GPIO_2_CLR_LEN)-1)<<GLB_REG_GPIO_2_CLR_POS)
#define GLB_REG_GPIO_2_CLR_UMSK                                 (~(((1U<<GLB_REG_GPIO_2_CLR_LEN)-1)<<GLB_REG_GPIO_2_CLR_POS))
#define GLB_REG_GPIO_3_CLR                                      GLB_REG_GPIO_3_CLR
#define GLB_REG_GPIO_3_CLR_POS                                  (3U)
#define GLB_REG_GPIO_3_CLR_LEN                                  (1U)
#define GLB_REG_GPIO_3_CLR_MSK                                  (((1U<<GLB_REG_GPIO_3_CLR_LEN)-1)<<GLB_REG_GPIO_3_CLR_POS)
#define GLB_REG_GPIO_3_CLR_UMSK                                 (~(((1U<<GLB_REG_GPIO_3_CLR_LEN)-1)<<GLB_REG_GPIO_3_CLR_POS))
#define GLB_REG_GPIO_7_CLR                                      GLB_REG_GPIO_7_CLR
#define GLB_REG_GPIO_7_CLR_POS                                  (7U)
#define GLB_REG_GPIO_7_CLR_LEN                                  (1U)
#define GLB_REG_GPIO_7_CLR_MSK                                  (((1U<<GLB_REG_GPIO_7_CLR_LEN)-1)<<GLB_REG_GPIO_7_CLR_POS)
#define GLB_REG_GPIO_7_CLR_UMSK                                 (~(((1U<<GLB_REG_GPIO_7_CLR_LEN)-1)<<GLB_REG_GPIO_7_CLR_POS))
#define GLB_REG_GPIO_8_CLR                                      GLB_REG_GPIO_8_CLR
#define GLB_REG_GPIO_8_CLR_POS                                  (8U)
#define GLB_REG_GPIO_8_CLR_LEN                                  (1U)
#define GLB_REG_GPIO_8_CLR_MSK                                  (((1U<<GLB_REG_GPIO_8_CLR_LEN)-1)<<GLB_REG_GPIO_8_CLR_POS)
#define GLB_REG_GPIO_8_CLR_UMSK                                 (~(((1U<<GLB_REG_GPIO_8_CLR_LEN)-1)<<GLB_REG_GPIO_8_CLR_POS))
#define GLB_REG_GPIO_9_CLR                                      GLB_REG_GPIO_9_CLR
#define GLB_REG_GPIO_9_CLR_POS                                  (9U)
#define GLB_REG_GPIO_9_CLR_LEN                                  (1U)
#define GLB_REG_GPIO_9_CLR_MSK                                  (((1U<<GLB_REG_GPIO_9_CLR_LEN)-1)<<GLB_REG_GPIO_9_CLR_POS)
#define GLB_REG_GPIO_9_CLR_UMSK                                 (~(((1U<<GLB_REG_GPIO_9_CLR_LEN)-1)<<GLB_REG_GPIO_9_CLR_POS))
#define GLB_REG_GPIO_10_CLR                                     GLB_REG_GPIO_10_CLR
#define GLB_REG_GPIO_10_CLR_POS                                 (10U)
#define GLB_REG_GPIO_10_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_10_CLR_MSK                                 (((1U<<GLB_REG_GPIO_10_CLR_LEN)-1)<<GLB_REG_GPIO_10_CLR_POS)
#define GLB_REG_GPIO_10_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_10_CLR_LEN)-1)<<GLB_REG_GPIO_10_CLR_POS))
#define GLB_REG_GPIO_11_CLR                                     GLB_REG_GPIO_11_CLR
#define GLB_REG_GPIO_11_CLR_POS                                 (11U)
#define GLB_REG_GPIO_11_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_11_CLR_MSK                                 (((1U<<GLB_REG_GPIO_11_CLR_LEN)-1)<<GLB_REG_GPIO_11_CLR_POS)
#define GLB_REG_GPIO_11_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_11_CLR_LEN)-1)<<GLB_REG_GPIO_11_CLR_POS))
#define GLB_REG_GPIO_12_CLR                                     GLB_REG_GPIO_12_CLR
#define GLB_REG_GPIO_12_CLR_POS                                 (12U)
#define GLB_REG_GPIO_12_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_12_CLR_MSK                                 (((1U<<GLB_REG_GPIO_12_CLR_LEN)-1)<<GLB_REG_GPIO_12_CLR_POS)
#define GLB_REG_GPIO_12_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_12_CLR_LEN)-1)<<GLB_REG_GPIO_12_CLR_POS))
#define GLB_REG_GPIO_13_CLR                                     GLB_REG_GPIO_13_CLR
#define GLB_REG_GPIO_13_CLR_POS                                 (13U)
#define GLB_REG_GPIO_13_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_13_CLR_MSK                                 (((1U<<GLB_REG_GPIO_13_CLR_LEN)-1)<<GLB_REG_GPIO_13_CLR_POS)
#define GLB_REG_GPIO_13_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_13_CLR_LEN)-1)<<GLB_REG_GPIO_13_CLR_POS))
#define GLB_REG_GPIO_14_CLR                                     GLB_REG_GPIO_14_CLR
#define GLB_REG_GPIO_14_CLR_POS                                 (14U)
#define GLB_REG_GPIO_14_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_14_CLR_MSK                                 (((1U<<GLB_REG_GPIO_14_CLR_LEN)-1)<<GLB_REG_GPIO_14_CLR_POS)
#define GLB_REG_GPIO_14_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_14_CLR_LEN)-1)<<GLB_REG_GPIO_14_CLR_POS))
#define GLB_REG_GPIO_15_CLR                                     GLB_REG_GPIO_15_CLR
#define GLB_REG_GPIO_15_CLR_POS                                 (15U)
#define GLB_REG_GPIO_15_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_15_CLR_MSK                                 (((1U<<GLB_REG_GPIO_15_CLR_LEN)-1)<<GLB_REG_GPIO_15_CLR_POS)
#define GLB_REG_GPIO_15_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_15_CLR_LEN)-1)<<GLB_REG_GPIO_15_CLR_POS))
#define GLB_REG_GPIO_16_CLR                                     GLB_REG_GPIO_16_CLR
#define GLB_REG_GPIO_16_CLR_POS                                 (16U)
#define GLB_REG_GPIO_16_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_16_CLR_MSK                                 (((1U<<GLB_REG_GPIO_16_CLR_LEN)-1)<<GLB_REG_GPIO_16_CLR_POS)
#define GLB_REG_GPIO_16_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_16_CLR_LEN)-1)<<GLB_REG_GPIO_16_CLR_POS))
#define GLB_REG_GPIO_17_CLR                                     GLB_REG_GPIO_17_CLR
#define GLB_REG_GPIO_17_CLR_POS                                 (17U)
#define GLB_REG_GPIO_17_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_17_CLR_MSK                                 (((1U<<GLB_REG_GPIO_17_CLR_LEN)-1)<<GLB_REG_GPIO_17_CLR_POS)
#define GLB_REG_GPIO_17_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_17_CLR_LEN)-1)<<GLB_REG_GPIO_17_CLR_POS))
#define GLB_REG_GPIO_18_CLR                                     GLB_REG_GPIO_18_CLR
#define GLB_REG_GPIO_18_CLR_POS                                 (18U)
#define GLB_REG_GPIO_18_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_18_CLR_MSK                                 (((1U<<GLB_REG_GPIO_18_CLR_LEN)-1)<<GLB_REG_GPIO_18_CLR_POS)
#define GLB_REG_GPIO_18_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_18_CLR_LEN)-1)<<GLB_REG_GPIO_18_CLR_POS))
#define GLB_REG_GPIO_19_CLR                                     GLB_REG_GPIO_19_CLR
#define GLB_REG_GPIO_19_CLR_POS                                 (19U)
#define GLB_REG_GPIO_19_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_19_CLR_MSK                                 (((1U<<GLB_REG_GPIO_19_CLR_LEN)-1)<<GLB_REG_GPIO_19_CLR_POS)
#define GLB_REG_GPIO_19_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_19_CLR_LEN)-1)<<GLB_REG_GPIO_19_CLR_POS))
#define GLB_REG_GPIO_20_CLR                                     GLB_REG_GPIO_20_CLR
#define GLB_REG_GPIO_20_CLR_POS                                 (20U)
#define GLB_REG_GPIO_20_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_20_CLR_MSK                                 (((1U<<GLB_REG_GPIO_20_CLR_LEN)-1)<<GLB_REG_GPIO_20_CLR_POS)
#define GLB_REG_GPIO_20_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_20_CLR_LEN)-1)<<GLB_REG_GPIO_20_CLR_POS))
#define GLB_REG_GPIO_21_CLR                                     GLB_REG_GPIO_21_CLR
#define GLB_REG_GPIO_21_CLR_POS                                 (21U)
#define GLB_REG_GPIO_21_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_21_CLR_MSK                                 (((1U<<GLB_REG_GPIO_21_CLR_LEN)-1)<<GLB_REG_GPIO_21_CLR_POS)
#define GLB_REG_GPIO_21_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_21_CLR_LEN)-1)<<GLB_REG_GPIO_21_CLR_POS))
#define GLB_REG_GPIO_22_CLR                                     GLB_REG_GPIO_22_CLR
#define GLB_REG_GPIO_22_CLR_POS                                 (22U)
#define GLB_REG_GPIO_22_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_22_CLR_MSK                                 (((1U<<GLB_REG_GPIO_22_CLR_LEN)-1)<<GLB_REG_GPIO_22_CLR_POS)
#define GLB_REG_GPIO_22_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_22_CLR_LEN)-1)<<GLB_REG_GPIO_22_CLR_POS))
#define GLB_REG_GPIO_23_CLR                                     GLB_REG_GPIO_23_CLR
#define GLB_REG_GPIO_23_CLR_POS                                 (23U)
#define GLB_REG_GPIO_23_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_23_CLR_MSK                                 (((1U<<GLB_REG_GPIO_23_CLR_LEN)-1)<<GLB_REG_GPIO_23_CLR_POS)
#define GLB_REG_GPIO_23_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_23_CLR_LEN)-1)<<GLB_REG_GPIO_23_CLR_POS))
#define GLB_REG_GPIO_24_CLR                                     GLB_REG_GPIO_24_CLR
#define GLB_REG_GPIO_24_CLR_POS                                 (24U)
#define GLB_REG_GPIO_24_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_24_CLR_MSK                                 (((1U<<GLB_REG_GPIO_24_CLR_LEN)-1)<<GLB_REG_GPIO_24_CLR_POS)
#define GLB_REG_GPIO_24_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_24_CLR_LEN)-1)<<GLB_REG_GPIO_24_CLR_POS))
#define GLB_REG_GPIO_25_CLR                                     GLB_REG_GPIO_25_CLR
#define GLB_REG_GPIO_25_CLR_POS                                 (25U)
#define GLB_REG_GPIO_25_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_25_CLR_MSK                                 (((1U<<GLB_REG_GPIO_25_CLR_LEN)-1)<<GLB_REG_GPIO_25_CLR_POS)
#define GLB_REG_GPIO_25_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_25_CLR_LEN)-1)<<GLB_REG_GPIO_25_CLR_POS))
#define GLB_REG_GPIO_26_CLR                                     GLB_REG_GPIO_26_CLR
#define GLB_REG_GPIO_26_CLR_POS                                 (26U)
#define GLB_REG_GPIO_26_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_26_CLR_MSK                                 (((1U<<GLB_REG_GPIO_26_CLR_LEN)-1)<<GLB_REG_GPIO_26_CLR_POS)
#define GLB_REG_GPIO_26_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_26_CLR_LEN)-1)<<GLB_REG_GPIO_26_CLR_POS))
#define GLB_REG_GPIO_27_CLR                                     GLB_REG_GPIO_27_CLR
#define GLB_REG_GPIO_27_CLR_POS                                 (27U)
#define GLB_REG_GPIO_27_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_27_CLR_MSK                                 (((1U<<GLB_REG_GPIO_27_CLR_LEN)-1)<<GLB_REG_GPIO_27_CLR_POS)
#define GLB_REG_GPIO_27_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_27_CLR_LEN)-1)<<GLB_REG_GPIO_27_CLR_POS))
#define GLB_REG_GPIO_28_CLR                                     GLB_REG_GPIO_28_CLR
#define GLB_REG_GPIO_28_CLR_POS                                 (28U)
#define GLB_REG_GPIO_28_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_28_CLR_MSK                                 (((1U<<GLB_REG_GPIO_28_CLR_LEN)-1)<<GLB_REG_GPIO_28_CLR_POS)
#define GLB_REG_GPIO_28_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_28_CLR_LEN)-1)<<GLB_REG_GPIO_28_CLR_POS))
#define GLB_REG_GPIO_29_CLR                                     GLB_REG_GPIO_29_CLR
#define GLB_REG_GPIO_29_CLR_POS                                 (29U)
#define GLB_REG_GPIO_29_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_29_CLR_MSK                                 (((1U<<GLB_REG_GPIO_29_CLR_LEN)-1)<<GLB_REG_GPIO_29_CLR_POS)
#define GLB_REG_GPIO_29_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_29_CLR_LEN)-1)<<GLB_REG_GPIO_29_CLR_POS))
#define GLB_REG_GPIO_30_CLR                                     GLB_REG_GPIO_30_CLR
#define GLB_REG_GPIO_30_CLR_POS                                 (30U)
#define GLB_REG_GPIO_30_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_30_CLR_MSK                                 (((1U<<GLB_REG_GPIO_30_CLR_LEN)-1)<<GLB_REG_GPIO_30_CLR_POS)
#define GLB_REG_GPIO_30_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_30_CLR_LEN)-1)<<GLB_REG_GPIO_30_CLR_POS))
#define GLB_REG_GPIO_31_CLR                                     GLB_REG_GPIO_31_CLR
#define GLB_REG_GPIO_31_CLR_POS                                 (31U)
#define GLB_REG_GPIO_31_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_31_CLR_MSK                                 (((1U<<GLB_REG_GPIO_31_CLR_LEN)-1)<<GLB_REG_GPIO_31_CLR_POS)
#define GLB_REG_GPIO_31_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_31_CLR_LEN)-1)<<GLB_REG_GPIO_31_CLR_POS))

/* 0x1A0 : GPIO_INT_MASK1 */
#define GLB_GPIO_INT_MASK1_OFFSET                               (0x1A0)
#define GLB_REG_GPIO0_INT_MASK                                  GLB_REG_GPIO0_INT_MASK
#define GLB_REG_GPIO0_INT_MASK_POS                              (0U)
#define GLB_REG_GPIO0_INT_MASK_LEN                              (1U)
#define GLB_REG_GPIO0_INT_MASK_MSK                              (((1U<<GLB_REG_GPIO0_INT_MASK_LEN)-1)<<GLB_REG_GPIO0_INT_MASK_POS)
#define GLB_REG_GPIO0_INT_MASK_UMSK                             (~(((1U<<GLB_REG_GPIO0_INT_MASK_LEN)-1)<<GLB_REG_GPIO0_INT_MASK_POS))
#define GLB_REG_GPIO1_INT_MASK                                  GLB_REG_GPIO1_INT_MASK
#define GLB_REG_GPIO1_INT_MASK_POS                              (1U)
#define GLB_REG_GPIO1_INT_MASK_LEN                              (1U)
#define GLB_REG_GPIO1_INT_MASK_MSK                              (((1U<<GLB_REG_GPIO1_INT_MASK_LEN)-1)<<GLB_REG_GPIO1_INT_MASK_POS)
#define GLB_REG_GPIO1_INT_MASK_UMSK                             (~(((1U<<GLB_REG_GPIO1_INT_MASK_LEN)-1)<<GLB_REG_GPIO1_INT_MASK_POS))
#define GLB_REG_GPIO2_INT_MASK                                  GLB_REG_GPIO2_INT_MASK
#define GLB_REG_GPIO2_INT_MASK_POS                              (2U)
#define GLB_REG_GPIO2_INT_MASK_LEN                              (1U)
#define GLB_REG_GPIO2_INT_MASK_MSK                              (((1U<<GLB_REG_GPIO2_INT_MASK_LEN)-1)<<GLB_REG_GPIO2_INT_MASK_POS)
#define GLB_REG_GPIO2_INT_MASK_UMSK                             (~(((1U<<GLB_REG_GPIO2_INT_MASK_LEN)-1)<<GLB_REG_GPIO2_INT_MASK_POS))
#define GLB_REG_GPIO3_INT_MASK                                  GLB_REG_GPIO3_INT_MASK
#define GLB_REG_GPIO3_INT_MASK_POS                              (3U)
#define GLB_REG_GPIO3_INT_MASK_LEN                              (1U)
#define GLB_REG_GPIO3_INT_MASK_MSK                              (((1U<<GLB_REG_GPIO3_INT_MASK_LEN)-1)<<GLB_REG_GPIO3_INT_MASK_POS)
#define GLB_REG_GPIO3_INT_MASK_UMSK                             (~(((1U<<GLB_REG_GPIO3_INT_MASK_LEN)-1)<<GLB_REG_GPIO3_INT_MASK_POS))
#define GLB_REG_GPIO7_INT_MASK                                  GLB_REG_GPIO7_INT_MASK
#define GLB_REG_GPIO7_INT_MASK_POS                              (7U)
#define GLB_REG_GPIO7_INT_MASK_LEN                              (1U)
#define GLB_REG_GPIO7_INT_MASK_MSK                              (((1U<<GLB_REG_GPIO7_INT_MASK_LEN)-1)<<GLB_REG_GPIO7_INT_MASK_POS)
#define GLB_REG_GPIO7_INT_MASK_UMSK                             (~(((1U<<GLB_REG_GPIO7_INT_MASK_LEN)-1)<<GLB_REG_GPIO7_INT_MASK_POS))
#define GLB_REG_GPIO8_INT_MASK                                  GLB_REG_GPIO8_INT_MASK
#define GLB_REG_GPIO8_INT_MASK_POS                              (8U)
#define GLB_REG_GPIO8_INT_MASK_LEN                              (1U)
#define GLB_REG_GPIO8_INT_MASK_MSK                              (((1U<<GLB_REG_GPIO8_INT_MASK_LEN)-1)<<GLB_REG_GPIO8_INT_MASK_POS)
#define GLB_REG_GPIO8_INT_MASK_UMSK                             (~(((1U<<GLB_REG_GPIO8_INT_MASK_LEN)-1)<<GLB_REG_GPIO8_INT_MASK_POS))
#define GLB_REG_GPIO9_INT_MASK                                  GLB_REG_GPIO9_INT_MASK
#define GLB_REG_GPIO9_INT_MASK_POS                              (9U)
#define GLB_REG_GPIO9_INT_MASK_LEN                              (1U)
#define GLB_REG_GPIO9_INT_MASK_MSK                              (((1U<<GLB_REG_GPIO9_INT_MASK_LEN)-1)<<GLB_REG_GPIO9_INT_MASK_POS)
#define GLB_REG_GPIO9_INT_MASK_UMSK                             (~(((1U<<GLB_REG_GPIO9_INT_MASK_LEN)-1)<<GLB_REG_GPIO9_INT_MASK_POS))
#define GLB_REG_GPIO10_INT_MASK                                 GLB_REG_GPIO10_INT_MASK
#define GLB_REG_GPIO10_INT_MASK_POS                             (10U)
#define GLB_REG_GPIO10_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO10_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO10_INT_MASK_LEN)-1)<<GLB_REG_GPIO10_INT_MASK_POS)
#define GLB_REG_GPIO10_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO10_INT_MASK_LEN)-1)<<GLB_REG_GPIO10_INT_MASK_POS))
#define GLB_REG_GPIO11_INT_MASK                                 GLB_REG_GPIO11_INT_MASK
#define GLB_REG_GPIO11_INT_MASK_POS                             (11U)
#define GLB_REG_GPIO11_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO11_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO11_INT_MASK_LEN)-1)<<GLB_REG_GPIO11_INT_MASK_POS)
#define GLB_REG_GPIO11_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO11_INT_MASK_LEN)-1)<<GLB_REG_GPIO11_INT_MASK_POS))
#define GLB_REG_GPIO12_INT_MASK                                 GLB_REG_GPIO12_INT_MASK
#define GLB_REG_GPIO12_INT_MASK_POS                             (12U)
#define GLB_REG_GPIO12_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO12_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO12_INT_MASK_LEN)-1)<<GLB_REG_GPIO12_INT_MASK_POS)
#define GLB_REG_GPIO12_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO12_INT_MASK_LEN)-1)<<GLB_REG_GPIO12_INT_MASK_POS))
#define GLB_REG_GPIO13_INT_MASK                                 GLB_REG_GPIO13_INT_MASK
#define GLB_REG_GPIO13_INT_MASK_POS                             (13U)
#define GLB_REG_GPIO13_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO13_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO13_INT_MASK_LEN)-1)<<GLB_REG_GPIO13_INT_MASK_POS)
#define GLB_REG_GPIO13_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO13_INT_MASK_LEN)-1)<<GLB_REG_GPIO13_INT_MASK_POS))
#define GLB_REG_GPIO14_INT_MASK                                 GLB_REG_GPIO14_INT_MASK
#define GLB_REG_GPIO14_INT_MASK_POS                             (14U)
#define GLB_REG_GPIO14_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO14_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO14_INT_MASK_LEN)-1)<<GLB_REG_GPIO14_INT_MASK_POS)
#define GLB_REG_GPIO14_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO14_INT_MASK_LEN)-1)<<GLB_REG_GPIO14_INT_MASK_POS))
#define GLB_REG_GPIO15_INT_MASK                                 GLB_REG_GPIO15_INT_MASK
#define GLB_REG_GPIO15_INT_MASK_POS                             (15U)
#define GLB_REG_GPIO15_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO15_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO15_INT_MASK_LEN)-1)<<GLB_REG_GPIO15_INT_MASK_POS)
#define GLB_REG_GPIO15_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO15_INT_MASK_LEN)-1)<<GLB_REG_GPIO15_INT_MASK_POS))
#define GLB_REG_GPIO16_INT_MASK                                 GLB_REG_GPIO16_INT_MASK
#define GLB_REG_GPIO16_INT_MASK_POS                             (16U)
#define GLB_REG_GPIO16_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO16_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO16_INT_MASK_LEN)-1)<<GLB_REG_GPIO16_INT_MASK_POS)
#define GLB_REG_GPIO16_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO16_INT_MASK_LEN)-1)<<GLB_REG_GPIO16_INT_MASK_POS))
#define GLB_REG_GPIO17_INT_MASK                                 GLB_REG_GPIO17_INT_MASK
#define GLB_REG_GPIO17_INT_MASK_POS                             (17U)
#define GLB_REG_GPIO17_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO17_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO17_INT_MASK_LEN)-1)<<GLB_REG_GPIO17_INT_MASK_POS)
#define GLB_REG_GPIO17_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO17_INT_MASK_LEN)-1)<<GLB_REG_GPIO17_INT_MASK_POS))
#define GLB_REG_GPIO18_INT_MASK                                 GLB_REG_GPIO18_INT_MASK
#define GLB_REG_GPIO18_INT_MASK_POS                             (18U)
#define GLB_REG_GPIO18_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO18_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO18_INT_MASK_LEN)-1)<<GLB_REG_GPIO18_INT_MASK_POS)
#define GLB_REG_GPIO18_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO18_INT_MASK_LEN)-1)<<GLB_REG_GPIO18_INT_MASK_POS))
#define GLB_REG_GPIO19_INT_MASK                                 GLB_REG_GPIO19_INT_MASK
#define GLB_REG_GPIO19_INT_MASK_POS                             (19U)
#define GLB_REG_GPIO19_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO19_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO19_INT_MASK_LEN)-1)<<GLB_REG_GPIO19_INT_MASK_POS)
#define GLB_REG_GPIO19_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO19_INT_MASK_LEN)-1)<<GLB_REG_GPIO19_INT_MASK_POS))
#define GLB_REG_GPIO20_INT_MASK                                 GLB_REG_GPIO20_INT_MASK
#define GLB_REG_GPIO20_INT_MASK_POS                             (20U)
#define GLB_REG_GPIO20_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO20_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO20_INT_MASK_LEN)-1)<<GLB_REG_GPIO20_INT_MASK_POS)
#define GLB_REG_GPIO20_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO20_INT_MASK_LEN)-1)<<GLB_REG_GPIO20_INT_MASK_POS))
#define GLB_REG_GPIO21_INT_MASK                                 GLB_REG_GPIO21_INT_MASK
#define GLB_REG_GPIO21_INT_MASK_POS                             (21U)
#define GLB_REG_GPIO21_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO21_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO21_INT_MASK_LEN)-1)<<GLB_REG_GPIO21_INT_MASK_POS)
#define GLB_REG_GPIO21_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO21_INT_MASK_LEN)-1)<<GLB_REG_GPIO21_INT_MASK_POS))
#define GLB_REG_GPIO22_INT_MASK                                 GLB_REG_GPIO22_INT_MASK
#define GLB_REG_GPIO22_INT_MASK_POS                             (22U)
#define GLB_REG_GPIO22_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO22_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO22_INT_MASK_LEN)-1)<<GLB_REG_GPIO22_INT_MASK_POS)
#define GLB_REG_GPIO22_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO22_INT_MASK_LEN)-1)<<GLB_REG_GPIO22_INT_MASK_POS))
#define GLB_REG_GPIO23_INT_MASK                                 GLB_REG_GPIO23_INT_MASK
#define GLB_REG_GPIO23_INT_MASK_POS                             (23U)
#define GLB_REG_GPIO23_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO23_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO23_INT_MASK_LEN)-1)<<GLB_REG_GPIO23_INT_MASK_POS)
#define GLB_REG_GPIO23_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO23_INT_MASK_LEN)-1)<<GLB_REG_GPIO23_INT_MASK_POS))
#define GLB_REG_GPIO24_INT_MASK                                 GLB_REG_GPIO24_INT_MASK
#define GLB_REG_GPIO24_INT_MASK_POS                             (24U)
#define GLB_REG_GPIO24_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO24_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO24_INT_MASK_LEN)-1)<<GLB_REG_GPIO24_INT_MASK_POS)
#define GLB_REG_GPIO24_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO24_INT_MASK_LEN)-1)<<GLB_REG_GPIO24_INT_MASK_POS))
#define GLB_REG_GPIO25_INT_MASK                                 GLB_REG_GPIO25_INT_MASK
#define GLB_REG_GPIO25_INT_MASK_POS                             (25U)
#define GLB_REG_GPIO25_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO25_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO25_INT_MASK_LEN)-1)<<GLB_REG_GPIO25_INT_MASK_POS)
#define GLB_REG_GPIO25_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO25_INT_MASK_LEN)-1)<<GLB_REG_GPIO25_INT_MASK_POS))
#define GLB_REG_GPIO26_INT_MASK                                 GLB_REG_GPIO26_INT_MASK
#define GLB_REG_GPIO26_INT_MASK_POS                             (26U)
#define GLB_REG_GPIO26_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO26_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO26_INT_MASK_LEN)-1)<<GLB_REG_GPIO26_INT_MASK_POS)
#define GLB_REG_GPIO26_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO26_INT_MASK_LEN)-1)<<GLB_REG_GPIO26_INT_MASK_POS))
#define GLB_REG_GPIO27_INT_MASK                                 GLB_REG_GPIO27_INT_MASK
#define GLB_REG_GPIO27_INT_MASK_POS                             (27U)
#define GLB_REG_GPIO27_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO27_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO27_INT_MASK_LEN)-1)<<GLB_REG_GPIO27_INT_MASK_POS)
#define GLB_REG_GPIO27_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO27_INT_MASK_LEN)-1)<<GLB_REG_GPIO27_INT_MASK_POS))
#define GLB_REG_GPIO28_INT_MASK                                 GLB_REG_GPIO28_INT_MASK
#define GLB_REG_GPIO28_INT_MASK_POS                             (28U)
#define GLB_REG_GPIO28_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO28_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO28_INT_MASK_LEN)-1)<<GLB_REG_GPIO28_INT_MASK_POS)
#define GLB_REG_GPIO28_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO28_INT_MASK_LEN)-1)<<GLB_REG_GPIO28_INT_MASK_POS))
#define GLB_REG_GPIO29_INT_MASK                                 GLB_REG_GPIO29_INT_MASK
#define GLB_REG_GPIO29_INT_MASK_POS                             (29U)
#define GLB_REG_GPIO29_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO29_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO29_INT_MASK_LEN)-1)<<GLB_REG_GPIO29_INT_MASK_POS)
#define GLB_REG_GPIO29_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO29_INT_MASK_LEN)-1)<<GLB_REG_GPIO29_INT_MASK_POS))
#define GLB_REG_GPIO30_INT_MASK                                 GLB_REG_GPIO30_INT_MASK
#define GLB_REG_GPIO30_INT_MASK_POS                             (30U)
#define GLB_REG_GPIO30_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO30_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO30_INT_MASK_LEN)-1)<<GLB_REG_GPIO30_INT_MASK_POS)
#define GLB_REG_GPIO30_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO30_INT_MASK_LEN)-1)<<GLB_REG_GPIO30_INT_MASK_POS))
#define GLB_REG_GPIO31_INT_MASK                                 GLB_REG_GPIO31_INT_MASK
#define GLB_REG_GPIO31_INT_MASK_POS                             (31U)
#define GLB_REG_GPIO31_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO31_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO31_INT_MASK_LEN)-1)<<GLB_REG_GPIO31_INT_MASK_POS)
#define GLB_REG_GPIO31_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO31_INT_MASK_LEN)-1)<<GLB_REG_GPIO31_INT_MASK_POS))

/* 0x1A8 : GPIO_INT_STAT1 */
#define GLB_GPIO_INT_STAT1_OFFSET                               (0x1A8)
#define GLB_GPIO_INT_STAT1                                      GLB_GPIO_INT_STAT1
#define GLB_GPIO_INT_STAT1_POS                                  (0U)
#define GLB_GPIO_INT_STAT1_LEN                                  (32U)
#define GLB_GPIO_INT_STAT1_MSK                                  (((1U<<GLB_GPIO_INT_STAT1_LEN)-1)<<GLB_GPIO_INT_STAT1_POS)
#define GLB_GPIO_INT_STAT1_UMSK                                 (~(((1U<<GLB_GPIO_INT_STAT1_LEN)-1)<<GLB_GPIO_INT_STAT1_POS))

/* 0x1B0 : GPIO_INT_CLR1 */
#define GLB_GPIO_INT_CLR1_OFFSET                                (0x1B0)
#define GLB_REG_GPIO0_INT_CLR                                   GLB_REG_GPIO0_INT_CLR
#define GLB_REG_GPIO0_INT_CLR_POS                               (0U)
#define GLB_REG_GPIO0_INT_CLR_LEN                               (1U)
#define GLB_REG_GPIO0_INT_CLR_MSK                               (((1U<<GLB_REG_GPIO0_INT_CLR_LEN)-1)<<GLB_REG_GPIO0_INT_CLR_POS)
#define GLB_REG_GPIO0_INT_CLR_UMSK                              (~(((1U<<GLB_REG_GPIO0_INT_CLR_LEN)-1)<<GLB_REG_GPIO0_INT_CLR_POS))
#define GLB_REG_GPIO1_INT_CLR                                   GLB_REG_GPIO1_INT_CLR
#define GLB_REG_GPIO1_INT_CLR_POS                               (1U)
#define GLB_REG_GPIO1_INT_CLR_LEN                               (1U)
#define GLB_REG_GPIO1_INT_CLR_MSK                               (((1U<<GLB_REG_GPIO1_INT_CLR_LEN)-1)<<GLB_REG_GPIO1_INT_CLR_POS)
#define GLB_REG_GPIO1_INT_CLR_UMSK                              (~(((1U<<GLB_REG_GPIO1_INT_CLR_LEN)-1)<<GLB_REG_GPIO1_INT_CLR_POS))
#define GLB_REG_GPIO2_INT_CLR                                   GLB_REG_GPIO2_INT_CLR
#define GLB_REG_GPIO2_INT_CLR_POS                               (2U)
#define GLB_REG_GPIO2_INT_CLR_LEN                               (1U)
#define GLB_REG_GPIO2_INT_CLR_MSK                               (((1U<<GLB_REG_GPIO2_INT_CLR_LEN)-1)<<GLB_REG_GPIO2_INT_CLR_POS)
#define GLB_REG_GPIO2_INT_CLR_UMSK                              (~(((1U<<GLB_REG_GPIO2_INT_CLR_LEN)-1)<<GLB_REG_GPIO2_INT_CLR_POS))
#define GLB_REG_GPIO3_INT_CLR                                   GLB_REG_GPIO3_INT_CLR
#define GLB_REG_GPIO3_INT_CLR_POS                               (3U)
#define GLB_REG_GPIO3_INT_CLR_LEN                               (1U)
#define GLB_REG_GPIO3_INT_CLR_MSK                               (((1U<<GLB_REG_GPIO3_INT_CLR_LEN)-1)<<GLB_REG_GPIO3_INT_CLR_POS)
#define GLB_REG_GPIO3_INT_CLR_UMSK                              (~(((1U<<GLB_REG_GPIO3_INT_CLR_LEN)-1)<<GLB_REG_GPIO3_INT_CLR_POS))
#define GLB_REG_GPIO7_INT_CLR                                   GLB_REG_GPIO7_INT_CLR
#define GLB_REG_GPIO7_INT_CLR_POS                               (7U)
#define GLB_REG_GPIO7_INT_CLR_LEN                               (1U)
#define GLB_REG_GPIO7_INT_CLR_MSK                               (((1U<<GLB_REG_GPIO7_INT_CLR_LEN)-1)<<GLB_REG_GPIO7_INT_CLR_POS)
#define GLB_REG_GPIO7_INT_CLR_UMSK                              (~(((1U<<GLB_REG_GPIO7_INT_CLR_LEN)-1)<<GLB_REG_GPIO7_INT_CLR_POS))
#define GLB_REG_GPIO8_INT_CLR                                   GLB_REG_GPIO8_INT_CLR
#define GLB_REG_GPIO8_INT_CLR_POS                               (8U)
#define GLB_REG_GPIO8_INT_CLR_LEN                               (1U)
#define GLB_REG_GPIO8_INT_CLR_MSK                               (((1U<<GLB_REG_GPIO8_INT_CLR_LEN)-1)<<GLB_REG_GPIO8_INT_CLR_POS)
#define GLB_REG_GPIO8_INT_CLR_UMSK                              (~(((1U<<GLB_REG_GPIO8_INT_CLR_LEN)-1)<<GLB_REG_GPIO8_INT_CLR_POS))
#define GLB_REG_GPIO9_INT_CLR                                   GLB_REG_GPIO9_INT_CLR
#define GLB_REG_GPIO9_INT_CLR_POS                               (9U)
#define GLB_REG_GPIO9_INT_CLR_LEN                               (1U)
#define GLB_REG_GPIO9_INT_CLR_MSK                               (((1U<<GLB_REG_GPIO9_INT_CLR_LEN)-1)<<GLB_REG_GPIO9_INT_CLR_POS)
#define GLB_REG_GPIO9_INT_CLR_UMSK                              (~(((1U<<GLB_REG_GPIO9_INT_CLR_LEN)-1)<<GLB_REG_GPIO9_INT_CLR_POS))
#define GLB_REG_GPIO10_INT_CLR                                  GLB_REG_GPIO10_INT_CLR
#define GLB_REG_GPIO10_INT_CLR_POS                              (10U)
#define GLB_REG_GPIO10_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO10_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO10_INT_CLR_LEN)-1)<<GLB_REG_GPIO10_INT_CLR_POS)
#define GLB_REG_GPIO10_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO10_INT_CLR_LEN)-1)<<GLB_REG_GPIO10_INT_CLR_POS))
#define GLB_REG_GPIO11_INT_CLR                                  GLB_REG_GPIO11_INT_CLR
#define GLB_REG_GPIO11_INT_CLR_POS                              (11U)
#define GLB_REG_GPIO11_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO11_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO11_INT_CLR_LEN)-1)<<GLB_REG_GPIO11_INT_CLR_POS)
#define GLB_REG_GPIO11_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO11_INT_CLR_LEN)-1)<<GLB_REG_GPIO11_INT_CLR_POS))
#define GLB_REG_GPIO12_INT_CLR                                  GLB_REG_GPIO12_INT_CLR
#define GLB_REG_GPIO12_INT_CLR_POS                              (12U)
#define GLB_REG_GPIO12_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO12_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO12_INT_CLR_LEN)-1)<<GLB_REG_GPIO12_INT_CLR_POS)
#define GLB_REG_GPIO12_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO12_INT_CLR_LEN)-1)<<GLB_REG_GPIO12_INT_CLR_POS))
#define GLB_REG_GPIO13_INT_CLR                                  GLB_REG_GPIO13_INT_CLR
#define GLB_REG_GPIO13_INT_CLR_POS                              (13U)
#define GLB_REG_GPIO13_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO13_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO13_INT_CLR_LEN)-1)<<GLB_REG_GPIO13_INT_CLR_POS)
#define GLB_REG_GPIO13_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO13_INT_CLR_LEN)-1)<<GLB_REG_GPIO13_INT_CLR_POS))
#define GLB_REG_GPIO14_INT_CLR                                  GLB_REG_GPIO14_INT_CLR
#define GLB_REG_GPIO14_INT_CLR_POS                              (14U)
#define GLB_REG_GPIO14_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO14_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO14_INT_CLR_LEN)-1)<<GLB_REG_GPIO14_INT_CLR_POS)
#define GLB_REG_GPIO14_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO14_INT_CLR_LEN)-1)<<GLB_REG_GPIO14_INT_CLR_POS))
#define GLB_REG_GPIO15_INT_CLR                                  GLB_REG_GPIO15_INT_CLR
#define GLB_REG_GPIO15_INT_CLR_POS                              (15U)
#define GLB_REG_GPIO15_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO15_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO15_INT_CLR_LEN)-1)<<GLB_REG_GPIO15_INT_CLR_POS)
#define GLB_REG_GPIO15_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO15_INT_CLR_LEN)-1)<<GLB_REG_GPIO15_INT_CLR_POS))
#define GLB_REG_GPIO16_INT_CLR                                  GLB_REG_GPIO16_INT_CLR
#define GLB_REG_GPIO16_INT_CLR_POS                              (16U)
#define GLB_REG_GPIO16_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO16_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO16_INT_CLR_LEN)-1)<<GLB_REG_GPIO16_INT_CLR_POS)
#define GLB_REG_GPIO16_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO16_INT_CLR_LEN)-1)<<GLB_REG_GPIO16_INT_CLR_POS))
#define GLB_REG_GPIO17_INT_CLR                                  GLB_REG_GPIO17_INT_CLR
#define GLB_REG_GPIO17_INT_CLR_POS                              (17U)
#define GLB_REG_GPIO17_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO17_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO17_INT_CLR_LEN)-1)<<GLB_REG_GPIO17_INT_CLR_POS)
#define GLB_REG_GPIO17_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO17_INT_CLR_LEN)-1)<<GLB_REG_GPIO17_INT_CLR_POS))
#define GLB_REG_GPIO18_INT_CLR                                  GLB_REG_GPIO18_INT_CLR
#define GLB_REG_GPIO18_INT_CLR_POS                              (18U)
#define GLB_REG_GPIO18_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO18_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO18_INT_CLR_LEN)-1)<<GLB_REG_GPIO18_INT_CLR_POS)
#define GLB_REG_GPIO18_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO18_INT_CLR_LEN)-1)<<GLB_REG_GPIO18_INT_CLR_POS))
#define GLB_REG_GPIO19_INT_CLR                                  GLB_REG_GPIO19_INT_CLR
#define GLB_REG_GPIO19_INT_CLR_POS                              (19U)
#define GLB_REG_GPIO19_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO19_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO19_INT_CLR_LEN)-1)<<GLB_REG_GPIO19_INT_CLR_POS)
#define GLB_REG_GPIO19_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO19_INT_CLR_LEN)-1)<<GLB_REG_GPIO19_INT_CLR_POS))
#define GLB_REG_GPIO20_INT_CLR                                  GLB_REG_GPIO20_INT_CLR
#define GLB_REG_GPIO20_INT_CLR_POS                              (20U)
#define GLB_REG_GPIO20_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO20_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO20_INT_CLR_LEN)-1)<<GLB_REG_GPIO20_INT_CLR_POS)
#define GLB_REG_GPIO20_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO20_INT_CLR_LEN)-1)<<GLB_REG_GPIO20_INT_CLR_POS))
#define GLB_REG_GPIO21_INT_CLR                                  GLB_REG_GPIO21_INT_CLR
#define GLB_REG_GPIO21_INT_CLR_POS                              (21U)
#define GLB_REG_GPIO21_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO21_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO21_INT_CLR_LEN)-1)<<GLB_REG_GPIO21_INT_CLR_POS)
#define GLB_REG_GPIO21_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO21_INT_CLR_LEN)-1)<<GLB_REG_GPIO21_INT_CLR_POS))
#define GLB_REG_GPIO22_INT_CLR                                  GLB_REG_GPIO22_INT_CLR
#define GLB_REG_GPIO22_INT_CLR_POS                              (22U)
#define GLB_REG_GPIO22_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO22_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO22_INT_CLR_LEN)-1)<<GLB_REG_GPIO22_INT_CLR_POS)
#define GLB_REG_GPIO22_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO22_INT_CLR_LEN)-1)<<GLB_REG_GPIO22_INT_CLR_POS))
#define GLB_REG_GPIO23_INT_CLR                                  GLB_REG_GPIO23_INT_CLR
#define GLB_REG_GPIO23_INT_CLR_POS                              (23U)
#define GLB_REG_GPIO23_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO23_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO23_INT_CLR_LEN)-1)<<GLB_REG_GPIO23_INT_CLR_POS)
#define GLB_REG_GPIO23_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO23_INT_CLR_LEN)-1)<<GLB_REG_GPIO23_INT_CLR_POS))
#define GLB_REG_GPIO24_INT_CLR                                  GLB_REG_GPIO24_INT_CLR
#define GLB_REG_GPIO24_INT_CLR_POS                              (24U)
#define GLB_REG_GPIO24_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO24_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO24_INT_CLR_LEN)-1)<<GLB_REG_GPIO24_INT_CLR_POS)
#define GLB_REG_GPIO24_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO24_INT_CLR_LEN)-1)<<GLB_REG_GPIO24_INT_CLR_POS))
#define GLB_REG_GPIO25_INT_CLR                                  GLB_REG_GPIO25_INT_CLR
#define GLB_REG_GPIO25_INT_CLR_POS                              (25U)
#define GLB_REG_GPIO25_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO25_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO25_INT_CLR_LEN)-1)<<GLB_REG_GPIO25_INT_CLR_POS)
#define GLB_REG_GPIO25_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO25_INT_CLR_LEN)-1)<<GLB_REG_GPIO25_INT_CLR_POS))
#define GLB_REG_GPIO26_INT_CLR                                  GLB_REG_GPIO26_INT_CLR
#define GLB_REG_GPIO26_INT_CLR_POS                              (26U)
#define GLB_REG_GPIO26_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO26_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO26_INT_CLR_LEN)-1)<<GLB_REG_GPIO26_INT_CLR_POS)
#define GLB_REG_GPIO26_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO26_INT_CLR_LEN)-1)<<GLB_REG_GPIO26_INT_CLR_POS))
#define GLB_REG_GPIO27_INT_CLR                                  GLB_REG_GPIO27_INT_CLR
#define GLB_REG_GPIO27_INT_CLR_POS                              (27U)
#define GLB_REG_GPIO27_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO27_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO27_INT_CLR_LEN)-1)<<GLB_REG_GPIO27_INT_CLR_POS)
#define GLB_REG_GPIO27_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO27_INT_CLR_LEN)-1)<<GLB_REG_GPIO27_INT_CLR_POS))
#define GLB_REG_GPIO28_INT_CLR                                  GLB_REG_GPIO28_INT_CLR
#define GLB_REG_GPIO28_INT_CLR_POS                              (28U)
#define GLB_REG_GPIO28_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO28_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO28_INT_CLR_LEN)-1)<<GLB_REG_GPIO28_INT_CLR_POS)
#define GLB_REG_GPIO28_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO28_INT_CLR_LEN)-1)<<GLB_REG_GPIO28_INT_CLR_POS))
#define GLB_REG_GPIO29_INT_CLR                                  GLB_REG_GPIO29_INT_CLR
#define GLB_REG_GPIO29_INT_CLR_POS                              (29U)
#define GLB_REG_GPIO29_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO29_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO29_INT_CLR_LEN)-1)<<GLB_REG_GPIO29_INT_CLR_POS)
#define GLB_REG_GPIO29_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO29_INT_CLR_LEN)-1)<<GLB_REG_GPIO29_INT_CLR_POS))
#define GLB_REG_GPIO30_INT_CLR                                  GLB_REG_GPIO30_INT_CLR
#define GLB_REG_GPIO30_INT_CLR_POS                              (30U)
#define GLB_REG_GPIO30_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO30_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO30_INT_CLR_LEN)-1)<<GLB_REG_GPIO30_INT_CLR_POS)
#define GLB_REG_GPIO30_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO30_INT_CLR_LEN)-1)<<GLB_REG_GPIO30_INT_CLR_POS))
#define GLB_REG_GPIO31_INT_CLR                                  GLB_REG_GPIO31_INT_CLR
#define GLB_REG_GPIO31_INT_CLR_POS                              (31U)
#define GLB_REG_GPIO31_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO31_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO31_INT_CLR_LEN)-1)<<GLB_REG_GPIO31_INT_CLR_POS)
#define GLB_REG_GPIO31_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO31_INT_CLR_LEN)-1)<<GLB_REG_GPIO31_INT_CLR_POS))

/* 0x1C0 : GPIO_INT_MODE_SET1 */
#define GLB_GPIO_INT_MODE_SET1_OFFSET                           (0x1C0)
#define GLB_REG_GPIO0_INT_MODE_SET                              GLB_REG_GPIO0_INT_MODE_SET
#define GLB_REG_GPIO0_INT_MODE_SET_POS                          (0U)
#define GLB_REG_GPIO0_INT_MODE_SET_LEN                          (4U)
#define GLB_REG_GPIO0_INT_MODE_SET_MSK                          (((1U<<GLB_REG_GPIO0_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO0_INT_MODE_SET_POS)
#define GLB_REG_GPIO0_INT_MODE_SET_UMSK                         (~(((1U<<GLB_REG_GPIO0_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO0_INT_MODE_SET_POS))
#define GLB_REG_GPIO1_INT_MODE_SET                              GLB_REG_GPIO1_INT_MODE_SET
#define GLB_REG_GPIO1_INT_MODE_SET_POS                          (4U)
#define GLB_REG_GPIO1_INT_MODE_SET_LEN                          (4U)
#define GLB_REG_GPIO1_INT_MODE_SET_MSK                          (((1U<<GLB_REG_GPIO1_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO1_INT_MODE_SET_POS)
#define GLB_REG_GPIO1_INT_MODE_SET_UMSK                         (~(((1U<<GLB_REG_GPIO1_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO1_INT_MODE_SET_POS))
#define GLB_REG_GPIO2_INT_MODE_SET                              GLB_REG_GPIO2_INT_MODE_SET
#define GLB_REG_GPIO2_INT_MODE_SET_POS                          (8U)
#define GLB_REG_GPIO2_INT_MODE_SET_LEN                          (4U)
#define GLB_REG_GPIO2_INT_MODE_SET_MSK                          (((1U<<GLB_REG_GPIO2_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO2_INT_MODE_SET_POS)
#define GLB_REG_GPIO2_INT_MODE_SET_UMSK                         (~(((1U<<GLB_REG_GPIO2_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO2_INT_MODE_SET_POS))
#define GLB_REG_GPIO3_INT_MODE_SET                              GLB_REG_GPIO3_INT_MODE_SET
#define GLB_REG_GPIO3_INT_MODE_SET_POS                          (12U)
#define GLB_REG_GPIO3_INT_MODE_SET_LEN                          (4U)
#define GLB_REG_GPIO3_INT_MODE_SET_MSK                          (((1U<<GLB_REG_GPIO3_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO3_INT_MODE_SET_POS)
#define GLB_REG_GPIO3_INT_MODE_SET_UMSK                         (~(((1U<<GLB_REG_GPIO3_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO3_INT_MODE_SET_POS))
#define GLB_REG_GPIO7_INT_MODE_SET                              GLB_REG_GPIO7_INT_MODE_SET
#define GLB_REG_GPIO7_INT_MODE_SET_POS                          (28U)
#define GLB_REG_GPIO7_INT_MODE_SET_LEN                          (4U)
#define GLB_REG_GPIO7_INT_MODE_SET_MSK                          (((1U<<GLB_REG_GPIO7_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO7_INT_MODE_SET_POS)
#define GLB_REG_GPIO7_INT_MODE_SET_UMSK                         (~(((1U<<GLB_REG_GPIO7_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO7_INT_MODE_SET_POS))

/* 0x1C4 : GPIO_INT_MODE_SET2 */
#define GLB_GPIO_INT_MODE_SET2_OFFSET                           (0x1C4)
#define GLB_REG_GPIO_INT_MODE_SET2                              GLB_REG_GPIO_INT_MODE_SET2
#define GLB_REG_GPIO_INT_MODE_SET2_POS                          (0U)
#define GLB_REG_GPIO_INT_MODE_SET2_LEN                          (32U)
#define GLB_REG_GPIO_INT_MODE_SET2_MSK                          (((1U<<GLB_REG_GPIO_INT_MODE_SET2_LEN)-1)<<GLB_REG_GPIO_INT_MODE_SET2_POS)
#define GLB_REG_GPIO_INT_MODE_SET2_UMSK                         (~(((1U<<GLB_REG_GPIO_INT_MODE_SET2_LEN)-1)<<GLB_REG_GPIO_INT_MODE_SET2_POS))

/* 0x1C8 : GPIO_INT_MODE_SET3 */
#define GLB_GPIO_INT_MODE_SET3_OFFSET                           (0x1C8)
#define GLB_REG_GPIO_INT_MODE_SET3                              GLB_REG_GPIO_INT_MODE_SET3
#define GLB_REG_GPIO_INT_MODE_SET3_POS                          (0U)
#define GLB_REG_GPIO_INT_MODE_SET3_LEN                          (32U)
#define GLB_REG_GPIO_INT_MODE_SET3_MSK                          (((1U<<GLB_REG_GPIO_INT_MODE_SET3_LEN)-1)<<GLB_REG_GPIO_INT_MODE_SET3_POS)
#define GLB_REG_GPIO_INT_MODE_SET3_UMSK                         (~(((1U<<GLB_REG_GPIO_INT_MODE_SET3_LEN)-1)<<GLB_REG_GPIO_INT_MODE_SET3_POS))

/* 0x1CC : GPIO_INT_MODE_SET4 */
#define GLB_GPIO_INT_MODE_SET4_OFFSET                           (0x1CC)
#define GLB_REG_GPIO_INT_MODE_SET4                              GLB_REG_GPIO_INT_MODE_SET4
#define GLB_REG_GPIO_INT_MODE_SET4_POS                          (0U)
#define GLB_REG_GPIO_INT_MODE_SET4_LEN                          (32U)
#define GLB_REG_GPIO_INT_MODE_SET4_MSK                          (((1U<<GLB_REG_GPIO_INT_MODE_SET4_LEN)-1)<<GLB_REG_GPIO_INT_MODE_SET4_POS)
#define GLB_REG_GPIO_INT_MODE_SET4_UMSK                         (~(((1U<<GLB_REG_GPIO_INT_MODE_SET4_LEN)-1)<<GLB_REG_GPIO_INT_MODE_SET4_POS))

/* 0x200 : dll */
#define GLB_DLL_OFFSET                                          (0x200)
#define GLB_TEN_DLL                                             GLB_TEN_DLL
#define GLB_TEN_DLL_POS                                         (2U)
#define GLB_TEN_DLL_LEN                                         (1U)
#define GLB_TEN_DLL_MSK                                         (((1U<<GLB_TEN_DLL_LEN)-1)<<GLB_TEN_DLL_POS)
#define GLB_TEN_DLL_UMSK                                        (~(((1U<<GLB_TEN_DLL_LEN)-1)<<GLB_TEN_DLL_POS))
#define GLB_DLL_VCTRL_SEL                                       GLB_DLL_VCTRL_SEL
#define GLB_DLL_VCTRL_SEL_POS                                   (8U)
#define GLB_DLL_VCTRL_SEL_LEN                                   (3U)
#define GLB_DLL_VCTRL_SEL_MSK                                   (((1U<<GLB_DLL_VCTRL_SEL_LEN)-1)<<GLB_DLL_VCTRL_SEL_POS)
#define GLB_DLL_VCTRL_SEL_UMSK                                  (~(((1U<<GLB_DLL_VCTRL_SEL_LEN)-1)<<GLB_DLL_VCTRL_SEL_POS))
#define GLB_DLL_PRECHG_SEL                                      GLB_DLL_PRECHG_SEL
#define GLB_DLL_PRECHG_SEL_POS                                  (12U)
#define GLB_DLL_PRECHG_SEL_LEN                                  (1U)
#define GLB_DLL_PRECHG_SEL_MSK                                  (((1U<<GLB_DLL_PRECHG_SEL_LEN)-1)<<GLB_DLL_PRECHG_SEL_POS)
#define GLB_DLL_PRECHG_SEL_UMSK                                 (~(((1U<<GLB_DLL_PRECHG_SEL_LEN)-1)<<GLB_DLL_PRECHG_SEL_POS))
#define GLB_DLL_PRECHG_REG                                      GLB_DLL_PRECHG_REG
#define GLB_DLL_PRECHG_REG_POS                                  (13U)
#define GLB_DLL_PRECHG_REG_LEN                                  (1U)
#define GLB_DLL_PRECHG_REG_MSK                                  (((1U<<GLB_DLL_PRECHG_REG_LEN)-1)<<GLB_DLL_PRECHG_REG_POS)
#define GLB_DLL_PRECHG_REG_UMSK                                 (~(((1U<<GLB_DLL_PRECHG_REG_LEN)-1)<<GLB_DLL_PRECHG_REG_POS))
#define GLB_DLL_PRECHG_EN                                       GLB_DLL_PRECHG_EN
#define GLB_DLL_PRECHG_EN_POS                                   (14U)
#define GLB_DLL_PRECHG_EN_LEN                                   (1U)
#define GLB_DLL_PRECHG_EN_MSK                                   (((1U<<GLB_DLL_PRECHG_EN_LEN)-1)<<GLB_DLL_PRECHG_EN_POS)
#define GLB_DLL_PRECHG_EN_UMSK                                  (~(((1U<<GLB_DLL_PRECHG_EN_LEN)-1)<<GLB_DLL_PRECHG_EN_POS))
#define GLB_DLL_VCTRL_FORCE_EN                                  GLB_DLL_VCTRL_FORCE_EN
#define GLB_DLL_VCTRL_FORCE_EN_POS                              (15U)
#define GLB_DLL_VCTRL_FORCE_EN_LEN                              (1U)
#define GLB_DLL_VCTRL_FORCE_EN_MSK                              (((1U<<GLB_DLL_VCTRL_FORCE_EN_LEN)-1)<<GLB_DLL_VCTRL_FORCE_EN_POS)
#define GLB_DLL_VCTRL_FORCE_EN_UMSK                             (~(((1U<<GLB_DLL_VCTRL_FORCE_EN_LEN)-1)<<GLB_DLL_VCTRL_FORCE_EN_POS))
#define GLB_DLL_POSTDIV                                         GLB_DLL_POSTDIV
#define GLB_DLL_POSTDIV_POS                                     (16U)
#define GLB_DLL_POSTDIV_LEN                                     (6U)
#define GLB_DLL_POSTDIV_MSK                                     (((1U<<GLB_DLL_POSTDIV_LEN)-1)<<GLB_DLL_POSTDIV_POS)
#define GLB_DLL_POSTDIV_UMSK                                    (~(((1U<<GLB_DLL_POSTDIV_LEN)-1)<<GLB_DLL_POSTDIV_POS))
#define GLB_DLL_DELAY_SEL                                       GLB_DLL_DELAY_SEL
#define GLB_DLL_DELAY_SEL_POS                                   (24U)
#define GLB_DLL_DELAY_SEL_LEN                                   (2U)
#define GLB_DLL_DELAY_SEL_MSK                                   (((1U<<GLB_DLL_DELAY_SEL_LEN)-1)<<GLB_DLL_DELAY_SEL_POS)
#define GLB_DLL_DELAY_SEL_UMSK                                  (~(((1U<<GLB_DLL_DELAY_SEL_LEN)-1)<<GLB_DLL_DELAY_SEL_POS))
#define GLB_DLL_CP_OP_EN                                        GLB_DLL_CP_OP_EN
#define GLB_DLL_CP_OP_EN_POS                                    (26U)
#define GLB_DLL_CP_OP_EN_LEN                                    (1U)
#define GLB_DLL_CP_OP_EN_MSK                                    (((1U<<GLB_DLL_CP_OP_EN_LEN)-1)<<GLB_DLL_CP_OP_EN_POS)
#define GLB_DLL_CP_OP_EN_UMSK                                   (~(((1U<<GLB_DLL_CP_OP_EN_LEN)-1)<<GLB_DLL_CP_OP_EN_POS))
#define GLB_DLL_CP_HIZ                                          GLB_DLL_CP_HIZ
#define GLB_DLL_CP_HIZ_POS                                      (27U)
#define GLB_DLL_CP_HIZ_LEN                                      (1U)
#define GLB_DLL_CP_HIZ_MSK                                      (((1U<<GLB_DLL_CP_HIZ_LEN)-1)<<GLB_DLL_CP_HIZ_POS)
#define GLB_DLL_CP_HIZ_UMSK                                     (~(((1U<<GLB_DLL_CP_HIZ_LEN)-1)<<GLB_DLL_CP_HIZ_POS))
#define GLB_DLL_REFCLK_SEL                                      GLB_DLL_REFCLK_SEL
#define GLB_DLL_REFCLK_SEL_POS                                  (28U)
#define GLB_DLL_REFCLK_SEL_LEN                                  (1U)
#define GLB_DLL_REFCLK_SEL_MSK                                  (((1U<<GLB_DLL_REFCLK_SEL_LEN)-1)<<GLB_DLL_REFCLK_SEL_POS)
#define GLB_DLL_REFCLK_SEL_UMSK                                 (~(((1U<<GLB_DLL_REFCLK_SEL_LEN)-1)<<GLB_DLL_REFCLK_SEL_POS))
#define GLB_DLL_RESET                                           GLB_DLL_RESET
#define GLB_DLL_RESET_POS                                       (29U)
#define GLB_DLL_RESET_LEN                                       (1U)
#define GLB_DLL_RESET_MSK                                       (((1U<<GLB_DLL_RESET_LEN)-1)<<GLB_DLL_RESET_POS)
#define GLB_DLL_RESET_UMSK                                      (~(((1U<<GLB_DLL_RESET_LEN)-1)<<GLB_DLL_RESET_POS))
#define GLB_PU_DLL                                              GLB_PU_DLL
#define GLB_PU_DLL_POS                                          (30U)
#define GLB_PU_DLL_LEN                                          (1U)
#define GLB_PU_DLL_MSK                                          (((1U<<GLB_PU_DLL_LEN)-1)<<GLB_PU_DLL_POS)
#define GLB_PU_DLL_UMSK                                         (~(((1U<<GLB_PU_DLL_LEN)-1)<<GLB_PU_DLL_POS))
#define GLB_PPU_DLL                                             GLB_PPU_DLL
#define GLB_PPU_DLL_POS                                         (31U)
#define GLB_PPU_DLL_LEN                                         (1U)
#define GLB_PPU_DLL_MSK                                         (((1U<<GLB_PPU_DLL_LEN)-1)<<GLB_PPU_DLL_POS)
#define GLB_PPU_DLL_UMSK                                        (~(((1U<<GLB_PPU_DLL_LEN)-1)<<GLB_PPU_DLL_POS))

/* 0x204 : dll2 */
#define GLB_DLL2_OFFSET                                         (0x204)
#define GLB_DLL_EN_DIV1_RF                                      GLB_DLL_EN_DIV1_RF
#define GLB_DLL_EN_DIV1_RF_POS                                  (0U)
#define GLB_DLL_EN_DIV1_RF_LEN                                  (1U)
#define GLB_DLL_EN_DIV1_RF_MSK                                  (((1U<<GLB_DLL_EN_DIV1_RF_LEN)-1)<<GLB_DLL_EN_DIV1_RF_POS)
#define GLB_DLL_EN_DIV1_RF_UMSK                                 (~(((1U<<GLB_DLL_EN_DIV1_RF_LEN)-1)<<GLB_DLL_EN_DIV1_RF_POS))
#define GLB_DLL_EN_DIV63                                        GLB_DLL_EN_DIV63
#define GLB_DLL_EN_DIV63_POS                                    (1U)
#define GLB_DLL_EN_DIV63_LEN                                    (1U)
#define GLB_DLL_EN_DIV63_MSK                                    (((1U<<GLB_DLL_EN_DIV63_LEN)-1)<<GLB_DLL_EN_DIV63_POS)
#define GLB_DLL_EN_DIV63_UMSK                                   (~(((1U<<GLB_DLL_EN_DIV63_LEN)-1)<<GLB_DLL_EN_DIV63_POS))
#define GLB_DLL_EN_DIV21                                        GLB_DLL_EN_DIV21
#define GLB_DLL_EN_DIV21_POS                                    (2U)
#define GLB_DLL_EN_DIV21_LEN                                    (1U)
#define GLB_DLL_EN_DIV21_MSK                                    (((1U<<GLB_DLL_EN_DIV21_LEN)-1)<<GLB_DLL_EN_DIV21_POS)
#define GLB_DLL_EN_DIV21_UMSK                                   (~(((1U<<GLB_DLL_EN_DIV21_LEN)-1)<<GLB_DLL_EN_DIV21_POS))
#define GLB_DLL_EN_DIV5                                         GLB_DLL_EN_DIV5
#define GLB_DLL_EN_DIV5_POS                                     (3U)
#define GLB_DLL_EN_DIV5_LEN                                     (1U)
#define GLB_DLL_EN_DIV5_MSK                                     (((1U<<GLB_DLL_EN_DIV5_LEN)-1)<<GLB_DLL_EN_DIV5_POS)
#define GLB_DLL_EN_DIV5_UMSK                                    (~(((1U<<GLB_DLL_EN_DIV5_LEN)-1)<<GLB_DLL_EN_DIV5_POS))
#define GLB_DLL_EN_DIV3                                         GLB_DLL_EN_DIV3
#define GLB_DLL_EN_DIV3_POS                                     (4U)
#define GLB_DLL_EN_DIV3_LEN                                     (1U)
#define GLB_DLL_EN_DIV3_MSK                                     (((1U<<GLB_DLL_EN_DIV3_LEN)-1)<<GLB_DLL_EN_DIV3_POS)
#define GLB_DLL_EN_DIV3_UMSK                                    (~(((1U<<GLB_DLL_EN_DIV3_LEN)-1)<<GLB_DLL_EN_DIV3_POS))
#define GLB_DLL_EN_DIV2                                         GLB_DLL_EN_DIV2
#define GLB_DLL_EN_DIV2_POS                                     (5U)
#define GLB_DLL_EN_DIV2_LEN                                     (1U)
#define GLB_DLL_EN_DIV2_MSK                                     (((1U<<GLB_DLL_EN_DIV2_LEN)-1)<<GLB_DLL_EN_DIV2_POS)
#define GLB_DLL_EN_DIV2_UMSK                                    (~(((1U<<GLB_DLL_EN_DIV2_LEN)-1)<<GLB_DLL_EN_DIV2_POS))
#define GLB_DLL_EN_DIV1                                         GLB_DLL_EN_DIV1
#define GLB_DLL_EN_DIV1_POS                                     (6U)
#define GLB_DLL_EN_DIV1_LEN                                     (1U)
#define GLB_DLL_EN_DIV1_MSK                                     (((1U<<GLB_DLL_EN_DIV1_LEN)-1)<<GLB_DLL_EN_DIV1_POS)
#define GLB_DLL_EN_DIV1_UMSK                                    (~(((1U<<GLB_DLL_EN_DIV1_LEN)-1)<<GLB_DLL_EN_DIV1_POS))
#define GLB_DLL_DL_CTRL_DIV1_RF                                 GLB_DLL_DL_CTRL_DIV1_RF
#define GLB_DLL_DL_CTRL_DIV1_RF_POS                             (7U)
#define GLB_DLL_DL_CTRL_DIV1_RF_LEN                             (1U)
#define GLB_DLL_DL_CTRL_DIV1_RF_MSK                             (((1U<<GLB_DLL_DL_CTRL_DIV1_RF_LEN)-1)<<GLB_DLL_DL_CTRL_DIV1_RF_POS)
#define GLB_DLL_DL_CTRL_DIV1_RF_UMSK                            (~(((1U<<GLB_DLL_DL_CTRL_DIV1_RF_LEN)-1)<<GLB_DLL_DL_CTRL_DIV1_RF_POS))
#define GLB_DLL_DL_CTRL_DIV5                                    GLB_DLL_DL_CTRL_DIV5
#define GLB_DLL_DL_CTRL_DIV5_POS                                (8U)
#define GLB_DLL_DL_CTRL_DIV5_LEN                                (1U)
#define GLB_DLL_DL_CTRL_DIV5_MSK                                (((1U<<GLB_DLL_DL_CTRL_DIV5_LEN)-1)<<GLB_DLL_DL_CTRL_DIV5_POS)
#define GLB_DLL_DL_CTRL_DIV5_UMSK                               (~(((1U<<GLB_DLL_DL_CTRL_DIV5_LEN)-1)<<GLB_DLL_DL_CTRL_DIV5_POS))
#define GLB_DLL_DL_CTRL_DIV3                                    GLB_DLL_DL_CTRL_DIV3
#define GLB_DLL_DL_CTRL_DIV3_POS                                (9U)
#define GLB_DLL_DL_CTRL_DIV3_LEN                                (1U)
#define GLB_DLL_DL_CTRL_DIV3_MSK                                (((1U<<GLB_DLL_DL_CTRL_DIV3_LEN)-1)<<GLB_DLL_DL_CTRL_DIV3_POS)
#define GLB_DLL_DL_CTRL_DIV3_UMSK                               (~(((1U<<GLB_DLL_DL_CTRL_DIV3_LEN)-1)<<GLB_DLL_DL_CTRL_DIV3_POS))
#define GLB_DLL_DL_CTRL_DIV2                                    GLB_DLL_DL_CTRL_DIV2
#define GLB_DLL_DL_CTRL_DIV2_POS                                (10U)
#define GLB_DLL_DL_CTRL_DIV2_LEN                                (1U)
#define GLB_DLL_DL_CTRL_DIV2_MSK                                (((1U<<GLB_DLL_DL_CTRL_DIV2_LEN)-1)<<GLB_DLL_DL_CTRL_DIV2_POS)
#define GLB_DLL_DL_CTRL_DIV2_UMSK                               (~(((1U<<GLB_DLL_DL_CTRL_DIV2_LEN)-1)<<GLB_DLL_DL_CTRL_DIV2_POS))
#define GLB_DLL_DL_CTRL_DIV1                                    GLB_DLL_DL_CTRL_DIV1
#define GLB_DLL_DL_CTRL_DIV1_POS                                (11U)
#define GLB_DLL_DL_CTRL_DIV1_LEN                                (1U)
#define GLB_DLL_DL_CTRL_DIV1_MSK                                (((1U<<GLB_DLL_DL_CTRL_DIV1_LEN)-1)<<GLB_DLL_DL_CTRL_DIV1_POS)
#define GLB_DLL_DL_CTRL_DIV1_UMSK                               (~(((1U<<GLB_DLL_DL_CTRL_DIV1_LEN)-1)<<GLB_DLL_DL_CTRL_DIV1_POS))
#define GLB_DLL_POSTDIV_DL_CTRL                                 GLB_DLL_POSTDIV_DL_CTRL
#define GLB_DLL_POSTDIV_DL_CTRL_POS                             (16U)
#define GLB_DLL_POSTDIV_DL_CTRL_LEN                             (1U)
#define GLB_DLL_POSTDIV_DL_CTRL_MSK                             (((1U<<GLB_DLL_POSTDIV_DL_CTRL_LEN)-1)<<GLB_DLL_POSTDIV_DL_CTRL_POS)
#define GLB_DLL_POSTDIV_DL_CTRL_UMSK                            (~(((1U<<GLB_DLL_POSTDIV_DL_CTRL_LEN)-1)<<GLB_DLL_POSTDIV_DL_CTRL_POS))
#define GLB_DLL_POSTDIV_STG2_DIV2                               GLB_DLL_POSTDIV_STG2_DIV2
#define GLB_DLL_POSTDIV_STG2_DIV2_POS                           (17U)
#define GLB_DLL_POSTDIV_STG2_DIV2_LEN                           (1U)
#define GLB_DLL_POSTDIV_STG2_DIV2_MSK                           (((1U<<GLB_DLL_POSTDIV_STG2_DIV2_LEN)-1)<<GLB_DLL_POSTDIV_STG2_DIV2_POS)
#define GLB_DLL_POSTDIV_STG2_DIV2_UMSK                          (~(((1U<<GLB_DLL_POSTDIV_STG2_DIV2_LEN)-1)<<GLB_DLL_POSTDIV_STG2_DIV2_POS))
#define GLB_DLL_POSTDIV_EN                                      GLB_DLL_POSTDIV_EN
#define GLB_DLL_POSTDIV_EN_POS                                  (18U)
#define GLB_DLL_POSTDIV_EN_LEN                                  (1U)
#define GLB_DLL_POSTDIV_EN_MSK                                  (((1U<<GLB_DLL_POSTDIV_EN_LEN)-1)<<GLB_DLL_POSTDIV_EN_POS)
#define GLB_DLL_POSTDIV_EN_UMSK                                 (~(((1U<<GLB_DLL_POSTDIV_EN_LEN)-1)<<GLB_DLL_POSTDIV_EN_POS))
#define GLB_DLL_REF_POLARITY                                    GLB_DLL_REF_POLARITY
#define GLB_DLL_REF_POLARITY_POS                                (19U)
#define GLB_DLL_REF_POLARITY_LEN                                (1U)
#define GLB_DLL_REF_POLARITY_MSK                                (((1U<<GLB_DLL_REF_POLARITY_LEN)-1)<<GLB_DLL_REF_POLARITY_POS)
#define GLB_DLL_REF_POLARITY_UMSK                               (~(((1U<<GLB_DLL_REF_POLARITY_LEN)-1)<<GLB_DLL_REF_POLARITY_POS))
#define GLB_DTEN_DLL_REFBUF                                     GLB_DTEN_DLL_REFBUF
#define GLB_DTEN_DLL_REFBUF_POS                                 (20U)
#define GLB_DTEN_DLL_REFBUF_LEN                                 (1U)
#define GLB_DTEN_DLL_REFBUF_MSK                                 (((1U<<GLB_DTEN_DLL_REFBUF_LEN)-1)<<GLB_DTEN_DLL_REFBUF_POS)
#define GLB_DTEN_DLL_REFBUF_UMSK                                (~(((1U<<GLB_DTEN_DLL_REFBUF_LEN)-1)<<GLB_DTEN_DLL_REFBUF_POS))
#define GLB_DTEN_DLL_POSTDIV                                    GLB_DTEN_DLL_POSTDIV
#define GLB_DTEN_DLL_POSTDIV_POS                                (21U)
#define GLB_DTEN_DLL_POSTDIV_LEN                                (1U)
#define GLB_DTEN_DLL_POSTDIV_MSK                                (((1U<<GLB_DTEN_DLL_POSTDIV_LEN)-1)<<GLB_DTEN_DLL_POSTDIV_POS)
#define GLB_DTEN_DLL_POSTDIV_UMSK                               (~(((1U<<GLB_DTEN_DLL_POSTDIV_LEN)-1)<<GLB_DTEN_DLL_POSTDIV_POS))
#define GLB_DTEN_DLL_CLKDIV63                                   GLB_DTEN_DLL_CLKDIV63
#define GLB_DTEN_DLL_CLKDIV63_POS                               (22U)
#define GLB_DTEN_DLL_CLKDIV63_LEN                               (1U)
#define GLB_DTEN_DLL_CLKDIV63_MSK                               (((1U<<GLB_DTEN_DLL_CLKDIV63_LEN)-1)<<GLB_DTEN_DLL_CLKDIV63_POS)
#define GLB_DTEN_DLL_CLKDIV63_UMSK                              (~(((1U<<GLB_DTEN_DLL_CLKDIV63_LEN)-1)<<GLB_DTEN_DLL_CLKDIV63_POS))
#define GLB_DLL_RESV                                            GLB_DLL_RESV
#define GLB_DLL_RESV_POS                                        (24U)
#define GLB_DLL_RESV_LEN                                        (8U)
#define GLB_DLL_RESV_MSK                                        (((1U<<GLB_DLL_RESV_LEN)-1)<<GLB_DLL_RESV_POS)
#define GLB_DLL_RESV_UMSK                                       (~(((1U<<GLB_DLL_RESV_LEN)-1)<<GLB_DLL_RESV_POS))

/* 0x224 : led_driver */
#define GLB_LED_DRIVER_OFFSET                                   (0x224)
#define GLB_LED_DIN_REG                                         GLB_LED_DIN_REG
#define GLB_LED_DIN_REG_POS                                     (0U)
#define GLB_LED_DIN_REG_LEN                                     (1U)
#define GLB_LED_DIN_REG_MSK                                     (((1U<<GLB_LED_DIN_REG_LEN)-1)<<GLB_LED_DIN_REG_POS)
#define GLB_LED_DIN_REG_UMSK                                    (~(((1U<<GLB_LED_DIN_REG_LEN)-1)<<GLB_LED_DIN_REG_POS))
#define GLB_LED_DIN_SEL                                         GLB_LED_DIN_SEL
#define GLB_LED_DIN_SEL_POS                                     (1U)
#define GLB_LED_DIN_SEL_LEN                                     (1U)
#define GLB_LED_DIN_SEL_MSK                                     (((1U<<GLB_LED_DIN_SEL_LEN)-1)<<GLB_LED_DIN_SEL_POS)
#define GLB_LED_DIN_SEL_UMSK                                    (~(((1U<<GLB_LED_DIN_SEL_LEN)-1)<<GLB_LED_DIN_SEL_POS))
#define GLB_LED_DIN_POLARITY_SEL                                GLB_LED_DIN_POLARITY_SEL
#define GLB_LED_DIN_POLARITY_SEL_POS                            (2U)
#define GLB_LED_DIN_POLARITY_SEL_LEN                            (1U)
#define GLB_LED_DIN_POLARITY_SEL_MSK                            (((1U<<GLB_LED_DIN_POLARITY_SEL_LEN)-1)<<GLB_LED_DIN_POLARITY_SEL_POS)
#define GLB_LED_DIN_POLARITY_SEL_UMSK                           (~(((1U<<GLB_LED_DIN_POLARITY_SEL_LEN)-1)<<GLB_LED_DIN_POLARITY_SEL_POS))
#define GLB_LEDDRV_IBIAS                                        GLB_LEDDRV_IBIAS
#define GLB_LEDDRV_IBIAS_POS                                    (4U)
#define GLB_LEDDRV_IBIAS_LEN                                    (3U)
#define GLB_LEDDRV_IBIAS_MSK                                    (((1U<<GLB_LEDDRV_IBIAS_LEN)-1)<<GLB_LEDDRV_IBIAS_POS)
#define GLB_LEDDRV_IBIAS_UMSK                                   (~(((1U<<GLB_LEDDRV_IBIAS_LEN)-1)<<GLB_LEDDRV_IBIAS_POS))
#define GLB_LEDDRV_OUT_EN                                       GLB_LEDDRV_OUT_EN
#define GLB_LEDDRV_OUT_EN_POS                                   (28U)
#define GLB_LEDDRV_OUT_EN_LEN                                   (2U)
#define GLB_LEDDRV_OUT_EN_MSK                                   (((1U<<GLB_LEDDRV_OUT_EN_LEN)-1)<<GLB_LEDDRV_OUT_EN_POS)
#define GLB_LEDDRV_OUT_EN_UMSK                                  (~(((1U<<GLB_LEDDRV_OUT_EN_LEN)-1)<<GLB_LEDDRV_OUT_EN_POS))
#define GLB_PU_LEDDRV                                           GLB_PU_LEDDRV
#define GLB_PU_LEDDRV_POS                                       (31U)
#define GLB_PU_LEDDRV_LEN                                       (1U)
#define GLB_PU_LEDDRV_MSK                                       (((1U<<GLB_PU_LEDDRV_LEN)-1)<<GLB_PU_LEDDRV_POS)
#define GLB_PU_LEDDRV_UMSK                                      (~(((1U<<GLB_PU_LEDDRV_LEN)-1)<<GLB_PU_LEDDRV_POS))

/* 0x228 : usb_xcvr */
#define GLB_USB_XCVR_OFFSET                                     (0x228)

/* 0x22C : usb_xcvr_config */
#define GLB_USB_XCVR_CONFIG_OFFSET                              (0x22C)

/* 0x300 : rc32m_ctrl0 */
#define GLB_RC32M_CTRL0_OFFSET                                  (0x300)
#define GLB_RC32M_CAL_DONE                                      GLB_RC32M_CAL_DONE
#define GLB_RC32M_CAL_DONE_POS                                  (0U)
#define GLB_RC32M_CAL_DONE_LEN                                  (1U)
#define GLB_RC32M_CAL_DONE_MSK                                  (((1U<<GLB_RC32M_CAL_DONE_LEN)-1)<<GLB_RC32M_CAL_DONE_POS)
#define GLB_RC32M_CAL_DONE_UMSK                                 (~(((1U<<GLB_RC32M_CAL_DONE_LEN)-1)<<GLB_RC32M_CAL_DONE_POS))
#define GLB_RC32M_RDY                                           GLB_RC32M_RDY
#define GLB_RC32M_RDY_POS                                       (1U)
#define GLB_RC32M_RDY_LEN                                       (1U)
#define GLB_RC32M_RDY_MSK                                       (((1U<<GLB_RC32M_RDY_LEN)-1)<<GLB_RC32M_RDY_POS)
#define GLB_RC32M_RDY_UMSK                                      (~(((1U<<GLB_RC32M_RDY_LEN)-1)<<GLB_RC32M_RDY_POS))
#define GLB_RC32M_CAL_INPROGRESS                                GLB_RC32M_CAL_INPROGRESS
#define GLB_RC32M_CAL_INPROGRESS_POS                            (2U)
#define GLB_RC32M_CAL_INPROGRESS_LEN                            (1U)
#define GLB_RC32M_CAL_INPROGRESS_MSK                            (((1U<<GLB_RC32M_CAL_INPROGRESS_LEN)-1)<<GLB_RC32M_CAL_INPROGRESS_POS)
#define GLB_RC32M_CAL_INPROGRESS_UMSK                           (~(((1U<<GLB_RC32M_CAL_INPROGRESS_LEN)-1)<<GLB_RC32M_CAL_INPROGRESS_POS))
#define GLB_RC32M_CAL_DIV                                       GLB_RC32M_CAL_DIV
#define GLB_RC32M_CAL_DIV_POS                                   (3U)
#define GLB_RC32M_CAL_DIV_LEN                                   (2U)
#define GLB_RC32M_CAL_DIV_MSK                                   (((1U<<GLB_RC32M_CAL_DIV_LEN)-1)<<GLB_RC32M_CAL_DIV_POS)
#define GLB_RC32M_CAL_DIV_UMSK                                  (~(((1U<<GLB_RC32M_CAL_DIV_LEN)-1)<<GLB_RC32M_CAL_DIV_POS))
#define GLB_RC32M_CAL_PRECHARGE                                 GLB_RC32M_CAL_PRECHARGE
#define GLB_RC32M_CAL_PRECHARGE_POS                             (5U)
#define GLB_RC32M_CAL_PRECHARGE_LEN                             (1U)
#define GLB_RC32M_CAL_PRECHARGE_MSK                             (((1U<<GLB_RC32M_CAL_PRECHARGE_LEN)-1)<<GLB_RC32M_CAL_PRECHARGE_POS)
#define GLB_RC32M_CAL_PRECHARGE_UMSK                            (~(((1U<<GLB_RC32M_CAL_PRECHARGE_LEN)-1)<<GLB_RC32M_CAL_PRECHARGE_POS))
#define GLB_RC32M_DIG_CODE_FR_CAL                               GLB_RC32M_DIG_CODE_FR_CAL
#define GLB_RC32M_DIG_CODE_FR_CAL_POS                           (6U)
#define GLB_RC32M_DIG_CODE_FR_CAL_LEN                           (8U)
#define GLB_RC32M_DIG_CODE_FR_CAL_MSK                           (((1U<<GLB_RC32M_DIG_CODE_FR_CAL_LEN)-1)<<GLB_RC32M_DIG_CODE_FR_CAL_POS)
#define GLB_RC32M_DIG_CODE_FR_CAL_UMSK                          (~(((1U<<GLB_RC32M_DIG_CODE_FR_CAL_LEN)-1)<<GLB_RC32M_DIG_CODE_FR_CAL_POS))
#define GLB_RC32M_ALLOW_CAL                                     GLB_RC32M_ALLOW_CAL
#define GLB_RC32M_ALLOW_CAL_POS                                 (17U)
#define GLB_RC32M_ALLOW_CAL_LEN                                 (1U)
#define GLB_RC32M_ALLOW_CAL_MSK                                 (((1U<<GLB_RC32M_ALLOW_CAL_LEN)-1)<<GLB_RC32M_ALLOW_CAL_POS)
#define GLB_RC32M_ALLOW_CAL_UMSK                                (~(((1U<<GLB_RC32M_ALLOW_CAL_LEN)-1)<<GLB_RC32M_ALLOW_CAL_POS))
#define GLB_RC32M_EXT_CODE_EN                                   GLB_RC32M_EXT_CODE_EN
#define GLB_RC32M_EXT_CODE_EN_POS                               (19U)
#define GLB_RC32M_EXT_CODE_EN_LEN                               (1U)
#define GLB_RC32M_EXT_CODE_EN_MSK                               (((1U<<GLB_RC32M_EXT_CODE_EN_LEN)-1)<<GLB_RC32M_EXT_CODE_EN_POS)
#define GLB_RC32M_EXT_CODE_EN_UMSK                              (~(((1U<<GLB_RC32M_EXT_CODE_EN_LEN)-1)<<GLB_RC32M_EXT_CODE_EN_POS))
#define GLB_RC32M_CAL_EN                                        GLB_RC32M_CAL_EN
#define GLB_RC32M_CAL_EN_POS                                    (20U)
#define GLB_RC32M_CAL_EN_LEN                                    (1U)
#define GLB_RC32M_CAL_EN_MSK                                    (((1U<<GLB_RC32M_CAL_EN_LEN)-1)<<GLB_RC32M_CAL_EN_POS)
#define GLB_RC32M_CAL_EN_UMSK                                   (~(((1U<<GLB_RC32M_CAL_EN_LEN)-1)<<GLB_RC32M_CAL_EN_POS))
#define GLB_RC32M_PD                                            GLB_RC32M_PD
#define GLB_RC32M_PD_POS                                        (21U)
#define GLB_RC32M_PD_LEN                                        (1U)
#define GLB_RC32M_PD_MSK                                        (((1U<<GLB_RC32M_PD_LEN)-1)<<GLB_RC32M_PD_POS)
#define GLB_RC32M_PD_UMSK                                       (~(((1U<<GLB_RC32M_PD_LEN)-1)<<GLB_RC32M_PD_POS))
#define GLB_RC32M_CODE_FR_EXT                                   GLB_RC32M_CODE_FR_EXT
#define GLB_RC32M_CODE_FR_EXT_POS                               (22U)
#define GLB_RC32M_CODE_FR_EXT_LEN                               (8U)
#define GLB_RC32M_CODE_FR_EXT_MSK                               (((1U<<GLB_RC32M_CODE_FR_EXT_LEN)-1)<<GLB_RC32M_CODE_FR_EXT_POS)
#define GLB_RC32M_CODE_FR_EXT_UMSK                              (~(((1U<<GLB_RC32M_CODE_FR_EXT_LEN)-1)<<GLB_RC32M_CODE_FR_EXT_POS))
#define GLB_RC32M_RC32K_CAL_CGEN                                GLB_RC32M_RC32K_CAL_CGEN
#define GLB_RC32M_RC32K_CAL_CGEN_POS                            (30U)
#define GLB_RC32M_RC32K_CAL_CGEN_LEN                            (1U)
#define GLB_RC32M_RC32K_CAL_CGEN_MSK                            (((1U<<GLB_RC32M_RC32K_CAL_CGEN_LEN)-1)<<GLB_RC32M_RC32K_CAL_CGEN_POS)
#define GLB_RC32M_RC32K_CAL_CGEN_UMSK                           (~(((1U<<GLB_RC32M_RC32K_CAL_CGEN_LEN)-1)<<GLB_RC32M_RC32K_CAL_CGEN_POS))

/* 0x304 : rc32m_ctrl1 */
#define GLB_RC32M_CTRL1_OFFSET                                  (0x304)
#define GLB_RC32M_TEST_EN                                       GLB_RC32M_TEST_EN
#define GLB_RC32M_TEST_EN_POS                                   (0U)
#define GLB_RC32M_TEST_EN_LEN                                   (1U)
#define GLB_RC32M_TEST_EN_MSK                                   (((1U<<GLB_RC32M_TEST_EN_LEN)-1)<<GLB_RC32M_TEST_EN_POS)
#define GLB_RC32M_TEST_EN_UMSK                                  (~(((1U<<GLB_RC32M_TEST_EN_LEN)-1)<<GLB_RC32M_TEST_EN_POS))
#define GLB_RC32M_CODE_FR_EXT2                                  GLB_RC32M_CODE_FR_EXT2
#define GLB_RC32M_CODE_FR_EXT2_POS                              (8U)
#define GLB_RC32M_CODE_FR_EXT2_LEN                              (8U)
#define GLB_RC32M_CODE_FR_EXT2_MSK                              (((1U<<GLB_RC32M_CODE_FR_EXT2_LEN)-1)<<GLB_RC32M_CODE_FR_EXT2_POS)
#define GLB_RC32M_CODE_FR_EXT2_UMSK                             (~(((1U<<GLB_RC32M_CODE_FR_EXT2_LEN)-1)<<GLB_RC32M_CODE_FR_EXT2_POS))
#define GLB_RC32M_EXT_CODE_SEL                                  GLB_RC32M_EXT_CODE_SEL
#define GLB_RC32M_EXT_CODE_SEL_POS                              (16U)
#define GLB_RC32M_EXT_CODE_SEL_LEN                              (1U)
#define GLB_RC32M_EXT_CODE_SEL_MSK                              (((1U<<GLB_RC32M_EXT_CODE_SEL_LEN)-1)<<GLB_RC32M_EXT_CODE_SEL_POS)
#define GLB_RC32M_EXT_CODE_SEL_UMSK                             (~(((1U<<GLB_RC32M_EXT_CODE_SEL_LEN)-1)<<GLB_RC32M_EXT_CODE_SEL_POS))
#define GLB_RC32M_EXT_CODE_SEL_CGEN                             GLB_RC32M_EXT_CODE_SEL_CGEN
#define GLB_RC32M_EXT_CODE_SEL_CGEN_POS                         (17U)
#define GLB_RC32M_EXT_CODE_SEL_CGEN_LEN                         (1U)
#define GLB_RC32M_EXT_CODE_SEL_CGEN_MSK                         (((1U<<GLB_RC32M_EXT_CODE_SEL_CGEN_LEN)-1)<<GLB_RC32M_EXT_CODE_SEL_CGEN_POS)
#define GLB_RC32M_EXT_CODE_SEL_CGEN_UMSK                        (~(((1U<<GLB_RC32M_EXT_CODE_SEL_CGEN_LEN)-1)<<GLB_RC32M_EXT_CODE_SEL_CGEN_POS))
#define GLB_RC32M_RESERVED                                      GLB_RC32M_RESERVED
#define GLB_RC32M_RESERVED_POS                                  (24U)
#define GLB_RC32M_RESERVED_LEN                                  (8U)
#define GLB_RC32M_RESERVED_MSK                                  (((1U<<GLB_RC32M_RESERVED_LEN)-1)<<GLB_RC32M_RESERVED_POS)
#define GLB_RC32M_RESERVED_UMSK                                 (~(((1U<<GLB_RC32M_RESERVED_LEN)-1)<<GLB_RC32M_RESERVED_POS))

/* 0x308 : rc32k_ctrl2 */
#define GLB_RC32K_CTRL2_OFFSET                                  (0x308)
#define GLB_RC32K_DIG_CAL_DONE                                  GLB_RC32K_DIG_CAL_DONE
#define GLB_RC32K_DIG_CAL_DONE_POS                              (0U)
#define GLB_RC32K_DIG_CAL_DONE_LEN                              (1U)
#define GLB_RC32K_DIG_CAL_DONE_MSK                              (((1U<<GLB_RC32K_DIG_CAL_DONE_LEN)-1)<<GLB_RC32K_DIG_CAL_DONE_POS)
#define GLB_RC32K_DIG_CAL_DONE_UMSK                             (~(((1U<<GLB_RC32K_DIG_CAL_DONE_LEN)-1)<<GLB_RC32K_DIG_CAL_DONE_POS))
#define GLB_RC32K_RDY                                           GLB_RC32K_RDY
#define GLB_RC32K_RDY_POS                                       (1U)
#define GLB_RC32K_RDY_LEN                                       (1U)
#define GLB_RC32K_RDY_MSK                                       (((1U<<GLB_RC32K_RDY_LEN)-1)<<GLB_RC32K_RDY_POS)
#define GLB_RC32K_RDY_UMSK                                      (~(((1U<<GLB_RC32K_RDY_LEN)-1)<<GLB_RC32K_RDY_POS))
#define GLB_RC32K_DIG_CAL_INPROGRESS                            GLB_RC32K_DIG_CAL_INPROGRESS
#define GLB_RC32K_DIG_CAL_INPROGRESS_POS                        (2U)
#define GLB_RC32K_DIG_CAL_INPROGRESS_LEN                        (1U)
#define GLB_RC32K_DIG_CAL_INPROGRESS_MSK                        (((1U<<GLB_RC32K_DIG_CAL_INPROGRESS_LEN)-1)<<GLB_RC32K_DIG_CAL_INPROGRESS_POS)
#define GLB_RC32K_DIG_CAL_INPROGRESS_UMSK                       (~(((1U<<GLB_RC32K_DIG_CAL_INPROGRESS_LEN)-1)<<GLB_RC32K_DIG_CAL_INPROGRESS_POS))
#define GLB_RC32K_DIG_CAL_DIV                                   GLB_RC32K_DIG_CAL_DIV
#define GLB_RC32K_DIG_CAL_DIV_POS                               (3U)
#define GLB_RC32K_DIG_CAL_DIV_LEN                               (2U)
#define GLB_RC32K_DIG_CAL_DIV_MSK                               (((1U<<GLB_RC32K_DIG_CAL_DIV_LEN)-1)<<GLB_RC32K_DIG_CAL_DIV_POS)
#define GLB_RC32K_DIG_CAL_DIV_UMSK                              (~(((1U<<GLB_RC32K_DIG_CAL_DIV_LEN)-1)<<GLB_RC32K_DIG_CAL_DIV_POS))
#define GLB_RC32K_DIG_CAL_PRECHARGE                             GLB_RC32K_DIG_CAL_PRECHARGE
#define GLB_RC32K_DIG_CAL_PRECHARGE_POS                         (5U)
#define GLB_RC32K_DIG_CAL_PRECHARGE_LEN                         (1U)
#define GLB_RC32K_DIG_CAL_PRECHARGE_MSK                         (((1U<<GLB_RC32K_DIG_CAL_PRECHARGE_LEN)-1)<<GLB_RC32K_DIG_CAL_PRECHARGE_POS)
#define GLB_RC32K_DIG_CAL_PRECHARGE_UMSK                        (~(((1U<<GLB_RC32K_DIG_CAL_PRECHARGE_LEN)-1)<<GLB_RC32K_DIG_CAL_PRECHARGE_POS))
#define GLB_RC32K_DIG_CAP_CODE_FR_CAL                           GLB_RC32K_DIG_CAP_CODE_FR_CAL
#define GLB_RC32K_DIG_CAP_CODE_FR_CAL_POS                       (6U)
#define GLB_RC32K_DIG_CAP_CODE_FR_CAL_LEN                       (4U)
#define GLB_RC32K_DIG_CAP_CODE_FR_CAL_MSK                       (((1U<<GLB_RC32K_DIG_CAP_CODE_FR_CAL_LEN)-1)<<GLB_RC32K_DIG_CAP_CODE_FR_CAL_POS)
#define GLB_RC32K_DIG_CAP_CODE_FR_CAL_UMSK                      (~(((1U<<GLB_RC32K_DIG_CAP_CODE_FR_CAL_LEN)-1)<<GLB_RC32K_DIG_CAP_CODE_FR_CAL_POS))
#define GLB_RC32K_DIG_VREF_DLY                                  GLB_RC32K_DIG_VREF_DLY
#define GLB_RC32K_DIG_VREF_DLY_POS                              (16U)
#define GLB_RC32K_DIG_VREF_DLY_LEN                              (2U)
#define GLB_RC32K_DIG_VREF_DLY_MSK                              (((1U<<GLB_RC32K_DIG_VREF_DLY_LEN)-1)<<GLB_RC32K_DIG_VREF_DLY_POS)
#define GLB_RC32K_DIG_VREF_DLY_UMSK                             (~(((1U<<GLB_RC32K_DIG_VREF_DLY_LEN)-1)<<GLB_RC32K_DIG_VREF_DLY_POS))
#define GLB_RC32K_ALLOW_CAL                                     GLB_RC32K_ALLOW_CAL
#define GLB_RC32K_ALLOW_CAL_POS                                 (18U)
#define GLB_RC32K_ALLOW_CAL_LEN                                 (1U)
#define GLB_RC32K_ALLOW_CAL_MSK                                 (((1U<<GLB_RC32K_ALLOW_CAL_LEN)-1)<<GLB_RC32K_ALLOW_CAL_POS)
#define GLB_RC32K_ALLOW_CAL_UMSK                                (~(((1U<<GLB_RC32K_ALLOW_CAL_LEN)-1)<<GLB_RC32K_ALLOW_CAL_POS))

/* 0x50C : gpadc_reg_cmd */
#define GLB_GPADC_REG_CMD_OFFSET                                (0x50C)
#define GLB_GPADC_GLOBAL_EN                                     GLB_GPADC_GLOBAL_EN
#define GLB_GPADC_GLOBAL_EN_POS                                 (0U)
#define GLB_GPADC_GLOBAL_EN_LEN                                 (1U)
#define GLB_GPADC_GLOBAL_EN_MSK                                 (((1U<<GLB_GPADC_GLOBAL_EN_LEN)-1)<<GLB_GPADC_GLOBAL_EN_POS)
#define GLB_GPADC_GLOBAL_EN_UMSK                                (~(((1U<<GLB_GPADC_GLOBAL_EN_LEN)-1)<<GLB_GPADC_GLOBAL_EN_POS))
#define GLB_GPADC_CONV_START                                    GLB_GPADC_CONV_START
#define GLB_GPADC_CONV_START_POS                                (1U)
#define GLB_GPADC_CONV_START_LEN                                (1U)
#define GLB_GPADC_CONV_START_MSK                                (((1U<<GLB_GPADC_CONV_START_LEN)-1)<<GLB_GPADC_CONV_START_POS)
#define GLB_GPADC_CONV_START_UMSK                               (~(((1U<<GLB_GPADC_CONV_START_LEN)-1)<<GLB_GPADC_CONV_START_POS))
#define GLB_GPADC_SOFT_RST                                      GLB_GPADC_SOFT_RST
#define GLB_GPADC_SOFT_RST_POS                                  (2U)
#define GLB_GPADC_SOFT_RST_LEN                                  (1U)
#define GLB_GPADC_SOFT_RST_MSK                                  (((1U<<GLB_GPADC_SOFT_RST_LEN)-1)<<GLB_GPADC_SOFT_RST_POS)
#define GLB_GPADC_SOFT_RST_UMSK                                 (~(((1U<<GLB_GPADC_SOFT_RST_LEN)-1)<<GLB_GPADC_SOFT_RST_POS))
#define GLB_GPADC_NEG_SEL                                       GLB_GPADC_NEG_SEL
#define GLB_GPADC_NEG_SEL_POS                                   (3U)
#define GLB_GPADC_NEG_SEL_LEN                                   (5U)
#define GLB_GPADC_NEG_SEL_MSK                                   (((1U<<GLB_GPADC_NEG_SEL_LEN)-1)<<GLB_GPADC_NEG_SEL_POS)
#define GLB_GPADC_NEG_SEL_UMSK                                  (~(((1U<<GLB_GPADC_NEG_SEL_LEN)-1)<<GLB_GPADC_NEG_SEL_POS))
#define GLB_GPADC_POS_SEL                                       GLB_GPADC_POS_SEL
#define GLB_GPADC_POS_SEL_POS                                   (8U)
#define GLB_GPADC_POS_SEL_LEN                                   (5U)
#define GLB_GPADC_POS_SEL_MSK                                   (((1U<<GLB_GPADC_POS_SEL_LEN)-1)<<GLB_GPADC_POS_SEL_POS)
#define GLB_GPADC_POS_SEL_UMSK                                  (~(((1U<<GLB_GPADC_POS_SEL_LEN)-1)<<GLB_GPADC_POS_SEL_POS))
#define GLB_GPADC_NEG_GND                                       GLB_GPADC_NEG_GND
#define GLB_GPADC_NEG_GND_POS                                   (13U)
#define GLB_GPADC_NEG_GND_LEN                                   (1U)
#define GLB_GPADC_NEG_GND_MSK                                   (((1U<<GLB_GPADC_NEG_GND_LEN)-1)<<GLB_GPADC_NEG_GND_POS)
#define GLB_GPADC_NEG_GND_UMSK                                  (~(((1U<<GLB_GPADC_NEG_GND_LEN)-1)<<GLB_GPADC_NEG_GND_POS))
#define GLB_GPADC_MICBIAS_EN                                    GLB_GPADC_MICBIAS_EN
#define GLB_GPADC_MICBIAS_EN_POS                                (14U)
#define GLB_GPADC_MICBIAS_EN_LEN                                (1U)
#define GLB_GPADC_MICBIAS_EN_MSK                                (((1U<<GLB_GPADC_MICBIAS_EN_LEN)-1)<<GLB_GPADC_MICBIAS_EN_POS)
#define GLB_GPADC_MICBIAS_EN_UMSK                               (~(((1U<<GLB_GPADC_MICBIAS_EN_LEN)-1)<<GLB_GPADC_MICBIAS_EN_POS))
#define GLB_GPADC_MICPGA_EN                                     GLB_GPADC_MICPGA_EN
#define GLB_GPADC_MICPGA_EN_POS                                 (15U)
#define GLB_GPADC_MICPGA_EN_LEN                                 (1U)
#define GLB_GPADC_MICPGA_EN_MSK                                 (((1U<<GLB_GPADC_MICPGA_EN_LEN)-1)<<GLB_GPADC_MICPGA_EN_POS)
#define GLB_GPADC_MICPGA_EN_UMSK                                (~(((1U<<GLB_GPADC_MICPGA_EN_LEN)-1)<<GLB_GPADC_MICPGA_EN_POS))
#define GLB_GPADC_BYP_MICBOOST                                  GLB_GPADC_BYP_MICBOOST
#define GLB_GPADC_BYP_MICBOOST_POS                              (16U)
#define GLB_GPADC_BYP_MICBOOST_LEN                              (1U)
#define GLB_GPADC_BYP_MICBOOST_MSK                              (((1U<<GLB_GPADC_BYP_MICBOOST_LEN)-1)<<GLB_GPADC_BYP_MICBOOST_POS)
#define GLB_GPADC_BYP_MICBOOST_UMSK                             (~(((1U<<GLB_GPADC_BYP_MICBOOST_LEN)-1)<<GLB_GPADC_BYP_MICBOOST_POS))
#define GLB_GPADC_RCAL_EN                                       GLB_GPADC_RCAL_EN
#define GLB_GPADC_RCAL_EN_POS                                   (17U)
#define GLB_GPADC_RCAL_EN_LEN                                   (1U)
#define GLB_GPADC_RCAL_EN_MSK                                   (((1U<<GLB_GPADC_RCAL_EN_LEN)-1)<<GLB_GPADC_RCAL_EN_POS)
#define GLB_GPADC_RCAL_EN_UMSK                                  (~(((1U<<GLB_GPADC_RCAL_EN_LEN)-1)<<GLB_GPADC_RCAL_EN_POS))
#define GLB_GPADC_DWA_EN                                        GLB_GPADC_DWA_EN
#define GLB_GPADC_DWA_EN_POS                                    (18U)
#define GLB_GPADC_DWA_EN_LEN                                    (1U)
#define GLB_GPADC_DWA_EN_MSK                                    (((1U<<GLB_GPADC_DWA_EN_LEN)-1)<<GLB_GPADC_DWA_EN_POS)
#define GLB_GPADC_DWA_EN_UMSK                                   (~(((1U<<GLB_GPADC_DWA_EN_LEN)-1)<<GLB_GPADC_DWA_EN_POS))
#define GLB_GPADC_MIC2_DIFF                                     GLB_GPADC_MIC2_DIFF
#define GLB_GPADC_MIC2_DIFF_POS                                 (19U)
#define GLB_GPADC_MIC2_DIFF_LEN                                 (1U)
#define GLB_GPADC_MIC2_DIFF_MSK                                 (((1U<<GLB_GPADC_MIC2_DIFF_LEN)-1)<<GLB_GPADC_MIC2_DIFF_POS)
#define GLB_GPADC_MIC2_DIFF_UMSK                                (~(((1U<<GLB_GPADC_MIC2_DIFF_LEN)-1)<<GLB_GPADC_MIC2_DIFF_POS))
#define GLB_GPADC_MIC1_DIFF                                     GLB_GPADC_MIC1_DIFF
#define GLB_GPADC_MIC1_DIFF_POS                                 (20U)
#define GLB_GPADC_MIC1_DIFF_LEN                                 (1U)
#define GLB_GPADC_MIC1_DIFF_MSK                                 (((1U<<GLB_GPADC_MIC1_DIFF_LEN)-1)<<GLB_GPADC_MIC1_DIFF_POS)
#define GLB_GPADC_MIC1_DIFF_UMSK                                (~(((1U<<GLB_GPADC_MIC1_DIFF_LEN)-1)<<GLB_GPADC_MIC1_DIFF_POS))
#define GLB_GPADC_MIC_PGA2_GAIN                                 GLB_GPADC_MIC_PGA2_GAIN
#define GLB_GPADC_MIC_PGA2_GAIN_POS                             (21U)
#define GLB_GPADC_MIC_PGA2_GAIN_LEN                             (2U)
#define GLB_GPADC_MIC_PGA2_GAIN_MSK                             (((1U<<GLB_GPADC_MIC_PGA2_GAIN_LEN)-1)<<GLB_GPADC_MIC_PGA2_GAIN_POS)
#define GLB_GPADC_MIC_PGA2_GAIN_UMSK                            (~(((1U<<GLB_GPADC_MIC_PGA2_GAIN_LEN)-1)<<GLB_GPADC_MIC_PGA2_GAIN_POS))
#define GLB_GPADC_MICBOOST_32DB_EN                              GLB_GPADC_MICBOOST_32DB_EN
#define GLB_GPADC_MICBOOST_32DB_EN_POS                          (23U)
#define GLB_GPADC_MICBOOST_32DB_EN_LEN                          (1U)
#define GLB_GPADC_MICBOOST_32DB_EN_MSK                          (((1U<<GLB_GPADC_MICBOOST_32DB_EN_LEN)-1)<<GLB_GPADC_MICBOOST_32DB_EN_POS)
#define GLB_GPADC_MICBOOST_32DB_EN_UMSK                         (~(((1U<<GLB_GPADC_MICBOOST_32DB_EN_LEN)-1)<<GLB_GPADC_MICBOOST_32DB_EN_POS))
#define GLB_GPADC_CHIP_SEN_PU                                   GLB_GPADC_CHIP_SEN_PU
#define GLB_GPADC_CHIP_SEN_PU_POS                               (27U)
#define GLB_GPADC_CHIP_SEN_PU_LEN                               (1U)
#define GLB_GPADC_CHIP_SEN_PU_MSK                               (((1U<<GLB_GPADC_CHIP_SEN_PU_LEN)-1)<<GLB_GPADC_CHIP_SEN_PU_POS)
#define GLB_GPADC_CHIP_SEN_PU_UMSK                              (~(((1U<<GLB_GPADC_CHIP_SEN_PU_LEN)-1)<<GLB_GPADC_CHIP_SEN_PU_POS))
#define GLB_GPADC_SEN_SEL                                       GLB_GPADC_SEN_SEL
#define GLB_GPADC_SEN_SEL_POS                                   (28U)
#define GLB_GPADC_SEN_SEL_LEN                                   (3U)
#define GLB_GPADC_SEN_SEL_MSK                                   (((1U<<GLB_GPADC_SEN_SEL_LEN)-1)<<GLB_GPADC_SEN_SEL_POS)
#define GLB_GPADC_SEN_SEL_UMSK                                  (~(((1U<<GLB_GPADC_SEN_SEL_LEN)-1)<<GLB_GPADC_SEN_SEL_POS))
#define GLB_GPADC_SEN_TEST_EN                                   GLB_GPADC_SEN_TEST_EN
#define GLB_GPADC_SEN_TEST_EN_POS                               (31U)
#define GLB_GPADC_SEN_TEST_EN_LEN                               (1U)
#define GLB_GPADC_SEN_TEST_EN_MSK                               (((1U<<GLB_GPADC_SEN_TEST_EN_LEN)-1)<<GLB_GPADC_SEN_TEST_EN_POS)
#define GLB_GPADC_SEN_TEST_EN_UMSK                              (~(((1U<<GLB_GPADC_SEN_TEST_EN_LEN)-1)<<GLB_GPADC_SEN_TEST_EN_POS))

/* 0x510 : gpadc_reg_config1 */
#define GLB_GPADC_REG_CONFIG1_OFFSET                            (0x510)
#define GLB_GPADC_CONT_CONV_EN                                  GLB_GPADC_CONT_CONV_EN
#define GLB_GPADC_CONT_CONV_EN_POS                              (1U)
#define GLB_GPADC_CONT_CONV_EN_LEN                              (1U)
#define GLB_GPADC_CONT_CONV_EN_MSK                              (((1U<<GLB_GPADC_CONT_CONV_EN_LEN)-1)<<GLB_GPADC_CONT_CONV_EN_POS)
#define GLB_GPADC_CONT_CONV_EN_UMSK                             (~(((1U<<GLB_GPADC_CONT_CONV_EN_LEN)-1)<<GLB_GPADC_CONT_CONV_EN_POS))
#define GLB_GPADC_RES_SEL                                       GLB_GPADC_RES_SEL
#define GLB_GPADC_RES_SEL_POS                                   (2U)
#define GLB_GPADC_RES_SEL_LEN                                   (3U)
#define GLB_GPADC_RES_SEL_MSK                                   (((1U<<GLB_GPADC_RES_SEL_LEN)-1)<<GLB_GPADC_RES_SEL_POS)
#define GLB_GPADC_RES_SEL_UMSK                                  (~(((1U<<GLB_GPADC_RES_SEL_LEN)-1)<<GLB_GPADC_RES_SEL_POS))
#define GLB_GPADC_VCM_SEL_EN                                    GLB_GPADC_VCM_SEL_EN
#define GLB_GPADC_VCM_SEL_EN_POS                                (8U)
#define GLB_GPADC_VCM_SEL_EN_LEN                                (1U)
#define GLB_GPADC_VCM_SEL_EN_MSK                                (((1U<<GLB_GPADC_VCM_SEL_EN_LEN)-1)<<GLB_GPADC_VCM_SEL_EN_POS)
#define GLB_GPADC_VCM_SEL_EN_UMSK                               (~(((1U<<GLB_GPADC_VCM_SEL_EN_LEN)-1)<<GLB_GPADC_VCM_SEL_EN_POS))
#define GLB_GPADC_VCM_HYST_SEL                                  GLB_GPADC_VCM_HYST_SEL
#define GLB_GPADC_VCM_HYST_SEL_POS                              (9U)
#define GLB_GPADC_VCM_HYST_SEL_LEN                              (1U)
#define GLB_GPADC_VCM_HYST_SEL_MSK                              (((1U<<GLB_GPADC_VCM_HYST_SEL_LEN)-1)<<GLB_GPADC_VCM_HYST_SEL_POS)
#define GLB_GPADC_VCM_HYST_SEL_UMSK                             (~(((1U<<GLB_GPADC_VCM_HYST_SEL_LEN)-1)<<GLB_GPADC_VCM_HYST_SEL_POS))
#define GLB_GPADC_LOWV_DET_EN                                   GLB_GPADC_LOWV_DET_EN
#define GLB_GPADC_LOWV_DET_EN_POS                               (10U)
#define GLB_GPADC_LOWV_DET_EN_LEN                               (1U)
#define GLB_GPADC_LOWV_DET_EN_MSK                               (((1U<<GLB_GPADC_LOWV_DET_EN_LEN)-1)<<GLB_GPADC_LOWV_DET_EN_POS)
#define GLB_GPADC_LOWV_DET_EN_UMSK                              (~(((1U<<GLB_GPADC_LOWV_DET_EN_LEN)-1)<<GLB_GPADC_LOWV_DET_EN_POS))
#define GLB_GPADC_PWM_TRG_EN                                    GLB_GPADC_PWM_TRG_EN
#define GLB_GPADC_PWM_TRG_EN_POS                                (11U)
#define GLB_GPADC_PWM_TRG_EN_LEN                                (1U)
#define GLB_GPADC_PWM_TRG_EN_MSK                                (((1U<<GLB_GPADC_PWM_TRG_EN_LEN)-1)<<GLB_GPADC_PWM_TRG_EN_POS)
#define GLB_GPADC_PWM_TRG_EN_UMSK                               (~(((1U<<GLB_GPADC_PWM_TRG_EN_LEN)-1)<<GLB_GPADC_PWM_TRG_EN_POS))
#define GLB_GPADC_CLK_ANA_DLY                                   GLB_GPADC_CLK_ANA_DLY
#define GLB_GPADC_CLK_ANA_DLY_POS                               (12U)
#define GLB_GPADC_CLK_ANA_DLY_LEN                               (4U)
#define GLB_GPADC_CLK_ANA_DLY_MSK                               (((1U<<GLB_GPADC_CLK_ANA_DLY_LEN)-1)<<GLB_GPADC_CLK_ANA_DLY_POS)
#define GLB_GPADC_CLK_ANA_DLY_UMSK                              (~(((1U<<GLB_GPADC_CLK_ANA_DLY_LEN)-1)<<GLB_GPADC_CLK_ANA_DLY_POS))
#define GLB_GPADC_CLK_ANA_DLY_EN                                GLB_GPADC_CLK_ANA_DLY_EN
#define GLB_GPADC_CLK_ANA_DLY_EN_POS                            (16U)
#define GLB_GPADC_CLK_ANA_DLY_EN_LEN                            (1U)
#define GLB_GPADC_CLK_ANA_DLY_EN_MSK                            (((1U<<GLB_GPADC_CLK_ANA_DLY_EN_LEN)-1)<<GLB_GPADC_CLK_ANA_DLY_EN_POS)
#define GLB_GPADC_CLK_ANA_DLY_EN_UMSK                           (~(((1U<<GLB_GPADC_CLK_ANA_DLY_EN_LEN)-1)<<GLB_GPADC_CLK_ANA_DLY_EN_POS))
#define GLB_GPADC_CLK_ANA_INV                                   GLB_GPADC_CLK_ANA_INV
#define GLB_GPADC_CLK_ANA_INV_POS                               (17U)
#define GLB_GPADC_CLK_ANA_INV_LEN                               (1U)
#define GLB_GPADC_CLK_ANA_INV_MSK                               (((1U<<GLB_GPADC_CLK_ANA_INV_LEN)-1)<<GLB_GPADC_CLK_ANA_INV_POS)
#define GLB_GPADC_CLK_ANA_INV_UMSK                              (~(((1U<<GLB_GPADC_CLK_ANA_INV_LEN)-1)<<GLB_GPADC_CLK_ANA_INV_POS))
#define GLB_GPADC_CLK_DIV_RATIO                                 GLB_GPADC_CLK_DIV_RATIO
#define GLB_GPADC_CLK_DIV_RATIO_POS                             (18U)
#define GLB_GPADC_CLK_DIV_RATIO_LEN                             (3U)
#define GLB_GPADC_CLK_DIV_RATIO_MSK                             (((1U<<GLB_GPADC_CLK_DIV_RATIO_LEN)-1)<<GLB_GPADC_CLK_DIV_RATIO_POS)
#define GLB_GPADC_CLK_DIV_RATIO_UMSK                            (~(((1U<<GLB_GPADC_CLK_DIV_RATIO_LEN)-1)<<GLB_GPADC_CLK_DIV_RATIO_POS))
#define GLB_GPADC_SCAN_LENGTH                                   GLB_GPADC_SCAN_LENGTH
#define GLB_GPADC_SCAN_LENGTH_POS                               (21U)
#define GLB_GPADC_SCAN_LENGTH_LEN                               (4U)
#define GLB_GPADC_SCAN_LENGTH_MSK                               (((1U<<GLB_GPADC_SCAN_LENGTH_LEN)-1)<<GLB_GPADC_SCAN_LENGTH_POS)
#define GLB_GPADC_SCAN_LENGTH_UMSK                              (~(((1U<<GLB_GPADC_SCAN_LENGTH_LEN)-1)<<GLB_GPADC_SCAN_LENGTH_POS))
#define GLB_GPADC_SCAN_EN                                       GLB_GPADC_SCAN_EN
#define GLB_GPADC_SCAN_EN_POS                                   (25U)
#define GLB_GPADC_SCAN_EN_LEN                                   (1U)
#define GLB_GPADC_SCAN_EN_MSK                                   (((1U<<GLB_GPADC_SCAN_EN_LEN)-1)<<GLB_GPADC_SCAN_EN_POS)
#define GLB_GPADC_SCAN_EN_UMSK                                  (~(((1U<<GLB_GPADC_SCAN_EN_LEN)-1)<<GLB_GPADC_SCAN_EN_POS))
#define GLB_GPADC_DITHER_EN                                     GLB_GPADC_DITHER_EN
#define GLB_GPADC_DITHER_EN_POS                                 (26U)
#define GLB_GPADC_DITHER_EN_LEN                                 (1U)
#define GLB_GPADC_DITHER_EN_MSK                                 (((1U<<GLB_GPADC_DITHER_EN_LEN)-1)<<GLB_GPADC_DITHER_EN_POS)
#define GLB_GPADC_DITHER_EN_UMSK                                (~(((1U<<GLB_GPADC_DITHER_EN_LEN)-1)<<GLB_GPADC_DITHER_EN_POS))
#define GLB_GPADC_V11_SEL                                       GLB_GPADC_V11_SEL
#define GLB_GPADC_V11_SEL_POS                                   (27U)
#define GLB_GPADC_V11_SEL_LEN                                   (2U)
#define GLB_GPADC_V11_SEL_MSK                                   (((1U<<GLB_GPADC_V11_SEL_LEN)-1)<<GLB_GPADC_V11_SEL_POS)
#define GLB_GPADC_V11_SEL_UMSK                                  (~(((1U<<GLB_GPADC_V11_SEL_LEN)-1)<<GLB_GPADC_V11_SEL_POS))
#define GLB_GPADC_V18_SEL                                       GLB_GPADC_V18_SEL
#define GLB_GPADC_V18_SEL_POS                                   (29U)
#define GLB_GPADC_V18_SEL_LEN                                   (2U)
#define GLB_GPADC_V18_SEL_MSK                                   (((1U<<GLB_GPADC_V18_SEL_LEN)-1)<<GLB_GPADC_V18_SEL_POS)
#define GLB_GPADC_V18_SEL_UMSK                                  (~(((1U<<GLB_GPADC_V18_SEL_LEN)-1)<<GLB_GPADC_V18_SEL_POS))

/* 0x514 : gpadc_reg_config2 */
#define GLB_GPADC_REG_CONFIG2_OFFSET                            (0x514)
#define GLB_GPADC_DIFF_MODE                                     GLB_GPADC_DIFF_MODE
#define GLB_GPADC_DIFF_MODE_POS                                 (2U)
#define GLB_GPADC_DIFF_MODE_LEN                                 (1U)
#define GLB_GPADC_DIFF_MODE_MSK                                 (((1U<<GLB_GPADC_DIFF_MODE_LEN)-1)<<GLB_GPADC_DIFF_MODE_POS)
#define GLB_GPADC_DIFF_MODE_UMSK                                (~(((1U<<GLB_GPADC_DIFF_MODE_LEN)-1)<<GLB_GPADC_DIFF_MODE_POS))
#define GLB_GPADC_VREF_SEL                                      GLB_GPADC_VREF_SEL
#define GLB_GPADC_VREF_SEL_POS                                  (3U)
#define GLB_GPADC_VREF_SEL_LEN                                  (1U)
#define GLB_GPADC_VREF_SEL_MSK                                  (((1U<<GLB_GPADC_VREF_SEL_LEN)-1)<<GLB_GPADC_VREF_SEL_POS)
#define GLB_GPADC_VREF_SEL_UMSK                                 (~(((1U<<GLB_GPADC_VREF_SEL_LEN)-1)<<GLB_GPADC_VREF_SEL_POS))
#define GLB_GPADC_VBAT_EN                                       GLB_GPADC_VBAT_EN
#define GLB_GPADC_VBAT_EN_POS                                   (4U)
#define GLB_GPADC_VBAT_EN_LEN                                   (1U)
#define GLB_GPADC_VBAT_EN_MSK                                   (((1U<<GLB_GPADC_VBAT_EN_LEN)-1)<<GLB_GPADC_VBAT_EN_POS)
#define GLB_GPADC_VBAT_EN_UMSK                                  (~(((1U<<GLB_GPADC_VBAT_EN_LEN)-1)<<GLB_GPADC_VBAT_EN_POS))
#define GLB_GPADC_TSEXT_SEL                                     GLB_GPADC_TSEXT_SEL
#define GLB_GPADC_TSEXT_SEL_POS                                 (5U)
#define GLB_GPADC_TSEXT_SEL_LEN                                 (1U)
#define GLB_GPADC_TSEXT_SEL_MSK                                 (((1U<<GLB_GPADC_TSEXT_SEL_LEN)-1)<<GLB_GPADC_TSEXT_SEL_POS)
#define GLB_GPADC_TSEXT_SEL_UMSK                                (~(((1U<<GLB_GPADC_TSEXT_SEL_LEN)-1)<<GLB_GPADC_TSEXT_SEL_POS))
#define GLB_GPADC_TS_EN                                         GLB_GPADC_TS_EN
#define GLB_GPADC_TS_EN_POS                                     (6U)
#define GLB_GPADC_TS_EN_LEN                                     (1U)
#define GLB_GPADC_TS_EN_MSK                                     (((1U<<GLB_GPADC_TS_EN_LEN)-1)<<GLB_GPADC_TS_EN_POS)
#define GLB_GPADC_TS_EN_UMSK                                    (~(((1U<<GLB_GPADC_TS_EN_LEN)-1)<<GLB_GPADC_TS_EN_POS))
#define GLB_GPADC_PGA_VCM                                       GLB_GPADC_PGA_VCM
#define GLB_GPADC_PGA_VCM_POS                                   (7U)
#define GLB_GPADC_PGA_VCM_LEN                                   (2U)
#define GLB_GPADC_PGA_VCM_MSK                                   (((1U<<GLB_GPADC_PGA_VCM_LEN)-1)<<GLB_GPADC_PGA_VCM_POS)
#define GLB_GPADC_PGA_VCM_UMSK                                  (~(((1U<<GLB_GPADC_PGA_VCM_LEN)-1)<<GLB_GPADC_PGA_VCM_POS))
#define GLB_GPADC_PGA_OS_CAL                                    GLB_GPADC_PGA_OS_CAL
#define GLB_GPADC_PGA_OS_CAL_POS                                (9U)
#define GLB_GPADC_PGA_OS_CAL_LEN                                (4U)
#define GLB_GPADC_PGA_OS_CAL_MSK                                (((1U<<GLB_GPADC_PGA_OS_CAL_LEN)-1)<<GLB_GPADC_PGA_OS_CAL_POS)
#define GLB_GPADC_PGA_OS_CAL_UMSK                               (~(((1U<<GLB_GPADC_PGA_OS_CAL_LEN)-1)<<GLB_GPADC_PGA_OS_CAL_POS))
#define GLB_GPADC_PGA_EN                                        GLB_GPADC_PGA_EN
#define GLB_GPADC_PGA_EN_POS                                    (13U)
#define GLB_GPADC_PGA_EN_LEN                                    (1U)
#define GLB_GPADC_PGA_EN_MSK                                    (((1U<<GLB_GPADC_PGA_EN_LEN)-1)<<GLB_GPADC_PGA_EN_POS)
#define GLB_GPADC_PGA_EN_UMSK                                   (~(((1U<<GLB_GPADC_PGA_EN_LEN)-1)<<GLB_GPADC_PGA_EN_POS))
#define GLB_GPADC_PGA_VCMI_EN                                   GLB_GPADC_PGA_VCMI_EN
#define GLB_GPADC_PGA_VCMI_EN_POS                               (14U)
#define GLB_GPADC_PGA_VCMI_EN_LEN                               (1U)
#define GLB_GPADC_PGA_VCMI_EN_MSK                               (((1U<<GLB_GPADC_PGA_VCMI_EN_LEN)-1)<<GLB_GPADC_PGA_VCMI_EN_POS)
#define GLB_GPADC_PGA_VCMI_EN_UMSK                              (~(((1U<<GLB_GPADC_PGA_VCMI_EN_LEN)-1)<<GLB_GPADC_PGA_VCMI_EN_POS))
#define GLB_GPADC_CHOP_MODE                                     GLB_GPADC_CHOP_MODE
#define GLB_GPADC_CHOP_MODE_POS                                 (15U)
#define GLB_GPADC_CHOP_MODE_LEN                                 (2U)
#define GLB_GPADC_CHOP_MODE_MSK                                 (((1U<<GLB_GPADC_CHOP_MODE_LEN)-1)<<GLB_GPADC_CHOP_MODE_POS)
#define GLB_GPADC_CHOP_MODE_UMSK                                (~(((1U<<GLB_GPADC_CHOP_MODE_LEN)-1)<<GLB_GPADC_CHOP_MODE_POS))
#define GLB_GPADC_BIAS_SEL                                      GLB_GPADC_BIAS_SEL
#define GLB_GPADC_BIAS_SEL_POS                                  (17U)
#define GLB_GPADC_BIAS_SEL_LEN                                  (1U)
#define GLB_GPADC_BIAS_SEL_MSK                                  (((1U<<GLB_GPADC_BIAS_SEL_LEN)-1)<<GLB_GPADC_BIAS_SEL_POS)
#define GLB_GPADC_BIAS_SEL_UMSK                                 (~(((1U<<GLB_GPADC_BIAS_SEL_LEN)-1)<<GLB_GPADC_BIAS_SEL_POS))
#define GLB_GPADC_TEST_EN                                       GLB_GPADC_TEST_EN
#define GLB_GPADC_TEST_EN_POS                                   (18U)
#define GLB_GPADC_TEST_EN_LEN                                   (1U)
#define GLB_GPADC_TEST_EN_MSK                                   (((1U<<GLB_GPADC_TEST_EN_LEN)-1)<<GLB_GPADC_TEST_EN_POS)
#define GLB_GPADC_TEST_EN_UMSK                                  (~(((1U<<GLB_GPADC_TEST_EN_LEN)-1)<<GLB_GPADC_TEST_EN_POS))
#define GLB_GPADC_TEST_SEL                                      GLB_GPADC_TEST_SEL
#define GLB_GPADC_TEST_SEL_POS                                  (19U)
#define GLB_GPADC_TEST_SEL_LEN                                  (3U)
#define GLB_GPADC_TEST_SEL_MSK                                  (((1U<<GLB_GPADC_TEST_SEL_LEN)-1)<<GLB_GPADC_TEST_SEL_POS)
#define GLB_GPADC_TEST_SEL_UMSK                                 (~(((1U<<GLB_GPADC_TEST_SEL_LEN)-1)<<GLB_GPADC_TEST_SEL_POS))
#define GLB_GPADC_PGA2_GAIN                                     GLB_GPADC_PGA2_GAIN
#define GLB_GPADC_PGA2_GAIN_POS                                 (22U)
#define GLB_GPADC_PGA2_GAIN_LEN                                 (3U)
#define GLB_GPADC_PGA2_GAIN_MSK                                 (((1U<<GLB_GPADC_PGA2_GAIN_LEN)-1)<<GLB_GPADC_PGA2_GAIN_POS)
#define GLB_GPADC_PGA2_GAIN_UMSK                                (~(((1U<<GLB_GPADC_PGA2_GAIN_LEN)-1)<<GLB_GPADC_PGA2_GAIN_POS))
#define GLB_GPADC_PGA1_GAIN                                     GLB_GPADC_PGA1_GAIN
#define GLB_GPADC_PGA1_GAIN_POS                                 (25U)
#define GLB_GPADC_PGA1_GAIN_LEN                                 (3U)
#define GLB_GPADC_PGA1_GAIN_MSK                                 (((1U<<GLB_GPADC_PGA1_GAIN_LEN)-1)<<GLB_GPADC_PGA1_GAIN_POS)
#define GLB_GPADC_PGA1_GAIN_UMSK                                (~(((1U<<GLB_GPADC_PGA1_GAIN_LEN)-1)<<GLB_GPADC_PGA1_GAIN_POS))
#define GLB_GPADC_DLY_SEL                                       GLB_GPADC_DLY_SEL
#define GLB_GPADC_DLY_SEL_POS                                   (28U)
#define GLB_GPADC_DLY_SEL_LEN                                   (3U)
#define GLB_GPADC_DLY_SEL_MSK                                   (((1U<<GLB_GPADC_DLY_SEL_LEN)-1)<<GLB_GPADC_DLY_SEL_POS)
#define GLB_GPADC_DLY_SEL_UMSK                                  (~(((1U<<GLB_GPADC_DLY_SEL_LEN)-1)<<GLB_GPADC_DLY_SEL_POS))
#define GLB_GPADC_TSVBE_LOW                                     GLB_GPADC_TSVBE_LOW
#define GLB_GPADC_TSVBE_LOW_POS                                 (31U)
#define GLB_GPADC_TSVBE_LOW_LEN                                 (1U)
#define GLB_GPADC_TSVBE_LOW_MSK                                 (((1U<<GLB_GPADC_TSVBE_LOW_LEN)-1)<<GLB_GPADC_TSVBE_LOW_POS)
#define GLB_GPADC_TSVBE_LOW_UMSK                                (~(((1U<<GLB_GPADC_TSVBE_LOW_LEN)-1)<<GLB_GPADC_TSVBE_LOW_POS))

/* 0x518 : adc converation sequence 1 */
#define GLB_GPADC_REG_SCN_POS1_OFFSET                           (0x518)
#define GLB_GPADC_SCAN_POS_0                                    GLB_GPADC_SCAN_POS_0
#define GLB_GPADC_SCAN_POS_0_POS                                (0U)
#define GLB_GPADC_SCAN_POS_0_LEN                                (5U)
#define GLB_GPADC_SCAN_POS_0_MSK                                (((1U<<GLB_GPADC_SCAN_POS_0_LEN)-1)<<GLB_GPADC_SCAN_POS_0_POS)
#define GLB_GPADC_SCAN_POS_0_UMSK                               (~(((1U<<GLB_GPADC_SCAN_POS_0_LEN)-1)<<GLB_GPADC_SCAN_POS_0_POS))
#define GLB_GPADC_SCAN_POS_1                                    GLB_GPADC_SCAN_POS_1
#define GLB_GPADC_SCAN_POS_1_POS                                (5U)
#define GLB_GPADC_SCAN_POS_1_LEN                                (5U)
#define GLB_GPADC_SCAN_POS_1_MSK                                (((1U<<GLB_GPADC_SCAN_POS_1_LEN)-1)<<GLB_GPADC_SCAN_POS_1_POS)
#define GLB_GPADC_SCAN_POS_1_UMSK                               (~(((1U<<GLB_GPADC_SCAN_POS_1_LEN)-1)<<GLB_GPADC_SCAN_POS_1_POS))
#define GLB_GPADC_SCAN_POS_2                                    GLB_GPADC_SCAN_POS_2
#define GLB_GPADC_SCAN_POS_2_POS                                (10U)
#define GLB_GPADC_SCAN_POS_2_LEN                                (5U)
#define GLB_GPADC_SCAN_POS_2_MSK                                (((1U<<GLB_GPADC_SCAN_POS_2_LEN)-1)<<GLB_GPADC_SCAN_POS_2_POS)
#define GLB_GPADC_SCAN_POS_2_UMSK                               (~(((1U<<GLB_GPADC_SCAN_POS_2_LEN)-1)<<GLB_GPADC_SCAN_POS_2_POS))
#define GLB_GPADC_SCAN_POS_3                                    GLB_GPADC_SCAN_POS_3
#define GLB_GPADC_SCAN_POS_3_POS                                (15U)
#define GLB_GPADC_SCAN_POS_3_LEN                                (5U)
#define GLB_GPADC_SCAN_POS_3_MSK                                (((1U<<GLB_GPADC_SCAN_POS_3_LEN)-1)<<GLB_GPADC_SCAN_POS_3_POS)
#define GLB_GPADC_SCAN_POS_3_UMSK                               (~(((1U<<GLB_GPADC_SCAN_POS_3_LEN)-1)<<GLB_GPADC_SCAN_POS_3_POS))
#define GLB_GPADC_SCAN_POS_4                                    GLB_GPADC_SCAN_POS_4
#define GLB_GPADC_SCAN_POS_4_POS                                (20U)
#define GLB_GPADC_SCAN_POS_4_LEN                                (5U)
#define GLB_GPADC_SCAN_POS_4_MSK                                (((1U<<GLB_GPADC_SCAN_POS_4_LEN)-1)<<GLB_GPADC_SCAN_POS_4_POS)
#define GLB_GPADC_SCAN_POS_4_UMSK                               (~(((1U<<GLB_GPADC_SCAN_POS_4_LEN)-1)<<GLB_GPADC_SCAN_POS_4_POS))
#define GLB_GPADC_SCAN_POS_5                                    GLB_GPADC_SCAN_POS_5
#define GLB_GPADC_SCAN_POS_5_POS                                (25U)
#define GLB_GPADC_SCAN_POS_5_LEN                                (5U)
#define GLB_GPADC_SCAN_POS_5_MSK                                (((1U<<GLB_GPADC_SCAN_POS_5_LEN)-1)<<GLB_GPADC_SCAN_POS_5_POS)
#define GLB_GPADC_SCAN_POS_5_UMSK                               (~(((1U<<GLB_GPADC_SCAN_POS_5_LEN)-1)<<GLB_GPADC_SCAN_POS_5_POS))

/* 0x51C : adc converation sequence 2 */
#define GLB_GPADC_REG_SCN_POS2_OFFSET                           (0x51C)
#define GLB_GPADC_SCAN_POS_6                                    GLB_GPADC_SCAN_POS_6
#define GLB_GPADC_SCAN_POS_6_POS                                (0U)
#define GLB_GPADC_SCAN_POS_6_LEN                                (5U)
#define GLB_GPADC_SCAN_POS_6_MSK                                (((1U<<GLB_GPADC_SCAN_POS_6_LEN)-1)<<GLB_GPADC_SCAN_POS_6_POS)
#define GLB_GPADC_SCAN_POS_6_UMSK                               (~(((1U<<GLB_GPADC_SCAN_POS_6_LEN)-1)<<GLB_GPADC_SCAN_POS_6_POS))
#define GLB_GPADC_SCAN_POS_7                                    GLB_GPADC_SCAN_POS_7
#define GLB_GPADC_SCAN_POS_7_POS                                (5U)
#define GLB_GPADC_SCAN_POS_7_LEN                                (5U)
#define GLB_GPADC_SCAN_POS_7_MSK                                (((1U<<GLB_GPADC_SCAN_POS_7_LEN)-1)<<GLB_GPADC_SCAN_POS_7_POS)
#define GLB_GPADC_SCAN_POS_7_UMSK                               (~(((1U<<GLB_GPADC_SCAN_POS_7_LEN)-1)<<GLB_GPADC_SCAN_POS_7_POS))
#define GLB_GPADC_SCAN_POS_8                                    GLB_GPADC_SCAN_POS_8
#define GLB_GPADC_SCAN_POS_8_POS                                (10U)
#define GLB_GPADC_SCAN_POS_8_LEN                                (5U)
#define GLB_GPADC_SCAN_POS_8_MSK                                (((1U<<GLB_GPADC_SCAN_POS_8_LEN)-1)<<GLB_GPADC_SCAN_POS_8_POS)
#define GLB_GPADC_SCAN_POS_8_UMSK                               (~(((1U<<GLB_GPADC_SCAN_POS_8_LEN)-1)<<GLB_GPADC_SCAN_POS_8_POS))
#define GLB_GPADC_SCAN_POS_9                                    GLB_GPADC_SCAN_POS_9
#define GLB_GPADC_SCAN_POS_9_POS                                (15U)
#define GLB_GPADC_SCAN_POS_9_LEN                                (5U)
#define GLB_GPADC_SCAN_POS_9_MSK                                (((1U<<GLB_GPADC_SCAN_POS_9_LEN)-1)<<GLB_GPADC_SCAN_POS_9_POS)
#define GLB_GPADC_SCAN_POS_9_UMSK                               (~(((1U<<GLB_GPADC_SCAN_POS_9_LEN)-1)<<GLB_GPADC_SCAN_POS_9_POS))
#define GLB_GPADC_SCAN_POS_10                                   GLB_GPADC_SCAN_POS_10
#define GLB_GPADC_SCAN_POS_10_POS                               (20U)
#define GLB_GPADC_SCAN_POS_10_LEN                               (5U)
#define GLB_GPADC_SCAN_POS_10_MSK                               (((1U<<GLB_GPADC_SCAN_POS_10_LEN)-1)<<GLB_GPADC_SCAN_POS_10_POS)
#define GLB_GPADC_SCAN_POS_10_UMSK                              (~(((1U<<GLB_GPADC_SCAN_POS_10_LEN)-1)<<GLB_GPADC_SCAN_POS_10_POS))
#define GLB_GPADC_SCAN_POS_11                                   GLB_GPADC_SCAN_POS_11
#define GLB_GPADC_SCAN_POS_11_POS                               (25U)
#define GLB_GPADC_SCAN_POS_11_LEN                               (5U)
#define GLB_GPADC_SCAN_POS_11_MSK                               (((1U<<GLB_GPADC_SCAN_POS_11_LEN)-1)<<GLB_GPADC_SCAN_POS_11_POS)
#define GLB_GPADC_SCAN_POS_11_UMSK                              (~(((1U<<GLB_GPADC_SCAN_POS_11_LEN)-1)<<GLB_GPADC_SCAN_POS_11_POS))

/* 0x520 : adc converation sequence 3 */
#define GLB_GPADC_REG_SCN_NEG1_OFFSET                           (0x520)
#define GLB_GPADC_SCAN_NEG_0                                    GLB_GPADC_SCAN_NEG_0
#define GLB_GPADC_SCAN_NEG_0_POS                                (0U)
#define GLB_GPADC_SCAN_NEG_0_LEN                                (5U)
#define GLB_GPADC_SCAN_NEG_0_MSK                                (((1U<<GLB_GPADC_SCAN_NEG_0_LEN)-1)<<GLB_GPADC_SCAN_NEG_0_POS)
#define GLB_GPADC_SCAN_NEG_0_UMSK                               (~(((1U<<GLB_GPADC_SCAN_NEG_0_LEN)-1)<<GLB_GPADC_SCAN_NEG_0_POS))
#define GLB_GPADC_SCAN_NEG_1                                    GLB_GPADC_SCAN_NEG_1
#define GLB_GPADC_SCAN_NEG_1_POS                                (5U)
#define GLB_GPADC_SCAN_NEG_1_LEN                                (5U)
#define GLB_GPADC_SCAN_NEG_1_MSK                                (((1U<<GLB_GPADC_SCAN_NEG_1_LEN)-1)<<GLB_GPADC_SCAN_NEG_1_POS)
#define GLB_GPADC_SCAN_NEG_1_UMSK                               (~(((1U<<GLB_GPADC_SCAN_NEG_1_LEN)-1)<<GLB_GPADC_SCAN_NEG_1_POS))
#define GLB_GPADC_SCAN_NEG_2                                    GLB_GPADC_SCAN_NEG_2
#define GLB_GPADC_SCAN_NEG_2_POS                                (10U)
#define GLB_GPADC_SCAN_NEG_2_LEN                                (5U)
#define GLB_GPADC_SCAN_NEG_2_MSK                                (((1U<<GLB_GPADC_SCAN_NEG_2_LEN)-1)<<GLB_GPADC_SCAN_NEG_2_POS)
#define GLB_GPADC_SCAN_NEG_2_UMSK                               (~(((1U<<GLB_GPADC_SCAN_NEG_2_LEN)-1)<<GLB_GPADC_SCAN_NEG_2_POS))
#define GLB_GPADC_SCAN_NEG_3                                    GLB_GPADC_SCAN_NEG_3
#define GLB_GPADC_SCAN_NEG_3_POS                                (15U)
#define GLB_GPADC_SCAN_NEG_3_LEN                                (5U)
#define GLB_GPADC_SCAN_NEG_3_MSK                                (((1U<<GLB_GPADC_SCAN_NEG_3_LEN)-1)<<GLB_GPADC_SCAN_NEG_3_POS)
#define GLB_GPADC_SCAN_NEG_3_UMSK                               (~(((1U<<GLB_GPADC_SCAN_NEG_3_LEN)-1)<<GLB_GPADC_SCAN_NEG_3_POS))
#define GLB_GPADC_SCAN_NEG_4                                    GLB_GPADC_SCAN_NEG_4
#define GLB_GPADC_SCAN_NEG_4_POS                                (20U)
#define GLB_GPADC_SCAN_NEG_4_LEN                                (5U)
#define GLB_GPADC_SCAN_NEG_4_MSK                                (((1U<<GLB_GPADC_SCAN_NEG_4_LEN)-1)<<GLB_GPADC_SCAN_NEG_4_POS)
#define GLB_GPADC_SCAN_NEG_4_UMSK                               (~(((1U<<GLB_GPADC_SCAN_NEG_4_LEN)-1)<<GLB_GPADC_SCAN_NEG_4_POS))
#define GLB_GPADC_SCAN_NEG_5                                    GLB_GPADC_SCAN_NEG_5
#define GLB_GPADC_SCAN_NEG_5_POS                                (25U)
#define GLB_GPADC_SCAN_NEG_5_LEN                                (5U)
#define GLB_GPADC_SCAN_NEG_5_MSK                                (((1U<<GLB_GPADC_SCAN_NEG_5_LEN)-1)<<GLB_GPADC_SCAN_NEG_5_POS)
#define GLB_GPADC_SCAN_NEG_5_UMSK                               (~(((1U<<GLB_GPADC_SCAN_NEG_5_LEN)-1)<<GLB_GPADC_SCAN_NEG_5_POS))

/* 0x524 : adc converation sequence 4 */
#define GLB_GPADC_REG_SCN_NEG2_OFFSET                           (0x524)
#define GLB_GPADC_SCAN_NEG_6                                    GLB_GPADC_SCAN_NEG_6
#define GLB_GPADC_SCAN_NEG_6_POS                                (0U)
#define GLB_GPADC_SCAN_NEG_6_LEN                                (5U)
#define GLB_GPADC_SCAN_NEG_6_MSK                                (((1U<<GLB_GPADC_SCAN_NEG_6_LEN)-1)<<GLB_GPADC_SCAN_NEG_6_POS)
#define GLB_GPADC_SCAN_NEG_6_UMSK                               (~(((1U<<GLB_GPADC_SCAN_NEG_6_LEN)-1)<<GLB_GPADC_SCAN_NEG_6_POS))
#define GLB_GPADC_SCAN_NEG_7                                    GLB_GPADC_SCAN_NEG_7
#define GLB_GPADC_SCAN_NEG_7_POS                                (5U)
#define GLB_GPADC_SCAN_NEG_7_LEN                                (5U)
#define GLB_GPADC_SCAN_NEG_7_MSK                                (((1U<<GLB_GPADC_SCAN_NEG_7_LEN)-1)<<GLB_GPADC_SCAN_NEG_7_POS)
#define GLB_GPADC_SCAN_NEG_7_UMSK                               (~(((1U<<GLB_GPADC_SCAN_NEG_7_LEN)-1)<<GLB_GPADC_SCAN_NEG_7_POS))
#define GLB_GPADC_SCAN_NEG_8                                    GLB_GPADC_SCAN_NEG_8
#define GLB_GPADC_SCAN_NEG_8_POS                                (10U)
#define GLB_GPADC_SCAN_NEG_8_LEN                                (5U)
#define GLB_GPADC_SCAN_NEG_8_MSK                                (((1U<<GLB_GPADC_SCAN_NEG_8_LEN)-1)<<GLB_GPADC_SCAN_NEG_8_POS)
#define GLB_GPADC_SCAN_NEG_8_UMSK                               (~(((1U<<GLB_GPADC_SCAN_NEG_8_LEN)-1)<<GLB_GPADC_SCAN_NEG_8_POS))
#define GLB_GPADC_SCAN_NEG_9                                    GLB_GPADC_SCAN_NEG_9
#define GLB_GPADC_SCAN_NEG_9_POS                                (15U)
#define GLB_GPADC_SCAN_NEG_9_LEN                                (5U)
#define GLB_GPADC_SCAN_NEG_9_MSK                                (((1U<<GLB_GPADC_SCAN_NEG_9_LEN)-1)<<GLB_GPADC_SCAN_NEG_9_POS)
#define GLB_GPADC_SCAN_NEG_9_UMSK                               (~(((1U<<GLB_GPADC_SCAN_NEG_9_LEN)-1)<<GLB_GPADC_SCAN_NEG_9_POS))
#define GLB_GPADC_SCAN_NEG_10                                   GLB_GPADC_SCAN_NEG_10
#define GLB_GPADC_SCAN_NEG_10_POS                               (20U)
#define GLB_GPADC_SCAN_NEG_10_LEN                               (5U)
#define GLB_GPADC_SCAN_NEG_10_MSK                               (((1U<<GLB_GPADC_SCAN_NEG_10_LEN)-1)<<GLB_GPADC_SCAN_NEG_10_POS)
#define GLB_GPADC_SCAN_NEG_10_UMSK                              (~(((1U<<GLB_GPADC_SCAN_NEG_10_LEN)-1)<<GLB_GPADC_SCAN_NEG_10_POS))
#define GLB_GPADC_SCAN_NEG_11                                   GLB_GPADC_SCAN_NEG_11
#define GLB_GPADC_SCAN_NEG_11_POS                               (25U)
#define GLB_GPADC_SCAN_NEG_11_LEN                               (5U)
#define GLB_GPADC_SCAN_NEG_11_MSK                               (((1U<<GLB_GPADC_SCAN_NEG_11_LEN)-1)<<GLB_GPADC_SCAN_NEG_11_POS)
#define GLB_GPADC_SCAN_NEG_11_UMSK                              (~(((1U<<GLB_GPADC_SCAN_NEG_11_LEN)-1)<<GLB_GPADC_SCAN_NEG_11_POS))

/* 0x528 : gpadc_reg_status */
#define GLB_GPADC_REG_STATUS_OFFSET                             (0x528)
#define GLB_GPADC_DATA_RDY                                      GLB_GPADC_DATA_RDY
#define GLB_GPADC_DATA_RDY_POS                                  (0U)
#define GLB_GPADC_DATA_RDY_LEN                                  (1U)
#define GLB_GPADC_DATA_RDY_MSK                                  (((1U<<GLB_GPADC_DATA_RDY_LEN)-1)<<GLB_GPADC_DATA_RDY_POS)
#define GLB_GPADC_DATA_RDY_UMSK                                 (~(((1U<<GLB_GPADC_DATA_RDY_LEN)-1)<<GLB_GPADC_DATA_RDY_POS))
#define GLB_GPADC_RESERVED                                      GLB_GPADC_RESERVED
#define GLB_GPADC_RESERVED_POS                                  (16U)
#define GLB_GPADC_RESERVED_LEN                                  (16U)
#define GLB_GPADC_RESERVED_MSK                                  (((1U<<GLB_GPADC_RESERVED_LEN)-1)<<GLB_GPADC_RESERVED_POS)
#define GLB_GPADC_RESERVED_UMSK                                 (~(((1U<<GLB_GPADC_RESERVED_LEN)-1)<<GLB_GPADC_RESERVED_POS))

/* 0x52C : gpadc_reg_isr */
#define GLB_GPADC_REG_ISR_OFFSET                                (0x52C)
#define GLB_GPADC_NEG_SATUR                                     GLB_GPADC_NEG_SATUR
#define GLB_GPADC_NEG_SATUR_POS                                 (0U)
#define GLB_GPADC_NEG_SATUR_LEN                                 (1U)
#define GLB_GPADC_NEG_SATUR_MSK                                 (((1U<<GLB_GPADC_NEG_SATUR_LEN)-1)<<GLB_GPADC_NEG_SATUR_POS)
#define GLB_GPADC_NEG_SATUR_UMSK                                (~(((1U<<GLB_GPADC_NEG_SATUR_LEN)-1)<<GLB_GPADC_NEG_SATUR_POS))
#define GLB_GPADC_POS_SATUR                                     GLB_GPADC_POS_SATUR
#define GLB_GPADC_POS_SATUR_POS                                 (1U)
#define GLB_GPADC_POS_SATUR_LEN                                 (1U)
#define GLB_GPADC_POS_SATUR_MSK                                 (((1U<<GLB_GPADC_POS_SATUR_LEN)-1)<<GLB_GPADC_POS_SATUR_POS)
#define GLB_GPADC_POS_SATUR_UMSK                                (~(((1U<<GLB_GPADC_POS_SATUR_LEN)-1)<<GLB_GPADC_POS_SATUR_POS))
#define GLB_GPADC_NEG_SATUR_CLR                                 GLB_GPADC_NEG_SATUR_CLR
#define GLB_GPADC_NEG_SATUR_CLR_POS                             (4U)
#define GLB_GPADC_NEG_SATUR_CLR_LEN                             (1U)
#define GLB_GPADC_NEG_SATUR_CLR_MSK                             (((1U<<GLB_GPADC_NEG_SATUR_CLR_LEN)-1)<<GLB_GPADC_NEG_SATUR_CLR_POS)
#define GLB_GPADC_NEG_SATUR_CLR_UMSK                            (~(((1U<<GLB_GPADC_NEG_SATUR_CLR_LEN)-1)<<GLB_GPADC_NEG_SATUR_CLR_POS))
#define GLB_GPADC_POS_SATUR_CLR                                 GLB_GPADC_POS_SATUR_CLR
#define GLB_GPADC_POS_SATUR_CLR_POS                             (5U)
#define GLB_GPADC_POS_SATUR_CLR_LEN                             (1U)
#define GLB_GPADC_POS_SATUR_CLR_MSK                             (((1U<<GLB_GPADC_POS_SATUR_CLR_LEN)-1)<<GLB_GPADC_POS_SATUR_CLR_POS)
#define GLB_GPADC_POS_SATUR_CLR_UMSK                            (~(((1U<<GLB_GPADC_POS_SATUR_CLR_LEN)-1)<<GLB_GPADC_POS_SATUR_CLR_POS))
#define GLB_GPADC_NEG_SATUR_MASK                                GLB_GPADC_NEG_SATUR_MASK
#define GLB_GPADC_NEG_SATUR_MASK_POS                            (8U)
#define GLB_GPADC_NEG_SATUR_MASK_LEN                            (1U)
#define GLB_GPADC_NEG_SATUR_MASK_MSK                            (((1U<<GLB_GPADC_NEG_SATUR_MASK_LEN)-1)<<GLB_GPADC_NEG_SATUR_MASK_POS)
#define GLB_GPADC_NEG_SATUR_MASK_UMSK                           (~(((1U<<GLB_GPADC_NEG_SATUR_MASK_LEN)-1)<<GLB_GPADC_NEG_SATUR_MASK_POS))
#define GLB_GPADC_POS_SATUR_MASK                                GLB_GPADC_POS_SATUR_MASK
#define GLB_GPADC_POS_SATUR_MASK_POS                            (9U)
#define GLB_GPADC_POS_SATUR_MASK_LEN                            (1U)
#define GLB_GPADC_POS_SATUR_MASK_MSK                            (((1U<<GLB_GPADC_POS_SATUR_MASK_LEN)-1)<<GLB_GPADC_POS_SATUR_MASK_POS)
#define GLB_GPADC_POS_SATUR_MASK_UMSK                           (~(((1U<<GLB_GPADC_POS_SATUR_MASK_LEN)-1)<<GLB_GPADC_POS_SATUR_MASK_POS))

/* 0x530 : gpadc_reg_result */
#define GLB_GPADC_REG_RESULT_OFFSET                             (0x530)
#define GLB_GPADC_DATA_OUT                                      GLB_GPADC_DATA_OUT
#define GLB_GPADC_DATA_OUT_POS                                  (0U)
#define GLB_GPADC_DATA_OUT_LEN                                  (26U)
#define GLB_GPADC_DATA_OUT_MSK                                  (((1U<<GLB_GPADC_DATA_OUT_LEN)-1)<<GLB_GPADC_DATA_OUT_POS)
#define GLB_GPADC_DATA_OUT_UMSK                                 (~(((1U<<GLB_GPADC_DATA_OUT_LEN)-1)<<GLB_GPADC_DATA_OUT_POS))

/* 0x534 : gpadc_reg_raw_result */
#define GLB_GPADC_REG_RAW_RESULT_OFFSET                         (0x534)
#define GLB_GPADC_RAW_DATA                                      GLB_GPADC_RAW_DATA
#define GLB_GPADC_RAW_DATA_POS                                  (0U)
#define GLB_GPADC_RAW_DATA_LEN                                  (12U)
#define GLB_GPADC_RAW_DATA_MSK                                  (((1U<<GLB_GPADC_RAW_DATA_LEN)-1)<<GLB_GPADC_RAW_DATA_POS)
#define GLB_GPADC_RAW_DATA_UMSK                                 (~(((1U<<GLB_GPADC_RAW_DATA_LEN)-1)<<GLB_GPADC_RAW_DATA_POS))

/* 0x538 : gpadc_reg_define */
#define GLB_GPADC_REG_DEFINE_OFFSET                             (0x538)
#define GLB_GPADC_OS_CAL_DATA                                   GLB_GPADC_OS_CAL_DATA
#define GLB_GPADC_OS_CAL_DATA_POS                               (0U)
#define GLB_GPADC_OS_CAL_DATA_LEN                               (16U)
#define GLB_GPADC_OS_CAL_DATA_MSK                               (((1U<<GLB_GPADC_OS_CAL_DATA_LEN)-1)<<GLB_GPADC_OS_CAL_DATA_POS)
#define GLB_GPADC_OS_CAL_DATA_UMSK                              (~(((1U<<GLB_GPADC_OS_CAL_DATA_LEN)-1)<<GLB_GPADC_OS_CAL_DATA_POS))

/* 0xF00 : tzc_glb_ctrl_0 */
#define GLB_TZC_GLB_CTRL_0_OFFSET                               (0xF00)
#define GLB_TZC_GLB_SWRST_S00_LOCK                              GLB_TZC_GLB_SWRST_S00_LOCK
#define GLB_TZC_GLB_SWRST_S00_LOCK_POS                          (0U)
#define GLB_TZC_GLB_SWRST_S00_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S00_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S00_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S00_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S00_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S00_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S00_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S01_LOCK                              GLB_TZC_GLB_SWRST_S01_LOCK
#define GLB_TZC_GLB_SWRST_S01_LOCK_POS                          (1U)
#define GLB_TZC_GLB_SWRST_S01_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S01_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S01_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S01_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S01_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S01_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S01_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S30_LOCK                              GLB_TZC_GLB_SWRST_S30_LOCK
#define GLB_TZC_GLB_SWRST_S30_LOCK_POS                          (8U)
#define GLB_TZC_GLB_SWRST_S30_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S30_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S30_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S30_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S30_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S30_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S30_LOCK_POS))
#define GLB_TZC_GLB_CTRL_PWRON_RST_LOCK                         GLB_TZC_GLB_CTRL_PWRON_RST_LOCK
#define GLB_TZC_GLB_CTRL_PWRON_RST_LOCK_POS                     (12U)
#define GLB_TZC_GLB_CTRL_PWRON_RST_LOCK_LEN                     (1U)
#define GLB_TZC_GLB_CTRL_PWRON_RST_LOCK_MSK                     (((1U<<GLB_TZC_GLB_CTRL_PWRON_RST_LOCK_LEN)-1)<<GLB_TZC_GLB_CTRL_PWRON_RST_LOCK_POS)
#define GLB_TZC_GLB_CTRL_PWRON_RST_LOCK_UMSK                    (~(((1U<<GLB_TZC_GLB_CTRL_PWRON_RST_LOCK_LEN)-1)<<GLB_TZC_GLB_CTRL_PWRON_RST_LOCK_POS))
#define GLB_TZC_GLB_CTRL_CPU_RESET_LOCK                         GLB_TZC_GLB_CTRL_CPU_RESET_LOCK
#define GLB_TZC_GLB_CTRL_CPU_RESET_LOCK_POS                     (13U)
#define GLB_TZC_GLB_CTRL_CPU_RESET_LOCK_LEN                     (1U)
#define GLB_TZC_GLB_CTRL_CPU_RESET_LOCK_MSK                     (((1U<<GLB_TZC_GLB_CTRL_CPU_RESET_LOCK_LEN)-1)<<GLB_TZC_GLB_CTRL_CPU_RESET_LOCK_POS)
#define GLB_TZC_GLB_CTRL_CPU_RESET_LOCK_UMSK                    (~(((1U<<GLB_TZC_GLB_CTRL_CPU_RESET_LOCK_LEN)-1)<<GLB_TZC_GLB_CTRL_CPU_RESET_LOCK_POS))
#define GLB_TZC_GLB_CTRL_SYS_RESET_LOCK                         GLB_TZC_GLB_CTRL_SYS_RESET_LOCK
#define GLB_TZC_GLB_CTRL_SYS_RESET_LOCK_POS                     (14U)
#define GLB_TZC_GLB_CTRL_SYS_RESET_LOCK_LEN                     (1U)
#define GLB_TZC_GLB_CTRL_SYS_RESET_LOCK_MSK                     (((1U<<GLB_TZC_GLB_CTRL_SYS_RESET_LOCK_LEN)-1)<<GLB_TZC_GLB_CTRL_SYS_RESET_LOCK_POS)
#define GLB_TZC_GLB_CTRL_SYS_RESET_LOCK_UMSK                    (~(((1U<<GLB_TZC_GLB_CTRL_SYS_RESET_LOCK_LEN)-1)<<GLB_TZC_GLB_CTRL_SYS_RESET_LOCK_POS))
#define GLB_TZC_GLB_CTRL_UNGATED_AP_LOCK                        GLB_TZC_GLB_CTRL_UNGATED_AP_LOCK
#define GLB_TZC_GLB_CTRL_UNGATED_AP_LOCK_POS                    (15U)
#define GLB_TZC_GLB_CTRL_UNGATED_AP_LOCK_LEN                    (1U)
#define GLB_TZC_GLB_CTRL_UNGATED_AP_LOCK_MSK                    (((1U<<GLB_TZC_GLB_CTRL_UNGATED_AP_LOCK_LEN)-1)<<GLB_TZC_GLB_CTRL_UNGATED_AP_LOCK_POS)
#define GLB_TZC_GLB_CTRL_UNGATED_AP_LOCK_UMSK                   (~(((1U<<GLB_TZC_GLB_CTRL_UNGATED_AP_LOCK_LEN)-1)<<GLB_TZC_GLB_CTRL_UNGATED_AP_LOCK_POS))
#define GLB_TZC_GLB_MISC_LOCK                                   GLB_TZC_GLB_MISC_LOCK
#define GLB_TZC_GLB_MISC_LOCK_POS                               (25U)
#define GLB_TZC_GLB_MISC_LOCK_LEN                               (1U)
#define GLB_TZC_GLB_MISC_LOCK_MSK                               (((1U<<GLB_TZC_GLB_MISC_LOCK_LEN)-1)<<GLB_TZC_GLB_MISC_LOCK_POS)
#define GLB_TZC_GLB_MISC_LOCK_UMSK                              (~(((1U<<GLB_TZC_GLB_MISC_LOCK_LEN)-1)<<GLB_TZC_GLB_MISC_LOCK_POS))
#define GLB_TZC_GLB_SRAM_LOCK                                   GLB_TZC_GLB_SRAM_LOCK
#define GLB_TZC_GLB_SRAM_LOCK_POS                               (26U)
#define GLB_TZC_GLB_SRAM_LOCK_LEN                               (1U)
#define GLB_TZC_GLB_SRAM_LOCK_MSK                               (((1U<<GLB_TZC_GLB_SRAM_LOCK_LEN)-1)<<GLB_TZC_GLB_SRAM_LOCK_POS)
#define GLB_TZC_GLB_SRAM_LOCK_UMSK                              (~(((1U<<GLB_TZC_GLB_SRAM_LOCK_LEN)-1)<<GLB_TZC_GLB_SRAM_LOCK_POS))
#define GLB_TZC_GLB_L2C_LOCK                                    GLB_TZC_GLB_L2C_LOCK
#define GLB_TZC_GLB_L2C_LOCK_POS                                (27U)
#define GLB_TZC_GLB_L2C_LOCK_LEN                                (1U)
#define GLB_TZC_GLB_L2C_LOCK_MSK                                (((1U<<GLB_TZC_GLB_L2C_LOCK_LEN)-1)<<GLB_TZC_GLB_L2C_LOCK_POS)
#define GLB_TZC_GLB_L2C_LOCK_UMSK                               (~(((1U<<GLB_TZC_GLB_L2C_LOCK_LEN)-1)<<GLB_TZC_GLB_L2C_LOCK_POS))
#define GLB_TZC_GLB_BMX_LOCK                                    GLB_TZC_GLB_BMX_LOCK
#define GLB_TZC_GLB_BMX_LOCK_POS                                (28U)
#define GLB_TZC_GLB_BMX_LOCK_LEN                                (1U)
#define GLB_TZC_GLB_BMX_LOCK_MSK                                (((1U<<GLB_TZC_GLB_BMX_LOCK_LEN)-1)<<GLB_TZC_GLB_BMX_LOCK_POS)
#define GLB_TZC_GLB_BMX_LOCK_UMSK                               (~(((1U<<GLB_TZC_GLB_BMX_LOCK_LEN)-1)<<GLB_TZC_GLB_BMX_LOCK_POS))
#define GLB_TZC_GLB_DBG_LOCK                                    GLB_TZC_GLB_DBG_LOCK
#define GLB_TZC_GLB_DBG_LOCK_POS                                (29U)
#define GLB_TZC_GLB_DBG_LOCK_LEN                                (1U)
#define GLB_TZC_GLB_DBG_LOCK_MSK                                (((1U<<GLB_TZC_GLB_DBG_LOCK_LEN)-1)<<GLB_TZC_GLB_DBG_LOCK_POS)
#define GLB_TZC_GLB_DBG_LOCK_UMSK                               (~(((1U<<GLB_TZC_GLB_DBG_LOCK_LEN)-1)<<GLB_TZC_GLB_DBG_LOCK_POS))
#define GLB_TZC_GLB_MBIST_LOCK                                  GLB_TZC_GLB_MBIST_LOCK
#define GLB_TZC_GLB_MBIST_LOCK_POS                              (30U)
#define GLB_TZC_GLB_MBIST_LOCK_LEN                              (1U)
#define GLB_TZC_GLB_MBIST_LOCK_MSK                              (((1U<<GLB_TZC_GLB_MBIST_LOCK_LEN)-1)<<GLB_TZC_GLB_MBIST_LOCK_POS)
#define GLB_TZC_GLB_MBIST_LOCK_UMSK                             (~(((1U<<GLB_TZC_GLB_MBIST_LOCK_LEN)-1)<<GLB_TZC_GLB_MBIST_LOCK_POS))
#define GLB_TZC_GLB_CLK_LOCK                                    GLB_TZC_GLB_CLK_LOCK
#define GLB_TZC_GLB_CLK_LOCK_POS                                (31U)
#define GLB_TZC_GLB_CLK_LOCK_LEN                                (1U)
#define GLB_TZC_GLB_CLK_LOCK_MSK                                (((1U<<GLB_TZC_GLB_CLK_LOCK_LEN)-1)<<GLB_TZC_GLB_CLK_LOCK_POS)
#define GLB_TZC_GLB_CLK_LOCK_UMSK                               (~(((1U<<GLB_TZC_GLB_CLK_LOCK_LEN)-1)<<GLB_TZC_GLB_CLK_LOCK_POS))

/* 0xF04 : tzc_glb_ctrl_1 */
#define GLB_TZC_GLB_CTRL_1_OFFSET                               (0xF04)
#define GLB_TZC_GLB_SWRST_S20_LOCK                              GLB_TZC_GLB_SWRST_S20_LOCK
#define GLB_TZC_GLB_SWRST_S20_LOCK_POS                          (0U)
#define GLB_TZC_GLB_SWRST_S20_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S20_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S20_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S20_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S20_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S20_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S20_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S21_LOCK                              GLB_TZC_GLB_SWRST_S21_LOCK
#define GLB_TZC_GLB_SWRST_S21_LOCK_POS                          (1U)
#define GLB_TZC_GLB_SWRST_S21_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S21_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S21_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S21_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S21_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S21_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S21_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S22_LOCK                              GLB_TZC_GLB_SWRST_S22_LOCK
#define GLB_TZC_GLB_SWRST_S22_LOCK_POS                          (2U)
#define GLB_TZC_GLB_SWRST_S22_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S22_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S22_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S22_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S22_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S22_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S22_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S23_LOCK                              GLB_TZC_GLB_SWRST_S23_LOCK
#define GLB_TZC_GLB_SWRST_S23_LOCK_POS                          (3U)
#define GLB_TZC_GLB_SWRST_S23_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S23_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S23_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S23_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S23_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S23_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S23_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S24_LOCK                              GLB_TZC_GLB_SWRST_S24_LOCK
#define GLB_TZC_GLB_SWRST_S24_LOCK_POS                          (4U)
#define GLB_TZC_GLB_SWRST_S24_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S24_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S24_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S24_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S24_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S24_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S24_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S25_LOCK                              GLB_TZC_GLB_SWRST_S25_LOCK
#define GLB_TZC_GLB_SWRST_S25_LOCK_POS                          (5U)
#define GLB_TZC_GLB_SWRST_S25_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S25_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S25_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S25_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S25_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S25_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S25_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S26_LOCK                              GLB_TZC_GLB_SWRST_S26_LOCK
#define GLB_TZC_GLB_SWRST_S26_LOCK_POS                          (6U)
#define GLB_TZC_GLB_SWRST_S26_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S26_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S26_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S26_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S26_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S26_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S26_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S27_LOCK                              GLB_TZC_GLB_SWRST_S27_LOCK
#define GLB_TZC_GLB_SWRST_S27_LOCK_POS                          (7U)
#define GLB_TZC_GLB_SWRST_S27_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S27_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S27_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S27_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S27_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S27_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S27_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S28_LOCK                              GLB_TZC_GLB_SWRST_S28_LOCK
#define GLB_TZC_GLB_SWRST_S28_LOCK_POS                          (8U)
#define GLB_TZC_GLB_SWRST_S28_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S28_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S28_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S28_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S28_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S28_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S28_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S29_LOCK                              GLB_TZC_GLB_SWRST_S29_LOCK
#define GLB_TZC_GLB_SWRST_S29_LOCK_POS                          (9U)
#define GLB_TZC_GLB_SWRST_S29_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S29_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S29_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S29_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S29_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S29_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S29_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S2A_LOCK                              GLB_TZC_GLB_SWRST_S2A_LOCK
#define GLB_TZC_GLB_SWRST_S2A_LOCK_POS                          (10U)
#define GLB_TZC_GLB_SWRST_S2A_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S2A_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S2A_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2A_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S2A_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S2A_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2A_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S2B_LOCK                              GLB_TZC_GLB_SWRST_S2B_LOCK
#define GLB_TZC_GLB_SWRST_S2B_LOCK_POS                          (11U)
#define GLB_TZC_GLB_SWRST_S2B_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S2B_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S2B_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2B_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S2B_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S2B_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2B_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S2C_LOCK                              GLB_TZC_GLB_SWRST_S2C_LOCK
#define GLB_TZC_GLB_SWRST_S2C_LOCK_POS                          (12U)
#define GLB_TZC_GLB_SWRST_S2C_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S2C_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S2C_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2C_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S2C_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S2C_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2C_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S2D_LOCK                              GLB_TZC_GLB_SWRST_S2D_LOCK
#define GLB_TZC_GLB_SWRST_S2D_LOCK_POS                          (13U)
#define GLB_TZC_GLB_SWRST_S2D_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S2D_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S2D_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2D_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S2D_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S2D_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2D_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S2E_LOCK                              GLB_TZC_GLB_SWRST_S2E_LOCK
#define GLB_TZC_GLB_SWRST_S2E_LOCK_POS                          (14U)
#define GLB_TZC_GLB_SWRST_S2E_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S2E_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S2E_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2E_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S2E_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S2E_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2E_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S2F_LOCK                              GLB_TZC_GLB_SWRST_S2F_LOCK
#define GLB_TZC_GLB_SWRST_S2F_LOCK_POS                          (15U)
#define GLB_TZC_GLB_SWRST_S2F_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S2F_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S2F_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2F_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S2F_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S2F_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2F_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S10_LOCK                              GLB_TZC_GLB_SWRST_S10_LOCK
#define GLB_TZC_GLB_SWRST_S10_LOCK_POS                          (16U)
#define GLB_TZC_GLB_SWRST_S10_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S10_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S10_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S10_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S10_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S10_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S10_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S11_LOCK                              GLB_TZC_GLB_SWRST_S11_LOCK
#define GLB_TZC_GLB_SWRST_S11_LOCK_POS                          (17U)
#define GLB_TZC_GLB_SWRST_S11_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S11_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S11_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S11_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S11_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S11_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S11_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S12_LOCK                              GLB_TZC_GLB_SWRST_S12_LOCK
#define GLB_TZC_GLB_SWRST_S12_LOCK_POS                          (18U)
#define GLB_TZC_GLB_SWRST_S12_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S12_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S12_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S12_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S12_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S12_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S12_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S13_LOCK                              GLB_TZC_GLB_SWRST_S13_LOCK
#define GLB_TZC_GLB_SWRST_S13_LOCK_POS                          (19U)
#define GLB_TZC_GLB_SWRST_S13_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S13_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S13_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S13_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S13_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S13_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S13_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S14_LOCK                              GLB_TZC_GLB_SWRST_S14_LOCK
#define GLB_TZC_GLB_SWRST_S14_LOCK_POS                          (20U)
#define GLB_TZC_GLB_SWRST_S14_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S14_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S14_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S14_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S14_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S14_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S14_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S15_LOCK                              GLB_TZC_GLB_SWRST_S15_LOCK
#define GLB_TZC_GLB_SWRST_S15_LOCK_POS                          (21U)
#define GLB_TZC_GLB_SWRST_S15_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S15_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S15_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S15_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S15_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S15_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S15_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S16_LOCK                              GLB_TZC_GLB_SWRST_S16_LOCK
#define GLB_TZC_GLB_SWRST_S16_LOCK_POS                          (22U)
#define GLB_TZC_GLB_SWRST_S16_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S16_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S16_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S16_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S16_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S16_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S16_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S17_LOCK                              GLB_TZC_GLB_SWRST_S17_LOCK
#define GLB_TZC_GLB_SWRST_S17_LOCK_POS                          (23U)
#define GLB_TZC_GLB_SWRST_S17_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S17_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S17_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S17_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S17_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S17_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S17_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S18_LOCK                              GLB_TZC_GLB_SWRST_S18_LOCK
#define GLB_TZC_GLB_SWRST_S18_LOCK_POS                          (24U)
#define GLB_TZC_GLB_SWRST_S18_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S18_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S18_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S18_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S18_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S18_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S18_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S19_LOCK                              GLB_TZC_GLB_SWRST_S19_LOCK
#define GLB_TZC_GLB_SWRST_S19_LOCK_POS                          (25U)
#define GLB_TZC_GLB_SWRST_S19_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S19_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S19_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S19_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S19_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S19_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S19_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S1A_LOCK                              GLB_TZC_GLB_SWRST_S1A_LOCK
#define GLB_TZC_GLB_SWRST_S1A_LOCK_POS                          (26U)
#define GLB_TZC_GLB_SWRST_S1A_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S1A_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S1A_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1A_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S1A_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S1A_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1A_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S1B_LOCK                              GLB_TZC_GLB_SWRST_S1B_LOCK
#define GLB_TZC_GLB_SWRST_S1B_LOCK_POS                          (27U)
#define GLB_TZC_GLB_SWRST_S1B_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S1B_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S1B_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1B_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S1B_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S1B_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1B_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S1C_LOCK                              GLB_TZC_GLB_SWRST_S1C_LOCK
#define GLB_TZC_GLB_SWRST_S1C_LOCK_POS                          (28U)
#define GLB_TZC_GLB_SWRST_S1C_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S1C_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S1C_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1C_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S1C_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S1C_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1C_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S1D_LOCK                              GLB_TZC_GLB_SWRST_S1D_LOCK
#define GLB_TZC_GLB_SWRST_S1D_LOCK_POS                          (29U)
#define GLB_TZC_GLB_SWRST_S1D_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S1D_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S1D_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1D_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S1D_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S1D_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1D_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S1E_LOCK                              GLB_TZC_GLB_SWRST_S1E_LOCK
#define GLB_TZC_GLB_SWRST_S1E_LOCK_POS                          (30U)
#define GLB_TZC_GLB_SWRST_S1E_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S1E_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S1E_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1E_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S1E_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S1E_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1E_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S1F_LOCK                              GLB_TZC_GLB_SWRST_S1F_LOCK
#define GLB_TZC_GLB_SWRST_S1F_LOCK_POS                          (31U)
#define GLB_TZC_GLB_SWRST_S1F_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S1F_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S1F_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1F_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S1F_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S1F_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1F_LOCK_POS))

/* 0xF08 : tzc_glb_ctrl_2 */
#define GLB_TZC_GLB_CTRL_2_OFFSET                               (0xF08)
#define GLB_TZC_GLB_GPIO_0_LOCK                                 GLB_TZC_GLB_GPIO_0_LOCK
#define GLB_TZC_GLB_GPIO_0_LOCK_POS                             (0U)
#define GLB_TZC_GLB_GPIO_0_LOCK_LEN                             (1U)
#define GLB_TZC_GLB_GPIO_0_LOCK_MSK                             (((1U<<GLB_TZC_GLB_GPIO_0_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_0_LOCK_POS)
#define GLB_TZC_GLB_GPIO_0_LOCK_UMSK                            (~(((1U<<GLB_TZC_GLB_GPIO_0_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_0_LOCK_POS))
#define GLB_TZC_GLB_GPIO_1_LOCK                                 GLB_TZC_GLB_GPIO_1_LOCK
#define GLB_TZC_GLB_GPIO_1_LOCK_POS                             (1U)
#define GLB_TZC_GLB_GPIO_1_LOCK_LEN                             (1U)
#define GLB_TZC_GLB_GPIO_1_LOCK_MSK                             (((1U<<GLB_TZC_GLB_GPIO_1_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_1_LOCK_POS)
#define GLB_TZC_GLB_GPIO_1_LOCK_UMSK                            (~(((1U<<GLB_TZC_GLB_GPIO_1_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_1_LOCK_POS))
#define GLB_TZC_GLB_GPIO_2_LOCK                                 GLB_TZC_GLB_GPIO_2_LOCK
#define GLB_TZC_GLB_GPIO_2_LOCK_POS                             (2U)
#define GLB_TZC_GLB_GPIO_2_LOCK_LEN                             (1U)
#define GLB_TZC_GLB_GPIO_2_LOCK_MSK                             (((1U<<GLB_TZC_GLB_GPIO_2_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_2_LOCK_POS)
#define GLB_TZC_GLB_GPIO_2_LOCK_UMSK                            (~(((1U<<GLB_TZC_GLB_GPIO_2_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_2_LOCK_POS))
#define GLB_TZC_GLB_GPIO_3_LOCK                                 GLB_TZC_GLB_GPIO_3_LOCK
#define GLB_TZC_GLB_GPIO_3_LOCK_POS                             (3U)
#define GLB_TZC_GLB_GPIO_3_LOCK_LEN                             (1U)
#define GLB_TZC_GLB_GPIO_3_LOCK_MSK                             (((1U<<GLB_TZC_GLB_GPIO_3_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_3_LOCK_POS)
#define GLB_TZC_GLB_GPIO_3_LOCK_UMSK                            (~(((1U<<GLB_TZC_GLB_GPIO_3_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_3_LOCK_POS))
#define GLB_TZC_GLB_GPIO_4_LOCK                                 GLB_TZC_GLB_GPIO_4_LOCK
#define GLB_TZC_GLB_GPIO_4_LOCK_POS                             (4U)
#define GLB_TZC_GLB_GPIO_4_LOCK_LEN                             (1U)
#define GLB_TZC_GLB_GPIO_4_LOCK_MSK                             (((1U<<GLB_TZC_GLB_GPIO_4_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_4_LOCK_POS)
#define GLB_TZC_GLB_GPIO_4_LOCK_UMSK                            (~(((1U<<GLB_TZC_GLB_GPIO_4_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_4_LOCK_POS))
#define GLB_TZC_GLB_GPIO_5_LOCK                                 GLB_TZC_GLB_GPIO_5_LOCK
#define GLB_TZC_GLB_GPIO_5_LOCK_POS                             (5U)
#define GLB_TZC_GLB_GPIO_5_LOCK_LEN                             (1U)
#define GLB_TZC_GLB_GPIO_5_LOCK_MSK                             (((1U<<GLB_TZC_GLB_GPIO_5_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_5_LOCK_POS)
#define GLB_TZC_GLB_GPIO_5_LOCK_UMSK                            (~(((1U<<GLB_TZC_GLB_GPIO_5_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_5_LOCK_POS))
#define GLB_TZC_GLB_GPIO_6_LOCK                                 GLB_TZC_GLB_GPIO_6_LOCK
#define GLB_TZC_GLB_GPIO_6_LOCK_POS                             (6U)
#define GLB_TZC_GLB_GPIO_6_LOCK_LEN                             (1U)
#define GLB_TZC_GLB_GPIO_6_LOCK_MSK                             (((1U<<GLB_TZC_GLB_GPIO_6_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_6_LOCK_POS)
#define GLB_TZC_GLB_GPIO_6_LOCK_UMSK                            (~(((1U<<GLB_TZC_GLB_GPIO_6_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_6_LOCK_POS))
#define GLB_TZC_GLB_GPIO_7_LOCK                                 GLB_TZC_GLB_GPIO_7_LOCK
#define GLB_TZC_GLB_GPIO_7_LOCK_POS                             (7U)
#define GLB_TZC_GLB_GPIO_7_LOCK_LEN                             (1U)
#define GLB_TZC_GLB_GPIO_7_LOCK_MSK                             (((1U<<GLB_TZC_GLB_GPIO_7_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_7_LOCK_POS)
#define GLB_TZC_GLB_GPIO_7_LOCK_UMSK                            (~(((1U<<GLB_TZC_GLB_GPIO_7_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_7_LOCK_POS))
#define GLB_TZC_GLB_GPIO_8_LOCK                                 GLB_TZC_GLB_GPIO_8_LOCK
#define GLB_TZC_GLB_GPIO_8_LOCK_POS                             (8U)
#define GLB_TZC_GLB_GPIO_8_LOCK_LEN                             (1U)
#define GLB_TZC_GLB_GPIO_8_LOCK_MSK                             (((1U<<GLB_TZC_GLB_GPIO_8_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_8_LOCK_POS)
#define GLB_TZC_GLB_GPIO_8_LOCK_UMSK                            (~(((1U<<GLB_TZC_GLB_GPIO_8_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_8_LOCK_POS))
#define GLB_TZC_GLB_GPIO_9_LOCK                                 GLB_TZC_GLB_GPIO_9_LOCK
#define GLB_TZC_GLB_GPIO_9_LOCK_POS                             (9U)
#define GLB_TZC_GLB_GPIO_9_LOCK_LEN                             (1U)
#define GLB_TZC_GLB_GPIO_9_LOCK_MSK                             (((1U<<GLB_TZC_GLB_GPIO_9_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_9_LOCK_POS)
#define GLB_TZC_GLB_GPIO_9_LOCK_UMSK                            (~(((1U<<GLB_TZC_GLB_GPIO_9_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_9_LOCK_POS))
#define GLB_TZC_GLB_GPIO_10_LOCK                                GLB_TZC_GLB_GPIO_10_LOCK
#define GLB_TZC_GLB_GPIO_10_LOCK_POS                            (10U)
#define GLB_TZC_GLB_GPIO_10_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_10_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_10_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_10_LOCK_POS)
#define GLB_TZC_GLB_GPIO_10_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_10_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_10_LOCK_POS))
#define GLB_TZC_GLB_GPIO_11_LOCK                                GLB_TZC_GLB_GPIO_11_LOCK
#define GLB_TZC_GLB_GPIO_11_LOCK_POS                            (11U)
#define GLB_TZC_GLB_GPIO_11_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_11_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_11_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_11_LOCK_POS)
#define GLB_TZC_GLB_GPIO_11_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_11_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_11_LOCK_POS))
#define GLB_TZC_GLB_GPIO_12_LOCK                                GLB_TZC_GLB_GPIO_12_LOCK
#define GLB_TZC_GLB_GPIO_12_LOCK_POS                            (12U)
#define GLB_TZC_GLB_GPIO_12_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_12_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_12_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_12_LOCK_POS)
#define GLB_TZC_GLB_GPIO_12_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_12_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_12_LOCK_POS))
#define GLB_TZC_GLB_GPIO_13_LOCK                                GLB_TZC_GLB_GPIO_13_LOCK
#define GLB_TZC_GLB_GPIO_13_LOCK_POS                            (13U)
#define GLB_TZC_GLB_GPIO_13_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_13_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_13_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_13_LOCK_POS)
#define GLB_TZC_GLB_GPIO_13_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_13_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_13_LOCK_POS))
#define GLB_TZC_GLB_GPIO_14_LOCK                                GLB_TZC_GLB_GPIO_14_LOCK
#define GLB_TZC_GLB_GPIO_14_LOCK_POS                            (14U)
#define GLB_TZC_GLB_GPIO_14_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_14_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_14_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_14_LOCK_POS)
#define GLB_TZC_GLB_GPIO_14_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_14_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_14_LOCK_POS))
#define GLB_TZC_GLB_GPIO_15_LOCK                                GLB_TZC_GLB_GPIO_15_LOCK
#define GLB_TZC_GLB_GPIO_15_LOCK_POS                            (15U)
#define GLB_TZC_GLB_GPIO_15_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_15_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_15_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_15_LOCK_POS)
#define GLB_TZC_GLB_GPIO_15_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_15_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_15_LOCK_POS))
#define GLB_TZC_GLB_GPIO_16_LOCK                                GLB_TZC_GLB_GPIO_16_LOCK
#define GLB_TZC_GLB_GPIO_16_LOCK_POS                            (16U)
#define GLB_TZC_GLB_GPIO_16_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_16_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_16_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_16_LOCK_POS)
#define GLB_TZC_GLB_GPIO_16_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_16_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_16_LOCK_POS))
#define GLB_TZC_GLB_GPIO_17_LOCK                                GLB_TZC_GLB_GPIO_17_LOCK
#define GLB_TZC_GLB_GPIO_17_LOCK_POS                            (17U)
#define GLB_TZC_GLB_GPIO_17_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_17_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_17_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_17_LOCK_POS)
#define GLB_TZC_GLB_GPIO_17_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_17_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_17_LOCK_POS))
#define GLB_TZC_GLB_GPIO_18_LOCK                                GLB_TZC_GLB_GPIO_18_LOCK
#define GLB_TZC_GLB_GPIO_18_LOCK_POS                            (18U)
#define GLB_TZC_GLB_GPIO_18_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_18_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_18_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_18_LOCK_POS)
#define GLB_TZC_GLB_GPIO_18_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_18_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_18_LOCK_POS))
#define GLB_TZC_GLB_GPIO_19_LOCK                                GLB_TZC_GLB_GPIO_19_LOCK
#define GLB_TZC_GLB_GPIO_19_LOCK_POS                            (19U)
#define GLB_TZC_GLB_GPIO_19_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_19_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_19_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_19_LOCK_POS)
#define GLB_TZC_GLB_GPIO_19_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_19_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_19_LOCK_POS))
#define GLB_TZC_GLB_GPIO_20_LOCK                                GLB_TZC_GLB_GPIO_20_LOCK
#define GLB_TZC_GLB_GPIO_20_LOCK_POS                            (20U)
#define GLB_TZC_GLB_GPIO_20_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_20_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_20_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_20_LOCK_POS)
#define GLB_TZC_GLB_GPIO_20_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_20_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_20_LOCK_POS))
#define GLB_TZC_GLB_GPIO_21_LOCK                                GLB_TZC_GLB_GPIO_21_LOCK
#define GLB_TZC_GLB_GPIO_21_LOCK_POS                            (21U)
#define GLB_TZC_GLB_GPIO_21_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_21_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_21_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_21_LOCK_POS)
#define GLB_TZC_GLB_GPIO_21_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_21_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_21_LOCK_POS))
#define GLB_TZC_GLB_GPIO_22_LOCK                                GLB_TZC_GLB_GPIO_22_LOCK
#define GLB_TZC_GLB_GPIO_22_LOCK_POS                            (22U)
#define GLB_TZC_GLB_GPIO_22_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_22_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_22_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_22_LOCK_POS)
#define GLB_TZC_GLB_GPIO_22_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_22_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_22_LOCK_POS))
#define GLB_TZC_GLB_GPIO_23_LOCK                                GLB_TZC_GLB_GPIO_23_LOCK
#define GLB_TZC_GLB_GPIO_23_LOCK_POS                            (23U)
#define GLB_TZC_GLB_GPIO_23_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_23_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_23_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_23_LOCK_POS)
#define GLB_TZC_GLB_GPIO_23_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_23_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_23_LOCK_POS))
#define GLB_TZC_GLB_GPIO_24_LOCK                                GLB_TZC_GLB_GPIO_24_LOCK
#define GLB_TZC_GLB_GPIO_24_LOCK_POS                            (24U)
#define GLB_TZC_GLB_GPIO_24_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_24_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_24_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_24_LOCK_POS)
#define GLB_TZC_GLB_GPIO_24_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_24_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_24_LOCK_POS))
#define GLB_TZC_GLB_GPIO_25_LOCK                                GLB_TZC_GLB_GPIO_25_LOCK
#define GLB_TZC_GLB_GPIO_25_LOCK_POS                            (25U)
#define GLB_TZC_GLB_GPIO_25_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_25_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_25_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_25_LOCK_POS)
#define GLB_TZC_GLB_GPIO_25_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_25_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_25_LOCK_POS))
#define GLB_TZC_GLB_GPIO_26_LOCK                                GLB_TZC_GLB_GPIO_26_LOCK
#define GLB_TZC_GLB_GPIO_26_LOCK_POS                            (26U)
#define GLB_TZC_GLB_GPIO_26_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_26_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_26_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_26_LOCK_POS)
#define GLB_TZC_GLB_GPIO_26_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_26_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_26_LOCK_POS))
#define GLB_TZC_GLB_GPIO_27_LOCK                                GLB_TZC_GLB_GPIO_27_LOCK
#define GLB_TZC_GLB_GPIO_27_LOCK_POS                            (27U)
#define GLB_TZC_GLB_GPIO_27_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_27_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_27_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_27_LOCK_POS)
#define GLB_TZC_GLB_GPIO_27_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_27_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_27_LOCK_POS))
#define GLB_TZC_GLB_GPIO_28_LOCK                                GLB_TZC_GLB_GPIO_28_LOCK
#define GLB_TZC_GLB_GPIO_28_LOCK_POS                            (28U)
#define GLB_TZC_GLB_GPIO_28_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_28_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_28_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_28_LOCK_POS)
#define GLB_TZC_GLB_GPIO_28_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_28_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_28_LOCK_POS))
#define GLB_TZC_GLB_GPIO_29_LOCK                                GLB_TZC_GLB_GPIO_29_LOCK
#define GLB_TZC_GLB_GPIO_29_LOCK_POS                            (29U)
#define GLB_TZC_GLB_GPIO_29_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_29_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_29_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_29_LOCK_POS)
#define GLB_TZC_GLB_GPIO_29_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_29_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_29_LOCK_POS))
#define GLB_TZC_GLB_GPIO_30_LOCK                                GLB_TZC_GLB_GPIO_30_LOCK
#define GLB_TZC_GLB_GPIO_30_LOCK_POS                            (30U)
#define GLB_TZC_GLB_GPIO_30_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_30_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_30_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_30_LOCK_POS)
#define GLB_TZC_GLB_GPIO_30_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_30_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_30_LOCK_POS))
#define GLB_TZC_GLB_GPIO_31_LOCK                                GLB_TZC_GLB_GPIO_31_LOCK
#define GLB_TZC_GLB_GPIO_31_LOCK_POS                            (31U)
#define GLB_TZC_GLB_GPIO_31_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_31_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_31_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_31_LOCK_POS)
#define GLB_TZC_GLB_GPIO_31_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_31_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_31_LOCK_POS))

/* 0xF0C : tzc_glb_ctrl_3 */
#define GLB_TZC_GLB_CTRL_3_OFFSET                               (0xF0C)
#define GLB_TZC_GLB_GPIO_32_LOCK                                GLB_TZC_GLB_GPIO_32_LOCK
#define GLB_TZC_GLB_GPIO_32_LOCK_POS                            (0U)
#define GLB_TZC_GLB_GPIO_32_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_32_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_32_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_32_LOCK_POS)
#define GLB_TZC_GLB_GPIO_32_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_32_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_32_LOCK_POS))
#define GLB_TZC_GLB_GPIO_33_LOCK                                GLB_TZC_GLB_GPIO_33_LOCK
#define GLB_TZC_GLB_GPIO_33_LOCK_POS                            (1U)
#define GLB_TZC_GLB_GPIO_33_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_33_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_33_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_33_LOCK_POS)
#define GLB_TZC_GLB_GPIO_33_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_33_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_33_LOCK_POS))
#define GLB_TZC_GLB_GPIO_34_LOCK                                GLB_TZC_GLB_GPIO_34_LOCK
#define GLB_TZC_GLB_GPIO_34_LOCK_POS                            (2U)
#define GLB_TZC_GLB_GPIO_34_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_34_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_34_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_34_LOCK_POS)
#define GLB_TZC_GLB_GPIO_34_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_34_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_34_LOCK_POS))
#define GLB_TZC_GLB_GPIO_35_LOCK                                GLB_TZC_GLB_GPIO_35_LOCK
#define GLB_TZC_GLB_GPIO_35_LOCK_POS                            (3U)
#define GLB_TZC_GLB_GPIO_35_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_35_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_35_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_35_LOCK_POS)
#define GLB_TZC_GLB_GPIO_35_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_35_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_35_LOCK_POS))
#define GLB_TZC_GLB_GPIO_36_LOCK                                GLB_TZC_GLB_GPIO_36_LOCK
#define GLB_TZC_GLB_GPIO_36_LOCK_POS                            (4U)
#define GLB_TZC_GLB_GPIO_36_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_36_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_36_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_36_LOCK_POS)
#define GLB_TZC_GLB_GPIO_36_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_36_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_36_LOCK_POS))
#define GLB_TZC_GLB_GPIO_37_LOCK                                GLB_TZC_GLB_GPIO_37_LOCK
#define GLB_TZC_GLB_GPIO_37_LOCK_POS                            (5U)
#define GLB_TZC_GLB_GPIO_37_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_37_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_37_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_37_LOCK_POS)
#define GLB_TZC_GLB_GPIO_37_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_37_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_37_LOCK_POS))


struct  glb_reg {
    /* 0x0 : clk_cfg0 */
    union {
        struct {
            uint32_t reg_pll_en                     :  1; /* [    0],        r/w,        0x1 */
            uint32_t reg_fclk_en                    :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_hclk_en                    :  1; /* [    2],        r/w,        0x1 */
            uint32_t reg_bclk_en                    :  1; /* [    3],        r/w,        0x1 */
            uint32_t reg_pll_sel                    :  2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t hbn_root_clk_sel               :  2; /* [ 7: 6],          r,        0x0 */
            uint32_t reg_hclk_div                   :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t reg_bclk_div                   :  8; /* [23:16],        r/w,        0x0 */
            uint32_t fclk_sw_state                  :  3; /* [26:24],          r,        0x0 */
            uint32_t chip_rdy                       :  1; /* [   27],          r,        0x0 */
            uint32_t glb_id                         :  4; /* [31:28],          r,        0x7 */
        }BF;
        uint32_t WORD;
    } clk_cfg0;

    /* 0x4 : clk_cfg1 */
    union {
        struct {
            uint32_t kys_1m_clk_div                 :  5; /* [ 4: 0],        r/w,       0x1f */
            uint32_t reserved_5_6                   :  2; /* [ 6: 5],       rsvd,        0x0 */
            uint32_t kys_1m_clk_sel                 :  1; /* [    7],        r/w,        0x0 */
            uint32_t reg_audio_clk_div              :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t reg_audio_clk_div_en           :  1; /* [   12],        r/w,        0x0 */
            uint32_t reg_audio_auto_div_en          :  1; /* [   13],        r/w,        0x0 */
            uint32_t reg_audio_clk_src_sel          :  1; /* [   14],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t ble_clk_sel                    :  6; /* [21:16],        r/w,       0x10 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t ble_en                         :  1; /* [   24],        r/w,        0x1 */
            uint32_t m154_zbEn                      :  1; /* [   25],        r/w,        0x1 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } clk_cfg1;

    /* 0x8 : clk_cfg2 */
    union {
        struct {
            uint32_t uart_clk_div                   :  3; /* [ 2: 0],        r/w,        0x7 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t uart_clk_en                    :  1; /* [    4],        r/w,        0x1 */
            uint32_t reserved_5_6                   :  2; /* [ 6: 5],       rsvd,        0x0 */
            uint32_t hbn_uart_clk_sel               :  1; /* [    7],          r,        0x0 */
            uint32_t sf_clk_div                     :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t sf_clk_en                      :  1; /* [   11],        r/w,        0x1 */
            uint32_t sf_clk_sel                     :  2; /* [13:12],        r/w,        0x2 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t ir_clk_div                     :  6; /* [21:16],        r/w,        0xf */
            uint32_t hbn_uart_clk_sel2              :  1; /* [   22],          r,        0x0 */
            uint32_t ir_clk_en                      :  1; /* [   23],        r/w,        0x1 */
            uint32_t dma_clk_en                     :  4; /* [27:24],        r/w,        0xf */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } clk_cfg2;

    /* 0xC : clk_cfg3 */
    union {
        struct {
            uint32_t spi_clk_div                    :  5; /* [ 4: 0],        r/w,        0x3 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t spi_clk_en                     :  1; /* [    8],        r/w,        0x1 */
            uint32_t spi_clk_sel                    :  1; /* [    9],        r/w,        0x0 */
            uint32_t chip_clk_out_en                :  4; /* [13:10],        r/w,        0x0 */
            uint32_t gpio_tmr_clk_sel               :  2; /* [15:14],        r/w,        0x0 */
            uint32_t i2c_clk_div                    :  8; /* [23:16],        r/w,       0xff */
            uint32_t i2c_clk_en                     :  1; /* [   24],        r/w,        0x1 */
            uint32_t i2c_clk_sel                    :  1; /* [   25],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t chip_clk_out_0_sel             :  2; /* [29:28],        r/w,        0x0 */
            uint32_t chip_clk_out_1_sel             :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } clk_cfg3;

    /* 0x10 : swrst_cfg0 */
    union {
        struct {
            uint32_t reserved_0_3                   :  4; /* [ 3: 0],       rsvd,        0x0 */
            uint32_t swrst_s300                     :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t swrst_s301                     :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_31                  : 23; /* [31: 9],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } swrst_cfg0;

    /* 0x14 : swrst_cfg1 */
    union {
        struct {
            uint32_t swrst_s10                      :  1; /* [    0],        r/w,        0x0 */
            uint32_t swrst_s11                      :  1; /* [    1],        r/w,        0x0 */
            uint32_t swrst_s12                      :  1; /* [    2],        r/w,        0x0 */
            uint32_t swrst_s13                      :  1; /* [    3],        r/w,        0x0 */
            uint32_t swrst_s14                      :  1; /* [    4],        r/w,        0x0 */
            uint32_t swrst_s15                      :  1; /* [    5],        r/w,        0x0 */
            uint32_t swrst_s16                      :  1; /* [    6],        r/w,        0x0 */
            uint32_t swrst_s17                      :  1; /* [    7],        r/w,        0x0 */
            uint32_t swrst_s18                      :  1; /* [    8],        r/w,        0x0 */
            uint32_t swrst_s19                      :  1; /* [    9],        r/w,        0x0 */
            uint32_t reserved_10                    :  1; /* [   10],       rsvd,        0x0 */
            uint32_t swrst_s1b                      :  1; /* [   11],        r/w,        0x0 */
            uint32_t swrst_s1c                      :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13                    :  1; /* [   13],       rsvd,        0x0 */
            uint32_t swrst_s1e                      :  1; /* [   14],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t swrst_s1a0                     :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17                    :  1; /* [   17],       rsvd,        0x0 */
            uint32_t swrst_s1a2                     :  1; /* [   18],        r/w,        0x0 */
            uint32_t swrst_s1a3                     :  1; /* [   19],        r/w,        0x0 */
            uint32_t swrst_s1a4                     :  1; /* [   20],        r/w,        0x0 */
            uint32_t swrst_s1a5                     :  1; /* [   21],        r/w,        0x0 */
            uint32_t swrst_s1a6                     :  1; /* [   22],        r/w,        0x0 */
            uint32_t swrst_s1a7                     :  1; /* [   23],        r/w,        0x0 */
            uint32_t reserved_24                    :  1; /* [   24],       rsvd,        0x0 */
            uint32_t swrst_s1a9                     :  1; /* [   25],        r/w,        0x0 */
            uint32_t reserved_26_28                 :  3; /* [28:26],       rsvd,        0x0 */
            uint32_t swrst_s1ad                     :  1; /* [   29],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } swrst_cfg1;

    /* 0x18 : swrst_cfg2 */
    union {
        struct {
            uint32_t reg_ctrl_pwron_rst             :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_ctrl_cpu_reset             :  1; /* [    1],        r/w,        0x0 */
            uint32_t reg_ctrl_sys_reset             :  1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t reg_ctrl_reset_dummy           :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t reserved_8_23                  : 16; /* [23: 8],       rsvd,        0x0 */
            uint32_t pka_clk_sel                    :  2; /* [25:24],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } swrst_cfg2;

    /* 0x1C : swrst_cfg3 */
    union {
        struct {
            uint32_t reserved_0                     :  1; /* [    0],       rsvd,        0x0 */
            uint32_t disrst_s11                     :  1; /* [    1],        r/w,        0x0 */
            uint32_t disrst_s12                     :  1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3_7                   :  5; /* [ 7: 3],       rsvd,        0x0 */
            uint32_t disrst_s18                     :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_10                  :  2; /* [10: 9],       rsvd,        0x0 */
            uint32_t disrst_s1b                     :  1; /* [   11],        r/w,        0x0 */
            uint32_t disrst_s1c                     :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t disrst_s1a0                    :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17                    :  1; /* [   17],       rsvd,        0x0 */
            uint32_t disrst_s1a2                    :  1; /* [   18],        r/w,        0x0 */
            uint32_t disrst_s1a3                    :  1; /* [   19],        r/w,        0x0 */
            uint32_t disrst_s1a4                    :  1; /* [   20],        r/w,        0x0 */
            uint32_t disrst_s1a5                    :  1; /* [   21],        r/w,        0x0 */
            uint32_t disrst_s1a6                    :  1; /* [   22],        r/w,        0x0 */
            uint32_t disrst_s1a7                    :  1; /* [   23],        r/w,        0x0 */
            uint32_t reserved_24                    :  1; /* [   24],       rsvd,        0x0 */
            uint32_t disrst_s1a9                    :  1; /* [   25],        r/w,        0x0 */
            uint32_t reserved_26_28                 :  3; /* [28:26],       rsvd,        0x0 */
            uint32_t disrst_s1ad                    :  1; /* [   29],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } swrst_cfg3;

    /* 0x20 : cgen_cfg0 */
    union {
        struct {
            uint32_t cgen_m0                        :  1; /* [    0],        r/w,        0x1 */
            uint32_t reserved_1                     :  1; /* [    1],       rsvd,        0x0 */
            uint32_t cgen_m2                        :  1; /* [    2],        r/w,        0x1 */
            uint32_t cgen_m3                        :  1; /* [    3],        r/w,        0x1 */
            uint32_t cgen_m4                        :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_31                  : 27; /* [31: 5],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cgen_cfg0;

    /* 0x24 : cgen_cfg1 */
    union {
        struct {
            uint32_t reserved_0_1                   :  2; /* [ 1: 0],       rsvd,        0x0 */
            uint32_t cgen_s12                       :  1; /* [    2],        r/w,        0x1 */
            uint32_t cgen_s13                       :  1; /* [    3],        r/w,        0x1 */
            uint32_t cgen_s14                       :  1; /* [    4],        r/w,        0x1 */
            uint32_t cgen_s15                       :  1; /* [    5],        r/w,        0x1 */
            uint32_t reserved_6                     :  1; /* [    6],       rsvd,        0x0 */
            uint32_t cgen_s17                       :  1; /* [    7],        r/w,        0x1 */
            uint32_t reserved_8_10                  :  3; /* [10: 8],       rsvd,        0x0 */
            uint32_t cgen_s1b                       :  1; /* [   11],        r/w,        0x1 */
            uint32_t cgen_s1c                       :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t cgen_s1a0                      :  1; /* [   16],        r/w,        0x1 */
            uint32_t reserved_17                    :  1; /* [   17],       rsvd,        0x0 */
            uint32_t cgen_s1a2                      :  1; /* [   18],        r/w,        0x0 */
            uint32_t cgen_s1a3                      :  1; /* [   19],        r/w,        0x0 */
            uint32_t cgen_s1a4                      :  1; /* [   20],        r/w,        0x0 */
            uint32_t cgen_s1a5                      :  1; /* [   21],        r/w,        0x1 */
            uint32_t cgen_s1a6                      :  1; /* [   22],        r/w,        0x0 */
            uint32_t cgen_s1a7                      :  1; /* [   23],        r/w,        0x0 */
            uint32_t reserved_24                    :  1; /* [   24],       rsvd,        0x0 */
            uint32_t cgen_s1a9                      :  1; /* [   25],        r/w,        0x0 */
            uint32_t reserved_26_28                 :  3; /* [28:26],       rsvd,        0x0 */
            uint32_t cgen_s1ad                      :  1; /* [   29],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cgen_cfg1;

    /* 0x28 : cgen_cfg2 */
    union {
        struct {
            uint32_t cgen_s300                      :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t cgen_s301                      :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_31                  : 27; /* [31: 5],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cgen_cfg2;

    /* 0x2C : cgen_cfg3 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cgen_cfg3;

    /* 0x30 : MBIST_CTL */
    union {
        struct {
            uint32_t irom_mbist_mode                :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1                     :  1; /* [    1],       rsvd,        0x0 */
            uint32_t hsram_cache_mbist_mode         :  1; /* [    2],        r/w,        0x0 */
            uint32_t tag_mbist_mode                 :  1; /* [    3],        r/w,        0x0 */
            uint32_t ocram_mbist_mode               :  1; /* [    4],        r/w,        0x0 */
            uint32_t em_ram_mbist_mode              :  1; /* [    5],        r/w,        0x0 */
            uint32_t m154_mbist_mode                :  1; /* [    6],        r/w,        0x0 */
            uint32_t sf_mbist_mode                  :  1; /* [    7],        r/w,        0x0 */
            uint32_t sec_mbist_mode                 :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_30                  : 22; /* [30: 9],       rsvd,        0x0 */
            uint32_t reg_mbist_rst_n                :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } MBIST_CTL;

    /* 0x34 : MBIST_STAT */
    union {
        struct {
            uint32_t irom_mbist_done                :  1; /* [    0],          r,        0x0 */
            uint32_t reserved_1                     :  1; /* [    1],       rsvd,        0x0 */
            uint32_t hsram_cache_mbist_done         :  1; /* [    2],          r,        0x0 */
            uint32_t tag_mbist_done                 :  1; /* [    3],          r,        0x0 */
            uint32_t ocram_mbist_done               :  1; /* [    4],          r,        0x0 */
            uint32_t em_ram_mbist_done              :  1; /* [    5],          r,        0x0 */
            uint32_t m154_mbist_done                :  1; /* [    6],          r,        0x0 */
            uint32_t sf_mbist_done                  :  1; /* [    7],          r,        0x0 */
            uint32_t sec_mbist_done                 :  1; /* [    8],          r,        0x0 */
            uint32_t reserved_9_15                  :  7; /* [15: 9],       rsvd,        0x0 */
            uint32_t irom_mbist_fail                :  1; /* [   16],          r,        0x0 */
            uint32_t reserved_17                    :  1; /* [   17],       rsvd,        0x0 */
            uint32_t hsram_cache_mbist_fail         :  1; /* [   18],          r,        0x0 */
            uint32_t tag_mbist_fail                 :  1; /* [   19],          r,        0x0 */
            uint32_t ocram_mbist_fail               :  1; /* [   20],          r,        0x0 */
            uint32_t em_ram_mbist_fail              :  1; /* [   21],          r,        0x0 */
            uint32_t m154_mbist_fail                :  1; /* [   22],          r,        0x0 */
            uint32_t sf_mbist_fail                  :  1; /* [   23],          r,        0x0 */
            uint32_t sec_mbist_fail                 :  1; /* [   24],          r,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } MBIST_STAT;

    /* 0x38  reserved */
    uint8_t RESERVED0x38[8];

    /* 0x40 : NEW_RTC_TIME_L */
    union {
        struct {
            uint32_t rtc_timer_latch_l              : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } NEW_RTC_TIME_L;

    /* 0x44 : NEW_RTC_TIME_H */
    union {
        struct {
            uint32_t rtc_timer_latch_h              :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t reserved_8_23                  : 16; /* [23: 8],       rsvd,        0x0 */
            uint32_t reg_rtc_timer_latch_en         :  1; /* [   24],        r/w,        0x1 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } NEW_RTC_TIME_H;

    /* 0x48  reserved */
    uint8_t RESERVED0x48[4];

    /* 0x4C : XTAL_DEG_32K */
    union {
        struct {
            uint32_t xtal_deg_cnt_limit             :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t reserved_8_12                  :  5; /* [12: 8],       rsvd,        0x0 */
            uint32_t reg_rc32k_deg_en               :  1; /* [   13],        r/w,        0x0 */
            uint32_t rc32k_deg_end_ps               :  1; /* [   14],        w1p,        0x0 */
            uint32_t rc32k_deg_start_ps             :  1; /* [   15],        w1p,        0x0 */
            uint32_t reserved_16_26                 : 11; /* [26:16],       rsvd,        0x0 */
            uint32_t xtal_cnt_32k_cgen              :  1; /* [   27],        r/w,        0x1 */
            uint32_t clr_xtal_cnt_32k_done          :  1; /* [   28],        w1p,        0x0 */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t xtal_cnt_32k_sw_trig_ps        :  1; /* [   31],        w1p,        0x0 */
        }BF;
        uint32_t WORD;
    } XTAL_DEG_32K;

    /* 0x50 : bmx_cfg1 */
    union {
        struct {
            uint32_t bmx_timeout_en                 :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t bmx_arb_mode                   :  2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t bmx_err_en                     :  1; /* [    8],        r/w,        0x0 */
            uint32_t bmx_busy_option_dis            :  1; /* [    9],        r/w,        0x0 */
            uint32_t bmx_gating_dis                 :  1; /* [   10],        r/w,        0x0 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t hsel_option                    :  4; /* [15:12],        r/w,        0x0 */
            uint32_t reserved_16_23                 :  8; /* [23:16],       rsvd,        0x0 */
            uint32_t hbn_apb_cfg                    :  8; /* [31:24],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } bmx_cfg1;

    /* 0x54 : bmx_cfg2 */
    union {
        struct {
            uint32_t bmx_err_addr_dis               :  1; /* [    0],        r/w,        0x0 */
            uint32_t bmx_err_clr                    :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t bmx_err_dec                    :  1; /* [    4],          r,        0x0 */
            uint32_t bmx_err_tz                     :  1; /* [    5],          r,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_w_thre_bmx                 :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t reg_w_thre_l1c                 :  2; /* [11:10],        r/w,        0x0 */
            uint32_t reserved_12_27                 : 16; /* [27:12],       rsvd,        0x0 */
            uint32_t bmx_dbg_sel                    :  4; /* [31:28],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } bmx_cfg2;

    /* 0x58 : bmx_err_addr */
    union {
        struct {
            uint32_t bmx_err_addr                   : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } bmx_err_addr;

    /* 0x5C : bmx_dbg_out */
    union {
        struct {
            uint32_t bmx_dbg_out                    : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } bmx_dbg_out;

    /* 0x60 : rsv0 */
    union {
        struct {
            uint32_t rsvd_31_0                      : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rsv0;

    /* 0x64 : rsv1 */
    union {
        struct {
            uint32_t rsvd_31_0                      : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rsv1;

    /* 0x68 : rsv2 */
    union {
        struct {
            uint32_t rsvd_31_0                      : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rsv2;

    /* 0x6C : rsv3 */
    union {
        struct {
            uint32_t rsvd_31_0                      : 32; /* [31: 0],       rsvd, 0xffffffff */
        }BF;
        uint32_t WORD;
    } rsv3;

    /* 0x70  reserved */
    uint8_t RESERVED0x70[8];

    /* 0x78 : sram_parm */
    union {
        struct {
            uint32_t reg_sram_parm                  : 18; /* [17: 0],        r/w,    0x1d00b */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t reg_ocram_parm                 : 11; /* [30:20],        r/w,        0xa */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sram_parm;

    /* 0x7C : seam_misc */
    union {
        struct {
            uint32_t em_sel                         :  4; /* [ 3: 0],        r/w,        0x3 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } seam_misc;

    /* 0x80 : glb_parm */
    union {
        struct {
            uint32_t jtag_swap_set                  :  8; /* [ 7: 0],        r/w,        0x4 */
            uint32_t cfg_sflash_swap_io0_io3        :  1; /* [    8],        r/w,        0x0 */
            uint32_t cfg_sflash_swap_cs_io2         :  1; /* [    9],        r/w,        0x0 */
            uint32_t cfg_sflash_reverse             :  1; /* [   10],        r/w,        0x0 */
            uint32_t cfg_flash2_scenario            :  1; /* [   11],        r/w,        0x0 */
            uint32_t reg_spi_0_master_mode          :  1; /* [   12],        r/w,        0x0 */
            uint32_t reg_spi_0_swap                 :  1; /* [   13],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t reg_cci_use_jtag_pin           :  1; /* [   16],        r/w,        0x1 */
            uint32_t p1_adc_test_with_cci           :  1; /* [   17],        r/w,        0x0 */
            uint32_t p2_dac_test_with_cci           :  1; /* [   18],        r/w,        0x0 */
            uint32_t p3_cci_use_io_0_2_7            :  1; /* [   19],        r/w,        0x0 */
            uint32_t p4_adc_test_with_jtag          :  1; /* [   20],        r/w,        0x0 */
            uint32_t p5_dac_test_with_jtag          :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22                    :  1; /* [   22],       rsvd,        0x0 */
            uint32_t p6_jtag_use_io_0_2_7           :  1; /* [   23],        r/w,        0x0 */
            uint32_t reserved_24_27                 :  4; /* [27:24],       rsvd,        0x0 */
            uint32_t reg_en_gpio_o_latch_mode       :  1; /* [   28],        r/w,        0x0 */
            uint32_t reg_kys_drv_val                :  1; /* [   29],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } glb_parm;

    /* 0x84 : PDM_CLK_CTRL */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } PDM_CLK_CTRL;

    /* 0x88 : GPIO_USE_PSRAM__IO */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_USE_PSRAM__IO;

    /* 0x8c  reserved */
    uint8_t RESERVED0x8c[4];

    /* 0x90 : CPU_CLK_CFG */
    union {
        struct {
            uint32_t cpu_rtc_div                    :  5; /* [ 4: 0],        r/w,        0x3 */
            uint32_t reserved_5_17                  : 13; /* [17: 5],       rsvd,        0x0 */
            uint32_t cpu_rtc_en                     :  1; /* [   18],        r/w,        0x0 */
            uint32_t cpu_rtc_sel                    :  1; /* [   19],        r/w,        0x1 */
            uint32_t debug_ndreset_gate             :  1; /* [   20],        r/w,        0x0 */
            uint32_t reserved_21_31                 : 11; /* [31:21],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } CPU_CLK_CFG;

    /* 0x94 : CLK_DESPUR_ROOT_CLK_CFG */
    union {
        struct {
            uint32_t cfg_despur_bmap_root_clk       : 28; /* [27: 0],        r/w,  0xfffffff */
            uint32_t cfg_despur_cgen_root_clk       :  1; /* [   28],        r/w,        0x0 */
            uint32_t cfg_despur_enable_root_clk     :  1; /* [   29],        r/w,        0x0 */
            uint32_t cfg_despur_clk_sel_root_clk    :  1; /* [   30],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } CLK_DESPUR_ROOT_CLK_CFG;

    /* 0x98 : CLK_DESPUR_CLK_SF_CFG */
    union {
        struct {
            uint32_t cfg_despur_bmap_clk_sf         : 28; /* [27: 0],        r/w,  0xfffffff */
            uint32_t cfg_despur_cgen_clk_sf         :  1; /* [   28],        r/w,        0x0 */
            uint32_t cfg_despur_enable_clk_sf       :  1; /* [   29],        r/w,        0x0 */
            uint32_t cfg_despur_clk_sel_clk_sf      :  1; /* [   30],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } CLK_DESPUR_CLK_SF_CFG;

    /* 0x9c  reserved */
    uint8_t RESERVED0x9c[8];

    /* 0xA4 : GPADC_32M_SRC_CTRL */
    union {
        struct {
            uint32_t gpadc_32m_clk_div              :  6; /* [ 5: 0],        r/w,        0x2 */
            uint32_t reserved_6                     :  1; /* [    6],       rsvd,        0x0 */
            uint32_t gpadc_32m_clk_sel              :  1; /* [    7],        r/w,        0x1 */
            uint32_t gpadc_32m_div_en               :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_31                  : 23; /* [31: 9],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPADC_32M_SRC_CTRL;

    /* 0xA8 : DIG32K_WAKEUP_CTRL */
    union {
        struct {
            uint32_t dig_32k_div                    : 11; /* [10: 0],        r/w,      0x3e8 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t dig_32k_en                     :  1; /* [   12],        r/w,        0x1 */
            uint32_t dig_32k_comp                   :  1; /* [   13],        r/w,        0x0 */
            uint32_t reserved_14_27                 : 14; /* [27:14],       rsvd,        0x0 */
            uint32_t dig_clk_src_sel                :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t sw_ble_wakeup_req              :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DIG32K_WAKEUP_CTRL;

    /* 0xAC : WIFI_BT_COEX_CTRL */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WIFI_BT_COEX_CTRL;

    /* 0xB0 : BZ_COEX_CTRL */
    union {
        struct {
            uint32_t coex_en                        :  1; /* [    0],        r/w,        0x0 */
            uint32_t wlan_en                        :  1; /* [    1],        r/w,        0x0 */
            uint32_t ble_rx_ignore                  :  1; /* [    2],        r/w,        0x0 */
            uint32_t m154_rx_ignore                 :  1; /* [    3],        r/w,        0x0 */
            uint32_t bz_pri_thr                     :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t bz_pri_en                      :  1; /* [    8],        r/w,        0x1 */
            uint32_t bz_pri_pol                     :  1; /* [    9],        r/w,        0x1 */
            uint32_t bz_active_pol                  :  1; /* [   10],        r/w,        0x1 */
            uint32_t bz_abort_pol                   :  1; /* [   11],        r/w,        0x1 */
            uint32_t coex_pri                       :  1; /* [   12],        r/w,        0x0 */
            uint32_t force_m154_win                 :  1; /* [   13],        r/w,        0x0 */
            uint32_t force_ble_win                  :  1; /* [   14],        r/w,        0x0 */
            uint32_t coex_option                    :  1; /* [   15],        r/w,        0x0 */
            uint32_t coex_force_ch                  :  7; /* [22:16],        r/w,        0x4 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t m154_rx_abort_dis              :  1; /* [   24],        r/w,        0x0 */
            uint32_t m154_tx_abort_dis              :  1; /* [   25],        r/w,        0x0 */
            uint32_t ble_rx_abort_dis               :  1; /* [   26],        r/w,        0x0 */
            uint32_t ble_tx_abort_dis               :  1; /* [   27],        r/w,        0x0 */
            uint32_t coex_arb                       :  4; /* [31:28],          r,        0x8 */
        }BF;
        uint32_t WORD;
    } BZ_COEX_CTRL;

    /* 0xb4  reserved */
    uint8_t RESERVED0xb4[12];

    /* 0xC0 : UART_SIG_SEL_0 */
    union {
        struct {
            uint32_t uart_sig_0_sel                 :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t uart_sig_1_sel                 :  4; /* [ 7: 4],        r/w,        0x1 */
            uint32_t uart_sig_2_sel                 :  4; /* [11: 8],        r/w,        0x2 */
            uint32_t uart_sig_3_sel                 :  4; /* [15:12],        r/w,        0x3 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } UART_SIG_SEL_0;

    /* 0xc4  reserved */
    uint8_t RESERVED0xc4[12];

    /* 0xD0 : DBG_SEL_LL */
    union {
        struct {
            uint32_t reg_dbg_ll_ctrl                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DBG_SEL_LL;

    /* 0xD4 : DBG_SEL_LH */
    union {
        struct {
            uint32_t reg_dbg_lh_ctrl                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DBG_SEL_LH;

    /* 0xD8 : DBG_SEL_HL */
    union {
        struct {
            uint32_t reg_dbg_hl_ctrl                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DBG_SEL_HL;

    /* 0xDC : DBG_SEL_HH */
    union {
        struct {
            uint32_t reg_dbg_hh_ctrl                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DBG_SEL_HH;

    /* 0xE0 : debug */
    union {
        struct {
            uint32_t debug_oe                       :  1; /* [    0],        r/w,        0x0 */
            uint32_t debug_i                        : 31; /* [31: 1],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } debug;

    /* 0xe4  reserved */
    uint8_t RESERVED0xe4[28];

    /* 0x100 : GPIO_CFGCTL0 */
    union {
        struct {
            uint32_t reg_gpio_0_ie                  :  1; /* [    0],        r/w,        0x1 */
            uint32_t reg_gpio_0_smt                 :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_0_drv                 :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_0_pu                  :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_0_pd                  :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_0_func_sel            :  5; /* [12: 8],        r/w,        0xf */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_0_mode                :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_1_ie                  :  1; /* [   16],        r/w,        0x1 */
            uint32_t reg_gpio_1_smt                 :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_1_drv                 :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_1_pu                  :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_1_pd                  :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_1_func_sel            :  5; /* [28:24],        r/w,        0xf */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_1_mode                :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL0;

    /* 0x104 : GPIO_CFGCTL1 */
    union {
        struct {
            uint32_t reg_gpio_2_ie                  :  1; /* [    0],        r/w,        0x1 */
            uint32_t reg_gpio_2_smt                 :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_2_drv                 :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_2_pu                  :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_2_pd                  :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_2_func_sel            :  5; /* [12: 8],        r/w,        0xf */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_2_mode                :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_3_ie                  :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_3_smt                 :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_3_drv                 :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_3_pu                  :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_3_pd                  :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_3_func_sel            :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_3_mode                :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL1;

    /* 0x108 : GPIO_CFGCTL2 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL2;

    /* 0x10C : GPIO_CFGCTL3 */
    union {
        struct {
            uint32_t reserved_0_15                  : 16; /* [15: 0],       rsvd,        0x0 */
            uint32_t reg_gpio_7_ie                  :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_7_smt                 :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_7_drv                 :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_7_pu                  :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_7_pd                  :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_7_func_sel            :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_7_mode                :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL3;

    /* 0x110 : GPIO_CFGCTL4 */
    union {
        struct {
            uint32_t reg_gpio_8_ie                  :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_8_smt                 :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_8_drv                 :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_8_pu                  :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_8_pd                  :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_8_func_sel            :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_8_mode                :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_9_ie                  :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_9_smt                 :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_9_drv                 :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_9_pu                  :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_9_pd                  :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_9_func_sel            :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_9_mode                :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL4;

    /* 0x114 : GPIO_CFGCTL5 */
    union {
        struct {
            uint32_t reg_gpio_10_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_10_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_10_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_10_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_10_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_10_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_10_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_11_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_11_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_11_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_11_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_11_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_11_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_11_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL5;

    /* 0x118 : GPIO_CFGCTL6 */
    union {
        struct {
            uint32_t reg_gpio_12_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_12_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_12_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_12_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_12_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_12_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_12_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_13_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_13_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_13_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_13_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_13_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_13_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_13_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL6;

    /* 0x11C : GPIO_CFGCTL7 */
    union {
        struct {
            uint32_t reg_gpio_14_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_14_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_14_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_14_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_14_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_14_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_14_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_15_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_15_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_15_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_15_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_15_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_15_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_15_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL7;

    /* 0x120 : GPIO_CFGCTL8 */
    union {
        struct {
            uint32_t reg_gpio_16_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_16_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_16_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_16_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_16_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_16_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_16_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_17_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_17_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_17_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_17_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_17_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_17_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_17_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL8;

    /* 0x124 : GPIO_CFGCTL9 */
    union {
        struct {
            uint32_t reg_gpio_18_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_18_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_18_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_18_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_18_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_18_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_18_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_19_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_19_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_19_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_19_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_19_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_19_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_19_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL9;

    /* 0x128 : GPIO_CFGCTL10 */
    union {
        struct {
            uint32_t reg_gpio_20_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_20_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_20_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_20_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_20_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_20_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_20_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_21_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_21_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_21_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_21_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_21_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_21_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_21_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL10;

    /* 0x12C : GPIO_CFGCTL11 */
    union {
        struct {
            uint32_t reg_gpio_22_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_22_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_22_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_22_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_22_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_22_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_22_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_23_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_23_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_23_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_23_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_23_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_23_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_23_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL11;

    /* 0x130 : GPIO_CFGCTL12 */
    union {
        struct {
            uint32_t reg_gpio_24_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_24_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_24_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_24_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_24_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_24_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_24_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_25_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_25_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_25_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_25_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_25_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_25_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_25_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL12;

    /* 0x134 : GPIO_CFGCTL13 */
    union {
        struct {
            uint32_t reg_gpio_26_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_26_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_26_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_26_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_26_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_26_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_26_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_27_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_27_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_27_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_27_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_27_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_27_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_27_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL13;

    /* 0x138 : GPIO_CFGCTL14 */
    union {
        struct {
            uint32_t reg_gpio_28_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_28_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_28_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_28_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_28_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_28_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_28_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_29_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_29_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_29_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_29_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_29_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_29_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_29_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL14;

    /* 0x13C : GPIO_CFGCTL15 */
    union {
        struct {
            uint32_t reg_gpio_30_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_30_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_30_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_30_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_30_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_30_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_30_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_31_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_31_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_31_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_31_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_31_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_31_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_31_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL15;

    /* 0x140 : GPIO_CFGCTL16 */
    union {
        struct {
            uint32_t reg_gpio_32_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_32_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_32_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_32_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_32_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_15                  : 10; /* [15: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_33_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_33_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_33_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_33_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_33_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL16;

    /* 0x144 : GPIO_CFGCTL17 */
    union {
        struct {
            uint32_t reg_gpio_34_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_34_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_34_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_34_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_34_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_15                  : 10; /* [15: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_35_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_35_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_35_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_35_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_35_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL17;

    /* 0x148 : GPIO_CFGCTL18 */
    union {
        struct {
            uint32_t reg_gpio_36_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_36_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_36_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_36_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_36_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_15                  : 10; /* [15: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_37_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_37_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_37_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_37_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_37_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL18;

    /* 0x14c  reserved */
    uint8_t RESERVED0x14c[52];

    /* 0x180 : GPIO_CFGCTL30 */
    union {
        struct {
            uint32_t reg_gpio_0_i                   :  1; /* [    0],          r,        0x0 */
            uint32_t reg_gpio_1_i                   :  1; /* [    1],          r,        0x0 */
            uint32_t reg_gpio_2_i                   :  1; /* [    2],          r,        0x0 */
            uint32_t reg_gpio_3_i                   :  1; /* [    3],          r,        0x0 */
            uint32_t reserved_4_6                   :  3; /* [ 6: 4],       rsvd,        0x0 */
            uint32_t reg_gpio_7_i                   :  1; /* [    7],          r,        0x0 */
            uint32_t reg_gpio_8_i                   :  1; /* [    8],          r,        0x0 */
            uint32_t reg_gpio_9_i                   :  1; /* [    9],          r,        0x0 */
            uint32_t reg_gpio_10_i                  :  1; /* [   10],          r,        0x0 */
            uint32_t reg_gpio_11_i                  :  1; /* [   11],          r,        0x0 */
            uint32_t reg_gpio_12_i                  :  1; /* [   12],          r,        0x0 */
            uint32_t reg_gpio_13_i                  :  1; /* [   13],          r,        0x0 */
            uint32_t reg_gpio_14_i                  :  1; /* [   14],          r,        0x0 */
            uint32_t reg_gpio_15_i                  :  1; /* [   15],          r,        0x0 */
            uint32_t reg_gpio_16_i                  :  1; /* [   16],          r,        0x0 */
            uint32_t reg_gpio_17_i                  :  1; /* [   17],          r,        0x0 */
            uint32_t reg_gpio_18_i                  :  1; /* [   18],          r,        0x0 */
            uint32_t reg_gpio_19_i                  :  1; /* [   19],          r,        0x0 */
            uint32_t reg_gpio_20_i                  :  1; /* [   20],          r,        0x0 */
            uint32_t reg_gpio_21_i                  :  1; /* [   21],          r,        0x0 */
            uint32_t reg_gpio_22_i                  :  1; /* [   22],          r,        0x0 */
            uint32_t reg_gpio_23_i                  :  1; /* [   23],          r,        0x0 */
            uint32_t reg_gpio_24_i                  :  1; /* [   24],          r,        0x0 */
            uint32_t reg_gpio_25_i                  :  1; /* [   25],          r,        0x0 */
            uint32_t reg_gpio_26_i                  :  1; /* [   26],          r,        0x0 */
            uint32_t reg_gpio_27_i                  :  1; /* [   27],          r,        0x0 */
            uint32_t reg_gpio_28_i                  :  1; /* [   28],          r,        0x0 */
            uint32_t reg_gpio_29_i                  :  1; /* [   29],          r,        0x0 */
            uint32_t reg_gpio_30_i                  :  1; /* [   30],          r,        0x0 */
            uint32_t reg_gpio_31_i                  :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL30;

    /* 0x184 : GPIO_CFGCTL31 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL31;

    /* 0x188 : GPIO_CFGCTL32 */
    union {
        struct {
            uint32_t reg_gpio_0_o                   :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_1_o                   :  1; /* [    1],        r/w,        0x0 */
            uint32_t reg_gpio_2_o                   :  1; /* [    2],        r/w,        0x0 */
            uint32_t reg_gpio_3_o                   :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_6                   :  3; /* [ 6: 4],       rsvd,        0x0 */
            uint32_t reg_gpio_7_o                   :  1; /* [    7],        r/w,        0x0 */
            uint32_t reg_gpio_8_o                   :  1; /* [    8],        r/w,        0x0 */
            uint32_t reg_gpio_9_o                   :  1; /* [    9],        r/w,        0x0 */
            uint32_t reg_gpio_10_o                  :  1; /* [   10],        r/w,        0x0 */
            uint32_t reg_gpio_11_o                  :  1; /* [   11],        r/w,        0x0 */
            uint32_t reg_gpio_12_o                  :  1; /* [   12],        r/w,        0x0 */
            uint32_t reg_gpio_13_o                  :  1; /* [   13],        r/w,        0x0 */
            uint32_t reg_gpio_14_o                  :  1; /* [   14],        r/w,        0x0 */
            uint32_t reg_gpio_15_o                  :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_16_o                  :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_17_o                  :  1; /* [   17],        r/w,        0x0 */
            uint32_t reg_gpio_18_o                  :  1; /* [   18],        r/w,        0x0 */
            uint32_t reg_gpio_19_o                  :  1; /* [   19],        r/w,        0x0 */
            uint32_t reg_gpio_20_o                  :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_21_o                  :  1; /* [   21],        r/w,        0x0 */
            uint32_t reg_gpio_22_o                  :  1; /* [   22],        r/w,        0x0 */
            uint32_t reg_gpio_23_o                  :  1; /* [   23],        r/w,        0x0 */
            uint32_t reg_gpio_24_o                  :  1; /* [   24],        r/w,        0x0 */
            uint32_t reg_gpio_25_o                  :  1; /* [   25],        r/w,        0x0 */
            uint32_t reg_gpio_26_o                  :  1; /* [   26],        r/w,        0x0 */
            uint32_t reg_gpio_27_o                  :  1; /* [   27],        r/w,        0x0 */
            uint32_t reg_gpio_28_o                  :  1; /* [   28],        r/w,        0x0 */
            uint32_t reg_gpio_29_o                  :  1; /* [   29],        r/w,        0x0 */
            uint32_t reg_gpio_30_o                  :  1; /* [   30],        r/w,        0x0 */
            uint32_t reg_gpio_31_o                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL32;

    /* 0x18C : GPIO_CFGCTL33 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL33;

    /* 0x190 : GPIO_CFGCTL34 */
    union {
        struct {
            uint32_t reg_gpio_0_oe                  :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_1_oe                  :  1; /* [    1],        r/w,        0x0 */
            uint32_t reg_gpio_2_oe                  :  1; /* [    2],        r/w,        0x0 */
            uint32_t reg_gpio_3_oe                  :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_6                   :  3; /* [ 6: 4],       rsvd,        0x0 */
            uint32_t reg_gpio_7_oe                  :  1; /* [    7],        r/w,        0x0 */
            uint32_t reg_gpio_8_oe                  :  1; /* [    8],        r/w,        0x0 */
            uint32_t reg_gpio_9_oe                  :  1; /* [    9],        r/w,        0x0 */
            uint32_t reg_gpio_10_oe                 :  1; /* [   10],        r/w,        0x0 */
            uint32_t reg_gpio_11_oe                 :  1; /* [   11],        r/w,        0x0 */
            uint32_t reg_gpio_12_oe                 :  1; /* [   12],        r/w,        0x0 */
            uint32_t reg_gpio_13_oe                 :  1; /* [   13],        r/w,        0x0 */
            uint32_t reg_gpio_14_oe                 :  1; /* [   14],        r/w,        0x0 */
            uint32_t reg_gpio_15_oe                 :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_16_oe                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_17_oe                 :  1; /* [   17],        r/w,        0x0 */
            uint32_t reg_gpio_18_oe                 :  1; /* [   18],        r/w,        0x0 */
            uint32_t reg_gpio_19_oe                 :  1; /* [   19],        r/w,        0x0 */
            uint32_t reg_gpio_20_oe                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_21_oe                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reg_gpio_22_oe                 :  1; /* [   22],        r/w,        0x0 */
            uint32_t reg_gpio_23_oe                 :  1; /* [   23],        r/w,        0x0 */
            uint32_t reg_gpio_24_oe                 :  1; /* [   24],        r/w,        0x0 */
            uint32_t reg_gpio_25_oe                 :  1; /* [   25],        r/w,        0x0 */
            uint32_t reg_gpio_26_oe                 :  1; /* [   26],        r/w,        0x0 */
            uint32_t reg_gpio_27_oe                 :  1; /* [   27],        r/w,        0x0 */
            uint32_t reg_gpio_28_oe                 :  1; /* [   28],        r/w,        0x0 */
            uint32_t reg_gpio_29_oe                 :  1; /* [   29],        r/w,        0x0 */
            uint32_t reg_gpio_30_oe                 :  1; /* [   30],        r/w,        0x0 */
            uint32_t reg_gpio_31_oe                 :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL34;

    /* 0x194 : GPIO_CFGCTL35 */
    union {
        struct {
            uint32_t reg_gpio_0_set                 :  1; /* [    0],        w1p,        0x0 */
            uint32_t reg_gpio_1_set                 :  1; /* [    1],        w1p,        0x0 */
            uint32_t reg_gpio_2_set                 :  1; /* [    2],        w1p,        0x0 */
            uint32_t reg_gpio_3_set                 :  1; /* [    3],        w1p,        0x0 */
            uint32_t reserved_4_6                   :  3; /* [ 6: 4],       rsvd,        0x0 */
            uint32_t reg_gpio_7_set                 :  1; /* [    7],        w1p,        0x0 */
            uint32_t reg_gpio_8_set                 :  1; /* [    8],        w1p,        0x0 */
            uint32_t reg_gpio_9_set                 :  1; /* [    9],        w1p,        0x0 */
            uint32_t reg_gpio_10_set                :  1; /* [   10],        w1p,        0x0 */
            uint32_t reg_gpio_11_set                :  1; /* [   11],        w1p,        0x0 */
            uint32_t reg_gpio_12_set                :  1; /* [   12],        w1p,        0x0 */
            uint32_t reg_gpio_13_set                :  1; /* [   13],        w1p,        0x0 */
            uint32_t reg_gpio_14_set                :  1; /* [   14],        w1p,        0x0 */
            uint32_t reg_gpio_15_set                :  1; /* [   15],        w1p,        0x0 */
            uint32_t reg_gpio_16_set                :  1; /* [   16],        w1p,        0x0 */
            uint32_t reg_gpio_17_set                :  1; /* [   17],        w1p,        0x0 */
            uint32_t reg_gpio_18_set                :  1; /* [   18],        w1p,        0x0 */
            uint32_t reg_gpio_19_set                :  1; /* [   19],        w1p,        0x0 */
            uint32_t reg_gpio_20_set                :  1; /* [   20],        w1p,        0x0 */
            uint32_t reg_gpio_21_set                :  1; /* [   21],        w1p,        0x0 */
            uint32_t reg_gpio_22_set                :  1; /* [   22],        w1p,        0x0 */
            uint32_t reg_gpio_23_set                :  1; /* [   23],        w1p,        0x0 */
            uint32_t reg_gpio_24_set                :  1; /* [   24],        w1p,        0x0 */
            uint32_t reg_gpio_25_set                :  1; /* [   25],        w1p,        0x0 */
            uint32_t reg_gpio_26_set                :  1; /* [   26],        w1p,        0x0 */
            uint32_t reg_gpio_27_set                :  1; /* [   27],        w1p,        0x0 */
            uint32_t reg_gpio_28_set                :  1; /* [   28],        w1p,        0x0 */
            uint32_t reg_gpio_29_set                :  1; /* [   29],        w1p,        0x0 */
            uint32_t reg_gpio_30_set                :  1; /* [   30],        w1p,        0x0 */
            uint32_t reg_gpio_31_set                :  1; /* [   31],        w1p,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL35;

    /* 0x198 : GPIO_CFGCTL36 */
    union {
        struct {
            uint32_t reg_gpio_0_clr                 :  1; /* [    0],        w1p,        0x0 */
            uint32_t reg_gpio_1_clr                 :  1; /* [    1],        w1p,        0x0 */
            uint32_t reg_gpio_2_clr                 :  1; /* [    2],        w1p,        0x0 */
            uint32_t reg_gpio_3_clr                 :  1; /* [    3],        w1p,        0x0 */
            uint32_t reserved_4_6                   :  3; /* [ 6: 4],       rsvd,        0x0 */
            uint32_t reg_gpio_7_clr                 :  1; /* [    7],        w1p,        0x0 */
            uint32_t reg_gpio_8_clr                 :  1; /* [    8],        w1p,        0x0 */
            uint32_t reg_gpio_9_clr                 :  1; /* [    9],        w1p,        0x0 */
            uint32_t reg_gpio_10_clr                :  1; /* [   10],        w1p,        0x0 */
            uint32_t reg_gpio_11_clr                :  1; /* [   11],        w1p,        0x0 */
            uint32_t reg_gpio_12_clr                :  1; /* [   12],        w1p,        0x0 */
            uint32_t reg_gpio_13_clr                :  1; /* [   13],        w1p,        0x0 */
            uint32_t reg_gpio_14_clr                :  1; /* [   14],        w1p,        0x0 */
            uint32_t reg_gpio_15_clr                :  1; /* [   15],        w1p,        0x0 */
            uint32_t reg_gpio_16_clr                :  1; /* [   16],        w1p,        0x0 */
            uint32_t reg_gpio_17_clr                :  1; /* [   17],        w1p,        0x0 */
            uint32_t reg_gpio_18_clr                :  1; /* [   18],        w1p,        0x0 */
            uint32_t reg_gpio_19_clr                :  1; /* [   19],        w1p,        0x0 */
            uint32_t reg_gpio_20_clr                :  1; /* [   20],        w1p,        0x0 */
            uint32_t reg_gpio_21_clr                :  1; /* [   21],        w1p,        0x0 */
            uint32_t reg_gpio_22_clr                :  1; /* [   22],        w1p,        0x0 */
            uint32_t reg_gpio_23_clr                :  1; /* [   23],        w1p,        0x0 */
            uint32_t reg_gpio_24_clr                :  1; /* [   24],        w1p,        0x0 */
            uint32_t reg_gpio_25_clr                :  1; /* [   25],        w1p,        0x0 */
            uint32_t reg_gpio_26_clr                :  1; /* [   26],        w1p,        0x0 */
            uint32_t reg_gpio_27_clr                :  1; /* [   27],        w1p,        0x0 */
            uint32_t reg_gpio_28_clr                :  1; /* [   28],        w1p,        0x0 */
            uint32_t reg_gpio_29_clr                :  1; /* [   29],        w1p,        0x0 */
            uint32_t reg_gpio_30_clr                :  1; /* [   30],        w1p,        0x0 */
            uint32_t reg_gpio_31_clr                :  1; /* [   31],        w1p,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL36;

    /* 0x19c  reserved */
    uint8_t RESERVED0x19c[4];

    /* 0x1A0 : GPIO_INT_MASK1 */
    union {
        struct {
            uint32_t reg_gpio0_int_mask             :  1; /* [    0],        r/w,        0x1 */
            uint32_t reg_gpio1_int_mask             :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio2_int_mask             :  1; /* [    2],        r/w,        0x1 */
            uint32_t reg_gpio3_int_mask             :  1; /* [    3],        r/w,        0x1 */
            uint32_t reserved_4_6                   :  3; /* [ 6: 4],       rsvd,        0x0 */
            uint32_t reg_gpio7_int_mask             :  1; /* [    7],        r/w,        0x1 */
            uint32_t reg_gpio8_int_mask             :  1; /* [    8],        r/w,        0x1 */
            uint32_t reg_gpio9_int_mask             :  1; /* [    9],        r/w,        0x1 */
            uint32_t reg_gpio10_int_mask            :  1; /* [   10],        r/w,        0x1 */
            uint32_t reg_gpio11_int_mask            :  1; /* [   11],        r/w,        0x1 */
            uint32_t reg_gpio12_int_mask            :  1; /* [   12],        r/w,        0x1 */
            uint32_t reg_gpio13_int_mask            :  1; /* [   13],        r/w,        0x1 */
            uint32_t reg_gpio14_int_mask            :  1; /* [   14],        r/w,        0x1 */
            uint32_t reg_gpio15_int_mask            :  1; /* [   15],        r/w,        0x1 */
            uint32_t reg_gpio16_int_mask            :  1; /* [   16],        r/w,        0x1 */
            uint32_t reg_gpio17_int_mask            :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio18_int_mask            :  1; /* [   18],        r/w,        0x1 */
            uint32_t reg_gpio19_int_mask            :  1; /* [   19],        r/w,        0x1 */
            uint32_t reg_gpio20_int_mask            :  1; /* [   20],        r/w,        0x1 */
            uint32_t reg_gpio21_int_mask            :  1; /* [   21],        r/w,        0x1 */
            uint32_t reg_gpio22_int_mask            :  1; /* [   22],        r/w,        0x1 */
            uint32_t reg_gpio23_int_mask            :  1; /* [   23],        r/w,        0x1 */
            uint32_t reg_gpio24_int_mask            :  1; /* [   24],        r/w,        0x1 */
            uint32_t reg_gpio25_int_mask            :  1; /* [   25],        r/w,        0x1 */
            uint32_t reg_gpio26_int_mask            :  1; /* [   26],        r/w,        0x1 */
            uint32_t reg_gpio27_int_mask            :  1; /* [   27],        r/w,        0x1 */
            uint32_t reg_gpio28_int_mask            :  1; /* [   28],        r/w,        0x1 */
            uint32_t reg_gpio29_int_mask            :  1; /* [   29],        r/w,        0x1 */
            uint32_t reg_gpio30_int_mask            :  1; /* [   30],        r/w,        0x1 */
            uint32_t reg_gpio31_int_mask            :  1; /* [   31],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } GPIO_INT_MASK1;

    /* 0x1a4  reserved */
    uint8_t RESERVED0x1a4[4];

    /* 0x1A8 : GPIO_INT_STAT1 */
    union {
        struct {
            uint32_t gpio_int_stat1                 : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_INT_STAT1;

    /* 0x1ac  reserved */
    uint8_t RESERVED0x1ac[4];

    /* 0x1B0 : GPIO_INT_CLR1 */
    union {
        struct {
            uint32_t reg_gpio0_int_clr              :  1; /* [    0],        w1p,        0x0 */
            uint32_t reg_gpio1_int_clr              :  1; /* [    1],        w1p,        0x0 */
            uint32_t reg_gpio2_int_clr              :  1; /* [    2],        w1p,        0x0 */
            uint32_t reg_gpio3_int_clr              :  1; /* [    3],        w1p,        0x0 */
            uint32_t reserved_4_6                   :  3; /* [ 6: 4],       rsvd,        0x0 */
            uint32_t reg_gpio7_int_clr              :  1; /* [    7],        w1p,        0x0 */
            uint32_t reg_gpio8_int_clr              :  1; /* [    8],        w1p,        0x0 */
            uint32_t reg_gpio9_int_clr              :  1; /* [    9],        w1p,        0x0 */
            uint32_t reg_gpio10_int_clr             :  1; /* [   10],        w1p,        0x0 */
            uint32_t reg_gpio11_int_clr             :  1; /* [   11],        w1p,        0x0 */
            uint32_t reg_gpio12_int_clr             :  1; /* [   12],        w1p,        0x0 */
            uint32_t reg_gpio13_int_clr             :  1; /* [   13],        w1p,        0x0 */
            uint32_t reg_gpio14_int_clr             :  1; /* [   14],        w1p,        0x0 */
            uint32_t reg_gpio15_int_clr             :  1; /* [   15],        w1p,        0x0 */
            uint32_t reg_gpio16_int_clr             :  1; /* [   16],        w1p,        0x0 */
            uint32_t reg_gpio17_int_clr             :  1; /* [   17],        w1p,        0x0 */
            uint32_t reg_gpio18_int_clr             :  1; /* [   18],        w1p,        0x0 */
            uint32_t reg_gpio19_int_clr             :  1; /* [   19],        w1p,        0x0 */
            uint32_t reg_gpio20_int_clr             :  1; /* [   20],        w1p,        0x0 */
            uint32_t reg_gpio21_int_clr             :  1; /* [   21],        w1p,        0x0 */
            uint32_t reg_gpio22_int_clr             :  1; /* [   22],        w1p,        0x0 */
            uint32_t reg_gpio23_int_clr             :  1; /* [   23],        w1p,        0x0 */
            uint32_t reg_gpio24_int_clr             :  1; /* [   24],        w1p,        0x0 */
            uint32_t reg_gpio25_int_clr             :  1; /* [   25],        w1p,        0x0 */
            uint32_t reg_gpio26_int_clr             :  1; /* [   26],        w1p,        0x0 */
            uint32_t reg_gpio27_int_clr             :  1; /* [   27],        w1p,        0x0 */
            uint32_t reg_gpio28_int_clr             :  1; /* [   28],        w1p,        0x0 */
            uint32_t reg_gpio29_int_clr             :  1; /* [   29],        w1p,        0x0 */
            uint32_t reg_gpio30_int_clr             :  1; /* [   30],        w1p,        0x0 */
            uint32_t reg_gpio31_int_clr             :  1; /* [   31],        w1p,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_INT_CLR1;

    /* 0x1b4  reserved */
    uint8_t RESERVED0x1b4[12];

    /* 0x1C0 : GPIO_INT_MODE_SET1 */
    union {
        struct {
            uint32_t reg_gpio0_int_mode_set         :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t reg_gpio1_int_mode_set         :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t reg_gpio2_int_mode_set         :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t reg_gpio3_int_mode_set         :  4; /* [15:12],        r/w,        0x0 */
            uint32_t reserved_16_27                 : 12; /* [27:16],       rsvd,        0x0 */
            uint32_t reg_gpio7_int_mode_set         :  4; /* [31:28],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_INT_MODE_SET1;

    /* 0x1C4 : GPIO_INT_MODE_SET2 */
    union {
        struct {
            uint32_t reg_gpio_int_mode_set2         : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_INT_MODE_SET2;

    /* 0x1C8 : GPIO_INT_MODE_SET3 */
    union {
        struct {
            uint32_t reg_gpio_int_mode_set3         : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_INT_MODE_SET3;

    /* 0x1CC : GPIO_INT_MODE_SET4 */
    union {
        struct {
            uint32_t reg_gpio_int_mode_set4         : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_INT_MODE_SET4;

    /* 0x1d0  reserved */
    uint8_t RESERVED0x1d0[48];

    /* 0x200 : dll */
    union {
        struct {
            uint32_t reserved_0_1                   :  2; /* [ 1: 0],       rsvd,        0x0 */
            uint32_t ten_dll                        :  1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3_7                   :  5; /* [ 7: 3],       rsvd,        0x0 */
            uint32_t dll_vctrl_sel                  :  3; /* [10: 8],        r/w,        0x4 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t dll_prechg_sel                 :  1; /* [   12],        r/w,        0x1 */
            uint32_t dll_prechg_reg                 :  1; /* [   13],        r/w,        0x1 */
            uint32_t dll_prechg_en                  :  1; /* [   14],        r/w,        0x1 */
            uint32_t dll_vctrl_force_en             :  1; /* [   15],        r/w,        0x0 */
            uint32_t dll_postdiv                    :  6; /* [21:16],        r/w,       0x15 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t dll_delay_sel                  :  2; /* [25:24],        r/w,        0x1 */
            uint32_t dll_cp_op_en                   :  1; /* [   26],        r/w,        0x1 */
            uint32_t dll_cp_hiz                     :  1; /* [   27],        r/w,        0x0 */
            uint32_t dll_refclk_sel                 :  1; /* [   28],        r/w,        0x0 */
            uint32_t dll_reset                      :  1; /* [   29],        r/w,        0x1 */
            uint32_t pu_dll                         :  1; /* [   30],        r/w,        0x0 */
            uint32_t ppu_dll                        :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dll;

    /* 0x204 : dll2 */
    union {
        struct {
            uint32_t dll_en_div1_rf                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t dll_en_div63                   :  1; /* [    1],        r/w,        0x0 */
            uint32_t dll_en_div21                   :  1; /* [    2],        r/w,        0x0 */
            uint32_t dll_en_div5                    :  1; /* [    3],        r/w,        0x0 */
            uint32_t dll_en_div3                    :  1; /* [    4],        r/w,        0x0 */
            uint32_t dll_en_div2                    :  1; /* [    5],        r/w,        0x0 */
            uint32_t dll_en_div1                    :  1; /* [    6],        r/w,        0x1 */
            uint32_t dll_dl_ctrl_div1_rf            :  1; /* [    7],        r/w,        0x0 */
            uint32_t dll_dl_ctrl_div5               :  1; /* [    8],        r/w,        0x0 */
            uint32_t dll_dl_ctrl_div3               :  1; /* [    9],        r/w,        0x0 */
            uint32_t dll_dl_ctrl_div2               :  1; /* [   10],        r/w,        0x0 */
            uint32_t dll_dl_ctrl_div1               :  1; /* [   11],        r/w,        0x0 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t dll_postdiv_dl_ctrl            :  1; /* [   16],        r/w,        0x0 */
            uint32_t dll_postdiv_stg2_div2          :  1; /* [   17],        r/w,        0x0 */
            uint32_t dll_postdiv_en                 :  1; /* [   18],        r/w,        0x0 */
            uint32_t dll_ref_polarity               :  1; /* [   19],        r/w,        0x0 */
            uint32_t dten_dll_refbuf                :  1; /* [   20],        r/w,        0x0 */
            uint32_t dten_dll_postdiv               :  1; /* [   21],        r/w,        0x0 */
            uint32_t dten_dll_clkdiv63              :  1; /* [   22],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t dll_resv                       :  8; /* [31:24],        r/w,        0x3 */
        }BF;
        uint32_t WORD;
    } dll2;

    /* 0x208  reserved */
    uint8_t RESERVED0x208[28];

    /* 0x224 : led_driver */
    union {
        struct {
            uint32_t led_din_reg                    :  1; /* [    0],        r/w,        0x0 */
            uint32_t led_din_sel                    :  1; /* [    1],        r/w,        0x0 */
            uint32_t led_din_polarity_sel           :  1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t leddrv_ibias                   :  3; /* [ 6: 4],        r/w,        0x4 */
            uint32_t reserved_7_27                  : 21; /* [27: 7],       rsvd,        0x0 */
            uint32_t leddrv_out_en                  :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t pu_leddrv                      :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } led_driver;

    /* 0x228 : usb_xcvr */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } usb_xcvr;

    /* 0x22C : usb_xcvr_config */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } usb_xcvr_config;

    /* 0x230  reserved */
    uint8_t RESERVED0x230[208];

    /* 0x300 : rc32m_ctrl0 */
    union {
        struct {
            uint32_t rc32m_cal_done                 :  1; /* [    0],          r,        0x0 */
            uint32_t rc32m_rdy                      :  1; /* [    1],          r,        0x0 */
            uint32_t rc32m_cal_inprogress           :  1; /* [    2],          r,        0x0 */
            uint32_t rc32m_cal_div                  :  2; /* [ 4: 3],        r/w,        0x3 */
            uint32_t rc32m_cal_precharge            :  1; /* [    5],          r,        0x0 */
            uint32_t rc32m_dig_code_fr_cal          :  8; /* [13: 6],          r,        0x0 */
            uint32_t reserved_14_16                 :  3; /* [16:14],       rsvd,        0x0 */
            uint32_t rc32m_allow_cal                :  1; /* [   17],        r/w,        0x0 */
            uint32_t reserved_18                    :  1; /* [   18],       rsvd,        0x0 */
            uint32_t rc32m_ext_code_en              :  1; /* [   19],        r/w,        0x1 */
            uint32_t rc32m_cal_en                   :  1; /* [   20],        r/w,        0x0 */
            uint32_t rc32m_pd                       :  1; /* [   21],        r/w,        0x0 */
            uint32_t rc32m_code_fr_ext              :  8; /* [29:22],        r/w,       0x60 */
            uint32_t rc32m_rc32k_cal_cgen           :  1; /* [   30],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rc32m_ctrl0;

    /* 0x304 : rc32m_ctrl1 */
    union {
        struct {
            uint32_t rc32m_test_en                  :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_7                   :  7; /* [ 7: 1],       rsvd,        0x0 */
            uint32_t rc32m_code_fr_ext2             :  8; /* [15: 8],        r/w,       0x60 */
            uint32_t rc32m_ext_code_sel             :  1; /* [   16],        r/w,        0x0 */
            uint32_t rc32m_ext_code_sel_cgen        :  1; /* [   17],        r/w,        0x0 */
            uint32_t reserved_18_23                 :  6; /* [23:18],       rsvd,        0x0 */
            uint32_t rc32m_reserved                 :  8; /* [31:24],        r/w,        0xf */
        }BF;
        uint32_t WORD;
    } rc32m_ctrl1;

    /* 0x308 : rc32k_ctrl2 */
    union {
        struct {
            uint32_t rc32k_dig_cal_done             :  1; /* [    0],          r,        0x1 */
            uint32_t rc32k_rdy                      :  1; /* [    1],          r,        0x1 */
            uint32_t rc32k_dig_cal_inprogress       :  1; /* [    2],          r,        0x0 */
            uint32_t rc32k_dig_cal_div              :  2; /* [ 4: 3],        r/w,        0x3 */
            uint32_t rc32k_dig_cal_precharge        :  1; /* [    5],          r,        0x0 */
            uint32_t rc32k_dig_cap_code_fr_cal      :  4; /* [ 9: 6],          r,        0x8 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t rc32k_dig_vref_dly             :  2; /* [17:16],        r/w,        0x0 */
            uint32_t rc32k_allow_cal                :  1; /* [   18],        r/w,        0x0 */
            uint32_t reserved_19_31                 : 13; /* [31:19],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rc32k_ctrl2;

    /* 0x30c  reserved */
    uint8_t RESERVED0x30c[512];

    /* 0x50C : gpadc_reg_cmd */
    union {
        struct {
            uint32_t gpadc_global_en                :  1; /* [    0],        r/w,        0x0 */
            uint32_t gpadc_conv_start               :  1; /* [    1],        r/w,        0x0 */
            uint32_t gpadc_soft_rst                 :  1; /* [    2],        r/w,        0x0 */
            uint32_t gpadc_neg_sel                  :  5; /* [ 7: 3],        r/w,        0xf */
            uint32_t gpadc_pos_sel                  :  5; /* [12: 8],        r/w,        0xf */
            uint32_t gpadc_neg_gnd                  :  1; /* [   13],        r/w,        0x0 */
            uint32_t gpadc_micbias_en               :  1; /* [   14],        r/w,        0x0 */
            uint32_t gpadc_micpga_en                :  1; /* [   15],        r/w,        0x0 */
            uint32_t gpadc_byp_micboost             :  1; /* [   16],        r/w,        0x0 */
            uint32_t gpadc_rcal_en                  :  1; /* [   17],        r/w,        0x0 */
            uint32_t gpadc_dwa_en                   :  1; /* [   18],        r/w,        0x0 */
            uint32_t gpadc_mic2_diff                :  1; /* [   19],        r/w,        0x0 */
            uint32_t gpadc_mic1_diff                :  1; /* [   20],        r/w,        0x0 */
            uint32_t gpadc_mic_pga2_gain            :  2; /* [22:21],        r/w,        0x0 */
            uint32_t gpadc_micboost_32db_en         :  1; /* [   23],        r/w,        0x0 */
            uint32_t reserved_24_26                 :  3; /* [26:24],       rsvd,        0x0 */
            uint32_t gpadc_chip_sen_pu              :  1; /* [   27],        r/w,        0x0 */
            uint32_t gpadc_sen_sel                  :  3; /* [30:28],        r/w,        0x0 */
            uint32_t gpadc_sen_test_en              :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_cmd;

    /* 0x510 : gpadc_reg_config1 */
    union {
        struct {
            uint32_t reserved_0                     :  1; /* [    0],       rsvd,        0x0 */
            uint32_t gpadc_cont_conv_en             :  1; /* [    1],        r/w,        0x1 */
            uint32_t gpadc_res_sel                  :  3; /* [ 4: 2],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t gpadc_vcm_sel_en               :  1; /* [    8],        r/w,        0x0 */
            uint32_t gpadc_vcm_hyst_sel             :  1; /* [    9],        r/w,        0x0 */
            uint32_t gpadc_lowv_det_en              :  1; /* [   10],        r/w,        0x0 */
            uint32_t gpadc_pwm_trg_en               :  1; /* [   11],        r/w,        0x0 */
            uint32_t gpadc_clk_ana_dly              :  4; /* [15:12],        r/w,        0x0 */
            uint32_t gpadc_clk_ana_dly_en           :  1; /* [   16],        r/w,        0x0 */
            uint32_t gpadc_clk_ana_inv              :  1; /* [   17],        r/w,        0x0 */
            uint32_t gpadc_clk_div_ratio            :  3; /* [20:18],        r/w,        0x3 */
            uint32_t gpadc_scan_length              :  4; /* [24:21],        r/w,        0x0 */
            uint32_t gpadc_scan_en                  :  1; /* [   25],        r/w,        0x0 */
            uint32_t gpadc_dither_en                :  1; /* [   26],        r/w,        0x0 */
            uint32_t gpadc_v11_sel                  :  2; /* [28:27],        r/w,        0x0 */
            uint32_t gpadc_v18_sel                  :  2; /* [30:29],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_config1;

    /* 0x514 : gpadc_reg_config2 */
    union {
        struct {
            uint32_t reserved_0_1                   :  2; /* [ 1: 0],       rsvd,        0x0 */
            uint32_t gpadc_diff_mode                :  1; /* [    2],        r/w,        0x0 */
            uint32_t gpadc_vref_sel                 :  1; /* [    3],        r/w,        0x0 */
            uint32_t gpadc_vbat_en                  :  1; /* [    4],        r/w,        0x0 */
            uint32_t gpadc_tsext_sel                :  1; /* [    5],        r/w,        0x0 */
            uint32_t gpadc_ts_en                    :  1; /* [    6],        r/w,        0x0 */
            uint32_t gpadc_pga_vcm                  :  2; /* [ 8: 7],        r/w,        0x2 */
            uint32_t gpadc_pga_os_cal               :  4; /* [12: 9],        r/w,        0x8 */
            uint32_t gpadc_pga_en                   :  1; /* [   13],        r/w,        0x0 */
            uint32_t gpadc_pga_vcmi_en              :  1; /* [   14],        r/w,        0x0 */
            uint32_t gpadc_chop_mode                :  2; /* [16:15],        r/w,        0x3 */
            uint32_t gpadc_bias_sel                 :  1; /* [   17],        r/w,        0x0 */
            uint32_t gpadc_test_en                  :  1; /* [   18],        r/w,        0x0 */
            uint32_t gpadc_test_sel                 :  3; /* [21:19],        r/w,        0x0 */
            uint32_t gpadc_pga2_gain                :  3; /* [24:22],        r/w,        0x0 */
            uint32_t gpadc_pga1_gain                :  3; /* [27:25],        r/w,        0x0 */
            uint32_t gpadc_dly_sel                  :  3; /* [30:28],        r/w,        0x0 */
            uint32_t gpadc_tsvbe_low                :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_config2;

    /* 0x518 : adc converation sequence 1 */
    union {
        struct {
            uint32_t gpadc_scan_pos_0               :  5; /* [ 4: 0],        r/w,        0xf */
            uint32_t gpadc_scan_pos_1               :  5; /* [ 9: 5],        r/w,        0xf */
            uint32_t gpadc_scan_pos_2               :  5; /* [14:10],        r/w,        0xf */
            uint32_t gpadc_scan_pos_3               :  5; /* [19:15],        r/w,        0xf */
            uint32_t gpadc_scan_pos_4               :  5; /* [24:20],        r/w,        0xf */
            uint32_t gpadc_scan_pos_5               :  5; /* [29:25],        r/w,        0xf */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_scn_pos1;

    /* 0x51C : adc converation sequence 2 */
    union {
        struct {
            uint32_t gpadc_scan_pos_6               :  5; /* [ 4: 0],        r/w,        0xf */
            uint32_t gpadc_scan_pos_7               :  5; /* [ 9: 5],        r/w,        0xf */
            uint32_t gpadc_scan_pos_8               :  5; /* [14:10],        r/w,        0xf */
            uint32_t gpadc_scan_pos_9               :  5; /* [19:15],        r/w,        0xf */
            uint32_t gpadc_scan_pos_10              :  5; /* [24:20],        r/w,        0xf */
            uint32_t gpadc_scan_pos_11              :  5; /* [29:25],        r/w,        0xf */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_scn_pos2;

    /* 0x520 : adc converation sequence 3 */
    union {
        struct {
            uint32_t gpadc_scan_neg_0               :  5; /* [ 4: 0],        r/w,        0xf */
            uint32_t gpadc_scan_neg_1               :  5; /* [ 9: 5],        r/w,        0xf */
            uint32_t gpadc_scan_neg_2               :  5; /* [14:10],        r/w,        0xf */
            uint32_t gpadc_scan_neg_3               :  5; /* [19:15],        r/w,        0xf */
            uint32_t gpadc_scan_neg_4               :  5; /* [24:20],        r/w,        0xf */
            uint32_t gpadc_scan_neg_5               :  5; /* [29:25],        r/w,        0xf */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_scn_neg1;

    /* 0x524 : adc converation sequence 4 */
    union {
        struct {
            uint32_t gpadc_scan_neg_6               :  5; /* [ 4: 0],        r/w,        0xf */
            uint32_t gpadc_scan_neg_7               :  5; /* [ 9: 5],        r/w,        0xf */
            uint32_t gpadc_scan_neg_8               :  5; /* [14:10],        r/w,        0xf */
            uint32_t gpadc_scan_neg_9               :  5; /* [19:15],        r/w,        0xf */
            uint32_t gpadc_scan_neg_10              :  5; /* [24:20],        r/w,        0xf */
            uint32_t gpadc_scan_neg_11              :  5; /* [29:25],        r/w,        0xf */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_scn_neg2;

    /* 0x528 : gpadc_reg_status */
    union {
        struct {
            uint32_t gpadc_data_rdy                 :  1; /* [    0],          r,        0x0 */
            uint32_t reserved_1_15                  : 15; /* [15: 1],       rsvd,        0x0 */
            uint32_t gpadc_reserved                 : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_status;

    /* 0x52C : gpadc_reg_isr */
    union {
        struct {
            uint32_t gpadc_neg_satur                :  1; /* [    0],          r,        0x0 */
            uint32_t gpadc_pos_satur                :  1; /* [    1],          r,        0x0 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t gpadc_neg_satur_clr            :  1; /* [    4],        r/w,        0x0 */
            uint32_t gpadc_pos_satur_clr            :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t gpadc_neg_satur_mask           :  1; /* [    8],        r/w,        0x0 */
            uint32_t gpadc_pos_satur_mask           :  1; /* [    9],        r/w,        0x0 */
            uint32_t reserved_10_31                 : 22; /* [31:10],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_isr;

    /* 0x530 : gpadc_reg_result */
    union {
        struct {
            uint32_t gpadc_data_out                 : 26; /* [25: 0],          r,  0x1ef0000 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_result;

    /* 0x534 : gpadc_reg_raw_result */
    union {
        struct {
            uint32_t gpadc_raw_data                 : 12; /* [11: 0],          r,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_raw_result;

    /* 0x538 : gpadc_reg_define */
    union {
        struct {
            uint32_t gpadc_os_cal_data              : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_define;

    /* 0x53c  reserved */
    uint8_t RESERVED0x53c[2500];

    /* 0xF00 : tzc_glb_ctrl_0 */
    union {
        struct {
            uint32_t tzc_glb_swrst_s00_lock         :  1; /* [    0],          r,        0x0 */
            uint32_t tzc_glb_swrst_s01_lock         :  1; /* [    1],          r,        0x0 */
            uint32_t reserved_2_7                   :  6; /* [ 7: 2],       rsvd,        0x0 */
            uint32_t tzc_glb_swrst_s30_lock         :  1; /* [    8],          r,        0x0 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t tzc_glb_ctrl_pwron_rst_lock    :  1; /* [   12],          r,        0x0 */
            uint32_t tzc_glb_ctrl_cpu_reset_lock    :  1; /* [   13],          r,        0x0 */
            uint32_t tzc_glb_ctrl_sys_reset_lock    :  1; /* [   14],          r,        0x0 */
            uint32_t tzc_glb_ctrl_ungated_ap_lock   :  1; /* [   15],          r,        0x0 */
            uint32_t reserved_16_24                 :  9; /* [24:16],       rsvd,        0x0 */
            uint32_t tzc_glb_misc_lock              :  1; /* [   25],          r,        0x0 */
            uint32_t tzc_glb_sram_lock              :  1; /* [   26],          r,        0x0 */
            uint32_t tzc_glb_l2c_lock               :  1; /* [   27],          r,        0x0 */
            uint32_t tzc_glb_bmx_lock               :  1; /* [   28],          r,        0x0 */
            uint32_t tzc_glb_dbg_lock               :  1; /* [   29],          r,        0x0 */
            uint32_t tzc_glb_mbist_lock             :  1; /* [   30],          r,        0x0 */
            uint32_t tzc_glb_clk_lock               :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } tzc_glb_ctrl_0;

    /* 0xF04 : tzc_glb_ctrl_1 */
    union {
        struct {
            uint32_t tzc_glb_swrst_s20_lock         :  1; /* [    0],          r,        0x0 */
            uint32_t tzc_glb_swrst_s21_lock         :  1; /* [    1],          r,        0x0 */
            uint32_t tzc_glb_swrst_s22_lock         :  1; /* [    2],          r,        0x0 */
            uint32_t tzc_glb_swrst_s23_lock         :  1; /* [    3],          r,        0x0 */
            uint32_t tzc_glb_swrst_s24_lock         :  1; /* [    4],          r,        0x0 */
            uint32_t tzc_glb_swrst_s25_lock         :  1; /* [    5],          r,        0x0 */
            uint32_t tzc_glb_swrst_s26_lock         :  1; /* [    6],          r,        0x0 */
            uint32_t tzc_glb_swrst_s27_lock         :  1; /* [    7],          r,        0x0 */
            uint32_t tzc_glb_swrst_s28_lock         :  1; /* [    8],          r,        0x0 */
            uint32_t tzc_glb_swrst_s29_lock         :  1; /* [    9],          r,        0x0 */
            uint32_t tzc_glb_swrst_s2a_lock         :  1; /* [   10],          r,        0x0 */
            uint32_t tzc_glb_swrst_s2b_lock         :  1; /* [   11],          r,        0x0 */
            uint32_t tzc_glb_swrst_s2c_lock         :  1; /* [   12],          r,        0x0 */
            uint32_t tzc_glb_swrst_s2d_lock         :  1; /* [   13],          r,        0x0 */
            uint32_t tzc_glb_swrst_s2e_lock         :  1; /* [   14],          r,        0x0 */
            uint32_t tzc_glb_swrst_s2f_lock         :  1; /* [   15],          r,        0x0 */
            uint32_t tzc_glb_swrst_s10_lock         :  1; /* [   16],          r,        0x0 */
            uint32_t tzc_glb_swrst_s11_lock         :  1; /* [   17],          r,        0x0 */
            uint32_t tzc_glb_swrst_s12_lock         :  1; /* [   18],          r,        0x0 */
            uint32_t tzc_glb_swrst_s13_lock         :  1; /* [   19],          r,        0x0 */
            uint32_t tzc_glb_swrst_s14_lock         :  1; /* [   20],          r,        0x0 */
            uint32_t tzc_glb_swrst_s15_lock         :  1; /* [   21],          r,        0x0 */
            uint32_t tzc_glb_swrst_s16_lock         :  1; /* [   22],          r,        0x0 */
            uint32_t tzc_glb_swrst_s17_lock         :  1; /* [   23],          r,        0x0 */
            uint32_t tzc_glb_swrst_s18_lock         :  1; /* [   24],          r,        0x0 */
            uint32_t tzc_glb_swrst_s19_lock         :  1; /* [   25],          r,        0x0 */
            uint32_t tzc_glb_swrst_s1a_lock         :  1; /* [   26],          r,        0x0 */
            uint32_t tzc_glb_swrst_s1b_lock         :  1; /* [   27],          r,        0x0 */
            uint32_t tzc_glb_swrst_s1c_lock         :  1; /* [   28],          r,        0x0 */
            uint32_t tzc_glb_swrst_s1d_lock         :  1; /* [   29],          r,        0x0 */
            uint32_t tzc_glb_swrst_s1e_lock         :  1; /* [   30],          r,        0x0 */
            uint32_t tzc_glb_swrst_s1f_lock         :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } tzc_glb_ctrl_1;

    /* 0xF08 : tzc_glb_ctrl_2 */
    union {
        struct {
            uint32_t tzc_glb_gpio_0_lock            :  1; /* [    0],          r,        0x0 */
            uint32_t tzc_glb_gpio_1_lock            :  1; /* [    1],          r,        0x0 */
            uint32_t tzc_glb_gpio_2_lock            :  1; /* [    2],          r,        0x0 */
            uint32_t tzc_glb_gpio_3_lock            :  1; /* [    3],          r,        0x0 */
            uint32_t tzc_glb_gpio_4_lock            :  1; /* [    4],          r,        0x0 */
            uint32_t tzc_glb_gpio_5_lock            :  1; /* [    5],          r,        0x0 */
            uint32_t tzc_glb_gpio_6_lock            :  1; /* [    6],          r,        0x0 */
            uint32_t tzc_glb_gpio_7_lock            :  1; /* [    7],          r,        0x0 */
            uint32_t tzc_glb_gpio_8_lock            :  1; /* [    8],          r,        0x0 */
            uint32_t tzc_glb_gpio_9_lock            :  1; /* [    9],          r,        0x0 */
            uint32_t tzc_glb_gpio_10_lock           :  1; /* [   10],          r,        0x0 */
            uint32_t tzc_glb_gpio_11_lock           :  1; /* [   11],          r,        0x0 */
            uint32_t tzc_glb_gpio_12_lock           :  1; /* [   12],          r,        0x0 */
            uint32_t tzc_glb_gpio_13_lock           :  1; /* [   13],          r,        0x0 */
            uint32_t tzc_glb_gpio_14_lock           :  1; /* [   14],          r,        0x0 */
            uint32_t tzc_glb_gpio_15_lock           :  1; /* [   15],          r,        0x0 */
            uint32_t tzc_glb_gpio_16_lock           :  1; /* [   16],          r,        0x0 */
            uint32_t tzc_glb_gpio_17_lock           :  1; /* [   17],          r,        0x0 */
            uint32_t tzc_glb_gpio_18_lock           :  1; /* [   18],          r,        0x0 */
            uint32_t tzc_glb_gpio_19_lock           :  1; /* [   19],          r,        0x0 */
            uint32_t tzc_glb_gpio_20_lock           :  1; /* [   20],          r,        0x0 */
            uint32_t tzc_glb_gpio_21_lock           :  1; /* [   21],          r,        0x0 */
            uint32_t tzc_glb_gpio_22_lock           :  1; /* [   22],          r,        0x0 */
            uint32_t tzc_glb_gpio_23_lock           :  1; /* [   23],          r,        0x0 */
            uint32_t tzc_glb_gpio_24_lock           :  1; /* [   24],          r,        0x0 */
            uint32_t tzc_glb_gpio_25_lock           :  1; /* [   25],          r,        0x0 */
            uint32_t tzc_glb_gpio_26_lock           :  1; /* [   26],          r,        0x0 */
            uint32_t tzc_glb_gpio_27_lock           :  1; /* [   27],          r,        0x0 */
            uint32_t tzc_glb_gpio_28_lock           :  1; /* [   28],          r,        0x0 */
            uint32_t tzc_glb_gpio_29_lock           :  1; /* [   29],          r,        0x0 */
            uint32_t tzc_glb_gpio_30_lock           :  1; /* [   30],          r,        0x0 */
            uint32_t tzc_glb_gpio_31_lock           :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } tzc_glb_ctrl_2;

    /* 0xF0C : tzc_glb_ctrl_3 */
    union {
        struct {
            uint32_t tzc_glb_gpio_32_lock           :  1; /* [    0],          r,        0x0 */
            uint32_t tzc_glb_gpio_33_lock           :  1; /* [    1],          r,        0x0 */
            uint32_t tzc_glb_gpio_34_lock           :  1; /* [    2],          r,        0x0 */
            uint32_t tzc_glb_gpio_35_lock           :  1; /* [    3],          r,        0x0 */
            uint32_t tzc_glb_gpio_36_lock           :  1; /* [    4],          r,        0x0 */
            uint32_t tzc_glb_gpio_37_lock           :  1; /* [    5],          r,        0x0 */
            uint32_t reserved_6_31                  : 26; /* [31: 6],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tzc_glb_ctrl_3;

};

typedef volatile struct glb_reg glb_reg_t;


#endif  /* __GLB_REG_H__ */
