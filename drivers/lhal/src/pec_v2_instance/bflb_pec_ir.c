#include "bflb_pec_v2.h"
#include "bflb_pec_v2_instance.h"
#include "bflb_pec_common.h"

/*** note
+-----+-----------------------+----+----+-----+----+------------+
| reg | 31                 16 | 15 | 14 | 13  | 12 | 11       0 |
+-----+-----------------------+----+----+-----+----+------------+
| R0  | data_0 carrier length | data_0 idle length              |
+-----+-----------------------+---------------------------------+
| R1  | data_1 carrier length | data_1 idle length              |
+-----+-----------------------+---------------------------------+
| R2  | start carrier length  | start idle length               |
+-----+-----------------------+---------------------------------+
| R3  | stop carrier length   | stop idle length                |
+-----+-----------------------+----+----+-----+----+------------+
| R4  |                       |    |    | end | en | bit length |
+-----+-----------------------+----+----+-----+----+------------+
| R5  | iterator of period count                                |
+-----+---------------------------------------------------------+
| R6  | iterator of data bits                                   |
+-----+---------------------------------------------------------+
*******************************************************************************/

int bflb_pec_ir_nec_init(struct bflb_device_s *dev, struct bflb_pec_ir_s *ir)
{
    struct bflb_pec_cfg_s cfg;
    uint8_t mem, pin;
    uint8_t idle, delay_high, delay_low;
    int ret;

    ret = bflb_pec_memory_size_check(dev, ir->mem, 36, &mem);
    if (ret != PEC_STS_OK) {
        return ret;
    }

    bflb_pec_disable(dev);
    cfg.clkdiv = ir->div;                              /*!< clock divisor, frequency = clock freq / (clkdiv + 1) */
    cfg.pc_origin = mem + 0;                           /*!< beginning of the program for state machine, 0~255 is available */
    cfg.side_en = 1;                                   /*!< allow instructions to perform sideset optionally, rather than on every instruction */
    cfg.side_pindir = 0;                               /*!< side data is asserted to pin OEs or pin values */
    cfg.fifo_join = PEC_FIFO_RX_JOIN_TX;               /*!< if rx_fifo and tx_fifo join */
    cfg.out_dir = PEC_SHIFT_DIR_TO_RIGHT;              /*!< shift OSR direction */
    cfg.in_dir = PEC_SHIFT_DIR_TO_RIGHT;               /*!< shift ISR direction */
    cfg.auto_pull = 0;                                 /*!< pull automatically when the output shift register is emptied */
    cfg.auto_push = 0;                                 /*!< push automatically when the input shift register is filled */
    cfg.wrap_top = mem + 35;                           /*!< after reaching this address, execution is wrapped to wrap_bottom */
    cfg.wrap_bottom = mem + 0;                         /*!< ater reaching wrap_top, execution is wrapped to this address */
    cfg.dma_tx = 0;                                    /*!< enable signal of dma_tx_req/ack interface */
    cfg.dma_rx = 0;                                    /*!< enable signal of dma_rx_req/ack interface */
    cfg.thresh_bits_pull = 0;                          /*!< number of bits shifted out of TXSR before autopull or conditional pull, 1~31, 0 present 32 */
    cfg.thresh_bits_push = 0;                          /*!< number of bits shifted into RXSR before autopush or conditional push, 1~31, 0 present 32 */
    cfg.thresh_fifo_tx = 0;                            /*!< threshold of TX FIFO for interrupt and DMA, 0~15 if no join, 0~31 if rx_join_tx */
    cfg.thresh_fifo_rx = 0;                            /*!< threshold of RX FIFO for interrupt and DMA, 0~15 if no join, 0~31 if tx_join_rx */
    bflb_pec_init(dev, &cfg);

    pin = ir->pin % 64;
    bflb_pec_pin_side_base_set(dev, pin);
    bflb_pec_pin_side_count_set(dev, 2);
    bflb_pec_pin_out_base_set(dev, 0, pin);
    bflb_pec_pin_out_count_set(dev, 0, 1);
    if (ir->idle_level) {
        bflb_pec_pad_o_set_bit(dev, pin);
        idle = 3;
    } else {
        bflb_pec_pad_o_clr_bit(dev, pin);
        idle = 2;
    }
    bflb_pec_pad_oe_set_bit(dev, pin);
    if (ir->carrier_low > 64) {
        delay_low = 63;
    } else if (ir->carrier_low > 0) {
        delay_low = ir->carrier_low - 1;
    } else {
        delay_low = 0;
    }
    if (ir->carrier_high > 64) {
        delay_high = 63;
    } else if (ir->carrier_high > 0) {
        delay_high = ir->carrier_high - 1;
    } else {
        delay_high = 0;
    }

    bflb_pec_mem_write(dev, mem + 0, pec_instr_JBZ(mem + 0, PEC_R4, 12, PEC_INSTR_BIT_NONE, pec_instr_side(0, 0, 2)));
    /* start carrier */
    bflb_pec_mem_write(dev, mem + 1, pec_instr_MOV(PEC_INSTR_MOV_DST_R5, PEC_INSTR_MOV_SRC_R2, 16, 16, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 2, pec_instr_NOP(PEC_R5, pec_instr_side(3, delay_high, 2)));
    bflb_pec_mem_write(dev, mem + 3, pec_instr_JRNZ(mem + 2, PEC_R5, PEC_INSTR_CMP_DEC, pec_instr_side(2, delay_low, 2)));
    /* start idle */
    bflb_pec_mem_write(dev, mem + 4, pec_instr_MOV(PEC_INSTR_MOV_DST_R5, PEC_INSTR_MOV_SRC_R2, 0, 16, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 5, pec_instr_NOP(PEC_R5, pec_instr_side(idle, delay_high, 2)));
    bflb_pec_mem_write(dev, mem + 6, pec_instr_JRNZ(mem + 5, PEC_R5, PEC_INSTR_CMP_DEC, pec_instr_side(idle, delay_low, 2)));
    /* judge whether send repetition code */
    bflb_pec_mem_write(dev, mem + 7, pec_instr_MOV(PEC_INSTR_MOV_DST_R6, PEC_INSTR_MOV_SRC_R4, 0, 12, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 8, pec_instr_JRZ(mem + 28, PEC_R6, PEC_INSTR_CMP_DEC, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 9, pec_instr_PULL(PEC_INSTR_PULL_0S, pec_instr_side(idle, 0, 2)));
    /* every data bit */
    bflb_pec_mem_write(dev, mem + 10, pec_instr_PULLE(PEC_INSTR_PULL_0S, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 11, pec_instr_OUTREG(PEC_R5, 1, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 12, pec_instr_JRNZ(mem + 21, PEC_R5, PEC_INSTR_CMP_NONE, pec_instr_side(idle, 0, 2)));
    /* data_0 carrier */
    bflb_pec_mem_write(dev, mem + 13, pec_instr_MOV(PEC_INSTR_MOV_DST_R5, PEC_INSTR_MOV_SRC_R0, 16, 16, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 14, pec_instr_NOP(PEC_R5, pec_instr_side(3, delay_high, 2)));
    bflb_pec_mem_write(dev, mem + 15, pec_instr_JRNZ(mem + 14, PEC_R5, PEC_INSTR_CMP_DEC, pec_instr_side(2, delay_low, 2)));
    /* data_0 idle */
    bflb_pec_mem_write(dev, mem + 16, pec_instr_MOV(PEC_INSTR_MOV_DST_R5, PEC_INSTR_MOV_SRC_R0, 0, 16, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 17, pec_instr_NOP(PEC_R5, pec_instr_side(idle, delay_high, 2)));
    bflb_pec_mem_write(dev, mem + 18, pec_instr_JRNZ(mem + 17, PEC_R5, PEC_INSTR_CMP_DEC, pec_instr_side(idle, delay_low, 2)));
    bflb_pec_mem_write(dev, mem + 19, pec_instr_JRNZ(mem + 10, PEC_R6, PEC_INSTR_CMP_DEC, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 20, pec_instr_GOTO(mem + 28, pec_instr_side(idle, 0, 2)));
    /* data_1 carrier */
    bflb_pec_mem_write(dev, mem + 21, pec_instr_MOV(PEC_INSTR_MOV_DST_R5, PEC_INSTR_MOV_SRC_R1, 16, 16, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 22, pec_instr_NOP(PEC_R5, pec_instr_side(3, delay_high, 2)));
    bflb_pec_mem_write(dev, mem + 23, pec_instr_JRNZ(mem + 22, PEC_R5, PEC_INSTR_CMP_DEC, pec_instr_side(2, delay_low, 2)));
    /* data_1 idle */
    bflb_pec_mem_write(dev, mem + 24, pec_instr_MOV(PEC_INSTR_MOV_DST_R5, PEC_INSTR_MOV_SRC_R1, 0, 16, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 25, pec_instr_NOP(PEC_R5, pec_instr_side(idle, delay_high, 2)));
    bflb_pec_mem_write(dev, mem + 26, pec_instr_JRNZ(mem + 25, PEC_R5, PEC_INSTR_CMP_DEC, pec_instr_side(idle, delay_low, 2)));
    bflb_pec_mem_write(dev, mem + 27, pec_instr_JRNZ(mem + 10, PEC_R6, PEC_INSTR_CMP_DEC, pec_instr_side(idle, 0, 2)));
    /* stop carrier */
    bflb_pec_mem_write(dev, mem + 28, pec_instr_MOV(PEC_INSTR_MOV_DST_R5, PEC_INSTR_MOV_SRC_R3, 16, 16, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 29, pec_instr_NOP(PEC_R5, pec_instr_side(3, delay_high, 2)));
    bflb_pec_mem_write(dev, mem + 30, pec_instr_JRNZ(mem + 29, PEC_R5, PEC_INSTR_CMP_DEC, pec_instr_side(2, delay_low, 2)));
    /* stop idle */
    bflb_pec_mem_write(dev, mem + 31, pec_instr_MOV(PEC_INSTR_MOV_DST_R5, PEC_INSTR_MOV_SRC_R3, 0, 16, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 32, pec_instr_NOP(PEC_R5, pec_instr_side(idle, delay_high, 2)));
    bflb_pec_mem_write(dev, mem + 33, pec_instr_JRNZ(mem + 32, PEC_R5, PEC_INSTR_CMP_DEC, pec_instr_side(idle, delay_low, 2)));
    /* frame end */
    bflb_pec_mem_write(dev, mem + 34, pec_instr_BCLRI(PEC_R4, PEC_R4, 12, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 35, pec_instr_BSETI(PEC_R4, PEC_R4, 13, pec_instr_side(idle, 0, 2)));

    bflb_pec_enable(dev);
    bflb_pec_sm_reg_set_bit(dev, PEC_R4, 13);
    return PEC_STS_OK;
}

void bflb_pec_ir_nec_set_timing(struct bflb_device_s *dev, struct bflb_pec_ir_nec_timing_s *nec_timing)
{
    uint32_t val;

    val = nec_timing->data0_carrier;
    val <<= 16;
    val |= nec_timing->data0_idle;
    bflb_pec_sm_reg_write(dev, PEC_R0, val);
    val = nec_timing->data1_carrier;
    val <<= 16;
    val |= nec_timing->data1_idle;
    bflb_pec_sm_reg_write(dev, PEC_R1, val);
    val = nec_timing->start_carrier;
    val <<= 16;
    val |= nec_timing->start_idle;
    bflb_pec_sm_reg_write(dev, PEC_R2, val);
    val = nec_timing->stop_carrier;
    val <<= 16;
    val |= nec_timing->stop_idle;
    bflb_pec_sm_reg_write(dev, PEC_R3, val);
}

void bflb_pec_ir_nec_send(struct bflb_device_s *dev, uint32_t *data, uint32_t bits)
{
    uint32_t val;

    if (bits > 32 * PEC_FIFO_DEEPTH * 2) {
        bits = 32 * PEC_FIFO_DEEPTH * 2;
    }
    /* en = 0 */
    bflb_pec_sm_reg_clr_bit(dev, PEC_R4, 12);
    /* set bit count */
    val = bits | (((~bits) & 0xFFF) << 16);
    bflb_pec_sm_regl_rs(dev, PEC_R4, val);
    /* flag_end = 0 */
    bflb_pec_sm_reg_clr_bit(dev, PEC_R4, 13);
    /* clear fifo */
    bflb_pec_fifo_clr_tx(dev);
    bits = (bits + 31) / 32;
    for (uint32_t i = 0; i < bits; i++) {
        bflb_pec_fifo_write(dev, data[i]);
    }
    /* en = 1 */
    bflb_pec_sm_reg_set_bit(dev, PEC_R4, 12);
}

int bflb_pec_ir_nec_is_busy(struct bflb_device_s *dev)
{
    uint8_t mem, addr;
    uint32_t flag_end;

    mem = bflb_pec_mem_restore(dev);
    addr = bflb_pec_current_pc_get(dev);
    flag_end = bflb_pec_sm_reg_read(dev, PEC_R4);
    flag_end &= (1 << 13);
    if ((addr == mem) && (flag_end != 0)) {
        return 0;
    } else {
        return 1;
    }
}

/*** note
+-----+-----------------------+----+----+-----+----+------------+
| reg | 31                 16 | 15 | 14 | 13  | 12 | 11       0 |
+-----+-----------------------+----+----+-----+----+------------+
| R0  | data_0 carrier length | data_0 idle length              |
+-----+-----------------------+---------------------------------+
| R1  | data_1 carrier length | data_1 idle length              |
+-----+-----------------------+---------------------------------+
| R2  |                                                         |
+-----+---------------------------------------------------------+
| R3  |                                                         |
+-----+-----------------------+----+----+-----+----+------------+
| R4  |                       |    |    | end | en | bit length |
+-----+-----------------------+----+----+-----+----+------------+
| R5  | iterator of period count                                |
+-----+---------------------------------------------------------+
| R6  | iterator of data bits                                   |
+-----+---------------------------------------------------------+
*******************************************************************************/

int bflb_pec_ir_rc5_init(struct bflb_device_s *dev, struct bflb_pec_ir_s *ir)
{
    struct bflb_pec_cfg_s cfg;
    uint8_t mem, pin;
    uint8_t idle, delay_high, delay_low;
    int ret;

    ret = bflb_pec_memory_size_check(dev, ir->mem, 29, &mem);
    if (ret != PEC_STS_OK) {
        return ret;
    }

    bflb_pec_disable(dev);
    cfg.clkdiv = ir->div;                              /*!< clock divisor, frequency = clock freq / (clkdiv + 1) */
    cfg.pc_origin = mem + 0;                           /*!< beginning of the program for state machine, 0~255 is available */
    cfg.side_en = 1;                                   /*!< allow instructions to perform sideset optionally, rather than on every instruction */
    cfg.side_pindir = 0;                               /*!< side data is asserted to pin OEs or pin values */
    cfg.fifo_join = PEC_FIFO_RX_JOIN_TX;               /*!< if rx_fifo and tx_fifo join */
    cfg.out_dir = PEC_SHIFT_DIR_TO_LEFT;               /*!< shift OSR direction */
    cfg.in_dir = PEC_SHIFT_DIR_TO_LEFT;                /*!< shift ISR direction */
    cfg.auto_pull = 0;                                 /*!< pull automatically when the output shift register is emptied */
    cfg.auto_push = 0;                                 /*!< push automatically when the input shift register is filled */
    cfg.wrap_top = mem + 28;                           /*!< after reaching this address, execution is wrapped to wrap_bottom */
    cfg.wrap_bottom = mem + 0;                         /*!< ater reaching wrap_top, execution is wrapped to this address */
    cfg.dma_tx = 0;                                    /*!< enable signal of dma_tx_req/ack interface */
    cfg.dma_rx = 0;                                    /*!< enable signal of dma_rx_req/ack interface */
    cfg.thresh_bits_pull = 0;                          /*!< number of bits shifted out of TXSR before autopull or conditional pull, 1~31, 0 present 32 */
    cfg.thresh_bits_push = 0;                          /*!< number of bits shifted into RXSR before autopush or conditional push, 1~31, 0 present 32 */
    cfg.thresh_fifo_tx = 0;                            /*!< threshold of TX FIFO for interrupt and DMA, 0~15 if no join, 0~31 if rx_join_tx */
    cfg.thresh_fifo_rx = 0;                            /*!< threshold of RX FIFO for interrupt and DMA, 0~15 if no join, 0~31 if tx_join_rx */
    bflb_pec_init(dev, &cfg);

    pin = ir->pin % 64;
    bflb_pec_pin_side_base_set(dev, pin);
    bflb_pec_pin_side_count_set(dev, 2);
    bflb_pec_pin_out_base_set(dev, 0, pin);
    bflb_pec_pin_out_count_set(dev, 0, 1);
    if (ir->idle_level) {
        bflb_pec_pad_o_set_bit(dev, pin);
        idle = 3;
    } else {
        bflb_pec_pad_o_clr_bit(dev, pin);
        idle = 2;
    }
    bflb_pec_pad_oe_set_bit(dev, pin);
    if (ir->carrier_low > 64) {
        delay_low = 63;
    } else if (ir->carrier_low > 0) {
        delay_low = ir->carrier_low - 1;
    } else {
        delay_low = 0;
    }
    if (ir->carrier_high > 64) {
        delay_high = 63;
    } else if (ir->carrier_high > 0) {
        delay_high = ir->carrier_high - 1;
    } else {
        delay_high = 0;
    }

    bflb_pec_mem_write(dev, mem + 0, pec_instr_JBZ(mem + 0, PEC_R4, 12, PEC_INSTR_BIT_NONE, pec_instr_side(0, 0, 2)));
    /* judge whether send repetition code */
    bflb_pec_mem_write(dev, mem + 1, pec_instr_MOV(PEC_INSTR_MOV_DST_R6, PEC_INSTR_MOV_SRC_R4, 0, 12, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 2, pec_instr_JRZ(mem + 27, PEC_R6, PEC_INSTR_CMP_DEC, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 3, pec_instr_PULL(PEC_INSTR_PULL_0S, pec_instr_side(idle, 0, 2)));
    /* every data bit */
    bflb_pec_mem_write(dev, mem + 4, pec_instr_PULLE(PEC_INSTR_PULL_0S, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 5, pec_instr_OUTREG(PEC_R5, 1, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 6, pec_instr_JRNZ(mem + 15, PEC_R5, PEC_INSTR_CMP_NONE, pec_instr_side(idle, 0, 2)));
    /* data_0 carrier */
    bflb_pec_mem_write(dev, mem + 7, pec_instr_MOV(PEC_INSTR_MOV_DST_R5, PEC_INSTR_MOV_SRC_R0, 16, 16, pec_instr_side(3, delay_high, 2)));
    bflb_pec_mem_write(dev, mem + 8, pec_instr_NOP(PEC_R5, pec_instr_side(2, delay_low, 2)));
    bflb_pec_mem_write(dev, mem + 9, pec_instr_JRNZ(mem + 8, PEC_R5, PEC_INSTR_CMP_DEC, pec_instr_side(3, delay_high, 2)));
    /* data_0 idle */
    bflb_pec_mem_write(dev, mem + 10, pec_instr_MOV(PEC_INSTR_MOV_DST_R5, PEC_INSTR_MOV_SRC_R0, 0, 16, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 11, pec_instr_NOP(PEC_R5, pec_instr_side(idle, delay_high, 2)));
    bflb_pec_mem_write(dev, mem + 12, pec_instr_JRNZ(mem + 11, PEC_R5, PEC_INSTR_CMP_DEC, pec_instr_side(idle, delay_low, 2)));
    bflb_pec_mem_write(dev, mem + 13, pec_instr_JRNZ(mem + 4, PEC_R6, PEC_INSTR_CMP_DEC, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 14, pec_instr_GOTO(mem + 27, pec_instr_side(idle, 0, 2)));
    /* data_1 idle */
    bflb_pec_mem_write(dev, mem + 15, pec_instr_MOV(PEC_INSTR_MOV_DST_R5, PEC_INSTR_MOV_SRC_R1, 0, 16, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 16, pec_instr_NOP(PEC_R5, pec_instr_side(idle, delay_high, 2)));
    bflb_pec_mem_write(dev, mem + 17, pec_instr_JRNZ(mem + 16, PEC_R5, PEC_INSTR_CMP_DEC, pec_instr_side(idle, delay_low, 2)));
    /* data_1 carrier */
    bflb_pec_mem_write(dev, mem + 18, pec_instr_MOV(PEC_INSTR_MOV_DST_R5, PEC_INSTR_MOV_SRC_R1, 16, 16, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 19, pec_instr_NOP(PEC_R5, pec_instr_side(3, delay_high, 2)));
    bflb_pec_mem_write(dev, mem + 20, pec_instr_JRNZ(mem + 19, PEC_R5, PEC_INSTR_CMP_DEC, pec_instr_side(2, delay_low, 2)));
    /* is all bits complete */
    bflb_pec_mem_write(dev, mem + 21, pec_instr_JRNZ(mem + 23, PEC_R6, PEC_INSTR_CMP_DEC, pec_instr_side(3, delay_high, 2)));
    bflb_pec_mem_write(dev, mem + 22, pec_instr_GOTO(mem + 27, pec_instr_side(idle, 0, 2)));
    /* loop for every bit after bit 1 */
    bflb_pec_mem_write(dev, mem + 23, pec_instr_PULLE(PEC_INSTR_PULL_0S, pec_instr_side(2, delay_low, 2)));
    bflb_pec_mem_write(dev, mem + 24, pec_instr_OUTREG(PEC_R5, 1, pec_instr_side(3, delay_high, 2)));
    bflb_pec_mem_write(dev, mem + 25, pec_instr_JRZ(mem + 7, PEC_R5, PEC_INSTR_CMP_NONE, pec_instr_side(2, delay_low, 2)));
    bflb_pec_mem_write(dev, mem + 26, pec_instr_GOTO(mem + 15, pec_instr_side(idle, 0, 2)));
    /* frame end */
    bflb_pec_mem_write(dev, mem + 27, pec_instr_BCLRI(PEC_R4, PEC_R4, 12, pec_instr_side(idle, 0, 2)));
    bflb_pec_mem_write(dev, mem + 28, pec_instr_BSETI(PEC_R4, PEC_R4, 13, pec_instr_side(idle, 0, 2)));

    bflb_pec_enable(dev);
    bflb_pec_sm_reg_set_bit(dev, PEC_R4, 13);
    return PEC_STS_OK;
}

void bflb_pec_ir_rc5_set_timing(struct bflb_device_s *dev, struct bflb_pec_ir_rc5_timing_s *rc5_timing)
{
    uint32_t val;

    val = rc5_timing->data0_carrier;
    val <<= 16;
    val |= rc5_timing->data0_idle;
    bflb_pec_sm_reg_write(dev, PEC_R0, val);
    val = rc5_timing->data1_carrier;
    val <<= 16;
    val |= rc5_timing->data1_idle;
    bflb_pec_sm_reg_write(dev, PEC_R1, val);
}

void bflb_pec_ir_rc5_send(struct bflb_device_s *dev, uint32_t *data, uint32_t bits)
{
    uint32_t val;

    if (bits > 32 * PEC_FIFO_DEEPTH * 2) {
        bits = 32 * PEC_FIFO_DEEPTH * 2;
    }
    /* en = 0 */
    bflb_pec_sm_reg_clr_bit(dev, PEC_R4, 12);
    /* set bit count */
    val = bits | (((~bits) & 0xFFF) << 16);
    bflb_pec_sm_regl_rs(dev, PEC_R4, val);
    /* flag_end = 0 */
    bflb_pec_sm_reg_clr_bit(dev, PEC_R4, 13);
    /* clear fifo */
    bflb_pec_fifo_clr_tx(dev);
    bits = (bits + 31) / 32;
    for (uint32_t i = 0; i < bits; i++) {
        bflb_pec_fifo_write(dev, data[i]);
    }
    /* en = 1 */
    bflb_pec_sm_reg_set_bit(dev, PEC_R4, 12);
}

int bflb_pec_ir_rc5_is_busy(struct bflb_device_s *dev)
{
    uint8_t mem, addr;
    uint32_t flag_end;

    mem = bflb_pec_mem_restore(dev);
    addr = bflb_pec_current_pc_get(dev);
    flag_end = bflb_pec_sm_reg_read(dev, PEC_R4);
    flag_end &= (1 << 13);
    if ((addr == mem) && (flag_end != 0)) {
        return 0;
    } else {
        return 1;
    }
}
