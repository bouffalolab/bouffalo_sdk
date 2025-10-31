#include "bflb_adc_v2.h"
#include "bflb_mtimer.h"
#include "bflb_dma.h"
#include "board.h"

struct bflb_adc_channel_s chan[] = {
    {.pos_chan = ADC_EXTERNAL_CHANNEL_0, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO0 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_1, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO1 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_2, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO2 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_3, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO3 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_4, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO4 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_5, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO5 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_0, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO0 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_1, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO1 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_2, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO2 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_3, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO3 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_4, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO4 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_5, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO5 -> GND */
};

struct bflb_device_s *dma;
struct bflb_device_s *adc;
__attribute__((aligned(32))) uint32_t raw_data[12];
struct bflb_adc_result_s result;
volatile int dma_tc_flag = 0;
struct bflb_dma_channel_lli_pool_s lli[1]; /* max trasnfer size 4064 * 1 */

void dma_isr(void *arg)
{
    dma_tc_flag++;
    printf("dma isr\r\n");
}

int main(void)
{
    board_init();
    board_adc_gpio_init();

    adc = bflb_device_get_by_name(BFLB_NAME_ADC_V2_0);
    printf("adc = 0x%08lX\r\n", adc);

    struct bflb_adc_config_s cfg;
    cfg.scan_conv_mode = true;
    cfg.continuous_conv_mode = false;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_14B;
    cfg.vref = ADC_VREF_INTERNAL_1P25;

    bflb_adc_init(adc, &cfg);
    bflb_adc_channel_config_external(adc, chan, sizeof(chan) / sizeof(chan[0]));
    bflb_adc_feature_control(adc, ADC_CMD_SET_THRE_DMA_REGULAR, ADC_THRE_FOR_DMA_4);
    bflb_adc_link_rxdma(adc, true);

    dma = bflb_device_get_by_name("dma0_ch0");
    struct bflb_dma_channel_config_s config;
    config.direction = DMA_PERIPH_TO_MEMORY;
    config.src_req = DMA_REQUEST_ADC;
    config.dst_req = DMA_REQUEST_NONE;
    config.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    config.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    config.src_burst_count = DMA_BURST_INCR4;
    config.dst_burst_count = DMA_BURST_INCR4;
    config.src_width = DMA_DATA_WIDTH_32BIT;
    config.dst_width = DMA_DATA_WIDTH_32BIT;
    bflb_dma_channel_init(dma, &config);
    bflb_dma_channel_irq_attach(dma, dma_isr, NULL);
    struct bflb_dma_channel_lli_transfer_s transfers[1];
    transfers[0].src_addr = (uint32_t)DMA_ADDR_ADC_RDR;
    transfers[0].dst_addr = (uint32_t)raw_data;
    transfers[0].nbytes = sizeof(raw_data);
    bflb_dma_channel_lli_reload(dma, lli, 1, transfers, 1);
    bflb_dma_channel_start(dma);

    bflb_adc_start_conversion(adc);
    while (dma_tc_flag == 0);
    bflb_adc_stop_conversion(adc);
    bflb_l1c_dcache_invalidate_range(raw_data, sizeof(raw_data));
    for (int i = 0; i < sizeof(chan) / sizeof(chan[0]); i++) {
        bflb_adc_parse_result(adc, &raw_data[i], &result, 1);
        printf("raw_data = 0x%08lX, pos=%d, neg=%d, millivolt=%d\r\n", raw_data, result.pos_chan, result.neg_chan, result.millivolt);
    }
    printf("ADC scan mode by dma test complete\r\n");

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
