#include "bflb_audac.h"
#include "bflb_gpio.h"
#include "bflb_dma.h"

#include "bl616_glb.h"
#include "board.h"

// clang-format off
static const uint16_t sin_0db_16point[16] = {
    0x0000, 0x30FB, 0x5A81, 0x7640, 0x7FFF, 0x7640, 0x5A81, 0x30FB, 0x0000, 0xCF05, 0xA57F, 0x89C0, 0x8001, 0x89C0, 0xA57F, 0xCF05,
};

static const uint16_t sin_0db_32point[32] = {
    0x0000, 0x18F8, 0x30FB, 0x471C, 0x5A81, 0x6A6C, 0x7640, 0x7D89, 0x7FFF, 0x7D89, 0x7640, 0x6A6C, 0x5A81, 0x471C, 0x30FB, 0x18F8,
    0x0000, 0xE708, 0xCF05, 0xB8E4, 0xA57F, 0x9594, 0x89C0, 0x8277, 0x8001, 0x8277, 0x89C0, 0x9594, 0xA57F, 0xB8E4, 0xCF05, 0xE708,
    };

static uint16_t sin_0db_l32_r16_2ch[64] = {
    0x0000,0x0000, 0x18F8,0x30FB, 0x30FB,0x5A81, 0x471C,0x7640, 0x5A81,0x7FFF, 0x6A6C,0x7640, 0x7640,0x5A81, 0x7D89,0x30FB,
    0x7FFF,0x0000, 0x7D89,0xCF05, 0x7640,0xA57F, 0x6A6C,0x89C0, 0x5A81,0x8001, 0x471C,0x89C0, 0x30FB,0xA57F, 0x18F8,0xCF05,
    0x0000,0x0000, 0xE708,0x30FB, 0xCF05,0x5A81, 0xB8E4,0x7640, 0xA57F,0x7FFF, 0x9594,0x7640, 0x89C0,0x5A81, 0x8277,0x30FB,
    0x8001,0x0000, 0x8277,0xCF05, 0x89C0,0xA57F, 0x9594,0x89C0, 0xA57F,0x8001, 0xB8E4,0x89C0, 0xCF05,0xA57F, 0xE708,0xCF05,
};
// clang-format on

struct bflb_device_s *audac_dma_hd;
struct bflb_device_s *audac_hd;
static struct bflb_dma_channel_lli_pool_s lli_pool[10];

void audio_dma_callback(void *arg)
{
    static uint16_t num = 0;
    num++;
    printf("scyle_n:%d\r\n", num);
}

/* audio gpio init*/
void audac_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    /* audac pwm output mode */
    bflb_gpio_init(gpio, GPIO_PIN_14, GPIO_FUNC_AUDAC_PWM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    bflb_gpio_init(gpio, GPIO_PIN_15, GPIO_FUNC_AUDAC_PWM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);

    // bflb_gpio_init(gpio, GPIO_PIN_22, GPIO_FUNC_AUDAC_PWM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    // bflb_gpio_init(gpio, GPIO_PIN_23, GPIO_FUNC_AUDAC_PWM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);

    // bflb_gpio_init(gpio, GPIO_PIN_27, GPIO_FUNC_AUDAC_PWM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    // bflb_gpio_init(gpio, GPIO_PIN_28, GPIO_FUNC_AUDAC_PWM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);

    /* PA enable */
    bflb_gpio_init(gpio, GPIO_PIN_10, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_11, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_set(gpio, GPIO_PIN_10);
    bflb_gpio_set(gpio, GPIO_PIN_11);
}

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
    audac_dma_cfg.src_burst_count = DMA_BURST_INCR8;
    audac_dma_cfg.dst_burst_count = DMA_BURST_INCR8;
    audac_dma_cfg.src_width = DMA_DATA_WIDTH_16BIT;
    audac_dma_cfg.dst_width = DMA_DATA_WIDTH_16BIT;

    audac_dma_hd = bflb_device_get_by_name("dma0_ch0");
    bflb_dma_channel_init(audac_dma_hd, &audac_dma_cfg);
    bflb_dma_channel_irq_attach(audac_dma_hd, audio_dma_callback, NULL);

    transfers[0].src_addr = (uint32_t)sin_0db_l32_r16_2ch;
    transfers[0].dst_addr = (uint32_t)DMA_ADDR_AUDAC_TDR;
    transfers[0].nbytes = sizeof(sin_0db_l32_r16_2ch);

    bflb_l1c_dcache_clean_range(sin_0db_l32_r16_2ch, sizeof(sin_0db_l32_r16_2ch));
    dma_lli_cnt = bflb_dma_channel_lli_reload(audac_dma_hd, lli_pool, 10, transfers, 1);
    bflb_dma_channel_lli_link_head(audac_dma_hd, lli_pool, dma_lli_cnt);
}

/* audio dac init */
static void audac_init(void)
{
    struct bflb_audac_init_config_s audac_init_cfg = {
        .sampling_rate = AUDAC_SAMPLING_RATE_32K,
        .output_mode = AUDAC_OUTPUT_MODE_PWM,
        .source_channels_num = AUDAC_SOURCE_CHANNEL_DUAL,
        .mixer_mode = AUDAC_MIXER_MODE_ONLY_L,
        .data_format = AUDAC_DATA_FORMAT_16BIT,
        .fifo_threshold = 7,
    };

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
    bflb_audac_feature_control(audac_hd, AUDAC_CMD_SET_VOLUME_VAL, (size_t)(-15 * 2));
    bflb_audac_volume_init(audac_hd, &audac_volume_cfg);
    /* audac enable dma */
    bflb_audac_link_rxdma(audac_hd, true);
}

int main(void)
{
    /* board init */
    board_init();

    printf("audac case start \r\n");

    /* gpio init */
    audac_gpio_init();

    /* audac init */
    audac_init();

    /* dma init */
    audac_dma_init();
    bflb_dma_channel_start(audac_dma_hd);

    /* audac start */
    bflb_audac_feature_control(audac_hd, AUDAC_CMD_PLAY_START, 0);
    bflb_mtimer_delay_ms(1000);

    /* enable mute */
    bflb_audac_feature_control(audac_hd, AUDAC_CMD_SET_MUTE, true);
    bflb_mtimer_delay_ms(1000);

    /* disable mute */
    bflb_audac_feature_control(audac_hd, AUDAC_CMD_SET_MUTE, false);
    bflb_mtimer_delay_ms(1000);

    printf("audac case end \r\n");

    while (1) {
        /* set volume */
        bflb_audac_feature_control(audac_hd, AUDAC_CMD_SET_VOLUME_VAL, (size_t)(-35 * 2));
        bflb_mtimer_delay_ms(1500);

        bflb_audac_feature_control(audac_hd, AUDAC_CMD_SET_VOLUME_VAL, (size_t)(-15 * 2));
        bflb_mtimer_delay_ms(1000);
    }
}
