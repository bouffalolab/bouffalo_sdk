/**
 * @file bflb_led_matrix.c
 * @brief LED matrix driver
 *
 * Copyright (c) 2026 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

#include "bflb_led_matrix.h"
#include <errno.h>
#include "bflb_mtimer.h"
#include "bflb_dma.h"
#include "bflb_gpio.h"
#include "bflb_clock.h"
#include "bflb_common.h"
#include "hardware/dma_reg.h"
#if defined(BL616CL)
#include "bflb_timer.h"
#include "hardware/timer_reg.h"
#else
#include "bflb_spi.h"
#include "hardware/spi_reg.h"
#endif

/* Global LED matrix buffer */
static led_matrix_buffer_t led_matrix_buffer = { 0 };

/* DMA device and channel */
static struct bflb_device_s *gpio;
static struct bflb_device_s *dma_pixel;
#if defined(BL616CL)
static struct bflb_device_s *timer0;
#define LED_MATRIX_LLI_PER_SLOT 1U
#else
static struct bflb_device_s *spi;
static ATTR_NOCACHE_RAM_SECTION uint8_t spi_dummy;
#define LED_MATRIX_LLI_PER_SLOT 2U
#endif
static uint32_t gpio_background_low;
static uint32_t gpio_background_high;

/*
 * DMA scanning sequence:
 * For each row (8) x each brightness level (104) = 832 total steps
 *   - 100 brightness levels (1-100%)
 *   - 4 blanking levels (blanking period)
 * Each step writes low register + high register = 2 * 4 bytes = 8 bytes
 * Total buffer size: 832 steps x 8 bytes = 6656 bytes
 */
static ATTR_NOCACHE_RAM_SECTION uint32_t dma_scan_data[LED_MATRIX_ROWS * LED_MATRIX_BRIGHTNESS_LEVELS_WITH_BLANKING * 2];
static ATTR_NOCACHE_RAM_SECTION struct bflb_dma_channel_lli_pool_s
    pixel_lli_pool[LED_MATRIX_ROWS * LED_MATRIX_BRIGHTNESS_LEVELS_WITH_BLANKING * LED_MATRIX_LLI_PER_SLOT];

/* Board-owned pin arrays, supplied by the application at initialization. */
static const uint16_t *row_gpios;
static const uint16_t *col_gpios;

/* 3x5 number bitmaps, row-major. 100 = LED on, 0 = LED off */
static const uint8_t number_patterns[10][LED_MATRIX_NUMBER_HEIGHT * LED_MATRIX_NUMBER_WIDTH] = {
    /* 0 */ { 100, 100, 100, 100, 0, 100, 100, 0, 100, 100, 0, 100, 100, 100, 100 },
    /* 1 */ { 0, 100, 0, 100, 100, 0, 0, 100, 0, 0, 100, 0, 100, 100, 100 },
    /* 2 */ { 100, 100, 100, 0, 0, 100, 100, 100, 100, 100, 0, 0, 100, 100, 100 },
    /* 3 */ { 100, 100, 100, 0, 0, 100, 100, 100, 100, 0, 0, 100, 100, 100, 100 },
    /* 4 */ { 100, 0, 100, 100, 0, 100, 100, 100, 100, 0, 0, 100, 0, 0, 100 },
    /* 5 */ { 100, 100, 100, 100, 0, 0, 100, 100, 100, 0, 0, 100, 100, 100, 100 },
    /* 6 */ { 100, 100, 100, 100, 0, 0, 100, 100, 100, 100, 0, 100, 100, 100, 100 },
    /* 7 */ { 100, 100, 100, 0, 0, 100, 0, 100, 0, 100, 0, 0, 100, 0, 0 },
    /* 8 */ { 100, 100, 100, 100, 0, 100, 100, 100, 100, 100, 0, 100, 100, 100, 100 },
    /* 9 */ { 100, 100, 100, 100, 0, 100, 100, 100, 100, 0, 0, 100, 100, 100, 100 }
};

/**
 * @brief Convert LED matrix state to GLB GPIO register values
 *
 * @param[in] row Row index (0-7)
 * @param[in] brightness_level Brightness level (0-103)
 * @param[out] low_reg Low GPIO register value
 * @param[out] high_reg High GPIO register value
 *
 * @retval None
 *
 * @note This follows the same principle as the DPI GPIO implementation.
 *       For each brightness level (1-100%), determine which LEDs should be ON.
 *       For blanking levels (0-3), turn off all LEDs (blanking period).
 *       Supports both common cathode and common anode configurations.
 */
static void led_matrix_to_glb_registers(uint8_t row, uint8_t brightness_level, uint32_t *low_reg, uint32_t *high_reg)
{
    uint32_t low_value = gpio_background_low, high_value = gpio_background_high;

#if LED_MATRIX_COMMON_CATHODE
    /* Common cathode: active high */
    /* Set row control (active high for common cathode configuration) */
    if (row_gpios[row] < 32) {
        low_value |= (1UL << row_gpios[row]);
    } else {
        high_value |= (1UL << (row_gpios[row] - 32));
    }

    /* Check if this is a blanking level (0-3) */
    if (brightness_level < LED_MATRIX_BLANKING_PERCENT) {
        /* Blanking period: all LEDs off, only row line active */
        *low_reg = low_value;
        *high_reg = high_value;
        return;
    }

    /* Normal brightness levels (4-103 representing 1%-100%) */
    uint8_t actual_brightness_level = brightness_level - LED_MATRIX_BLANKING_PERCENT;

    /* Set column controls based on brightness level */
    for (int col = 0; col < LED_MATRIX_COLS; col++) {
        uint8_t pixel_brightness = led_matrix_buffer.pixels[row][col].brightness;

        /* LED should be ON if its brightness level > current scanning level */
        /* actual_brightness_level ranges from 0-99 (representing 1%-100%) */
        if (pixel_brightness > actual_brightness_level) {
            if (col_gpios[col] < 32) {
                low_value |= (1UL << col_gpios[col]);
            } else {
                high_value |= (1UL << (col_gpios[col] - 32));
            }
        }
    }
#else
    /* Common anode: active low */
    /* Set row control (active low for common anode configuration) */
    /* Set all rows to 0, then set current row to 1 (inactive) */
    for (int r = 0; r < LED_MATRIX_ROWS; r++) {
        if (r != row) {
            if (row_gpios[r] < 32) {
                low_value |= (1UL << row_gpios[r]);
            } else {
                high_value |= (1UL << (row_gpios[r] - 32));
            }
        }
    }

    /* IMPORTANT: Set ALL columns to high (inactive) first, including unused ones */
    /* This prevents ghost lighting on unused column pins */
    for (int col = 0; col < LED_MATRIX_COLS; col++) {
        if (col_gpios[col] < 32) {
            low_value |= (1UL << col_gpios[col]);
        } else {
            high_value |= (1UL << (col_gpios[col] - 32));
        }
    }

    /* Check if this is a blanking level (0-3) */
    if (brightness_level < LED_MATRIX_BLANKING_PERCENT) {
        /* Blanking period: all LEDs off, all columns already set to high above */
        *low_reg = low_value;
        *high_reg = high_value;
        return;
    }

    /* Normal brightness levels (4-103 representing 1%-100%) */
    uint8_t actual_brightness_level = brightness_level - LED_MATRIX_BLANKING_PERCENT;

    /* Set column controls based on brightness level (inverted for common anode) */
    for (int col = 0; col < LED_MATRIX_COLS; col++) {
        uint8_t pixel_brightness = led_matrix_buffer.pixels[row][col].brightness;

        /* LED should be ON (active low) if brightness > current level */
        /* LED should be OFF (active high) if brightness <= current level */
        if (pixel_brightness > actual_brightness_level) {
            /* LED ON: set column low (active), clear the bit we set earlier */
            if (col_gpios[col] < 32) {
                low_value &= ~(1UL << col_gpios[col]);
            } else {
                high_value &= ~(1UL << (col_gpios[col] - 32));
            }
        }
        /* else: LED OFF: keep column high (inactive), already set above */
    }
#endif

    *low_reg = low_value;
    *high_reg = high_value;
}

/**
 * @brief Initialize GPIO pins for LED matrix
 *
 * @param[in] None
 *
 * @retval None
 *
 * @note Similar to DPI GPIO initialization.
 */
static void led_matrix_gpio_init(void)
{
    gpio = bflb_device_get_by_name("gpio");

    /* Preserve unrelated output latches throughout the DMA scan. */
    gpio_background_low = getreg32(GLB_GPIO_OUTPUT_LOW);
    gpio_background_high = getreg32(GLB_GPIO_OUTPUT_HIGH);
    for (uint32_t i = 0; i < LED_MATRIX_ROWS + LED_MATRIX_COLS; i++) {
        uint16_t pin = i < LED_MATRIX_ROWS ? row_gpios[i] : col_gpios[i - LED_MATRIX_ROWS];
        if (pin < 32) {
            gpio_background_low &= ~(1UL << pin);
        } else {
            gpio_background_high &= ~(1UL << (pin - 32));
        }
    }

    /* Configure all GPIO pins as outputs */
    for (int i = 0; i < LED_MATRIX_ROWS; i++) {
        bflb_gpio_init(gpio, row_gpios[i], GPIO_OUTPUT | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_2);
        /* Configure for direct GLB control */
        *(volatile uint32_t *)(0x200008c4 + 4 * row_gpios[i]) &= ~(1 << 24);
        *(volatile uint32_t *)(0x200008c4 + 4 * row_gpios[i]) &= ~(3 << 30);
    }

    for (int i = 0; i < LED_MATRIX_COLS; i++) {
        bflb_gpio_init(gpio, col_gpios[i], GPIO_OUTPUT | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_2);
        /* Configure for direct GLB control */
        *(volatile uint32_t *)(0x200008c4 + 4 * col_gpios[i]) &= ~(1 << 24);
        *(volatile uint32_t *)(0x200008c4 + 4 * col_gpios[i]) &= ~(3 << 30);
    }
}

/**
 * @brief Initialize TIMER0 for DMA triggering
 *
 * @param[in] None
 *
 * @retval None
 *
 * @note TIMER0 compare0 generates one DMA request per GPIO word.
 */
#if defined(BL616CL)
static void led_matrix_timing_init(void)
{
    timer0 = bflb_device_get_by_name("timer0");

    struct bflb_timer_config_s timer_config = {
        .counter_mode = TIMER_COUNTER_MODE_PROLOAD,
        .clock_source = TIMER_CLKSRC_XTAL,
        .clock_div = LED_MATRIX_TIMER_CLOCK_DIV, /* 40MHz, 1 tick = 25ns */
        .trigger_comp_id = TIMER_COMP_ID_0,
        .comp0_val = LED_MATRIX_TIMER_COMP_TICKS,
        .comp1_val = 0xffffffff,
        .comp2_val = 0xffffffff,
        .preload_val = 0,
    };

    bflb_timer_init(timer0, &timer_config);
    bflb_timer_compint_mask(timer0, TIMER_COMP_ID_0, true);
    bflb_timer_compint_clear(timer0, TIMER_COMP_ID_0);

    uint32_t regval = getreg32(timer0->reg_base + TIMER_TIER0_OFFSET);
    regval &= ~TIMER0_DMA_REQ_EN_MASK;
    regval |= (TIMER_DMA_REQUEST_COMP_ID_0 << TIMER0_DMA_REQ_EN_SHIFT);
    putreg32(regval, timer0->reg_base + TIMER_TIER0_OFFSET);
}

#else
static void led_matrix_timing_init(void)
{
    spi = bflb_device_get_by_name("spi0");
    struct bflb_spi_config_s config = {
        .freq = LED_MATRIX_REFRESH_RATE * LED_MATRIX_ROWS * LED_MATRIX_BRIGHTNESS_LEVELS_WITH_BLANKING * 8U,
        .role = SPI_ROLE_MASTER,
        .mode = SPI_MODE0,
        .data_width = SPI_DATA_WIDTH_8BIT,
        .bit_order = SPI_BIT_MSB,
        .byte_order = SPI_BYTE_LSB,
        .tx_fifo_threshold = 0,
        .rx_fifo_threshold = 0,
    };
    bflb_spi_init(spi, &config);
    bflb_spi_deinit(spi);
    /* Internal pacing only: do not assign any GPIO to SPI. Keep CS asserted
     * across frames and discard RX data so the RX FIFO cannot overflow. */
    uint32_t regval = getreg32(spi->reg_base + SPI_CONFIG_OFFSET);
    regval |= SPI_CR_SPI_M_CONT_EN;
    putreg32(regval, spi->reg_base + SPI_CONFIG_OFFSET);
    /* Ignore the full 0-31 frame window during continuous transmission. */
    bflb_spi_feature_control(spi, SPI_CMD_RX_IGNORE_ENABLE, 31);
    printf("Scan pacing: SPI0 TX, requested clock %lu Hz\r\n", (unsigned long)config.freq);
}
#endif

/**
 * @brief Build complete DMA scanning sequence
 *
 * @param[in] None
 *
 * @retval None
 *
 * @note Creates 832-step sequence: 8 rows x 104 timing levels (100 brightness + 4 blanking).
 *       Each level represents 1% of row time, with blanking at the beginning.
 */
static void led_matrix_build_scan_sequence(void)
{
    uint32_t scan_index = 0;

    printf("Building scan sequence: %d rows x %d timing levels = %d total steps\r\n", LED_MATRIX_ROWS,
           LED_MATRIX_BRIGHTNESS_LEVELS_WITH_BLANKING, LED_MATRIX_ROWS * LED_MATRIX_BRIGHTNESS_LEVELS_WITH_BLANKING);
#if defined(BL616CL)
    printf("Row time: %d us, Slot: %d us, Blanking: %d us\r\n", LED_MATRIX_ROW_TIME_US, LED_MATRIX_ONE_PERCENT_US,
           LED_MATRIX_ONE_PERCENT_US * LED_MATRIX_BLANKING_PERCENT);
#endif

    for (uint8_t row = 0; row < LED_MATRIX_ROWS; row++) {
        for (uint8_t level = 0; level < LED_MATRIX_BRIGHTNESS_LEVELS_WITH_BLANKING; level++) {
            uint32_t low_reg = 0, high_reg = 0;

            led_matrix_to_glb_registers(row, level, &low_reg, &high_reg);

            dma_scan_data[scan_index * 2] = low_reg;
            dma_scan_data[scan_index * 2 + 1] = high_reg;

            scan_index++;
        }
    }

    bflb_l1c_dcache_clean_range(dma_scan_data, sizeof(dma_scan_data));

    printf("Scan sequence built successfully, total buffer size: %d bytes\r\n", (int)(sizeof(dma_scan_data)));
    printf("Refresh rate: %d Hz, Effective brightness precision: 1/%d\r\n", LED_MATRIX_REFRESH_RATE,
           LED_MATRIX_BRIGHTNESS_LEVELS_WITH_BLANKING);
}

/**
 * @brief Initialize DMA channel with LLI support
 *
 * @param[in] None
 *
 * @retval None
 */
static void led_matrix_dma_init(void)
{
    dma_pixel = bflb_device_get_by_name("dma0_ch0");
    struct bflb_dma_channel_config_s pixel_config = {
#if defined(BL616CL)
        .direction = DMA_PERIPH_TO_MEMORY,
        .src_req = DMA_REQUEST_TIMER0,
        .dst_req = DMA_REQUEST_NONE,
#else
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_SPI0_TX,
#endif
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_32BIT,
        .dst_width = DMA_DATA_WIDTH_32BIT,
    };
    bflb_dma_channel_init(dma_pixel, &pixel_config);
}

/**
 * @brief Start DMA with linked-list for continuous scanning
 *
 * @param[in] None
 *
 * @retval None
 *
 * @note Creates a circular LLI chain for continuous PWM generation.
 */
static void led_matrix_dma_start(void)
{
    uint32_t total_scans = LED_MATRIX_ROWS * LED_MATRIX_BRIGHTNESS_LEVELS_WITH_BLANKING;

    printf("Starting DMA with %lu LLI nodes for continuous scanning\r\n",
           (unsigned long)(total_scans * LED_MATRIX_LLI_PER_SLOT));

#if defined(BL616CL)
    bflb_timer_stop(timer0);
    bflb_timer_compint_clear(timer0, TIMER_COMP_ID_0);
#else
    bflb_spi_link_txdma(spi, false);
    bflb_spi_deinit(spi);
    bflb_spi_feature_control(spi, SPI_CMD_CLEAR_TX_FIFO, 0);
    bflb_spi_feature_control(spi, SPI_CMD_CLEAR_RX_FIFO, 0);
    /* Start with a full FIFO, avoiding a rapid initial run of scan slots. */
    for (uint32_t i = 0; i < SPI_FIFO_BYTE_NUM_MAX; i++) {
        putreg8(0, spi->reg_base + SPI_FIFO_WDATA_OFFSET);
    }
#endif
    bflb_dma_channel_stop(dma_pixel);

    /* Obtain the channel control template, then build the entire ring before
     * loading its first descriptor. Never reload hardware for each node. */
    union bflb_dma_lli_control_s control;
    control.WORD = getreg32(dma_pixel->reg_base + DMA_CxCONTROL_OFFSET);
    control.bits.TransferSize = 2;
    control.bits.SWidth = DMA_DATA_WIDTH_32BIT;
    control.bits.DWidth = DMA_DATA_WIDTH_32BIT;
    control.bits.SI = 1;
    control.bits.DI = 1;
    control.bits.I = 0;
    uint32_t node_count = total_scans * LED_MATRIX_LLI_PER_SLOT;
    for (uint32_t i = 0; i < total_scans; i++) {
        uint32_t node = i * LED_MATRIX_LLI_PER_SLOT;
        pixel_lli_pool[node].src_addr = (uint32_t)(uintptr_t)&dma_scan_data[i * 2];
        pixel_lli_pool[node].dst_addr = GLB_GPIO_OUTPUT_LOW;
        pixel_lli_pool[node].control = control;
#if !defined(BL616CL)
        /* The same channel first writes both GPIO words while TX requests are
         * asserted, then supplies exactly one byte to clear the FIFO request.
         * There is no race between two channels sharing a request signal. */
        pixel_lli_pool[node + 1].src_addr = (uint32_t)(uintptr_t)&spi_dummy;
        pixel_lli_pool[node + 1].dst_addr = spi->reg_base + SPI_FIFO_WDATA_OFFSET;
        pixel_lli_pool[node + 1].control = control;
        pixel_lli_pool[node + 1].control.bits.TransferSize = 1;
        pixel_lli_pool[node + 1].control.bits.SWidth = DMA_DATA_WIDTH_8BIT;
        pixel_lli_pool[node + 1].control.bits.DWidth = DMA_DATA_WIDTH_8BIT;
        pixel_lli_pool[node + 1].control.bits.SI = 0;
        pixel_lli_pool[node + 1].control.bits.DI = 0;
#endif
    }
    for (uint32_t i = 0; i < node_count; i++) {
        pixel_lli_pool[i].nextlli = (uint32_t)(uintptr_t)&pixel_lli_pool[(i + 1) % node_count];
    }
    bflb_l1c_dcache_clean_range(pixel_lli_pool, sizeof(pixel_lli_pool));
    putreg32(pixel_lli_pool[0].src_addr, dma_pixel->reg_base + DMA_CxSRCADDR_OFFSET);
    putreg32(pixel_lli_pool[0].dst_addr, dma_pixel->reg_base + DMA_CxDSTADDR_OFFSET);
    putreg32(pixel_lli_pool[0].nextlli, dma_pixel->reg_base + DMA_CxLLI_OFFSET);
    putreg32(pixel_lli_pool[0].control.WORD, dma_pixel->reg_base + DMA_CxCONTROL_OFFSET);
    bflb_dma_channel_start(dma_pixel);
#if defined(BL616CL)
    bflb_timer_start(timer0);
#else
    bflb_spi_link_txdma(spi, true);
    uint32_t regval = getreg32(spi->reg_base + SPI_CONFIG_OFFSET);
    putreg32(regval | SPI_CR_SPI_M_EN, spi->reg_base + SPI_CONFIG_OFFSET);
#endif

    printf("DMA started successfully - continuous scanning active\r\n");
    printf("Each complete cycle takes %d steps (%d rows x %d timing levels)\r\n", total_scans, LED_MATRIX_ROWS,
           LED_MATRIX_BRIGHTNESS_LEVELS_WITH_BLANKING);
    printf("Target matrix refresh rate: %d Hz, Target row frequency: %d Hz\r\n", LED_MATRIX_REFRESH_RATE,
           LED_MATRIX_REFRESH_RATE * LED_MATRIX_ROWS);
}

/**
 * @brief Stop DMA scanning
 *
 * @param[in] None
 *
 * @retval None
 */
static void led_matrix_dma_stop(void)
{
#if defined(BL616CL)
    if (timer0) {
        bflb_timer_stop(timer0);
    }
#else
    if (spi) {
        bflb_spi_link_txdma(spi, false);
        bflb_spi_deinit(spi);
    }
#endif
    if (dma_pixel) {
        bflb_dma_channel_stop(dma_pixel);
    }
}

int led_matrix_init(const led_matrix_gpio_config_t *config)
{
    if (led_matrix_buffer.is_enabled) {
        return -EBUSY;
    }
    if (config == NULL) {
        return -EINVAL;
    }
    /* Reject invalid or shared row/column pins before touching hardware. */
    uint64_t used_pins = 0;
    for (uint32_t i = 0; i < LED_MATRIX_ROWS + LED_MATRIX_COLS; i++) {
        uint16_t pin = i < LED_MATRIX_ROWS ? config->row_gpios[i] : config->col_gpios[i - LED_MATRIX_ROWS];
        if (pin >= GPIO_PIN_MAX || (used_pins & (1ULL << pin))) {
            return -EINVAL;
        }
        used_pins |= 1ULL << pin;
    }
    row_gpios = config->row_gpios;
    col_gpios = config->col_gpios;

    printf("Initializing LED Matrix Driver\r\n");
    printf("Matrix: %dx%d, Brightness: %d levels (1%% precision)\r\n", LED_MATRIX_ROWS, LED_MATRIX_COLS,
           LED_MATRIX_BRIGHTNESS_LEVELS);

    led_matrix_gpio_init();
    led_matrix_timing_init();
    led_matrix_dma_init();

    led_matrix_clear();
    led_matrix_build_scan_sequence();

    led_matrix_buffer.is_enabled = 0;

    printf("LED Matrix initialization completed\r\n");
    return 0;
}

void led_matrix_on(void)
{
    if (led_matrix_buffer.is_enabled) {
        printf("LED matrix is already enabled\r\n");
        return;
    }

    printf("Turning LED matrix ON\r\n");

    led_matrix_build_scan_sequence();
    led_matrix_dma_start();

    led_matrix_buffer.is_enabled = 1;
    printf("LED matrix is now active with %d%% brightness precision\r\n", 100 / LED_MATRIX_BRIGHTNESS_LEVELS);
}

void led_matrix_off(void)
{
    if (!led_matrix_buffer.is_enabled) {
        printf("LED matrix is already disabled\r\n");
        return;
    }

    printf("Turning LED matrix OFF\r\n");

    led_matrix_dma_stop();

#if LED_MATRIX_COMMON_CATHODE
    /* Common cathode: set all to low (inactive) */
    for (int i = 0; i < LED_MATRIX_ROWS; i++) {
        bflb_gpio_reset(gpio, row_gpios[i]);
    }
    for (int i = 0; i < LED_MATRIX_COLS; i++) {
        bflb_gpio_reset(gpio, col_gpios[i]);
    }
#else
    /* Common anode: set all to high (inactive) */
    for (int i = 0; i < LED_MATRIX_ROWS; i++) {
        bflb_gpio_set(gpio, row_gpios[i]);
    }
    for (int i = 0; i < LED_MATRIX_COLS; i++) {
        bflb_gpio_set(gpio, col_gpios[i]);
    }
#endif

    led_matrix_buffer.is_enabled = 0;
    printf("LED matrix disabled\r\n");
}

void led_matrix_set_pixel(uint8_t row, uint8_t col, uint8_t brightness)
{
    if (row >= LED_MATRIX_ROWS || col >= LED_MATRIX_COLS) {
        return;
    }

    /* Clamp brightness to 0-100 range */
    if (brightness > 100) {
        brightness = 100;
    }

    led_matrix_buffer.pixels[row][col].brightness = brightness;
}

uint8_t led_matrix_get_pixel(uint8_t row, uint8_t col)
{
    if (row >= LED_MATRIX_ROWS || col >= LED_MATRIX_COLS) {
        return 0;
    }

    return led_matrix_buffer.pixels[row][col].brightness;
}

void led_matrix_clear(void)
{
    for (int row = 0; row < LED_MATRIX_ROWS; row++) {
        for (int col = 0; col < LED_MATRIX_COLS; col++) {
            led_matrix_buffer.pixels[row][col].brightness = 0;
        }
    }
}

void led_matrix_update(void)
{
    if (led_matrix_buffer.is_enabled) {
        /* Rebuild in place; DMA may observe mixed frame data during this update. */
        led_matrix_build_scan_sequence();
    }
}

void led_matrix_set_row(uint8_t row, const uint8_t *brightness)
{
    if (row >= LED_MATRIX_ROWS || brightness == NULL) {
        return;
    }

    for (int col = 0; col < LED_MATRIX_COLS; col++) {
        uint8_t value = brightness[col];
        if (value > 100)
            value = 100;
        led_matrix_buffer.pixels[row][col].brightness = value;
    }
}

void led_matrix_set_column(uint8_t col, const uint8_t *brightness)
{
    if (col >= LED_MATRIX_COLS || brightness == NULL) {
        return;
    }

    for (int row = 0; row < LED_MATRIX_ROWS; row++) {
        uint8_t value = brightness[row];
        if (value > 100)
            value = 100;
        led_matrix_buffer.pixels[row][col].brightness = value;
    }
}

void led_matrix_set_rectangle(uint8_t row, uint8_t col, uint8_t height, uint8_t width, const uint8_t *brightness)
{
    if (row >= LED_MATRIX_ROWS || col >= LED_MATRIX_COLS || height == 0 || width == 0 || brightness == NULL) {
        return;
    }

    uint8_t end_row = row + height;
    uint8_t end_col = col + width;

    if (end_row > LED_MATRIX_ROWS || end_row < row) {
        end_row = LED_MATRIX_ROWS;
    }
    if (end_col > LED_MATRIX_COLS || end_col < col) {
        end_col = LED_MATRIX_COLS;
    }

    for (uint8_t r = row; r < end_row; r++) {
        for (uint8_t c = col; c < end_col; c++) {
            uint8_t value = brightness[(r - row) * width + (c - col)];
            if (value > 100) {
                value = 100;
            }
            led_matrix_buffer.pixels[r][c].brightness = value;
        }
    }
}

void led_matrix_set_number(uint8_t row, uint8_t col, uint8_t num)
{
    if (row >= LED_MATRIX_ROWS || col >= LED_MATRIX_COLS || num > 9) {
        printf("config error!\r\n");
        return;
    }

    led_matrix_set_rectangle(row, col, LED_MATRIX_NUMBER_HEIGHT, LED_MATRIX_NUMBER_WIDTH, number_patterns[num]);
}
