/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdlib.h>
#include <stdio.h>
#include <xcodec.h>
#include <msp/kernel.h>

//#define CODEC_DUMP_INPUT_DATA   (1)
#ifdef CODEC_DUMP_INPUT_DATA
#include <bflb_flash.h>
#endif

/* user config param */
#define READ_TIME               	(2000)  //MS
#define INPUT_CACHE_TIME        	(1200) //MS
#define INPUT_PERIOD_TIME             (20)  // this config will decide XCODEC_EVENT_PERIOD_READ_COMPLETE evernt callback time MS
#define OUTPUT_CACHE_TIME       	(1200) //MS
#define OUTPUT_PERIOD_TIME            (20)  // this config will decide XCODEC_EVENT_PERIOD_READ_COMPLETE evernt callback time MS

/* end user config param */

#define BYTE_TO_BITS            (8)
#define SECOND_TO_MILLISECONDS  (1000)

//#define DMA_NUM                 (0)
/* input fixed config for bl616 ，other configurations are not supported for now */
#define INPUT_SAMPLE_RATE       (16000)
#define INPUT_SAMPLE_BITS       (16)
#define INPUT_CHANNELS          (1)
#define OUTPUT_SAMPLE_RATE       (16000)
#define OUTPUT_SAMPLE_BITS       (16)
#define OUTPUT_CHANNELS          (1)

#define INPUT_FRAME_SIZE        (INPUT_SAMPLE_RATE * INPUT_SAMPLE_BITS / BYTE_TO_BITS * INPUT_CHANNELS / SECOND_TO_MILLISECONDS)  // 32bytes/ms
#define INPUT_BUFFER_SIZE       (INPUT_FRAME_SIZE * INPUT_CACHE_TIME)   /*for ringbuffer*/
#define INPUT_PERIOD_SIZE       (INPUT_FRAME_SIZE * INPUT_PERIOD_TIME)
#define READ_BUFFER_SIZE        (INPUT_FRAME_SIZE * READ_TIME)

#define OUTPUT_FRAME_SIZE       (OUTPUT_SAMPLE_RATE * OUTPUT_SAMPLE_BITS / BYTE_TO_BITS * OUTPUT_CHANNELS / SECOND_TO_MILLISECONDS)  // 32bytes/ms
#define OUTPUT_BUFFER_SIZE      (OUTPUT_FRAME_SIZE * OUTPUT_CACHE_TIME)   /*for ringbuffer*/
#define OUTPUT_PERIOD_SIZE      (OUTPUT_FRAME_SIZE * OUTPUT_PERIOD_TIME)


static xcodec_t             g_codec;
static uint8_t              g_start_run = 0;

static msp_sem_t            g_input_sem;
static uint8_t              *g_input_buffer;
static xcodec_dma_ch_t      *g_dma_hdl_in;
static xcodec_input_t       g_codec_input_ch;
static uint32_t             g_input_size = 0;
static m_ringbuf_t          g_input_ringbuf;

static uint8_t              *g_output_buffer;
static xcodec_dma_ch_t      *g_dma_hdl_out;
static xcodec_input_t       g_codec_output_ch;
static uint32_t             g_output_size = 0;
static m_ringbuf_t          g_output_ringbuf;

static void codec_output_event_cb_fun(xcodec_output_t *output, xcodec_event_t event, void *arg)
{
    if (event == XCODEC_EVENT_PERIOD_WRITE_COMPLETE) {
        //printf("out_flag:%d\r\n", out_flag);
    } else if (event == XCODEC_EVENT_WRITE_BUFFER_EMPTY) {
        //printf("output CODEC_EVENT_WRITE_BUFFER_EMPTY\r\n");
    } else {

    }
}
static void _codec_input_event_cb_fun(xcodec_input_t *i2s, xcodec_event_t event, void *arg)
{
    if (event == XCODEC_EVENT_PERIOD_READ_COMPLETE) {
        printf("compile cb:%lu\r\n", (uint32_t)msp_now_ms()); 
        msp_sem_signal(&g_input_sem);
    } 
}

static void _input_wait(void)
{    
    int ret = xcodec_input_buffer_avail(&g_codec_input_ch);
    //printf("ret:%d\r\n", ret);
    if (ret < INPUT_PERIOD_SIZE) {
        msp_sem_wait(&g_input_sem, MSP_WAIT_FOREVER);
    }
}

static int _codec_loop_init(void)
{
    xcodec_error_t ret = XCODEC_OK;
    
    g_start_run = 1;
    msp_sem_new(&g_input_sem, 0);

    printf("codec_init\r\n");
    
    ret = xcodec_init(&g_codec, 0);
    if (ret != XCODEC_OK) {
        printf("xcodec_init error\n");
        return -1;
    }

    /* input */
    g_input_buffer = msp_zalloc_check(INPUT_BUFFER_SIZE);
    if (NULL == g_input_buffer) {
        printf("malloc error!");
        return -1;
    }
    
    /* micbias set high if need */
    msp_gpio_output_config(24, 1);
    msp_gpio_output_set(1);
    
    g_dma_hdl_in = msp_zalloc_check(sizeof(xcodec_dma_ch_t)); 
    //g_dma_hdl_in->ctrl_id = DMA_NUM;
    //g_dma_hdl_in->ch_id = DMA_CHANNEL;
    
    g_codec_input_ch.dma = g_dma_hdl_in;
    g_codec_input_ch.ring_buf = &g_input_ringbuf;
    xcodec_input_open(&g_codec, &g_codec_input_ch, 0);
    /* input ch config */
    xcodec_input_attach_callback(&g_codec_input_ch, _codec_input_event_cb_fun, NULL);

    xcodec_input_config_t input_config;
    
    input_config.sample_rate  = INPUT_SAMPLE_RATE;
    input_config.bit_width    = INPUT_SAMPLE_BITS;
    input_config.mode         = XCODEC_INPUT_DIFFERENCE;
    input_config.buffer       = g_input_buffer;
    input_config.buffer_size  = INPUT_BUFFER_SIZE;
    input_config.period = INPUT_PERIOD_SIZE;
    input_config.sound_channel_num = INPUT_CHANNELS;
    xcodec_input_config(&g_codec_input_ch, &input_config);
    xcodec_input_analog_gain(&g_codec_input_ch, 0x0);
    
    xcodec_input_link_dma(&g_codec_input_ch, g_dma_hdl_in);

    printf("start code input\r\n");
    xcodec_input_start(&g_codec_input_ch);

    /* output */
    printf("output init\r\n");
    g_output_buffer = msp_zalloc_check(OUTPUT_BUFFER_SIZE); 

    if (g_output_buffer == NULL){
        printf("malloc failed!\r\n");
        g_start_run = 0;
        return -1;
    }

    xcodec_dma_ch_t *g_dma_hdl_out = msp_zalloc_check(sizeof(xcodec_dma_ch_t)); 

    g_codec_output_ch.dma = g_dma_hdl_out;
    g_codec_output_ch.sound_channel_num = OUTPUT_CHANNELS;
    g_codec_output_ch.ring_buf = &g_output_ringbuf;
    xcodec_output_open(&g_codec, &g_codec_output_ch, 0);
    /* output ch config */
    xcodec_output_attach_callback(&g_codec_output_ch, codec_output_event_cb_fun, NULL);
    
    xcodec_output_config_t output_config;

    output_config.bit_width = OUTPUT_SAMPLE_BITS;
    output_config.sample_rate = OUTPUT_SAMPLE_RATE;
    output_config.buffer = g_output_buffer;
    output_config.buffer_size = OUTPUT_BUFFER_SIZE;
    output_config.period = OUTPUT_PERIOD_SIZE;
    output_config.mode = XCODEC_OUTPUT_DIFFERENCE;
    output_config.sound_channel_num = OUTPUT_CHANNELS;
    //output_config.negative_pin = OUTPUT_NEGATIVE_PIN;
    //output_config.positive_pin = OUTPUT_POSITIVE_PIN;
    //output_config.pa_pin = 4;
    xcodec_output_config(&g_codec_output_ch, &output_config);
    xcodec_output_analog_gain(&g_codec_output_ch, 50);
    xcodec_output_buffer_reset(&g_codec_output_ch);

    msp_msleep(INPUT_CACHE_TIME - 200);    
    xcodec_output_link_dma(&g_codec_output_ch, g_dma_hdl_out);
    
    xcodec_output_start(&g_codec_output_ch);

}
static void _codec_loop_task(void *arg)
{
    uint8_t *buffer = msp_zalloc_check(INPUT_PERIOD_SIZE); 
    int size = 0; 
    _codec_loop_init();

    while (1) {
        //printf("addr204:%x, addr200:%x\r\n",*((volatile uint32_t *)0x2000c204), *((volatile uint32_t *)0x2000c200));   
        _input_wait();
        //printf("r_siez:%lu, g_input_size:%lu\r\n", r_size,g_input_size);
        size = xcodec_input_buffer_avail(&g_codec_input_ch);
        //printf("input buffer left:%d\r\n", size);
        size = xcodec_input_read(&g_codec_input_ch, buffer, INPUT_PERIOD_SIZE);
        size = xcodec_output_buffer_avail(&g_codec_output_ch);
        if (size <= INPUT_PERIOD_SIZE * 20) {
            msp_msleep(40);
        }
        size = xcodec_output_write(&g_codec_output_ch, buffer, OUTPUT_PERIOD_SIZE);
        size = xcodec_output_buffer_remain(&g_codec_output_ch); 
        //printf("ouput buffer left:%lu\r\n", size);
    }
    
    xcodec_input_stop(&g_codec_input_ch);
    xcodec_input_link_dma(&g_codec_input_ch, NULL);
    xcodec_input_detach_callback(&g_codec_input_ch);
    xcodec_input_close(&g_codec_input_ch);
    
    xcodec_output_stop(&g_codec_output_ch);
    xcodec_output_link_dma(&g_codec_output_ch, NULL);
    xcodec_output_detach_callback(&g_codec_output_ch);
    xcodec_output_close(&g_codec_output_ch);
    
    xcodec_uninit(&g_codec);
    msp_sem_free(&g_input_sem);
    msp_free(buffer);
    msp_free(g_dma_hdl_in);
    msp_free(g_input_buffer);
    msp_free(g_dma_hdl_out);
    msp_free(g_output_buffer);
    printf("exit loop\r\n");
    g_start_run = 0;
    msp_task_exit(0);
}

#ifdef CONFIG_SHELL
#include <shell.h>

int cmd_codec_loop(int argc, char **argv)
{
    if (g_start_run) {
        printf("loop task is running\r\n");
        return 0;
    }
    msp_task_new("input_task", _codec_loop_task, NULL, 4096);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_codec_loop, codec_loop, wifi codec loop test);
#endif

