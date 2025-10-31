#include "bflb_ir.h"
#include "bflb_dma.h"
#include "bflb_mtimer.h"
#include "board.h"

struct bflb_device_s *irtx;
struct bflb_device_s *dma0_ch0;

static ATTR_NOCACHE_RAM_SECTION uint32_t tx_buffer[128];
static volatile uint8_t dma_tc_flag0 = 0;

struct bflb_dma_channel_lli_pool_s tx_llipool[1];

void dma0_ch0_isr(void *arg)
{
    dma_tc_flag0++;
    printf("tc done\r\n");
}

/* main */
int main(void)
{
    uint32_t i;
    struct bflb_dma_channel_lli_transfer_s tx_transfers[1];

    struct bflb_ir_tx_config_s tx_cfg = {
        .tx_mode = IR_TX_CUSTOMIZE,
        .data_bits = 0,
        .tail_inverse = 0,
        .tail_enable = 0,
        .head_inverse = 0,
        .head_enable = 0,
        .logic1_inverse = 1,
        .logic0_inverse = 1,
        .data_enable = 1,
        .swm_enable = 0,
        .output_modulation = 1,
        .output_inverse = 0,
        .freerun_enable = 1,
        .continue_enable = 1,
        .fifo_width = IR_TX_FIFO_WIDTH_24BIT,
        .fifo_threshold = 1,
        .logic0_pulse_width_1 = 0,
        .logic0_pulse_width_0 = 0,
        .logic1_pulse_width_1 = 2,
        .logic1_pulse_width_0 = 0,
        .head_pulse_width_1 = 0,
        .head_pulse_width_0 = 0,
        .tail_pulse_width_1 = 0,
        .tail_pulse_width_0 = 0,
        .modu_width_1 = 17,
        .modu_width_0 = 34,
        .pulse_width_unit = 1124,
    };

    struct bflb_dma_channel_config_s dma_config = {
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_IR_TX,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_32BIT,
        .dst_width = DMA_DATA_WIDTH_32BIT,
    };

    board_init();

    printf("IR TX DMA case:\r\n");

    board_ir_gpio_init();

    irtx = bflb_device_get_by_name("irtx");

    /* TX init */
    bflb_ir_tx_init(irtx, &tx_cfg);
    bflb_ir_link_txdma(irtx, true);
    bflb_ir_tx_enable(irtx, true);

    for (i = 0; i < 128; i++) {
        tx_buffer[i] = i * 0x01010101;
    }

    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");
    bflb_dma_channel_init(dma0_ch0, &dma_config);
    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);

    tx_transfers[0].src_addr = (uint32_t)tx_buffer;
    tx_transfers[0].dst_addr = (uint32_t)DMA_ADDR_IR_TDR;
    tx_transfers[0].nbytes = 128 * 4;
    bflb_dma_channel_lli_reload(dma0_ch0, tx_llipool, 1, tx_transfers, 1);
    bflb_dma_channel_start(dma0_ch0);

    while (dma_tc_flag0 != 1) {
        bflb_mtimer_delay_ms(1);
    }
    printf("Check wave\r\n");

    printf("end\r\n");

    while (1) {
    }
}
