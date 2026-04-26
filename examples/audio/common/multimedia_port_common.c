/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdio.h>

#include <board.h>
#include <msp/kernel.h>
#include <xutils/debug.h>
#include <msp_port.h>
#include <bflb_gpio.h>
#if defined(BL616CL)
#include <bl616cl_gpio.h>
#elif defined(BL616)
#include <bl616_gpio.h>
#endif
uint32_t g_pa_delay_1 = 5;
uint32_t g_pa_delay_2 = 90;     /* this is time set according to the pa manual */

/***********user config************/

#if !defined(CONFIG_CODEC_USE_I2S)
/* codec pin config */
/* if use single mode. pin set as 255 */
#define INPUT_NEGATIVE_PIN      (28)
#define INPUT_POSITIVE_PIN      (27)

/* if use single mode. pin set as 255 */
#define OUTPUT_NEGATIVE_PIN     (14)
#define OUTPUT_POSITIVE_PIN     (15)
#endif

/* dma channel config */
#define DMA_TX_CHANNEL_ID       (0)
#define DMA_RX_CHANNEL_ID       (1)

/* Pa control pin */
#ifndef CONFIG_AUDIO_PA_PIN
#define CONFIG_AUDIO_PA_PIN     (30)    /* user config pa */
#endif

/*********** end user config************/

#if defined(CONFIG_CODEC_USE_I2S)
#include <bflb_gpio.h>
#include <bflb_core.h>
#include <bflb_i2c.h>
#include <bflb_i2s.h>
#include <audio_codec.h>
#include <audio_codec_es8388.h>

/* i2s gpio def */
#define I2S_FS_PIN    (GPIO_PIN_1)
#define I2S_DI_PIN    (GPIO_PIN_19)
#define I2S_DO_PIN    (GPIO_PIN_18)
#define I2S_BCLK_PIN  (GPIO_PIN_20)
/* mclk def */
#define I2S_MCLK_PIN  (GPIO_PIN_2)
/* i2c gpio def */
#define I2C_SCL_PIN  (GPIO_PIN_0)
#define I2C_SDA_PIN  (GPIO_PIN_3)

static struct bflb_device_s *i2s0;
static audio_codec_dev_t g_es8388_dev = { 0 };
static int g_codec_mute = 0;
static int g_codec_volume_percent = 30;

static void i2s_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    /* I2S_FS */
    bflb_gpio_init(gpio, I2S_FS_PIN, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* I2S_DI */
    bflb_gpio_init(gpio, I2S_DI_PIN, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* I2S_DO */
    bflb_gpio_init(gpio, I2S_DO_PIN, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* I2S_BCLK */
    bflb_gpio_init(gpio, I2S_BCLK_PIN, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

static struct bflb_device_s *es8388_i2c_gpio_init(void)
{
    struct bflb_device_s *gpio;
    struct bflb_device_s *i2c0;

    gpio = bflb_device_get_by_name("gpio");
    i2c0 = bflb_device_get_by_name("i2c0");

    /* I2C0_SCL */
    bflb_gpio_init(gpio, I2C_SCL_PIN, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    /* I2C0_SDA */
    bflb_gpio_init(gpio, I2C_SDA_PIN, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);

    bflb_i2c_init(i2c0, 200000);

    return i2c0;
}

static void mclk_out_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    /* MCLK CLKOUT */
    bflb_gpio_init(gpio, I2S_MCLK_PIN, GPIO_FUNC_CLKOUT | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);

    /*!< output MCLK,
        Will change the clock source of i2s,
        It needs to be called before i2s is initialized
        clock source 24.576M */
    switch (I2S_MCLK_PIN % 4) {
        case 0:
            GLB_Set_Chip_Clock_Out0_Sel(GLB_CHIP_CLK_OUT_0_I2S_REF_CLK);
            break;
        case 1:
            GLB_Set_Chip_Clock_Out1_Sel(GLB_CHIP_CLK_OUT_1_I2S_REF_CLK);
            break;
        case 2:
            GLB_Set_Chip_Clock_Out2_Sel(GLB_CHIP_CLK_OUT_2_I2S_REF_CLK);
            break;
        case 3:
            GLB_Set_Chip_Clock_Out3_Sel(GLB_CHIP_CLK_OUT_3_I2S_REF_CLK);
            break;
        default:
            break;
    }
}

static int msp_i2s_set_mclk(uint32_t sample_rate)
{
    uint32_t mclk_hz;
    uint32_t mclk_source_hz;
    uint32_t ref_clk_div;
    uint32_t ref_clk_factor;

    board_audio_pll_config_for_rate(sample_rate);

    mclk_hz = audio_codec_calc_mclk_hz(&g_es8388_dev, sample_rate);
    if ((sample_rate == 0U) || (mclk_hz == 0U)) {
        return -1;
    }

    mclk_source_hz = ((sample_rate == 11025U) ||
                      (sample_rate == 22050U) ||
                      (sample_rate == 44100U)) ? 22579200U : 24576000U;

    if ((mclk_source_hz % mclk_hz) != 0U) {
        printf("unsupported i2s mclk: rate=%u mclk=%u src=%u\r\n",
               sample_rate, mclk_hz, mclk_source_hz);
        return -1;
    }

    ref_clk_factor = mclk_source_hz / mclk_hz;
    if ((ref_clk_factor == 0U) || (ref_clk_factor > 64U)) {
        return -1;
    }

    ref_clk_div = ref_clk_factor - 1U;
    if (GLB_Set_I2S_CLK(ENABLE, (uint8_t)ref_clk_div,
                        GLB_I2S_DI_SEL_I2S_DI_INPUT,
                        GLB_I2S_DO_SEL_I2S_DO_OUTPT) != SUCCESS) {
        return -1;
    }

    printf("i2s mclk configured: rate=%u mclk=%u div=%u\r\n",
           sample_rate, mclk_hz, ref_clk_div);

    return 0;
}

static int msp_i2s_slot_width_to_lhal(uint32_t slot_width)
{
    switch (slot_width) {
        case 8:
            return I2S_SLOT_WIDTH_8;
        case 16:
            return I2S_SLOT_WIDTH_16;
        case 24:
            return I2S_SLOT_WIDTH_24;
        case 32:
            return I2S_SLOT_WIDTH_32;
        default:
            return -1;
    }
}

static int msp_codec_bits_from_width(uint32_t bit_width)
{
    switch (bit_width) {
        case 16:
            return AUDIO_CODEC_BITS_16;
        case 24:
            return AUDIO_CODEC_BITS_24;
        case 32:
            return AUDIO_CODEC_BITS_32;
        default:
            return -1;
    }
}

static int msp_i2s_validate_config(const msp_i2s_device_cfg_t *config)
{
    if (config == NULL || config->sample_rate == 0U) {
        return -1;
    }
    if ((config->channels != 1U) && (config->channels != 2U)) {
        return -1;
    }
    if ((config->bit_width != 16U) && (config->bit_width != 24U) && (config->bit_width != 32U)) {
        return -1;
    }
    if ((config->slot_width != 16U) && (config->slot_width != 32U)) {
        return -1;
    }
    if (config->bit_width > config->slot_width) {
        return -1;
    }
    if ((config->bit_width == 24U) && (config->slot_width != 32U)) {
        return -1;
    }

    return 0;
}

static int msp_codec_reconfigure(const msp_i2s_device_cfg_t *config)
{
    int ret;
    audio_codec_cfg_t codec_cfg = {
        .work_mode = AUDIO_CODEC_WORK_MODE_CODEC,
        .codec_master = false,
        .mic_input_mode = AUDIO_CODEC_MIC_INPUT_DIFF,
        .i2s_fmt = AUDIO_CODEC_I2S_FMT_LEFT_J,
        .bits = (audio_codec_bits_t)0,
        .sample_rate_hz = 0,
        .channels = 0,
        .enable_adc = true,
        .enable_dac = true,
    };

    if (config == NULL || g_es8388_dev.i2c == NULL || g_es8388_dev.drv == NULL) {
        return -1;
    }

    codec_cfg.bits = (audio_codec_bits_t)msp_codec_bits_from_width(config->bit_width);
    if (codec_cfg.bits == 0U) {
        return -1;
    }

    codec_cfg.sample_rate_hz = config->sample_rate;
    codec_cfg.channels = (uint8_t)config->channels;

    ret = audio_codec_init(&g_es8388_dev, &codec_cfg);
    if (ret != 0) {
        return ret;
    }

    audio_codec_set_mute(&g_es8388_dev, g_codec_mute != 0);
    audio_codec_set_volume(&g_es8388_dev, g_codec_volume_percent);

    return 0;
}

void msp_i2s_port_init(void)
{
    msp_i2s_device_cfg_t default_config = {
        .sample_rate = 16000,
        .channels = 2,
        .bit_width = 16,
        .slot_width = 16,
    };

    /* mclk clkout init */
    mclk_out_gpio_init();
    if (msp_i2s_set_mclk(default_config.sample_rate) != 0) {
        printf("mclk init failed: rate=%u\r\n", default_config.sample_rate);
        while (1) {}
    }

    /* Get I2C device */
    {
        int init_ret;
        struct bflb_device_s *i2c0 = es8388_i2c_gpio_init();

        /* init ES8388 Codec using new audio_codec API */
        printf("es8388 init (using new audio_codec API)\n\r");

        g_es8388_dev.i2c = i2c0;
        g_es8388_dev.i2c_addr = 0x10;
        g_es8388_dev.drv = &audio_codec_es8388_driver;
        g_es8388_dev.type = AUDIO_CODEC_TYPE_ES8388;

        init_ret = msp_i2s_device_config(&default_config);
        printf("es8388 ready: init=%d mute=%d volume=%d\n\r",
               init_ret, g_codec_mute, g_codec_volume_percent);
        if (init_ret != 0) {
            while (1) {}
        }
    }
}

int msp_i2s_device_config(const msp_i2s_device_cfg_t *config)
{
    int frame_width;
    int data_width;
    uint64_t bclk_freq_hz;
    uint32_t frame_slots;
    struct bflb_i2s_config_s i2s_cfg = {
        .bclk_freq_hz = 0,
        .role = I2S_ROLE_MASTER,
        .format_mode = I2S_MODE_LEFT_JUSTIFIED,
        .channel_mode = I2S_CHANNEL_MODE_NUM_2,
        .frame_width = I2S_SLOT_WIDTH_16,
        .data_width = I2S_SLOT_WIDTH_16,
        .fs_offset_cycle = 0,
        .tx_fifo_threshold = 7,
        .rx_fifo_threshold = 7,
    };

    if (msp_i2s_validate_config(config) != 0) {
        printf("invalid i2s config sample_rate=%u channels=%u bit_width=%u slot_width=%u\r\n",
               config ? config->sample_rate : 0U,
               config ? config->channels : 0U,
               config ? config->bit_width : 0U,
               config ? config->slot_width : 0U);
        return -1;
    }

    frame_width = msp_i2s_slot_width_to_lhal(config->slot_width);
    data_width = msp_i2s_slot_width_to_lhal(config->bit_width);
    if ((frame_width < 0) || (data_width < 0)) {
        return -1;
    }

    /* External codecs on this port still use a 2-slot I2S frame on the wire,
     * even when the logical audio stream is mono.
     */
    frame_slots = (config->channels == 1U) ? 2U : config->channels;
    bclk_freq_hz = (uint64_t)config->sample_rate * config->slot_width * frame_slots;
    if (bclk_freq_hz > 0xffffffffULL) {
        return -1;
    }

    i2s_cfg.bclk_freq_hz = (uint32_t)bclk_freq_hz;
    i2s_cfg.channel_mode = (config->channels == 1U) ? I2S_CHANNEL_MODE_NUM_1 : I2S_CHANNEL_MODE_NUM_2;
    i2s_cfg.frame_width = (uint8_t)frame_width;
    i2s_cfg.data_width = (uint8_t)data_width;

    if (msp_i2s_set_mclk(config->sample_rate) != 0) {
        return -1;
    }

    i2s_gpio_init();
    i2s0 = bflb_device_get_by_name("i2s0");
    bflb_i2s_init(i2s0, &i2s_cfg);
    /* enable dma */
#if defined(CONFIG_CODEC_USE_I2S)
    bflb_i2s_link_txdma(i2s0, true);
    bflb_i2s_link_rxdma(i2s0, true);
#endif
    bflb_i2s_feature_control(i2s0, I2S_CMD_CHANNEL_LR_MERGE, true);
    bflb_i2s_feature_control(i2s0, I2S_CMD_DATA_ENABLE, I2S_CMD_DATA_ENABLE_TX | I2S_CMD_DATA_ENABLE_RX);

    if (msp_codec_reconfigure(config) != 0) {
        return -1;
    }

    printf("i2s configured: rate=%u channels=%u bit_width=%u slot_width=%u mclk=%u bclk=%u frame_slots=%u\r\n",
           config->sample_rate, config->channels, config->bit_width,
           config->slot_width, audio_codec_calc_mclk_hz(&g_es8388_dev, config->sample_rate),
           i2s_cfg.bclk_freq_hz, frame_slots);

    return 0;
}

void msp_i2s_device_init(uint32_t sample_rate)
{
    msp_i2s_device_cfg_t config = {
        .sample_rate = sample_rate,
        .channels = 2,
        .bit_width = 16,
        .slot_width = 16,
    };

    (void)msp_i2s_device_config(&config);
}

void msp_i2s_tx_enable(void)
{
    //put to msp_i2s_device_init
#if 0
    if (i2s0) {
        //bflb_i2s_feature_control(i2s0, I2S_CMD_DATA_ENABLE, I2S_CMD_DATA_ENABLE_TX);
    }
#endif
}

void msp_i2s_rx_enable(void)
{
    //put to msp_i2s_device_init
#if 0
    if (i2s0) {
        //bflb_i2s_feature_control(i2s0, I2S_CMD_DATA_ENABLE, I2S_CMD_DATA_ENABLE_RX | I2S_CMD_DATA_ENABLE_TX);
    }
#endif
}
#endif

/* dma config */
msp_dma_ch_cfg_t msp_dma_channel_config(void)
{
    msp_dma_ch_cfg_t dma_channel_cfg = {
        .dma_tx_ctrl_id = 0,        // default 0
        .dma_rx_ctrl_id = 0,        // default 0
        .dma_tx_ch_id = DMA_TX_CHANNEL_ID,
        .dma_rx_ch_id = DMA_RX_CHANNEL_ID,
#if defined(CONFIG_CODEC_USE_I2S)
        .dma_tx_dstPeriph = 17,     //13:DMA_REQ_AUDIO_TX, 17:DMA_REQ_I2S_TX
        .dma_rx_srcPeriph = 16,     //21:DMA_REQ_SOLO_RX, 16:DMA_REQ_I2S_RX
#else
        .dma_tx_dstPeriph = 13,     //13:DMA_REQ_AUDIO_TX, 17:DMA_REQ_I2S_TX
        .dma_rx_srcPeriph = 21,     //21:DMA_REQ_SOLO_RX, 16:DMA_REQ_I2S_RX
#endif
    };

    return dma_channel_cfg;
}

#if !defined(CONFIG_CODEC_USE_I2S)
/* codec pin config */
msp_codec_pin_cfg_t msp_codec_pin_config(void)
{
    msp_codec_pin_cfg_t codec_pin_cfg = {
        .input_negative_pin = INPUT_NEGATIVE_PIN,
        .input_positive_pin = INPUT_POSITIVE_PIN,
        .output_negative_pin = OUTPUT_NEGATIVE_PIN,
        .output_positive_pin = OUTPUT_POSITIVE_PIN,
    };

    return codec_pin_cfg;
}
#endif

void msp_codec_pa_init_pre(void)
{
    msp_gpio_output_config(CONFIG_AUDIO_PA_PIN, 1);
    MSP_LOGD("msp_codec_pa_init_pre\r\n");
}

void msp_codec_pa_init(void)
{
    msp_gpio_output_set(CONFIG_AUDIO_PA_PIN, 0);
    msp_msleep(1);
    MSP_LOGD("msp_codec_pa_init\r\n");
}

void msp_codec_pa_after_opendac(void)
{
    static int init = 0;
    msp_msleep(g_pa_delay_1);
    if (0 == init) {
    }
    msp_gpio_output_set(CONFIG_AUDIO_PA_PIN, 1);
    if (0 == init) {
        init = 1;
    }
    msp_msleep(g_pa_delay_2);
    MSP_LOGD("msp_codec_pa_after_opendac\r\n");
}

void msp_codec_pa_before_closedac(void)
{
    msp_gpio_output_set(CONFIG_AUDIO_PA_PIN, 0);
    msp_msleep(20);
    MSP_LOGD("msp_codec_pa_before_closedac\r\n");
}
