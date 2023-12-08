/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

// #include <bl616_platform.h>
#include <stdio.h>
#include <msp/kernel.h>
#include <xutils/debug.h>
#include <msp_port.h>
#include <bl616_glb_gpio.h>
#include <bl616_gpio.h>
uint32_t g_pa_delay_1 = 5;
uint32_t g_pa_delay_2 = 90;     /* this is time set according to the pa manual */

/***********user config************/

#if CONFIG_CODEC_USE_I2S_RX
/* empty here now */
#else
/* codec pin config */
/* if use single mode. pin set as 255 */
#define INPUT_NEGATIVE_PIN      (28)
#define INPUT_POSITIVE_PIN      (27)
#endif

#if CONFIG_CODEC_USE_I2S_TX
/* empty here now */
#else
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

#if CONFIG_CODEC_USE_I2S_RX || CONFIG_CODEC_USE_I2S_TX
#define CONFIG_CODEC_USE_I2S  (1)
#endif

#if CONFIG_CODEC_USE_I2S
#include <bflb_gpio.h>
#include <bflb_core.h>
#include <bflb_i2s.h>
#include <codec_es8388.h>

static void i2s_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    /* I2S_FS */
    bflb_gpio_init(gpio, GPIO_PIN_13, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* I2S_DI */
    bflb_gpio_init(gpio, GPIO_PIN_14, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* I2S_DO */
    bflb_gpio_init(gpio, GPIO_PIN_15, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* I2S_BCLK */
    bflb_gpio_init(gpio, GPIO_PIN_20, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* MCLK CLKOUT */
    bflb_gpio_init(gpio, GPIO_PIN_23, GPIO_FUNC_CLKOUT | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);

    /* I2C0_SCL */
    bflb_gpio_init(gpio, GPIO_PIN_10, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    /* I2C0_SDA */
    bflb_gpio_init(gpio, GPIO_PIN_11, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
}

static void mclk_out_init()
{
    /* output MCLK,
    Will change the clock source of i2s,
    It needs to be called before i2s is initialized
    clock source 25M
    */
    GLB_Set_I2S_CLK(ENABLE, 2, GLB_I2S_DI_SEL_I2S_DI_INPUT, GLB_I2S_DO_SEL_I2S_DO_OUTPT);
    GLB_Set_Chip_Clock_Out3_Sel(GLB_CHIP_CLK_OUT_3_I2S_REF_CLK);
}

static struct bflb_device_s *i2s0;
void msp_i2s_device_init(uint32_t sample_rate)
{
    struct bflb_i2s_config_s i2s_cfg = {
        .bclk_freq_hz = 16000 * 16 * 2, /* bclk = Sampling_rate * frame_width * channel_num */
        .role = I2S_ROLE_MASTER,
        .format_mode = I2S_MODE_LEFT_JUSTIFIED,
        .channel_mode = I2S_CHANNEL_MODE_NUM_1,
        .frame_width = I2S_SLOT_WIDTH_16,
        .data_width = I2S_SLOT_WIDTH_16,
        .fs_offset_cycle = 0,

        .tx_fifo_threshold = 0,
        .rx_fifo_threshold = 0,
    };
    static ES8388_Cfg_Type ES8388Cfg = {
        .work_mode = ES8388_CODEC_MDOE,          /*!< ES8388 work mode */
        .role = ES8388_SLAVE,                    /*!< ES8388 role */
        .mic_input_mode = ES8388_DIFF_ENDED_MIC, /*!< ES8388 mic input mode */
        .mic_pga = ES8388_MIC_PGA_3DB,           /*!< ES8388 mic PGA */
        .i2s_frame = ES8388_LEFT_JUSTIFY_FRAME,  /*!< ES8388 I2S frame */
        .data_width = ES8388_DATA_LEN_16,        /*!< ES8388 I2S dataWitdh */
    };
    
    i2s_cfg.bclk_freq_hz = sample_rate * 16 * 2;
    /* gpio init */ 
    i2s_gpio_init();
    /* mclk clkout init */
    mclk_out_init();
    /* init ES8388 Codec */
    printf("es8388 init\n\r");
    ES8388_Init(&ES8388Cfg);
    ES8388_Set_Voice_Volume(100);
    i2s0 = bflb_device_get_by_name("i2s0");
    /* i2s init */
    bflb_i2s_init(i2s0, &i2s_cfg);
    /* enable dma */
#if CONFIG_CODEC_USE_I2S_TX
    bflb_i2s_link_txdma(i2s0, true);
#endif
#if CONFIG_CODEC_USE_I2S_RX
    bflb_i2s_link_rxdma(i2s0, true);
#endif
    //bflb_i2s_txint_mask(i2s0, 0);
    //bflb_i2s_rxint_mask(i2s0, 0);
        //bflb_i2s_feature_control(i2s0, I2S_CMD_DATA_ENABLE, I2S_CMD_DATA_ENABLE_TX);
}

void msp_i2s_tx_enable(void)
{
    if (i2s0) {
        bflb_i2s_feature_control(i2s0, I2S_CMD_DATA_ENABLE, I2S_CMD_DATA_ENABLE_TX);
    }
}

void msp_i2s_rx_enable(void)
{
    if (i2s0) {
        bflb_i2s_feature_control(i2s0, I2S_CMD_DATA_ENABLE, I2S_CMD_DATA_ENABLE_RX);
    }
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
#if CONFIG_CODEC_USE_I2S_TX
        .dma_tx_dstPeriph = 17,     //13:DMA_REQ_AUDIO_TX, 17:DMA_REQ_I2S_TX
#else
        .dma_tx_dstPeriph = 13,     //13:DMA_REQ_AUDIO_TX, 17:DMA_REQ_I2S_TX
#endif

#if CONFIG_CODEC_USE_I2S_RX
        .dma_rx_srcPeriph = 16,     //21:DMA_REQ_SOLO_RX, 16:DMA_REQ_I2S_RX
#else
        .dma_rx_srcPeriph = 21,     //21:DMA_REQ_SOLO_RX, 16:DMA_REQ_I2S_RX
#endif
    };

    return dma_channel_cfg;
}

#if !CONFIG_CODEC_USE_I2S_TX || !CONFIG_CODEC_USE_I2S_RX
/* codec pin config */
msp_codec_pin_cfg_t msp_codec_pin_config(void)
{
    msp_codec_pin_cfg_t codec_pin_cfg = {
#if !CONFIG_CODEC_USE_I2S_RX
        .input_negative_pin = INPUT_NEGATIVE_PIN, 
        .input_positive_pin = INPUT_POSITIVE_PIN, 
#endif
#if !CONFIG_CODEC_USE_I2S_TX
        .output_negative_pin = OUTPUT_NEGATIVE_PIN, 
        .output_positive_pin = OUTPUT_POSITIVE_PIN, 
#endif
    };

    return codec_pin_cfg;
}
#endif

void msp_codec_pa_init_pre(void)
{
//#if CONFIG_ENABLE_BOTTOM_PA
    // pullup
#if CONFIG_CODEC_USE_I2S
    //msp_i2s_device_init();
#endif
    msp_gpio_output_config(CONFIG_AUDIO_PA_PIN, 1);
    //bl_gpio_enable_output(pa_pin, 1, 0);
    MSP_LOGD("msp_codec_pa_init_pre\r\n");
//#endif
}

void msp_codec_pa_init(void)
{
// #if CONFIG_ENABLE_BOTTOM_PA
//     // init
//     bl_gpio_enable_output(CONFIG_AUDIO_PA_PIN, 1, 0);
// #endif
//#if CONFIG_ENABLE_BOTTOM_PA
    // gpio set 0 ---> disable
    //msp_msleep(1000);
    msp_gpio_output_set(CONFIG_AUDIO_PA_PIN, 0);
    msp_msleep(1);
    MSP_LOGD("msp_codec_pa_init\r\n");
//#endif
}

//void msp_codec_pa_after_opendac(void){}

void msp_codec_pa_after_opendac(void)
{
    static int init = 0;
//#if CONFIG_ENABLE_BOTTOM_PA
    // enable
    // enable, gpio set 1
    //user_log("msp_codec_pa_after_opendac delay1 ---> %d ms\r\n", g_pa_delay_1);
    msp_msleep(g_pa_delay_1);
    if (0 == init) {
        //msp_msleep(1000);
    }
    msp_gpio_output_set(CONFIG_AUDIO_PA_PIN, 1);
    //user_log("msp_codec_pa_after_opendac delay2 ---> %d ms\r\n", g_pa_delay_2);
    if (0 == init) {
        //msp_msleep(1000);
        init = 1;
    }
    msp_msleep(g_pa_delay_2);
    MSP_LOGD("msp_codec_pa_after_opendac\r\n");
//#endif
}

void msp_codec_pa_before_closedac(void)
{
//#if CONFIG_ENABLE_BOTTOM_PA
    //msp_msleep(50);
    // disable, gpio set 0
    msp_gpio_output_set(CONFIG_AUDIO_PA_PIN, 0);
    msp_msleep(20);
    MSP_LOGD("msp_codec_pa_before_closedac\r\n");
//#endif
}
#if 0
extern void bl_irq_register_with_ctx(int irqnum, void *handler, void *ctx);
extern void bl_irq_unregister(int irqnum, void *handler);
extern void bl_irq_enable(unsigned int source);
extern void bl_irq_disable(unsigned int source);

// irq register
void msp_irq_register(int irqnum, void *handler, void *ctx)
{
   bl_irq_register_with_ctx(irqnum, handler, ctx);
}

void msp_irq_unregister(int irqnum)
{
   bl_irq_register_with_ctx(irqnum, NULL, NULL);
}

void msp_irq_enable(int irqnum)
{
   bl_irq_enable((unsigned int)irqnum);
}

void msp_irq_disenable(int irqnum)
{
   bl_irq_disable((unsigned int)irqnum);
}

// memory
void *msp_malloc_align(int size, int align_bytes)
{
    void *base_ptr = NULL;
    void *mem_ptr = NULL;

    base_ptr = msp_malloc(size + align_bytes);//alloc alignbytes, rather than align_bytes-1, because we need to store offset

    mem_ptr = (void *)((int)((int *)base_ptr + align_bytes -1) & ~(align_bytes-1));
    if(mem_ptr == base_ptr) {//base_ptr already align_bytes align
        mem_ptr = base_ptr + align_bytes;//force move it to one more  alignbytes
    }
    *((int *)mem_ptr-1) = mem_ptr - base_ptr;
    // MSP_LOGD("offset is %d base prt %x mem_ptr %x\n",*((int *)mem_ptr -1),base_ptr,mem_ptr);
    return mem_ptr;
}

void msp_free_align(void *ptr)
{
    void *base_addr = NULL;
    // MSP_LOGD("%x %x\n",ptr,*((int *)ptr - 1));
    base_addr = (void *)(ptr- *((int *)ptr-1));
    // MSP_LOGD("ptr %x base_addr %x\n",ptr,base_addr);
    msp_free(base_addr);
}

//xTaskGetTickCount
uint64_t msp_gettick_count(void)
{
    uint64_t count;

    count = (uint64_t)((xPortIsInsideInterrupt())?(xTaskGetTickCountFromISR()):(xTaskGetTickCount()));

    return count;
}

/*
 * msp_test_mem(10, 100, 32);
 * return 0-success
 */
int msp_test_mem(uint32_t test_times, uint32_t test_mem_length, uint32_t test_mem_align)
{
    char *p1 = NULL;
    char *p2 = NULL;
    int i = 0;

    for (i = 0; i < test_times; i++) {
        // test malloc p1
        p1 = msp_malloc_align(test_mem_length, test_mem_align);
        if (NULL == p1) {
            MSP_LOGE("mem error\r\n");
            return -1;
        }

        // ("msp_malloc_align = %p\r\n", p1);

        if ((uint32_t)p1%test_mem_align) {
            MSP_LOGE("mem error\r\n");
            return -2;
        }

        // test malloc p2
        p2 = msp_malloc_align(test_mem_length, test_mem_align);
        if (NULL == p2) {
            MSP_LOGE("mem error\r\n");
            return -1;
        }

        // ("msp_malloc_align = %p\r\n", p2);

        if ((uint32_t)p2%test_mem_align) {
            MSP_LOGE("mem error\r\n");
            return -2;
        }

        // test free p1/p2
        msp_free_align(p1);
        msp_free_align(p2);
    }

    return 0;
}

void msp_dma_chan_start(int chan)
{
    hosal_dma_chan_start(chan);
}

void msp_dma_chan_stop(int chan)
{
    hosal_dma_chan_stop(chan);
}

void msp_dma_irq_callback_set(int chan, msp_dma_irq_t pfn, void *p_arg)
{
    hosal_dma_irq_callback_set(chan, pfn, p_arg);
}

int msp_dma_chan_request(int arg)
{
    int chan = 0;

    chan = hosal_dma_chan_request(0);

    return chan;
}

int msp_dma_chan_release(int chan)
{
    int ret;

    ret = hosal_dma_chan_release(chan);

    return ret;
}
#endif
