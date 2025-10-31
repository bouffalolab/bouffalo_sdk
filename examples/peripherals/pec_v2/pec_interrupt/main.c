#include "board.h"
#include "bflb_pec_v2_instr.h"
#include "bflb_pec_v2.h"

struct bflb_device_s *pec;
uint32_t buffer[256];

int bflb_pec_interrupt_init(struct bflb_device_s *dev)
{
    struct bflb_pec_cfg_s cfg;
    uint8_t mem = 0;

    bflb_pec_disable(dev);
    cfg.clkdiv = 0;                                    /*!< clock divisor, frequency = clock freq / (clkdiv + 1) */
    cfg.pc_origin = mem + 0;                           /*!< beginning of the program for state machine, 0~255 is available */
    cfg.side_en = 0;                                   /*!< allow instructions to perform sideset optionally, rather than on every instruction */
    cfg.side_pindir = 0;                               /*!< side data is asserted to pin OEs or pin values */
    cfg.fifo_join = PEC_FIFO_JOIN_NONE;                /*!< if rx_fifo and tx_fifo join */
    cfg.out_dir = PEC_SHIFT_DIR_TO_LEFT;               /*!< shift OSR direction */
    cfg.in_dir = PEC_SHIFT_DIR_TO_LEFT;                /*!< shift ISR direction */
    cfg.auto_pull = 0;                                 /*!< pull automatically when the output shift register is emptied */
    cfg.auto_push = 0;                                 /*!< push automatically when the input shift register is filled */
    cfg.wrap_top = 255;                                /*!< after reaching this address, execution is wrapped to wrap_bottom */
    cfg.wrap_bottom = 0;                               /*!< ater reaching wrap_top, execution is wrapped to this address */
    cfg.dma_tx = 0;                                    /*!< enable signal of dma_tx_req/ack interface */
    cfg.dma_rx = 0;                                    /*!< enable signal of dma_rx_req/ack interface */
    cfg.thresh_bits_pull = 0;                          /*!< number of bits shifted out of TXSR before autopull or conditional pull, 1~31, 0 present 32 */
    cfg.thresh_bits_push = 0;                          /*!< number of bits shifted into RXSR before autopush or conditional push, 1~31, 0 present 32 */
    cfg.thresh_fifo_tx = 2;                            /*!< threshold of TX FIFO for interrupt and DMA, 0~15 if no join, 0~31 if rx_join_tx */
    cfg.thresh_fifo_rx = 4;                            /*!< threshold of RX FIFO for interrupt and DMA, 0~15 if no join, 0~31 if tx_join_rx */
    bflb_pec_init(dev, &cfg);

#if defined(BL616L)
    if (dev->sub_idx == 1) {
        mem += PEC_MEMORY_SIZE_0;
    }
#endif
    bflb_pec_mem_write(dev, mem + 0, pec_instr_SET(PEC_R3, 40, pec_instr_side(0, 0, 0)));
    bflb_pec_mem_write(dev, mem + 1, pec_instr_IN1S(4, pec_instr_side(0, 0, 0)));
    bflb_pec_mem_write(dev, mem + 2, pec_instr_PUSH(PEC_INSTR_PUSH_DROP, pec_instr_side(0, 0, 0)));
    bflb_pec_mem_write(dev, mem + 3, pec_instr_JRNZ(mem + 1, PEC_R3, 1, pec_instr_side(0, 0, 0)));
    bflb_pec_mem_write(dev, mem + 4, pec_instr_IPCS(31, pec_instr_side(0, 0, 0)));
    bflb_pec_mem_write(dev, mem + 5, pec_instr_GOTO(mem + 5, pec_instr_side(0, 0, 0)));

    bflb_pec_enable(dev);
    return PEC_STS_OK;
}

void pec_isr(int irq, void *arg)
{
    uint32_t status = bflb_pec_int1_status_get(pec, PEC_INT_ALL);
    printf("status = 0x%08lX\r\n", status);

    if (status & PEC_INT_SM1_RXF_RDY) {
        uint8_t length = bflb_pec_fifo_get_level_rx(pec);
        printf("rx fifo level: %u\r\n", length);
        for (uint8_t i = 0; i < length; i++) {
            printf("%08lX ", bflb_pec_fifo_read(pec));
        }
        printf("\r\n");
    }
    if (status & PEC_INT_SM1_TXF_RDY) {
        printf("tx fifo level: %u\r\n", bflb_pec_fifo_get_level_tx(pec));
        bflb_pec_fifo_write_multi_32bits(pec, buffer, 1);
    }
    if (status & PEC_INT_IPC_24_31) {
        printf("ipc 24-31\r\n");
        bflb_pec_ipc_clr_bit(pec, 31);
    }
}

int main(void)
{
    board_init();
    pec = bflb_device_get_by_name("pec_sm1");
    printf("pec = 0x%08lX\r\n", pec);

    bflb_pec_interrupt_init(pec);
    bflb_pec_ipc_int_enable_bit(pec, 31);
    bflb_pec_int1_enable(pec, PEC_INT_SM1_TXF_RDY | PEC_INT_SM1_RXF_RDY | PEC_INT_IPC_16_23 | PEC_INT_IPC_0_7 | PEC_INT_IPC_8_15 | PEC_INT_IPC_24_31);
    bflb_irq_attach(pec->irq_num + 1, pec_isr, NULL);
    bflb_irq_enable(pec->irq_num + 1);

    return 0;
}
