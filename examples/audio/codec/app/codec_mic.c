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
#define READ_TIME               (2000)  //MS
#define INPUT_CACHE_TIME        (1200) //MS
#define PERIOD_TIME             (20)  // this config will decide XCODEC_EVENT_PERIOD_READ_COMPLETE evernt callback time MS
/* end user config param */

#define BYTE_TO_BITS            (8)
#define SECOND_TO_MILLISECONDS  (1000)

//#define DMA_NUM                 (0)
/* input fixed config for bl616 ，other configurations are not supported for now */
#define INPUT_SAMPLE_RATE       (16000)
#define INPUT_SAMPLE_BITS       (16)
#define INPUT_CHANNELS          (1)

#define FRAME_SIZE              (INPUT_SAMPLE_RATE * INPUT_SAMPLE_BITS / BYTE_TO_BITS * INPUT_CHANNELS / SECOND_TO_MILLISECONDS)  // 32bytes/ms
#define INPUT_BUFFER_SIZE       (FRAME_SIZE * INPUT_CACHE_TIME)   /*for ringbuffer*/
#define INPUT_PERIOD_SIZE       (FRAME_SIZE * PERIOD_TIME)
#define READ_BUFFER_SIZE        (FRAME_SIZE * READ_TIME)

static xcodec_t             g_codec;
static uint8_t              g_start_run = 0;
static msp_sem_t            g_input_sem;
static uint8_t              *g_input_buffer;
static uint8_t              *g_read_buffer;
static xcodec_dma_ch_t      *g_dma_hdl_in;
static xcodec_input_t       g_codec_input_ch;
static uint32_t             g_input_size = 0;
static m_ringbuf_t          g_input_ringbuf;

static void _codec_input_event_cb_fun(xcodec_input_t *i2s, xcodec_event_t event, void *arg)
{
    if (event == XCODEC_EVENT_PERIOD_READ_COMPLETE) {
        //printf("compile cb:%lu\r\n", (uint32_t)msp_now_ms()); 
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

static void _codec_input_task(void *arg)
{
    int ret = -1;
    
    g_start_run = 1;
    msp_sem_new(&g_input_sem, 0);

    printf("codec_mic_init\r\n");
    
    ret = xcodec_init(&g_codec, 0);
    if (ret != XCODEC_OK) {
        printf("xcodec_init error\n");
        msp_task_exit(0);
    }
    
    g_input_buffer = msp_zalloc_check(INPUT_BUFFER_SIZE);
    g_read_buffer = msp_zalloc_check(READ_BUFFER_SIZE);
    if ((NULL == g_input_buffer) || (NULL == g_read_buffer)) {
        printf("malloc error!");
        msp_task_exit(0);
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
    /*config negative and positive*/
    xcodec_input_config(&g_codec_input_ch, &input_config);
    xcodec_input_analog_gain(&g_codec_input_ch, 0x0);
    
    xcodec_input_link_dma(&g_codec_input_ch, g_dma_hdl_in);

    printf("start code input\r\n");
    xcodec_input_start(&g_codec_input_ch);
   
    uint32_t size   = 0;
    uint32_t r_size = 0;
    g_input_size = 0;
    //printf("addr204:%x, addr200:%x\r\n",*((volatile uint32_t *)0x2000c204), *((volatile uint32_t *)0x2000c200));   
    while (1) {
        //printf("addr204:%x, addr200:%x\r\n",*((volatile uint32_t *)0x2000c204), *((volatile uint32_t *)0x2000c200));   
        _input_wait();
        r_size = (g_input_size + INPUT_PERIOD_SIZE) < READ_BUFFER_SIZE ? INPUT_PERIOD_SIZE : (READ_BUFFER_SIZE-g_input_size);
        //printf("r_siez:%lu, g_input_size:%lu\r\n", r_size,g_input_size);
        size = xcodec_input_read(&g_codec_input_ch, g_read_buffer + g_input_size, r_size);
        //printf("size:%lu\r\n", size);
        if (size != INPUT_PERIOD_SIZE) {
            printf("input stop (%lu)(%lu)\r\n", size, r_size);
            break;
        }
        g_input_size += size;
    }
    
    xcodec_input_stop(&g_codec_input_ch);
    xcodec_input_link_dma(&g_codec_input_ch, NULL);
    xcodec_input_detach_callback(&g_codec_input_ch);
    xcodec_input_close(&g_codec_input_ch);
    xcodec_uninit(&g_codec);
    msp_sem_free(&g_input_sem);
#ifdef CODEC_DUMP_INPUT_DATA
    bflb_flash_erase(0x270000, 0x79000);
    bflb_flash_write(0x270000, g_read_buffer, READ_BUFFER_SIZE);
#endif
    printf("g_read_buffer:%p, size:%d\r\n", g_read_buffer, READ_BUFFER_SIZE);
    msp_free(g_dma_hdl_in);
    msp_free(g_input_buffer);
    msp_free(g_read_buffer); 
    printf("exit mic\r\n");
    g_start_run = 0;
    msp_task_exit(0);
}

#ifdef CONFIG_SHELL
#include <shell.h>

int cmd_codec_mic(int argc, char **argv)
{
    if (g_start_run) {
        printf("input task is running\r\n");
        return 0;
    }
    msp_task_new("input_task", _codec_input_task, NULL, 4096);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_codec_mic, codec_mic, wifi codec mic test);
#endif

