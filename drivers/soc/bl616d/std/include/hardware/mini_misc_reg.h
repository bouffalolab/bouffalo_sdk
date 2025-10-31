/**
  ******************************************************************************
  * @file    mini_misc_reg.h
  * @version V1.0
  * @date    2024-11-11
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
#ifndef __MINI_MISC_REG_H__
#define __MINI_MISC_REG_H__

#include "bl616d.h"

/* 0x0 : mini_bus_0 */
#define MINI_MISC_MINI_BUS_0_OFFSET       (0x0)
#define MINI_MISC_CR_FRUN_PCLK            MINI_MISC_CR_FRUN_PCLK
#define MINI_MISC_CR_FRUN_PCLK_POS        (16U)
#define MINI_MISC_CR_FRUN_PCLK_LEN        (16U)
#define MINI_MISC_CR_FRUN_PCLK_MSK        (((1U << MINI_MISC_CR_FRUN_PCLK_LEN) - 1) << MINI_MISC_CR_FRUN_PCLK_POS)
#define MINI_MISC_CR_FRUN_PCLK_UMSK       (~(((1U << MINI_MISC_CR_FRUN_PCLK_LEN) - 1) << MINI_MISC_CR_FRUN_PCLK_POS))

/* 0x4 : mini_bus_1 */
#define MINI_MISC_MINI_BUS_1_OFFSET       (0x4)
#define MINI_MISC_CR_W_THRE_TOP2MINI      MINI_MISC_CR_W_THRE_TOP2MINI
#define MINI_MISC_CR_W_THRE_TOP2MINI_POS  (0U)
#define MINI_MISC_CR_W_THRE_TOP2MINI_LEN  (2U)
#define MINI_MISC_CR_W_THRE_TOP2MINI_MSK  (((1U << MINI_MISC_CR_W_THRE_TOP2MINI_LEN) - 1) << MINI_MISC_CR_W_THRE_TOP2MINI_POS)
#define MINI_MISC_CR_W_THRE_TOP2MINI_UMSK (~(((1U << MINI_MISC_CR_W_THRE_TOP2MINI_LEN) - 1) << MINI_MISC_CR_W_THRE_TOP2MINI_POS))
#define MINI_MISC_CR_W_THRE_MINI2TOP      MINI_MISC_CR_W_THRE_MINI2TOP
#define MINI_MISC_CR_W_THRE_MINI2TOP_POS  (2U)
#define MINI_MISC_CR_W_THRE_MINI2TOP_LEN  (2U)
#define MINI_MISC_CR_W_THRE_MINI2TOP_MSK  (((1U << MINI_MISC_CR_W_THRE_MINI2TOP_LEN) - 1) << MINI_MISC_CR_W_THRE_MINI2TOP_POS)
#define MINI_MISC_CR_W_THRE_MINI2TOP_UMSK (~(((1U << MINI_MISC_CR_W_THRE_MINI2TOP_LEN) - 1) << MINI_MISC_CR_W_THRE_MINI2TOP_POS))

/* 0x8 : mini_bus_2 */
#define MINI_MISC_MINI_BUS_2_OFFSET       (0x8)
#define MINI_MISC_CR_TIMEOUT_EN           MINI_MISC_CR_TIMEOUT_EN
#define MINI_MISC_CR_TIMEOUT_EN_POS       (0U)
#define MINI_MISC_CR_TIMEOUT_EN_LEN       (1U)
#define MINI_MISC_CR_TIMEOUT_EN_MSK       (((1U << MINI_MISC_CR_TIMEOUT_EN_LEN) - 1) << MINI_MISC_CR_TIMEOUT_EN_POS)
#define MINI_MISC_CR_TIMEOUT_EN_UMSK      (~(((1U << MINI_MISC_CR_TIMEOUT_EN_LEN) - 1) << MINI_MISC_CR_TIMEOUT_EN_POS))
#define MINI_MISC_CR_TIMEOUT_CLR          MINI_MISC_CR_TIMEOUT_CLR
#define MINI_MISC_CR_TIMEOUT_CLR_POS      (1U)
#define MINI_MISC_CR_TIMEOUT_CLR_LEN      (1U)
#define MINI_MISC_CR_TIMEOUT_CLR_MSK      (((1U << MINI_MISC_CR_TIMEOUT_CLR_LEN) - 1) << MINI_MISC_CR_TIMEOUT_CLR_POS)
#define MINI_MISC_CR_TIMEOUT_CLR_UMSK     (~(((1U << MINI_MISC_CR_TIMEOUT_CLR_LEN) - 1) << MINI_MISC_CR_TIMEOUT_CLR_POS))
#define MINI_MISC_CR_ARB_MODE             MINI_MISC_CR_ARB_MODE
#define MINI_MISC_CR_ARB_MODE_POS         (2U)
#define MINI_MISC_CR_ARB_MODE_LEN         (1U)
#define MINI_MISC_CR_ARB_MODE_MSK         (((1U << MINI_MISC_CR_ARB_MODE_LEN) - 1) << MINI_MISC_CR_ARB_MODE_POS)
#define MINI_MISC_CR_ARB_MODE_UMSK        (~(((1U << MINI_MISC_CR_ARB_MODE_LEN) - 1) << MINI_MISC_CR_ARB_MODE_POS))
#define MINI_MISC_STS_TIMEOUT_CM0         MINI_MISC_STS_TIMEOUT_CM0
#define MINI_MISC_STS_TIMEOUT_CM0_POS     (8U)
#define MINI_MISC_STS_TIMEOUT_CM0_LEN     (1U)
#define MINI_MISC_STS_TIMEOUT_CM0_MSK     (((1U << MINI_MISC_STS_TIMEOUT_CM0_LEN) - 1) << MINI_MISC_STS_TIMEOUT_CM0_POS)
#define MINI_MISC_STS_TIMEOUT_CM0_UMSK    (~(((1U << MINI_MISC_STS_TIMEOUT_CM0_LEN) - 1) << MINI_MISC_STS_TIMEOUT_CM0_POS))

/* 0x20 : mini_clock_0 */
#define MINI_MISC_MINI_CLOCK_0_OFFSET     (0x20)
#define MINI_MISC_CR_CGEN_FCLK            MINI_MISC_CR_CGEN_FCLK
#define MINI_MISC_CR_CGEN_FCLK_POS        (0U)
#define MINI_MISC_CR_CGEN_FCLK_LEN        (1U)
#define MINI_MISC_CR_CGEN_FCLK_MSK        (((1U << MINI_MISC_CR_CGEN_FCLK_LEN) - 1) << MINI_MISC_CR_CGEN_FCLK_POS)
#define MINI_MISC_CR_CGEN_FCLK_UMSK       (~(((1U << MINI_MISC_CR_CGEN_FCLK_LEN) - 1) << MINI_MISC_CR_CGEN_FCLK_POS))
#define MINI_MISC_CR_CGEN_BCLK_PWM        MINI_MISC_CR_CGEN_BCLK_PWM
#define MINI_MISC_CR_CGEN_BCLK_PWM_POS    (1U)
#define MINI_MISC_CR_CGEN_BCLK_PWM_LEN    (1U)
#define MINI_MISC_CR_CGEN_BCLK_PWM_MSK    (((1U << MINI_MISC_CR_CGEN_BCLK_PWM_LEN) - 1) << MINI_MISC_CR_CGEN_BCLK_PWM_POS)
#define MINI_MISC_CR_CGEN_BCLK_PWM_UMSK   (~(((1U << MINI_MISC_CR_CGEN_BCLK_PWM_LEN) - 1) << MINI_MISC_CR_CGEN_BCLK_PWM_POS))
#define MINI_MISC_CR_CGEN_BCLK_I2C        MINI_MISC_CR_CGEN_BCLK_I2C
#define MINI_MISC_CR_CGEN_BCLK_I2C_POS    (2U)
#define MINI_MISC_CR_CGEN_BCLK_I2C_LEN    (1U)
#define MINI_MISC_CR_CGEN_BCLK_I2C_MSK    (((1U << MINI_MISC_CR_CGEN_BCLK_I2C_LEN) - 1) << MINI_MISC_CR_CGEN_BCLK_I2C_POS)
#define MINI_MISC_CR_CGEN_BCLK_I2C_UMSK   (~(((1U << MINI_MISC_CR_CGEN_BCLK_I2C_LEN) - 1) << MINI_MISC_CR_CGEN_BCLK_I2C_POS))
#define MINI_MISC_CR_CGEN_BCLK_TMR        MINI_MISC_CR_CGEN_BCLK_TMR
#define MINI_MISC_CR_CGEN_BCLK_TMR_POS    (3U)
#define MINI_MISC_CR_CGEN_BCLK_TMR_LEN    (1U)
#define MINI_MISC_CR_CGEN_BCLK_TMR_MSK    (((1U << MINI_MISC_CR_CGEN_BCLK_TMR_LEN) - 1) << MINI_MISC_CR_CGEN_BCLK_TMR_POS)
#define MINI_MISC_CR_CGEN_BCLK_TMR_UMSK   (~(((1U << MINI_MISC_CR_CGEN_BCLK_TMR_LEN) - 1) << MINI_MISC_CR_CGEN_BCLK_TMR_POS))
#define MINI_MISC_CR_CGEN_BCLK_UART       MINI_MISC_CR_CGEN_BCLK_UART
#define MINI_MISC_CR_CGEN_BCLK_UART_POS   (4U)
#define MINI_MISC_CR_CGEN_BCLK_UART_LEN   (1U)
#define MINI_MISC_CR_CGEN_BCLK_UART_MSK   (((1U << MINI_MISC_CR_CGEN_BCLK_UART_LEN) - 1) << MINI_MISC_CR_CGEN_BCLK_UART_POS)
#define MINI_MISC_CR_CGEN_BCLK_UART_UMSK  (~(((1U << MINI_MISC_CR_CGEN_BCLK_UART_LEN) - 1) << MINI_MISC_CR_CGEN_BCLK_UART_POS))
#define MINI_MISC_CR_CGEN_BCLK_SPI        MINI_MISC_CR_CGEN_BCLK_SPI
#define MINI_MISC_CR_CGEN_BCLK_SPI_POS    (5U)
#define MINI_MISC_CR_CGEN_BCLK_SPI_LEN    (1U)
#define MINI_MISC_CR_CGEN_BCLK_SPI_MSK    (((1U << MINI_MISC_CR_CGEN_BCLK_SPI_LEN) - 1) << MINI_MISC_CR_CGEN_BCLK_SPI_POS)
#define MINI_MISC_CR_CGEN_BCLK_SPI_UMSK   (~(((1U << MINI_MISC_CR_CGEN_BCLK_SPI_LEN) - 1) << MINI_MISC_CR_CGEN_BCLK_SPI_POS))
#define MINI_MISC_CR_FCLK_EN              MINI_MISC_CR_FCLK_EN
#define MINI_MISC_CR_FCLK_EN_POS          (16U)
#define MINI_MISC_CR_FCLK_EN_LEN          (1U)
#define MINI_MISC_CR_FCLK_EN_MSK          (((1U << MINI_MISC_CR_FCLK_EN_LEN) - 1) << MINI_MISC_CR_FCLK_EN_POS)
#define MINI_MISC_CR_FCLK_EN_UMSK         (~(((1U << MINI_MISC_CR_FCLK_EN_LEN) - 1) << MINI_MISC_CR_FCLK_EN_POS))
#define MINI_MISC_CR_FCLK_SEL             MINI_MISC_CR_FCLK_SEL
#define MINI_MISC_CR_FCLK_SEL_POS         (18U)
#define MINI_MISC_CR_FCLK_SEL_LEN         (2U)
#define MINI_MISC_CR_FCLK_SEL_MSK         (((1U << MINI_MISC_CR_FCLK_SEL_LEN) - 1) << MINI_MISC_CR_FCLK_SEL_POS)
#define MINI_MISC_CR_FCLK_SEL_UMSK        (~(((1U << MINI_MISC_CR_FCLK_SEL_LEN) - 1) << MINI_MISC_CR_FCLK_SEL_POS))
#define MINI_MISC_CR_FCLK_DIV             MINI_MISC_CR_FCLK_DIV
#define MINI_MISC_CR_FCLK_DIV_POS         (20U)
#define MINI_MISC_CR_FCLK_DIV_LEN         (7U)
#define MINI_MISC_CR_FCLK_DIV_MSK         (((1U << MINI_MISC_CR_FCLK_DIV_LEN) - 1) << MINI_MISC_CR_FCLK_DIV_POS)
#define MINI_MISC_CR_FCLK_DIV_UMSK        (~(((1U << MINI_MISC_CR_FCLK_DIV_LEN) - 1) << MINI_MISC_CR_FCLK_DIV_POS))

/* 0x24 : mini_clock_1 */
#define MINI_MISC_MINI_CLOCK_1_OFFSET     (0x24)
#define MINI_MISC_CR_SPI_CLK_SEL          MINI_MISC_CR_SPI_CLK_SEL
#define MINI_MISC_CR_SPI_CLK_SEL_POS      (0U)
#define MINI_MISC_CR_SPI_CLK_SEL_LEN      (1U)
#define MINI_MISC_CR_SPI_CLK_SEL_MSK      (((1U << MINI_MISC_CR_SPI_CLK_SEL_LEN) - 1) << MINI_MISC_CR_SPI_CLK_SEL_POS)
#define MINI_MISC_CR_SPI_CLK_SEL_UMSK     (~(((1U << MINI_MISC_CR_SPI_CLK_SEL_LEN) - 1) << MINI_MISC_CR_SPI_CLK_SEL_POS))
#define MINI_MISC_CR_SPI_CLK_EN           MINI_MISC_CR_SPI_CLK_EN
#define MINI_MISC_CR_SPI_CLK_EN_POS       (1U)
#define MINI_MISC_CR_SPI_CLK_EN_LEN       (1U)
#define MINI_MISC_CR_SPI_CLK_EN_MSK       (((1U << MINI_MISC_CR_SPI_CLK_EN_LEN) - 1) << MINI_MISC_CR_SPI_CLK_EN_POS)
#define MINI_MISC_CR_SPI_CLK_EN_UMSK      (~(((1U << MINI_MISC_CR_SPI_CLK_EN_LEN) - 1) << MINI_MISC_CR_SPI_CLK_EN_POS))
#define MINI_MISC_CR_SPI_CLK_DIV          MINI_MISC_CR_SPI_CLK_DIV
#define MINI_MISC_CR_SPI_CLK_DIV_POS      (8U)
#define MINI_MISC_CR_SPI_CLK_DIV_LEN      (5U)
#define MINI_MISC_CR_SPI_CLK_DIV_MSK      (((1U << MINI_MISC_CR_SPI_CLK_DIV_LEN) - 1) << MINI_MISC_CR_SPI_CLK_DIV_POS)
#define MINI_MISC_CR_SPI_CLK_DIV_UMSK     (~(((1U << MINI_MISC_CR_SPI_CLK_DIV_LEN) - 1) << MINI_MISC_CR_SPI_CLK_DIV_POS))

/* 0x40 : mini_rst_0 */
#define MINI_MISC_MINI_RST_0_OFFSET       (0x40)
#define MINI_MISC_CR_SWRST_DMA            MINI_MISC_CR_SWRST_DMA
#define MINI_MISC_CR_SWRST_DMA_POS        (0U)
#define MINI_MISC_CR_SWRST_DMA_LEN        (1U)
#define MINI_MISC_CR_SWRST_DMA_MSK        (((1U << MINI_MISC_CR_SWRST_DMA_LEN) - 1) << MINI_MISC_CR_SWRST_DMA_POS)
#define MINI_MISC_CR_SWRST_DMA_UMSK       (~(((1U << MINI_MISC_CR_SWRST_DMA_LEN) - 1) << MINI_MISC_CR_SWRST_DMA_POS))
#define MINI_MISC_CR_SWRST_SPI            MINI_MISC_CR_SWRST_SPI
#define MINI_MISC_CR_SWRST_SPI_POS        (1U)
#define MINI_MISC_CR_SWRST_SPI_LEN        (1U)
#define MINI_MISC_CR_SWRST_SPI_MSK        (((1U << MINI_MISC_CR_SWRST_SPI_LEN) - 1) << MINI_MISC_CR_SWRST_SPI_POS)
#define MINI_MISC_CR_SWRST_SPI_UMSK       (~(((1U << MINI_MISC_CR_SWRST_SPI_LEN) - 1) << MINI_MISC_CR_SWRST_SPI_POS))
#define MINI_MISC_CR_SWRST_UART           MINI_MISC_CR_SWRST_UART
#define MINI_MISC_CR_SWRST_UART_POS       (2U)
#define MINI_MISC_CR_SWRST_UART_LEN       (1U)
#define MINI_MISC_CR_SWRST_UART_MSK       (((1U << MINI_MISC_CR_SWRST_UART_LEN) - 1) << MINI_MISC_CR_SWRST_UART_POS)
#define MINI_MISC_CR_SWRST_UART_UMSK      (~(((1U << MINI_MISC_CR_SWRST_UART_LEN) - 1) << MINI_MISC_CR_SWRST_UART_POS))
#define MINI_MISC_CR_SWRST_TMR            MINI_MISC_CR_SWRST_TMR
#define MINI_MISC_CR_SWRST_TMR_POS        (3U)
#define MINI_MISC_CR_SWRST_TMR_LEN        (1U)
#define MINI_MISC_CR_SWRST_TMR_MSK        (((1U << MINI_MISC_CR_SWRST_TMR_LEN) - 1) << MINI_MISC_CR_SWRST_TMR_POS)
#define MINI_MISC_CR_SWRST_TMR_UMSK       (~(((1U << MINI_MISC_CR_SWRST_TMR_LEN) - 1) << MINI_MISC_CR_SWRST_TMR_POS))
#define MINI_MISC_CR_SWRST_I2C            MINI_MISC_CR_SWRST_I2C
#define MINI_MISC_CR_SWRST_I2C_POS        (4U)
#define MINI_MISC_CR_SWRST_I2C_LEN        (1U)
#define MINI_MISC_CR_SWRST_I2C_MSK        (((1U << MINI_MISC_CR_SWRST_I2C_LEN) - 1) << MINI_MISC_CR_SWRST_I2C_POS)
#define MINI_MISC_CR_SWRST_I2C_UMSK       (~(((1U << MINI_MISC_CR_SWRST_I2C_LEN) - 1) << MINI_MISC_CR_SWRST_I2C_POS))
#define MINI_MISC_CR_SWRST_PWM            MINI_MISC_CR_SWRST_PWM
#define MINI_MISC_CR_SWRST_PWM_POS        (5U)
#define MINI_MISC_CR_SWRST_PWM_LEN        (1U)
#define MINI_MISC_CR_SWRST_PWM_MSK        (((1U << MINI_MISC_CR_SWRST_PWM_LEN) - 1) << MINI_MISC_CR_SWRST_PWM_POS)
#define MINI_MISC_CR_SWRST_PWM_UMSK       (~(((1U << MINI_MISC_CR_SWRST_PWM_LEN) - 1) << MINI_MISC_CR_SWRST_PWM_POS))
#define MINI_MISC_CR_SYSREQ_RST_EN        MINI_MISC_CR_SYSREQ_RST_EN
#define MINI_MISC_CR_SYSREQ_RST_EN_POS    (16U)
#define MINI_MISC_CR_SYSREQ_RST_EN_LEN    (1U)
#define MINI_MISC_CR_SYSREQ_RST_EN_MSK    (((1U << MINI_MISC_CR_SYSREQ_RST_EN_LEN) - 1) << MINI_MISC_CR_SYSREQ_RST_EN_POS)
#define MINI_MISC_CR_SYSREQ_RST_EN_UMSK   (~(((1U << MINI_MISC_CR_SYSREQ_RST_EN_LEN) - 1) << MINI_MISC_CR_SYSREQ_RST_EN_POS))
#define MINI_MISC_CR_LOCKUP_RST_EN        MINI_MISC_CR_LOCKUP_RST_EN
#define MINI_MISC_CR_LOCKUP_RST_EN_POS    (17U)
#define MINI_MISC_CR_LOCKUP_RST_EN_LEN    (1U)
#define MINI_MISC_CR_LOCKUP_RST_EN_MSK    (((1U << MINI_MISC_CR_LOCKUP_RST_EN_LEN) - 1) << MINI_MISC_CR_LOCKUP_RST_EN_POS)
#define MINI_MISC_CR_LOCKUP_RST_EN_UMSK   (~(((1U << MINI_MISC_CR_LOCKUP_RST_EN_LEN) - 1) << MINI_MISC_CR_LOCKUP_RST_EN_POS))

/* 0x44 : mini_rst_1 */
#define MINI_MISC_MINI_RST_1_OFFSET       (0x44)
#define MINI_MISC_CR_MINI_MCU_SW_RST      MINI_MISC_CR_MINI_MCU_SW_RST
#define MINI_MISC_CR_MINI_MCU_SW_RST_POS  (0U)
#define MINI_MISC_CR_MINI_MCU_SW_RST_LEN  (1U)
#define MINI_MISC_CR_MINI_MCU_SW_RST_MSK  (((1U << MINI_MISC_CR_MINI_MCU_SW_RST_LEN) - 1) << MINI_MISC_CR_MINI_MCU_SW_RST_POS)
#define MINI_MISC_CR_MINI_MCU_SW_RST_UMSK (~(((1U << MINI_MISC_CR_MINI_MCU_SW_RST_LEN) - 1) << MINI_MISC_CR_MINI_MCU_SW_RST_POS))
#define MINI_MISC_CR_MINI_SYS_RESET       MINI_MISC_CR_MINI_SYS_RESET
#define MINI_MISC_CR_MINI_SYS_RESET_POS   (1U)
#define MINI_MISC_CR_MINI_SYS_RESET_LEN   (1U)
#define MINI_MISC_CR_MINI_SYS_RESET_MSK   (((1U << MINI_MISC_CR_MINI_SYS_RESET_LEN) - 1) << MINI_MISC_CR_MINI_SYS_RESET_POS)
#define MINI_MISC_CR_MINI_SYS_RESET_UMSK  (~(((1U << MINI_MISC_CR_MINI_SYS_RESET_LEN) - 1) << MINI_MISC_CR_MINI_SYS_RESET_POS))

/* 0x60 : mini_module_0 */
#define MINI_MISC_MINI_MODULE_0_OFFSET    (0x60)
#define MINI_MISC_CR_DMA_CLK_EN           MINI_MISC_CR_DMA_CLK_EN
#define MINI_MISC_CR_DMA_CLK_EN_POS       (0U)
#define MINI_MISC_CR_DMA_CLK_EN_LEN       (4U)
#define MINI_MISC_CR_DMA_CLK_EN_MSK       (((1U << MINI_MISC_CR_DMA_CLK_EN_LEN) - 1) << MINI_MISC_CR_DMA_CLK_EN_POS)
#define MINI_MISC_CR_DMA_CLK_EN_UMSK      (~(((1U << MINI_MISC_CR_DMA_CLK_EN_LEN) - 1) << MINI_MISC_CR_DMA_CLK_EN_POS))

/* 0x80 : mini_rtc */
#define MINI_MISC_MINI_RTC_OFFSET         (0x80)
#define MINI_MISC_CR_LPCPU_RTC_DIV        MINI_MISC_CR_LPCPU_RTC_DIV
#define MINI_MISC_CR_LPCPU_RTC_DIV_POS    (0U)
#define MINI_MISC_CR_LPCPU_RTC_DIV_LEN    (10U)
#define MINI_MISC_CR_LPCPU_RTC_DIV_MSK    (((1U << MINI_MISC_CR_LPCPU_RTC_DIV_LEN) - 1) << MINI_MISC_CR_LPCPU_RTC_DIV_POS)
#define MINI_MISC_CR_LPCPU_RTC_DIV_UMSK   (~(((1U << MINI_MISC_CR_LPCPU_RTC_DIV_LEN) - 1) << MINI_MISC_CR_LPCPU_RTC_DIV_POS))
#define MINI_MISC_CR_LPCPU_RTC_EN         MINI_MISC_CR_LPCPU_RTC_EN
#define MINI_MISC_CR_LPCPU_RTC_EN_POS     (12U)
#define MINI_MISC_CR_LPCPU_RTC_EN_LEN     (1U)
#define MINI_MISC_CR_LPCPU_RTC_EN_MSK     (((1U << MINI_MISC_CR_LPCPU_RTC_EN_LEN) - 1) << MINI_MISC_CR_LPCPU_RTC_EN_POS)
#define MINI_MISC_CR_LPCPU_RTC_EN_UMSK    (~(((1U << MINI_MISC_CR_LPCPU_RTC_EN_LEN) - 1) << MINI_MISC_CR_LPCPU_RTC_EN_POS))
#define MINI_MISC_CR_LPCPU_RTC_RST        MINI_MISC_CR_LPCPU_RTC_RST
#define MINI_MISC_CR_LPCPU_RTC_RST_POS    (13U)
#define MINI_MISC_CR_LPCPU_RTC_RST_LEN    (1U)
#define MINI_MISC_CR_LPCPU_RTC_RST_MSK    (((1U << MINI_MISC_CR_LPCPU_RTC_RST_LEN) - 1) << MINI_MISC_CR_LPCPU_RTC_RST_POS)
#define MINI_MISC_CR_LPCPU_RTC_RST_UMSK   (~(((1U << MINI_MISC_CR_LPCPU_RTC_RST_LEN) - 1) << MINI_MISC_CR_LPCPU_RTC_RST_POS))

/* 0x84 : mini_rst_addr */
#define MINI_MISC_MINI_RST_ADDR_OFFSET    (0x84)
#define MINI_MISC_CR_LPCPU_RST_ADDR       MINI_MISC_CR_LPCPU_RST_ADDR
#define MINI_MISC_CR_LPCPU_RST_ADDR_POS   (0U)
#define MINI_MISC_CR_LPCPU_RST_ADDR_LEN   (32U)
#define MINI_MISC_CR_LPCPU_RST_ADDR_MSK   (((1U << MINI_MISC_CR_LPCPU_RST_ADDR_LEN) - 1) << MINI_MISC_CR_LPCPU_RST_ADDR_POS)
#define MINI_MISC_CR_LPCPU_RST_ADDR_UMSK  (~(((1U << MINI_MISC_CR_LPCPU_RST_ADDR_LEN) - 1) << MINI_MISC_CR_LPCPU_RST_ADDR_POS))

/* 0x88 : mini_int */
#define MINI_MISC_MINI_INT_OFFSET         (0x88)
#define MINI_MISC_CR_LPCPU_TRIG_INT       MINI_MISC_CR_LPCPU_TRIG_INT
#define MINI_MISC_CR_LPCPU_TRIG_INT_POS   (0U)
#define MINI_MISC_CR_LPCPU_TRIG_INT_LEN   (1U)
#define MINI_MISC_CR_LPCPU_TRIG_INT_MSK   (((1U << MINI_MISC_CR_LPCPU_TRIG_INT_LEN) - 1) << MINI_MISC_CR_LPCPU_TRIG_INT_POS)
#define MINI_MISC_CR_LPCPU_TRIG_INT_UMSK  (~(((1U << MINI_MISC_CR_LPCPU_TRIG_INT_LEN) - 1) << MINI_MISC_CR_LPCPU_TRIG_INT_POS))

struct mini_misc_reg {
    /* 0x0 : mini_bus_0 */
    union {
        struct {
            uint32_t reserved_0_15 : 16; /* [15: 0],       rsvd,        0x0 */
            uint32_t cr_frun_pclk  : 16; /* [31:16],        r/w,     0xffff */
        } BF;
        uint32_t WORD;
    } mini_bus_0;

    /* 0x4 : mini_bus_1 */
    union {
        struct {
            uint32_t cr_w_thre_top2mini : 2;  /* [ 1: 0],        r/w,        0x0 */
            uint32_t cr_w_thre_mini2top : 2;  /* [ 3: 2],        r/w,        0x0 */
            uint32_t reserved_4_31      : 28; /* [31: 4],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } mini_bus_1;

    /* 0x8 : mini_bus_2 */
    union {
        struct {
            uint32_t cr_timeout_en   : 1;  /* [    0],        r/w,        0x0 */
            uint32_t cr_timeout_clr  : 1;  /* [    1],        r/w,        0x0 */
            uint32_t cr_arb_mode     : 1;  /* [    2],        r/w,        0x0 */
            uint32_t reserved_3_7    : 5;  /* [ 7: 3],       rsvd,        0x0 */
            uint32_t sts_timeout_cm0 : 1;  /* [    8],          r,        0x0 */
            uint32_t reserved_9_31   : 23; /* [31: 9],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } mini_bus_2;

    /* 0xc  reserved */
    uint8_t RESERVED0xc[20];

    /* 0x20 : mini_clock_0 */
    union {
        struct {
            uint32_t cr_cgen_fclk      : 1;  /* [    0],        r/w,        0x1 */
            uint32_t cr_cgen_bclk_pwm  : 1;  /* [    1],        r/w,        0x1 */
            uint32_t cr_cgen_bclk_i2c  : 1;  /* [    2],        r/w,        0x1 */
            uint32_t cr_cgen_bclk_tmr  : 1;  /* [    3],        r/w,        0x1 */
            uint32_t cr_cgen_bclk_uart : 1;  /* [    4],        r/w,        0x1 */
            uint32_t cr_cgen_bclk_spi  : 1;  /* [    5],        r/w,        0x1 */
            uint32_t reserved_6_15     : 10; /* [15: 6],       rsvd,        0x0 */
            uint32_t cr_fclk_en        : 1;  /* [   16],        r/w,        0x1 */
            uint32_t reserved_17       : 1;  /* [   17],       rsvd,        0x0 */
            uint32_t cr_fclk_sel       : 2;  /* [19:18],        r/w,        0x0 */
            uint32_t cr_fclk_div       : 7;  /* [26:20],        r/w,        0x0 */
            uint32_t reserved_27_31    : 5;  /* [31:27],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } mini_clock_0;

    /* 0x24 : mini_clock_1 */
    union {
        struct {
            uint32_t cr_spi_clk_sel : 1;  /* [    0],        r/w,        0x0 */
            uint32_t cr_spi_clk_en  : 1;  /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_7   : 6;  /* [ 7: 2],       rsvd,        0x0 */
            uint32_t cr_spi_clk_div : 5;  /* [12: 8],        r/w,        0x0 */
            uint32_t reserved_13_31 : 19; /* [31:13],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } mini_clock_1;

    /* 0x28  reserved */
    uint8_t RESERVED0x28[24];

    /* 0x40 : mini_rst_0 */
    union {
        struct {
            uint32_t cr_swrst_dma     : 1;  /* [    0],        r/w,        0x0 */
            uint32_t cr_swrst_spi     : 1;  /* [    1],        r/w,        0x0 */
            uint32_t cr_swrst_uart    : 1;  /* [    2],        r/w,        0x0 */
            uint32_t cr_swrst_tmr     : 1;  /* [    3],        r/w,        0x0 */
            uint32_t cr_swrst_i2c     : 1;  /* [    4],        r/w,        0x0 */
            uint32_t cr_swrst_pwm     : 1;  /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_15    : 10; /* [15: 6],       rsvd,        0x0 */
            uint32_t cr_sysreq_rst_en : 1;  /* [   16],        r/w,        0x1 */
            uint32_t cr_lockup_rst_en : 1;  /* [   17],        r/w,        0x0 */
            uint32_t reserved_18_31   : 14; /* [31:18],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } mini_rst_0;

    /* 0x44 : mini_rst_1 */
    union {
        struct {
            uint32_t cr_mini_mcu_sw_rst : 1;  /* [    0],        r/w,        0x0 */
            uint32_t cr_mini_sys_reset  : 1;  /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_31      : 30; /* [31: 2],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } mini_rst_1;

    /* 0x48  reserved */
    uint8_t RESERVED0x48[24];

    /* 0x60 : mini_module_0 */
    union {
        struct {
            uint32_t cr_dma_clk_en : 4;  /* [ 3: 0],        r/w,        0xf */
            uint32_t reserved_4_31 : 28; /* [31: 4],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } mini_module_0;

    /* 0x64  reserved */
    uint8_t RESERVED0x64[28];

    /* 0x80 : mini_rtc */
    union {
        struct {
            uint32_t cr_lpcpu_rtc_div : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_11   : 2;  /* [11:10],       rsvd,        0x0 */
            uint32_t cr_lpcpu_rtc_en  : 1;  /* [   12],        r/w,        0x1 */
            uint32_t cr_lpcpu_rtc_rst : 1;  /* [   13],        r/w,        0x0 */
            uint32_t reserved_14_31   : 18; /* [31:14],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } mini_rtc;

    /* 0x84 : mini_rst_addr */
    union {
        struct {
            uint32_t cr_lpcpu_rst_addr : 32; /* [31: 0],        r/w,        0x0 */
        } BF;
        uint32_t WORD;
    } mini_rst_addr;

    /* 0x88 : mini_int */
    union {
        struct {
            uint32_t cr_lpcpu_trig_int : 1;  /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_31     : 31; /* [31: 1],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } mini_int;
};

typedef volatile struct mini_misc_reg mini_misc_reg_t;

#endif /* __MINI_MISC_REG_H__ */
