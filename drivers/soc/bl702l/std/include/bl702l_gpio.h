/**
  ******************************************************************************
  * @file    bl702l_gpio.h
  * @version V1.0
  * @date    2022-05-10
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

#ifndef __BL702L_GPIO_H__
#define __BL702L_GPIO_H__

typedef enum {
    GLB_GPIO_PIN_0 = 0,
    GLB_GPIO_PIN_1,
    GLB_GPIO_PIN_2,
    GLB_GPIO_PIN_3,
    GLB_GPIO_PIN_4,
    GLB_GPIO_PIN_5,
    GLB_GPIO_PIN_6,
    GLB_GPIO_PIN_7,
    GLB_GPIO_PIN_8,
    GLB_GPIO_PIN_9,
    GLB_GPIO_PIN_10,
    GLB_GPIO_PIN_11,
    GLB_GPIO_PIN_12,
    GLB_GPIO_PIN_13,
    GLB_GPIO_PIN_14,
    GLB_GPIO_PIN_15,
    GLB_GPIO_PIN_16,
    GLB_GPIO_PIN_17,
    GLB_GPIO_PIN_18,
    GLB_GPIO_PIN_19,
    GLB_GPIO_PIN_20,
    GLB_GPIO_PIN_21,
    GLB_GPIO_PIN_22,
    GLB_GPIO_PIN_23,
    GLB_GPIO_PIN_24,
    GLB_GPIO_PIN_25,
    GLB_GPIO_PIN_26,
    GLB_GPIO_PIN_27,
    GLB_GPIO_PIN_28,
    GLB_GPIO_PIN_29,
    GLB_GPIO_PIN_30,
    GLB_GPIO_PIN_31,
    GLB_GPIO_PIN_MAX,
} GLB_GPIO_Type;

#define GPIO_MODE_INPUT  ((uint32_t)0x00000000U) /*!< Input Floating Mode                   */
#define GPIO_MODE_OUTPUT ((uint32_t)0x00000001U) /*!< Output Push Pull Mode                 */
#define GPIO_MODE_AF     ((uint32_t)0x00000002U) /*!< Alternate function                    */
#define GPIO_MODE_ANALOG ((uint32_t)0x00000003U) /*!< Analog function                       */
#define GPIO_PULL_UP     ((uint32_t)0x00000000U) /*!< GPIO pull up                          */
#define GPIO_PULL_DOWN   ((uint32_t)0x00000001U) /*!< GPIO pull down                        */
#define GPIO_PULL_NONE   ((uint32_t)0x00000002U) /*!< GPIO no pull up or down               */

typedef enum {
    GPIO_FUN_CLK_OUT = 0,
    GPIO_FUN_BT_COEXIST = 1,
    GPIO_FUN_FLASH_PSRAM = 2,
    GPIO_FUN_QSPI = 2,
    GPIO_FUN_SPI = 4,
    GPIO_FUN_PDM = 5,
    GPIO_FUN_I2C = 6,
    GPIO_FUN_UART = 7,
    GPIO_FUN_PWM = 8,
    GPIO_FUN_ANALOG = 10,
    GPIO_FUN_GPIO = 11,
    GPIO_FUN_SCAN = 13,
    GPIO_FUN_E21_JTAG = 14,
    GPIO_FUN_DEBUG = 15,
    GPIO_FUN_KEY_SCAN_IN = 21,
    GPIO_FUN_KEY_SCAN_ROW = 21,
    GPIO_FUN_KEY_SCAN_DRIVE = 22,
    GPIO_FUN_KEY_SCAN_COL = 22,

    GPIO_FUN_DAC = 0xD1,
    GPIO_FUN_ADC = 0xD2,
    GPIO_FUN_GPIO_OUTPUT_UP = 0xE0,
    GPIO_FUN_GPIO_OUTPUT_DOWN = 0xE1,
    GPIO_FUN_GPIO_OUTPUT_NONE = 0xE2,
    GPIO_FUN_GPIO_INPUT_UP = 0xE3,
    GPIO_FUN_GPIO_INPUT_DOWN = 0xE4,
    GPIO_FUN_GPIO_INPUT_NONE = 0xE5,
    GPIO_FUN_GPIO_EXTI_SYNC_FALLING_EDGE = 0xE6,
    GPIO_FUN_GPIO_EXTI_SYNC_RISING_EDGE = 0xE7,
    GPIO_FUN_GPIO_EXTI_SYNC_LOW_LEVEL = 0xE8,
    GPIO_FUN_GPIO_EXTI_SYNC_HIGH_LEVEL = 0xE9,
    GPIO_FUN_GPIO_EXIT_SYNC_RISING_FALLING_EDGE = 0xEA,
    GPIO_FUN_GPIO_EXTI_ASYNC_FALLING_EDGE = 0xEB,
    GPIO_FUN_GPIO_EXTI_AYNC_RISING_EDGE = 0xEC,
    GPIO_FUN_GPIO_EXTI_AYNC_LOW_LEVEL = 0xED,
    GPIO_FUN_GPIO_EXTI_AYNC_HIGH_LEVEL = 0xEE,
    GPIO_FUN_UART0_RTS = 0xF0,
    GPIO_FUN_UART0_CTS = 0xF1,
    GPIO_FUN_UART0_TX = 0xF2,
    GPIO_FUN_UART0_RX = 0xF3,
    GPIO_FUN_WAKEUP = 0xFE,

    GPIO_FUN_UNUSED = 0xFF,
} GLB_GPIO_FUNC_Type;

typedef struct
{
    uint8_t gpioPin;
    uint8_t gpioFun;
    uint8_t gpioMode;
    uint8_t pullType;
    uint8_t drive;
    uint8_t smtCtrl;
} GLB_GPIO_Cfg_Type;
#endif /*__BL702L_GPIO_H__ */
