#include "bflb_pec_v2.h"
#include "bflb_pec_v2_instance.h"
#include "bflb_pec_common.h"

/*** note
R0: (high count - 2)
R1: (low  count - 3)
R2[0]: 1 for start, 0 for stop
R3: iterative value
*******************************************************************************/

/****************************************************************************/ /**
 * @brief  initialize pwm ip
 *
 * @param  dev: device handle
 * @param  pwm: pointor of pwm configuration structure
 *
 * @return status of initialization
 *
*******************************************************************************/
int bflb_pec_pwm_init(struct bflb_device_s *dev, struct bflb_pec_pwm_s *pwm)
{
    struct bflb_pec_cfg_s cfg;
    uint8_t mem, pin;
    int ret;

    ret = bflb_pec_memory_size_check(dev, pwm->mem, 5, &mem);
    if (ret != PEC_STS_OK) {
        return ret;
    }

    bflb_pec_disable(dev);
    cfg.clkdiv = pwm->div;                             /*!< clock divisor, frequency = clock freq / (clkdiv + 1) */
    cfg.pc_origin = mem + 4;                           /*!< beginning of the program for state machine, 0~255 is available */
    cfg.side_en = 1;                                   /*!< allow instructions to perform sideset optionally, rather than on every instruction */
    cfg.side_pindir = 0;                               /*!< side data is asserted to pin OEs or pin values */
    cfg.fifo_join = 0;                                 /*!< if rx_fifo and tx_fifo join */
    cfg.out_dir = PEC_SHIFT_DIR_TO_LEFT;               /*!< shift OSR direction */
    cfg.in_dir = PEC_SHIFT_DIR_TO_LEFT;                /*!< shift ISR direction */
    cfg.auto_pull = 0;                                 /*!< pull automatically when the output shift register is emptied */
    cfg.auto_push = 0;                                 /*!< push automatically when the input shift register is filled */
    cfg.wrap_top = mem + 4;                            /*!< after reaching this address, execution is wrapped to wrap_bottom */
    cfg.wrap_bottom = mem + 0;                         /*!< ater reaching wrap_top, execution is wrapped to this address */
    cfg.dma_tx = 0;                                    /*!< enable signal of dma_tx_req/ack interface */
    cfg.dma_rx = 0;                                    /*!< enable signal of dma_rx_req/ack interface */
    cfg.thresh_bits_pull = 0;                          /*!< number of bits shifted out of TXSR before autopull or conditional pull, 1~31, 0 present 32 */
    cfg.thresh_bits_push = 0;                          /*!< number of bits shifted into RXSR before autopush or conditional push, 1~31, 0 present 32 */
    cfg.thresh_fifo_tx = 0;                            /*!< threshold of TX FIFO for interrupt and DMA, 0~15 if no join, 0~31 if rx_join_tx */
    cfg.thresh_fifo_rx = 0;                            /*!< threshold of RX FIFO for interrupt and DMA, 0~15 if no join, 0~31 if tx_join_rx */
    bflb_pec_init(dev, &cfg);

    pin = pwm->pin % 64;
    bflb_pec_pin_side_base_set(dev, pin);
    bflb_pec_pin_side_count_set(dev, 2);
    if (pwm->inverse) {
        bflb_pec_pad_o_inv_enable_bit(dev, pin);
        bflb_pec_pad_o_set_bit(dev, pin);
    } else {
        bflb_pec_pad_o_inv_disable_bit(dev, pin);
        bflb_pec_pad_o_clr_bit(dev, pin);
    }
    bflb_pec_pad_oe_set_bit(dev, pin);

    bflb_pec_mem_write(dev, mem + 0, pec_instr_MOVREG(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R0, PEC_INSTR_MOV_OP_NONE, pec_instr_side(3, 0, 2))); /* set pin high */
    bflb_pec_mem_write(dev, mem + 1, pec_instr_JRNZ(mem + 1, PEC_R3, PEC_INSTR_CMP_DEC, pec_instr_side(0, 0, 2))); /* high level time */
    bflb_pec_mem_write(dev, mem + 2, pec_instr_MOVREG(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R1, PEC_INSTR_MOV_OP_NONE, pec_instr_side(2, 0, 2))); /* set pin low */
    bflb_pec_mem_write(dev, mem + 3, pec_instr_JRNZ(mem + 3, PEC_R3, PEC_INSTR_CMP_DEC, pec_instr_side(0, 0, 2))); /* low level time */
    bflb_pec_mem_write(dev, mem + 4, pec_instr_JBZ(mem + 4, PEC_R2, 0, PEC_INSTR_CMP_NONE, pec_instr_side(0, 0, 2))); /* judge enable bit */

    bflb_pec_enable(dev);
    return PEC_STS_OK;
}

/****************************************************************************/ /**
 * @brief  deinit pwm ip
 *
 * @param  dev: device handle
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pwm_deinit(struct bflb_device_s *dev)
{
    bflb_pec_disable(dev);
}

/****************************************************************************/ /**
 * @brief  start pwm
 *
 * @param  dev: device handle
 * @param  high_cnt: high level count, must >= 2
 * @param  low_cnt: low level count, must >= 3
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pwm_start(struct bflb_device_s *dev, uint32_t high_cnt, uint32_t low_cnt)
{
    high_cnt = (high_cnt >= 2) ? (high_cnt - 2) : 0;
    low_cnt = (low_cnt >= 3) ? (low_cnt - 3) : 0;
    bflb_pec_sm_reg_write(dev, PEC_R0, high_cnt);
    bflb_pec_sm_reg_write(dev, PEC_R1, low_cnt);
    bflb_pec_sm_reg_set_bit(dev, PEC_R2, 0);
}

/****************************************************************************/ /**
 * @brief  stop pwm
 *
 * @param  dev: device handle
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pwm_stop(struct bflb_device_s *dev)
{
    bflb_pec_sm_reg_clr_bit(dev, PEC_R2, 0);
}
