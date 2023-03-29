#include "../lcd.h"

#if (defined(LCD_DBI_INTERFACE_TYPE) && (LCD_DBI_INTERFACE_TYPE == 1))

#if (__has_include("bflb_dbi.h"))

#if ((LCD_DBI_WORK_MODE == 4) && (DBI_QSPI_SUPPORT == 0))
#error : "The DBI of this chip does not support QSPI mode."
#endif

#include "bl_mipi_dbi.h"
#include "bflb_dbi.h"
#include "bflb_dma.h"
#include "bflb_gpio.h"
#include "bflb_l1c.h"

#define LCD_DBI_DMA_LLI_NUM (DBI_DBI_DATA_SIZE_MAX / 4 / 4064 + 1)

/* asynchronous flush interrupt callback */
typedef void (*dbi_callback)(void);
static volatile dbi_callback dbi_async_callback = NULL;
static volatile bool dbi_async_callback_en_flag = true;

/* dma device */
static struct bflb_device_s *dbi_dma_hd;
/* pec dbi typeB device */
static struct bflb_device_s *dbi_hd;

/* The memory space of DMA */
static struct bflb_dma_channel_lli_pool_s dma_tx_llipool[LCD_DBI_DMA_LLI_NUM];
static struct bflb_dma_channel_lli_transfer_s dma_tx_transfers[1];

static volatile bool dbi_async_flag = false;
static volatile bool dbi_async_cycle_fill_flag = false;

static void dbi_tc_callback(int irq, void *arg)
{
    /* disable dbi dma */
    bflb_dbi_link_txdma(dbi_hd, false);

    /* clear flag and mask int */
    bflb_dbi_int_clear(dbi_hd, DBI_INTCLR_TC);
    bflb_dbi_tcint_mask(dbi_hd, true);

    if (dbi_async_flag == false && dbi_async_cycle_fill_flag == false) {
        return;
    }

    if (dbi_async_cycle_fill_flag == true) {
        /* enable dma addr increment */
        bflb_dma_feature_control(dbi_dma_hd, DMA_CMD_SET_SRCADDR_INCREMENT, true);
    }

    /* disable DMA request for dbi */
    bflb_dbi_link_txdma(dbi_hd, false);

    dbi_async_flag = false;
    dbi_async_cycle_fill_flag = false;

    /* async callback */
    if (dbi_async_callback_en_flag && dbi_async_callback != NULL) {
        dbi_async_callback();
    }
}

int lcd_dbi_async_callback_register(void (*callback)(void))
{
    dbi_async_callback = callback;
    return 0;
}

int lcd_dbi_async_callback_enable(bool enable)
{
    dbi_async_callback_en_flag = enable;
    return 0;
}

int lcd_dbi_init(lcd_dbi_init_t *dbi_parra)
{
    struct bflb_device_s *gpio;

    /* dbi cfg */
    struct bflb_dbi_config_s dbi_cfg = {
        .clk_mode = DBI_CLOCK_MODE_1,
        .clk_freq_hz = dbi_parra->clock_freq,
        .tx_fifo_threshold = 3,
#if (LCD_DBI_WORK_MODE == 4)
        .cmd_wire_mode = dbi_parra->cmd_wire_dual_en,
        .addr_wire_mode = dbi_parra->addr_wire_dual_en,
        .data_wire_mode = dbi_parra->data_wire_dual_en,
#endif
    };

    /* dma cfg */
    struct bflb_dma_channel_config_s dma_dbi_config = {
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_DBI_TX,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR4,
        .dst_burst_count = DMA_BURST_INCR4,
        .src_width = DMA_DATA_WIDTH_32BIT,
        .dst_width = DMA_DATA_WIDTH_32BIT,
    };

#if (LCD_DBI_WORK_MODE == 1)
    /* dbi typeC 4-wire mode */
    dbi_cfg.dbi_mode = DBI_MODE_TYPE_C_4_WIRE;
    uint32_t dbi_gpio_func = GPIO_FUNC_DBI_C;
    uint8_t dbi_gpio_list[] = { LCD_DBI_TYPEC_PIN_CLK, LCD_DBI_TYPEC_PIN_CS, LCD_DBI_TYPEC_PIN_DAT, LCD_DBI_TYPEC_PIN_DC };

#elif (LCD_DBI_WORK_MODE == 2)
    /* dbi typeC 3-wire mode */
    dbi_cfg.dbi_mode = DBI_MODE_TYPE_C_3_WIRE;
    uint32_t dbi_gpio_func = GPIO_FUNC_DBI_C;
    uint8_t dbi_gpio_list[] = { LCD_DBI_TYPEC_PIN_CLK, LCD_DBI_TYPEC_PIN_CS, LCD_DBI_TYPEC_PIN_DAT };

#elif (LCD_DBI_WORK_MODE == 3)
    /* dbi typeB x8 mode */
    dbi_cfg.dbi_mode = DBI_MODE_TYPE_B;
    uint32_t dbi_gpio_func = GPIO_FUNC_DBI_B;
    uint8_t dbi_gpio_list[] = { LCD_DBI_TYPEB_PIN_WR, LCD_DBI_TYPEB_PIN_CS, LCD_DBI_TYPEB_PIN_RD, LCD_DBI_TYPEB_PIN_DC,
                                LCD_DBI_TYPEB_PIN_DAT0, LCD_DBI_TYPEB_PIN_DAT1, LCD_DBI_TYPEB_PIN_DAT2, LCD_DBI_TYPEB_PIN_DAT3,
                                LCD_DBI_TYPEB_PIN_DAT4, LCD_DBI_TYPEB_PIN_DAT5, LCD_DBI_TYPEB_PIN_DAT6, LCD_DBI_TYPEB_PIN_DAT7 };

#elif (LCD_DBI_WORK_MODE == 4)
    /* dbi Ex QSPI */
    dbi_cfg.dbi_mode = DBI_MODE_EX_QSPI;
    uint32_t dbi_gpio_func = GPIO_FUNC_DBI_QSPI;
    uint8_t dbi_gpio_list[] = { LCD_DBI_QSPI_PIN_CLK, LCD_DBI_QSPI_PIN_CS,
                                LCD_DBI_QSPI_PIN_DAT0, LCD_DBI_QSPI_PIN_DAT1, LCD_DBI_QSPI_PIN_DAT2, LCD_DBI_QSPI_PIN_DAT3 };
#endif

    if (dbi_parra->pixel_format == LCD_DBI_LCD_PIXEL_FORMAT_RGB565) {
        dbi_cfg.pixel_input_format = DBI_PIXEL_INPUT_FORMAT_RGB_565;
        dbi_cfg.pixel_output_format = DBI_PIXEL_OUTPUT_FORMAT_RGB_565;
    } else if (dbi_parra->pixel_format == LCD_DBI_LCD_PIXEL_FORMAT_NRGB8888) {
        dbi_cfg.pixel_input_format = DBI_PIXEL_INPUT_FORMAT_NRGB_8888;
        dbi_cfg.pixel_output_format = DBI_PIXEL_OUTPUT_FORMAT_RGB_888;
    }

    /* gpio init */
    gpio = bflb_device_get_by_name("gpio");
    for (uint8_t i = 0; i < sizeof(dbi_gpio_list) / sizeof(dbi_gpio_list[0]); i++) {
        bflb_gpio_init(gpio, dbi_gpio_list[i], dbi_gpio_func | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    }

    /* dbi init */
    dbi_hd = bflb_device_get_by_name("dbi");
    bflb_dbi_init(dbi_hd, &dbi_cfg);
    /* cs continuous mode */
    bflb_dbi_feature_control(dbi_hd, DBI_CMD_CS_CONTINUE, true);

    /* dbi tx intertupt init */
    bflb_dbi_tcint_mask(dbi_hd, true);
    bflb_irq_attach(dbi_hd->irq_num, dbi_tc_callback, NULL);
    bflb_irq_enable(dbi_hd->irq_num);

    /* dma init */
    dbi_dma_hd = bflb_device_get_by_name(LCD_DBI_DMA_NAME);
    bflb_dma_channel_init(dbi_dma_hd, &dma_dbi_config);

    return 0;
}

#if (LCD_DBI_WORK_MODE == 4)
int lcd_dbi_ex_qspi_addr_cfg(uint8_t addr_byte_size, uint32_t addr_val)
{
    bflb_dbi_qspi_set_addr(dbi_hd, addr_byte_size, addr_val);
    return 0;
}
#endif

int lcd_dbi_is_busy(void)
{
    if (dbi_async_flag || dbi_async_cycle_fill_flag) {
        return 1;
    } else {
        return 0;
    }
}

int lcd_dbi_transmit_cmd_para(uint8_t cmd, uint32_t *para, size_t para_num)
{
    bflb_dbi_send_cmd_data(dbi_hd, cmd, para_num, (void *)para);

    return 0;
}

int lcd_dbi_transmit_cmd_pixel_sync(uint8_t cmd, uint32_t *pixel, size_t pixel_num)
{
    bflb_dbi_send_cmd_pixel(dbi_hd, cmd, pixel_num, (void *)pixel);

    return 0;
}

int lcd_dbi_transmit_cmd_pixel_async(uint8_t cmd, uint32_t *pixel, size_t pixel_num)
{
    uint32_t data_size;

    dbi_async_flag = true;

    /* get data size */
    data_size = bflb_dbi_feature_control(dbi_hd, DBI_CMD_GET_SIZE_OF_PIXEL_CNT, pixel_num);

    /* dma transfers cfg */
    dma_tx_transfers[0].src_addr = (uint32_t)pixel;
    dma_tx_transfers[0].dst_addr = DMA_ADDR_DBI_TDR;
    dma_tx_transfers[0].nbytes = data_size;
    bflb_dma_channel_lli_reload(dbi_dma_hd, dma_tx_llipool, LCD_DBI_DMA_LLI_NUM, dma_tx_transfers, 1);

    /* clean cache */
    bflb_l1c_dcache_clean_range((void *)pixel, data_size);

    /* trigger dbi data transfer */
    bflb_dbi_send_cmd_pixel(dbi_hd, cmd, pixel_num, NULL);

    /* unmask int */
    bflb_dbi_tcint_mask(dbi_hd, false);

    /* enabled DMA request for dbi */
    bflb_dbi_link_txdma(dbi_hd, true);

    /* enable dma */
    bflb_dma_channel_start(dbi_dma_hd);

    return 0;
}

int lcd_dbi_transmit_cmd_pixel_fill_sync(uint8_t cmd, uint32_t pixel_val, size_t pixel_num)
{
    uint32_t words_cnt;
    uint8_t fifo_cnt;

    /* get data size */
    words_cnt = bflb_dbi_feature_control(dbi_hd, DBI_CMD_GET_SIZE_OF_PIXEL_CNT, pixel_num) / 4;

    /* trigger dbi data transfer */
    bflb_dbi_send_cmd_pixel(dbi_hd, cmd, pixel_num, NULL);

    /* fill data into fifo */
    for (; words_cnt > 0;) {
        fifo_cnt = bflb_dbi_feature_control(dbi_hd, DBI_CMD_GET_TX_FIFO_CNT, 0);
        fifo_cnt = words_cnt > fifo_cnt ? fifo_cnt : words_cnt;
        words_cnt -= fifo_cnt;

        for (; fifo_cnt > 0; fifo_cnt--) {
            putreg32(pixel_val, DMA_ADDR_DBI_TDR);
        }
    }

    /* wait transfer complete */
    while ((bflb_dbi_get_intstatus(dbi_hd) & DBI_INTSTS_TC) == 0) {
    };

    /* clear end flag */
    bflb_dbi_int_clear(dbi_hd, DBI_INTCLR_TC);

    return 0;
}

int lcd_dbi_transmit_cmd_pixel_fill_async(uint8_t cmd, uint32_t pixel_val, size_t pixel_num)
{
    uint32_t data_size;

    dbi_async_cycle_fill_flag = true;

    /* get data size */
    data_size = bflb_dbi_feature_control(dbi_hd, DBI_CMD_GET_SIZE_OF_PIXEL_CNT, pixel_num);

    /* dma cycle fill mode */
    bflb_dma_feature_control(dbi_dma_hd, DMA_CMD_SET_SRCADDR_INCREMENT, false);

    /* dma transfers cfg */
    dma_tx_transfers[0].src_addr = (uint32_t)&pixel_val;
    dma_tx_transfers[0].dst_addr = DMA_ADDR_DBI_TDR;
    dma_tx_transfers[0].nbytes = data_size;
    bflb_dma_channel_lli_reload(dbi_dma_hd, dma_tx_llipool, LCD_DBI_DMA_LLI_NUM, dma_tx_transfers, 1);

    /* clean cache */
    bflb_l1c_dcache_clean_range((void *)&pixel_val, sizeof(pixel_val));

    /* trigger dbi data transfer */
    bflb_dbi_send_cmd_pixel(dbi_hd, cmd, pixel_num, NULL);

    /* unmask int */
    bflb_dbi_tcint_mask(dbi_hd, false);

    /* enabled DMA request for dbi */
    bflb_dbi_link_txdma(dbi_hd, true);

    /* enable dma */
    bflb_dma_channel_start(dbi_dma_hd);

    return 0;
}

#else
#endif

#endif