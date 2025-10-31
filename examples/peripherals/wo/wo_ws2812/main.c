#include "bflb_mtimer.h"
#include "bflb_dma.h"
#include "board.h"
#include "bflb_wo.h"

#define DMA_ENABLE
#define WS2812_PIN     (10)
#define WS2812_NUM     (60)
#define WS2812_BUFFER  (WS2812_NUM * 24)

uint32_t buffer_rgb[WS2812_NUM];
uint16_t buffer_data[WS2812_BUFFER] __attribute__((aligned(32)));

struct bflb_device_s *wo;
#if defined(DMA_ENABLE)
struct bflb_device_s *dma0_ch0;
static ATTR_NOCACHE_RAM_SECTION struct bflb_dma_channel_lli_pool_s llipool[1];
static ATTR_NOCACHE_RAM_SECTION struct bflb_dma_channel_lli_transfer_s transfers[1];
#endif

struct bflb_wo_cfg_s cfg = {
    .code_total_cnt = 50,   /* 40MHz(XCLK) / 50 = 800KHz, period = 1.25us */
    .code0_first_cnt = 16,  /* high = 1.25us * 16 / 50 = 0.4us, low = 1.25us * (50 - 16) / 50 = 0.85us */
    .code1_first_cnt = 34,  /* high = 1.25us * 34 / 50 = 0.85us, low = 1.25us * (50 - 34) / 50 = 0.4us */
    .code0_first_level = 1,
    .code1_first_level = 1,
    .idle_level = 0,
    .fifo_threshold = 64,
    .mode = WO_MODE_WRITE,
};

#if defined(DMA_ENABLE)
struct bflb_dma_channel_config_s dma_cfg = {
    .direction = DMA_MEMORY_TO_PERIPH,
    .src_req = DMA_REQUEST_NONE,
    .dst_req = DMA_REQUEST_WO,
    .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
    .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
    .src_burst_count = DMA_BURST_INCR8,
    .dst_burst_count = DMA_BURST_INCR8,
    .src_width = DMA_DATA_WIDTH_16BIT,
    .dst_width = DMA_DATA_WIDTH_16BIT,
};
#endif

static uint32_t get_rgb(uint32_t x, uint8_t a)
{
    uint8_t r, g, b;
    uint16_t i = 0, j = 0;
    x %= 1530;
    i = x / 256;

    j = x % 256;

    switch (i) {
        case 0: {
            r = 255;
            g = 0;
            b = j;
        } break;

        case 1: {
            r = 255 - j;
            g = 0;
            b = 255;
        } break;
        case 2: {
            r = 0;
            g = j;
            b = 255;
        } break;
        case 3: {
            r = 0;
            g = 255;
            b = 255 - j;
        } break;
        case 4: {
            r = j;
            g = 255;
            b = 0;
        } break;
        case 5: {
            r = 255;
            g = 255 - j;
            b = 0;
        } break;

        default:
            return 0;
            break;
    }

    return ((r >> (a + 16)) | (g >> (a + 8)) | (b >> a));
}
static void data_update(void)
{
    static uint32_t rgb_base = 0;
    static uint32_t k = 51;
    uint32_t rgb_temp;

    rgb_temp = rgb_base;
    for (uint32_t i = 0; i < WS2812_NUM; i++) {
        buffer_rgb[i] = get_rgb((rgb_temp += k), 4);
    }
    rgb_base += 5;
    if (rgb_base >= 1530) {
        rgb_base = 0;
    }

    for (uint32_t i = 0; i < WS2812_NUM; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            buffer_data[i * 24 + j] = ((buffer_rgb[i] >> (15 - j)) & 1) << (WS2812_PIN % 16);
            buffer_data[i * 24 + j + 8] = ((buffer_rgb[i] >> (23 - j)) & 1) << (WS2812_PIN % 16);
            buffer_data[i * 24 + j + 16] = ((buffer_rgb[i] >> (7 - j)) & 1) << (WS2812_PIN % 16);
        }
    }
    bflb_l1c_dcache_clean_range(buffer_data, sizeof(buffer_data));
}

int main(void)
{
    board_init();
    wo = bflb_device_get_by_name("wo");
    bflb_wo_init(wo, &cfg);
    bflb_wo_pin_init(wo, WS2812_PIN, WO_MODE_WRITE);
#if defined(DMA_ENABLE)
    bflb_wo_enable_dma(wo);
    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");
    transfers[0].src_addr = (uint32_t)buffer_data;
    transfers[0].dst_addr = DMA_ADDR_WO_TDR;
    transfers[0].nbytes = WS2812_BUFFER * sizeof(uint16_t);
    printf("wo_ws2812 example with dma\r\n");
#else
    printf("wo_ws2812 example by polling\r\n");
#endif

    while (1) {
        data_update();
#if defined(DMA_ENABLE)
        bflb_dma_channel_stop(dma0_ch0);
        bflb_dma_channel_init(dma0_ch0, &dma_cfg);
        bflb_dma_channel_lli_reload(dma0_ch0, llipool, 1, transfers, 1);
        bflb_dma_channel_start(dma0_ch0);
#else
        bflb_wo_push_fifo(wo, buffer_data, WS2812_BUFFER);
#endif
        bflb_mtimer_delay_ms(5);
    }
}
