#include "board.h"

#include "bflb_clock.h"
#include "bflb_mtimer.h"
#include "bflb_i2s.h"
#include "bflb_i2c.h"
#include "bflb_dma.h"
#include "bflb_gpio.h"

#include "bl616_glb.h"

#include "fhm_onechannel_16k_20.h"
#include "bsp_wm8978.h"

#define DBG_TAG "MAIN"
#include "log.h"

struct bflb_device_s *gpio;
struct bflb_device_s *i2s0;
struct bflb_device_s *i2c0;
struct bflb_device_s *dma0_ch0;

volatile uint8_t dma_tc_flag0 = 0;
static struct bflb_dma_channel_lli_pool_s tx_llipool[100];

void dma0_ch0_isr(void *arg)
{
    dma_tc_flag0++;
    LOG_I("tc done\r\n");
}

void i2s_dma_init()
{
    static struct bflb_dma_channel_lli_transfer_s tx_transfers[1];

    struct bflb_i2s_config_s i2s_cfg = {
        .bclk_freq_hz = 16000 * 32 * 2, /* bclk = Sampling_rate * frame_width * channel_num */
        .role = I2S_ROLE_MASTER,
        .format_mode = I2S_MODE_LEFT_JUSTIFIED,
        .channel_mode = I2S_CHANNEL_MODE_NUM_1,
        .frame_width = I2S_SLOT_WIDTH_32,
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

    LOG_I("i2s init\r\n");
    i2s0 = bflb_device_get_by_name("i2s0");
    /* i2s init */
    bflb_i2s_init(i2s0, &i2s_cfg);
    /* enable dma */
    bflb_i2s_link_txdma(i2s0, true);
    bflb_i2s_link_rxdma(i2s0, true);

    /* I2S_FS */
    bflb_gpio_init(gpio, GPIO_PIN_13, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    /* I2S_DI */
    bflb_gpio_init(gpio, GPIO_PIN_14, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    /* I2S_DO */
    bflb_gpio_init(gpio, GPIO_PIN_15, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    /* I2S_BCLK */
    bflb_gpio_init(gpio, GPIO_PIN_20, GPIO_FUNC_I2S | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);

    LOG_I("dma init\r\n");
    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");
    bflb_dma_channel_init(dma0_ch0, &tx_config);
    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);

    tx_transfers[0].src_addr = (uint32_t)fhm_onechannel_16k_20;
    tx_transfers[0].dst_addr = (uint32_t)DMA_ADDR_I2S_TDR;
    tx_transfers[0].nbytes = sizeof(fhm_onechannel_16k_20);

    LOG_I("dma lli init\r\n");
    uint32_t num = bflb_dma_channel_lli_reload(dma0_ch0, tx_llipool, 100, tx_transfers, 1);
    bflb_dma_channel_lli_link_head(dma0_ch0, tx_llipool, num);
    LOG_I("dma lli num: %d \r\n", num);
    bflb_dma_channel_start(dma0_ch0);
}

void mclk_out_init()
{
#ifdef BL616
    GLB_Config_AUDIO_PLL_To_491P52M();
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_AUDIO);

    /*!< output MCLK,
        Will change the clock source of i2s,
        It needs to be called before i2s is initialized
        clock source 24.576M */

    /*!< MCLK = 24.576 / (5+1) = 4.096MHz */
    GLB_Set_I2S_CLK(ENABLE, 5, GLB_I2S_DI_SEL_I2S_DI_INPUT, GLB_I2S_DO_SEL_I2S_DO_OUTPT);
    GLB_Set_Chip_Clock_Out3_Sel(GLB_CHIP_CLK_OUT_3_I2S_REF_CLK);
#endif

    /* MCLK CLKOUT */
    bflb_gpio_init(gpio, GPIO_PIN_23, GPIO_FUNC_CLKOUT | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void wm8978_init(void)
{
    /* I2C0_SCL */
    bflb_gpio_init(gpio, GPIO_PIN_10, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    /* I2C0_SDA */
    bflb_gpio_init(gpio, GPIO_PIN_11, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);

    i2c0 = bflb_device_get_by_name("i2c0");
    _ASSERT_PARAM(NULL != i2c0);

    bflb_i2c_init(i2c0, 100000);

    _ASSERT_FUNC(0 == bsp_wm8978_init(i2c0));

    /*!< enable dac, disable adc */
    bsp_wm8978_config_adda(1, 0);

    /*!< disable all input */
    bsp_wm8978_config_input(0, 0, 0);

    /*!< enable dac output */
    bsp_wm8978_config_output(1, 0);

    bsp_wm8978_setvol_headset(40, 40);
    bsp_wm8978_setvol_speaker(40);

    bsp_wm8978_setgain_mic(0);

    /*!< config i2s mode */
    bsp_wm8978_config_i2s(1, 0);
}

int main(void)
{
    board_init();

    gpio = bflb_device_get_by_name("gpio");

    LOG_I("\r\ni2s wm8988 play test\r\n");

    /* init wm8978 Codec */
    LOG_I("wm8978 init\r\n");
    wm8978_init();
    LOG_I("wm8978 init success\r\n");

    /* mclk clkout init */
    mclk_out_init();

    /* i2s init */
    i2s_dma_init();

    /* enable i2s tx and rx */
    bflb_i2s_feature_control(i2s0, I2S_CMD_DATA_ENABLE, I2S_CMD_DATA_ENABLE_TX | I2S_CMD_DATA_ENABLE_RX);

    LOG_I("test end\r\n");

    while (1) {
        bflb_mtimer_delay_ms(10);
    }
}
