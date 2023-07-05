/**
 * @file es8388_i2s.c
 * @brief
 *
 * Copyright (c) 2023 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#include "../audio.h"
#include "bflb_dma.h"
#include "bflb_i2c.h"
#include "bflb_i2s.h"
#include "es8388_i2s.h"

#if defined(BL616)
#include "bl616_glb.h"
#endif

#define ES8388_I2C_SLAVE_ADDR (0x10)

static struct bflb_device_s *i2c0;
static struct bflb_i2c_msg_s msgs[2];

/*******************************************************************************
 * @brief  es8388_write_reg
 *
 * @param  addr: Register address
 * @param  data: write data
 *
 * @return None
 *
*******************************************************************************/
static int es8388_write_reg(uint8_t addr, uint8_t data)
{
    msgs[0].addr = ES8388_I2C_SLAVE_ADDR;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = &addr;
    msgs[0].length = 1;

    msgs[1].flags = 0;
    msgs[1].buffer = &data;
    msgs[1].length = 1;

    return bflb_i2c_transfer(i2c0, msgs, 2);
}

/*******************************************************************************
 * @brief  es8388_read_reg
 *
 * @param  addr: Register address
 * @param  rdata: data
 *
 * @return None
 *
*******************************************************************************/
static int es8388_read_reg(uint8_t addr, uint8_t *rdata)
{
    msgs[0].addr = ES8388_I2C_SLAVE_ADDR;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = &addr;
    msgs[0].length = 1;

    msgs[1].flags = I2C_M_READ;
    msgs[1].buffer = rdata;
    msgs[1].length = 1;

    return bflb_i2c_transfer(i2c0, msgs, 2);
}

/******************************************************************************
 * @brief  es8388 codec mode
 *
 * @param  cfg: None
 *
 * @return success or error
 *
*******************************************************************************/
static int es8388_codec_mode(struct es8388_cfg *cfg)
{
    uint8_t tempVal = 0;
    int ret = 0;

    ret = es8388_write_reg(0x01, 0x58); //power down whole chip analog
    ret = es8388_write_reg(0x01, 0x50); //power up whole chip analog

    ret = es8388_write_reg(0x02, 0xf3); //power down ADC/DAC and reset ADC/DAC state machine
    ret = es8388_write_reg(0x02, 0xf0); //power on   ADC/DAC

    ret = es8388_write_reg(0x2B, 0x80); //set internal ADC and DAC use sanme LRCK clock

    ret = es8388_write_reg(0x00, 0x36); //ADC clock is same as DAC . DACMCLK is the chip master clock source

    if (cfg->role == ES8388_MASTER) {
        ret = es8388_write_reg(0x08, 0x8D);
    } else {
        ret = es8388_write_reg(0x08, 0x00);
    }

    ret = es8388_write_reg(0x04, 0x00); //power up dac

    ret = es8388_write_reg(0x05, 0x00); //stop lowpower mode
    ret = es8388_write_reg(0x06, 0xc3); //stop lowpower mode

    if (cfg->mic_input_mode == ES8388_SINGLE_ENDED_MIC) {
        /*
         * default set LINPUT1 and RINPUT1 as single ended mic input
         * if user want to use LINPUT2 and RINPUT2 as input
         * please set 0x0a register as 0x50
         */
        ret = es8388_write_reg(0x0A, 0xf8);
        ret = es8388_write_reg(0x0B, 0x88); //analog mono mix to left ADC
    } else {
        /*
         * defualt select LIN1 and RIN1 as Mic diff input
         * if user want to use LIN2 and RIN2 as input
         * please set 0x0b register as 0x82
         */
        ret = es8388_write_reg(0x0A, 0xf8); // Fixed stereo problems
        ret = es8388_write_reg(0x0B, 0x82); // stereo
    }

    tempVal = cfg->data_width;
    tempVal <<= 2;
    tempVal |= cfg->i2s_frame;

    ret = es8388_write_reg(0x0C, tempVal); //ADC I2S Format sel as i2s_frame  , data len as data_width

    tempVal = cfg->data_width;
    tempVal <<= 2;
    tempVal |= cfg->i2s_frame;
    tempVal <<= 1;

    ret = es8388_write_reg(0x17, tempVal); //DAC I2S Format sel as i2s_frame  , data len as data_width

    /* when work in master mode , BCLK is devided form MCLK
     * default divider is 256 , see datasheet reigster 13
     */
    if (cfg->role == ES8388_MASTER) {
        ret = es8388_write_reg(0x0d, 0x06); //ADCLRCK = MCLK/256
        ret = es8388_write_reg(0x18, 0x06); //DACLRCK = MCLK/256
    }

    /*set ADC/DAC default volume as 0 db */
    ret = es8388_write_reg(0x10, 0x00); //LADC volume as 0db
    ret = es8388_write_reg(0x11, 0x00); //RADC volume as 0db
    ret = es8388_write_reg(0x1A, 0x00); //LDAC volume as 0db
    ret = es8388_write_reg(0x1B, 0x00); //RDAC volume as 0db

    tempVal = cfg->mic_pga;
    tempVal <<= 4;
    tempVal |= cfg->mic_pga;

    ret = es8388_write_reg(0x09, tempVal); //set ADC PGA as mic pga

    /*ADC ALC default Setting */
    ret = es8388_write_reg(0x12, 0xE2);
    ret = es8388_write_reg(0x13, 0xC0);
    ret = es8388_write_reg(0x14, 0x12);
    ret = es8388_write_reg(0x15, 0x06);
    ret = es8388_write_reg(0x16, 0xC3);

    /*Mixer setting for LDAC to LOUT and RDAC TO ROUT
     * default close mixer
     */
    ret = es8388_write_reg(0x27, 0xB8);
    ret = es8388_write_reg(0x2A, 0xB8);

    ret = es8388_write_reg(0x02, 0x00); //startup FSM and DLL
    bflb_mtimer_delay_ms(500);

    /* set LOUT1 ROUT1 LOUT2 ROUT2 volume */
    ret = es8388_write_reg(0x2e, 0x1E);
    ret = es8388_write_reg(0x2f, 0x1E);
    ret = es8388_write_reg(0x30, 0x1E);
    ret = es8388_write_reg(0x31, 0x1E);

    //ret = es8388_write_reg(0x04,0x3C);//enable LOUT & ROUT
    ret = es8388_write_reg(0x04, 0x24);
    ret = es8388_write_reg(0x26, 0x01);
    ret = es8388_write_reg(0x03, 0x09); //power up ADC Enable LIN &RIN.

    return ret;
}

/******************************************************************************
 * @brief  ES8388 recording mode
 *
 * @param  cfg: None
 *
 * @return None
 *
*******************************************************************************/
static int es8388_recording_mode(struct es8388_cfg *cfg)
{
    uint8_t tempVal = 0;
    int ret = 0;
    ret = es8388_write_reg(0x01, 0x58); //power down whole chip analog
    ret = es8388_write_reg(0x01, 0x50); //power up whole chip analog

    ret = es8388_write_reg(0x02, 0xf3); //power down ADC/DAC and reset ADC/DAC state machine
    ret = es8388_write_reg(0x02, 0xf0); //power on   ADC/DAC
    ret = es8388_write_reg(0x2B, 0x80); //set internal ADC and DAC use the same LRCK clock
    ret = es8388_write_reg(0x00, 0x16); //ADC clock is same as DAC. use ADC MCLK as internal MCLK

    if (cfg->role == ES8388_MASTER) {
        ret = es8388_write_reg(0x08, 0x80);
    } else {
        ret = es8388_write_reg(0x08, 0x00);
    }

    ret = es8388_write_reg(0x04, 0xc0);
    ret = es8388_write_reg(0x05, 0x00);
    ret = es8388_write_reg(0x06, 0xc3);

    if (cfg->mic_input_mode == ES8388_SINGLE_ENDED_MIC) {
        /*
         * default set LINPUT1 and RINPUT1 as single ended mic input
         * if user want to use LINPUT2 and RINPUT2 as input
         * please set 0x0a register as 0x50
         */
        //ret = es8388_write_reg(0x0A,0x00);//select lin1 and rin1 as micphone input
        ret = es8388_write_reg(0x0A, 0xf8);
        ret = es8388_write_reg(0x0B, 0x88); //analog mono mix to left ADC
        //ret = es8388_write_reg(0x0B,0x90); //analog mono mix to right ADC
    } else {
        /*
         * defualt select LIN1 and RIN1 as Mic diff input
         * if user want to use LIN2 and RIN2 as input
         * please set 0x0b register as 0x82
         */
        //ret = es8388_write_reg(0x0A,0xf0);//set micphone input as difference mode
        //ret = es8388_write_reg(0x0B,0x02);//set LIN1 and RIN1 as micphone different input
        ret = es8388_write_reg(0x0A, 0xf8); // Fixed stereo problems
        ret = es8388_write_reg(0x0B, 0x82); // stereo
    }

    tempVal = cfg->data_width;
    tempVal <<= 2;
    tempVal |= cfg->i2s_frame;

    ret = es8388_write_reg(0x0C, tempVal); //ADC I2S Format sel as i2s_frame  , data len as data_width

    ret = es8388_write_reg(0x0d, 0x02);    //ADC LRCK = MCLK/256

    ret = es8388_write_reg(0x10, 0x00);    //ADC VOLUME = 0 DB
    ret = es8388_write_reg(0x11, 0x00);    //set ADC VOLUME as 0 DB

    tempVal = cfg->mic_pga;
    tempVal <<= 4;
    tempVal |= cfg->mic_pga;

    ret = es8388_write_reg(0x09, tempVal); //MIC PGA SEL

    ret = es8388_write_reg(0x12, 0xe2);    //MIC ALC SETTING

    ret = es8388_write_reg(0x13, 0xc0);
    ret = es8388_write_reg(0x14, 0x12);
    ret = es8388_write_reg(0x15, 0x06);
    ret = es8388_write_reg(0x16, 0xc3);

    ret = es8388_write_reg(0x02, 0x55); //startup FSM and DLL
    ret = es8388_write_reg(0x03, 0x09); //power up adc , enable LIN and RIN

    return ret;
}

/******************************************************************************
 * @brief  ES8388 playback mode
 *
 * @param  cfg: None
 *
 * @return None
 *
*******************************************************************************/
static int es8388_playback_mode(struct es8388_cfg *cfg)
{
    uint8_t tempVal = 0;
    int ret = 0;

    ret = es8388_write_reg(0x00, 0x80); //Reset control port register to default
    ret = es8388_write_reg(0x00, 0x06); //Close reset
    bflb_mtimer_delay_ms(10);
    ret = es8388_write_reg(0x02, 0xF3); //Stop STM, DLL and digital block

    if (cfg->role == ES8388_MASTER) {
        ret = es8388_write_reg(0x08, 0x80);
    } else {
        ret = es8388_write_reg(0x08, 0x00);
    }

    ret = es8388_write_reg(0x2B, 0x80); //Set ADC and DAC have the same LRCK
    ret = es8388_write_reg(0x00, 0x05); //Start up reference
    ret = es8388_write_reg(0x01, 0x40); //Start up reference
    bflb_mtimer_delay_ms(30);
    ret = es8388_write_reg(0x03, 0x00); //Power on ADC and LIN/RIN input
    ret = es8388_write_reg(0x04, 0x3F); //Power on DAC and LOUT/ROUT input

    /* Set ADC */
    ret = es8388_write_reg(0x09, 0x77); //MicBoost PGA = +21dB
    ret = es8388_write_reg(0x0A, 0xF0); //Differential input
    ret = es8388_write_reg(0x0B, 0x02); //Select LIN1 and RIN1 as differential input pairs

    tempVal = cfg->data_width;
    tempVal <<= 2;
    tempVal |= cfg->i2s_frame;

    ret = es8388_write_reg(0x0C, tempVal); //ADC I2S Format sel as i2s_frame  , data len as data_width

    ret = es8388_write_reg(0x0D, 0x04);    //MCLK / LRCK = 512
    ret = es8388_write_reg(0x10, 0x00);    //LADC volume = 0dB
    ret = es8388_write_reg(0x11, 0x00);    //RADC volume = 0dB
    ret = es8388_write_reg(0x12, 0xE2);    //ALC enable, PGA Max.Gain = 23.5dB, Min.Gain = 0dB
    ret = es8388_write_reg(0x13, 0xC0);    //ALC target = -4.5dB, ALC hold time = 0 ms
    ret = es8388_write_reg(0x14, 0x12);    //Decay time = 820us, attack time = 416us
    ret = es8388_write_reg(0x15, 0x06);    //ALC mode
    ret = es8388_write_reg(0x16, 0xC3);    //Nose gate =-40.5dB, NGG = 0x01(mute ADC)

    tempVal = cfg->data_width;
    tempVal <<= 2;
    tempVal |= cfg->i2s_frame;
    tempVal <<= 1;

    ret = es8388_write_reg(0x18, 0x04); //MCLK / LRCK = 512
    ret = es8388_write_reg(0x1A, 0x00); //LDAC volume = 0dB
    ret = es8388_write_reg(0x1B, 0x00); //RDAC volume = 0dB
    ret = es8388_write_reg(0x26, 0x00); //Setup mixer
    ret = es8388_write_reg(0x27, 0xB8); //Setup mixer
    ret = es8388_write_reg(0x28, 0x38); //Setup mixer
    ret = es8388_write_reg(0x29, 0x38); //Setup mixer
    ret = es8388_write_reg(0x2A, 0xB8); //Setup mixer
    ret = es8388_write_reg(0x2E, 0x20); //Set Lout/Rout volume:0dB
    ret = es8388_write_reg(0x2F, 0x20); //Set Lout/Rout volume:0dB
    ret = es8388_write_reg(0x30, 0x20); //Set Lout/Rout volume:0dB
    ret = es8388_write_reg(0x31, 0x20); //Set Lout/Rout volume:0dB
    ret = es8388_write_reg(0x02, 0x00); //Power up DEM and STM

    return ret;
}

/******************************************************************************
 * @brief  ES8388 set volume
 *
 * @param  volume
 *
 * @return success or error
 *
*******************************************************************************/
int es8388_set_volume(int volume)
{
    int res;

    if (volume < 0) {
        volume = 0;
    } else if (volume > 100) {
        volume = 100;
    }

    volume /= 3;
    res = es8388_write_reg(0x2e, volume);
    res |= es8388_write_reg(0x2f, volume);
    res |= es8388_write_reg(0x30, volume);
    res |= es8388_write_reg(0x31, volume);
    return res;
}

/******************************************************************************
 * @brief  es8388 reg dump
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void es8388_reg_dump(void)
{
    int i;
    uint8_t tmp;

    for (i = 0; i < 0X3F; i++) {
        if (es8388_read_reg(i, &tmp) != 0) {
            printf("iic read err\r\n");
        }

        printf("Reg[%02d]=0x%02x \n", i, tmp);
    }
}

static void mclk_out_init()
{
    /* output MCLK,
    Will change the clock source of i2s,
    It needs to be called before i2s is initialized
    clock source 25M
    */
    GLB_Set_I2S_CLK(1, 2, 0, 0);
    GLB_Set_Chip_Clock_Out3_Sel(1);
}

// 8388
struct bflb_device_s *i2s0;
struct bflb_device_s *dma0_ch0;
struct bflb_device_s *dma0_ch1;

static void dma0_ch0_isr(void *arg)
{
    printf("tc done\r\n");
}

static void dma0_ch1_isr(void *arg)
{
    printf("rx done\r\n");
}

static void i2s_dma_init(struct audio_buffer *bufs)
{
    static struct bflb_dma_channel_lli_pool_s tx_llipool[100];
    static struct bflb_dma_channel_lli_transfer_s tx_transfers[1];
    static struct bflb_dma_channel_lli_pool_s rx_llipool[100];
    static struct bflb_dma_channel_lli_transfer_s rx_transfers[1];

    struct bflb_i2s_config_s i2s_cfg = {
        .bclk_freq_hz = 16000 * 16 * 2, /* bclk = Sampling_rate * frame_width * channel_num */
        .role = I2S_ROLE_MASTER,
        .format_mode = I2S_MODE_LEFT_JUSTIFIED,
        .channel_mode = I2S_CHANNEL_MODE_NUM_2,
        .frame_width = I2S_SLOT_WIDTH_16,
        .data_width = I2S_SLOT_WIDTH_16,
        .fs_offset_cycle = 0,

        .tx_fifo_threshold = 0,
        .rx_fifo_threshold = 0,
    };

    struct bflb_dma_channel_config_s tx_config = {
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_I2S_TX,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_16BIT,
        .dst_width = DMA_DATA_WIDTH_16BIT,
    };

    struct bflb_dma_channel_config_s rx_config = {
        .direction = DMA_PERIPH_TO_MEMORY,
        .src_req = DMA_REQUEST_I2S_RX,
        .dst_req = DMA_REQUEST_NONE,
        .src_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_16BIT,
        .dst_width = DMA_DATA_WIDTH_16BIT
    };

    printf("i2s init\r\n");
    i2s0 = bflb_device_get_by_name("i2s0");
    /* i2s init */
    bflb_i2s_init(i2s0, &i2s_cfg);
    /* enable dma */
    bflb_i2s_link_txdma(i2s0, true);
    bflb_i2s_link_rxdma(i2s0, true);

    printf("dma init\r\n");
    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");
    dma0_ch1 = bflb_device_get_by_name("dma0_ch1");

    bflb_dma_channel_init(dma0_ch0, &tx_config);
    bflb_dma_channel_init(dma0_ch1, &rx_config);

    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);
    bflb_dma_channel_irq_attach(dma0_ch1, dma0_ch1_isr, NULL);

    tx_transfers[0].src_addr = (uint32_t)bufs->tx_buf;
    tx_transfers[0].dst_addr = (uint32_t)DMA_ADDR_I2S_TDR;
    tx_transfers[0].nbytes = (bufs->tx_len);

    rx_transfers[0].src_addr = (uint32_t)DMA_ADDR_I2S_RDR;
    rx_transfers[0].dst_addr = (uint32_t)bufs->rx_buf;
    rx_transfers[0].nbytes = (bufs->rx_len);

    printf("dma lli init\r\n");
    uint32_t num = bflb_dma_channel_lli_reload(dma0_ch0, tx_llipool, 100, tx_transfers, 1);
    bflb_dma_channel_lli_link_head(dma0_ch0, tx_llipool, num);
    printf("tx dma lli num: %d \r\n", num);
    num = bflb_dma_channel_lli_reload(dma0_ch1, rx_llipool, 100, rx_transfers, 1);
    printf("rx dma lli num: %d \r\n", num);
    bflb_dma_channel_lli_link_head(dma0_ch1, rx_llipool, num);
    bflb_dma_channel_start(dma0_ch0);
    bflb_dma_channel_start(dma0_ch1);
}

void i2s_gpio_init()
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

int es8388_init(void *cfg)
{
    int ret = 0;
    struct audio_cfg *audio_cfg = (struct audio_cfg*)cfg;

    static struct es8388_cfg es8388_cfg = {
        .work_mode = ES8388_CODEC_MDOE,          /*!< ES8388 work mode */
        .role = ES8388_SLAVE,                    /*!< ES8388 role */
        .mic_input_mode = ES8388_DIFF_ENDED_MIC, /*!< ES8388 mic input mode */
        .mic_pga = ES8388_MIC_PGA_9DB,           /*!< ES8388 mic PGA */
        .i2s_frame = ES8388_LEFT_JUSTIFY_FRAME,  /*!< ES8388 I2S frame */
        .data_width = ES8388_DATA_LEN_16,        /*!< ES8388 I2S dataWitdh */
    };

    struct audio_buffer bufs = audio_cfg->buffers;

    /* gpio init */
    i2s_gpio_init();
    /* mclk clkout init */
    mclk_out_init();

    i2c0 = bflb_device_get_by_name("i2c0");
    bflb_i2c_init(i2c0, 200000);

    switch (es8388_cfg.work_mode) {
        case ES8388_CODEC_MDOE:
            ret = es8388_codec_mode(&es8388_cfg);
            break;

        case ES8388_RECORDING_MODE:
            ret = es8388_recording_mode(&es8388_cfg);
            break;

        case ES8388_PLAY_BACK_MODE:
            ret = es8388_playback_mode(&es8388_cfg);
            break;

        default:
            break;
    }

    i2s_dma_init(&bufs);

    return ret;
}

int es8388_play(void *cfg)
{
    /* enable i2s tx and rx */
    bflb_i2s_feature_control(i2s0, I2S_CMD_DATA_ENABLE, I2S_CMD_DATA_ENABLE_TX | I2S_CMD_DATA_ENABLE_RX);
}
