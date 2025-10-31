#ifndef __BFLB_PEC_V2_H__
#define __BFLB_PEC_V2_H__

#include "bflb_core.h"
#include "bflb_pec_v2_instr.h"

#define PEC_STS_OK               (0)
#define PEC_STS_MEMORY_LACK      (1)
#define PEC_STS_TIMEOUT          (2)
#define PEC_STS_SM_INVALID       (3)

#if defined(BL616L)
#define PEC_SM_MAX               (2)
#define PEC_FIFO_DEEPTH          (8)
#define PEC_MEMORY_SIZE_0        (128)
#define PEC_MEMORY_SIZE_1        (PEC_MEMORY_SIZE_0 + 64)
#define PEC_PIN_MAX              (64)
#else
#define PEC_SM_MAX               (4)
#define PEC_FIFO_DEEPTH          (16)
#define PEC_MEMORY_SIZE          (256)
#define PEC_PIN_MAX              (64)
#endif

#define PEC_SHIFT_DIR_TO_LEFT    (0) /*!< shift OSR/ISR to left (data enters from right) */
#define PEC_SHIFT_DIR_TO_RIGHT   (1) /*!< shift OSR/ISR to right (data enters from left) */

#define PEC_FIFO_JOIN_NONE       (0) /*!< tx_fifo and rx_fifo all work normally */
#define PEC_FIFO_RX_JOIN_TX      (1) /*!< rx_fifo join tx_fifo, rx_fifo deepth = 0, tx_fifo expand to double */
#define PEC_FIFO_TX_JOIN_RX      (2) /*!< tx_fifo join rx_fifo, tx_fifo deepth = 0, rx_fifo expand to double */

#define PEC_INT_SM0_RXF_RDY      (1 << 0)
#define PEC_INT_SM1_RXF_RDY      (1 << 1)
#define PEC_INT_SM2_RXF_RDY      (1 << 2)
#define PEC_INT_SM3_RXF_RDY      (1 << 3)
#define PEC_INT_SM0_TXF_RDY      (1 << 4)
#define PEC_INT_SM1_TXF_RDY      (1 << 5)
#define PEC_INT_SM2_TXF_RDY      (1 << 6)
#define PEC_INT_SM3_TXF_RDY      (1 << 7)
#define PEC_INT_IPC_0_7          (1 << 8)
#define PEC_INT_IPC_8_15         (1 << 9)
#define PEC_INT_IPC_16_23        (1 << 10)
#define PEC_INT_IPC_24_31        (1 << 11)
#define PEC_INT_ALL              ((1 << 12) - 1)

#define PEC_GLB_CLK_PLL160M      (0)
#define PEC_GLB_CLK_XCLK         (1)

struct bflb_pec_cfg_s {
    uint16_t clkdiv;               /*!< clock divisor, frequency = clock freq / (clkdiv + 1) */
    uint8_t pc_origin;             /*!< beginning of the program for state machine, 0~255 is available */
    uint8_t side_en;               /*!< allow instructions to perform sideset optionally, rather than on every instruction */
    uint8_t side_pindir;           /*!< side data is asserted to pin OEs or pin values */
    uint8_t fifo_join;             /*!< if rx_fifo and tx_fifo join */
    uint8_t out_dir;               /*!< shift OSR direction */
    uint8_t in_dir;                /*!< shift ISR direction */
    uint8_t auto_pull;             /*!< pull automatically when the output shift register is emptied */
    uint8_t auto_push;             /*!< push automatically when the input shift register is filled */
    uint8_t wrap_top;              /*!< after reaching this address, execution is wrapped to wrap_bottom */
    uint8_t wrap_bottom;           /*!< ater reaching wrap_top, execution is wrapped to this address */
    uint8_t dma_tx;                /*!< enable signal of dma_tx_req/ack interface */
    uint8_t dma_rx;                /*!< enable signal of dma_rx_req/ack interface */
    uint8_t thresh_bits_pull;      /*!< number of bits shifted out of TXSR before autopull or conditional pull, 1~31, 0 present 32 */
    uint8_t thresh_bits_push;      /*!< number of bits shifted into RXSR before autopush or conditional push, 1~31, 0 present 32 */
    uint8_t thresh_fifo_tx;        /*!< threshold of TX FIFO for interrupt and DMA, 0~15 if no join, 0~31 if rx_join_tx */
    uint8_t thresh_fifo_rx;        /*!< threshold of RX FIFO for interrupt and DMA, 0~15 if no join, 0~31 if tx_join_rx */
};

#ifdef __cplusplus
extern "C" {
#endif

void bflb_pec_init(struct bflb_device_s *dev, struct bflb_pec_cfg_s *cfg);
void bflb_pec_enable(struct bflb_device_s *dev);
void bflb_pec_disable(struct bflb_device_s *dev);
void bflb_pec_enable_mask(struct bflb_device_s *dev, uint8_t sm_mask);
void bflb_pec_disable_mask(struct bflb_device_s *dev, uint8_t sm_mask);
void bflb_pec_clock_div_set(struct bflb_device_s *dev, uint16_t div);
uint16_t bflb_pec_clock_div_get(struct bflb_device_s *dev);
uint8_t bflb_pec_current_pc_get(struct bflb_device_s *dev);
uint32_t bflb_pec_current_instr_get(struct bflb_device_s *dev);
void bflb_pec_instr_insert(struct bflb_device_s *dev, uint32_t instr);
uint8_t bflb_pec_is_stalled(struct bflb_device_s *dev);
void bflb_pec_mem_write(struct bflb_device_s *dev, uint8_t addr, uint32_t instr);
void bflb_pec_mem_write_multi(struct bflb_device_s *dev, uint8_t addr, uint32_t *instr, uint16_t len);
uint32_t bflb_pec_mem_read(struct bflb_device_s *dev, uint8_t addr);
void bflb_pec_mem_store(struct bflb_device_s *dev, uint8_t addr);
uint8_t bflb_pec_mem_restore(struct bflb_device_s *dev);

/* IPC */
void bflb_pec_ipc_set_bit(struct bflb_device_s *dev, uint8_t bit);
void bflb_pec_ipc_clr_bit(struct bflb_device_s *dev, uint8_t bit);
uint8_t bflb_pec_ipc_get_bit(struct bflb_device_s *dev, uint8_t bit);
void bflb_pec_ipc_set_mask(struct bflb_device_s *dev, uint32_t mask);
void bflb_pec_ipc_clr_mask(struct bflb_device_s *dev, uint32_t mask);
uint32_t bflb_pec_ipc_get(struct bflb_device_s *dev);
void bflb_pec_ipc_int_enable_bit(struct bflb_device_s *dev, uint8_t bit);
void bflb_pec_ipc_int_disable_bit(struct bflb_device_s *dev, uint8_t bit);
uint8_t bflb_pec_ipc_int_enable_get_bit(struct bflb_device_s *dev, uint8_t bit);
void bflb_pec_ipc_int_enable_mask(struct bflb_device_s *dev, uint32_t mask);
void bflb_pec_ipc_int_disable_mask(struct bflb_device_s *dev, uint32_t mask);
uint32_t bflb_pec_ipc_int_enable_get(struct bflb_device_s *dev);

/* synchronization bypass */
void bflb_pec_sync_bypass_enable_bit(struct bflb_device_s *dev, uint8_t pin);
void bflb_pec_sync_bypass_disable_bit(struct bflb_device_s *dev, uint8_t pin);
uint8_t bflb_pec_sync_bypass_enable_get_bit(struct bflb_device_s *dev, uint8_t pin);
void bflb_pec_sync_bypass_enable_mask(struct bflb_device_s *dev, uint64_t mask);
void bflb_pec_sync_bypass_disable_mask(struct bflb_device_s *dev, uint64_t mask);
uint64_t bflb_pec_sync_bypass_get(struct bflb_device_s *dev);

/* fifo */
uint8_t bflb_pec_fifo_get_deepth_tx(struct bflb_device_s *dev);
uint8_t bflb_pec_fifo_get_deepth_rx(struct bflb_device_s *dev);
uint8_t bflb_pec_fifo_get_level_tx(struct bflb_device_s *dev);
uint8_t bflb_pec_fifo_get_level_rx(struct bflb_device_s *dev);
void bflb_pec_fifo_clr_tx(struct bflb_device_s *dev);
void bflb_pec_fifo_clr_rx(struct bflb_device_s *dev);
uint8_t bflb_pec_fifo_is_rdy_tx(struct bflb_device_s *dev);
uint8_t bflb_pec_fifo_is_rdy_rx(struct bflb_device_s *dev);
uint32_t bflb_pec_fifo_get_addr_tx(struct bflb_device_s *dev);
uint32_t bflb_pec_fifo_get_addr_rx(struct bflb_device_s *dev);
void bflb_pec_fifo_write(struct bflb_device_s *dev, uint32_t value);
uint32_t bflb_pec_fifo_read(struct bflb_device_s *dev);
uint32_t bflb_pec_fifo_write_multi_32bits(struct bflb_device_s *dev, uint32_t *data, uint32_t len);
uint32_t bflb_pec_fifo_write_multi_16bits(struct bflb_device_s *dev, uint16_t *data, uint32_t len);
uint32_t bflb_pec_fifo_write_multi_8bits(struct bflb_device_s *dev, uint8_t *data, uint32_t len);
void bflb_pec_fifo_write_multi_32bits_blocking(struct bflb_device_s *dev, uint32_t *data, uint32_t len);
void bflb_pec_fifo_write_multi_16bits_blocking(struct bflb_device_s *dev, uint16_t *data, uint32_t len);
void bflb_pec_fifo_write_multi_8bits_blocking(struct bflb_device_s *dev, uint8_t *data, uint32_t len);
uint32_t bflb_pec_fifo_read_multi_32bits(struct bflb_device_s *dev, uint32_t *data, uint32_t len);
uint32_t bflb_pec_fifo_read_multi_16bits(struct bflb_device_s *dev, uint16_t *data, uint32_t len);
uint32_t bflb_pec_fifo_read_multi_8bits(struct bflb_device_s *dev, uint8_t *data, uint32_t len);
void bflb_pec_fifo_set_threshold_tx(struct bflb_device_s *dev, uint8_t threshold);
void bflb_pec_fifo_set_threshold_rx(struct bflb_device_s *dev, uint8_t threshold);

/* pin */
void bflb_pec_pin_side_base_set(struct bflb_device_s *dev, uint8_t base);
void bflb_pec_pin_side_count_set(struct bflb_device_s *dev, uint8_t count);
void bflb_pec_pin_set_base_set(struct bflb_device_s *dev, uint8_t index, uint8_t base);
void bflb_pec_pin_set_count_set(struct bflb_device_s *dev, uint8_t index, uint8_t count);
void bflb_pec_pin_out_base_set(struct bflb_device_s *dev, uint8_t index, uint8_t base);
void bflb_pec_pin_out_count_set(struct bflb_device_s *dev, uint8_t index, uint8_t count);
void bflb_pec_pin_in_base_set(struct bflb_device_s *dev, uint8_t index, uint8_t base);
uint8_t bflb_pec_pin_side_base_get(struct bflb_device_s *dev);
uint8_t bflb_pec_pin_side_count_get(struct bflb_device_s *dev);
uint8_t bflb_pec_pin_set_base_get(struct bflb_device_s *dev, uint8_t index);
uint8_t bflb_pec_pin_set_count_get(struct bflb_device_s *dev, uint8_t index);
uint8_t bflb_pec_pin_out_base_get(struct bflb_device_s *dev, uint8_t index);
uint8_t bflb_pec_pin_out_count_get(struct bflb_device_s *dev, uint8_t index);
uint8_t bflb_pec_pin_in_base_get(struct bflb_device_s *dev, uint8_t index);

/* pad inverse */
void bflb_pec_pad_o_inv_enable_bit(struct bflb_device_s *dev, uint8_t pin);
void bflb_pec_pad_o_inv_disable_bit(struct bflb_device_s *dev, uint8_t pin);
uint8_t bflb_pec_pad_o_inv_enable_get_bit(struct bflb_device_s *dev, uint8_t pin);
void bflb_pec_pad_o_inv_enable_mask(struct bflb_device_s *dev, uint64_t mask);
void bflb_pec_pad_o_inv_disable_mask(struct bflb_device_s *dev, uint64_t mask);
uint64_t bflb_pec_pad_o_inv_enable_get(struct bflb_device_s *dev);
void bflb_pec_pad_oe_inv_enable_bit(struct bflb_device_s *dev, uint8_t pin);
void bflb_pec_pad_oe_inv_disable_bit(struct bflb_device_s *dev, uint8_t pin);
uint8_t bflb_pec_pad_oe_inv_enable_get_bit(struct bflb_device_s *dev, uint8_t pin);
void bflb_pec_pad_oe_inv_enable_mask(struct bflb_device_s *dev, uint64_t mask);
void bflb_pec_pad_oe_inv_disable_mask(struct bflb_device_s *dev, uint64_t mask);
uint64_t bflb_pec_pad_oe_inv_enable_get(struct bflb_device_s *dev);
void bflb_pec_pad_i_inv_enable_bit(struct bflb_device_s *dev, uint8_t pin);
void bflb_pec_pad_i_inv_disable_bit(struct bflb_device_s *dev, uint8_t pin);
uint8_t bflb_pec_pad_i_inv_enable_get_bit(struct bflb_device_s *dev, uint8_t pin);
void bflb_pec_pad_i_inv_enable_mask(struct bflb_device_s *dev, uint64_t mask);
void bflb_pec_pad_i_inv_disable_mask(struct bflb_device_s *dev, uint64_t mask);
uint64_t bflb_pec_pad_i_inv_enable_get(struct bflb_device_s *dev);
/* pad output value and output enable */
void bflb_pec_pad_o_set_bit(struct bflb_device_s *dev, uint8_t pin);
void bflb_pec_pad_o_clr_bit(struct bflb_device_s *dev, uint8_t pin);
uint8_t bflb_pec_pad_o_get_bit(struct bflb_device_s *dev, uint8_t pin);
void bflb_pec_pad_o_set_mask(struct bflb_device_s *dev, uint64_t mask);
void bflb_pec_pad_o_clr_mask(struct bflb_device_s *dev, uint64_t mask);
uint64_t bflb_pec_pad_o_get(struct bflb_device_s *dev);
void bflb_pec_pad_oe_set_bit(struct bflb_device_s *dev, uint8_t pin);
void bflb_pec_pad_oe_clr_bit(struct bflb_device_s *dev, uint8_t pin);
uint8_t bflb_pec_pad_oe_get_bit(struct bflb_device_s *dev, uint8_t pin);
void bflb_pec_pad_oe_set_mask(struct bflb_device_s *dev, uint64_t mask);
void bflb_pec_pad_oe_clr_mask(struct bflb_device_s *dev, uint64_t mask);
uint64_t bflb_pec_pad_oe_get(struct bflb_device_s *dev);

/* interrupt */
uint16_t bflb_pec_int_status_get_raw(struct bflb_device_s *dev, uint16_t int_mask);
uint16_t bflb_pec_int0_status_get(struct bflb_device_s *dev, uint16_t int_mask);
void bflb_pec_int0_enable(struct bflb_device_s *dev, uint16_t int_mask);
void bflb_pec_int0_disable(struct bflb_device_s *dev, uint16_t int_mask);
uint16_t bflb_pec_int0_enable_get(struct bflb_device_s *dev, uint16_t int_mask);
uint16_t bflb_pec_int1_status_get(struct bflb_device_s *dev, uint16_t int_mask);
void bflb_pec_int1_enable(struct bflb_device_s *dev, uint16_t int_mask);
void bflb_pec_int1_disable(struct bflb_device_s *dev, uint16_t int_mask);
uint16_t bflb_pec_int1_enable_get(struct bflb_device_s *dev, uint16_t int_mask);

/* state machine register r/w */
void bflb_pec_sm_reg_write(struct bflb_device_s *dev, uint8_t idx, uint32_t val);
uint32_t bflb_pec_sm_reg_read(struct bflb_device_s *dev, uint8_t idx);
void bflb_pec_sm_reg_set_bit(struct bflb_device_s *dev, uint8_t idx, uint8_t bit);
void bflb_pec_sm_reg_clr_bit(struct bflb_device_s *dev, uint8_t idx, uint8_t bit);
uint8_t bflb_pec_sm_reg_get_bit(struct bflb_device_s *dev, uint8_t idx, uint8_t bit);
void bflb_pec_sm_regl_rs(struct bflb_device_s *dev, uint8_t idx, uint32_t val);
void bflb_pec_sm_regh_rs(struct bflb_device_s *dev, uint8_t idx, uint32_t val);

/* dma */
void bflb_pec_dma_enable_tx(struct bflb_device_s *dev);
void bflb_pec_dma_enable_rx(struct bflb_device_s *dev);
void bflb_pec_dma_disable_tx(struct bflb_device_s *dev);
void bflb_pec_dma_disable_rx(struct bflb_device_s *dev);

/* common register */
void bflb_pec_reg_write(struct bflb_device_s *dev, uint32_t offset, uint32_t value);
uint32_t bflb_pec_reg_read(struct bflb_device_s *dev, uint32_t offset);

#ifdef __cplusplus
}
#endif

#endif /* __BFLB_PEC_V2_H__ */
