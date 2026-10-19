/**
  ******************************************************************************
  * @file    bl616_gpio.h
  * @version V1.0
  * @date    2020-11-06
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
#ifndef __BL616_GPIO_H__
#define __BL616_GPIO_H__

#define GLB_GPIO_PIN_0   (0)
#define GLB_GPIO_PIN_1   (1)
#define GLB_GPIO_PIN_2   (2)
#define GLB_GPIO_PIN_3   (3)
#define GLB_GPIO_PIN_4   (4)
#define GLB_GPIO_PIN_5   (5)
#define GLB_GPIO_PIN_6   (6)
#define GLB_GPIO_PIN_7   (7)
#define GLB_GPIO_PIN_8   (8)
#define GLB_GPIO_PIN_9   (9)
#define GLB_GPIO_PIN_10  (10)
#define GLB_GPIO_PIN_11  (11)
#define GLB_GPIO_PIN_12  (12)
#define GLB_GPIO_PIN_13  (13)
#define GLB_GPIO_PIN_14  (14)
#define GLB_GPIO_PIN_15  (15)
#define GLB_GPIO_PIN_16  (16)
#define GLB_GPIO_PIN_17  (17)
#define GLB_GPIO_PIN_18  (18)
#define GLB_GPIO_PIN_19  (19)
#define GLB_GPIO_PIN_20  (20)
#define GLB_GPIO_PIN_21  (21)
#define GLB_GPIO_PIN_22  (22)
#define GLB_GPIO_PIN_23  (23)
#define GLB_GPIO_PIN_24  (24)
#define GLB_GPIO_PIN_25  (25)
#define GLB_GPIO_PIN_26  (26)
#define GLB_GPIO_PIN_27  (27)
#define GLB_GPIO_PIN_28  (28)
#define GLB_GPIO_PIN_29  (29)
#define GLB_GPIO_PIN_30  (30)
#define GLB_GPIO_PIN_31  (31)
#define GLB_GPIO_PIN_32  (32)
#define GLB_GPIO_PIN_33  (33)
#define GLB_GPIO_PIN_34  (34)
#define GLB_GPIO_PIN_MAX (35)

#define GPIO_MODE_INPUT            ((uint32_t)0x00000000U) /*!< Input Floating Mode                   */
#define GPIO_MODE_OUTPUT           ((uint32_t)0x00000001U) /*!< Output Push Pull Mode                 */
#define GPIO_MODE_AF               ((uint32_t)0x00000002U) /*!< Alternate function                    */
#define GPIO_MODE_ANALOG           ((uint32_t)0x00000003U) /*!< Analog function                       */
#define GPIO_PULL_UP               ((uint32_t)0x00000000U) /*!< GPIO pull up                          */
#define GPIO_PULL_DOWN             ((uint32_t)0x00000001U) /*!< GPIO pull down                        */
#define GPIO_PULL_NONE             ((uint32_t)0x00000002U) /*!< GPIO no pull up or down               */
#define GPIO_OUTPUT_VALUE_MODE     ((uint8_t)0x00U)        /*!< GPIO Output by reg_gpio_x_o Value                             */
#define GPIO_SET_CLR_MODE          ((uint8_t)0x01U)        /*!< GPIO Output set by reg_gpio_x_set and clear by reg_gpio_x_clr */
#define GPIO_DMA_OUTPUT_VALUE_MODE ((uint8_t)0x02U)        /*!< GPIO Output value by gpio_dma_o                               */
#define GPIO_DMA_SET_CLR_MODE      ((uint8_t)0x03U)        /*!< GPIO Outout value by gpio_dma_set/gpio_dma_clr                */

#define GPIO_FUN_SDH                          (0)
#define GPIO_FUN_SPI                          (1)
#define GPIO_FUN_FLASH                        (2)
#define GPIO_FUN_I2S                          (3)
#define GPIO_FUN_PDM                          (4)
#define GPIO_FUN_I2C0                         (5)
#define GPIO_FUN_I2C1                         (6)
#define GPIO_FUN_UART                         (7)
#define GPIO_FUN_ETHER_MAC                    (8)
#define GPIO_FUN_CAM                          (9)
#define GPIO_FUN_ANALOG                       (10)
#define GPIO_FUN_GPIO                         (11)
#define GPIO_FUN_SDIO                         (12)
#define GPIO_FUN_PWM0                         (16)
#define GPIO_FUN_MD_JTAG                      (17)
#define GPIO_FUN_MD_UART                      (18)
#define GPIO_FUN_MD_PWM                       (19)
#define GPIO_FUN_MD_SPI                       (20)
#define GPIO_FUN_MD_I2S                       (21)
#define GPIO_FUN_DBI_B                        (22)
#define GPIO_FUN_DBI_C                        (23)
#define GPIO_FUN_DISP_QSPI                    (24)
#define GPIO_FUN_AUDAC_PWM                    (25)
#define GPIO_FUN_JTAG                         (26)
#define GPIO_FUN_CLOCK_OUT                    (31)

#define GPIO_FUN_CLOCK_OUT_X_CAM_REF_CLK      (0xE0)
#define GPIO_FUN_CLOCK_OUT_X_I2S_REF_CLK      (0xE1)
#define GPIO_FUN_CLOCK_OUT_0_1_3_NONE         (0xE2)
#define GPIO_FUN_CLOCK_OUT_0_1_SOLO_128FS_CLK (0xE3)
#define GPIO_FUN_CLOCK_OUT_2_ANA_XTAL_CLK     (0xE3)
#define GPIO_FUN_CLOCK_OUT_2_WIFI_32M_CLK     (0xE3)
#define GPIO_FUN_CLOCK_OUT_3_WIFI_48M_CLK     (0xE3)

#define GPIO_FUN_UART0_RTS                    (0xF0)
#define GPIO_FUN_UART0_CTS                    (0xF1)
#define GPIO_FUN_UART0_TX                     (0xF2)
#define GPIO_FUN_UART0_RX                     (0xF3)
#define GPIO_FUN_UART1_RTS                    (0xF4)
#define GPIO_FUN_UART1_CTS                    (0xF5)
#define GPIO_FUN_UART1_TX                     (0xF6)
#define GPIO_FUN_UART1_RX                     (0xF7)

#define GPIO_FUN_UNUSED                       (0xFF)

typedef struct
{
    uint8_t gpioPin;
    uint8_t gpioFun;
    uint8_t gpioMode;
    uint8_t pullType;
    uint8_t drive;
    uint8_t smtCtrl;
    uint8_t outputMode;
} GLB_GPIO_Cfg_Type;

typedef uint8_t GLB_GPIO_Type;

#endif /*__BL616_GPIO_H__ */
