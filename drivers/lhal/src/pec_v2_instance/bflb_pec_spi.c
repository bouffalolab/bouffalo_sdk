#include "bflb_pec_v2.h"
#include "bflb_pec_v2_instance.h"
#include "bflb_pec_common.h"
#include "bflb_clock.h"

/*** note
+-----+------------------------+-----------+------+------+------+
| reg | 31                   8 |     7     |  6   |  5   | 4  0 |
+-----+------------------------+-----------+------+------+------+
| R0  |                        | CPOL^CPHA | CPOL | CPHA | bits |
+-----+------------------------+-----------+------+------+------+
| R1  |                                                         |
+-----+---------------------------------------------------------+
| R2  | iterator of bits                                        |
+-----+---------------------------------------------------------+
| R3  | temporary register                                      |
+-----+---------------------------------------------------------+
| R4  |                                                         |
+-----+---------------------------------------------------------+
| R5  |                                                         |
+-----+---------------------------------------------------------+
*******************************************************************************/

int bflb_pec_spi_init(struct bflb_device_s *dev, struct bflb_pec_spi_s *spi)
{
    struct bflb_pec_cfg_s cfg;
    uint8_t mem, pin;
    uint32_t clk;
    int ret;

    ret = bflb_pec_memory_size_check(dev, spi->mem, 18, &mem);
    if (ret != PEC_STS_OK) {
        return ret;
    }

    clk = bflb_clk_get_peripheral_clock(BFLB_DEVICE_TYPE_PEC, dev->idx) / 8 / spi->freq;
    bflb_pec_disable(dev);
    cfg.clkdiv = (clk > 0) ? (clk - 1) : 0;            /*!< clock divisor, frequency = clock freq / (clkdiv + 1) */
    cfg.pc_origin = mem + 0;                           /*!< beginning of the program for state machine, 0~255 is available */
    cfg.side_en = 1;                                   /*!< allow instructions to perform sideset optionally, rather than on every instruction */
    cfg.side_pindir = 0;                               /*!< side data is asserted to pin OEs or pin values */
    cfg.fifo_join = PEC_FIFO_JOIN_NONE;                /*!< if rx_fifo and tx_fifo join */
    cfg.out_dir = PEC_SHIFT_DIR_TO_LEFT;               /*!< shift OSR direction */
    cfg.in_dir = PEC_SHIFT_DIR_TO_LEFT;                /*!< shift ISR direction */
    cfg.auto_pull = 0;                                 /*!< pull automatically when the output shift register is emptied */
    cfg.auto_push = 0;                                 /*!< push automatically when the input shift register is filled */
    cfg.wrap_top = mem + 10;                           /*!< after reaching this address, execution is wrapped to wrap_bottom */
    cfg.wrap_bottom = mem + 16;                        /*!< ater reaching wrap_top, execution is wrapped to this address */
    cfg.dma_tx = 0;                                    /*!< enable signal of dma_tx_req/ack interface */
    cfg.dma_rx = 0;                                    /*!< enable signal of dma_rx_req/ack interface */
    cfg.thresh_bits_pull = 32;                         /*!< number of bits shifted out of TXSR before autopull or conditional pull, 1~31, 0 present 32 */
    cfg.thresh_bits_push = 32;                         /*!< number of bits shifted into RXSR before autopush or conditional push, 1~31, 0 present 32 */
    cfg.thresh_fifo_tx = 0;                            /*!< threshold of TX FIFO for interrupt and DMA, 0~15 if no join, 0~31 if rx_join_tx */
    cfg.thresh_fifo_rx = 0;                            /*!< threshold of RX FIFO for interrupt and DMA, 0~15 if no join, 0~31 if tx_join_rx */
    bflb_pec_init(dev, &cfg);

    pin = spi->pin_sck % 64;
    bflb_pec_pin_side_base_set(dev, pin);
    bflb_pec_pin_side_count_set(dev, 2);
    bflb_pec_pin_out_base_set(dev, 1, pin);
    bflb_pec_pin_out_count_set(dev, 1, 1);
    if (spi->cpol) {
        bflb_pec_pad_o_set_bit(dev, pin);
    } else {
        bflb_pec_pad_o_clr_bit(dev, pin);
    }
    bflb_pec_pad_oe_set_bit(dev, pin);
    pin = spi->pin_mosi % 64;
    bflb_pec_pin_out_base_set(dev, 0, pin);
    bflb_pec_pin_out_count_set(dev, 0, 1);
    bflb_pec_pad_o_clr_bit(dev, pin);
    bflb_pec_pad_oe_set_bit(dev, pin);
    pin = spi->pin_miso % 64;
    bflb_pec_pin_in_base_set(dev, 0, pin);
    bflb_pec_pad_oe_clr_bit(dev, pin);

    bflb_pec_mem_write(dev, mem + 0, pec_instr_PULLB(pec_instr_side(0, 0, 2)));
    bflb_pec_mem_write(dev, mem + 1, pec_instr_MOV(PEC_INSTR_MOV_DST_R2, PEC_INSTR_MOV_SRC_R0, 0, 5, pec_instr_side(0, 0, 2)));
    bflb_pec_mem_write(dev, mem + 2, pec_instr_SET(PEC_R3, 31, pec_instr_side(0, 0, 2)));
    bflb_pec_mem_write(dev, mem + 3, pec_instr_SUB(PEC_R3, PEC_R3, PEC_R2, pec_instr_side(0, 0, 2)));
    bflb_pec_mem_write(dev, mem + 4, pec_instr_SLL(PEC_OSR, PEC_OSR, PEC_R3, pec_instr_side(0, 0, 2)));
    bflb_pec_mem_write(dev, mem + 5, pec_instr_JBNZ(mem + 11, PEC_R0, 7, PEC_INSTR_BIT_NONE, pec_instr_side(0, 0, 2)));
    /* mode == 0 or 3 */
    bflb_pec_mem_write(dev, mem + 6, pec_instr_OUTPINVAL(PEC_PIN_OUT_0, 1, pec_instr_side(2, 3, 2)));
    bflb_pec_mem_write(dev, mem + 7, pec_instr_NOP(PEC_R2, pec_instr_side(3, 1, 2)));
    bflb_pec_mem_write(dev, mem + 8, pec_instr_INPINVAL(PEC_PIN_IN_0, 1, pec_instr_side(0, 0, 2)));
    bflb_pec_mem_write(dev, mem + 9, pec_instr_JRNZ(mem + 6, PEC_R2, PEC_INSTR_CMP_DEC, pec_instr_side(0, 0, 2)));
    bflb_pec_mem_write(dev, mem + 10, pec_instr_MOV(PEC_INSTR_MOV_DST_PINVAL_1, PEC_R0, 6, 1, pec_instr_side(0, 0, 2)));
    /* mode == 1 or 2 */
    bflb_pec_mem_write(dev, mem + 11, pec_instr_OUTPINVAL(PEC_PIN_OUT_0, 1, pec_instr_side(3, 3, 2)));
    bflb_pec_mem_write(dev, mem + 12, pec_instr_NOP(PEC_R2, pec_instr_side(2, 1, 2)));
    bflb_pec_mem_write(dev, mem + 13, pec_instr_INPINVAL(PEC_PIN_IN_0, 1, pec_instr_side(0, 0, 2)));
    bflb_pec_mem_write(dev, mem + 14, pec_instr_JRNZ(mem + 11, PEC_R2, PEC_INSTR_CMP_DEC, pec_instr_side(0, 0, 2)));
    bflb_pec_mem_write(dev, mem + 15, pec_instr_MOV(PEC_INSTR_MOV_DST_PINVAL_1, PEC_R0, 6, 1, pec_instr_side(0, 0, 2)));
    bflb_pec_mem_write(dev, mem + 16, pec_instr_PUSH(PEC_INSTR_PUSH_DROP, pec_instr_side(0, 0, 2)));
    bflb_pec_mem_write(dev, mem + 17, pec_instr_GOTO(mem + 0, pec_instr_side(0, 0, 2)));

    bflb_pec_enable(dev);
    return PEC_STS_OK;
}

void bflb_pec_spi_set_bits(struct bflb_device_s *dev, uint8_t bits)
{
    uint32_t val;

    if (bits == 0 || bits >= 32) {
        bits = 31;
    } else {
        bits = bits - 1;
    }
    val = (0xFFFFFFFF << 5) | (bits << 0);
    val = ~val;
    val <<= 16;
    val |= (bits << 0);
    bflb_pec_sm_regl_rs(dev, PEC_R0, val);
}
