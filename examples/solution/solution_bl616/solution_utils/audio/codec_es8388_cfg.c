#include "bflb_gpio.h"
#include "bflb_i2c.h"

#include "board.h"

#include "codec_es8388_cfg.h"

#if (0)
#define ES8388_DBG(a, ...) printf("[ES8388 dbg]:" a, ##__VA_ARGS__)
#else
#define ES8388_DBG(a, ...)
#endif

#if (1)
#define ES8388_INFO(a, ...) printf("[ES8388 info]:" a, ##__VA_ARGS__)
#else
#define ES8388_INFO(a, ...)
#endif

#define ES8388_ERR(a, ...)    printf("[ES8388 err]:" a, ##__VA_ARGS__)

/********************** es8388 ctrl **********************/

#define ES8388_I2C_SLAVE_ADDR 0x10

static struct bflb_device_s *i2c0;
static struct bflb_i2c_msg_s msgs[2];

static void es8388_i2c_init(void)
{
    board_i2c0_gpio_init();

    i2c0 = bflb_device_get_by_name("i2c0");
    bflb_i2c_init(i2c0, 200000);
}

static int es8388_write_reg(uint8_t addr, uint8_t data)
{
    int ret;

    msgs[0].addr = ES8388_I2C_SLAVE_ADDR;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = &addr;
    msgs[0].length = 1;

    msgs[1].flags = 0;
    msgs[1].buffer = &data;
    msgs[1].length = 1;

    ret = bflb_i2c_transfer(i2c0, msgs, 2);

    if (ret < 0) {
        ES8388_ERR("es8388_i2c_write error\r\n");
    }

    return ret;
}

static int es8388_read_reg(uint8_t addr, uint8_t *rdata)
{
    int ret;

    msgs[0].addr = ES8388_I2C_SLAVE_ADDR;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = &addr;
    msgs[0].length = 1;

    msgs[1].flags = I2C_M_READ;
    msgs[1].buffer = rdata;
    msgs[1].length = 1;

    ret = bflb_i2c_transfer(i2c0, msgs, 2);

    if (ret < 0) {
        ES8388_ERR("es8388_i2c_read error\r\n");
    }

    return ret;
}

static void es8388_codec_mode_cfg(es8388_cfg_t *cfg)
{
    uint8_t tempVal = 0;

    es8388_write_reg(0x01, 0x58); //power down whole chip analog
    es8388_write_reg(0x01, 0x50); //power up whole chip analog

    es8388_write_reg(0x02, 0xf3); //power down ADC/DAC and reset ADC/DAC state machine
    es8388_write_reg(0x02, 0xf0); //power on   ADC/DAC

    es8388_write_reg(0x2B, 0x80); //set internal ADC and DAC use sanme LRCK clock

    es8388_write_reg(0x00, 0x36); //ADC clock is same as DAC . DACMCLK is the chip master clock source

    if (cfg->role == ES8388_MASTER) {
        es8388_write_reg(0x08, 0x8D);
    } else {
        es8388_write_reg(0x08, 0x00);
    }

    es8388_write_reg(0x04, 0x00); //power up dac

    es8388_write_reg(0x05, 0x00); //stop lowpower mode
    es8388_write_reg(0x06, 0xc3); //stop lowpower mode

    if (cfg->mic_input_mode == ES8388_SINGLE_ENDED_MIC) {
        /*
         * default set LINPUT1 and RINPUT1 as single ended mic input
         * if user want to use LINPUT2 and RINPUT2 as input
         * please set 0x0a register as 0x50
         */
        es8388_write_reg(0x0A, 0xf8);
        es8388_write_reg(0x0B, 0x88); //analog mono mix to left ADC
    } else {
        /*
         * defualt select LIN1 and RIN1 as Mic diff input
         * if user want to use LIN2 and RIN2 as input
         * please set 0x0b register as 0x82
         */
        // mic2 select
        es8388_write_reg(0x0A, 0xf8); // Fixed stereo problems
        es8388_write_reg(0x0B, 0x82); // stereo
    }

    tempVal = cfg->data_width;
    tempVal <<= 2;
    tempVal |= cfg->i2s_frame;

    es8388_write_reg(0x0C, tempVal); //ADC I2S Format sel as i2s_frame  , data len as data_width

    tempVal = cfg->data_width;
    tempVal <<= 2;
    tempVal |= cfg->i2s_frame;
    tempVal <<= 1;

    es8388_write_reg(0x17, tempVal); //DAC I2S Format sel as i2s_frame  , data len as data_width

    /* when work in master mode , BCLK is devided form MCLK
     * default divider is 256 , see datasheet reigster 13
     */
    if (cfg->role == ES8388_MASTER) {
        es8388_write_reg(0x0d, 0x06); //ADCLRCK = MCLK/256
        es8388_write_reg(0x18, 0x06); //DACLRCK = MCLK/256
    }

    /*set ADC/DAC default volume as 0 db */
    es8388_write_reg(0x10, 0x00); //LADC volume as 0db
    es8388_write_reg(0x11, 0x00); //RADC volume as 0db
    es8388_write_reg(0x1A, 0x00); //LDAC volume as 0db
    es8388_write_reg(0x1B, 0x00); //RDAC volume as 0db

    tempVal = cfg->mic_pga;
    tempVal <<= 4;
    tempVal |= cfg->mic_pga;

    es8388_write_reg(0x09, tempVal); //set ADC PGA as mic pga

    /*ADC ALC default Setting */
    es8388_write_reg(0x12, 0xE2);
    es8388_write_reg(0x13, 0xC0);
    es8388_write_reg(0x14, 0x12);
    es8388_write_reg(0x15, 0x06);
    es8388_write_reg(0x16, 0xC3);

    /*Mixer setting for LDAC to LOUT and RDAC TO ROUT
     * default close mixer
     */
    es8388_write_reg(0x27, 0xB8);
    es8388_write_reg(0x2A, 0xB8);

    es8388_write_reg(0x02, 0x00); //startup FSM and DLL
    bflb_mtimer_delay_ms(500);

    /* set LOUT1 ROUT1 LOUT2 ROUT2 volume */
    es8388_write_reg(0x2e, 0x1E);
    es8388_write_reg(0x2f, 0x1E);
    es8388_write_reg(0x30, 0x1E);
    es8388_write_reg(0x31, 0x1E);

    es8388_write_reg(0x04, 0x3C); //enable LOUT & ROUT
    es8388_write_reg(0x04, 0x24);
    es8388_write_reg(0x04, 0x18);

    es8388_write_reg(0x26, 0x01);
    es8388_write_reg(0x03, 0x09); //power up ADC Enable LIN &RIN.
}

int es8388_set_voice_volume(int volume)
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

void es8388_reg_dump(void)
{
    int i;
    uint8_t tmp;

    ES8388_DBG("reg dump start:\r\n");

    for (i = 0; i < 0X3F; i++) {
        if (es8388_read_reg(i, &tmp) < 0) {
            ES8388_ERR("read[%02d] err\r\n", i);
            break;
        } else {
            ES8388_DBG("Reg[%02d] = 0x%02x\r\n", i, tmp);
        }
    }

    ES8388_DBG("reg dump end.\r\n");
}

void es8388_init(es8388_cfg_t *cfg)
{
    es8388_i2c_init();

    es8388_codec_mode_cfg(cfg);

    es8388_reg_dump();
}

/********************** es8388 ctrl end **********************/

/* es8388 cfg */
int codec_es8388_cfg(void)
{
    ES8388_INFO("codec_es8388_cfg");

    es8388_cfg_t es8388_cfg = {
        .role = ES8388_SLAVE,                    /*!< ES8388 role */
        .mic_input_mode = ES8388_DIFF_ENDED_MIC, /*!< ES8388 mic input mode */
        .mic_pga = ES8388_MIC_PGA_3DB,           /*!< ES8388 mic PGA */
        .i2s_frame = ES8388_LEFT_JUSTIFY_FRAME,  /*!< ES8388 I2S frame */
        .data_width = ES8388_DATA_LEN_16,        /*!< ES8388 I2S dataWitdh */
    };

    es8388_init(&es8388_cfg);

    es8388_set_voice_volume(50);

    return 0;
}
