#include "bflb_adc_v2.h"
#include "bflb_mtimer.h"
#include "bflb_dma.h"
#include "board.h"

struct bflb_adc_channel_s chan_regular[] = {
    {.pos_chan = ADC_EXTERNAL_CHANNEL_1, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO1 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_2, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO2 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_3, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO3 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_4, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO4 -> GND */
};

struct bflb_adc_channel_s chan_inject[] = {
    {.pos_chan = ADC_EXTERNAL_CHANNEL_8, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO12 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_9, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO13 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_10, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO32 -> GND */
    {.pos_chan = ADC_EXTERNAL_CHANNEL_11, .neg_chan = ADC_EXTERNAL_CHANNEL_NULL,}, /* GPIO33 -> GND */
};

struct bflb_device_s *dma_regular;
struct bflb_device_s *dma_inject;
struct bflb_device_s *adc;
__attribute__((aligned(32))) uint32_t raw_data_regular[4];
__attribute__((aligned(32))) uint32_t raw_data_inject[4];
struct bflb_adc_result_s result;
volatile int dma_flag_regular = 0;
volatile int dma_flag_inject = 0;
struct bflb_dma_channel_lli_pool_s lli_regular[1]; /* max trasnfer size 4064 * 1 */
struct bflb_dma_channel_lli_pool_s lli_inject[1]; /* max trasnfer size 4064 * 1 */

void dma_isr_regular(void *arg)
{
    dma_flag_regular++;
    printf("dma isr regular\r\n");
}

void dma_isr_inject(void *arg)
{
    dma_flag_inject++;
    printf("dma isr inject\r\n");
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
    cfg.resolution = ADC_RESOLUTION_16B;
    cfg.vref = ADC_VREF_INTERNAL_1P25;

    bflb_adc_init(adc, &cfg);
    bflb_adc_channel_config_external(adc, chan_regular, sizeof(chan_regular) / sizeof(chan_regular[0]));
    bflb_adc_feature_control(adc, ADC_CMD_SET_THRE_DMA_REGULAR, ADC_THRE_FOR_DMA_4);
    bflb_adc_link_rxdma(adc, true);
    bflb_adc_channel_config_external_inject(adc, chan_inject, sizeof(chan_inject) / sizeof(chan_inject[0]));
    bflb_adc_feature_control(adc, ADC_CMD_SET_THRE_DMA_INJECT, ADC_THRE_FOR_DMA_4);
    bflb_adc_link_rxdma_inject(adc, true);

    dma_regular = bflb_device_get_by_name("dma0_ch0");
    struct bflb_dma_channel_config_s config_regular;
    config_regular.direction = DMA_PERIPH_TO_MEMORY;
    config_regular.src_req = DMA_REQUEST_ADC;
    config_regular.dst_req = DMA_REQUEST_NONE;
    config_regular.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    config_regular.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    config_regular.src_burst_count = DMA_BURST_INCR4;
    config_regular.dst_burst_count = DMA_BURST_INCR4;
    config_regular.src_width = DMA_DATA_WIDTH_32BIT;
    config_regular.dst_width = DMA_DATA_WIDTH_32BIT;
    bflb_dma_channel_init(dma_regular, &config_regular);
    bflb_dma_channel_irq_attach(dma_regular, dma_isr_regular, NULL);
    struct bflb_dma_channel_lli_transfer_s transfers_regular[1];
    transfers_regular[0].src_addr = (uint32_t)DMA_ADDR_ADC_RDR;
    transfers_regular[0].dst_addr = (uint32_t)raw_data_regular;
    transfers_regular[0].nbytes = sizeof(raw_data_regular);
    bflb_dma_channel_lli_reload(dma_regular, lli_regular, 1, transfers_regular, 1);
    bflb_dma_channel_start(dma_regular);

    dma_inject = bflb_device_get_by_name("dma0_ch1");
    struct bflb_dma_channel_config_s config_inject;
    config_inject.direction = DMA_PERIPH_TO_MEMORY;
    config_inject.src_req = DMA_REQUEST_ADC_INJECT;
    config_inject.dst_req = DMA_REQUEST_NONE;
    config_inject.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    config_inject.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    config_inject.src_burst_count = DMA_BURST_INCR4;
    config_inject.dst_burst_count = DMA_BURST_INCR4;
    config_inject.src_width = DMA_DATA_WIDTH_32BIT;
    config_inject.dst_width = DMA_DATA_WIDTH_32BIT;
    bflb_dma_channel_init(dma_inject, &config_inject);
    bflb_dma_channel_irq_attach(dma_inject, dma_isr_inject, NULL);
    struct bflb_dma_channel_lli_transfer_s transfers_inject[1];
    transfers_inject[0].src_addr = (uint32_t)DMA_ADDR_ADC_INJECT_RDR;
    transfers_inject[0].dst_addr = (uint32_t)raw_data_inject;
    transfers_inject[0].nbytes = sizeof(raw_data_inject);
    bflb_dma_channel_lli_reload(dma_inject, lli_inject, 1, transfers_inject, 1);
    bflb_dma_channel_start(dma_inject);

    bflb_adc_start_conversion(adc);
    bflb_mtimer_delay_ms(1);
    bflb_adc_start_conversion_inject(adc);

    while (dma_flag_regular == 0 || dma_flag_inject == 0);
    bflb_adc_stop_conversion(adc);
    bflb_l1c_dcache_invalidate_range(raw_data_regular, sizeof(raw_data_regular));
    bflb_l1c_dcache_invalidate_range(raw_data_inject, sizeof(raw_data_inject));
    for (int i = 0; i < sizeof(chan_regular) / sizeof(chan_regular[0]); i++) {
        bflb_adc_parse_result(adc, &raw_data_regular[i], &result, 1);
        printf("regular: raw_data = 0x%08lX, pos=%d, neg=%d, millivolt=%d\r\n", raw_data_regular[i], result.pos_chan, result.neg_chan, result.millivolt);
    }
    for (int i = 0; i < sizeof(chan_inject) / sizeof(chan_inject[0]); i++) {
        bflb_adc_parse_result(adc, &raw_data_inject[i], &result, 1);
        printf("inject: raw_data = 0x%08lX, pos=%d, neg=%d, millivolt=%d\r\n", raw_data_inject[i], result.pos_chan, result.neg_chan, result.millivolt);
    }

    printf("ADC regular and inject mode by dma test complete\r\n");

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
