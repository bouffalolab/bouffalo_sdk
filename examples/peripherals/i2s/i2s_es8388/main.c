#include "board.h"

#include "bflb_clock.h"
#include "bflb_mtimer.h"
#include "bflb_i2s.h"
#include "bflb_i2c.h"
#include "bflb_dma.h"
#include "bflb_gpio.h"
#include "board_gpio.h"
#include "fhm_onechannel_16k_20.h"
#include "bsp_es8388.h"

struct bflb_device_s *i2s0;
struct bflb_device_s *i2c0;
struct bflb_device_s *dma0_ch0;

volatile uint8_t dma_tc_flag0 = 0;
static struct bflb_dma_channel_lli_pool_s tx_llipool[100];

static ES8388_Cfg_Type ES8388Cfg = {
    .work_mode = ES8388_CODEC_MDOE,          /*!< ES8388 work mode */
    .role = ES8388_SLAVE,                    /*!< ES8388 role */
    .mic_input_mode = ES8388_DIFF_ENDED_MIC, /*!< ES8388 mic input mode */
    .mic_pga = ES8388_MIC_PGA_3DB,           /*!< ES8388 mic PGA */
    .i2s_frame = ES8388_LEFT_JUSTIFY_FRAME,  /*!< ES8388 I2S frame */
    .data_width = ES8388_DATA_LEN_16,        /*!< ES8388 I2S dataWitdh */
};

void dma0_ch0_isr(void *arg)
{
    dma_tc_flag0++;
    printf("tc done\r\n");
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

    printf("i2s init\r\n");
    i2s0 = bflb_device_get_by_name("i2s0");
    /* i2s init */
    bflb_i2s_init(i2s0, &i2s_cfg);
    /* enable dma */
    bflb_i2s_link_txdma(i2s0, true);
    bflb_i2s_link_rxdma(i2s0, true);

    printf("dma init\r\n");
    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");
    bflb_dma_channel_init(dma0_ch0, &tx_config);
    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);

    tx_transfers[0].src_addr = (uint32_t)fhm_onechannel_16k_20;
    tx_transfers[0].dst_addr = (uint32_t)DMA_ADDR_I2S_TDR;
    tx_transfers[0].nbytes = sizeof(fhm_onechannel_16k_20);

    printf("dma lli init\r\n");
    uint32_t num = bflb_dma_channel_lli_reload(dma0_ch0, tx_llipool, 100, tx_transfers, 1);
    bflb_dma_channel_lli_link_head(dma0_ch0, tx_llipool, num);
    printf("dma lli num: %d \r\n", num);
    bflb_dma_channel_start(dma0_ch0);
}

int main(void)
{
    board_init();
    bflb_mtimer_delay_ms(100);
    printf("i2s dma test\n\r");

    /* gpio init */
    board_i2s_codec_gpio_init();

    /* init ES8388 Codec */
    printf("es8388 init\n\r");
    ES8388_Init(&ES8388Cfg);
    ES8388_Set_Voice_Volume(50);

    /* i2s init */
    i2s_dma_init();

    /* enable i2s tx and rx */
    bflb_i2s_feature_control(i2s0, I2S_CMD_DATA_ENABLE, I2S_CMD_DATA_ENABLE_TX | I2S_CMD_DATA_ENABLE_RX);

    printf("test end\n\r");

    while (1) {
        bflb_mtimer_delay_ms(10);
    }
}
