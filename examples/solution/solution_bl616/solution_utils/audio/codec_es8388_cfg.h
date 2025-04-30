#ifndef __ES8388_H__
#define __ES8388_H__

/********************** es8388 ctrl **********************/

/**
 *  @brief ES8388 Role Matser Or Slave
 */
typedef enum {
    ES8388_MASTER, /*!< Master Mode */
    ES8388_SLAVE,  /*!< Slave Mode */
} es8388_role_t;

/**
 *  @brief ES8388 Microphone input type
 */
typedef enum {
    ES8388_SINGLE_ENDED_MIC, /*!< Mic Single Input Mode */
    ES8388_DIFF_ENDED_MIC,   /*!< Mic Different Input Mode */
} es8388_mic_input_t;

/**
 *  @brief ES8388 Microphone pga sel type
 */
typedef enum {
    ES8388_MIC_PGA_0DB,  /*!< Mic PGA as 0db */
    ES8388_MIC_PGA_3DB,  /*!< Mic PGA as 3db */
    ES8388_MIC_PGA_6DB,  /*!< Mic PGA as 6db */
    ES8388_MIC_PGA_9DB,  /*!< Mic PGA as 9db */
    ES8388_MIC_PGA_12DB, /*!< Mic PGA as 12db */
    ES8388_MIC_PGA_15DB, /*!< Mic PGA as 15db */
    ES8388_MIC_PGA_18DB, /*!< Mic PGA as 18db */
    ES8388_MIC_PGA_21DB, /*!< Mic PGA as 21db */
    ES8388_MIC_PGA_24DB, /*!< Mic PGA as 24db */
} es8388_mic_input_pga_t;

/**
 *  @brief ES8388 I2S Frame Type
 */
typedef enum {
    ES8388_STD_I2S_FRAME,       /*!< Standard I2S Frame */
    ES8388_LEFT_JUSTIFY_FRAME,  /*!< Left  Justify Frame */
    ES8388_RIGHT_JUSTIFY_FRAME, /*!< Right Justify Frame */
    ES8388_DSP_FRAME,           /*!< DSP Frame */
} es8388_i2s_frame_t;

/**
 *  @brief ES8388 I2S Data Len Type
 */
typedef enum {
    ES8388_DATA_LEN_24, /*!< I2S Auido Data Len 24 */
    ES8388_DATA_LEN_20, /*!< I2S Auido Data Len 20 */
    ES8388_DATA_LEN_18, /*!< I2S Auido Data Len 18 */
    ES8388_DATA_LEN_16, /*!< I2S Auido Data Len 16 */
    ES8388_DATA_LEN_32, /*!< I2S Auido Data Len 32 */
} es8388_i2s_data_width_t;

/**
 *  @brief es8388_cfg_t
 */
typedef struct
{
    es8388_role_t role;                 /*!< ES8388 role */
    es8388_mic_input_t mic_input_mode;  /*!< ES8388 mic input mode */
    es8388_mic_input_pga_t mic_pga;     /*!< ES8388 mic PGA */
    es8388_i2s_frame_t i2s_frame;       /*!< ES8388 I2S frame */
    es8388_i2s_data_width_t data_width; /*!< ES8388 I2S dataWitdh */
} es8388_cfg_t;

void es8388_init(es8388_cfg_t *cfg);
void es8388_reg_dump(void);
int es8388_set_voice_volume(int volume);

/********************** es8388 ctrl end **********************/

int codec_es8388_cfg(void);

#endif /* __ES8388_H__ */
