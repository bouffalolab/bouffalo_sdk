/**
 * @file audio_codec_jy6311.c
 * @brief JY6311 codec driver implementation
 */

#include "audio_codec_jy6311.h"
#include "audio_codec_internal.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "bflb_core.h"
#include "bflb_mtimer.h"

typedef struct {
    struct bflb_device_s *i2c;
    uint8_t i2c_addr;
    audio_codec_cfg_t cfg;
    uint8_t dac_volume_reg;
    uint8_t adc_volume_reg;
    JY6311_ADC_PGAGainETypeDef adc_pga_gain;
    bool dac_muted;
    bool initialized;
} audio_codec_jy6311_ctx_t;

static struct bflb_device_s *g_jy6311_i2c_dev;
static audio_codec_jy6311_ctx_t g_jy6311_ctx;

static int audio_codec_jy6311_map_i2s_fmt(audio_codec_i2s_fmt_t i2s_fmt, JY6311_I2S_FmtETypeDef *out_fmt);
static int audio_codec_jy6311_map_word_len(audio_codec_bits_t bits, JY6311_I2S_WordLenETypeDef *out_word_len);
static int audio_codec_jy6311_map_sample_rate(uint32_t sample_rate_hz, JY6311_UserSmpRateETypeDef *out_rate);

/** @defgroup JY6311_Driver JY6311 Driver
  * @brief    JY6311 Driver for Bare metal and RTOS
  * @{
  */

#ifdef JY6311_MODULE_ENABLED

/* Private Constants ---------------------------------------------------------*/
/* Private Macros ------------------------------------------------------------*/
/** @defgroup JY6311_Private_Macros JY6311 Private Macros
  * @brief    JY6311 Private Macros
  * @{
  */

/**
  * @brief JY6311 DAC EQx Band Coff config macro function definition
  * @param  i2c_addr jy6311 i2c address
  * @param  x DAC EQ Band number, range in [1, 2, 3]
  * @param  array_cfg dac eq coff config array pointer
  */
#define JY6311_DAC_EQx_BAND_COFF_CFG(i2c_addr, x, array_cfg)                          \
    do {                                                                              \
        __JY6311_DAC_EQ##x##_Coff_b0h_Set(i2c_addr, (uint8_t)((array_cfg)[0] >> 16)); \
        __JY6311_DAC_EQ##x##_Coff_b0m_Set(i2c_addr, (uint8_t)((array_cfg)[0] >> 8));  \
        __JY6311_DAC_EQ##x##_Coff_b0l_Set(i2c_addr, (uint8_t)(array_cfg)[0]);         \
        __JY6311_DAC_EQ##x##_Coff_b1h_Set(i2c_addr, (uint8_t)((array_cfg)[1] >> 16)); \
        __JY6311_DAC_EQ##x##_Coff_b1m_Set(i2c_addr, (uint8_t)((array_cfg)[1] >> 8));  \
        __JY6311_DAC_EQ##x##_Coff_b1l_Set(i2c_addr, (uint8_t)(array_cfg)[1]);         \
        __JY6311_DAC_EQ##x##_Coff_b2h_Set(i2c_addr, (uint8_t)((array_cfg)[2] >> 16)); \
        __JY6311_DAC_EQ##x##_Coff_b2m_Set(i2c_addr, (uint8_t)((array_cfg)[2] >> 8));  \
        __JY6311_DAC_EQ##x##_Coff_b2l_Set(i2c_addr, (uint8_t)(array_cfg)[2]);         \
        __JY6311_DAC_EQ##x##_Coff_a1h_Set(i2c_addr, (uint8_t)((array_cfg)[3] >> 16)); \
        __JY6311_DAC_EQ##x##_Coff_a1m_Set(i2c_addr, (uint8_t)((array_cfg)[3] >> 8));  \
        __JY6311_DAC_EQ##x##_Coff_a1l_Set(i2c_addr, (uint8_t)(array_cfg)[3]);         \
        __JY6311_DAC_EQ##x##_Coff_a2h_Set(i2c_addr, (uint8_t)((array_cfg)[4] >> 16)); \
        __JY6311_DAC_EQ##x##_Coff_a2m_Set(i2c_addr, (uint8_t)((array_cfg)[4] >> 8));  \
        __JY6311_DAC_EQ##x##_Coff_a2l_Set(i2c_addr, (uint8_t)(array_cfg)[4]);         \
    } while (0)

/**
  * @}
  */

/* Private Types -------------------------------------------------------------*/
/** @defgroup JY6311_Private_Types JY6311 Private Types
  * @brief    JY6311 Private Types
  * @{
  */

enum {
    JY6311_SMP_OSR_ADC = 0, /*!< smp_osr_pll_out array idx ADC  */
    JY6311_SMP_OSR_DAC,     /*!< smp_osr_pll_out array idx DAC  */
};

/**
  * @brief JY6311 PLL Divider factor struct definition
  * @note  FOUT = FIN * N / (M*P)
  */
struct pll_div {
    uint32_t freq_in;  /*!< PLL input frequence            */
    uint32_t freq_out; /*!< PLL output frequence           */
    uint16_t n;        /*!< PLL feedback-div factor N      */
    uint8_t p;         /*!< PLL pre-div factor P           */
    uint8_t m;         /*!< PLL post-div factor M          */
};

/**
  * @brief JY6311 register value struct definition
  */
struct reg_val {
    uint8_t reg; /*!< JY6311 register address        */
    uint8_t val; /*!< JY6311 register value          */
};

/**
  * @brief JY6311 sample rate to OSR and pll frequence out struct definition
  */
struct smp_osr_pll_out {
    JY6311_UserSmpRateETypeDef smp; /*!< User sample rate in Hz         */
    JY6311_ADDA_OSRETypeDef osr;    /*!< ADC/DAC OSR                    */
    uint32_t pll_out;               /*!< PLL frequence out              */
};

/**
  * @}
  */

/* Private Variables ---------------------------------------------------------*/
/** @defgroup JY6311_Private_Variables JY6311 Private Variables
  * @brief    JY6311 Private Variables
  * @{
  */

/**
  * @brief JY6311 PLL Divider factor array definition
  * @note  FOUT = FIN * N / (M*P), N[1, 256], P[1, 32], M[2, 16] and must be even
  */
static const struct pll_div jy6311_pll_div[] = {
    // 48k Series @6.144M
    { 512000, 6144000, 192, 1, 16 },
    { 768000, 6144000, 128, 1, 16 },
    { 1024000, 6144000, 96, 1, 16 },
    { 1536000, 6144000, 64, 1, 16 },
    { 2048000, 6144000, 96, 2, 16 },
    { 3072000, 6144000, 96, 3, 16 },
    { 4096000, 6144000, 96, 4, 16 },
    { 4608000, 6144000, 64, 3, 16 },
    { 6000000, 6144000, 82, 5, 16 },   //out: 6.15M
    { 12000000, 6144000, 82, 10, 16 }, //out: 6.15M
    { 13000000, 6144000, 68, 9, 16 },  //out: 6.139M
    { 15360000, 6144000, 64, 10, 16 },
    { 16000000, 6144000, 92, 15, 16 }, //out: 6.133M
    { 19200000, 6144000, 82, 16, 16 }, //out: 6.15M
    { 19680000, 6144000, 95, 19, 16 }, //out: 6.15M

    // 44.1k Series @5.6448M
    { 705600, 5644800, 128, 1, 16 },
    { 1058400, 5644800, 96, 1, 18 },
    { 1411200, 5644800, 64, 1, 16 },
    { 1587600, 5644800, 64, 1, 18 },
    { 2116800, 5644800, 96, 2, 18 },
    { 2822400, 5644800, 96, 3, 16 },
    { 3175200, 5644800, 64, 2, 18 },
    { 4233600, 5644800, 64, 3, 16 },
    { 6000000, 5644800, 113, 6, 20 },   //out: 5.65M
    { 12000000, 5644800, 113, 12, 20 }, //out: 5.65M
    { 13000000, 5644800, 113, 13, 20 }, //out: 5.65M
    { 15360000, 5644800, 86, 13, 18 },  //out: 5.6451M
    { 16000000, 5644800, 113, 16, 20 }, //out: 5.65M
    { 19200000, 5644800, 53, 10, 18 },  //out: 5.6533M
    { 19680000, 5644800, 62, 12, 18 },  //out: 5.6489M

    // 48k Series @12.288M
    { 512000, 12288000, 192, 1, 8 },
    { 768000, 12288000, 128, 1, 8 },
    { 1024000, 12288000, 96, 1, 8 },
    { 1536000, 12288000, 64, 1, 8 },
    { 2048000, 12288000, 96, 2, 8 },
    { 3072000, 12288000, 96, 3, 8 },
    { 4096000, 12288000, 96, 4, 8 },
    { 4608000, 12288000, 64, 3, 8 },
    { 6000000, 12288000, 82, 5, 8 },   //out: 12.3M
    { 12000000, 12288000, 82, 10, 8 }, //out: 12.3M
    { 13000000, 12288000, 68, 9, 8 },  //out: 12.278M
    { 15360000, 12288000, 64, 10, 8 },
    { 16000000, 12288000, 92, 15, 8 }, //out: 12.267M
    { 19200000, 12288000, 82, 16, 8 }, //out: 12.3M
    { 19680000, 12288000, 95, 19, 8 }, //out: 12.3M

    // 44.1k Series @11.2896M
    { 705600, 11289600, 128, 1, 8 },
    { 1411200, 11289600, 64, 1, 8 },
    { 2822400, 11289600, 96, 3, 8 },
    { 4233600, 11289600, 64, 3, 8 },
    { 6000000, 11289600, 113, 6, 10 },   //out: 11.3M
    { 12000000, 11289600, 113, 12, 10 }, //out: 11.3M
    { 13000000, 11289600, 113, 13, 10 }, //out: 11.3M
    { 16000000, 11289600, 113, 16, 10 }, //out: 11.3M
};

/**
  * @brief jy6311 all register defalut value array
  */
static const struct reg_val jy6311_reg_defaults[] = {
    // Power and PLL control register
    { SRST, 0x00 },
    { PWR_CTRL1, 0x8c },
    { PWR_CTRL2, 0x48 },
    { PLL_CTRL, 0x48 },
    { PLL_CFG1, 0x00 },
    { PLL_CFG2, 0x01 },
    { PLL_CFG3, 0x00 },

    // Clock Control register
    { SYSCLK_CTRL, 0x00 },
    { MODCLK_EN, 0x00 },
    { ADDA_FS, 0x25 },
    { ADC_CLK_CTRL, 0x20 },
    { DAC_CLK_CTRL, 0x20 },

    // I2S Control register
    { I2S_CTRL, 0x00 },
    { I2S_FMT1, 0x00 },
    { I2S_FMT2, 0x3f },
    { I2S_FMT3, 0x05 },
    { I2S_TX_CTRL, 0x35 },
    { I2S_RX_CTRL, 0x05 },
    { I2S_TXM_CTRL, 0x11 },
    { I2S_RXM_CTRL, 0x01 },
    { I2S_PAD_CTRL, 0x80 },

    // ADC Digital control register
    { ADC_DIG_CTRL, 0x06 },
    { ADC_HPF_COEF, 0x00 },
    { ADC_DVC_CTRL, 0xbf },
    { ADC_SMUTE, 0x10 },

    // ADC EQ Control register
    { ADC_EQ_CTRL, 0x00 },
    { ADC_EQ1_b0_H, 0x00 },
    { ADC_EQ1_b0_M, 0x00 },
    { ADC_EQ1_b0_L, 0x00 },
    { ADC_EQ1_b1_H, 0x00 },
    { ADC_EQ1_b1_M, 0x00 },
    { ADC_EQ1_b1_L, 0x00 },
    { ADC_EQ1_b2_H, 0x00 },
    { ADC_EQ1_b2_M, 0x00 },
    { ADC_EQ1_b2_L, 0x00 },
    { ADC_EQ1_a1_H, 0x00 },
    { ADC_EQ1_a1_M, 0x00 },
    { ADC_EQ1_a1_L, 0x00 },
    { ADC_EQ1_a2_H, 0x00 },
    { ADC_EQ1_a2_M, 0x00 },
    { ADC_EQ1_a2_L, 0x00 },

    // ADC DRC Control register
    { ADC_DRC_CTRL, 0x02 },
    { ADC_PEAK_CTRL, 0x16 },
    { ADC_ATK_TIME, 0x27 },
    { ADC_DRC_THD1, 0x02 },
    { ADC_DRC_THD2, 0x0a },
    { ADC_DRC_THD3, 0x16 },
    { ADC_DRC_THD4, 0x0f },
    { ADC_DRC_SLP1, 0x72 },
    { ADC_DRC_SLP2, 0x70 },

    // DAC Digital control register
    { DAC_DIG_CTRL, 0x06 },
    { DAC_DVC_CTRL, 0xbf },
    { DAC_SMUTE, 0x10 },
    { DAC_DMIX_CTRL, 0x01 },

    // DAC EQ Control register
    { DAC_EQ_CTRL, 0x00 },
    { DAC_EQ1_b0_H, 0x00 },
    { DAC_EQ1_b0_M, 0x00 },
    { DAC_EQ1_b0_L, 0x00 },
    { DAC_EQ1_b1_H, 0x00 },
    { DAC_EQ1_b1_M, 0x00 },
    { DAC_EQ1_b1_L, 0x00 },
    { DAC_EQ1_b2_H, 0x00 },
    { DAC_EQ1_b2_M, 0x00 },
    { DAC_EQ1_b2_L, 0x00 },
    { DAC_EQ1_a1_H, 0x00 },
    { DAC_EQ1_a1_M, 0x00 },
    { DAC_EQ1_a1_L, 0x00 },
    { DAC_EQ1_a2_H, 0x00 },
    { DAC_EQ1_a2_M, 0x00 },
    { DAC_EQ1_a2_L, 0x00 },
    { DAC_EQ2_b0_H, 0x00 },
    { DAC_EQ2_b0_M, 0x00 },
    { DAC_EQ2_b0_L, 0x00 },
    { DAC_EQ2_b1_H, 0x00 },
    { DAC_EQ2_b1_M, 0x00 },
    { DAC_EQ2_b1_L, 0x00 },
    { DAC_EQ2_b2_H, 0x00 },
    { DAC_EQ2_b2_M, 0x00 },
    { DAC_EQ2_b2_L, 0x00 },
    { DAC_EQ2_a1_H, 0x00 },
    { DAC_EQ2_a1_M, 0x00 },
    { DAC_EQ2_a1_L, 0x00 },
    { DAC_EQ2_a2_H, 0x00 },
    { DAC_EQ2_a2_M, 0x00 },
    { DAC_EQ2_a2_L, 0x00 },
    { DAC_EQ3_b0_H, 0x00 },
    { DAC_EQ3_b0_M, 0x00 },
    { DAC_EQ3_b0_L, 0x00 },
    { DAC_EQ3_b1_H, 0x00 },
    { DAC_EQ3_b1_M, 0x00 },
    { DAC_EQ3_b1_L, 0x00 },
    { DAC_EQ3_b2_H, 0x00 },
    { DAC_EQ3_b2_M, 0x00 },
    { DAC_EQ3_b2_L, 0x00 },
    { DAC_EQ3_a1_H, 0x00 },
    { DAC_EQ3_a1_M, 0x00 },
    { DAC_EQ3_a1_L, 0x00 },
    { DAC_EQ3_a2_H, 0x00 },
    { DAC_EQ3_a2_M, 0x00 },
    { DAC_EQ3_a2_L, 0x00 },

    // DAC DRC Control register
    { DAC_DRC_CTRL, 0x02 },
    { DAC_PEAK_CTRL, 0x16 },
    { DAC_ATK_TIME, 0x27 },
    { DAC_DRC_THD1, 0x02 },
    { DAC_DRC_THD2, 0x0a },
    { DAC_DRC_THD3, 0x16 },
    { DAC_DRC_THD4, 0x0f },
    { DAC_DRC_SLP1, 0x72 },
    { DAC_DRC_SLP2, 0x70 },

    // ADC Analog Control register
    { ANA_ADC_CTRL1, 0x00 },
    { ANA_ADC_CTRL2, 0x29 },
    { ANA_ADC_CTRL3, 0xff },
    { ANA_ADC_TUNE1, 0x7e },
    { ANA_ADC_TUNE2, 0x5a },

    // DAC Analog Control register
    { ANA_DAC_CTRL1, 0x20 },
    { ANA_DAC_CTRL2, 0x93 },
    { ANA_DAC_CTRL3, 0x04 },
    { ANA_DAC_TUNE1, 0x50 },
    { ANA_DAC_TUNE2, 0xaa },

    // Debug Register
    { ADDA_DEBUG, 0x80 },
};

/**
  * @brief jy6311 ADC register optimize array
  */
static const struct reg_val jy6311_adc_reg_optimize[2][2] = {
    // Normal
    {
        { ANA_ADC_TUNE1, 0x66 },
        { ANA_ADC_TUNE2, 0x52 },
    },

    // Double Speed
    {
        { ANA_ADC_TUNE1, 0x7f },
        { ANA_ADC_TUNE2, 0x7a },
    },
};

/**
  * @brief jy6311 DAC register optimize array
  */
static const struct reg_val jy6311_dac_reg_optimize[2][1] = {
    // Normal
    {
        { ANA_DAC_TUNE2, 0x0c },
    },

    // Double Speed
    {
        { ANA_DAC_TUNE2, 0xae },
    },
};

/**
  * @brief JY6311 sample rate to OSR and pll frequence out struct variables array definition
  */
static const struct smp_osr_pll_out jy6311_smp_osr_pll_out[][2][13] = {
    // ADC
    {
        // Normal
        {
            { JY6311_USER_SR_8K, JY6311_ADDA_OSR_64, 6144000 },
            { JY6311_USER_SR_12K, JY6311_ADDA_OSR_64, 6144000 },
            { JY6311_USER_SR_16K, JY6311_ADDA_OSR_64, 6144000 },
            { JY6311_USER_SR_24K, JY6311_ADDA_OSR_64, 6144000 },
            { JY6311_USER_SR_32K, JY6311_ADDA_OSR_64, 6144000 },
            { JY6311_USER_SR_48K, JY6311_ADDA_OSR_64, 6144000 },
            { JY6311_USER_SR_96K, JY6311_ADDA_OSR_64, 6144000 },
            { JY6311_USER_SR_192K, JY6311_ADDA_OSR_32, 6144000 },
            { JY6311_USER_SR_11025, JY6311_ADDA_OSR_64, 5644800 },
            { JY6311_USER_SR_22050, JY6311_ADDA_OSR_64, 5644800 },
            { JY6311_USER_SR_44100, JY6311_ADDA_OSR_64, 5644800 },
            { JY6311_USER_SR_88200, JY6311_ADDA_OSR_64, 5644800 },
            { JY6311_USER_SR_176400, JY6311_ADDA_OSR_32, 5644800 },
        },

        // Double Speed
        {
            { JY6311_USER_SR_8K, JY6311_ADDA_OSR_128, 6144000 },
            { JY6311_USER_SR_12K, JY6311_ADDA_OSR_128, 6144000 },
            { JY6311_USER_SR_16K, JY6311_ADDA_OSR_128, 6144000 },
            { JY6311_USER_SR_24K, JY6311_ADDA_OSR_128, 6144000 },
            { JY6311_USER_SR_32K, JY6311_ADDA_OSR_64, 6144000 },
            { JY6311_USER_SR_48K, JY6311_ADDA_OSR_128, 6144000 },
            { JY6311_USER_SR_96K, JY6311_ADDA_OSR_64, 6144000 },
            { JY6311_USER_SR_192K, JY6311_ADDA_OSR_32, 6144000 },
            { JY6311_USER_SR_11025, JY6311_ADDA_OSR_128, 5644800 },
            { JY6311_USER_SR_22050, JY6311_ADDA_OSR_128, 5644800 },
            { JY6311_USER_SR_44100, JY6311_ADDA_OSR_128, 5644800 },
            { JY6311_USER_SR_88200, JY6311_ADDA_OSR_64, 5644800 },
            { JY6311_USER_SR_176400, JY6311_ADDA_OSR_32, 5644800 },
        },
    },

    // DAC
    {
        // Normal
        {
            { JY6311_USER_SR_8K, JY6311_ADDA_OSR_128, 6144000 },
            { JY6311_USER_SR_12K, JY6311_ADDA_OSR_128, 6144000 },
            { JY6311_USER_SR_16K, JY6311_ADDA_OSR_128, 6144000 },
            { JY6311_USER_SR_24K, JY6311_ADDA_OSR_128, 6144000 },
            { JY6311_USER_SR_32K, JY6311_ADDA_OSR_128, 12288000 },
            { JY6311_USER_SR_48K, JY6311_ADDA_OSR_128, 6144000 },
            { JY6311_USER_SR_96K, JY6311_ADDA_OSR_128, 12288000 },
            { JY6311_USER_SR_192K, JY6311_ADDA_OSR_64, 12288000 },
            { JY6311_USER_SR_11025, JY6311_ADDA_OSR_128, 5644800 },
            { JY6311_USER_SR_22050, JY6311_ADDA_OSR_128, 5644800 },
            { JY6311_USER_SR_44100, JY6311_ADDA_OSR_128, 5644800 },
            { JY6311_USER_SR_88200, JY6311_ADDA_OSR_128, 11289600 },
            { JY6311_USER_SR_176400, JY6311_ADDA_OSR_64, 11289600 },
        },

        // Double Speed
        {
            { JY6311_USER_SR_8K, JY6311_ADDA_OSR_256, 6144000 },
            { JY6311_USER_SR_12K, JY6311_ADDA_OSR_256, 6144000 },
            { JY6311_USER_SR_16K, JY6311_ADDA_OSR_256, 12288000 },
            { JY6311_USER_SR_24K, JY6311_ADDA_OSR_256, 6144000 },
            { JY6311_USER_SR_32K, JY6311_ADDA_OSR_128, 12288000 },
            { JY6311_USER_SR_48K, JY6311_ADDA_OSR_256, 12288000 },
            { JY6311_USER_SR_96K, JY6311_ADDA_OSR_128, 12288000 },
            { JY6311_USER_SR_192K, JY6311_ADDA_OSR_64, 12288000 },
            { JY6311_USER_SR_11025, JY6311_ADDA_OSR_256, 5644800 },
            { JY6311_USER_SR_22050, JY6311_ADDA_OSR_256, 5644800 },
            { JY6311_USER_SR_44100, JY6311_ADDA_OSR_256, 11289600 },
            { JY6311_USER_SR_88200, JY6311_ADDA_OSR_128, 11289600 },
            { JY6311_USER_SR_176400, JY6311_ADDA_OSR_64, 11289600 },
        },
    },
};

/**
  * @brief jy6311 adc/dac osr real value array
  */
static const uint16_t jy6311_adda_osr[] = { 32, 64, 128, 256 };

/**
  * @brief jy6311 adc/dac DCLK and I2S BCLK divider real value array
  */
static const uint8_t jy6311_adda_dclk_div[] = { 1, 2, 3, 4, 6, 8, 12, 16, 24 };

/**
  * @}
  */

/* Private Function Prototypes -----------------------------------------------*/
/** @addtogroup JY6311_Private_Functions
  * @{
  */
static uint8_t jy6311_smpRateToRegOrIdx(uint32_t rate, bool reg);
static JY6311_StatusETypeDef jy6311_pllCfg(uint8_t i2c_addr, JY6311_PLLCLKSrcETypeDef clk_src, uint32_t freq_in,
                                           uint32_t freq_out);
static void jy6311_play_start_internal(uint8_t i2c_addr, bool check_wait_rampup);
static void jy6311_play_stop_internal(uint8_t i2c_addr, bool check_wait_rampdown);
/**
  * @}
  */

/* Exported Variables --------------------------------------------------------*/
/* Exported Functions --------------------------------------------------------*/
/** @defgroup JY6311_Exported_Functions JY6311 Exported Functions
  * @brief    JY6311 Exported Functions
  * @{
  */

/** @defgroup JY6311_Exported_Functions_Group1 JY6311 I2C Read/Write Functions
  * @brief    JY6311 I2C Read/Write Functions
  * @{
  */

/**
  * @brief  jy6311 update bits
  * @param  i2c_addr jy6311 i2c address
  * @param  reg JY6311 register address to update
  * @param  mask update mask
  * @param  val update value
  * @retval 0      Register update Success
  * @retval others Register update Failed
  */
int8_t jy6311_update_bits(uint8_t i2c_addr, uint8_t reg, uint8_t mask, uint8_t val)
{
    int8_t ret = 0;
    uint8_t val_old, val_new;

    val_old = JY6311_I2C_READ_BYTE(i2c_addr, reg);
    val_new = (val_old & ~mask) | (val & mask);

    if (val_new != val_old) {
        ret = JY6311_I2C_WRITE_BYTE(i2c_addr, reg, val_new);
    }

    return ret;
}

/**
  * @}
  */

/** @defgroup JY6311_Exported_Functions_Group2 JY6311 Init and DeInit Functions
  * @brief    JY6311 Init and DeInit Functions
  * @{
  */

/**
  * @brief  jy6311 init
  * @param  i2c_addr jy6311 i2c address
  * @param  mod jy6311 init module select
  * @return JY6311_StatusETypeDef
  */
JY6311_StatusETypeDef jy6311_init(uint8_t i2c_addr, JY6311_InitModETypeDef mod)
{
    JY6311_LOG_I("Amplore JY6311 Driver " JY6311_DRV_STAGE_STR " V%d.%d " __DATE__ " " __TIME__ "\n\n",
                 JY6311_DRV_VER_MAIN, JY6311_DRV_VER_SUB);

    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    if (mod == JY6311_INIT_MOD_NONE) {
        JY6311_LOG_D("Init module is None, nothing to do.");
        return JY6311_OK;
    }

    /* ADC Init */
    if (mod & JY6311_INIT_MOD_ADC) {
        // ADC Module Clock Enable
        __JY6311_ADC_ModClkAll_En(i2c_addr);

        // MICBIAS Output Voltage Set and Enable
        __JY6311_MicbiasVol_Set_And_En(i2c_addr, JY6311_MICBIAS_VOL_2V5);

        // HPF Coff Config, general MIC can only ensure a frequency response above 100Hz
        __JY6311_ADC_HPF_Coff_Set(i2c_addr, JY6311_ADC_HPF_COFF_70Hz);

        // ADC Delay Function set to 16ms
        __JY6311_ADC_DelayFunc_Set(i2c_addr, JY6311_ADC_DELAY_FUNC_16ms);

        // Optimize Config
        __JY6311_ADC_SDM_Optimize_Set(i2c_addr, 0x7);
#if 0
        __JY6311_ADC_HPF_Dis(i2c_addr);
        __JY6311_DebugSel_ADC(i2c_addr);
        __JY6311_DebugPat_Set(i2c_addr, JY6311_DBG_PAT_0x1234);
        __JY6311_Debug_En(i2c_addr);
#endif
    }

    /* DAC Init */
    if (mod & JY6311_INIT_MOD_DAC) {
        // DAC Module Clock Enable
        __JY6311_DAC_ModClkAll_En(i2c_addr);

        // DAC Auto Mute Enable
        __JY6311_DAC_AutoMute_En(i2c_addr);

        // DAC Ramp Up Disable
        __JY6311_DAC_OutRampUp_Dis(i2c_addr);

        // DAC Output Source Select DAC
        __JY6311_DAC_OutSrc_DAC_En(i2c_addr);

        // DAC Dithering Amplitude Selection
        __JY6311_DAC_DitherAmp_Set(i2c_addr, JY6311_DAC_DITHER_AMP_1_8LSB);
    }

    /* Common Init */
    __JY6311_DMICDAT_Pad_Dis(i2c_addr);
    __JY6311_I2S_PadDrvLvl_Set(i2c_addr, JY6311_PAD_DRV_LVL_0);

    /* ALDO Power On, and need to delay 10ms for ALDO output stable */
    __JY6311_ALDO_En(i2c_addr);
    JY6311_DELAY_MS_USER(10);

    return JY6311_OK;
}

/**
  * @brief  jy6311 deinit
  * @param  i2c_addr jy6311 i2c address
  * @param  mod jy6311 deinit module select
  * @return JY6311_StatusETypeDef
  */
JY6311_StatusETypeDef jy6311_deinit(uint8_t i2c_addr, JY6311_InitModETypeDef mod)
{
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    if (mod == JY6311_INIT_MOD_NONE) {
        JY6311_LOG_D("DeInit module is None, nothing to do.");
        return JY6311_OK;
    }

    /* ADC DeInit */
    if (mod & JY6311_INIT_MOD_ADC) {
        // MICBIAS Disable
        __JY6311_Micbias_Dis(i2c_addr);

        // ADC Module Clock Disable
        __JY6311_ADC_ModClkAll_Dis(i2c_addr);
    }

    /* DAC DeInit */
    if (mod & JY6311_INIT_MOD_DAC) {
        //DAC Auto Mute Disable
        __JY6311_DAC_AutoMute_Dis(i2c_addr);

        // DAC Module Clock Disable
        __JY6311_DAC_ModClkAll_Dis(i2c_addr);
    }

    /* Common DeInit */
    if (mod == JY6311_INIT_MOD_ADC_DAC) {
        // ALDO Power Off
        __JY6311_ALDO_Dis(i2c_addr);

        // I2S Global Disable
        __JY6311_I2S_Global_Dis(i2c_addr);

        // SYSCLK Disable
        __JY6311_SYSCLK_Dis(i2c_addr);
    }

    return JY6311_OK;
}

/**
  * @}
  */

/** @defgroup JY6311_Exported_Functions_Group3 JY6311 Play and Record Functions
  * @brief    JY6311 Play and Record Functions
  * @{
  */

/**
  * @brief  jy6311 play start
  * @param  i2c_addr jy6311 i2c address
  */
void jy6311_play_start(uint8_t i2c_addr)
{
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // play start internal
    jy6311_play_start_internal(i2c_addr, true);
}

/**
  * @brief  jy6311 play stop
  * @param  i2c_addr jy6311 i2c address
  */
void jy6311_play_stop(uint8_t i2c_addr)
{
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // play stop internal
    jy6311_play_stop_internal(i2c_addr, true);
}

/**
  * @brief  jy6311 record start
  * @param  i2c_addr jy6311 i2c address
  */
void jy6311_record_start(uint8_t i2c_addr)
{
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // ADC Analog Enable
    __JY6311_ADC_SDMConvPGA_En(i2c_addr);

    // ADC Digital Enable
    __JY6311_ADC_Dig_En(i2c_addr);

    // I2S Tx Enable
    __JY6311_I2S_Tx_En(i2c_addr);
}

/**
  * @brief  jy6311 record stop
  * @param  i2c_addr jy6311 i2c address
  */
void jy6311_record_stop(uint8_t i2c_addr)
{
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // I2S Tx Disable
    __JY6311_I2S_Tx_Dis(i2c_addr);

    // ADC Digital Disable
    __JY6311_ADC_Dig_Dis(i2c_addr);

    // ADC Analog Disable
    __JY6311_ADC_SDMConvPGA_Dis(i2c_addr);
}

/**
  * @brief  jy6311 play volume config
  * @param  i2c_addr jy6311 i2c address
  * @param  vol jy6311 DAC digial volume
  * @note   vol range in [0x0, 0xff], which means [-95dB, 32dB], 0.5dB/Step, while 0 means Mute, 1 means -95dB
  */
void jy6311_play_vol_cfg(uint8_t i2c_addr, uint8_t vol)
{
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // DAC Digital volume config
    __JY6311_DAC_DigVol_Set(i2c_addr, vol);
}

/**
  * @brief  jy6311 record gain config
  * @param  i2c_addr jy6311 i2c address
  * @param  gain jy6311 ADC analog gain
  */
void jy6311_record_gain_cfg(uint8_t i2c_addr, JY6311_ADC_PGAGainETypeDef gain)
{
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // ADC PGA gain config
    __JY6311_ADC_PGAGain_Set(i2c_addr, gain);
}

/**
  * @}
  */

/** @defgroup JY6311_Exported_Functions_Group4 JY6311 Common Config Functions
  * @brief    JY6311 Common Config Functions
  * @{
  */

/**
  * @brief  jy6311 i2s config
  * @param  i2c_addr jy6311 i2c address
  * @param  cfg jy6311 i2s config struct pointer
  * @return JY6311_StatusETypeDef
  */
JY6311_StatusETypeDef jy6311_i2s_cfg(uint8_t i2c_addr, JY6311_I2SCfgTypeDef *cfg)
{
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check cfg pointer to be valid
    if (cfg == NULL) {
        JY6311_LOG_E("I2S config pointer is NULL!\n");
        return JY6311_INVALID;
    }

    // I2S Global Disable first before config
    __JY6311_I2S_Global_Dis(i2c_addr);

    // I2S Config
    __JY6311_I2S_Role_Set(i2c_addr, cfg->role);
    __JY6311_I2S_FMT3_Reg_Cfg(i2c_addr, cfg->bclk_invert, cfg->lrck_invert, cfg->fmt, cfg->word_len, false, false);

    // I2S Global Enable
    __JY6311_I2S_Global_En(i2c_addr);

    return JY6311_OK;
}

/**
  * @brief  jy6311 sysclk config
  * @param  i2c_addr jy6311 i2c address
  * @param  cfg jy6311 sysclk config struct pointer
  * @return JY6311_StatusETypeDef
  */
JY6311_StatusETypeDef jy6311_sysclk_cfg(uint8_t i2c_addr, JY6311_SysclkCfgTypeDef *cfg)
{
#define JY6311_DCLK_DIV_INVALID (0xff)
    uint32_t adc_multi_times, dac_multi_times;
    uint32_t sysclk_freq, i2s_lrck_freq;
    uint16_t adc_dclk_div_real, dac_dclk_div_real;
    uint8_t i, adc_smp_idx = 0, dac_smp_idx = 0, adc_osr_reg = 0, dac_osr_reg = 0;
    uint8_t i2s_bclk_div_real, adc_dclk_div_reg = JY6311_DCLK_DIV_INVALID, dac_dclk_div_reg = JY6311_DCLK_DIV_INVALID;

    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check cfg pointer to be valid
    if (cfg == NULL) {
        JY6311_LOG_E("SYSCLK config pointer is NULL!\n");
        return JY6311_INVALID;
    }

    // Check play and record sample rate to be valid
    if (cfg->play_smp_rate == JY6311_USER_SR_NONE && cfg->rec_smp_rate == JY6311_USER_SR_NONE) {
        JY6311_LOG_E("Both play_smp_rate[%d] and rec_smp_rate[%d] are config to JY6311_USER_SR_NONE!\n",
                     cfg->play_smp_rate, cfg->rec_smp_rate);
        return JY6311_INVALID;
    }

    // Check play and record sample rate to be the same 48K/44.1K series
    if (cfg->play_smp_rate != JY6311_USER_SR_NONE && cfg->rec_smp_rate != JY6311_USER_SR_NONE) {
        if (!(cfg->play_smp_rate % 1000) ^ !(cfg->rec_smp_rate % 1000)) {
            JY6311_LOG_E("play_smp_rate[%d] and rec_smp_rate[%d] isn't the same 48K/44.1K series!\n",
                         cfg->play_smp_rate, cfg->rec_smp_rate);
            return JY6311_INVALID;
        }
    }

    // SYSCLK Disable first before config
    __JY6311_SYSCLK_Dis(i2c_addr);

    // Calc DAC DCLK Div without PLL if possible
    if (cfg->play_smp_rate != JY6311_USER_SR_NONE) {
        // Decide DAC OSR
        dac_smp_idx = jy6311_smpRateToRegOrIdx(cfg->play_smp_rate, false);
        dac_osr_reg = jy6311_smp_osr_pll_out[JY6311_SMP_OSR_DAC][cfg->work_mode][dac_smp_idx].osr;

        if (!(cfg->extclk_freq % cfg->play_smp_rate)) {
            dac_multi_times = cfg->extclk_freq / cfg->play_smp_rate;

            if (!(dac_multi_times % jy6311_adda_osr[dac_osr_reg])) {
                dac_dclk_div_real = dac_multi_times / jy6311_adda_osr[dac_osr_reg];

                for (i = 0; i < JY6311_ARRAY_SIZE(jy6311_adda_dclk_div); i++) {
                    if (jy6311_adda_dclk_div[i] == dac_dclk_div_real) {
                        dac_dclk_div_reg = i;
                        break;
                    }
                }
            }
        }
    }

    // Calc ADC DCLK Div without PLL if possible
    if (cfg->rec_smp_rate != JY6311_USER_SR_NONE) {
        // Decide ADC OSR
        adc_smp_idx = jy6311_smpRateToRegOrIdx(cfg->rec_smp_rate, false);
        adc_osr_reg = jy6311_smp_osr_pll_out[JY6311_SMP_OSR_ADC][cfg->work_mode][adc_smp_idx].osr;

        if (!(cfg->extclk_freq % cfg->rec_smp_rate)) {
            adc_multi_times = cfg->extclk_freq / cfg->rec_smp_rate;

            if (!(adc_multi_times % jy6311_adda_osr[adc_osr_reg])) {
                adc_dclk_div_real = adc_multi_times / jy6311_adda_osr[adc_osr_reg];

                for (i = 0; i < JY6311_ARRAY_SIZE(jy6311_adda_dclk_div); i++) {
                    if (jy6311_adda_dclk_div[i] == adc_dclk_div_real) {
                        adc_dclk_div_reg = i;
                        break;
                    }
                }
            }
        }
    }

    if (cfg->rec_smp_rate == JY6311_USER_SR_NONE) {        /* Only use DAC */
        if (dac_dclk_div_reg != JY6311_DCLK_DIV_INVALID) { // Don't need to use PLL
            // SYSCLK Source select MCLK/BCLK
            sysclk_freq = cfg->extclk_freq;
            __JY6311_SYSCLK_Src_Set(i2c_addr, cfg->extclk_src);
            JY6311_LOG_D("SYSCLK Source select [%s] @%" PRIu32 "Hz.\n",
                         cfg->extclk_src == JY6311_EXT_CLK_SRC_MCLK ? "MCLK" : "BCLK", cfg->extclk_freq);
        } else { // Need to use PLL
            // Get PLL out frequence
            sysclk_freq = jy6311_smp_osr_pll_out[JY6311_SMP_OSR_DAC][cfg->work_mode][dac_smp_idx].pll_out;

            // Calc DAC DCLK Div
            dac_dclk_div_real = sysclk_freq / (cfg->play_smp_rate * jy6311_adda_osr[dac_osr_reg]);

            for (i = 0; i < JY6311_ARRAY_SIZE(jy6311_adda_dclk_div); i++) {
                if (jy6311_adda_dclk_div[i] == dac_dclk_div_real) {
                    dac_dclk_div_reg = i;
                    break;
                }
            }

            if (i == JY6311_ARRAY_SIZE(jy6311_adda_dclk_div)) {
                JY6311_LOG_E("DAC DCLK divide don't support [%d]!\n", dac_dclk_div_real);
                return JY6311_ERROR;
            }

            // PLL Config
            if (jy6311_pllCfg(i2c_addr, (JY6311_PLLCLKSrcETypeDef)cfg->extclk_src, cfg->extclk_freq, sysclk_freq) !=
                JY6311_OK) {
                JY6311_LOG_E("PLL config error!\n");
                return JY6311_ERROR;
            }

            // SYSCLK Source select PLL
            __JY6311_SYSCLK_Src_Set(i2c_addr, JY6311_SYSCLK_SRC_PLL);
            JY6311_LOG_D("SYSCLK Source select [PLL] @%" PRIu32 "Hz.\n", sysclk_freq);
        }

        // DAC SR/OSR/DCLK_DIV Config
        __JY6311_DAC_SampleRate_Set(i2c_addr, jy6311_smpRateToRegOrIdx(cfg->play_smp_rate, true));
        __JY6311_DAC_OsrClkDiv_Set(i2c_addr, dac_osr_reg, dac_dclk_div_reg);
        i2s_lrck_freq = cfg->play_smp_rate;

        // DAC optimize config
        for (i = 0; i < JY6311_ARRAY_SIZE(jy6311_dac_reg_optimize[cfg->work_mode]); i++) {
            JY6311_WRITE_REG(i2c_addr, jy6311_dac_reg_optimize[cfg->work_mode][i].reg,
                             jy6311_dac_reg_optimize[cfg->work_mode][i].val);
        }
    } else if (cfg->play_smp_rate == JY6311_USER_SR_NONE) { /* Only use ADC */
        if (adc_dclk_div_reg != JY6311_DCLK_DIV_INVALID) {  // Don't need to use PLL
            // SYSCLK Source select MCLK/BCLK
            sysclk_freq = cfg->extclk_freq;
            __JY6311_SYSCLK_Src_Set(i2c_addr, cfg->extclk_src);
            JY6311_LOG_D("SYSCLK Source select [%s] @%" PRIu32 "Hz.\n",
                         cfg->extclk_src == JY6311_EXT_CLK_SRC_MCLK ? "MCLK" : "BCLK", cfg->extclk_freq);
        } else { // Need to use PLL
            // Get PLL out frequence
            sysclk_freq = jy6311_smp_osr_pll_out[JY6311_SMP_OSR_ADC][cfg->work_mode][adc_smp_idx].pll_out;

            // Calc ADC DCLK Div
            adc_dclk_div_real = sysclk_freq / (cfg->rec_smp_rate * jy6311_adda_osr[adc_osr_reg]);

            for (i = 0; i < JY6311_ARRAY_SIZE(jy6311_adda_dclk_div); i++) {
                if (jy6311_adda_dclk_div[i] == adc_dclk_div_real) {
                    adc_dclk_div_reg = i;
                    break;
                }
            }

            if (i == JY6311_ARRAY_SIZE(jy6311_adda_dclk_div)) {
                JY6311_LOG_E("ADC DCLK divide don't support [%d]!\n", adc_dclk_div_real);
                return JY6311_ERROR;
            }

            // PLL Config
            if (jy6311_pllCfg(i2c_addr, (JY6311_PLLCLKSrcETypeDef)cfg->extclk_src, cfg->extclk_freq, sysclk_freq) !=
                JY6311_OK) {
                JY6311_LOG_E("PLL config error!\n");
                return JY6311_ERROR;
            }

            // SYSCLK Source select PLL
            __JY6311_SYSCLK_Src_Set(i2c_addr, JY6311_SYSCLK_SRC_PLL);
            JY6311_LOG_D("SYSCLK Source select [PLL] @%" PRIu32 "Hz.\n", sysclk_freq);
        }

        // ADC SR/OSR/DCLK_DIV Config
        __JY6311_ADC_SampleRate_Set(i2c_addr, jy6311_smpRateToRegOrIdx(cfg->rec_smp_rate, true));
        __JY6311_ADC_OsrClkDiv_Set(i2c_addr, adc_osr_reg, adc_dclk_div_reg);
        i2s_lrck_freq = cfg->rec_smp_rate;

        // ADC optimize config
        for (i = 0; i < JY6311_ARRAY_SIZE(jy6311_adc_reg_optimize[cfg->work_mode]); i++) {
            JY6311_WRITE_REG(i2c_addr, jy6311_adc_reg_optimize[cfg->work_mode][i].reg,
                             jy6311_adc_reg_optimize[cfg->work_mode][i].val);
        }
    } else { /* Both use ADC/DAC */
        if (dac_dclk_div_reg != JY6311_DCLK_DIV_INVALID &&
            adc_dclk_div_reg != JY6311_DCLK_DIV_INVALID) { // Don't need to use PLL
            // SYSCLK Source select MCLK/BCLK
            sysclk_freq = cfg->extclk_freq;
            __JY6311_SYSCLK_Src_Set(i2c_addr, cfg->extclk_src);
            JY6311_LOG_D("SYSCLK Source select [%s] @%" PRIu32 "Hz.\n",
                         cfg->extclk_src == JY6311_EXT_CLK_SRC_MCLK ? "MCLK" : "BCLK", cfg->extclk_freq);
        } else { // Need to use PLL
            // Get PLL out frequence
            sysclk_freq = JY6311_MAX(jy6311_smp_osr_pll_out[JY6311_SMP_OSR_DAC][cfg->work_mode][dac_smp_idx].pll_out,
                                     jy6311_smp_osr_pll_out[JY6311_SMP_OSR_ADC][cfg->work_mode][adc_smp_idx].pll_out);

            // Calc DAC DCLK Div
            dac_dclk_div_real = sysclk_freq / (cfg->play_smp_rate * jy6311_adda_osr[dac_osr_reg]);

            for (i = 0; i < JY6311_ARRAY_SIZE(jy6311_adda_dclk_div); i++) {
                if (jy6311_adda_dclk_div[i] == dac_dclk_div_real) {
                    dac_dclk_div_reg = i;
                    break;
                }
            }

            if (i == JY6311_ARRAY_SIZE(jy6311_adda_dclk_div)) {
                JY6311_LOG_E("DAC DCLK divide don't support [%d]!\n", dac_dclk_div_real);
                return JY6311_ERROR;
            }

            // Calc ADC DCLK Div
            adc_dclk_div_real = sysclk_freq / (cfg->rec_smp_rate * jy6311_adda_osr[adc_osr_reg]);

            for (i = 0; i < JY6311_ARRAY_SIZE(jy6311_adda_dclk_div); i++) {
                if (jy6311_adda_dclk_div[i] == adc_dclk_div_real) {
                    adc_dclk_div_reg = i;
                    break;
                }
            }

            if (i == JY6311_ARRAY_SIZE(jy6311_adda_dclk_div)) {
                JY6311_LOG_E("ADC DCLK divide don't support [%d]!\n", adc_dclk_div_real);
                return JY6311_ERROR;
            }

            // PLL Config
            if (jy6311_pllCfg(i2c_addr, (JY6311_PLLCLKSrcETypeDef)cfg->extclk_src, cfg->extclk_freq, sysclk_freq) !=
                JY6311_OK) {
                JY6311_LOG_E("PLL config error!\n");
                return JY6311_ERROR;
            }

            // SYSCLK Source select PLL
            __JY6311_SYSCLK_Src_Set(i2c_addr, JY6311_SYSCLK_SRC_PLL);
            JY6311_LOG_D("SYSCLK Source select [PLL] @%" PRIu32 "Hz.\n", sysclk_freq);
        }

        // DAC SR/OSR/DCLK_DIV Config
        __JY6311_DAC_SampleRate_Set(i2c_addr, jy6311_smpRateToRegOrIdx(cfg->play_smp_rate, true));
        __JY6311_DAC_OsrClkDiv_Set(i2c_addr, dac_osr_reg, dac_dclk_div_reg);

        // ADC SR/OSR/DCLK_DIV Config
        __JY6311_ADC_SampleRate_Set(i2c_addr, jy6311_smpRateToRegOrIdx(cfg->rec_smp_rate, true));
        __JY6311_ADC_OsrClkDiv_Set(i2c_addr, adc_osr_reg, adc_dclk_div_reg);

        i2s_lrck_freq = JY6311_MAX(cfg->play_smp_rate, cfg->rec_smp_rate);

        // DAC optimize config
        for (i = 0; i < JY6311_ARRAY_SIZE(jy6311_dac_reg_optimize[cfg->work_mode]); i++) {
            JY6311_WRITE_REG(i2c_addr, jy6311_dac_reg_optimize[cfg->work_mode][i].reg,
                             jy6311_dac_reg_optimize[cfg->work_mode][i].val);
        }

        // ADC optimize config
        for (i = 0; i < JY6311_ARRAY_SIZE(jy6311_adc_reg_optimize[cfg->work_mode]); i++) {
            JY6311_WRITE_REG(i2c_addr, jy6311_adc_reg_optimize[cfg->work_mode][i].reg,
                             jy6311_adc_reg_optimize[cfg->work_mode][i].val);
        }
    }

    // I2S BCLK Divide config in Master mode if need
    if (cfg->i2s_lrck_period) {
        if (sysclk_freq % (i2s_lrck_freq * cfg->i2s_lrck_period)) {
            JY6311_LOG_W("I2S BCLK can't divide for SYSCLK in integer multiples, please check it again!\n");
        }

        // Calc I2S BCLK Div and set
        i2s_bclk_div_real = sysclk_freq / (i2s_lrck_freq * cfg->i2s_lrck_period);

        for (i = 0; i < JY6311_ARRAY_SIZE(jy6311_adda_dclk_div); i++) {
            if (jy6311_adda_dclk_div[i] == i2s_bclk_div_real) {
                __JY6311_I2S_BCLK_Div_Set(i2c_addr, i);
                JY6311_LOG_W("I2S BCLK divide set to [%d].\n", i2s_bclk_div_real);
                break;
            }
        }

        if (i == JY6311_ARRAY_SIZE(jy6311_adda_dclk_div)) {
            JY6311_LOG_W("I2S BCLK divide don't support [%d], set failed!\n", i2s_bclk_div_real);
        }

        // LRCK Div set
        __JY6311_I2S_LRCK_Div_Set(i2c_addr, cfg->i2s_lrck_period);
    }

    // SYSCLK Enable
    __JY6311_SYSCLK_En(i2c_addr);

    return JY6311_OK;
}

/**
  * @brief  jy6311 play path config
  * @param  i2c_addr jy6311 i2c address
  * @param  path DAC output path
  */
void jy6311_play_path_cfg(uint8_t i2c_addr, JY6311_DAC_OutPathETypeDef path)
{
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // DAC Ramp Up Enable and Output Mode Config
    if (path == JY6311_DAC_OUT_PATH_HEADPHONE) {
        __JY6311_DAC_OutRampUp_En(i2c_addr);
        __JY6311_DAC_OutMode_Set(i2c_addr, JY6311_DAC_OUT_MODE_SINGLE);
    } else {
        __JY6311_DAC_OutRampUp_Dis(i2c_addr);
        __JY6311_DAC_OutMode_Set(i2c_addr, JY6311_DAC_OUT_MODE_DIFFERENTIAL);
    }
}

/**
  * @brief  jy6311 pdm interface config
  * @param  i2c_addr jy6311 i2c address
  * @param  en pdm enable config
  * @param  cfg jy6311 pdm config struct pointer
  * @return JY6311_StatusETypeDef
  */
JY6311_StatusETypeDef jy6311_pdm_cfg(uint8_t i2c_addr, bool en, JY6311_PDMCfgTypeDef *cfg)
{
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check cfg pointer to be valid
    if (en && cfg == NULL) {
        JY6311_LOG_E("PDM config pointer is NULL!\n");
        return JY6311_INVALID;
    }

    if (en) {
        // PDM Interface Enable config
        JY6311_FUNC_ALTER(cfg->timing_invert, __JY6311_PDM_Timing_Invert(i2c_addr),
                          __JY6311_PDM_Timing_Normal(i2c_addr));

        if (cfg->clk_io == JY6311_PDMCLK_IO_MCLK) {
            __JY6311_MCLK_IO_PinFunc_Set(i2c_addr, JY6311_MCLK_IO_PIN_FUNC_OUTPUT_PDMCLK);
        } else if (cfg->clk_io == JY6311_PDMCLK_IO_DEV_ID) {
            __JY6311_PDMIO_PinFunc_OutPDMCLK(i2c_addr);
        } else {
            JY6311_LOG_E("PDMCLK IO config error: %d\n", cfg->clk_io);
            return JY6311_INVALID;
        }

        __JY6311_DMICDAT_Pad_En(i2c_addr);
        __JY6311_PDM_En(i2c_addr);
    } else {
        // PDM Interface Disable config
        __JY6311_MCLK_IO_PinFunc_Set(i2c_addr, JY6311_MCLK_IO_PIN_FUNC_INPUT);
        __JY6311_PDMIO_PinFunc_InDevID(i2c_addr);
        __JY6311_DMICDAT_Pad_Dis(i2c_addr);
        __JY6311_PDM_Dis(i2c_addr);
    }

    return JY6311_OK;
}

/**
  * @}
  */

/** @defgroup JY6311_Exported_Functions_Group5 JY6311 EQ and DRC Config Functions
  * @brief    JY6311 EQ and DRC Config Functions
  * @{
  */

/**
  * @brief  jy6311 adc eq config
  * @param  i2c_addr jy6311 i2c address
  * @param  en adc eq enable config
  * @param  array_cfg[] adc eq coff config array pointer
  * @param  array_size adc eq coff config array size in int32_t unit
  * @return JY6311_StatusETypeDef
  */
JY6311_StatusETypeDef jy6311_adc_eq_cfg(uint8_t i2c_addr, bool en, const int32_t array_cfg[], uint8_t array_size)
{
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    if (!en) {
        // ADC EQ Band 1 Disable
        __JY6311_ADC_EQ1_Dis(i2c_addr);
        return JY6311_OK;
    }

    // Check array cfg pointer and array size to be valid
    if (array_cfg == NULL || array_size != JY6311_EQ_BAND_COFF_NUMS) {
        JY6311_LOG_E("ADC EQ config array pointer is NULL or array size isn't equal to %d!\n",
                     JY6311_EQ_BAND_COFF_NUMS);
        return JY6311_INVALID;
    }

    // ADC DSP Clock Enable
    __JY6311_ADC_DSP_Clk_En(i2c_addr);

    // ADC EQ Band 1 Disable first before config
    __JY6311_ADC_EQ1_Dis(i2c_addr);

    // b0 coff config
    __JY6311_ADC_EQ1_Coff_b0h_Set(i2c_addr, (uint8_t)(array_cfg[0] >> 16));
    __JY6311_ADC_EQ1_Coff_b0m_Set(i2c_addr, (uint8_t)(array_cfg[0] >> 8));
    __JY6311_ADC_EQ1_Coff_b0l_Set(i2c_addr, (uint8_t)array_cfg[0]);

    // b1 coff config
    __JY6311_ADC_EQ1_Coff_b1h_Set(i2c_addr, (uint8_t)(array_cfg[1] >> 16));
    __JY6311_ADC_EQ1_Coff_b1m_Set(i2c_addr, (uint8_t)(array_cfg[1] >> 8));
    __JY6311_ADC_EQ1_Coff_b1l_Set(i2c_addr, (uint8_t)array_cfg[1]);

    // b2 coff config
    __JY6311_ADC_EQ1_Coff_b2h_Set(i2c_addr, (uint8_t)(array_cfg[2] >> 16));
    __JY6311_ADC_EQ1_Coff_b2m_Set(i2c_addr, (uint8_t)(array_cfg[2] >> 8));
    __JY6311_ADC_EQ1_Coff_b2l_Set(i2c_addr, (uint8_t)array_cfg[2]);

    // a1 coff config
    __JY6311_ADC_EQ1_Coff_a1h_Set(i2c_addr, (uint8_t)(array_cfg[3] >> 16));
    __JY6311_ADC_EQ1_Coff_a1m_Set(i2c_addr, (uint8_t)(array_cfg[3] >> 8));
    __JY6311_ADC_EQ1_Coff_a1l_Set(i2c_addr, (uint8_t)array_cfg[3]);

    // a2 coff config
    __JY6311_ADC_EQ1_Coff_a2h_Set(i2c_addr, (uint8_t)(array_cfg[4] >> 16));
    __JY6311_ADC_EQ1_Coff_a2m_Set(i2c_addr, (uint8_t)(array_cfg[4] >> 8));
    __JY6311_ADC_EQ1_Coff_a2l_Set(i2c_addr, (uint8_t)array_cfg[4]);

    // ADC EQ Band 1 Enable
    __JY6311_ADC_EQ1_En(i2c_addr);

    return JY6311_OK;
}

/**
  * @brief  jy6311 adc drc config
  * @param  i2c_addr jy6311 i2c address
  * @param  en adc drc enable config
  * @param  array_cfg[] adc drc coff config array pointer
  * @param  array_size adc drc coff config array size in int32_t unit
  * @return JY6311_StatusETypeDef
  */
JY6311_StatusETypeDef jy6311_adc_drc_cfg(uint8_t i2c_addr, bool en, const int32_t array_cfg[], uint8_t array_size)
{
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    if (!en) {
        // ADC DRC Disable
        __JY6311_ADC_DRC_Dis(i2c_addr);
        return JY6311_OK;
    }

    // Check array cfg pointer and array size to be valid
    if (array_cfg == NULL || array_size != JY6311_DRC_COFF_NUMS) {
        JY6311_LOG_E("ADC DRC config array pointer is NULL or array size isn't equal to %d!\n", JY6311_DRC_COFF_NUMS);
        return JY6311_INVALID;
    }

    // ADC DSP Clock Enable
    __JY6311_ADC_DSP_Clk_En(i2c_addr);

    // ADC DRC Disable first before config
    __JY6311_ADC_DRC_Dis(i2c_addr);

    // ADC DRC Coff config
    JY6311_WRITE_REG(i2c_addr, ADC_PEAK_CTRL, (uint8_t)array_cfg[0]);
    JY6311_WRITE_REG(i2c_addr, ADC_ATK_TIME, (uint8_t)array_cfg[1]);
    JY6311_WRITE_REG(i2c_addr, ADC_DRC_THD1, (uint8_t)array_cfg[2]);
    JY6311_WRITE_REG(i2c_addr, ADC_DRC_THD2, (uint8_t)array_cfg[3]);
    JY6311_WRITE_REG(i2c_addr, ADC_DRC_THD3, (uint8_t)array_cfg[4]);
    JY6311_WRITE_REG(i2c_addr, ADC_DRC_THD4, (uint8_t)array_cfg[5]);
    JY6311_WRITE_REG(i2c_addr, ADC_DRC_SLP1, (uint8_t)array_cfg[6]);
    JY6311_WRITE_REG(i2c_addr, ADC_DRC_SLP2, (uint8_t)array_cfg[7]);

    // ADC DRC Enable
    __JY6311_ADC_DRC_En(i2c_addr);

    return JY6311_OK;
}

/**
  * @brief  jy6311 dac eq config
  * @param  i2c_addr jy6311 i2c address
  * @param  en dac eq enable config
  * @param  array_cfg[] dac eq coff config array pointer
  * @param  array_size dac eq coff config array size in int32_t unit
  * @return JY6311_StatusETypeDef
  */
JY6311_StatusETypeDef jy6311_dac_eq_cfg(uint8_t i2c_addr, bool en, const int32_t array_cfg[], uint8_t array_size)
{
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    if (!en) {
        // DAC EQ Band 1/2/3 all Disable
        __JY6311_DAC_EQ_AllBand_Dis(i2c_addr);
        return JY6311_OK;
    }

    // Check array cfg pointer and array size to be valid
    if (array_cfg == NULL || !array_size || (array_size % JY6311_EQ_BAND_COFF_NUMS) ||
        array_size > JY6311_EQ_BAND_COFF_NUMS * JY6311_DAC_EQ_BAND_NUMS) {
        JY6311_LOG_E("DAC EQ config array pointer is NULL or array size is invalid!\n");
        return JY6311_INVALID;
    }

    // DAC DSP Clock Enable
    __JY6311_DAC_DSP_Clk_En(i2c_addr);

    // DAC EQ Band 1/2/3 all Disable first before config
    __JY6311_DAC_EQ_AllBand_Dis(i2c_addr);

    // DAC EQ Band 1 Enable and coff config
    if (array_size >= JY6311_EQ_BAND_COFF_NUMS) {
        JY6311_DAC_EQx_BAND_COFF_CFG(i2c_addr, 1, array_cfg);
        __JY6311_DAC_EQ1_En(i2c_addr);
    }

    // DAC EQ Band 2 Enable and coff config
    if (array_size >= JY6311_EQ_BAND_COFF_NUMS * 2) {
        JY6311_DAC_EQx_BAND_COFF_CFG(i2c_addr, 2, array_cfg + JY6311_EQ_BAND_COFF_NUMS * 1);
        __JY6311_DAC_EQ2_En(i2c_addr);
    }

    // DAC EQ Band 3 Enable and coff config
    if (array_size >= JY6311_EQ_BAND_COFF_NUMS * 3) {
        JY6311_DAC_EQx_BAND_COFF_CFG(i2c_addr, 3, array_cfg + JY6311_EQ_BAND_COFF_NUMS * 2);
        __JY6311_DAC_EQ3_En(i2c_addr);
    }

    return JY6311_OK;
}

/**
  * @brief  jy6311 dac drc config
  * @param  i2c_addr jy6311 i2c address
  * @param  en dac drc enable config
  * @param  array_cfg[] dac drc coff config array pointer
  * @param  array_size dac drc coff config array size in int32_t unit
  * @return JY6311_StatusETypeDef
  */
JY6311_StatusETypeDef jy6311_dac_drc_cfg(uint8_t i2c_addr, bool en, const int32_t array_cfg[], uint8_t array_size)
{
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    if (!en) {
        // DAC DRC Disable
        __JY6311_DAC_DRC_Dis(i2c_addr);
        return JY6311_OK;
    }

    // Check array cfg pointer and array size to be valid
    if (array_cfg == NULL || array_size != JY6311_DRC_COFF_NUMS) {
        JY6311_LOG_E("DAC DRC config array pointer is NULL or array size isn't equal to %d!\n", JY6311_DRC_COFF_NUMS);
        return JY6311_INVALID;
    }

    // DAC DSP Clock Enable
    __JY6311_DAC_DSP_Clk_En(i2c_addr);

    // DAC DRC Disable first before config
    __JY6311_DAC_DRC_Dis(i2c_addr);

    // DAC DRC Coff config
    JY6311_WRITE_REG(i2c_addr, DAC_PEAK_CTRL, (uint8_t)array_cfg[0]);
    JY6311_WRITE_REG(i2c_addr, DAC_ATK_TIME, (uint8_t)array_cfg[1]);
    JY6311_WRITE_REG(i2c_addr, DAC_DRC_THD1, (uint8_t)array_cfg[2]);
    JY6311_WRITE_REG(i2c_addr, DAC_DRC_THD2, (uint8_t)array_cfg[3]);
    JY6311_WRITE_REG(i2c_addr, DAC_DRC_THD3, (uint8_t)array_cfg[4]);
    JY6311_WRITE_REG(i2c_addr, DAC_DRC_THD4, (uint8_t)array_cfg[5]);
    JY6311_WRITE_REG(i2c_addr, DAC_DRC_SLP1, (uint8_t)array_cfg[6]);
    JY6311_WRITE_REG(i2c_addr, DAC_DRC_SLP2, (uint8_t)array_cfg[7]);

    // DAC DRC Enable
    __JY6311_DAC_DRC_En(i2c_addr);

    return JY6311_OK;
}

/**
  * @}
  */

/** @defgroup JY6311_Exported_Functions_Group6 JY6311 Debug Functions
  * @brief    JY6311 Debug Functions
  * @{
  */

/**
  * @brief  jy6311 all registers read for debug
  * @param  i2c_addr jy6311 i2c address
  * @param  print_all print all registers value or only print the registers value which not equal to default
  */
void jy6311_all_regs_read(uint8_t i2c_addr, bool print_all)
{
    uint16_t i;
    uint8_t reg_val;

    for (i = 0; i < JY6311_ARRAY_SIZE(jy6311_reg_defaults); i++) {
        reg_val = JY6311_I2C_READ_BYTE(i2c_addr, jy6311_reg_defaults[i].reg);

        if (print_all || reg_val != jy6311_reg_defaults[i].val) {
            JY6311_LOG_D("[0x%02x]: 0x%02x\n", jy6311_reg_defaults[i].reg, reg_val);
        }
    }
}

/**
  * @}
  */

/** @defgroup JY6311_Exported_Functions_Group7 JY6311 Multi Chip Config Functions
  * @brief    JY6311 Multi Chip Config Functions
  * @{
  */

/**
  * @brief  jy6311 multi-chips read
  * @param  i2c_addrs[] jy6311 i2c address array
  * @param  chip_nums chip numbers (array size)
  * @param  reg JY6311 register address to read
  * @param  rt_val return read value array
  * @retval 0      Register read Success
  * @retval others Register read Failed
  */
int8_t jy6311_multichips_read(const uint8_t i2c_addrs[], uint8_t chip_nums, uint8_t reg, uint8_t rt_val[])
{
    uint8_t i;
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check params config to be valid
    if (i2c_addrs == NULL || !chip_nums) {
        JY6311_LOG_E("Multi-chips <read> i2c_addrs array is NULL or chip_nums is 0!\n");
        return -1;
    }

    if (rt_val == NULL) {
        JY6311_LOG_E("Multi-chips <read> return rt_val array is NULL!\n");
        return -1;
    }

    // Multi-chips read
    for (i = 0; i < chip_nums; i++) {
        rt_val[i] = JY6311_I2C_READ_BYTE(i2c_addrs[i], reg);
    }

    return 0;
}

/**
  * @brief  jy6311 multi-chips write
  * @param  i2c_addrs[] jy6311 i2c address array
  * @param  chip_nums chip numbers (array size)
  * @param  reg JY6311 register address to write
  * @param  val register value
  * @retval 0      Register write Success
  * @retval others Register write Failed
  */
int8_t jy6311_multichips_write(const uint8_t i2c_addrs[], uint8_t chip_nums, uint8_t reg, uint8_t val)
{
    uint8_t i;
    int8_t ret = 0;
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check params config to be valid
    if (i2c_addrs == NULL || !chip_nums) {
        JY6311_LOG_E("Multi-chips <write> i2c_addrs array is NULL or chip_nums is 0!\n");
        return -1;
    }

    // Multi-chips write
    for (i = 0; i < chip_nums; i++) {
        ret |= JY6311_I2C_WRITE_BYTE(i2c_addrs[i], reg, val);
    }

    return ret;
}

/**
  * @brief  jy6311 multi-chips update bits
  * @param  i2c_addrs[] jy6311 i2c address array
  * @param  chip_nums chip numbers (array size)
  * @param  reg JY6311 register address to update
  * @param  mask update mask
  * @param  val update value
  * @retval 0      Register update Success
  * @retval others Register update Failed
  */
int8_t jy6311_multichips_update_bits(const uint8_t i2c_addrs[], uint8_t chip_nums, uint8_t reg, uint8_t mask,
                                     uint8_t val)
{
    uint8_t i;
    int8_t ret = 0;
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check params config to be valid
    if (i2c_addrs == NULL || !chip_nums) {
        JY6311_LOG_E("Multi-chips <update_bits> i2c_addrs array is NULL or chip_nums is 0!\n");
        return -1;
    }

    // Multi-chips update bits
    for (i = 0; i < chip_nums; i++) {
        ret |= jy6311_update_bits(i2c_addrs[i], reg, mask, val);
    }

    return ret;
}

/**
  * @brief  jy6311 multi-chips init
  * @param  i2c_addrs[] jy6311 i2c address array
  * @param  chip_nums chip numbers (array size)
  * @param  mod jy6311 init module select
  * @return JY6311_StatusETypeDef
  */
JY6311_StatusETypeDef jy6311_multichips_init(const uint8_t i2c_addrs[], uint8_t chip_nums, JY6311_InitModETypeDef mod)
{
    uint8_t i;
    JY6311_StatusETypeDef ret = JY6311_OK;
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check params config to be valid
    if (i2c_addrs == NULL || !chip_nums) {
        JY6311_LOG_E("Multi-chips <init> i2c_addrs array is NULL or chip_nums is 0!\n");
        return JY6311_INVALID;
    }

    // Multi-chips init
    for (i = 0; i < chip_nums; i++) {
        ret |= jy6311_init(i2c_addrs[i], mod);
    }

    JY6311_FUNC_ALTER(ret == JY6311_OK, return JY6311_OK, return JY6311_FAILED);
}

/**
  * @brief  jy6311 multi-chips deinit
  * @param  i2c_addrs[] jy6311 i2c address array
  * @param  chip_nums chip numbers (array size)
  * @param  mod jy6311 deinit module select
  * @return JY6311_StatusETypeDef
  */
JY6311_StatusETypeDef jy6311_multichips_deinit(const uint8_t i2c_addrs[], uint8_t chip_nums, JY6311_InitModETypeDef mod)
{
    uint8_t i;
    JY6311_StatusETypeDef ret = JY6311_OK;
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check params config to be valid
    if (i2c_addrs == NULL || !chip_nums) {
        JY6311_LOG_E("Multi-chips <deinit> i2c_addrs array is NULL or chip_nums is 0!\n");
        return JY6311_INVALID;
    }

    // Multi-chips deinit
    for (i = 0; i < chip_nums; i++) {
        ret |= jy6311_deinit(i2c_addrs[i], mod);
    }

    JY6311_FUNC_ALTER(ret == JY6311_OK, return JY6311_OK, return JY6311_FAILED);
}

/**
  * @brief  jy6311 multi-chips play start
  * @param  i2c_addrs[] jy6311 i2c address array
  * @param  chip_nums chip numbers (array size)
  */
void jy6311_multichips_play_start(const uint8_t i2c_addrs[], uint8_t chip_nums)
{
    uint8_t i;
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check params config to be valid
    if (i2c_addrs == NULL || !chip_nums) {
        JY6311_LOG_E("Multi-chips <play_start> i2c_addrs array is NULL or chip_nums is 0!\n");
        return;
    }

    // Multi-chips play start
    for (i = 0; i < chip_nums; i++) {
        // Only config the last chip to check for waiting for ramp up complete
        jy6311_play_start_internal(i2c_addrs[i], (chip_nums > 1 && i < chip_nums - 1) ? false : true);
    }
}

/**
  * @brief  jy6311 multi-chips play stop
  * @param  i2c_addrs[] jy6311 i2c address array
  * @param  chip_nums chip numbers (array size)
  */
void jy6311_multichips_play_stop(const uint8_t i2c_addrs[], uint8_t chip_nums)
{
    uint8_t i;
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check params config to be valid
    if (i2c_addrs == NULL || !chip_nums) {
        JY6311_LOG_E("Multi-chips <play_stop> i2c_addrs array is NULL or chip_nums is 0!\n");
        return;
    }

    // Multi-chips play stop
    for (i = 0; i < chip_nums; i++) {
        // Only config the last chip to check for waiting for ramp down complete
        jy6311_play_stop_internal(i2c_addrs[i], (chip_nums > 1 && i < chip_nums - 1) ? false : true);
    }
}

/**
  * @brief  jy6311 multi-chips record start
  * @param  i2c_addrs[] jy6311 i2c address array
  * @param  chip_nums chip numbers (array size)
  */
void jy6311_multichips_record_start(const uint8_t i2c_addrs[], uint8_t chip_nums)
{
    uint8_t i;
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check params config to be valid
    if (i2c_addrs == NULL || !chip_nums) {
        JY6311_LOG_E("Multi-chips <record_start> i2c_addrs array is NULL or chip_nums is 0!\n");
        return;
    }

    // Multi-chips record start
    for (i = 0; i < chip_nums; i++) {
        jy6311_record_start(i2c_addrs[i]);
    }
}

/**
  * @brief  jy6311 multi-chips record stop
  * @param  i2c_addrs[] jy6311 i2c address array
  * @param  chip_nums chip numbers (array size)
  */
void jy6311_multichips_record_stop(const uint8_t i2c_addrs[], uint8_t chip_nums)
{
    uint8_t i;
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check params config to be valid
    if (i2c_addrs == NULL || !chip_nums) {
        JY6311_LOG_E("Multi-chips <record_stop> i2c_addrs array is NULL or chip_nums is 0!\n");
        return;
    }

    // Multi-chips record stop
    for (i = 0; i < chip_nums; i++) {
        jy6311_record_stop(i2c_addrs[i]);
    }
}

/**
  * @brief  jy6311 multi-chips play volume config
  * @param  i2c_addrs[] jy6311 i2c address array
  * @param  chip_nums chip numbers (array size)
  * @param  vol jy6311 DAC digial volume
  * @note   vol range in [0x0, 0xff], which means [-95dB, 32dB], 0.5dB/Step, while 0 means Mute, 1 means -95dB
  */
void jy6311_multichips_play_vol_cfg(const uint8_t i2c_addrs[], uint8_t chip_nums, uint8_t vol)
{
    uint8_t i;
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check params config to be valid
    if (i2c_addrs == NULL || !chip_nums) {
        JY6311_LOG_E("Multi-chips <play_vol_cfg> i2c_addrs array is NULL or chip_nums is 0!\n");
        return;
    }

    // Multi-chips play volume config
    for (i = 0; i < chip_nums; i++) {
        jy6311_play_vol_cfg(i2c_addrs[i], vol);
    }
}

/**
  * @brief  jy6311 multi-chips record gain config
  * @param  i2c_addrs[] jy6311 i2c address array
  * @param  chip_nums chip numbers (array size)
  * @param  gain jy6311 ADC analog gain
  */
void jy6311_multichips_record_gain_cfg(const uint8_t i2c_addrs[], uint8_t chip_nums, JY6311_ADC_PGAGainETypeDef gain)
{
    uint8_t i;
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check params config to be valid
    if (i2c_addrs == NULL || !chip_nums) {
        JY6311_LOG_E("Multi-chips <record_gain_cfg> i2c_addrs array is NULL or chip_nums is 0!\n");
        return;
    }

    // Multi-chips record gain config
    for (i = 0; i < chip_nums; i++) {
        jy6311_record_gain_cfg(i2c_addrs[i], gain);
    }
}

/**
  * @brief  jy6311 multi-chips i2s config
  * @param  i2c_addrs[] jy6311 i2c address array
  * @param  chip_nums chip numbers (array size)
  * @param  cfg jy6311 i2s config struct pointer
  * @return JY6311_StatusETypeDef
  */
JY6311_StatusETypeDef jy6311_multichips_i2s_cfg(const uint8_t i2c_addrs[], uint8_t chip_nums, JY6311_I2SCfgTypeDef *cfg)
{
    uint8_t i;
    JY6311_StatusETypeDef ret = JY6311_OK;
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check params config to be valid
    if (i2c_addrs == NULL || !chip_nums) {
        JY6311_LOG_E("Multi-chips <i2s_cfg> i2c_addrs array is NULL or chip_nums is 0!\n");
        return JY6311_INVALID;
    }

    if (cfg == NULL) {
        JY6311_LOG_E("Multi-chips <i2s_cfg> i2s config pointer is NULL!\n");
        return JY6311_INVALID;
    }

    // Multi-chips i2s config
    for (i = 0; i < chip_nums; i++) {
        ret |= jy6311_i2s_cfg(i2c_addrs[i], cfg);
    }

    JY6311_FUNC_ALTER(ret == JY6311_OK, return JY6311_OK, return JY6311_FAILED);
}

/**
  * @brief  jy6311 multi-chips i2s extend config
  * @param  i2c_addrs[] jy6311 i2c address array
  * @param  chip_nums chip numbers (array size)
  * @return JY6311_StatusETypeDef
  */
JY6311_StatusETypeDef jy6311_multichips_i2s_ext_cfg(const uint8_t i2c_addrs[], uint8_t chip_nums)
{
    uint8_t i;
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check params config to be valid
    if (i2c_addrs == NULL || !chip_nums) {
        JY6311_LOG_E("Multi-chips <i2s_ext_cfg> i2c_addrs array is NULL or chip_nums is 0!\n");
        return JY6311_INVALID;
    }

    if (chip_nums == 1) {
        JY6311_LOG_D("Multi-chips <i2s_ext_cfg> single chip don't need to config i2s extend.\n");
        return JY6311_OK;
    }

    // Multi-chips I2S Extend Config
    for (i = 0; i < chip_nums; i++) {
        __JY6311_I2S_Tx_SlotEn_Set(i2c_addrs[i], JY6311_BIT(i));
        __JY6311_I2S_Rx_MixerSrcEn_Set(i2c_addrs[i], JY6311_BIT(i));
        //__JY6311_I2S_Tx_SlotNums_Set(i2c_addrs[i], chip_nums);
        //__JY6311_I2S_Rx_SlotNums_Set(i2c_addrs[i], chip_nums);

        // Tx Last word and idle state config
        if (chip_nums > 1) {
            __JY6311_I2S_Tx_LastWordState_HiZ(i2c_addrs[i]);
            __JY6311_I2S_Tx_IdleState_HiZ(i2c_addrs[i]);
        } else {
            __JY6311_I2S_Tx_LastWordState_Normal(i2c_addrs[i]);
            __JY6311_I2S_Tx_IdleState_Zero(i2c_addrs[i]);
        }
    }

    return JY6311_OK;
}

/**
  * @brief  jy6311 multi-chips sysclk config
  * @param  i2c_addrs[] jy6311 i2c address array
  * @param  chip_nums chip numbers (array size)
  * @param  cfg jy6311 sysclk config struct pointer
  * @return JY6311_StatusETypeDef
  */
JY6311_StatusETypeDef jy6311_multichips_sysclk_cfg(const uint8_t i2c_addrs[], uint8_t chip_nums,
                                                   JY6311_SysclkCfgTypeDef *cfg)
{
    uint8_t i;
    JY6311_StatusETypeDef ret = JY6311_OK;
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check params config to be valid
    if (i2c_addrs == NULL || !chip_nums) {
        JY6311_LOG_E("Multi-chips <sysclk_cfg> i2c_addrs array is NULL or chip_nums is 0!\n");
        return JY6311_INVALID;
    }

    if (cfg == NULL) {
        JY6311_LOG_E("Multi-chips <sysclk_cfg> sysclk config pointer is NULL!\n");
        return JY6311_INVALID;
    }

    // Multi-chips sysclk config
    for (i = 0; i < chip_nums; i++) {
        ret |= jy6311_sysclk_cfg(i2c_addrs[i], cfg);
    }

    JY6311_FUNC_ALTER(ret == JY6311_OK, return JY6311_OK, return JY6311_FAILED);
}

/**
  * @brief  jy6311 multi-chips play path config
  * @param  i2c_addrs[] jy6311 i2c address array
  * @param  chip_nums chip numbers (array size)
  * @param  path DAC output path
  */
void jy6311_multichips_play_path_cfg(const uint8_t i2c_addrs[], uint8_t chip_nums, JY6311_DAC_OutPathETypeDef path)
{
    uint8_t i;
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check params config to be valid
    if (i2c_addrs == NULL || !chip_nums) {
        JY6311_LOG_E("Multi-chips <play_path_cfg> i2c_addrs array is NULL or chip_nums is 0!\n");
        return;
    }

    // Multi-chips play path config
    for (i = 0; i < chip_nums; i++) {
        jy6311_play_path_cfg(i2c_addrs[i], path);
    }
}

/**
  * @brief  jy6311 multi-chips pdm config
  * @param  i2c_addrs[] jy6311 i2c address array
  * @param  chip_nums chip numbers (array size)
  * @param  en pdm enable config
  * @param  cfg jy6311 pdm config struct pointer
  * @return JY6311_StatusETypeDef
  */
JY6311_StatusETypeDef jy6311_multichips_pdm_cfg(const uint8_t i2c_addrs[], uint8_t chip_nums, bool en,
                                                JY6311_PDMCfgTypeDef *cfg)
{
    uint8_t i;
    JY6311_StatusETypeDef ret = JY6311_OK;
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check params config to be valid
    if (i2c_addrs == NULL || !chip_nums) {
        JY6311_LOG_E("Multi-chips <pdm_cfg> i2c_addrs array is NULL or chip_nums is 0!\n");
        return JY6311_INVALID;
    }

    if (en && cfg == NULL) {
        JY6311_LOG_E("Multi-chips <pdm_cfg> pdm config pointer is NULL!\n");
        return JY6311_INVALID;
    }

    // Multi-chips pdm config
    for (i = 0; i < chip_nums; i++) {
        ret |= jy6311_pdm_cfg(i2c_addrs[i], en, cfg);
    }

    JY6311_FUNC_ALTER(ret == JY6311_OK, return JY6311_OK, return JY6311_FAILED);
}

/**
  * @brief  jy6311 multi-chips adc eq config
  * @param  i2c_addrs[] jy6311 i2c address array
  * @param  chip_nums chip numbers (array size)
  * @param  en adc eq enable config
  * @param  array_cfg[] adc eq coff config array pointer
  * @param  array_size adc eq coff config array size in int32_t unit
  * @return JY6311_StatusETypeDef
  */
JY6311_StatusETypeDef jy6311_multichips_adc_eq_cfg(const uint8_t i2c_addrs[], uint8_t chip_nums, bool en,
                                                   const int32_t array_cfg[], uint8_t array_size)
{
    uint8_t i;
    JY6311_StatusETypeDef ret = JY6311_OK;
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check params config to be valid
    if (i2c_addrs == NULL || !chip_nums) {
        JY6311_LOG_E("Multi-chips <adc_eq_cfg> i2c_addrs array is NULL or chip_nums is 0!\n");
        return JY6311_INVALID;
    }

    // Multi-chips adc eq config
    for (i = 0; i < chip_nums; i++) {
        ret |= jy6311_adc_eq_cfg(i2c_addrs[i], en, array_cfg, array_size);
    }

    JY6311_FUNC_ALTER(ret == JY6311_OK, return JY6311_OK, return JY6311_FAILED);
}

/**
  * @brief  jy6311 multi-chips adc drc config
  * @param  i2c_addrs[] jy6311 i2c address array
  * @param  chip_nums chip numbers (array size)
  * @param  en adc drc enable config
  * @param  array_cfg[] adc drc coff config array pointer
  * @param  array_size adc drc coff config array size in int32_t unit
  * @return JY6311_StatusETypeDef
  */
JY6311_StatusETypeDef jy6311_multichips_adc_drc_cfg(const uint8_t i2c_addrs[], uint8_t chip_nums, bool en,
                                                    const int32_t array_cfg[], uint8_t array_size)
{
    uint8_t i;
    JY6311_StatusETypeDef ret = JY6311_OK;
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check params config to be valid
    if (i2c_addrs == NULL || !chip_nums) {
        JY6311_LOG_E("Multi-chips <adc_drc_cfg> i2c_addrs array is NULL or chip_nums is 0!\n");
        return JY6311_INVALID;
    }

    // Multi-chips adc drc config
    for (i = 0; i < chip_nums; i++) {
        ret |= jy6311_adc_drc_cfg(i2c_addrs[i], en, array_cfg, array_size);
    }

    JY6311_FUNC_ALTER(ret == JY6311_OK, return JY6311_OK, return JY6311_FAILED);
}

/**
  * @brief  jy6311 multi-chips dac eq config
  * @param  i2c_addrs[] jy6311 i2c address array
  * @param  chip_nums chip numbers (array size)
  * @param  en dac eq enable config
  * @param  array_cfg[] dac eq coff config array pointer
  * @param  array_size dac eq coff config array size in int32_t unit
  * @return JY6311_StatusETypeDef
  */
JY6311_StatusETypeDef jy6311_multichips_dac_eq_cfg(const uint8_t i2c_addrs[], uint8_t chip_nums, bool en,
                                                   const int32_t array_cfg[], uint8_t array_size)
{
    uint8_t i;
    JY6311_StatusETypeDef ret = JY6311_OK;
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check params config to be valid
    if (i2c_addrs == NULL || !chip_nums) {
        JY6311_LOG_E("Multi-chips <dac_eq_cfg> i2c_addrs array is NULL or chip_nums is 0!\n");
        return JY6311_INVALID;
    }

    // Multi-chips dac eq config
    for (i = 0; i < chip_nums; i++) {
        ret |= jy6311_dac_eq_cfg(i2c_addrs[i], en, array_cfg, array_size);
    }

    JY6311_FUNC_ALTER(ret == JY6311_OK, return JY6311_OK, return JY6311_FAILED);
}

/**
  * @brief  jy6311 multi-chips dac drc config
  * @param  i2c_addrs[] jy6311 i2c address array
  * @param  chip_nums chip numbers (array size)
  * @param  en dac drc enable config
  * @param  array_cfg[] dac drc coff config array pointer
  * @param  array_size dac drc coff config array size in int32_t unit
  * @return JY6311_StatusETypeDef
  */
JY6311_StatusETypeDef jy6311_multichips_dac_drc_cfg(const uint8_t i2c_addrs[], uint8_t chip_nums, bool en,
                                                    const int32_t array_cfg[], uint8_t array_size)
{
    uint8_t i;
    JY6311_StatusETypeDef ret = JY6311_OK;
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check params config to be valid
    if (i2c_addrs == NULL || !chip_nums) {
        JY6311_LOG_E("Multi-chips <dac_drc_cfg> i2c_addrs array is NULL or chip_nums is 0!\n");
        return JY6311_INVALID;
    }

    // Multi-chips dac drc config
    for (i = 0; i < chip_nums; i++) {
        ret |= jy6311_dac_drc_cfg(i2c_addrs[i], en, array_cfg, array_size);
    }

    JY6311_FUNC_ALTER(ret == JY6311_OK, return JY6311_OK, return JY6311_FAILED);
}

/**
  * @brief  jy6311 multi-chips all registers read for debug
  * @param  i2c_addrs[] jy6311 i2c address array
  * @param  chip_nums chip numbers (array size)
  * @param  print_all print all registers value or only print the registers value which not equal to default
  */
void jy6311_multichips_all_regs_read(const uint8_t i2c_addrs[], uint8_t chip_nums, bool print_all)
{
    uint8_t i;
    JY6311_LOG_I("--->%s\n", __FUNCTION__);

    // Check params config to be valid
    if (i2c_addrs == NULL || !chip_nums) {
        JY6311_LOG_E("Multi-chips <all_regs_read> i2c_addrs array is NULL or chip_nums is 0!\n");
        return;
    }

    // Multi-chips all registers read
    for (i = 0; i < chip_nums; i++) {
        JY6311_LOG_D("Chip[%d] register read start:\n", i);
        jy6311_all_regs_read(i2c_addrs[i], print_all);
    }
}

/**
  * @}
  */

/**
  * @}
  */

/* Private Functions ---------------------------------------------------------*/
/** @defgroup JY6311_Private_Functions JY6311 Private Functions
  * @brief    JY6311 Private Functions
  * @{
  */

/**
  * @brief  jy6311 sample rate real value to register value or array index
  * @param  rate sample rate real
  * @param  reg return register flag, if true return register value, else return jy6311_smp_osr_pll_out array index
  * @return register value or array index
  */
static uint8_t jy6311_smpRateToRegOrIdx(uint32_t rate, bool reg)
{
    switch (rate) {
        case 8000:
            return JY6311_ADDA_SR_8K;

        case 12000:
            return JY6311_ADDA_SR_12K;

        case 16000:
            return JY6311_ADDA_SR_16K;

        case 24000:
            return JY6311_ADDA_SR_24K;

        case 32000:
            return JY6311_ADDA_SR_32K;

        case 48000:
            return JY6311_ADDA_SR_48K;

        case 96000:
            return JY6311_ADDA_SR_96K;

        case 192000:
            return JY6311_ADDA_SR_192K;

        case 11025:
            return reg ? JY6311_ADDA_SR_11025 : JY6311_ADDA_SR_192K + 1;

        case 22050:
            return reg ? JY6311_ADDA_SR_22050 : JY6311_ADDA_SR_192K + 2;

        case 44100:
            return reg ? JY6311_ADDA_SR_44100 : JY6311_ADDA_SR_192K + 3;

        case 88200:
            return reg ? JY6311_ADDA_SR_88200 : JY6311_ADDA_SR_192K + 4;

        case 176400:
            return reg ? JY6311_ADDA_SR_176400 : JY6311_ADDA_SR_192K + 5;

        default:
            JY6311_LOG_E("jy6311 don't support sample rate real value: %" PRIu32 "!\n", rate);
            return 0;
    }
}

/**
  * @brief  jy6311 pll config
  * @param  i2c_addr jy6311 i2c address
  * @param  clk_src PLLCLK source
  * @param  freq_in PLLCLK input frequence
  * @param  freq_out PLLCLK output frequence
  * @return JY6311_StatusETypeDef
  */
static JY6311_StatusETypeDef jy6311_pllCfg(uint8_t i2c_addr, JY6311_PLLCLKSrcETypeDef clk_src, uint32_t freq_in,
                                           uint32_t freq_out)
{
    uint8_t p = 1, m = 2;
    uint16_t i, n = 1;

    // Check PLL frequence out to be valid
    if (!freq_out) {
        JY6311_LOG_E("PLLCLK frequence out [%" PRIu32 "] config error!\n", freq_out);
        return JY6311_INVALID;
    }

    // Check PLL frequence in to be valid
    if (freq_in < 512000 || freq_in > 24576000) {
        JY6311_LOG_E("PLLCLK source input freq only support [512K,24M], while now %" PRIu32 "!\n", freq_in);
        return JY6311_INVALID;
    }

    // FOUT = FIN * N / (M*P), N[1, 256], P[1, 32], M[2, 16] and must be even
    for (i = 0; i < JY6311_ARRAY_SIZE(jy6311_pll_div); i++) {
        if (jy6311_pll_div[i].freq_in == freq_in && jy6311_pll_div[i].freq_out == freq_out) {
            n = jy6311_pll_div[i].n;
            p = jy6311_pll_div[i].p;
            m = jy6311_pll_div[i].m;
            JY6311_LOG_D("PLLCLK freq_in match:%" PRIu32 ", freq_out:%" PRIu32 ".\n", freq_in, freq_out);
            break;
        }
    }

    if (i == JY6311_ARRAY_SIZE(jy6311_pll_div)) {
        JY6311_LOG_E("Don't match PLLCLK freq_in[%" PRIu32 "] and freq_out[%" PRIu32 "] table, "
                     "please add a configuration item in jy6311_pll_div array for this!\n",
                     freq_in, freq_out);
        return JY6311_FAILED;
    }

    // PLLCLK Disable first before config
    __JY6311_PLLCLK_Dis(i2c_addr);
    __JY6311_PLL_LDOAndCirc_Dis(i2c_addr);

    // PLL Clock Source Select
    __JY6311_PLLCLK_Src_Set(i2c_addr, clk_src);
    JY6311_LOG_D("PLLCLK input source select %s.\n", clk_src == JY6311_PLLCLK_SRC_MCLK ? "MCLK" : "BCLK");

    // PLL DIV param Config
    __JY6311_PLL_FeedbackDiv_Set(i2c_addr, n);
    __JY6311_PLL_PreDiv_Set(i2c_addr, p);
    __JY6311_PLL_PostDiv_Set(i2c_addr, m);

    // PLLCLK Enable
    __JY6311_PLLCLK_En(i2c_addr);
    __JY6311_PLL_LDOAndCirc_En(i2c_addr);

    return JY6311_OK;
}

/**
  * @brief  jy6311 play start internal
  * @param  i2c_addr jy6311 i2c address
  * @param  check_wait_rampup whether need to check for waiting for ramp up complete
  */
static void jy6311_play_start_internal(uint8_t i2c_addr, bool check_wait_rampup)
{
    // I2S Rx Enable
    __JY6311_I2S_Rx_En(i2c_addr);

    // DAC Digital Enable
    __JY6311_DAC_Dig_En(i2c_addr);

    // DAC Analog Enable
    __JY6311_DAC_OutDCT_En(i2c_addr);

    // Wait for DAC Ramp Up Complete if Ramp Up Enable
    if (check_wait_rampup && __JY6311_DAC_OutRampUpEn_Get(i2c_addr)) {
        JY6311_LOG_D("Wait for DAC Ramp Up Complete...\n");
        JY6311_DELAY_MS_USER(JY6311_DAC_RAMP_UP_TIME_MS);
        JY6311_LOG_D("DAC Ramp Up Complete\n");
    }
}

/**
  * @brief  jy6311 play stop internal
  * @param  i2c_addr jy6311 i2c address
  * @param  check_wait_rampdown whether need to check for waiting for ramp down complete
  */
static void jy6311_play_stop_internal(uint8_t i2c_addr, bool check_wait_rampdown)
{
    // DAC Analog Disable
    __JY6311_DAC_OutDCT_Dis(i2c_addr);

    // Wait for DAC Ramp Down Complete if Ramp Up Enable
    if (check_wait_rampdown && __JY6311_DAC_OutRampUpEn_Get(i2c_addr)) {
        JY6311_LOG_D("Wait for DAC Ramp Down Complete...\n");
        JY6311_DELAY_MS_USER(JY6311_DAC_RAMP_DOWN_TIME_MS);
        JY6311_LOG_D("DAC Ramp Down Complete\n");
    }

    // DAC Digital Disable
    __JY6311_DAC_Dig_Dis(i2c_addr);

    // I2S Rx Disable
    __JY6311_I2S_Rx_Dis(i2c_addr);
}

/**
  * @}
  */

#endif /* JY6311_MODULE_ENABLED */

/**
  * @}
  */

/** @defgroup JY6311_config JY6311 config
  * @brief    JY6311 config
  * @{
  */

/* Private Constants ---------------------------------------------------------*/
/** @defgroup JY6311_config_Private_Constants JY6311 config Private Constants
  * @brief    JY6311 config Private Constants
  * @{
  */

/**
  * @brief JY6311 I2C Address
  */
#define JY6311_I2C_ADDR (JY6311_I2C_ADDR_0)

/**
  * @}
  */

/* Private Macros ------------------------------------------------------------*/
/* Private Types -------------------------------------------------------------*/
/* Private Variables ---------------------------------------------------------*/
/** @defgroup JY6311_config_Private_Variables JY6311 config Private Variables
  * @brief    JY6311 config Private Variables
  * @{
  */

/**
  * @brief JY6311 DAC EQ Coefficient array
  * @note  This parameter array is generated by JY6311 AEtool
  */
static const int32_t dac_eq_filt_coef[] = {
    0xfece5,  0xe02635, 0xfece5, 0xe0264c, 0xfd9e1, 0x10a9fc, 0xe0aca1, 0xeb35e,
    0xe0aca1, 0xf5d5b,  0xc638b, 0xef09f8, 0x79ca0, 0xef09f8, 0x4002c,
};

/**
  * @brief JY6311 DAC DRC Coefficient array
  * @note  This parameter array is generated by JY6311 AEtool
  */
static const int32_t dac_drc_filt_coef[] = {
    0x76, 0x27, 0x0, 0x12, 0x11, 0x26, 0x74, 0x7a,
};

/**
  * @brief JY6311 ADC EQ Coefficient array
  * @note  This parameter array is generated by JY6311 AEtool
  */
static const int32_t adc_eq_filt_coef[] = {
    0xd0307, 0xe5f9f1, 0xd0307, 0xe687af, 0xa93cc,
};

/**
  * @brief JY6311 ADC DRC Coefficient array
  * @note  This parameter array is generated by JY6311 AEtool
  */
static const int32_t adc_drc_filt_coef[] = {
    0x76, 0x27, 0x0, 0x12, 0x11, 0x26, 0x74, 0x7a,
};

/**
  * @}
  */

/* Private Function Prototypes -----------------------------------------------*/
/* Exported Variables --------------------------------------------------------*/
/* Exported Functions --------------------------------------------------------*/
/** @defgroup JY6311_config_Exported_Functions JY6311 config Exported Functions
  * @brief    JY6311 config Exported Functions
  * @{
  */

/**
  * @brief jy6311 play config
  * @note  1. This config merely demonstrates the interface invocation process. All parameters can be configured by the user.
  *        2. When use multichips, all interfaces change to multichips version, and call jy6311_multichips_i2s_ext_cfg interface
  *            to config I2S extend after i2s config interface.
  */
void jy6311_config_play(const audio_codec_cfg_t *cfg)
{
    JY6311_I2SCfgTypeDef i2s_cfg;
    JY6311_SysclkCfgTypeDef sysclk_cfg;
    JY6311_I2S_FmtETypeDef i2s_fmt = JY6311_I2S_FMT_I2S;
    JY6311_I2S_WordLenETypeDef word_len = JY6311_I2S_WORD_LEN_16bit;
    JY6311_I2SRoleETypeDef i2s_role = JY6311_I2S_ROLE_SLAVE;
    JY6311_UserSmpRateETypeDef sample_rate_hz = JY6311_USER_SR_16K;
    memset((void *)&i2s_cfg, 0x0, sizeof(i2s_cfg));
    memset((void *)&sysclk_cfg, 0x0, sizeof(sysclk_cfg));

    if (cfg != NULL) {
        audio_codec_jy6311_map_i2s_fmt(cfg->i2s_fmt, &i2s_fmt);
        audio_codec_jy6311_map_word_len(cfg->bits, &word_len);
        audio_codec_jy6311_map_sample_rate(cfg->sample_rate_hz, &sample_rate_hz);
        i2s_role = cfg->codec_master ? JY6311_I2S_ROLE_MASTER : JY6311_I2S_ROLE_SLAVE;
    }

    /******************************************************** Play Before ********************************************************/
    // JY6311 Init
    jy6311_init(JY6311_I2C_ADDR, JY6311_INIT_MOD_DAC);

    // JY6311 I2S Config
    i2s_cfg.bclk_invert = false;
    i2s_cfg.lrck_invert = false;
    i2s_cfg.fmt = i2s_fmt;
    i2s_cfg.role = i2s_role;
    i2s_cfg.word_len = word_len;
    jy6311_i2s_cfg(JY6311_I2C_ADDR, &i2s_cfg);
    // Call jy6311_multichips_i2s_ext_cfg if use multichips.

    // JY6311 SYSCLK Config
    sysclk_cfg.extclk_freq = 4096000;
    sysclk_cfg.extclk_src = JY6311_EXT_CLK_SRC_MCLK;
    sysclk_cfg.play_smp_rate = sample_rate_hz;
    sysclk_cfg.rec_smp_rate = JY6311_USER_SR_NONE;
    sysclk_cfg.work_mode = JY6311_ADDA_WORK_MODE_NORMAL;
    sysclk_cfg.i2s_lrck_period = 0;
    jy6311_sysclk_cfg(JY6311_I2C_ADDR, &sysclk_cfg);

    /* [Optional] DAC Optional Configuration, eg Path/Volume/EQ/DRC */
    jy6311_play_vol_cfg(JY6311_I2C_ADDR, 0xBF); // DAC Digital Volume 0dB
    jy6311_dac_eq_cfg(JY6311_I2C_ADDR, true, dac_eq_filt_coef, JY6311_ARRAY_SIZE(dac_eq_filt_coef));
    jy6311_dac_drc_cfg(JY6311_I2C_ADDR, true, dac_drc_filt_coef, JY6311_ARRAY_SIZE(dac_drc_filt_coef));
    jy6311_play_path_cfg(JY6311_I2C_ADDR, JY6311_DAC_OUT_PATH_HEADPHONE);

    // JY6311 Play Start
    jy6311_play_start(JY6311_I2C_ADDR);

#if 0
    /******************************************************** Play After ********************************************************/
    // JY6311 Play Stop
    jy6311_play_stop(JY6311_I2C_ADDR);

    /* [Optional] DAC Optional Configuration, eg Volume/EQ/DRC */
    jy6311_play_vol_cfg(JY6311_I2C_ADDR, 0xBF);                             // DAC Digital Volume 0dB
    jy6311_dac_eq_cfg(JY6311_I2C_ADDR, false, NULL, 0);
    jy6311_dac_drc_cfg(JY6311_I2C_ADDR, false, NULL, 0);

    // JY6311 DeInit
    jy6311_deinit(JY6311_I2C_ADDR, JY6311_INIT_MOD_DAC);
#endif
}

/**
  * @brief jy6311 record config
  * @note  1. This config merely demonstrates the interface invocation process. All parameters can be configured by the user.
  *        2. When use multichips, all interfaces change to multichips version, and call jy6311_multichips_i2s_ext_cfg interface
  *            to config I2S extend after i2s config interface.
  */
void jy6311_config_record(const audio_codec_cfg_t *cfg)
{
    JY6311_I2SCfgTypeDef i2s_cfg;
    JY6311_SysclkCfgTypeDef sysclk_cfg;
    JY6311_I2S_FmtETypeDef i2s_fmt = JY6311_I2S_FMT_I2S;
    JY6311_I2S_WordLenETypeDef word_len = JY6311_I2S_WORD_LEN_16bit;
    JY6311_I2SRoleETypeDef i2s_role = JY6311_I2S_ROLE_SLAVE;
    JY6311_UserSmpRateETypeDef sample_rate_hz = JY6311_USER_SR_16K;

    memset((void *)&i2s_cfg, 0x0, sizeof(i2s_cfg));
    memset((void *)&sysclk_cfg, 0x0, sizeof(sysclk_cfg));

    if (cfg != NULL) {
        audio_codec_jy6311_map_i2s_fmt(cfg->i2s_fmt, &i2s_fmt);
        audio_codec_jy6311_map_word_len(cfg->bits, &word_len);
        audio_codec_jy6311_map_sample_rate(cfg->sample_rate_hz, &sample_rate_hz);
        i2s_role = cfg->codec_master ? JY6311_I2S_ROLE_MASTER : JY6311_I2S_ROLE_SLAVE;
    }

    /******************************************************** Record Before ********************************************************/
    // JY6311 Init
    jy6311_init(JY6311_I2C_ADDR, JY6311_INIT_MOD_ADC);

    // JY6311 I2S Config
    i2s_cfg.bclk_invert = false;
    i2s_cfg.lrck_invert = false;
    i2s_cfg.fmt = i2s_fmt;
    i2s_cfg.role = i2s_role;
    i2s_cfg.word_len = word_len;
    jy6311_i2s_cfg(JY6311_I2C_ADDR, &i2s_cfg);
    // Call jy6311_multichips_i2s_ext_cfg if use multichips.

    // JY6311 SYSCLK Config
    sysclk_cfg.extclk_freq = 4096000;
    sysclk_cfg.extclk_src = JY6311_EXT_CLK_SRC_MCLK;
    sysclk_cfg.play_smp_rate = JY6311_USER_SR_NONE;
    sysclk_cfg.rec_smp_rate = sample_rate_hz;
    sysclk_cfg.work_mode = JY6311_ADDA_WORK_MODE_NORMAL;
    sysclk_cfg.i2s_lrck_period = 0;
    jy6311_sysclk_cfg(JY6311_I2C_ADDR, &sysclk_cfg);

    /* [Optional] ADC Optional Configuration, eg Gain/EQ/DRC */
    jy6311_record_gain_cfg(JY6311_I2C_ADDR, JY6311_ADC_PGA_GAIN_36dB); // ADC PGA Gain 36dB
    jy6311_adc_eq_cfg(JY6311_I2C_ADDR, true, adc_eq_filt_coef, JY6311_ARRAY_SIZE(adc_eq_filt_coef));
    jy6311_adc_drc_cfg(JY6311_I2C_ADDR, true, adc_drc_filt_coef, JY6311_ARRAY_SIZE(adc_drc_filt_coef));
#if 0
    /* [Optional] ADC Optional PDM Interface config if use  */
    JY6311_PDMCfgTypeDef pdm_cfg;
    memset((void *)&pdm_cfg, 0, sizeof(pdm_cfg));
    pdm_cfg.timing_invert = false;
    pdm_cfg.clk_io = JY6311_PDMCLK_IO_DEV_ID;
    jy6311_pdm_cfg(JY6311_I2C_ADDR, true, &pdm_cfg);
#endif

    // JY6311 Record Start
    jy6311_record_start(JY6311_I2C_ADDR);

    /******************************************************** Record After ********************************************************/
    // JY6311 Record Stop
    jy6311_record_stop(JY6311_I2C_ADDR);

    /* [Optional] ADC Optional Configuration, eg Gain/EQ/DRC */
    jy6311_record_gain_cfg(JY6311_I2C_ADDR, JY6311_ADC_PGA_GAIN_0dB); // ADC PGA Gain 0dB
    jy6311_adc_eq_cfg(JY6311_I2C_ADDR, false, NULL, 0);
    jy6311_adc_drc_cfg(JY6311_I2C_ADDR, false, NULL, 0);
#if 0
    /* [Optional] ADC Optional PDM Interface config if use */
    jy6311_pdm_cfg(JY6311_I2C_ADDR, false, NULL);
#endif

    // JY6311 DeInit
    jy6311_deinit(JY6311_I2C_ADDR, JY6311_INIT_MOD_ADC);
}

/**
  * @brief jy6311 play and record config
  * @note  1. This config merely demonstrates the interface invocation process. All parameters can be configured by the user.
  *        2. When use multichips, all interfaces change to multichips version, and call jy6311_multichips_i2s_ext_cfg interface
  *            to config I2S extend after i2s config interface.
  */
#define CHIP_NUM 1

void jy6311_config_play_record(const audio_codec_cfg_t *cfg)
{
    JY6311_I2SCfgTypeDef i2s_cfg;
    JY6311_SysclkCfgTypeDef sysclk_cfg;
    JY6311_I2S_FmtETypeDef i2s_fmt = JY6311_I2S_FMT_I2S;
    JY6311_I2S_WordLenETypeDef word_len = JY6311_I2S_WORD_LEN_16bit;
    JY6311_I2SRoleETypeDef i2s_role = JY6311_I2S_ROLE_SLAVE;
    JY6311_UserSmpRateETypeDef sample_rate_hz = JY6311_USER_SR_16K;

    uint8_t i2c_addr[2] = { 0x44, 0x45 };

    memset((void *)&i2s_cfg, 0x0, sizeof(i2s_cfg));
    memset((void *)&sysclk_cfg, 0x0, sizeof(sysclk_cfg));
    if (cfg != NULL) {
        audio_codec_jy6311_map_i2s_fmt(cfg->i2s_fmt, &i2s_fmt);
        audio_codec_jy6311_map_word_len(cfg->bits, &word_len);
        audio_codec_jy6311_map_sample_rate(cfg->sample_rate_hz, &sample_rate_hz);
        i2s_role = cfg->codec_master ? JY6311_I2S_ROLE_MASTER : JY6311_I2S_ROLE_SLAVE;
    }
    /**************************************************** Play&Record Before ****************************************************/
    // JY6311 Init
#ifdef CHIP_NUM
    jy6311_multichips_init(i2c_addr, 2, JY6311_INIT_MOD_ADC_DAC);
#else
    jy6311_init(JY6311_I2C_ADDR, JY6311_INIT_MOD_ADC_DAC);
#endif
    // JY6311 I2S Config
    i2s_cfg.bclk_invert = false;
    i2s_cfg.lrck_invert = false;
    i2s_cfg.fmt = i2s_fmt;
    i2s_cfg.role = i2s_role;
    i2s_cfg.word_len = word_len;
#ifdef CHIP_NUM
    jy6311_multichips_i2s_cfg(i2c_addr, 2, &i2s_cfg);
    jy6311_multichips_i2s_ext_cfg(i2c_addr, 2);
#else
    jy6311_i2s_cfg(JY6311_I2C_ADDR, &i2s_cfg);
#endif
    // Call jy6311_multichips_i2s_ext_cfg if use multichips.

    // JY6311 SYSCLK Config
    sysclk_cfg.extclk_freq = 4096000;
    sysclk_cfg.extclk_src = JY6311_EXT_CLK_SRC_MCLK;
    sysclk_cfg.play_smp_rate = sample_rate_hz;
    sysclk_cfg.rec_smp_rate = sample_rate_hz;
    sysclk_cfg.work_mode = JY6311_ADDA_WORK_MODE_NORMAL;
    sysclk_cfg.i2s_lrck_period = 0;
#ifdef CHIP_NUM
    jy6311_multichips_sysclk_cfg(i2c_addr, 2, &sysclk_cfg);
#else
    jy6311_sysclk_cfg(JY6311_I2C_ADDR, &sysclk_cfg);
#endif
    /* [Optional] DAC Optional Configuration, eg Path/Volume/EQ/DRC */

#ifdef CHIP_NUM
    jy6311_multichips_play_vol_cfg(i2c_addr, 2, 0xBF); // DAC Digital Volume 0dB
    jy6311_multichips_dac_eq_cfg(i2c_addr, 2, true, dac_eq_filt_coef, JY6311_ARRAY_SIZE(dac_eq_filt_coef));
    jy6311_multichips_dac_drc_cfg(i2c_addr, 2, true, dac_drc_filt_coef, JY6311_ARRAY_SIZE(dac_drc_filt_coef));
    //jy6311_multichips_play_path_cfg(i2c_addr, 2, JY6311_DAC_OUT_PATH_LINEOUT);
    jy6311_multichips_play_path_cfg(i2c_addr, 2, JY6311_DAC_OUT_PATH_HEADPHONE);
#else
    jy6311_play_vol_cfg(JY6311_I2C_ADDR, 0xBF); // DAC Digital Volume 0dB
    jy6311_dac_eq_cfg(JY6311_I2C_ADDR, true, dac_eq_filt_coef, JY6311_ARRAY_SIZE(dac_eq_filt_coef));
    jy6311_dac_drc_cfg(JY6311_I2C_ADDR, true, dac_drc_filt_coef, JY6311_ARRAY_SIZE(dac_drc_filt_coef));
    jy6311_play_path_cfg(JY6311_I2C_ADDR, JY6311_DAC_OUT_PATH_HEADPHONE);
#endif

    /* [Optional] ADC Optional Configuration, eg Gain/EQ/DRC */

#ifdef CHIP_NUM
    //jy6311_multichips_record_gain_cfg(i2c_addr, 2, JY6311_ADC_PGA_GAIN_30dB);      // ADC PGA Gain 36dB
    jy6311_record_gain_cfg(0x44, JY6311_ADC_PGA_GAIN_24dB); // ADC PGA Gain 36dB
    jy6311_record_gain_cfg(0x45, JY6311_ADC_PGA_GAIN_30dB); // ADC PGA Gain 36dB
    //jy6311_multichips_adc_eq_cfg(i2c_addr, 2, true, adc_eq_filt_coef, JY6311_ARRAY_SIZE(adc_eq_filt_coef));
    //jy6311_multichips_adc_drc_cfg(i2c_addr, 2, true, adc_drc_filt_coef, JY6311_ARRAY_SIZE(adc_drc_filt_coef));
#else
    jy6311_record_gain_cfg(JY6311_I2C_ADDR, JY6311_ADC_PGA_GAIN_30dB); // ADC PGA Gain 36dB
    //jy6311_adc_eq_cfg(JY6311_I2C_ADDR, true, adc_eq_filt_coef, JY6311_ARRAY_SIZE(adc_eq_filt_coef));
    //jy6311_adc_drc_cfg(JY6311_I2C_ADDR, true, adc_drc_filt_coef, JY6311_ARRAY_SIZE(adc_drc_filt_coef));
#endif

#if 0
    /* [Optional] ADC Optional PDM Interface config if use */
    JY6311_PDMCfgTypeDef pdm_cfg;
    memset((void *)&pdm_cfg, 0, sizeof(pdm_cfg));
    pdm_cfg.timing_invert = false;
    pdm_cfg.clk_io = JY6311_PDMCLK_IO_DEV_ID;
    jy6311_pdm_cfg(JY6311_I2C_ADDR, true, &pdm_cfg);
#endif

    // JY6311 Play Start
#ifdef CHIP_NUM
    jy6311_multichips_play_start(i2c_addr, 2);

    // JY6311 Record Start
    jy6311_multichips_record_start(i2c_addr, 2);
#else
    jy6311_play_start(JY6311_I2C_ADDR);

    // JY6311 Record Start
    jy6311_record_start(JY6311_I2C_ADDR);
#endif
    //jy6311_multichips_all_regs_read(i2c_addr, 2, 1);
#if 0
    /**************************************************** Play&Record After ****************************************************/
    // JY6311 Play Stop
    jy6311_play_stop(JY6311_I2C_ADDR);

    // JY6311 Record Stop
    jy6311_record_stop(JY6311_I2C_ADDR);

    /* [Optional] DAC Optional Configuration, eg Volume/EQ/DRC */
    jy6311_play_vol_cfg(JY6311_I2C_ADDR, 0xBF);                             // DAC Digital Volume 0dB
    jy6311_dac_eq_cfg(JY6311_I2C_ADDR, false, NULL, 0);
    jy6311_dac_drc_cfg(JY6311_I2C_ADDR, false, NULL, 0);

    /* [Optional] ADC Optional Configuration, eg Gain/EQ/DRC */
    jy6311_record_gain_cfg(JY6311_I2C_ADDR, JY6311_ADC_PGA_GAIN_0dB);       // ADC PGA Gain 0dB
    jy6311_adc_eq_cfg(JY6311_I2C_ADDR, false, NULL, 0);
    jy6311_adc_drc_cfg(JY6311_I2C_ADDR, false, NULL, 0);
#if 0
    /* [Optional] ADC Optional PDM Interface config if use */
    jy6311_pdm_cfg(JY6311_I2C_ADDR, false, NULL);
#endif

    // JY6311 DeInit
    jy6311_deinit(JY6311_I2C_ADDR, JY6311_INIT_MOD_ADC_DAC);
#endif
}

static struct bflb_device_s *jy6311_get_i2c_dev(void)
{
    if (g_jy6311_i2c_dev == NULL) {
        g_jy6311_i2c_dev = bflb_device_get_by_name("i2c0");
    }

    return g_jy6311_i2c_dev;
}

static void audio_codec_jy6311_bind_i2c(struct bflb_device_s *i2c)
{
    if (i2c != NULL) {
        g_jy6311_i2c_dev = i2c;
    }
}

void jy6311_delay_ms(int ms)
{
    bflb_mtimer_delay_ms(ms);
}

void jy6311_i2c_init(void)
{
    struct bflb_device_s *i2c = jy6311_get_i2c_dev();

    if (i2c != NULL) {
        bflb_i2c_init(i2c, 200000);
    }
}

unsigned char jy6311_i2c_read_byte(unsigned char i2c_addr, unsigned char reg)
{
    uint8_t value = 0;
    struct bflb_device_s *i2c = jy6311_get_i2c_dev();

    if (i2c == NULL) {
        return 0;
    }

    if (audio_codec_i2c_read_reg8(i2c, i2c_addr, reg, &value) != 0) {
        return 0;
    }

    return value;
}

signed char jy6311_i2c_write_byte(unsigned char i2c_addr, unsigned char reg, unsigned char val)
{
    struct bflb_device_s *i2c = jy6311_get_i2c_dev();

    if (i2c == NULL) {
        return -1;
    }

    return (signed char)audio_codec_i2c_write_reg8(i2c, i2c_addr, reg, val);
}

static uint8_t audio_codec_jy6311_percent_to_dig_vol(int volume_percent)
{
    if (volume_percent < 0) {
        volume_percent = 0;
    } else if (volume_percent > 100) {
        volume_percent = 100;
    }

    if (volume_percent == 0) {
        return 0x00;
    }

    return (uint8_t)(1 + (((volume_percent - 1) * 254) + 49) / 99);
}

static JY6311_ADC_PGAGainETypeDef audio_codec_jy6311_percent_to_pga(int pga_percent)
{
    static const JY6311_ADC_PGAGainETypeDef gain_table[] = {
        JY6311_ADC_PGA_GAIN_0dB,  JY6311_ADC_PGA_GAIN_6dB,  JY6311_ADC_PGA_GAIN_12dB, JY6311_ADC_PGA_GAIN_18dB,
        JY6311_ADC_PGA_GAIN_24dB, JY6311_ADC_PGA_GAIN_30dB, JY6311_ADC_PGA_GAIN_36dB, JY6311_ADC_PGA_GAIN_42dB,
    };
    int index;

    if (pga_percent <= 0) {
        return JY6311_ADC_PGA_GAIN_0dB;
    }
    if (pga_percent >= 100) {
        return JY6311_ADC_PGA_GAIN_42dB;
    }

    index = (pga_percent * ((int)JY6311_ARRAY_SIZE(gain_table) - 1) + 50) / 100;
    if (index < 0) {
        index = 0;
    }
    if (index >= (int)JY6311_ARRAY_SIZE(gain_table)) {
        index = (int)JY6311_ARRAY_SIZE(gain_table) - 1;
    }

    return gain_table[index];
}

static int audio_codec_jy6311_map_sample_rate(uint32_t sample_rate_hz, JY6311_UserSmpRateETypeDef *out_rate)
{
    switch (sample_rate_hz) {
        case 8000:
        case 11025:
        case 12000:
        case 16000:
        case 22050:
        case 24000:
        case 32000:
        case 44100:
        case 48000:
        case 88200:
        case 96000:
        case 176400:
        case 192000:
            *out_rate = (JY6311_UserSmpRateETypeDef)sample_rate_hz;
            return 0;
        default:
            return 1;
    }
}

static int audio_codec_jy6311_map_i2s_fmt(audio_codec_i2s_fmt_t i2s_fmt, JY6311_I2S_FmtETypeDef *out_fmt)
{
    switch (i2s_fmt) {
        case AUDIO_CODEC_I2S_FMT_I2S:
            *out_fmt = JY6311_I2S_FMT_I2S;
            return true;
        case AUDIO_CODEC_I2S_FMT_LEFT_J:
            *out_fmt = JY6311_I2S_FMT_LEFT_J;
            return true;
        case AUDIO_CODEC_I2S_FMT_DSP:
            *out_fmt = JY6311_I2S_FMT_PCM_A;
            return 0;
        default:
            return -1;
    }
}

static int audio_codec_jy6311_map_word_len(audio_codec_bits_t bits, JY6311_I2S_WordLenETypeDef *out_word_len)
{
    switch (bits) {
        case AUDIO_CODEC_BITS_16:
            *out_word_len = JY6311_I2S_WORD_LEN_16bit;
            return 0;
        case AUDIO_CODEC_BITS_24:
            *out_word_len = JY6311_I2S_WORD_LEN_24bit;
            return 0;
        case AUDIO_CODEC_BITS_32:
            *out_word_len = JY6311_I2S_WORD_LEN_32bit;
            return 0;
        default:
            return -1;
    }
}

static uint32_t audio_codec_jy6311_default_mclk(uint32_t sample_rate_hz)
{
    if (sample_rate_hz == 0U) {
        return 0U;
    }

    return sample_rate_hz * 256U;
}

static JY6311_InitModETypeDef audio_codec_jy6311_get_init_mod(const audio_codec_cfg_t *cfg)
{
    if (cfg->enable_adc && cfg->enable_dac) {
        return JY6311_INIT_MOD_ADC_DAC;
    }
    if (cfg->enable_adc) {
        return JY6311_INIT_MOD_ADC;
    }
    if (cfg->enable_dac) {
        return JY6311_INIT_MOD_DAC;
    }

    return JY6311_INIT_MOD_NONE;
}

static int audio_codec_jy6311_apply_runtime_state(audio_codec_dev_t *dev)
{
    if (dev == NULL) {
        return -1;
    }

    if (g_jy6311_ctx.cfg.enable_dac) {
        jy6311_play_vol_cfg(dev->i2c_addr, g_jy6311_ctx.dac_muted ? 0x00 : g_jy6311_ctx.dac_volume_reg);
    }

    if (g_jy6311_ctx.cfg.enable_adc) {
        __JY6311_ADC_DigVol_Set(dev->i2c_addr, g_jy6311_ctx.adc_volume_reg);
        jy6311_record_gain_cfg(dev->i2c_addr, g_jy6311_ctx.adc_pga_gain);
    }

    return 0;
}

static int audio_codec_jy6311_configure(audio_codec_dev_t *dev, const audio_codec_cfg_t *cfg)
{
    if (dev == NULL || dev->i2c == NULL || cfg == NULL) {
        return -1;
    }

    audio_codec_jy6311_bind_i2c(dev->i2c);
    if (dev->i2c_addr == 0U) {
        dev->i2c_addr = JY6311_I2C_ADDR_0;
    }

    switch (cfg->work_mode) {
        case AUDIO_CODEC_WORK_MODE_RECORDING:
            jy6311_config_record(cfg);
            break;
        case AUDIO_CODEC_WORK_MODE_PLAYBACK:
            jy6311_config_play(cfg);
            break;
        case AUDIO_CODEC_WORK_MODE_CODEC:
        default:
            jy6311_config_play_record(cfg);
            break;
    }

    return 0;
}

int audio_codec_jy6311_probe(struct bflb_device_s *i2c, uint8_t addr)
{
    uint8_t pwr_ctrl1 = 0;
    uint8_t pwr_ctrl2 = 0;
    uint8_t adda_fs = 0;

    if (i2c == NULL) {
        return -1;
    }

    if (audio_codec_i2c_read_reg8(i2c, addr, PWR_CTRL1, &pwr_ctrl1) != 0) {
        return -1;
    }
    if (audio_codec_i2c_read_reg8(i2c, addr, PWR_CTRL2, &pwr_ctrl2) != 0) {
        return -1;
    }
    if (audio_codec_i2c_read_reg8(i2c, addr, ADDA_FS, &adda_fs) != 0) {
        return -1;
    }

    if (pwr_ctrl1 == 0x8c && pwr_ctrl2 == 0x48 && adda_fs == 0x25) {
        audio_codec_jy6311_bind_i2c(i2c);
        return 0;
    }

    return -1;
}

int audio_codec_jy6311_init(audio_codec_dev_t *dev, const audio_codec_cfg_t *cfg)
{
    if (dev == NULL || cfg == NULL || dev->i2c == NULL) {
        return -1;
    }

    if (!g_jy6311_ctx.initialized) {
        g_jy6311_ctx.dac_volume_reg = JY6311_DAC_DIG_VOL_0dB;
        g_jy6311_ctx.adc_volume_reg = JY6311_DAC_DIG_VOL_0dB;
        g_jy6311_ctx.adc_pga_gain = JY6311_ADC_PGA_GAIN_0dB;
        g_jy6311_ctx.dac_muted = false;
    }

    return audio_codec_jy6311_configure(dev, cfg);
}

int audio_codec_jy6311_set_volume(audio_codec_dev_t *dev, int volume_percent)
{
    uint8_t volume_reg;

    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    audio_codec_jy6311_bind_i2c(dev->i2c);
    volume_reg = audio_codec_jy6311_percent_to_dig_vol(volume_percent);
    g_jy6311_ctx.dac_volume_reg = volume_reg;

    if (g_jy6311_ctx.dac_muted) {
        return 0;
    }

    jy6311_play_vol_cfg(dev->i2c_addr, volume_reg);
    return 0;
}

static int audio_codec_jy6311_set_mute(audio_codec_dev_t *dev, bool mute)
{
    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    audio_codec_jy6311_bind_i2c(dev->i2c);
    g_jy6311_ctx.dac_muted = mute;
    jy6311_play_vol_cfg(dev->i2c_addr, mute ? 0x00 : g_jy6311_ctx.dac_volume_reg);
    return 0;
}

static int audio_codec_jy6311_set_record_volume(audio_codec_dev_t *dev, int volume_percent)
{
    uint8_t volume_reg;

    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    audio_codec_jy6311_bind_i2c(dev->i2c);
    volume_reg = audio_codec_jy6311_percent_to_dig_vol(volume_percent);
    g_jy6311_ctx.adc_volume_reg = volume_reg;
    __JY6311_ADC_DigVol_Set(dev->i2c_addr, volume_reg);
    return 0;
}

static int audio_codec_jy6311_set_mic_pga(audio_codec_dev_t *dev, int pga_percent)
{
    JY6311_ADC_PGAGainETypeDef pga_gain;

    if (dev == NULL || dev->i2c == NULL) {
        return -1;
    }

    audio_codec_jy6311_bind_i2c(dev->i2c);
    pga_gain = audio_codec_jy6311_percent_to_pga(pga_percent);
    g_jy6311_ctx.adc_pga_gain = pga_gain;
    jy6311_record_gain_cfg(dev->i2c_addr, pga_gain);
    return 0;
}

int audio_codec_jy6311_set_sample_rate(audio_codec_dev_t *dev, uint32_t sample_rate_hz)
{
    audio_codec_cfg_t cfg;

    if (dev == NULL || dev->i2c == NULL || !g_jy6311_ctx.initialized) {
        return -1;
    }

    cfg = g_jy6311_ctx.cfg;
    cfg.sample_rate_hz = sample_rate_hz;
    return audio_codec_jy6311_configure(dev, &cfg);
}

uint32_t audio_codec_jy6311_calc_mclk_hz(uint32_t sample_rate_hz)
{
    return audio_codec_jy6311_default_mclk(sample_rate_hz);
}

static const uint8_t jy6311_addrs[] = { JY6311_I2C_ADDR_0, JY6311_I2C_ADDR_1 };

const audio_codec_driver_t audio_codec_jy6311_driver = {
    .name = "JY6311",
    .i2c_addrs = jy6311_addrs,
    .probe = audio_codec_jy6311_probe,
    .init = audio_codec_jy6311_init,
    .set_mute = audio_codec_jy6311_set_mute,
    .set_volume = audio_codec_jy6311_set_volume,
    .set_record_volume = audio_codec_jy6311_set_record_volume,
    .set_mic_pga = audio_codec_jy6311_set_mic_pga,
    .set_sample_rate = audio_codec_jy6311_set_sample_rate,
    .calc_mclk_hz = audio_codec_jy6311_calc_mclk_hz,
    .type = AUDIO_CODEC_TYPE_JY6311,
    .i2c_addrs_num = (uint8_t)JY6311_ARRAY_SIZE(jy6311_addrs),
};
