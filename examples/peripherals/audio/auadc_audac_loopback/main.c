#include "bflb_auadc.h"
#include "bflb_audac.h"
#include "bflb_gpio.h"
#include "bflb_dma.h"

#include "bl616_glb.h"
#include "board.h"

struct bflb_device_s *auadc_dma_hd;
struct bflb_device_s *audac_dma_hd;

struct bflb_device_s *auadc_hd;
struct bflb_device_s *audac_hd;

struct bflb_dma_channel_lli_pool_s auadc_dma_lli_pool[10];
struct bflb_dma_channel_lli_pool_s audac_dma_lli_pool[10];
uint16_t audio_loopback_buff[8 * 1000];

void auadc_dma_callback(void *arg)
{
    static uint16_t num = 0;
    num++;
    printf("scyle_n:%d\r\n", num);
}

/* audio gpio init*/
void audio_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    /* auadc ch0 */
    //bflb_gpio_init(gpio, GPIO_PIN_20, GPIO_ANALOG | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_2);
    /* auadc ch3 */
    //bflb_gpio_init(gpio, GPIO_PIN_22, GPIO_ANALOG | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_2);
    /* auadc ch4 */
    bflb_gpio_init(gpio, GPIO_PIN_27, GPIO_ANALOG | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_2);
    /* auadc ch7 */
    bflb_gpio_init(gpio, GPIO_PIN_30, GPIO_ANALOG | GPIO_FLOAT | GPIO_SMT_EN | GPIO_DRV_2);

    /* audac pwm output mode */
    // bflb_gpio_init(gpio, GPIO_PIN_14, GPIO_FUNC_AUDAC_PWM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    // bflb_gpio_init(gpio, GPIO_PIN_15, GPIO_FUNC_AUDAC_PWM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);

    // bflb_gpio_init(gpio, GPIO_PIN_22, GPIO_FUNC_AUDAC_PWM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    // bflb_gpio_init(gpio, GPIO_PIN_23, GPIO_FUNC_AUDAC_PWM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);

    // bflb_gpio_init(gpio, GPIO_PIN_27, GPIO_FUNC_AUDAC_PWM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    // bflb_gpio_init(gpio, GPIO_PIN_28, GPIO_FUNC_AUDAC_PWM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);

    /* audac gpdac output mode */
    bflb_gpio_init(gpio, GPIO_PIN_2, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_3, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
}

/* audio adc init */
static void auadc_init(void)
{
    /* audio adc config */
    struct bflb_auadc_init_config_s auadc_init_cfg = {
        .sampling_rate = AUADC_SAMPLING_RATE_32K,
        .input_mode = AUADC_INPUT_MODE_ADC,
        .data_format = AUADC_DATA_FORMAT_16BIT,
        .fifo_threshold = 3,
    };

    /* audio adc analog config */
    struct bflb_auadc_adc_init_config_s auadc_analog_init_cfg = {
        .auadc_analog_en = true,
        .adc_mode = AUADC_ADC_MODE_AUDIO,
        .adc_pga_mode = AUADC_ADC_PGA_MODE_AC_DIFFER,
        .adc_pga_posi_ch = AUADC_ADC_ANALOG_CH_4,
        .adc_pga_nega_ch = AUADC_ADC_ANALOG_CH_7,
        .adc_pga_gain = 21,
    };

    /* clock cfg */
    GLB_Config_AUDIO_PLL_To_491P52M();
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_AUDIO);

    /* auadc init */
    auadc_hd = bflb_device_get_by_name("auadc");
    bflb_auadc_init(auadc_hd, &auadc_init_cfg);
    bflb_auadc_adc_init(auadc_hd, &auadc_analog_init_cfg);
    /* auadc enable dma */
    bflb_auadc_link_rxdma(auadc_hd, true);
}

/* audio adc dma init */
void auadc_dma_init(void)
{
    uint32_t dma_lli_cnt;
    struct bflb_dma_channel_lli_transfer_s transfers[1];
    struct bflb_dma_channel_config_s auadc_dma_cfg;

    auadc_dma_cfg.direction = DMA_PERIPH_TO_MEMORY;
    auadc_dma_cfg.src_req = DMA_REQUEST_AUADC_RX;
    auadc_dma_cfg.dst_req = DMA_REQUEST_NONE;
    auadc_dma_cfg.src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    auadc_dma_cfg.dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    auadc_dma_cfg.src_burst_count = DMA_BURST_INCR4;
    auadc_dma_cfg.dst_burst_count = DMA_BURST_INCR4;
    auadc_dma_cfg.src_width = DMA_DATA_WIDTH_16BIT;
    auadc_dma_cfg.dst_width = DMA_DATA_WIDTH_16BIT;

    auadc_dma_hd = bflb_device_get_by_name("dma0_ch0");
    bflb_dma_channel_init(auadc_dma_hd, &auadc_dma_cfg);
    bflb_dma_channel_irq_attach(auadc_dma_hd, auadc_dma_callback, NULL);

    transfers[0].src_addr = (uint32_t)DMA_ADDR_AUADC_RDR;
    transfers[0].dst_addr = (uint32_t)audio_loopback_buff;
    transfers[0].nbytes = sizeof(audio_loopback_buff);

    dma_lli_cnt = bflb_dma_channel_lli_reload(auadc_dma_hd, auadc_dma_lli_pool, 10, transfers, 1);
    bflb_dma_channel_lli_link_head(auadc_dma_hd, auadc_dma_lli_pool, dma_lli_cnt);
}

/* audio dac init */
static void audac_init(void)
{
    /* audio dac config */
    struct bflb_audac_init_config_s audac_init_cfg = {
        .sampling_rate = AUDAC_SAMPLING_RATE_32K,
        .output_mode = AUDAC_OUTPUT_MODE_GPDAC_CH_A_B,      /* gpdac output */
        .source_channels_num = AUDAC_SOURCE_CHANNEL_SINGLE,
        .mixer_mode = AUDAC_MIXER_MODE_ONLY_L,
        .data_format = AUDAC_DATA_FORMAT_16BIT,
        .fifo_threshold = 3,
    };

    /* audio dac volume config */
    struct bflb_audac_volume_config_s audac_volume_cfg = {
        .mute_ramp_en = true,
        .mute_up_ramp_rate = AUDAC_RAMP_RATE_FS_32,
        .mute_down_ramp_rate = AUDAC_RAMP_RATE_FS_8,
        .volume_update_mode = AUDAC_VOLUME_UPDATE_MODE_RAMP,
        .volume_ramp_rate = AUDAC_RAMP_RATE_FS_128,
        .volume_zero_cross_timeout = AUDAC_RAMP_RATE_FS_128,
    };

    /* clock cfg */
    GLB_Config_AUDIO_PLL_To_491P52M();
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_AUDIO);

    /* audac init */
    audac_hd = bflb_device_get_by_name("audac");
    bflb_audac_init(audac_hd, &audac_init_cfg);
    bflb_audac_feature_control(audac_hd, AUDAC_CMD_SET_VOLUME_VAL, (size_t)(-5 * 2));
    bflb_audac_volume_init(audac_hd, &audac_volume_cfg);
    /* audac enable dma */
    bflb_audac_link_rxdma(audac_hd, true);
}

/* audio dac dma init */
void audac_dma_init(void)
{
    uint32_t dma_lli_cnt;
    struct bflb_dma_channel_lli_transfer_s transfers[1];
    struct bflb_dma_channel_config_s audac_dma_cfg;

    audac_dma_cfg.direction = DMA_MEMORY_TO_PERIPH;
    audac_dma_cfg.src_req = DMA_REQUEST_NONE;
    audac_dma_cfg.dst_req = DMA_REQUEST_AUDAC_TX;
    audac_dma_cfg.src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    audac_dma_cfg.dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    audac_dma_cfg.src_burst_count = DMA_BURST_INCR4;
    audac_dma_cfg.dst_burst_count = DMA_BURST_INCR4;
    audac_dma_cfg.src_width = DMA_DATA_WIDTH_16BIT;
    audac_dma_cfg.dst_width = DMA_DATA_WIDTH_16BIT;

    audac_dma_hd = bflb_device_get_by_name("dma0_ch1");
    bflb_dma_channel_init(audac_dma_hd, &audac_dma_cfg);

    transfers[0].src_addr = (uint32_t)audio_loopback_buff;
    transfers[0].dst_addr = (uint32_t)DMA_ADDR_AUDAC_TDR;
    transfers[0].nbytes = sizeof(audio_loopback_buff);

    dma_lli_cnt = bflb_dma_channel_lli_reload(audac_dma_hd, audac_dma_lli_pool, 10, transfers, 1);
    bflb_dma_channel_lli_link_head(audac_dma_hd, audac_dma_lli_pool, dma_lli_cnt);
}

int main(void)
{
    /* board init */
    board_init();

    printf("audac case start \r\n");

    /* gpio init */
    audio_gpio_init();

    /* auadc init */
    auadc_init();
    auadc_dma_init();
    bflb_dma_channel_start(auadc_dma_hd);
    bflb_auadc_feature_control(auadc_hd, AUADC_CMD_RECORD_START, 0);

    /* delay 10ms */
    bflb_mtimer_delay_ms(10);
    
    /* audac init */
    audac_init();
    audac_dma_init();
    bflb_dma_channel_start(audac_dma_hd);
    bflb_audac_feature_control(audac_hd, AUDAC_CMD_PLAY_START, 0);

    printf("audac case end \r\n");

    while (1) {
        bflb_mtimer_delay_ms(1);
    }
}
