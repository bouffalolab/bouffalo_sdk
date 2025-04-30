/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include "bflb_auadc.h"
#include "bflb_audac.h"
#include "bflb_gpio.h"
#include "bflb_dma.h"

#include "bl616_glb.h"
#include <codec_hfp.h>
#include <msp_port.h>
#include <msp/kernel.h>

typedef struct 
{
    uint8_t dma_rx_ctrl_id;
    uint8_t dma_rx_ch_id;
    uint8_t dma_tx_ctrl_id;
    uint8_t dma_tx_ch_id;
    struct bflb_device_s *auadc_dma_hd;
    struct bflb_device_s *audac_dma_hd;
    struct bflb_device_s *auadc_hd;
    struct bflb_device_s *audac_hd;
    struct bflb_dma_channel_lli_pool_s auadc_dma_lli_pool[10];
    struct bflb_dma_channel_lli_pool_s audac_dma_lli_pool[10];
    msp_task_t task_handle;
    msp_sem_t  start_sem_handle;
    msp_sem_t  stop_sem_handle;
} codec_hfp_private_t;

static codec_hfp_private_t *hfp_context = NULL;

extern void hfp_codec_input_stop(void);
extern void hfp_codec_output_stop(void);

void codec_hfp_start_entry(void *arg)
{
    while (1) {
        msp_sem_wait(&(hfp_context->start_sem_handle), MSP_WAIT_FOREVER);
        msp_codec_pa_after_opendac();
        printf("enable pa\r\n");
        
        msp_sem_wait(&(hfp_context->stop_sem_handle), MSP_WAIT_FOREVER);
        msp_codec_pa_before_closedac();
        hfp_codec_input_stop();
        hfp_codec_output_stop();
        printf("stop codec\r\n");
    }
}

int codec_hfp_init(void)
{
    if (hfp_context) {
        printf("hfp codec is already init\r\n");
        return 0;
    }
    
    hfp_context = msp_malloc(sizeof(codec_hfp_private_t));
    if (!hfp_context) {
        printf("hfp codec init failed!\r\n");
        return -1;
    }
    memset(hfp_context, 0, sizeof(codec_hfp_private_t));
    printf("hfp codec init success\r\n");

    msp_sem_new(&(hfp_context->start_sem_handle), 0);
    msp_sem_new(&(hfp_context->stop_sem_handle), 0);
    msp_task_new_ext(&(hfp_context->task_handle),"codec_hfp", codec_hfp_start_entry, NULL, 1024, 15);

    return 0;
}

int codec_hfp_deinit(void)
{
    if (hfp_context) {
        msp_task_exist("codec_hfp");
        msp_sem_free(&(hfp_context->start_sem_handle));
        msp_sem_free(&(hfp_context->stop_sem_handle));
        msp_free(hfp_context);
        hfp_context = NULL;
    }
    
    return 0;
}

#if 0 
void auadc_dma_callback(void *arg)
{
   static uint16_t num = 0;
    num++;
    printf("scyle_n:%d\r\n", num);
}

void audac_dma_callback(void *arg)
{
    static uint16_t num1 = 0;
    num1++;
    printf("dac_scyle_n:%d\r\n", num1);
}
#endif

static int audioadc_pin_to_channel(uint8_t pin)
{
    int channel = -1;

    switch (pin) {
        case 20:
            channel = 0;
            break;
        case 21:
            channel = 1;
            break;
        case 22:
            channel = 2;
            break;
        case 23:
            channel = 3;
            break;
        case 27:
            channel = 4;
            break;
        case 28:
            channel = 5;
            break;
        case 29:
            channel = 6;
            break;
        case 30:
            channel = 7;
            break;

        default :
            channel = -1;
            break;
    }

    return channel;
}

/* audio adc init */
void hfp_codec_input_init(hfp_codec_cfg_t config)
{
    /*  adc pin init */
    msp_codec_pin_cfg_t pin_cfg = msp_codec_pin_config();
extern int codec_auadc_pinmux_config(uint8_t pin);
    if (pin_cfg.input_positive_pin != 255) {
        codec_auadc_pinmux_config(pin_cfg.input_positive_pin); 
    }
    if (pin_cfg.input_negative_pin != 255) {
        codec_auadc_pinmux_config(pin_cfg.input_negative_pin);
    }
    
    /* audio adc config */
    struct bflb_auadc_init_config_s auadc_init_cfg = {
        .sampling_rate = AUADC_SAMPLING_RATE_8K,
        .input_mode = AUADC_INPUT_MODE_ADC,
        .data_format = AUADC_DATA_FORMAT_16BIT,
        .fifo_threshold = 3,
    };
    
    if (config.sample_rate == 8000) {
        auadc_init_cfg.sampling_rate = AUADC_SAMPLING_RATE_8K;
    } else if (config.sample_rate == 16000) {
        auadc_init_cfg.sampling_rate = AUADC_SAMPLING_RATE_16K;
    } else if (config.sample_rate == 24000) {
        auadc_init_cfg.sampling_rate = AUADC_SAMPLING_RATE_24K;
    } else if (config.sample_rate == 32000) {
        auadc_init_cfg.sampling_rate = AUADC_SAMPLING_RATE_32K;
    } else if (config.sample_rate == 48000) {
        auadc_init_cfg.sampling_rate = AUADC_SAMPLING_RATE_48K;
    } else {
    	auadc_init_cfg.sampling_rate = AUADC_SAMPLING_RATE_8K;
        printf("sample rate not support,set to 8000\r\n");
    }
    
    /* audio adc analog config */
    struct bflb_auadc_adc_init_config_s auadc_analog_init_cfg = {
        .auadc_analog_en = true,
        .adc_mode = AUADC_ADC_MODE_AUDIO,
        .adc_pga_mode = AUADC_ADC_PGA_MODE_AC_DIFFER,
        .adc_pga_posi_ch = AUADC_ADC_ANALOG_CH_4,
        .adc_pga_nega_ch = 5,
        .adc_pga_gain = 21,
    };

    if ((pin_cfg.input_positive_pin != 255) && (pin_cfg.input_negative_pin != 255)) {
        //printf("input differ\r\n");
        auadc_analog_init_cfg.adc_pga_mode = AUADC_ADC_PGA_MODE_AC_DIFFER; 
    } else {
        auadc_analog_init_cfg.adc_pga_mode = AUADC_ADC_PGA_MODE_AC_SINGLE; 
    } 
    
    int channel = -1;
    if (pin_cfg.input_positive_pin != 255) {
        channel = audioadc_pin_to_channel(pin_cfg.input_positive_pin);
        if (channel != -1) {
            //printf("channel:%d\r\n", channel);
            auadc_analog_init_cfg.adc_pga_posi_ch = channel;
        } else {
            printf("aui pin is error, pin:%d\r\n", pin_cfg.input_positive_pin);
        }
    }
    
    if (pin_cfg.input_negative_pin != 255) {
        channel = audioadc_pin_to_channel(pin_cfg.input_negative_pin);
        if (channel != -1) {
            //printf("channel:%d\r\n", channel);
            auadc_analog_init_cfg.adc_pga_nega_ch = channel;
        } else {
            printf("aui pin is error, pin:%d\r\n", pin_cfg.input_negative_pin);
        }
    }

    /* clock cfg */
    GLB_Config_AUDIO_PLL_To_491P52M();
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_AUDIO);

    /* auadc init */
    hfp_context->auadc_hd = bflb_device_get_by_name("auadc");
    bflb_auadc_init(hfp_context->auadc_hd, &auadc_init_cfg);
    bflb_auadc_adc_init(hfp_context->auadc_hd, &auadc_analog_init_cfg);
    /* auadc enable dma */
    bflb_auadc_link_rxdma(hfp_context->auadc_hd, true);
}

/* audio adc dma init */
void hfp_codec_input_config(uint32_t buffer0, uint32_t buffer1, uint32_t buf_size)
{
    uint32_t dma_lli_cnt;
    struct bflb_dma_channel_lli_transfer_s transfers[2];
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

    hfp_context->auadc_dma_hd = bflb_device_get_by_name("dma0_ch0");
    bflb_dma_channel_init(hfp_context->auadc_dma_hd, &auadc_dma_cfg);
    //bflb_dma_channel_irq_attach(auadc_dma_hd, auadc_dma_callback, NULL);

    transfers[0].src_addr = (uint32_t)DMA_ADDR_AUADC_RDR;
    transfers[0].dst_addr = (uint32_t)buffer0;
    transfers[0].nbytes = buf_size;
    transfers[1].src_addr = (uint32_t)DMA_ADDR_AUADC_RDR;
    transfers[1].dst_addr = (uint32_t)buffer1;
    transfers[1].nbytes = buf_size;

    dma_lli_cnt = bflb_dma_channel_lli_reload(hfp_context->auadc_dma_hd, hfp_context->auadc_dma_lli_pool, 10, transfers, 2);
    bflb_dma_channel_lli_link_head(hfp_context->auadc_dma_hd, hfp_context->auadc_dma_lli_pool, dma_lli_cnt);
}

void hfp_codec_input_start(void)
{
    bflb_dma_channel_start(hfp_context->auadc_dma_hd);
    bflb_audac_feature_control(hfp_context->auadc_hd, AUDAC_CMD_PLAY_START, 0);
}

void hfp_codec_input_stop(void)
{
    bflb_dma_channel_stop(hfp_context->auadc_dma_hd);
    bflb_audac_feature_control(hfp_context->auadc_hd, AUDAC_CMD_PLAY_STOP, 0);
}

/* audio dac init */
void hfp_codec_output_init(hfp_codec_cfg_t config)
{
    msp_codec_pin_cfg_t pin_cfg = msp_codec_pin_config();
extern int codec_aupwm_pinmux_config(uint8_t pin);
    if (pin_cfg.output_positive_pin != 255) {
        codec_aupwm_pinmux_config(pin_cfg.output_positive_pin); 
    }
    if (pin_cfg.output_negative_pin != 255) {
        codec_aupwm_pinmux_config(pin_cfg.output_negative_pin);
    }
    msp_codec_pa_init_pre();
    /* audio dac config */
    struct bflb_audac_init_config_s audac_init_cfg = {
        .sampling_rate = AUDAC_SAMPLING_RATE_8K,
        .output_mode = AUDAC_OUTPUT_MODE_PWM,      /* gpdac output */
        .source_channels_num = AUDAC_SOURCE_CHANNEL_SINGLE,
        .mixer_mode = AUDAC_MIXER_MODE_ONLY_L,
        .data_format = AUDAC_DATA_FORMAT_16BIT,
        .fifo_threshold = 3,
    };
    if (config.sample_rate == 8000) {
        audac_init_cfg.sampling_rate = AUDAC_SAMPLING_RATE_8K;
    } else if (config.sample_rate == 16000) {
        audac_init_cfg.sampling_rate = AUDAC_SAMPLING_RATE_16K;
    } else if (config.sample_rate == 24000) {
        audac_init_cfg.sampling_rate = AUDAC_SAMPLING_RATE_24K;
    } else if (config.sample_rate == 32000) {
        audac_init_cfg.sampling_rate = AUDAC_SAMPLING_RATE_32K;
    } else if (config.sample_rate == 48000) {
        audac_init_cfg.sampling_rate = AUDAC_SAMPLING_RATE_48K;
    } else {
        audac_init_cfg.sampling_rate = AUDAC_SAMPLING_RATE_8K;
        printf("sample rate not support, set as 8000\r\n");
    }
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
    hfp_context->audac_hd = bflb_device_get_by_name("audac");
    //hfp_context.audac_hd = audac_hd;
    bflb_audac_init(hfp_context->audac_hd, &audac_init_cfg);
    bflb_audac_feature_control(hfp_context->audac_hd, AUDAC_CMD_SET_VOLUME_VAL, (size_t)(-5 * 2));
    bflb_audac_volume_init(hfp_context->audac_hd, &audac_volume_cfg);
    /* audac enable dma */
    bflb_audac_link_rxdma(hfp_context->audac_hd, true);
}

/* audio dac dma init */
void hfp_codec_output_config(uint32_t buffer0, uint32_t buffer1, uint32_t buf_size)
{
    uint32_t dma_lli_cnt;
    struct bflb_dma_channel_lli_transfer_s transfers[2];
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

    hfp_context->audac_dma_hd = bflb_device_get_by_name("dma0_ch1");
    //hfp_context.audac_dma_hd = audac_dma_hd;
    bflb_dma_channel_init(hfp_context->audac_dma_hd, &audac_dma_cfg);
    //bflb_dma_channel_irq_attach(audac_dma_hd, audac_dma_callback, NULL);

    transfers[0].src_addr = (uint32_t)buffer0;
    transfers[0].dst_addr = (uint32_t)DMA_ADDR_AUDAC_TDR;
    transfers[0].nbytes = buf_size;

    transfers[1].src_addr = (uint32_t)buffer1;
    transfers[1].dst_addr = (uint32_t)DMA_ADDR_AUDAC_TDR;
    transfers[1].nbytes = buf_size;
    
    dma_lli_cnt = bflb_dma_channel_lli_reload(hfp_context->audac_dma_hd, hfp_context->audac_dma_lli_pool, 10, transfers, 2);
    bflb_dma_channel_lli_link_head(hfp_context->audac_dma_hd, hfp_context->audac_dma_lli_pool, dma_lli_cnt);
}

void hfp_codec_output_start(void)
{
    //printf("hfp_context->audac_dma_hd:%p\r\n", hfp_context->audac_dma_hd); 
    bflb_dma_channel_start(hfp_context->audac_dma_hd);
    bflb_audac_feature_control(hfp_context->audac_hd, AUDAC_CMD_PLAY_START, 0);
}

void hfp_codec_output_stop(void)
{
    bflb_dma_channel_stop(hfp_context->audac_dma_hd);
    bflb_audac_feature_control(hfp_context->audac_hd, AUDAC_CMD_PLAY_STOP, 0);
}

void codec_bt_hfp_cb(uint16_t interval_halfslot,
                        uint32_t tx_buffer_0,
                        uint32_t tx_buffer_1,
                        uint32_t rx_buffer_0,
                        uint32_t rx_buffer_1,
                        uint32_t tx_buffer_size,
                        uint32_t rx_buffer_size,
                        uint32_t start_time_halfslot,
                        uint8_t buffer_index)
{
    static uint8_t first_flag = 1;
    //printf("inter:%u, start:%lu, index:%u, tx_buffer_size:%lu\r\n", interval_halfslot,start_time_halfslot, buffer_index, tx_buffer_size);
    /* When starting for the first time, need to determine which buffer to use */
    /* Calculate codec mic acquisition time, sample rate 8K, 16bit */
    uint32_t mic_sample_time = tx_buffer_size / (8000 * 2 / 1000);
    //printf("time:%lu\r\n", mic_sample_time);
    extern uint32_t lld_read_clock(void);
    uint32_t now_time = lld_read_clock();
    //printf("now time:%lu, 0tx:%p, 1tx:%p, 0rx:%p, 1rx:%p\r\n", now_time, tx_buffer_0,tx_buffer_1,rx_buffer_0,rx_buffer_1);
    if (first_flag) {
        if (mic_sample_time + now_time > start_time_halfslot) {
            /* codec use buffer 1 */
            hfp_codec_input_config(tx_buffer_1, tx_buffer_0, tx_buffer_size);
            //hfp_codec_input_start();
            bflb_dma_channel_start(hfp_context->auadc_dma_hd);
            bflb_auadc_feature_control(hfp_context->auadc_hd, AUADC_CMD_RECORD_START, 0);
	    
            hfp_codec_output_config(rx_buffer_1, rx_buffer_0, rx_buffer_size);
            //hfp_codec_output_start();
            bflb_dma_channel_start(hfp_context->audac_dma_hd);
            bflb_audac_feature_control(hfp_context->audac_hd, AUDAC_CMD_PLAY_START, 0);
            /*open pa*/
            //printf("start sem\r\n");
            msp_sem_signal(&(hfp_context->start_sem_handle));
        } else {
            /* use buffer 0 */
            hfp_codec_input_config(tx_buffer_0, tx_buffer_1, tx_buffer_size);
            //hfp_codec_input_start();
            bflb_dma_channel_start(hfp_context->auadc_dma_hd);
            bflb_auadc_feature_control(hfp_context->auadc_hd, AUADC_CMD_RECORD_START, 0);
	    
            hfp_codec_output_config(rx_buffer_0, rx_buffer_1, rx_buffer_size);
            //hfp_codec_output_start();
            bflb_dma_channel_start(hfp_context->audac_dma_hd);
            bflb_audac_feature_control(hfp_context->audac_hd, AUDAC_CMD_PLAY_START, 0);
            /*open pa*/
            //printf("start sem\r\n");
            msp_sem_signal(&(hfp_context->start_sem_handle));
        }
        first_flag = 0;
    }
    if (buffer_index == 0xFF) {
        //hfp_codec_output_stop();
        //msp_codec_pa_before_closedac();
        //hfp_codec_input_stop();
        //hfp_codec_output_stop();
        first_flag = 1;
        msp_sem_signal(&(hfp_context->stop_sem_handle));
    }
}

