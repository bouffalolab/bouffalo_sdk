#include "bflb_adc.h"
#include "bflb_mtimer.h"
#include "bflb_dma.h"
#include "board.h"
#include "bflb_uart.h"
#include "stdarg.h"

struct bflb_device_s *adc;
struct bflb_device_s *dma0_ch0;
struct bflb_device_s *uartx;

#define TEST_ADC_CHANNEL_0  1
#define TEST_ADC_CHANNEL_1  1
#define TEST_ADC_CHANNEL_2  0
#define TEST_ADC_CHANNEL_3  0
#define TEST_ADC_CHANNEL_4  0
#define TEST_ADC_CHANNEL_5  0
#define TEST_ADC_CHANNEL_6  0
#define TEST_ADC_CHANNEL_7  0
#define TEST_ADC_CHANNEL_8  0
#define TEST_ADC_CHANNEL_9  0
#define TEST_ADC_CHANNEL_10 0

#define TEST_ADC_CHANNELS (TEST_ADC_CHANNEL_0 + \
                           TEST_ADC_CHANNEL_1 + \
                           TEST_ADC_CHANNEL_2 + \
                           TEST_ADC_CHANNEL_3 + \
                           TEST_ADC_CHANNEL_4 + \
                           TEST_ADC_CHANNEL_5 + \
                           TEST_ADC_CHANNEL_6 + \
                           TEST_ADC_CHANNEL_7 + \
                           TEST_ADC_CHANNEL_8 + \
                           TEST_ADC_CHANNEL_9 + \
                           TEST_ADC_CHANNEL_10)

struct bflb_adc_channel_s chan[] = {
#if TEST_ADC_CHANNEL_0
    { .pos_chan = ADC_CHANNEL_0,
      .neg_chan = ADC_CHANNEL_GND },
#endif
#if TEST_ADC_CHANNEL_1
    { .pos_chan = ADC_CHANNEL_1,
      .neg_chan = ADC_CHANNEL_GND },
#endif
#if TEST_ADC_CHANNEL_2
    { .pos_chan = ADC_CHANNEL_2,
      .neg_chan = ADC_CHANNEL_GND },
#endif
#if TEST_ADC_CHANNEL_3
    { .pos_chan = ADC_CHANNEL_3,
      .neg_chan = ADC_CHANNEL_GND },
#endif
#if TEST_ADC_CHANNEL_4
    { .pos_chan = ADC_CHANNEL_4,
      .neg_chan = ADC_CHANNEL_GND },
#endif
#if TEST_ADC_CHANNEL_5
    { .pos_chan = ADC_CHANNEL_5,
      .neg_chan = ADC_CHANNEL_GND },
#endif
#if TEST_ADC_CHANNEL_6
    { .pos_chan = ADC_CHANNEL_6,
      .neg_chan = ADC_CHANNEL_GND },
#endif
#if TEST_ADC_CHANNEL_7
    { .pos_chan = ADC_CHANNEL_7,
      .neg_chan = ADC_CHANNEL_GND },
#endif
#if TEST_ADC_CHANNEL_8
    { .pos_chan = ADC_CHANNEL_8,
      .neg_chan = ADC_CHANNEL_GND },
#endif
#if TEST_ADC_CHANNEL_9
    { .pos_chan = ADC_CHANNEL_9,
      .neg_chan = ADC_CHANNEL_GND },
#endif
#if TEST_ADC_CHANNEL_10
    { .pos_chan = ADC_CHANNEL_10,
      .neg_chan = ADC_CHANNEL_GND },
#endif
};

static uint8_t dma_tc_flag0 = 0;

#define TEST_COUNT 500

ATTR_NOCACHE_NOINIT_RAM_SECTION uint32_t raw_data[TEST_ADC_CHANNELS * TEST_COUNT];

void dma0_ch0_isr(void *arg)
{
    dma_tc_flag0++;
    printf("tc done\r\n");
}

void bflb_uart_printf(struct bflb_device_s *dev,const char *format, ...)
{

	    va_list args;
//	    uint32_t u32Length;
	    static char i8LogBuf[256];

	    va_start(args, format);
	    /* the return value of vsnprintf is the number of bytes that would be
	     * written to buffer had if the size of the buffer been sufficiently
	     * large excluding the terminating null byte. If the output string
	     * would be larger than the i8LogBuf, we have to adjust the output
	     * length. */
	    vsnprintf(i8LogBuf, sizeof(i8LogBuf) - 1, format, args);

	    // 使用串口发送函数发送格式化后的字符串
	    for (int i = 0; i8LogBuf[i] != '\0'; i++) {
	        bflb_uart_putchar(dev,i8LogBuf[i]);
	    }

	    va_end(args);


}

int main(void)
{
    struct bflb_adc_result_s result[TEST_ADC_CHANNELS * TEST_COUNT];

    board_init();
    board_adc_gpio_init();
    board_uartx_gpio_init();
    uartx = bflb_device_get_by_name(DEFAULT_TEST_UART);

    struct bflb_uart_config_s uart_cfg;

    uart_cfg.baudrate = 115200;
    uart_cfg.data_bits = UART_DATA_BITS_8;
    uart_cfg.stop_bits = UART_STOP_BITS_1;
    uart_cfg.parity = UART_PARITY_NONE;
    uart_cfg.flow_ctrl = 0;
    uart_cfg.tx_fifo_threshold = 0;
    uart_cfg.rx_fifo_threshold = 0;
    bflb_uart_init(uartx, &uart_cfg);

    adc = bflb_device_get_by_name("adc");

    /* adc clock = XCLK / 2 / 32 */
    struct bflb_adc_config_s cfg;
    cfg.clk_div = ADC_CLK_DIV_32;
    cfg.scan_conv_mode = true;
    cfg.continuous_conv_mode = true;
    cfg.differential_mode = false;
    cfg.resolution = ADC_RESOLUTION_12B;
    cfg.vref = ADC_VREF_3P2V;

    bflb_adc_init(adc, &cfg);
    bflb_adc_channel_config(adc, chan, TEST_ADC_CHANNELS);
    bflb_adc_link_rxdma(adc, true);

    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");

    struct bflb_dma_channel_config_s config;

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
    transfers[0].nbytes = sizeof(raw_data);

    bflb_dma_channel_lli_reload(dma0_ch0, lli, 1, transfers, 1);
    bflb_dma_channel_start(dma0_ch0);

    bflb_adc_start_conversion(adc);

    while (dma_tc_flag0 != 1) {
        bflb_mtimer_delay_ms(1);
    }

    bflb_adc_stop_conversion(adc);

    bflb_adc_parse_result(adc, raw_data, result, TEST_ADC_CHANNELS * TEST_COUNT);

    for (size_t j = 0; j < TEST_ADC_CHANNELS * TEST_COUNT; j++) {
        printf("raw data:%08x\r\n", raw_data[j]);
        printf("pos chan %d,%d mv \r\n", result[j].pos_chan, result[j].millivolt);
        bflb_uart_printf(uartx,"pos chan %d,%d mv \r\n", result[j].pos_chan, result[j].millivolt);
    }

    while (1) {
    }
}
