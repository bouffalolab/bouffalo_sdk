#include "bflb_mtimer.h"
#include "bflb_dma.h"
#include "board.h"
#include "bflb_wo.h"

#define PIN_USE  (23)
#define LED_NUM  (6)
#define BUFF_LEN (LED_NUM * 24)
#define LUMI     (50)

struct bflb_device_s *wo;

uint32_t rgb[LED_NUM] = { LUMI, (LUMI << 8) | LUMI, LUMI << 8, (LUMI << 8) | (LUMI << 16), LUMI << 16, (LUMI << 16) | LUMI };
uint16_t buff[LED_NUM * 24] __attribute__((aligned(4)));

struct bflb_wo_cfg_s cfg = {
    .code_total_cnt = 50,
    .code0_first_cnt = 16,
    .code1_first_cnt = 34,
    .code0_first_level = 1,
    .code1_first_level = 1,
    .idle_level = 0,
    .fifo_threshold = 64,
    .mode = WO_MODE_WRITE,
};

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

struct bflb_device_s *dma0_ch0;
static ATTR_NOCACHE_RAM_SECTION struct bflb_dma_channel_lli_pool_s llipool[1];
static ATTR_NOCACHE_RAM_SECTION struct bflb_dma_channel_lli_transfer_s transfers[1];

void rgb_water(void)
{
    uint32_t temp = rgb[0];
    for (uint32_t i = 0; i < LED_NUM - 1; i++) {
        rgb[i] = rgb[i + 1];
    }
    rgb[LED_NUM - 1] = temp;
}

void rgb_to_buff(uint32_t *rgb, uint16_t *buff, uint32_t len, uint8_t pin)
{
    pin = pin % 16;
    uint16_t val1 = (1 << pin);

    for (uint32_t i = 0; i < len; i++) {
        uint32_t rgb_val = rgb[i];
        uint32_t buff_val;
        for (uint32_t j = 0; j < 24; j++) {
            buff_val = rgb_val >> (23 - j);
            buff_val &= 1;
            if (buff_val) {
                buff[i * 24 + j] = val1;
            } else {
                buff[i * 24 + j] = 0;
            }
        }
    }
    bflb_l1c_dcache_clean_invalidate_range(buff, sizeof(uint16_t) * BUFF_LEN);
}

int main(void)
{
    board_init();
    wo = bflb_device_get_by_name("wo");
    bflb_wo_init(wo, &cfg);
    bflb_wo_pin_init(wo, PIN_USE, WO_MODE_WRITE);
    bflb_wo_enable_dma(wo);
    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");
    bflb_dma_channel_tcint_mask(dma0_ch0, 1);
    transfers[0].src_addr = (uint32_t)buff;
    transfers[0].dst_addr = DMA_ADDR_WO_TDR;
    transfers[0].nbytes = BUFF_LEN * sizeof(uint16_t);

    while (1) {
        rgb_water();
        rgb_to_buff(rgb, buff, LED_NUM, PIN_USE);
        // bflb_wo_push_fifo(buff, BUFF_LEN);
        bflb_dma_channel_init(dma0_ch0, &dma_cfg);
        bflb_dma_channel_lli_reload(dma0_ch0, llipool, 1, transfers, 1);
        bflb_dma_channel_start(dma0_ch0);
        arch_delay_ms(500);
        bflb_dma_channel_stop(dma0_ch0);
    }
}
