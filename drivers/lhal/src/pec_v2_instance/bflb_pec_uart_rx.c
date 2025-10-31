#include "bflb_pec_v2.h"
#include "bflb_pec_v2_instance.h"
#include "bflb_pec_common.h"
#include "bflb_clock.h"

/*** note
R0[15:0]: bit time
R1[4:0]: databits(sub 1, not include parity)
R1[5]: odd(1) or even(0) parity
R1[6]: parity enable or disable
R1[7]: receive enable
R1[8]: RTO flag, used for state machine, software should not modify this value
R2: RTO time
R3: iterative value for bit time
R4: iterative value for databits and RTO time
R5: sotre count of 1 in one frame, for parity
IPC[0]: RTO flag, IPC[1]: parity error flag
*******************************************************************************/

static void bflb_pec_uart_rx_set_baudrate_and_rto(struct bflb_device_s *dev, struct bflb_pec_uart_rx_s *uart)
{
    uint32_t clk, cnt;

    clk = bflb_clk_get_peripheral_clock(BFLB_DEVICE_TYPE_PEC, dev->idx);
    if (uart->baudrate < 9600) {
        bflb_pec_clock_div_set(dev, 31);
        clk = clk / 32;
    } else {
        bflb_pec_clock_div_set(dev, 0);
    }
    cnt = clk / uart->baudrate;
    cnt = (cnt > 32767) ? 32767 : cnt;
    cnt = (cnt < 25) ? 25 : cnt;
    bflb_pec_sm_regl_rs(dev, PEC_R0, cnt | 0xFFFF0000);
    cnt = uart->rto_bits * cnt;
    bflb_pec_sm_reg_write(dev, PEC_R2, cnt);
}

static void bflb_pec_uart_rx_set_databits(struct bflb_device_s *dev, struct bflb_pec_uart_rx_s *uart)
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

static void bflb_pec_uart_rx_set_parity(struct bflb_device_s *dev, struct bflb_pec_uart_rx_s *uart)
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

int bflb_pec_uart_rx_init(struct bflb_device_s *dev, struct bflb_pec_uart_rx_s *uart)
{
    struct bflb_pec_cfg_s cfg;
    uint8_t mem, pin;
    int ret;

    ret = bflb_pec_memory_size_check(dev, uart->mem, 52, &mem);
    if (ret != PEC_STS_OK) {
        return ret;
    }

    bflb_pec_disable(dev);
    cfg.clkdiv = 0;                                    /*!< clock divisor, frequency = clock freq / (clkdiv + 1) */
    cfg.pc_origin = mem + 4;                           /*!< beginning of the program for state machine, 0~255 is available */
    cfg.side_en = 0;                                   /*!< allow instructions to perform sideset optionally, rather than on every instruction */
    cfg.side_pindir = 0;                               /*!< side data is asserted to pin OEs or pin values */
    cfg.fifo_join = PEC_FIFO_TX_JOIN_RX;               /*!< if rx_fifo and tx_fifo join */
    cfg.out_dir = PEC_SHIFT_DIR_TO_RIGHT;              /*!< shift OSR direction */
    cfg.in_dir = PEC_SHIFT_DIR_TO_RIGHT;               /*!< shift ISR direction */
    cfg.auto_pull = 0;                                 /*!< pull automatically when the output shift register is emptied */
    cfg.auto_push = 0;                                 /*!< push automatically when the input shift register is filled */
    cfg.wrap_top = 255;                                /*!< after reaching this address, execution is wrapped to wrap_bottom */
    cfg.wrap_bottom = 0;                               /*!< ater reaching wrap_top, execution is wrapped to this address */
    cfg.dma_tx = 0;                                    /*!< enable signal of dma_tx_req/ack interface */
    cfg.dma_rx = uart->dma_enable;                     /*!< enable signal of dma_rx_req/ack interface */
    cfg.thresh_bits_pull = 0;                          /*!< number of bits shifted out of TXSR before autopull or conditional pull, 1~31, 0 present 32 */
    cfg.thresh_bits_push = 0;                          /*!< number of bits shifted into RXSR before autopush or conditional push, 1~31, 0 present 32 */
    cfg.thresh_fifo_tx = 0;                            /*!< threshold of TX FIFO for interrupt and DMA, 0~15 if no join, 0~31 if rx_join_tx */
    cfg.thresh_fifo_rx = uart->fifo_threshold;         /*!< threshold of RX FIFO for interrupt and DMA, 0~15 if no join, 0~31 if tx_join_rx */
    bflb_pec_init(dev, &cfg);

    pin = uart->pin % 64;
    bflb_pec_pin_in_base_set(dev, 0, pin);
    if (uart->pin_inverse) {
        bflb_pec_pad_o_inv_enable_bit(dev, pin);
    } else {
        bflb_pec_pad_o_inv_disable_bit(dev, pin);
    }
    bflb_pec_pad_oe_clr_bit(dev, pin);

    /* waiting for stable start bit, low level */
    bflb_pec_mem_write(dev, mem + 0, pec_instr_JPL(mem + 5, PEC_PIN_IN_0, 0, pec_instr_side(0, 0, 0))); /* wait for low level */
    bflb_pec_mem_write(dev, mem + 1, pec_instr_JBNZ(mem + 0, PEC_R1, 8, PEC_INSTR_BIT_NONE, pec_instr_side(0, 0, 0))); /* judge whether RTO flag is already set */
    bflb_pec_mem_write(dev, mem + 2, pec_instr_JRNZ(mem + 0, PEC_R4, PEC_INSTR_CMP_DEC, pec_instr_side(0, 0, 0)));
    bflb_pec_mem_write(dev, mem + 3, pec_instr_IPCS(dev->sub_idx * 8 + 1, pec_instr_side(0, 0, 0))); /* pending RTO interrupt */
    bflb_pec_mem_write(dev, mem + 4, pec_instr_JBA(mem + 0, PEC_R1, 8, PEC_INSTR_BIT_SET, pec_instr_side(0, 0, 0))); /* set RTO flag */
    /* detect low level */
    bflb_pec_mem_write(dev, mem + 5, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R0, 2, 14, pec_instr_side(0, 0, 0))); /* R0[15:2] = bit_time[15:0] / 4 */
    bflb_pec_mem_write(dev, mem + 6, pec_instr_SUBI(PEC_R3, PEC_R3, 4, pec_instr_side(0, 0, 0))); /* calibrate 0.25 bit time */
    bflb_pec_mem_write(dev, mem + 7, pec_instr_JRNZ(mem + 7, PEC_R3, PEC_INSTR_CMP_DEC, pec_instr_side(0, 0, 0))); /* wait 0.25 bit time end */
    bflb_pec_mem_write(dev, mem + 8, pec_instr_JPH(mem + 0, PEC_PIN_IN_0, 0, pec_instr_side(0, 0, 0))); /* at 0.25 bit time of start bit */
    bflb_pec_mem_write(dev, mem + 9, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R0, 2, 14, pec_instr_side(0, 0, 0))); /* R0[15:2] = bit_time[15:0] / 4 */
    bflb_pec_mem_write(dev, mem + 10, pec_instr_SUBI(PEC_R3, PEC_R3, 4, pec_instr_side(0, 0, 0))); /* calibrate 0.25 bit time */
    bflb_pec_mem_write(dev, mem + 11, pec_instr_JRNZ(mem + 11, PEC_R3, PEC_INSTR_CMP_DEC, pec_instr_side(0, 0, 0))); /* wait 0.25 bit time end */
    bflb_pec_mem_write(dev, mem + 12, pec_instr_JPH(mem + 0, PEC_PIN_IN_0, 0, pec_instr_side(0, 0, 0))); /* at 0.5 bit time of start bit */
    /* is uart_rx enable ? */
    bflb_pec_mem_write(dev, mem + 13, pec_instr_JBZ(mem + 0, PEC_R1, 7, PEC_INSTR_BIT_NONE, pec_instr_side(0, 0, 0)));
    /* init register value for receive data bits */
    bflb_pec_mem_write(dev, mem + 14, pec_instr_SET(PEC_INSTR_SET_DST_ISR, 0, pec_instr_side(0, 0, 0))); /* clear ISR */
    bflb_pec_mem_write(dev, mem + 15, pec_instr_MOV(PEC_INSTR_MOV_DST_R4, PEC_INSTR_MOV_SRC_R1, 0, 5, pec_instr_side(0, 0, 0))); /* R4 store bit number of 1 frame */
    bflb_pec_mem_write(dev, mem + 16, pec_instr_MOV(PEC_INSTR_MOV_DST_R5, PEC_INSTR_MOV_SRC_R1, 5, 1, pec_instr_side(0, 0, 0))); /* init parity value */
    /* at start bit midpoint */
    bflb_pec_mem_write(dev, mem + 17, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R0, 0, 16, pec_instr_side(0, 0, 0))); /* 1bit time */
    bflb_pec_mem_write(dev, mem + 18, pec_instr_SUBI(PEC_R3, PEC_R3, 7, pec_instr_side(0, 0, 0))); /* calibrate 1bit time */
    bflb_pec_mem_write(dev, mem + 19, pec_instr_JRNZ(mem + 19, PEC_R3, PEC_INSTR_CMP_DEC, pec_instr_side(0, 0, 0))); /* wait 1bit time end */
    bflb_pec_mem_write(dev, mem + 20, pec_instr_JPH(mem + 23, PEC_PIN_IN_0, 0, pec_instr_side(0, 0, 0)));
    /* receive 1bit of low level */
    bflb_pec_mem_write(dev, mem + 21, pec_instr_IN0S(1, pec_instr_side(0, 0, 0)));
    bflb_pec_mem_write(dev, mem + 22, pec_instr_GOTO(mem + 25, pec_instr_side(0, 0, 2)));
    /* receive 1bit of high level */
    bflb_pec_mem_write(dev, mem + 23, pec_instr_IN1S(1, pec_instr_side(0, 0, 0)));
    bflb_pec_mem_write(dev, mem + 24, pec_instr_ADDI(PEC_R5, PEC_R5, 1, pec_instr_side(0, 0, 0))); /* count 1 in one frame */
    bflb_pec_mem_write(dev, mem + 25, pec_instr_JRNZ(mem + 17, PEC_R4, PEC_INSTR_CMP_DEC, pec_instr_side(0, 0, 0))); /* judge whether databits all received */
    /* parity */
    bflb_pec_mem_write(dev, mem + 26, pec_instr_JBZ(mem + 38, PEC_R1, 6, PEC_INSTR_BIT_NONE, pec_instr_side(0, 0, 2))); /* parity enable or disable */
    bflb_pec_mem_write(dev, mem + 27, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R0, 0, 16, pec_instr_side(0, 0, 0))); /* parity 1bit time */
    bflb_pec_mem_write(dev, mem + 28, pec_instr_SUBI(PEC_R3, PEC_R3, 8, pec_instr_side(0, 0, 0))); /* calibrate 1bit time */
    bflb_pec_mem_write(dev, mem + 29, pec_instr_JRNZ(mem + 29, PEC_R3, PEC_INSTR_CMP_DEC, pec_instr_side(0, 0, 0))); /* wait 1bit time end */
    bflb_pec_mem_write(dev, mem + 30, pec_instr_JPL(mem + 32, PEC_PIN_IN_0, 0, pec_instr_side(0, 0, 0))); /* judge parity value */
    bflb_pec_mem_write(dev, mem + 31, pec_instr_ADDI(PEC_R5, PEC_R5, 1, pec_instr_side(0, 0, 0))); /* count 1 including parity */
    bflb_pec_mem_write(dev, mem + 32, pec_instr_JBNZ(mem + 35, PEC_R5, 0, PEC_INSTR_BIT_NONE, pec_instr_side(0, 0, 0))); /* parity is right(0) or error(1) */
    /* prity right */
    bflb_pec_mem_write(dev, mem + 33, pec_instr_IN0S(1, pec_instr_side(0, 0, 0)));
    bflb_pec_mem_write(dev, mem + 34, pec_instr_GOTO(mem + 37, pec_instr_side(0, 0, 2)));
    /* prity error */
    bflb_pec_mem_write(dev, mem + 35, pec_instr_IN1S(1, pec_instr_side(0, 0, 0)));
    bflb_pec_mem_write(dev, mem + 36, pec_instr_IPCS(dev->sub_idx * 8 + 0, pec_instr_side(0, 0, 0)));
    bflb_pec_mem_write(dev, mem + 37, pec_instr_GOTO(mem + 39, pec_instr_side(0, 0, 2)));
    bflb_pec_mem_write(dev, mem + 38, pec_instr_IN0S(1, pec_instr_side(0, 0, 0)));
    /* shift right to bit0 */
    bflb_pec_mem_write(dev, mem + 39, pec_instr_MOV(PEC_INSTR_MOV_DST_R4, PEC_INSTR_MOV_SRC_R1, 0, 5, pec_instr_side(0, 0, 0)));
    bflb_pec_mem_write(dev, mem + 40, pec_instr_SET(PEC_INSTR_SET_DST_R3, 30, pec_instr_side(0, 0, 0))); /* calc shift bits remained */
    bflb_pec_mem_write(dev, mem + 41, pec_instr_SUB(PEC_R3, PEC_R3, PEC_R4, pec_instr_side(0, 0, 0)));
    bflb_pec_mem_write(dev, mem + 42, pec_instr_SRL(PEC_ISR, PEC_ISR, PEC_R3, pec_instr_side(0, 0, 0)));
    bflb_pec_mem_write(dev, mem + 43, pec_instr_PUSH(PEC_INSTR_PUSH_DROP, pec_instr_side(0, 0, 0)));
    /* wait parity phase end, stop phase start */
    bflb_pec_mem_write(dev, mem + 44, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R0, 1, 15, pec_instr_side(0, 0, 0))); /* R0[15:1] = bit_time[15:0] / 2 */
    bflb_pec_mem_write(dev, mem + 45, pec_instr_SUBI(PEC_R3, PEC_R3, 12, pec_instr_side(0, 0, 0))); /* calibrate 1bit time */
    bflb_pec_mem_write(dev, mem + 46, pec_instr_JRNZ(mem + 46, PEC_R3, PEC_INSTR_CMP_DEC, pec_instr_side(0, 0, 0))); /* wait 0.5bit time end */
    /* at 0.25stop point */
    bflb_pec_mem_write(dev, mem + 47, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R0, 2, 14, pec_instr_side(0, 0, 0))); /* R0[15:2] = bit_time[15:0] / 4 */
    bflb_pec_mem_write(dev, mem + 48, pec_instr_SUBI(PEC_R3, PEC_R3, 6, pec_instr_side(0, 0, 0))); /* calibrate 0.25 bit time */
    bflb_pec_mem_write(dev, mem + 49, pec_instr_JRNZ(mem + 49, PEC_R3, PEC_INSTR_CMP_DEC, pec_instr_side(0, 0, 0))); /* wait 0.25 bit time end */
    /* RTO(receive timeout) */
    bflb_pec_mem_write(dev, mem + 50, pec_instr_MOVREG(PEC_INSTR_MOV_DST_R4, PEC_INSTR_MOV_SRC_R2, PEC_INSTR_MOV_OP_NONE, pec_instr_side(0, 0, 0)));
    bflb_pec_mem_write(dev, mem + 51, pec_instr_JBA(mem + 0, PEC_R1, 8, PEC_INSTR_BIT_CLR, pec_instr_side(0, 0, 0))); /* clear RTO flag */

    bflb_pec_uart_rx_set_baudrate_and_rto(dev, uart);
    bflb_pec_uart_rx_set_databits(dev, uart);
    bflb_pec_uart_rx_set_parity(dev, uart);

    bflb_pec_enable(dev);
    return PEC_STS_OK;
}

void bflb_pec_uart_rx_deinit(struct bflb_device_s *dev)
{
    bflb_pec_disable(dev);
}

void bflb_pec_uart_rx_start(struct bflb_device_s *dev)
{
    bflb_pec_sm_reg_set_bit(dev, PEC_R1, 7);
}

void bflb_pec_uart_rx_stop(struct bflb_device_s *dev)
{
    bflb_pec_sm_reg_clr_bit(dev, PEC_R1, 7);
}
