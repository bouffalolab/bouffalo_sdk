/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdlib.h>
#include <msp/kernel.h>
#include <msp_port.h>
#include <alsa/snd.h>
#include <alsa/pcm.h>
#include <alsa/mixer.h>
#include <xutils/driver.h>
#include <xutils/debug.h>
#include <xcodec.h>
#include <xutils/mringbuffer.h>

#include <devices/drv_snd_bl616.h>

static snd_bl616_config_t *g_audio_gain_config = NULL;

#define TAG "snd"

#if 1
#define user_log(M, ...)
#else
uint64_t msp_gettick_count(void);

#define user_log(M, ...) do {  printf("[%9lld]------[%s:%d] " M "",  \
                                 msp_gettick_count(),          \
                                 __func__,                       \
                                 __LINE__,                       \
                                 ##__VA_ARGS__);                 \
                               } while(0==1)
#endif

#define pcm_uninit msp_device_free
#define mixer_uninit msp_device_free

#define pcm_dev(dev) &(((msp_pcm_dev_t *)dev)->pcm)
#define pcm_ops(dev) &(((msp_pcm_drv_t *)((((msp_pcm_dev_t *)dev)->device.drv)))->ops)


//#define DAC_WRITE_EVENT  (0x01)
//#define ADC_READ_EVENT   (0x02)

//#define DAC_GAIN_MIN     (-31)
//#define DAC_GAIN_MAX     (-5)   // FIXME: because of pangu C2 board.
#define INPUT_CACHE_TIME            (1200) //MS

#define BYTE_TO_BITS            (8)
#define SECOND_TO_MILLISECONDS  (1000)

//#define DMA_NUM                 (0)
/* input fixed config for bl616 ，other configurations are not supported for now */
#define INPUT_SAMPLE_RATE       (16000)
#define INPUT_SAMPLE_BITS       (16)
#define INPUT_CHANNELS          (1)

#define FRAME_SIZE              (INPUT_SAMPLE_RATE * INPUT_SAMPLE_BITS / BYTE_TO_BITS * INPUT_CHANNELS / SECOND_TO_MILLISECONDS)  // 32bytes/ms
#define CONST_RX_INPUT_BUFSIZE      (FRAME_SIZE * INPUT_CACHE_TIME)   /*for ringbuffer*/
#define CONST_TX_OUTPUT_BUFSIZE     ((24+512)*16 + 2*32) // output

#if 0
#define CONST_TX_DMA_CTRL_ID        (0)
#define CONST_TX_DMA_CH_ID          (0)
#define CONST_RX_DMA_CTRL_ID        (0)
#define CONST_RX_DMA_CH_ID          (1)
#endif
typedef struct {
    xcodec_output_t *hdl;
    void *dma_hdl;
    msp_pcm_hw_params_t params;
    xcodec_output_config_t *output_config;
    int state;
} playback_t;

typedef struct {
    xcodec_input_t  *hdl;
    void *dma_hdl;
    msp_pcm_hw_params_t params;
    int state;
} capture_t;

typedef struct {
    xcodec_output_t hdl;
    int l;
    int r;
} mixer_playback_t;

static xcodec_t codec_a;

mixer_playback_t mixp0;

static int pcmp_param_set(msp_pcm_t *pcm, msp_pcm_hw_params_t *params);
static int pcmc_param_set(msp_pcm_t *pcm, struct msp_pcm_hw_params *params);

static void playback_free(playback_t *playback)
{
    if (playback->state == 1) {
        xcodec_output_stop(playback->hdl);
        xcodec_output_link_dma(playback->hdl, NULL);
        xcodec_output_close(playback->hdl);

        //msp_free(playback->hdl->ring_buf->buffer_ptr);
        msp_free(playback->hdl->ring_buf);

        msp_free(playback->hdl);
        msp_free(playback->dma_hdl);

        msp_free(playback->output_config->buffer);
        msp_free(playback->output_config);

        playback->state   = 0;
        playback->state   = 0;
        playback->state   = 0;
        playback->hdl     = NULL;
        playback->dma_hdl = NULL;
        playback->output_config->buffer = NULL;
        playback->output_config = NULL;
    }
}

static int pcmp_lpm(msp_dev_t *dev, int state)
{
    msp_pcm_t *pcm = pcm_dev(dev);
    playback_t *playback = (playback_t *)pcm->hdl;

    if (state > 0) {
        playback_free(playback);
    } else {
        pcmp_param_set(pcm, &playback->params);
    }

    return 0;
}

static int pcmp_open(msp_dev_t *dev)
{
    msp_pcm_t *pcm = pcm_dev(dev);
    playback_t *playback = msp_zalloc(sizeof(playback_t));

    CHECK_RET_TAG_WITH_RET(NULL != playback, -1);
    pcm->hdl = playback;
    playback->state = 0;

    return 0;
}

static int pcmp_close(msp_dev_t *dev)
{
    msp_pcm_t *pcm = pcm_dev(dev);
    playback_t *playback = (playback_t *)pcm->hdl;

    playback_free(playback);
    msp_free(playback);
    pcm->hdl = NULL;

    return 0;
}

volatile int g_tx_cnt;
static void codec_event_cb(xcodec_input_t *codec, xcodec_event_t event, void *arg)
{
    msp_pcm_t *pcm = (msp_pcm_t *)arg;

    if (event == XCODEC_EVENT_PERIOD_WRITE_COMPLETE) {
        g_tx_cnt ++;
        pcm->event.cb(pcm, PCM_EVT_WRITE, pcm->event.priv);
    } else if (event == XCODEC_EVENT_PERIOD_READ_COMPLETE) {
        pcm->event.cb(pcm, PCM_EVT_READ, pcm->event.priv);
    } else if (event == XCODEC_EVENT_READ_BUFFER_FULL) {
        pcm->event.cb(pcm, PCM_EVT_XRUN, pcm->event.priv);
    } else {
        pcm->event.cb(pcm, PCM_EVT_XRUN, pcm->event.priv);
    }
}

static int pcmp_param_set(msp_pcm_t *pcm, msp_pcm_hw_params_t *params)
{
    //uint32_t g_dmachoutput_handle;// fixme unused
    playback_t *playback = (playback_t *)pcm->hdl;

    playback_free(playback);

    xcodec_output_t *codec = msp_zalloc(sizeof(xcodec_output_t));
    codec->ring_buf = msp_zalloc(sizeof(m_ringbuf_t));
    codec->sound_channel_num = 1;
    CHECK_RET_TAG_WITH_RET(NULL != codec, -1);

#ifdef CONST_TX_OUTPUT_BUFSIZE
    uint8_t *send = msp_malloc(CONST_TX_OUTPUT_BUFSIZE);
#else
    uint8_t *send = msp_malloc(params->buffer_bytes);
#endif
    if (send == NULL) {
        goto pcmp_err0;
    }

    playback->output_config = msp_malloc(sizeof(xcodec_output_config_t));
    if (playback->output_config == NULL) {
        goto pcmp_err0;
    }

    int ret = xcodec_output_open(&codec_a, codec, pcm->pcm_name[4] - 0x30);
    if (ret != 0) {
        goto pcmp_err1;
    }

    xcodec_output_attach_callback(codec, codec_event_cb, pcm);

    playback->output_config->bit_width = params->sample_bits;
    playback->output_config->sample_rate = params->rate;
    playback->output_config->buffer = send;

#ifdef CONST_TX_OUTPUT_BUFSIZE
    playback->output_config->buffer_size = CONST_TX_OUTPUT_BUFSIZE;
#else
    playback->output_config->buffer_size = params->buffer_bytes;
#endif
    playback->output_config->period = 512;
    //output_config.period = 2048;
    playback->output_config->mode = XCODEC_OUTPUT_SINGLE_ENDED;
    playback->output_config->sound_channel_num = params->channels;
#if 0
    if (g_audio_gain_config) {
        playback->output_config->negative_pin = g_audio_gain_config->out_negative_pin;
        playback->output_config->positive_pin = g_audio_gain_config->out_positive_pin;
    }
#endif
    ret = xcodec_output_config(codec, playback->output_config);
    if (ret != 0) {
        goto pcmp_err1;
    }

    if (mixp0.hdl.callback == NULL) {
        memcpy(&mixp0.hdl, codec, sizeof(xcodec_output_t));
    }

    // if (mixp0.l == -31 || mixp0.r == -31) {
    //     xcodec_output_mute(codec, 1);
    // } else {
    //     xcodec_output_mute(codec, 0);

    //     // xcodec_output_analog_gain(codec, mixp0.l);

    // }
#if 0
    if (g_audio_gain_config) {
        extern int auo_analog_gain(void *context, float val);
        auo_analog_gain(NULL, (float)g_audio_gain_config->audio_out_gain_list[0]);
    }
#endif
    // xcodec_output_buffer_reset(codec);

    xcodec_dma_ch_t *dma_hdl = msp_zalloc_check(sizeof(xcodec_dma_ch_t));
//    dma_hdl->ctrl_id = CONST_TX_DMA_CTRL_ID;
//    dma_hdl->ch_id = CONST_TX_DMA_CH_ID;

    xcodec_output_link_dma(codec, dma_hdl);
    playback->state         = 1;
    playback->hdl           = codec;
    playback->dma_hdl       = dma_hdl;
    memcpy(&playback->params, params, sizeof(msp_pcm_hw_params_t));

    xcodec_output_start(codec);
    user_log("code init ok\r\n");
    return 0;
pcmp_err1:
    msp_free(send);
pcmp_err0:
    msp_free(codec);

    return -1;
}

//#define DUMP_DATA_FEATURE
#ifdef DUMP_DATA_FEATURE
#define DUMP_DATA_LEN (256*1024)
uint8_t g_xdata[DUMP_DATA_LEN];
int g_xlen;
#endif

static int pcm_send(msp_pcm_t *pcm, void *data, int len)
{
    playback_t *playback = (playback_t *)pcm->hdl;
    int ret = xcodec_output_write_async(playback->hdl, (uint8_t *)data, len);

#ifdef DUMP_DATA_FEATURE
    if (ret + g_xlen > DUMP_DATA_LEN) {
        // g_xlen = -1;
    } else {
        memcpy(g_xdata + g_xlen, (uint8_t*)data, ret);
        g_xlen += ret;
    }
#endif
    // printf("---------------------(%d)\r\n", ret);
    return ret;
}

static int pcm_pause(msp_pcm_t *pcm, int enable)
{
    playback_t *playback = (playback_t *)pcm->hdl;

    if (enable) {
        xcodec_output_pause(playback->hdl);
    } else {
        xcodec_output_resume(playback->hdl);
    }

    return 0;
}

/* left_gain/right_gain [-31, 0] 1dB step*/
static int snd_set_gain(msp_mixer_elem_t *elem, int l, int r)
{
    if (mixp0.hdl.callback != NULL) {

        // xcodec_output_t *p = &mixp0.hdl;

        // if (l == -31 || r == -31) {
        //     xcodec_output_mute(p, 1);
        // } else {
        //     xcodec_output_mute(p, 0);
        //     xcodec_output_analog_gain(p, l);
        // }
        mixp0.l = l;
        mixp0.r = r;
    } else {
        mixp0.l = l;
        mixp0.r = r;
    }

    return 0;
}

static int snd_volume_to_dB(msp_mixer_elem_t *elem, int val)
{
#if 0
    int gain;
    int gain_s = DAC_GAIN_MIN, gain_e = DAC_GAIN_MAX;

    //FIXME
    gain = gain_s + (val / 100.0 * (gain_e - gain_s));
    return gain;
#endif
    return 0;
}


static sm_elem_ops_t elem_codec1_ops = {
    .set_dB       = snd_set_gain,
    .volume_to_dB = snd_volume_to_dB,
};

static msp_dev_t *pcm_init(msp_driver_t *drv, void *config, int id)
{
    msp_pcm_dev_t *pcm_dev = (msp_pcm_dev_t *)msp_device_new(drv, sizeof(msp_pcm_dev_t), id);
    msp_pcm_drv_t *pcm_drv = (msp_pcm_drv_t *)drv;

    memset(&pcm_dev->pcm, 0x00, sizeof(msp_pcm_t));
    pcm_dev->pcm.ops = &(pcm_drv->ops);

    return (msp_dev_t *)(pcm_dev);
}

static void capture_free(capture_t *capture)
{
    if (capture->state == 1) {
        xcodec_input_stop(capture->hdl);
        xcodec_input_close(capture->hdl);
        //msp_free(capture->hdl->ring_buf->buffer_ptr);
        msp_free(capture->hdl->ring_buf);
        msp_free(capture->hdl);
        capture->state = 0;
        capture->hdl = 0;
    }
}

static int pcmc_lpm(msp_dev_t *dev, int state)
{
    msp_pcm_t *pcm = pcm_dev(dev);
    capture_t *capture = (capture_t *)pcm->hdl;

    if (state > 0) {
        capture_free(capture);
    } else {
        pcmc_param_set(pcm, &capture->params);
    }

    return 0;
}

static int pcmc_open(msp_dev_t *dev)
{
    msp_pcm_t *pcm = pcm_dev(dev);
    capture_t *capture = msp_zalloc(sizeof(capture_t));

    CHECK_RET_TAG_WITH_RET(NULL != capture, -1);
    pcm->hdl = capture;
    capture->state = 0;

    return 0;
}

static int pcmc_close(msp_dev_t *dev)
{
    msp_pcm_t *pcm = pcm_dev(dev);
    capture_t *capture = (capture_t *)pcm->hdl;

    capture_free(capture);
    msp_free(capture);
    return 0;
}

static int pcmc_param_set(msp_pcm_t *pcm, struct msp_pcm_hw_params *params)
{
#if 1
    capture_t *capture = (capture_t *)pcm->hdl;

    capture_free(capture);

    xcodec_input_config_t input_config;
    xcodec_input_t *codec = msp_zalloc(sizeof(xcodec_input_t));

    CHECK_RET_TAG_WITH_RET(NULL != codec, -1);

#ifdef CONST_RX_INPUT_BUFSIZE
    uint8_t *recv = msp_malloc(CONST_RX_INPUT_BUFSIZE);
#else
    uint8_t *recv = msp_malloc(params->buffer_bytes);
#endif
    if (recv == NULL) {
        goto pcmc_err0;
    }

    /* micbias set high if need */
    //msp_gpio_output_config(24, 1);
    //msp_gpio_output_set(24, 1);

    // pcm->pcm_name[4] is ch ascii data, change to decimal num
    codec->ring_buf = msp_zalloc(sizeof(m_ringbuf_t));
    codec->dma = msp_zalloc_check(sizeof(xcodec_dma_ch_t));
    int ret = xcodec_input_open(&codec_a, codec, pcm->pcm_name[4] - 0x30);
    if (ret != 0) {
        goto pcmc_err1;
    }

    /* input ch config */
    xcodec_input_attach_callback(codec, codec_event_cb, pcm);
    input_config.bit_width = params->sample_bits;
    input_config.sample_rate = params->rate;
    input_config.buffer = recv;
#ifdef CONST_RX_INPUT_BUFSIZE
    input_config.buffer_size = CONST_RX_INPUT_BUFSIZE;
#else
    input_config.buffer_size = params->buffer_bytes;
#endif
    input_config.period = params->period_bytes;
    input_config.mode = XCODEC_INPUT_DIFFERENCE;
    input_config.sound_channel_num = params->channels;
    /*config negative and positive*/
    //input_config.negative_pin = INPUT_NEGATIVE_PIN;   //negative pin
    //input_config.positive_pin = INPUT_POSITIVE_PIN;  //positive pin

    printf("input bit_width(%ld), sample_rate(%ld), buffer_size(%ld), period(%ld), sound_channel_num(%ld)\r\n", \
            input_config.bit_width, input_config.sample_rate, input_config.buffer_size, input_config.period, input_config.sound_channel_num);
    ret = xcodec_input_config(codec, &input_config);
    if (ret != 0) {
        goto pcmc_err1;
    }
    xcodec_input_analog_gain(codec, 0);

    //xdma_ch_t *dma_hdl = msp_zalloc_check(sizeof(xdma_ch_t));
    xcodec_input_link_dma(codec, codec->dma);

    xcodec_input_start(codec);

    capture->state = 1;
    capture->hdl = codec;
    capture->dma_hdl = NULL;//dma_hdl;
    memcpy(&capture->params, params, sizeof(msp_pcm_hw_params_t));

    printf("csi codec input open success\r\n");
    return 0;

pcmc_err1:
    msp_free(recv);
pcmc_err0:
    msp_free(codec);
#endif
    return -1;
}

static int pcm_recv(msp_pcm_t *pcm, void *buf, int size)
{
    capture_t *capture = (capture_t *)pcm->hdl;

    int ret = xcodec_input_read(capture->hdl, (uint8_t *)buf, size);

    return ret;
}

int pcmc_get_remain_size(msp_pcm_t *pcm)
{
    capture_t *capture = (capture_t *)pcm->hdl;

    return xcodec_input_buffer_avail(capture->hdl);
}

static msp_pcm_drv_t msp_pcm_drv[] = {
    {
        .drv = {
            .name               = "pcmP",
            .init               = pcm_init,
            .uninit             = pcm_uninit,
            .open               = pcmp_open,
            .close              = pcmp_close,
            .lpm                = pcmp_lpm,
        },
        .ops = {
            .hw_params_set      = pcmp_param_set,
            .write              = pcm_send,
            .pause              = pcm_pause,
        },
    },
    {
        .drv = {
            .name               = "pcmC",
            .init               = pcm_init,
            .uninit             = pcm_uninit,
            .open               = pcmc_open,
            .close              = pcmc_close,
            .lpm                = pcmc_lpm,
        },
        .ops = {
            .hw_params_set = pcmc_param_set,
            .read = pcm_recv,
            .hw_get_remain_size = pcmc_get_remain_size,
        },
    }
};

static int msp_pcm_register(void)
{
    msp_driver_register(&msp_pcm_drv[0].drv, NULL, 0);
    msp_driver_register(&msp_pcm_drv[1].drv, NULL, 0);
    // driver_register(&msp_pcm_drv[1].drv, NULL, 2);
    // driver_register(&msp_pcm_drv[1].drv, NULL, 1);

    return 0;
}

static int msp_pcm_unregister(void)
{
    msp_driver_unregister("pcmP0");

    return 0;
}

static msp_dev_t *card_init(msp_driver_t *drv, void *config, int id)
{
    card_dev_t *card = (card_dev_t *)msp_device_new(drv, sizeof(card_dev_t), id);
    snd_card_drv_t *card_drv = (snd_card_drv_t *)drv;
    msp_mixer_elem_t *elem;

    xcodec_output_t *codec = msp_zalloc(sizeof(xcodec_output_t));
    //codec->pa_pin = ((snd_bl616_config_t*)config)->pa_pin;
    codec_a.output_chs = codec;
    xcodec_error_t ret;
    ret = xcodec_init(&codec_a, 0);
    if (ret != XCODEC_OK) {
        printf("xcodec_init error\n");
    }

    //FIXME:  must sleep 500ms at least before PA ON, otherwise baoyin happens
    msp_msleep(500);
    msp_pcm_register();
    msp_slist_init(&card_drv->mixer_head);

    snd_elem_new(&elem, "codec0", &elem_codec1_ops);
    msp_slist_add(&elem->next, &card_drv->mixer_head);

    msp_free(codec);
    return (msp_dev_t *)card;
}

static void card_uninit(msp_dev_t *dev)
{
    xcodec_uninit(&codec_a);
    msp_pcm_unregister();
    msp_device_free(dev);
}

static int card_open(msp_dev_t *dev)
{

    return 0;
}

static int card_close(msp_dev_t *dev)
{

    return 0;
}

static int card_lpm(msp_dev_t *dev, int state)
{

    return 0;
}

static snd_card_drv_t snd_card_drv = {
    .drv = {
        .name   = "card",
        .init   = card_init,
        .uninit = card_uninit,
        .open   = card_open,
        .close  = card_close,
        .lpm    = card_lpm,
    }
};

void snd_card_bl616_register(void *config)
{
    if (config) {
        if (g_audio_gain_config == NULL) {
            g_audio_gain_config = (snd_bl616_config_t*)msp_malloc(sizeof(snd_bl616_config_t));
            *g_audio_gain_config = *((snd_bl616_config_t*)config);
        } else {
            /* 再次调用注册,仅仅更新参数 */
            *g_audio_gain_config = *((snd_bl616_config_t*)config);
            return;
        }
    }
    msp_driver_register(&snd_card_drv.drv, config, 0);
}

void snd_card_bl616_unregister(void *config)
{
    msp_driver_unregister(snd_card_drv.drv.name);
    if (g_audio_gain_config) {
        msp_free(g_audio_gain_config);
    }
}

