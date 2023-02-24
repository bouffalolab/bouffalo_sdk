#include "../lcd.h"

#if (defined(LCD_DBI_INTERFACE_TYPE) && (LCD_DBI_INTERFACE_TYPE == 2))

#if (defined(BL616) && __has_include("bflb_pec_ip.h"))

#include "bl_mipi_dbi_typeb_pec.h"
#include "bflb_pec_ip.h"
#include "bflb_dma.h"
#include "bflb_gpio.h"
#include "bflb_l1c.h"

#define LCD_DBI_PEC_DMA_LLI_NUM (DBI_PEC_DBI_DATA_SIZE_MAX / 4 / 4064 + 1)

/* asynchronous flush interrupt callback */
typedef void (*pec_dbi_b_callback)(void);
static volatile pec_dbi_b_callback pec_dbi_b_async_callback = NULL;
static volatile bool pec_dbi_b_async_callback_en_flag = true;

/* dma device */
static struct bflb_device_s *pec_dbi_b_dma_handle;
static struct bflb_dma_channel_config_s dma_pec_config;
/* pec dbi typeB device */
static struct bflb_pec_dbi_b_s pec_dbi_b_handle;

/* The memory space of DMA */
static struct bflb_dma_channel_lli_pool_s dma_tx_llipool[LCD_DBI_PEC_DMA_LLI_NUM];
static struct bflb_dma_channel_lli_transfer_s dma_tx_transfers[1];

static volatile uint32_t pixel_format;

static volatile bool pec_dbi_b_async_flag = false;
static volatile bool pec_dbi_b_async_cycle_fill_flag = false;

/**
 * @brief pec_dbi_b_dma_flush_callback
 *
 * @return
 */
static void pec_dbi_b_dma_flush_callback(void *args)
{
    /* disable pec dma */
    bflb_pec_dbi_b_dma_enable(&pec_dbi_b_handle, false);

    if (pec_dbi_b_async_flag == false && pec_dbi_b_async_cycle_fill_flag == false) {
        return;
    }

    if (pec_dbi_b_async_cycle_fill_flag == true) {
        /* enable dma addr increment */
        bflb_dma_feature_control(pec_dbi_b_dma_handle, DMA_CMD_SET_SRCADDR_INCREMENT, true);
    }

    pec_dbi_b_async_flag = false;
    pec_dbi_b_async_cycle_fill_flag = false;

    /* async callback */
    if (pec_dbi_b_async_callback_en_flag && pec_dbi_b_async_callback != NULL) {
        pec_dbi_b_async_callback();
    }
}

/**
 * @brief pec_dbi_b_async_callback_register
 *
 * @return int
 */
int pec_dbi_b_async_callback_register(void (*callback)(void))
{
    pec_dbi_b_async_callback = callback;
    return 0;
}

/**
 * @brief pec_dbi_b_async_callback_enable
 *
 * @return int
 */
int pec_dbi_b_async_callback_enable(bool enable)
{
    pec_dbi_b_async_callback_en_flag = enable;
    return 0;
}

/**
 * @brief pec_dbi_b_init
 *
 * @return int
 */
int pec_dbi_b_init(lcd_mipi_dbi_init_t *dbi_parra)
{
    /* pec gpio init */
    struct bflb_device_s *gpio;
    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_init(gpio, LCD_DBI_PEC_SIMU_PIN_DC, GPIO_FUNC_PEC | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_2);
    bflb_gpio_init(gpio, LCD_DBI_PEC_SIMU_PIN_WR, GPIO_FUNC_PEC | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_2);
    for (uint8_t i = 0; i < 8; i++) {
        bflb_gpio_init(gpio, LCD_DBI_PEC_SIMU_PIN_DATA + i, GPIO_FUNC_PEC | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_2);
    }

    /* pec init */
    pec_dbi_b_handle.clk_src = 40 * 1000 * 1000;
    pec_dbi_b_handle.clk = 10 * 1000 * 1000;

    pixel_format = dbi_parra->pixel_format;
    if (pixel_format == LCD_DBI_LCD_PIXEL_FORMAT_RGB565) {
        pec_dbi_b_handle.pixel_format = PEC_DBI_B_PIXEL_FORMAT_RGB565;
    } else if (pixel_format == LCD_DBI_LCD_PIXEL_FORMAT_NRGB8888) {
        pec_dbi_b_handle.pixel_format = PEC_DBI_B_PIXEL_FORMAT_NRGB8888;
    }
    pec_dbi_b_handle.pin_dc = LCD_DBI_PEC_SIMU_PIN_DC;
    pec_dbi_b_handle.pin_wr = LCD_DBI_PEC_SIMU_PIN_WR;
    pec_dbi_b_handle.pin_data = LCD_DBI_PEC_SIMU_PIN_DATA;
    pec_dbi_b_handle.pec = PEC0;
    pec_dbi_b_handle.sm = PEC_SM3;
    pec_dbi_b_handle.fifo_threshold = 3;

    bflb_pec_dbi_b_init(&pec_dbi_b_handle);

    /* dma init */
    dma_pec_config.direction = DMA_MEMORY_TO_PERIPH;
    dma_pec_config.src_req = DMA_REQUEST_NONE;
    dma_pec_config.dst_req = DMA_REQUEST_PEC0_SM0_TX + pec_dbi_b_handle.sm;
    dma_pec_config.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    dma_pec_config.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    dma_pec_config.src_burst_count = DMA_BURST_INCR4;
    dma_pec_config.dst_burst_count = DMA_BURST_INCR4;
    dma_pec_config.src_width = DMA_DATA_WIDTH_32BIT;
    dma_pec_config.dst_width = DMA_DATA_WIDTH_32BIT;

    pec_dbi_b_dma_handle = bflb_device_get_by_name(LCD_PEC_DBI_B_DMA_NAME);
    bflb_dma_channel_init(pec_dbi_b_dma_handle, &dma_pec_config);

    /* dma int cfg */
    bflb_dma_channel_irq_attach(pec_dbi_b_dma_handle, pec_dbi_b_dma_flush_callback, NULL);

    return 0;
}

/**
 * @brief pec_dbi_b_dma_is_busy, After the call ili9488_spi_draw_picture_dma must check this,
 *         if pec_dbi_b_dma_is_busy() == 1, Don't allow other draw !!
 *         can run in the DMA interrupt callback function.
 *
 * @return int 0:draw end; 1:Being draw
 */
int pec_dbi_b_dma_is_busy(void)
{
    if (pec_dbi_b_async_flag || pec_dbi_b_async_cycle_fill_flag) {
        return 1;
    } else {
        return 0;
    }
}

int pec_dbi_b_transmit_cmd_para(uint8_t cmd, uint32_t *para, size_t para_num)
{
    uint32_t pec_dbi_head;
    uint32_t pec_dbi_fifo_address;
    int fifo_cnt = 0;

    /* Build pec-dbi control head */
    pec_dbi_head = bflb_pec_dbi_b_build_head(&pec_dbi_b_handle, PEC_DBI_TRANSMIT_MODE_CMD, cmd, para_num);

    /* get pec tx fifo address */
    pec_dbi_fifo_address = bflb_pec_dbi_b_get_fifo_address(&pec_dbi_b_handle);

    /* uint8 to uint32 */
    para_num = (para_num + 3) / 4;

    /* get tx fifo level */
    do {
        fifo_cnt = bflb_pec_dbi_b_get_fifo_cnt(&pec_dbi_b_handle);
    } while (fifo_cnt == 0);

    /* Write control head with cmd */
    putreg32(pec_dbi_head, pec_dbi_fifo_address);
    fifo_cnt--;

    /* Write parameter */
    for (size_t i = 0; i < para_num; i++) {
        /* Get the available fifo depth */
        while (fifo_cnt == 0) {
            fifo_cnt = bflb_pec_dbi_b_get_fifo_cnt(&pec_dbi_b_handle);
        }
        putreg32(para[i], pec_dbi_fifo_address);
        fifo_cnt--;
    }

    return 0;
}

int pec_dbi_b_transmit_cmd_pixel_sync(uint8_t cmd, uint32_t *pixel, size_t pixel_num)
{
    uint32_t pec_dbi_head;
    uint32_t pec_dbi_fifo_address;
    int fifo_cnt = 0;

    /* Build pec-dbi control head */
    pec_dbi_head = bflb_pec_dbi_b_build_head(&pec_dbi_b_handle, PEC_DBI_TRANSMIT_MODE_PIXEL, cmd, pixel_num);

    /* get pec tx fifo address (rgb565) */
    pec_dbi_fifo_address = bflb_pec_dbi_b_get_fifo_address(&pec_dbi_b_handle);

    if (pixel_format == LCD_DBI_LCD_PIXEL_FORMAT_RGB565) {
        /* uint16 to uint32 */
        pixel_num = (pixel_num + 1) / 2;
    } else if (pixel_format == LCD_DBI_LCD_PIXEL_FORMAT_NRGB8888) {
        pixel_num = pixel_num;
    }

    /* get tx fifo level */
    do {
        fifo_cnt = bflb_pec_dbi_b_get_fifo_cnt(&pec_dbi_b_handle);
    } while (fifo_cnt == 0);

    /* Write control head */
    putreg32(pec_dbi_head, pec_dbi_fifo_address);
    fifo_cnt--;

    /* Write pixel */
    for (size_t i = 0; i < pixel_num; i++) {
        /* Get the available fifo depth */
        while (fifo_cnt == 0) {
            fifo_cnt = bflb_pec_dbi_b_get_fifo_cnt(&pec_dbi_b_handle);
        }

        putreg32(pixel[i], pec_dbi_fifo_address);
        fifo_cnt--;
    }

    return 0;
}

int pec_dbi_b_transmit_cmd_pixel_async(uint8_t cmd, uint32_t *pixel, size_t pixel_num)
{
    uint32_t pec_dbi_head;
    uint32_t pec_dbi_fifo_address;

    pec_dbi_b_async_flag = true;

    /* Build pec-dbi control head */
    pec_dbi_head = bflb_pec_dbi_b_build_head(&pec_dbi_b_handle, PEC_DBI_TRANSMIT_MODE_PIXEL, cmd, pixel_num);

    /* get pec tx fifo address */
    pec_dbi_fifo_address = bflb_pec_dbi_b_get_fifo_address(&pec_dbi_b_handle);

    if (pixel_format == LCD_DBI_LCD_PIXEL_FORMAT_RGB565) {
        /* uint16 to uint32 */
        pixel_num = (pixel_num + 1) / 2;
    } else if (pixel_format == LCD_DBI_LCD_PIXEL_FORMAT_NRGB8888) {
        pixel_num = pixel_num;
    }

    /* get tx fifo level */
    while (bflb_pec_dbi_b_get_fifo_cnt(&pec_dbi_b_handle) == 0) {
    };

    /* Write control head */
    putreg32(pec_dbi_head, pec_dbi_fifo_address);

    /* Use dma to do asynchronous writes */
    bflb_pec_dbi_b_dma_enable(&pec_dbi_b_handle, true);

    /* dma transfers cfg */
    dma_tx_transfers[0].src_addr = (uint32_t)pixel;
    dma_tx_transfers[0].dst_addr = pec_dbi_fifo_address;
    dma_tx_transfers[0].nbytes = pixel_num * 4;
    bflb_dma_channel_lli_reload(pec_dbi_b_dma_handle, dma_tx_llipool, LCD_DBI_PEC_DMA_LLI_NUM, dma_tx_transfers, 1);

    /* clean cache */
    bflb_l1c_dcache_clean_range((void *)pixel, pixel_num * 4);

    /* enable dma */
    bflb_dma_channel_start(pec_dbi_b_dma_handle);

    return 0;
}

int pec_dbi_b_transmit_cmd_pixel_fill_sync(uint8_t cmd, uint32_t pixel_val, size_t pixel_num)
{
    uint32_t pec_dbi_head;
    uint32_t pec_dbi_fifo_address;
    int fifo_cnt = 0;

    /* Build pec-dbi control head */
    pec_dbi_head = bflb_pec_dbi_b_build_head(&pec_dbi_b_handle, PEC_DBI_TRANSMIT_MODE_PIXEL, cmd, pixel_num);

    /* get pec tx fifo address */
    pec_dbi_fifo_address = bflb_pec_dbi_b_get_fifo_address(&pec_dbi_b_handle);

    if (pixel_format == LCD_DBI_LCD_PIXEL_FORMAT_RGB565) {
        /* uint16 to uint32 */
        pixel_num = (pixel_num + 1) / 2;
    } else if (pixel_format == LCD_DBI_LCD_PIXEL_FORMAT_NRGB8888) {
        pixel_num = pixel_num;
    }

    /* get tx fifo level */
    do {
        fifo_cnt = bflb_pec_dbi_b_get_fifo_cnt(&pec_dbi_b_handle);
    } while (fifo_cnt == 0);

    /* Write control head */
    putreg32(pec_dbi_head, pec_dbi_fifo_address);
    fifo_cnt--;

    /* Write pixel */
    for (size_t i = 0; i < pixel_num; i++) {
        /* Get the available fifo depth */
        while (fifo_cnt == 0) {
            fifo_cnt = bflb_pec_dbi_b_get_fifo_cnt(&pec_dbi_b_handle);
        }

        putreg32(pixel_val, pec_dbi_fifo_address);
        fifo_cnt--;
    }

    return 0;
}

int pec_dbi_b_transmit_cmd_pixel_fill_async(uint8_t cmd, uint32_t pixel_val, size_t pixel_num)
{
    uint32_t pec_dbi_head;
    uint32_t pec_dbi_fifo_address;

    pec_dbi_b_async_cycle_fill_flag = true;

    /* Build pec-dbi control head */
    pec_dbi_head = bflb_pec_dbi_b_build_head(&pec_dbi_b_handle, PEC_DBI_TRANSMIT_MODE_PIXEL, cmd, pixel_num);

    /* get pec tx fifo address */
    pec_dbi_fifo_address = bflb_pec_dbi_b_get_fifo_address(&pec_dbi_b_handle);

    if (pixel_format == LCD_DBI_LCD_PIXEL_FORMAT_RGB565) {
        /* uint16 to uint32 */
        pixel_num = (pixel_num + 1) / 2;
    } else if (pixel_format == LCD_DBI_LCD_PIXEL_FORMAT_NRGB8888) {
        pixel_num = pixel_num;
    }

    /* get tx fifo level */
    while (bflb_pec_dbi_b_get_fifo_cnt(&pec_dbi_b_handle) == 0) {
    };

    /* Write control head */
    putreg32(pec_dbi_head, pec_dbi_fifo_address);

    /* Use dma to do asynchronous writes */
    bflb_pec_dbi_b_dma_enable(&pec_dbi_b_handle, true);

    /* dma cycle fill mode */
    bflb_dma_feature_control(pec_dbi_b_dma_handle, DMA_CMD_SET_SRCADDR_INCREMENT, false);

    /* dma transfers cfg */
    dma_tx_transfers[0].src_addr = (uint32_t)&pixel_val;
    dma_tx_transfers[0].dst_addr = pec_dbi_fifo_address;
    dma_tx_transfers[0].nbytes = pixel_num * 4;
    bflb_dma_channel_lli_reload(pec_dbi_b_dma_handle, dma_tx_llipool, LCD_DBI_PEC_DMA_LLI_NUM, dma_tx_transfers, 1);

    /* clean cache */
    bflb_l1c_dcache_clean_range((void *)&pixel_val, sizeof(pixel_val));

    /* enable dma */
    bflb_dma_channel_start(pec_dbi_b_dma_handle);

    return 0;
}

#else
#error "Devices that do not support PEC-DBI! Replace the driver port (lcd_conf.h)"
#endif

#endif