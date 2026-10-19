/* Converted media.c */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <peer_connection.h>

#include <opus/opus.h>
#include <xcodec.h>

#define OPUS_OUT_BUFFER_SIZE 1276  // 1276 bytes is recommended by opus_encode

#define OPUS_ENCODER_BITRATE 30000
#define OPUS_ENCODER_COMPLEXITY 0

/* user config param */
#define INPUT_CACHE_TIME            (1000) //MS
#define INPUT_PERIOD_TIME           (20)  // this config will decide XCODEC_EVENT_PERIOD_READ_COMPLETE evernt callback time MS
#define OUTPUT_CACHE_TIME           (1000) //MS
#define OUTPUT_PERIOD_TIME          (20)  // this config will decide XCODEC_EVENT_PERIOD_READ_COMPLETE evernt callback time MS

/* end user config param */

#define BYTE_TO_BITS                (8)
#define SECOND_TO_MILLISECONDS      (1000)

/* input fixed config for bl616 ，other configurations are not supported for now */
#define INPUT_SAMPLE_RATE           (8000)
#define INPUT_SAMPLE_BITS           (16)
#define INPUT_CHANNELS              (1)
#define OUTPUT_SAMPLE_RATE          (8000)
#define OUTPUT_SAMPLE_BITS          (16)
#define OUTPUT_CHANNELS             (1)

#define INPUT_FRAME_SIZE        (INPUT_SAMPLE_RATE * INPUT_SAMPLE_BITS / BYTE_TO_BITS * INPUT_CHANNELS / SECOND_TO_MILLISECONDS)  // 16bytes/ms
#define INPUT_BUFFER_SIZE       (INPUT_FRAME_SIZE * INPUT_CACHE_TIME)   /*for ringbuffer*/
#define INPUT_PERIOD_SIZE       (INPUT_FRAME_SIZE * INPUT_PERIOD_TIME)  /*320bytes,20ms*/

#define OUTPUT_FRAME_SIZE       (OUTPUT_SAMPLE_RATE * OUTPUT_SAMPLE_BITS / BYTE_TO_BITS * OUTPUT_CHANNELS / SECOND_TO_MILLISECONDS)  // 16bytes/ms
#define OUTPUT_BUFFER_SIZE      (OUTPUT_FRAME_SIZE * OUTPUT_CACHE_TIME)   /*for ringbuffer*/
#define OUTPUT_PERIOD_SIZE      (OUTPUT_FRAME_SIZE * OUTPUT_PERIOD_TIME)  /*320bytes,20ms*/

#define BUFFER_SAMPLES          (INPUT_FRAME_SIZE * INPUT_PERIOD_TIME)

static xcodec_t             g_codec;

static uint8_t              *g_input_buffer;
static xcodec_dma_ch_t      *g_dma_hdl_in;
static xcodec_input_t       g_codec_input_ch;
static m_ringbuf_t          g_input_ringbuf;

static uint8_t              *g_output_buffer;
static xcodec_dma_ch_t      *g_dma_hdl_out;
static xcodec_input_t       g_codec_output_ch;
static m_ringbuf_t          g_output_ringbuf;

static void codec_output_event_cb_fun(xcodec_output_t *output, xcodec_event_t event, void *arg)
{
    if (event == XCODEC_EVENT_PERIOD_WRITE_COMPLETE) {
        //printf("out_period\r\n");
    } else if (event == XCODEC_EVENT_WRITE_BUFFER_EMPTY) {
        //printf("output CODEC_EVENT_WRITE_BUFFER_EMPTY\r\n");
    } else {

    }
}
static void _codec_input_event_cb_fun(xcodec_input_t *i2s, xcodec_event_t event, void *arg)
{
    if (event == XCODEC_EVENT_PERIOD_READ_COMPLETE) {
        //printf("compile cb:%lu\r\n", (uint32_t)msp_now_ms()); 
    } 
}

int oai_audio_init(void)
{
    xcodec_error_t ret = XCODEC_OK;

    printf("codec_init\r\n");

    ret = xcodec_init(&g_codec, 0);
    if (ret != XCODEC_OK) {
        printf("xcodec_init error\r\n");
        return -1;
    }

    /* input */
    g_input_buffer = msp_zalloc_check(INPUT_BUFFER_SIZE);
    if (NULL == g_input_buffer) {
        printf("malloc error!\r\n");
        return -1;
    }

    /* micbias set high if need */
    //msp_gpio_output_config(24, 1);
    //msp_gpio_output_set(1);

    g_dma_hdl_in = msp_zalloc_check(sizeof(xcodec_dma_ch_t)); 

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

    /* output */
    printf("output init\r\n");
    g_output_buffer = msp_zalloc_check(OUTPUT_BUFFER_SIZE); 

    if (g_output_buffer == NULL) {
        printf("malloc failed!\r\n");
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

    xcodec_output_config(&g_codec_output_ch, &output_config);
    xcodec_output_analog_gain(&g_codec_output_ch, 50);
    xcodec_output_buffer_reset(&g_codec_output_ch);

    xcodec_output_link_dma(&g_codec_output_ch, g_dma_hdl_out);
    return 0;
}

static opus_int16 *output_buffer = NULL;
static OpusDecoder *opus_decoder = NULL;

void oai_init_audio_decoder(void)
{
    int decoder_error = 0;

    opus_decoder = opus_decoder_create(OUTPUT_SAMPLE_RATE, 1, &decoder_error);
    if (decoder_error != OPUS_OK) {
        printf("Failed to create OPUS decoder\r\n");
        return;
    }

    output_buffer = (opus_int16 *)malloc(BUFFER_SAMPLES * sizeof(opus_int16));
    if (!output_buffer) {
        printf("malloc error!\r\n");
    }
}

void oai_audio_decode(uint8_t *data, size_t size)
{
    int decoded_size =
        opus_decode(opus_decoder, data, size, output_buffer, BUFFER_SAMPLES, 0);
    static uint8_t play_start = 0;
    if (!play_start) {
        xcodec_output_start(&g_codec_output_ch);
        play_start = 1;
    }
#if OAI_CONFIG_DEBUG
    static uint32_t now_time, pre_time,total;
    extern long long msp_now_ms(void );
    static int flag = 0;
    now_time = msp_now_ms();
    total += decoded_size;
    if (now_time - pre_time >= 1000) {
        pre_time = now_time;
        printf("====total:%d\r\n",total);
        total = 0;
    }
#endif
    if (decoded_size > 0) {
        int bytes_written = xcodec_output_write(&g_codec_output_ch, output_buffer, decoded_size * sizeof(uint16_t));
#if OAI_CONFIG_DEBUG
    uint32_t xcodec_output_buffer_remain(xcodec_output_t *ch);
    printf("time:%lu, remain:%d, write:%d\r\n",(uint32_t)msp_now_ms(), xcodec_output_buffer_remain(&g_codec_output_ch), bytes_written);
#endif
    }
}

static OpusEncoder *opus_encoder = NULL;
static opus_int16 *encoder_input_buffer = NULL;
static uint8_t *encoder_output_buffer = NULL;

void oai_init_audio_encoder(void)
{
    int encoder_error;
    opus_encoder = opus_encoder_create(INPUT_SAMPLE_RATE, 1, OPUS_APPLICATION_VOIP,
                                     &encoder_error);
    if (encoder_error != OPUS_OK) {
        printf("Failed to create OPUS encoder\r\n");
        return;
    }

    if (opus_encoder_init(opus_encoder, INPUT_SAMPLE_RATE, 1, OPUS_APPLICATION_VOIP) !=
        OPUS_OK) {
        printf("Failed to initialize OPUS encoder\r\n");
        return;
    }

    opus_encoder_ctl(opus_encoder, OPUS_SET_BITRATE(OPUS_ENCODER_BITRATE));
    opus_encoder_ctl(opus_encoder, OPUS_SET_COMPLEXITY(OPUS_ENCODER_COMPLEXITY));
    opus_encoder_ctl(opus_encoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));
    encoder_input_buffer = (opus_int16 *)malloc(BUFFER_SAMPLES);
    encoder_output_buffer = (uint8_t *)malloc(OPUS_OUT_BUFFER_SIZE);
    if (!encoder_input_buffer || !encoder_output_buffer) {
        printf("malloc failed\r\n");
    }
}

void oai_audio_record_start(void)
{
    xcodec_input_start(&g_codec_input_ch);
}

void oai_send_audio(PeerConnection *peer_connection)
{
    if (xcodec_input_buffer_avail(&g_codec_input_ch) < BUFFER_SAMPLES) {
        return;
    }
    int bytes_read = xcodec_input_read(&g_codec_input_ch, encoder_input_buffer, BUFFER_SAMPLES);
    if (bytes_read != BUFFER_SAMPLES) {
        printf("bytes_read:%d\r\n", bytes_read);
    }
    
    int encoded_size =
        opus_encode(opus_encoder, encoder_input_buffer, BUFFER_SAMPLES / 2,
                  encoder_output_buffer, OPUS_OUT_BUFFER_SIZE);

    peer_connection_send_audio(peer_connection, encoder_output_buffer,
                             encoded_size);
}

