#include "bflb_adc.h"
#include "bflb_mtimer.h"
#include "bflb_dma.h"
#include "board.h"

struct bflb_device_s *adc;
struct bflb_device_s *dma0_ch0;

extern void board_init();

static uint8_t dma_tc_flag0 = 0;

#define ADC_CONVERT_COUNT 16

ATTR_NOCACHE_NOINIT_RAM_SECTION uint32_t raw_data[ADC_CONVERT_COUNT];

void dma0_ch0_isr(void *arg)
{
    dma_tc_flag0++;
    printf("tc done\r\n");
}

int main(void)
{
    board_init();
    board_adc_gpio_init();

    adc = bflb_device_get_by_name("adc");

    struct bflb_adc_config_s cfg;
    cfg.clk_div = ADC_CLK_DIV_32;
    cfg.scan_conv_mode = false;
    cfg.continuous_conv_mode = true;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_3P2V;

    struct bflb_adc_channel_s chan;

    chan.pos_chan = ADC_CHANNEL_0;
    chan.neg_chan = ADC_CHANNEL_GND;

    bflb_adc_init(adc, &cfg);
    bflb_adc_channel_config(adc, &chan, 1);
    bflb_adc_link_rxdma(adc, true);

    struct bflb_dma_channel_config_s config;

    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");

    config.direction = DMA_PERIPH_TO_MEMORY;
    config.src_req = DMA_REQUEST_ADC;
    config.dst_req = DMA_REQUEST_NONE;
    config.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    config.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    config.src_burst_count = DMA_BURST_INCR1;
    config.dst_burst_count = DMA_BURST_INCR1;
    config.src_width = DMA_DATA_WIDTH_32BIT;
    config.dst_width = DMA_DATA_WIDTH_32BIT;
    bflb_dma_channel_init(dma0_ch0, &config);

    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);

    struct bflb_dma_channel_lli_pool_s lli[1]; /* max trasnfer size 4064 * 1 */
    struct bflb_dma_channel_lli_transfer_s transfers[1];

    memset(raw_data, 0, sizeof(raw_data));

    transfers[0].src_addr = (uint32_t)DMA_ADDR_ADC_RDR;
    transfers[0].dst_addr = (uint32_t)raw_data;
    transfers[0].nbytes = 64;

    bflb_dma_channel_lli_reload(dma0_ch0, lli, 1, transfers, 1);
    bflb_dma_channel_start(dma0_ch0);

    bflb_adc_start_conversion(adc);

    while (dma_tc_flag0 != 1) {
        bflb_mtimer_delay_ms(1);
    }
    struct bflb_adc_result_s result[ADC_CONVERT_COUNT];

    bflb_adc_parse_result(adc, raw_data, result, ADC_CONVERT_COUNT);

    for (uint8_t i = 0; i < ADC_CONVERT_COUNT; i++) {
        printf("raw data:%08x\r\n", raw_data[i]);
        printf("pos chan %d,%d mv \r\n", result[i].pos_chan, result[i].millivolt);
    }

    bflb_adc_stop_conversion(adc);

    while (1) {
    }
}
