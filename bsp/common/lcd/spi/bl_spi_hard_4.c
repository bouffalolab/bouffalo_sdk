#include "../lcd.h"

#if (defined(LCD_SPI_INTERFACE_TYPE) && (LCD_SPI_INTERFACE_TYPE == 1))

#if (__has_include("bflb_spi.h"))

#include "bl_spi_hard_4.h"
#include "bflb_spi.h"
#include "bflb_dma.h"
#include "bflb_gpio.h"
#include "bflb_l1c.h"

#define LCD_SPI_HARD_4_DMA_LLI_NUM (LCD_SPI_HARD_4_PIXEL_CNT_MAX / 4064 + 1)

/* asynchronous flush interrupt callback */
typedef void (*spi_callback)(void);
static volatile spi_callback spi_async_callback = NULL;
static volatile bool spi_async_callback_en_flag = true;

/* dma device */
static struct bflb_device_s *spi_dma_hd;
/* spi */
static struct bflb_device_s *spi_hd;
/* goio */
static struct bflb_device_s *gpio;

/* The memory space of DMA */
static struct bflb_dma_channel_lli_pool_s dma_tx_llipool[LCD_SPI_HARD_4_DMA_LLI_NUM];
static struct bflb_dma_channel_lli_transfer_s dma_tx_transfers[1];
static uint32_t spi_tx_fifo_address;

static volatile bool spi_async_flag = false;
static volatile bool spi_async_cycle_fill_flag = false;

static uint8_t pixel_format;

static void spi_dma_callback(void *arg)
{
    /* Wait for the SPI bus to be idle */
    while (bflb_spi_isbusy(spi_hd)) {
        __ASM volatile("nop");
    };

    /* Switch the SPI to non-DMA mode */
    bflb_spi_link_txdma(spi_hd, false);

    /* clear rx fifo */
    bflb_spi_feature_control(spi_hd, SPI_CMD_CLEAR_RX_FIFO, 0);

    LCD_SPI_HARD_4_CS_HIGH;

    /* 8-bit data */
    bflb_spi_feature_control(spi_hd, SPI_CMD_SET_DATA_WIDTH, SPI_DATA_WIDTH_8BIT);

    if (spi_async_flag == false && spi_async_cycle_fill_flag == false) {
        return;
    }

    if (spi_async_cycle_fill_flag == true) {
        /* enable dma addr increment */
        bflb_dma_feature_control(spi_dma_hd, DMA_CMD_SET_SRCADDR_INCREMENT, true);
    }

    spi_async_flag = false;
    spi_async_cycle_fill_flag = false;

    /* async callback */
    if (spi_async_callback_en_flag && spi_async_callback != NULL) {
        spi_async_callback();
    }
}

int lcd_spi_hard_4_async_callback_register(void (*callback)(void))
{
    spi_async_callback = callback;
    return 0;
}

int lcd_spi_hard_4_async_callback_enable(bool enable)
{
    spi_async_callback_en_flag = enable;
    return 0;
}

int lcd_spi_hard_4_init(lcd_spi_hard_4_init_t *dbi_parra)
{
    /* spi */
    struct bflb_spi_config_s spi_cfg = {
        .freq = dbi_parra->clock_freq,
        .role = SPI_ROLE_MASTER,
        .mode = SPI_MODE3,
        .data_width = SPI_DATA_WIDTH_8BIT,
        .bit_order = SPI_BIT_MSB,
        .byte_order = SPI_BYTE_LSB,
        .tx_fifo_threshold = 0,
        .rx_fifo_threshold = 0,
    };

    /* dma cfg */
    struct bflb_dma_channel_config_s dma_spi_tx_cfg = {
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_SPI0_TX,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR4,
        .dst_burst_count = DMA_BURST_INCR4,
        .src_width = DMA_DATA_WIDTH_16BIT,
        .dst_width = DMA_DATA_WIDTH_16BIT,
    };

    pixel_format = dbi_parra->pixel_format;

#if (SPI_FIFO_WIDTH_VARIABLE_SUPPORT)
    /* SPI support burst*4 */
    spi_cfg.tx_fifo_threshold = (2 * 4 - 1);
    spi_cfg.rx_fifo_threshold = (2 * 4 - 1);
    spi_cfg.byte_order = SPI_BYTE_MSB;
#elif
    spi_cfg.tx_fifo_threshold = 4 - 1;
    spi_cfg.rx_fifo_threshold = 4 - 1;
#endif

    spi_hd = bflb_device_get_by_name(LCD_SPI_HARD_4_NAME);

    /* CS and DC pin init */
    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, LCD_SPI_HARD_4_PIN_CS, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, LCD_SPI_HARD_4_PIN_DC, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    LCD_SPI_HARD_4_CS_HIGH;
    LCD_SPI_HARD_4_DC_HIGH;

    if (spi_hd->idx == 0) {
        bflb_gpio_init(gpio, LCD_SPI_HARD_4_PIN_CLK, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
        bflb_gpio_init(gpio, LCD_SPI_HARD_4_PIN_DAT, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    }
#if defined(GPIO_FUNC_SPI1)
    else if (spi_hd->idx == 1) {
        bflb_gpio_init(gpio, LCD_SPI_HARD_4_PIN_CLK, GPIO_FUNC_SPI1 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
        bflb_gpio_init(gpio, LCD_SPI_HARD_4_PIN_DAT, GPIO_FUNC_SPI1 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    }

#endif

    /* spi init */
    bflb_spi_init(spi_hd, &spi_cfg);

    /* spi enabled continuous mode */
    // bflb_spi_feature_control(spi_hd, SPI_CMD_SET_CS_INTERVAL, true);

    if (pixel_format == LCD_SPI_LCD_PIXEL_FORMAT_RGB565) {
        dma_spi_tx_cfg.src_width = DMA_DATA_WIDTH_16BIT;
        dma_spi_tx_cfg.dst_width = DMA_DATA_WIDTH_16BIT;
    }

    if (spi_hd->idx == 0) {
        dma_spi_tx_cfg.dst_req = DMA_REQUEST_SPI0_TX;
        spi_tx_fifo_address = DMA_ADDR_SPI0_TDR;
    }
#if defined(DMA_REQUEST_SPI1_TX)
    else if (spi_hd->idx == 1) {
        dma_spi_tx_cfg.dst_req = DMA_REQUEST_SPI1_TX;
        spi_tx_fifo_address = DMA_ADDR_SPI1_TDR;
    }
#endif

    /* dma init */
    spi_dma_hd = bflb_device_get_by_name(LCD_SPI_HARD_4_DMA_NAME);
    bflb_dma_channel_init(spi_dma_hd, &dma_spi_tx_cfg);

    /* dma int cfg */
    bflb_dma_channel_irq_attach(spi_dma_hd, spi_dma_callback, NULL);

    return 0;
}

#if (SPI_FIFO_WIDTH_VARIABLE_SUPPORT == 0)
static int lcd_swap_color_data16(uint16_t *dst, uint16_t *src, uint32_t color_num)
{
    for (size_t i = 0; i < color_num; i++) {
        dst[i] = (src[i] << 8) | (src[i] >> 8);
    }
    return 0;
}
#endif

int lcd_spi_hard_4_is_busy(void)
{
    if (spi_async_flag || spi_async_cycle_fill_flag) {
        return 1;
    } else {
        return 0;
    }
}

int lcd_spi_hard_4_transmit_cmd_para(uint8_t cmd, uint32_t *para, size_t para_num)
{
    LCD_SPI_HARD_4_DC_LOW;
    LCD_SPI_HARD_4_CS_LOW;

    /* send cmd */
    bflb_spi_poll_send(spi_hd, cmd);

    LCD_SPI_HARD_4_DC_HIGH;

    /* send para */
    if (para_num && para != NULL) {
        bflb_spi_poll_exchange(spi_hd, para, NULL, para_num);
    }

    LCD_SPI_HARD_4_CS_HIGH;

    return 0;
}

int lcd_spi_hard_4_transmit_cmd_pixel_sync(uint8_t cmd, uint32_t *pixel, size_t pixel_num)
{
#if (SPI_FIFO_WIDTH_VARIABLE_SUPPORT == 0)
    lcd_swap_color_data16(picture, picture, pixel_num);
#endif

    LCD_SPI_HARD_4_DC_LOW;
    LCD_SPI_HARD_4_CS_LOW;

    /* send cmd */
    bflb_spi_poll_send(spi_hd, cmd);

    LCD_SPI_HARD_4_DC_HIGH;

    if (pixel_num && pixel != NULL) {
        if (pixel_format == LCD_SPI_LCD_PIXEL_FORMAT_RGB565) {
            /* spi 16-bit data mode */
            bflb_spi_feature_control(spi_hd, SPI_CMD_SET_DATA_WIDTH, SPI_DATA_WIDTH_16BIT);
        }

        /* send pixel */
        if (pixel_format == LCD_SPI_LCD_PIXEL_FORMAT_RGB565) {
            bflb_spi_poll_exchange(spi_hd, pixel, NULL, pixel_num * 2);
        }

        /* spi 8-bit data mode */
        bflb_spi_feature_control(spi_hd, SPI_CMD_SET_DATA_WIDTH, SPI_DATA_WIDTH_8BIT);
    }

    LCD_SPI_HARD_4_CS_HIGH;

    return 0;
}

int lcd_spi_hard_4_transmit_cmd_pixel_async(uint8_t cmd, uint32_t *pixel, size_t pixel_num)
{
#if (SPI_FIFO_WIDTH_VARIABLE_SUPPORT == 0)
    lcd_swap_color_data16(picture, picture, pixel_num);
#endif

    LCD_SPI_HARD_4_DC_LOW;
    LCD_SPI_HARD_4_CS_LOW;

    /* send cmd */
    bflb_spi_poll_send(spi_hd, cmd);

    LCD_SPI_HARD_4_DC_HIGH;

    if (pixel_num && pixel != NULL) {
        spi_async_flag = true;

        if (pixel_format == LCD_SPI_LCD_PIXEL_FORMAT_RGB565) {
            /* spi 16-bit data mode */
            bflb_spi_feature_control(spi_hd, SPI_CMD_SET_DATA_WIDTH, SPI_DATA_WIDTH_16BIT);
        }

        /* enable spi dma mode */
        bflb_spi_link_txdma(spi_hd, true);

        dma_tx_transfers[0].src_addr = (uint32_t)(uintptr_t)pixel;
        dma_tx_transfers[0].dst_addr = spi_tx_fifo_address;
        if (pixel_format == LCD_SPI_LCD_PIXEL_FORMAT_RGB565) {
            dma_tx_transfers[0].nbytes = pixel_num * 2;

            /* clean cache */
            bflb_l1c_dcache_clean_range((void *)pixel, pixel_num * 2);
        }

        bflb_dma_channel_lli_reload(spi_dma_hd, dma_tx_llipool, LCD_SPI_HARD_4_DMA_LLI_NUM, dma_tx_transfers, 1);
        bflb_dma_channel_start(spi_dma_hd);
    } else {
        LCD_SPI_HARD_4_CS_HIGH;
    }
    return 0;
}

int lcd_spi_hard_4_transmit_cmd_pixel_fill_sync(uint8_t cmd, uint32_t pixel_val, size_t pixel_num)
{
#if (SPI_FIFO_WIDTH_VARIABLE_SUPPORT == 0)
    pixel_val = ((pixel_val >> 8) & 0xFF) | pixel_val << 8;
#endif

    LCD_SPI_HARD_4_DC_LOW;
    LCD_SPI_HARD_4_CS_LOW;

    /* send cmd */
    bflb_spi_poll_send(spi_hd, cmd);

    LCD_SPI_HARD_4_DC_HIGH;

    if (pixel_num) {
        if (pixel_format == LCD_SPI_LCD_PIXEL_FORMAT_RGB565) {
            /* spi 16-bit data mode */
            bflb_spi_feature_control(spi_hd, SPI_CMD_SET_DATA_WIDTH, SPI_DATA_WIDTH_16BIT);
        }

        /* send pixel */
        for (size_t i = 0; i < pixel_num; i++) {
            bflb_spi_poll_send(spi_hd, pixel_val);
        }

        /* spi 8-bit data mode */
        bflb_spi_feature_control(spi_hd, SPI_CMD_SET_DATA_WIDTH, SPI_DATA_WIDTH_8BIT);
    }

    LCD_SPI_HARD_4_CS_HIGH;

    return 0;
}

int lcd_spi_hard_4_transmit_cmd_pixel_fill_async(uint8_t cmd, uint32_t pixel_val, size_t pixel_num)
{
#if (SPI_FIFO_WIDTH_VARIABLE_SUPPORT == 0)
    pixel_val = ((pixel_val >> 8) & 0xFF) | pixel_val << 8;
#endif

    LCD_SPI_HARD_4_DC_LOW;
    LCD_SPI_HARD_4_CS_LOW;

    /* send cmd */
    bflb_spi_poll_send(spi_hd, cmd);

    LCD_SPI_HARD_4_DC_HIGH;

    if (pixel_num) {
        spi_async_cycle_fill_flag = true;

        if (pixel_format == LCD_SPI_LCD_PIXEL_FORMAT_RGB565) {
            /* spi 16-bit data mode */
            bflb_spi_feature_control(spi_hd, SPI_CMD_SET_DATA_WIDTH, SPI_DATA_WIDTH_16BIT);
        }

        /* enable spi dma mode */
        bflb_spi_link_txdma(spi_hd, true);

        /* disable dma src_addr_inc */
        bflb_dma_feature_control(spi_hd, DMA_CMD_SET_SRCADDR_INCREMENT, false);

        dma_tx_transfers[0].src_addr = (uint32_t)(uintptr_t)&pixel_val;
        dma_tx_transfers[0].dst_addr = spi_tx_fifo_address;
        if (pixel_format == LCD_SPI_LCD_PIXEL_FORMAT_RGB565) {
            dma_tx_transfers[0].nbytes = pixel_num * 2;
        }

        /* clean cache */
        bflb_l1c_dcache_clean_range((void *)&pixel_val, sizeof(pixel_val));

        bflb_dma_channel_lli_reload(spi_dma_hd, dma_tx_llipool, LCD_SPI_HARD_4_DMA_LLI_NUM, dma_tx_transfers, 1);
        bflb_dma_channel_start(spi_dma_hd);
    } else {
        LCD_SPI_HARD_4_CS_HIGH;
    }

    return 0;
}

#else
#error "Devices that do not support SPI! Replace the driver port (lcd_conf.h)"
#endif

#endif