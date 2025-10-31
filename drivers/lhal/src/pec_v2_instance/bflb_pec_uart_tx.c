#include "bflb_pec_v2.h"
#include "bflb_pec_v2_instance.h"
#include "bflb_pec_common.h"
#include "bflb_clock.h"

/*** note
R0[31:16]: stopbit time(sub 6), R0[15:0]: 1bit time(sub 6)
R1[6]: parity enable or disable, R1[5]: odd or even parity, R1[4:0]: databits(sub 1, not include parity)
R2: 
R3: iterative value for 1bit time
R4: iterative value for databits
R5: sotre count of 1 in one frame, for parity
*******************************************************************************/

static void bflb_pec_uart_tx_set_baudrate_and_stopbits(struct bflb_device_s *dev, struct bflb_pec_uart_tx_s *uart)
{
    uint32_t clk, cnt, val;

    clk = bflb_clk_get_peripheral_clock(BFLB_DEVICE_TYPE_PEC, dev->idx);
    if (uart->baudrate < 9600) {
        bflb_pec_clock_div_set(dev, 31);
        clk = clk / 32;
    } else {
        bflb_pec_clock_div_set(dev, 0);
    }
    cnt = clk / uart->baudrate;
    cnt = (cnt > 32768) ? 32768 : cnt;
    cnt = (cnt < 12) ? 12 : cnt;
    if (uart->stopbits == PEC_UART_STOPBITS_0P5) {
        val = cnt / 2;
    } else if (uart->stopbits == PEC_UART_STOPBITS_1P5) {
        val = cnt * 3 / 2;
    } else if (uart->stopbits == PEC_UART_STOPBITS_2P0) {
        val = cnt * 2;
    } else {
        val = cnt;
    }
    val = (val - 6) << 16; /* stop time */
    val |= (cnt - 6); /* 1bit time */
    bflb_pec_sm_reg_write(dev, PEC_R0, val);
}

static void bflb_pec_uart_tx_set_databits(struct bflb_device_s *dev, struct bflb_pec_uart_tx_s *uart)
{
    uint32_t val;
    uint32_t databits;

    databits = uart->databits - 1;
    val = (0xFFFFFFFF << 5) | (databits << 0);
    val = ~val;
    val <<= 16;
    val |= (databits << 0);
    bflb_pec_sm_regl_rs(dev, PEC_R1, val);
}

static void bflb_pec_uart_tx_set_parity(struct bflb_device_s *dev, struct bflb_pec_uart_tx_s *uart)
{
    if (uart->parity == PEC_UART_PARITY_NONE) {
        bflb_pec_sm_regl_rs(dev, PEC_R1, 3 << (5 + 16)); /* en=0, parity=0 */
    } else if (uart->parity == PEC_UART_PARITY_ODD) {
        bflb_pec_sm_regl_rs(dev, PEC_R1, 3 << (5 + 0)); /* en=1, parity=1 */
    } else if (uart->parity == PEC_UART_PARITY_EVEN) {
        bflb_pec_sm_regl_rs(dev, PEC_R1, (1 << 6) | (1 << (5 + 16))); /* en=1, parity=0 */
    } else {

    }
}

int bflb_pec_uart_tx_init(struct bflb_device_s *dev, struct bflb_pec_uart_tx_s *uart)
{
    struct bflb_pec_cfg_s cfg;
    uint8_t mem, pin;
    int ret;

    ret = bflb_pec_memory_size_check(dev, uart->mem, 20, &mem);
    if (ret != PEC_STS_OK) {
        return ret;
    }

    bflb_pec_disable(dev);
    cfg.clkdiv = 0;                                    /*!< clock divisor, frequency = clock freq / (clkdiv + 1) */
    cfg.pc_origin = mem + 0;                           /*!< beginning of the program for state machine, 0~255 is available */
    cfg.side_en = 1;                                   /*!< allow instructions to perform sideset optionally, rather than on every instruction */
    cfg.side_pindir = 0;                               /*!< side data is asserted to pin OEs or pin values */
    cfg.fifo_join = PEC_FIFO_RX_JOIN_TX;               /*!< if rx_fifo and tx_fifo join */
    cfg.out_dir = PEC_SHIFT_DIR_TO_RIGHT;              /*!< shift OSR direction */
    cfg.in_dir = PEC_SHIFT_DIR_TO_RIGHT;               /*!< shift ISR direction */
    cfg.auto_pull = 0;                                 /*!< pull automatically when the output shift register is emptied */
    cfg.auto_push = 0;                                 /*!< push automatically when the input shift register is filled */
    cfg.wrap_top = mem + 19;                           /*!< after reaching this address, execution is wrapped to wrap_bottom */
    cfg.wrap_bottom = mem + 0;                         /*!< ater reaching wrap_top, execution is wrapped to this address */
    cfg.dma_tx = uart->dma_enable;                     /*!< enable signal of dma_tx_req/ack interface */
    cfg.dma_rx = 0;                                    /*!< enable signal of dma_rx_req/ack interface */
    cfg.thresh_bits_pull = 0;                          /*!< number of bits shifted out of TXSR before autopull or conditional pull, 1~31, 0 present 32 */
    cfg.thresh_bits_push = 0;                          /*!< number of bits shifted into RXSR before autopush or conditional push, 1~31, 0 present 32 */
    cfg.thresh_fifo_tx = uart->fifo_threshold;         /*!< threshold of TX FIFO for interrupt and DMA, 0~15 if no join, 0~31 if rx_join_tx */
    cfg.thresh_fifo_rx = 0;                            /*!< threshold of RX FIFO for interrupt and DMA, 0~15 if no join, 0~31 if tx_join_rx */
    bflb_pec_init(dev, &cfg);

    pin = uart->pin % 64;
    bflb_pec_pin_side_base_set(dev, pin);
    bflb_pec_pin_side_count_set(dev, 2);
    bflb_pec_pin_out_base_set(dev, 0, pin);
    bflb_pec_pin_out_count_set(dev, 0, 1);
    if (uart->pin_inverse) {
        bflb_pec_pad_o_inv_enable_bit(dev, pin);
        bflb_pec_pad_o_clr_bit(dev, pin);
    } else {
        bflb_pec_pad_o_inv_disable_bit(dev, pin);
        bflb_pec_pad_o_set_bit(dev, pin);
    }
    bflb_pec_pad_oe_set_bit(dev, pin);

    bflb_pec_mem_write(dev, mem + 0, pec_instr_PULLB(pec_instr_side(3, 0, 2))); /* pull blocking for a data */
    bflb_pec_mem_write(dev, mem + 1, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R0, 0, 16, pec_instr_side(2, 0, 2))); /* prepare start bit time and side pin as low */
    bflb_pec_mem_write(dev, mem + 2, pec_instr_JRNZ(mem + 2, PEC_R3, PEC_INSTR_CMP_DEC, pec_instr_side(2, 0, 2))); /* wait start phase end */
    /* start transmit data */
    bflb_pec_mem_write(dev, mem + 3, pec_instr_MOV(PEC_INSTR_MOV_DST_R4, PEC_INSTR_MOV_SRC_R1, 0, 5, pec_instr_side(0, 1, 2))); /* R4 store bit number of 1 frame */
    bflb_pec_mem_write(dev, mem + 4, pec_instr_MOV(PEC_INSTR_MOV_DST_R5, PEC_INSTR_MOV_SRC_R1, 5, 1, pec_instr_side(0, 0, 2))); /* init parity value */
    /* judge every bit value */
    bflb_pec_mem_write(dev, mem + 5, pec_instr_JBNZ(mem + 9, PEC_OSR, 0, PEC_INSTR_BIT_NONE, pec_instr_side(0, 0, 2))); /* LSB 0 or 1, to be transmitted */
    /* transmit low level of 1bit */
    bflb_pec_mem_write(dev, mem + 6, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R0, 0, 16, pec_instr_side(2, 0, 2))); /* prepare 1bit time and side pin as low */
    bflb_pec_mem_write(dev, mem + 7, pec_instr_JRNZ(mem + 7, PEC_R3, PEC_INSTR_CMP_DEC, pec_instr_side(2, 0, 2))); /* wait low level of 1bit end */
    bflb_pec_mem_write(dev, mem + 8, pec_instr_GOTO(mem + 12, pec_instr_side(0, 0, 2)));
    /* transmit high level of 1bit */
    bflb_pec_mem_write(dev, mem + 9, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R0, 0, 16, pec_instr_side(3, 0, 2))); /* prepare 1bit time and side pin as high */
    bflb_pec_mem_write(dev, mem + 10, pec_instr_JRNZ(mem + 10, PEC_R3, PEC_INSTR_CMP_DEC, pec_instr_side(3, 0, 2))); /* wait high level of 1bit end */
    bflb_pec_mem_write(dev, mem + 11, pec_instr_ADDI(PEC_R5, PEC_R5, 1, pec_instr_side(0, 0, 2)));
    /* prepare next bit */
    bflb_pec_mem_write(dev, mem + 12, pec_instr_SRLI(PEC_OSR, PEC_OSR, 1, pec_instr_side(0, 0, 2)));
    bflb_pec_mem_write(dev, mem + 13, pec_instr_JRNZ(mem + 5, PEC_R4, PEC_INSTR_CMP_DEC, pec_instr_side(0, 0, 2)));
    /* parity */
    bflb_pec_mem_write(dev, mem + 14, pec_instr_JBZ(mem + 18, PEC_R1, 6, PEC_INSTR_BIT_NONE, pec_instr_side(0, 0, 2))); /* parity enable or disable */
    bflb_pec_mem_write(dev, mem + 15, pec_instr_MOV(PEC_INSTR_MOV_DST_PINVAL_0, PEC_INSTR_MOV_SRC_R5, 0, 1, pec_instr_side(0, 3, 2))); /* transmit parity */
    bflb_pec_mem_write(dev, mem + 16, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R0, 0, 16, pec_instr_side(0, 0, 2))); /* prepare 1bit time for parity time */
    bflb_pec_mem_write(dev, mem + 17, pec_instr_JRNZ(mem + 17, PEC_R3, PEC_INSTR_CMP_DEC, pec_instr_side(0, 0, 2))); /* wait parity end */
    /* stopbit */
    bflb_pec_mem_write(dev, mem + 18, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R0, 16, 16, pec_instr_side(3, 3, 2))); /* prepare stop bit time and side pin as high */
    bflb_pec_mem_write(dev, mem + 19, pec_instr_JRNZ(mem + 19, PEC_R3, PEC_INSTR_CMP_DEC, pec_instr_side(3, 0, 2))); /* wait stopbit end */

    bflb_pec_uart_tx_set_baudrate_and_stopbits(dev, uart);
    bflb_pec_uart_tx_set_databits(dev, uart);
    bflb_pec_uart_tx_set_parity(dev, uart);

    bflb_pec_enable(dev);
    return PEC_STS_OK;
}

void bflb_pec_uart_tx_deinit(struct bflb_device_s *dev)
{
    bflb_pec_disable(dev);
}
