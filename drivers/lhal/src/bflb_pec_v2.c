#include "bflb_pec_v2.h"
#include "hardware/pec_v2_reg.h"

/****************************************************************************/ /**
 * @brief  PEC initialization function
 *
 * @param  dev: device handle
 * @param  cfg: pec configuration structure pointer
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_init(struct bflb_device_s *dev, struct bflb_pec_cfg_s *cfg)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    /* Step 1: disable state machine */
    regval = getreg32(reg_base + PEC_RESET_OFFSET);
    regval &= ~(PEC_CR_SM_EN << (dev->sub_idx * PEC_RESET_SM_OFFSET));
    putreg32(regval, reg_base + PEC_RESET_OFFSET);

    /* Step 2: reset state machine */
    regval = getreg32(reg_base + PEC_RESET_OFFSET);
    regval |= (PEC_CR_SM_RESET << (dev->sub_idx * PEC_RESET_SM_OFFSET));
    putreg32(regval, reg_base + PEC_RESET_OFFSET);

    /* Step 3: reset clock division of state machine */
    regval = getreg32(reg_base + PEC_RESET_OFFSET);
    regval |= (PEC_CR_SM_CLKDIV_RESET << (dev->sub_idx * PEC_RESET_SM_OFFSET));
    putreg32(regval, reg_base + PEC_RESET_OFFSET);

    /* Step 4: config clock division */
    regval = getreg32(reg_base + PEC_SM_CLKDIV_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    regval &= ~PEC_CR_SM_CLKDIV_MASK;
    regval |= (cfg->clkdiv << PEC_CR_SM_CLKDIV_SHIFT);
    putreg32(regval, reg_base + PEC_SM_CLKDIV_OFFSET + dev->sub_idx * PEC_SM_OFFSET);

    /* Step 5: config origin PC(program counter) */
    regval = getreg32(reg_base + PEC_ORIGIN_OFFSET);
    regval &= ~(PEC_CR_SM_ORIGIN_MASK << (PEC_CR_SM_ORIGIN_SHIFT + dev->sub_idx * PEC_ORIGIN_SM_OFFSET));
    regval |= (cfg->pc_origin << (PEC_CR_SM_ORIGIN_SHIFT + dev->sub_idx * PEC_ORIGIN_SM_OFFSET));
    putreg32(regval, reg_base + PEC_ORIGIN_OFFSET);

    /* Step 6: config0 settings for state machine */
    regval = getreg32(reg_base + PEC_SM_CFG0_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    /* allow instructions to perform sideset optionally, rather than on every instruction */
    if (cfg->side_en) {
        regval |= PEC_CR_SM_SIDE_EN;
    } else {
        regval &= ~PEC_CR_SM_SIDE_EN;
    }
    /* side data is asserted to pin OEs or pin values */
    if (cfg->side_pindir) {
        regval |= PEC_CR_SM_SIDE_PINDIR;
    } else {
        regval &= ~PEC_CR_SM_SIDE_PINDIR;
    }
    /* if rx_fifo and tx_fifo join */
    if (cfg->fifo_join == PEC_FIFO_RX_JOIN_TX) {
        regval |= PEC_CR_SM_FIFO_RX_JOIN_TX;
        regval &= ~PEC_CR_SM_FIFO_TX_JOIN_RX;
    } else if (cfg->fifo_join == PEC_FIFO_TX_JOIN_RX) {
        regval |= PEC_CR_SM_FIFO_TX_JOIN_RX;
        regval &= ~PEC_CR_SM_FIFO_RX_JOIN_TX;
    } else {
        regval &= ~PEC_CR_SM_FIFO_TX_JOIN_RX;
        regval &= ~PEC_CR_SM_FIFO_RX_JOIN_TX;
    }
    /* shift OSR direction */
    if (cfg->out_dir == PEC_SHIFT_DIR_TO_LEFT) {
        regval &= ~PEC_CR_SM_OUT_SHIFTDIR;
    } else {
        regval |= PEC_CR_SM_OUT_SHIFTDIR;
    }
    /* shift ISR direction */
    if (cfg->in_dir == PEC_SHIFT_DIR_TO_LEFT) {
        regval &= ~PEC_CR_SM_IN_SHIFTDIR;
    } else {
        regval |= PEC_CR_SM_IN_SHIFTDIR;
    }
    /* pull automatically when the output shift register is emptied */
    if (cfg->auto_pull) {
        regval |= PEC_CR_SM_AUTOPULL;
    } else {
        regval &= ~PEC_CR_SM_AUTOPULL;
    }
    /* push automatically when the input shift register is filled */
    if (cfg->auto_push) {
        regval |= PEC_CR_SM_AUTOPUSH;
    } else {
        regval &= ~PEC_CR_SM_AUTOPUSH;
    }
    /* after reaching this address, execution is wrapped to wrap_bottom */
    regval &= ~(PEC_CR_SM_WRAP_TOP_MASK);
    regval |= (cfg->wrap_top << PEC_CR_SM_WRAP_TOP_SHIFT);
    /* ater reaching wrap_top, execution is wrapped to this address */
    regval &= ~(PEC_CR_SM_WRAP_BOTTOM_MASK);
    regval |= (cfg->wrap_bottom << PEC_CR_SM_WRAP_BOTTOM_SHIFT);
    /* clear fifo */
    regval |= PEC_CR_SM_RXF_CLR;
    regval |= PEC_CR_SM_TXF_CLR;
    putreg32(regval, reg_base + PEC_SM_CFG0_OFFSET + dev->sub_idx * PEC_SM_OFFSET);

    /* Step 7: config1 settings for state machine */
    regval = getreg32(reg_base + PEC_SM_CFG1_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    /* dma enable or disable */
    if (cfg->dma_rx) {
        regval |= PEC_CR_SM_DMA_RX_EN;
    } else {
        regval &= ~PEC_CR_SM_DMA_RX_EN;
    }
    if (cfg->dma_tx) {
        regval |= PEC_CR_SM_DMA_TX_EN;
    } else {
        regval &= ~PEC_CR_SM_DMA_TX_EN;
    }
    /* number of bits shifted out of TXSR before autopull or conditional pull, 1~31, 0 present 32 */
    regval &= ~PEC_CR_SM_PULL_THRESH_MASK;
    regval |= ((cfg->thresh_bits_pull & 0x1F) << PEC_CR_SM_PULL_THRESH_SHIFT);
    /* number of bits shifted into RXSR before autopush or conditional push, 1~31, 0 present 32 */
    regval &= ~PEC_CR_SM_PUSH_THRESH_MASK;
    regval |= ((cfg->thresh_bits_push & 0x1F) << PEC_CR_SM_PUSH_THRESH_SHIFT);
    putreg32(regval, reg_base + PEC_SM_CFG1_OFFSET + dev->sub_idx * PEC_SM_OFFSET);

    /* Step 8: config fifo threshold */
    regval = getreg32(reg_base + PEC_SM_FIFO_CTRL_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    regval &= ~PEC_CR_SM_RXF_THR_MASK;
    regval |= ((cfg->thresh_fifo_rx & 0x1F) << PEC_CR_SM_RXF_THR_SHIFT);
    regval &= ~PEC_CR_SM_TXF_THR_MASK;
    regval |= ((cfg->thresh_fifo_tx & 0x1F) << PEC_CR_SM_TXF_THR_SHIFT);
    putreg32(regval, reg_base + PEC_SM_FIFO_CTRL_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
}

/****************************************************************************/ /**
 * @brief  enable state machine
 *
 * @param  dev: device handle
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_enable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_RESET_OFFSET);
    regval |= (PEC_CR_SM_EN << (dev->sub_idx * PEC_RESET_SM_OFFSET));
    putreg32(regval, reg_base + PEC_RESET_OFFSET);
}

/****************************************************************************/ /**
 * @brief  disable state machine
 *
 * @param  dev: device handle
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_disable(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    /* Step 1: disable state machine */
    regval = getreg32(reg_base + PEC_RESET_OFFSET);
    regval &= ~(PEC_CR_SM_EN << (dev->sub_idx * PEC_RESET_SM_OFFSET));
    putreg32(regval, reg_base + PEC_RESET_OFFSET);

    /* Step 2: reset state machine */
    regval = getreg32(reg_base + PEC_RESET_OFFSET);
    regval |= (PEC_CR_SM_RESET << (dev->sub_idx * PEC_RESET_SM_OFFSET));
    putreg32(regval, reg_base + PEC_RESET_OFFSET);
}

/****************************************************************************/ /**
 * @brief  enable multiple state machine
 *
 * @param  dev: device handle
 * @param  sm_mask: state machine ID mask, bit0~3 orderly represent sm0~3
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_enable_mask(struct bflb_device_s *dev, uint8_t sm_mask)
{
    uint32_t reg_base;
    uint32_t regval;
    uint32_t mask;

    reg_base = dev->reg_base;
    mask = 0;
    for (uint8_t i = 0; i < PEC_SM_MAX; i++) {
        if (sm_mask & (1 << i)) {
            mask |= (PEC_CR_SM_EN << (i * PEC_RESET_SM_OFFSET));
        }
    }
    regval = getreg32(reg_base + PEC_RESET_OFFSET);
    regval |= mask;
    putreg32(regval, reg_base + PEC_RESET_OFFSET);
}

/****************************************************************************/ /**
 * @brief  disable multiple state machine
 *
 * @param  dev: device handle
 * @param  sm_mask: state machine ID mask, bit0~3 orderly represent sm0~3
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_disable_mask(struct bflb_device_s *dev, uint8_t sm_mask)
{
    uint32_t reg_base;
    uint32_t regval;
    uint32_t mask;

    reg_base = dev->reg_base;
    /* Step 1: disable state machine */
    mask = 0;
    for (uint8_t i = 0; i < PEC_SM_MAX; i++) {
        if (sm_mask & (1 << i)) {
            mask |= (PEC_CR_SM_EN << (i * PEC_RESET_SM_OFFSET));
        }
    }
    regval = getreg32(reg_base + PEC_RESET_OFFSET);
    regval &= ~mask;
    putreg32(regval, reg_base + PEC_RESET_OFFSET);

    /* Step 2: reset state machine */
    mask = 0;
    for (uint8_t i = 0; i < PEC_SM_MAX; i++) {
        if (sm_mask & (1 << i)) {
            mask |= (PEC_CR_SM_RESET << (i * PEC_RESET_SM_OFFSET));
        }
    }
    regval = getreg32(reg_base + PEC_RESET_OFFSET);
    regval |= mask;
    putreg32(regval, reg_base + PEC_RESET_OFFSET);
}

/****************************************************************************/ /**
 * @brief  set clock divisor for state machine
 *
 * @param  dev: device handle
 * @param  div: clock divisor, Frequency = clock freq / (div + 1)
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_clock_div_set(struct bflb_device_s *dev, uint16_t div)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_CLKDIV_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    regval &= ~PEC_CR_SM_CLKDIV_MASK;
    regval |= (div << PEC_CR_SM_CLKDIV_SHIFT);
    putreg32(regval, reg_base + PEC_SM_CLKDIV_OFFSET + dev->sub_idx * PEC_SM_OFFSET);

    regval = getreg32(reg_base + PEC_RESET_OFFSET);
    regval |= (PEC_CR_SM_CLKDIV_RESET << (dev->sub_idx * PEC_RESET_SM_OFFSET));
    putreg32(regval, reg_base + PEC_RESET_OFFSET);
}

/****************************************************************************/ /**
 * @brief  get clock divisor for state machine
 *
 * @param  dev: device handle
 *
 * @return clock divisor, Frequency = clock freq / (div + 1)
 *
*******************************************************************************/
uint16_t bflb_pec_clock_div_get(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_CLKDIV_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    regval &= PEC_CR_SM_CLKDIV_MASK;
    regval >>= PEC_CR_SM_CLKDIV_SHIFT;
    return (uint16_t)regval;
}

/****************************************************************************/ /**
 * @brief  get current program counter
 *
 * @param  dev: device handle
 *
 * @return current program counter
 *
*******************************************************************************/
uint8_t bflb_pec_current_pc_get(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_CFG1_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    regval &= PEC_ST_SM_ADDR_MASK;
    regval >>= PEC_ST_SM_ADDR_SHIFT;
    return (uint8_t)regval;
}

/****************************************************************************/ /**
 * @brief  get current instruction code
 *
 * @param  dev: device handle
 *
 * @return current instruction code
 *
*******************************************************************************/
uint32_t bflb_pec_current_instr_get(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_INSTR_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    return regval;
}

/****************************************************************************/ /**
 * @brief  insert an instruction to state machine
 *
 * @param  dev: device handle
 * @param  instr: instruction to be inserted
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_instr_insert(struct bflb_device_s *dev, uint32_t instr)
{
    uint32_t reg_base;

    reg_base = dev->reg_base;
    putreg32(instr, reg_base + PEC_SM_INSTR_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
}

/****************************************************************************/ /**
 * @brief  Check whether the inserted instruction is stalled
 *
 * @param  dev: device handle
 *
 * @return 0: instruction is completed, 1: instruction is stalled
 *
*******************************************************************************/
uint8_t bflb_pec_is_stalled(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_CFG0_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    if (regval & PEC_ST_SM_EXEC_STALLED) {
        return 1;
    } else {
        return 0;
    }
}

/****************************************************************************/ /**
 * @brief  write an instruction to memory
 *
 * @param  dev: device handle
 * @param  addr: address, 0~255
 * @param  instr: instruction
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_mem_write(struct bflb_device_s *dev, uint8_t addr, uint32_t instr)
{
    uint32_t reg_base;
#if defined(BL616L)
    uint32_t regval;
#endif

    reg_base = dev->reg_base;
#if defined(BL616L)
    /* enable system bus access */
    putreg32(0x428A2F98, reg_base + 8);
    regval = getreg32(reg_base + PEC_MEM_CFG_OFFSET);
    regval |= (PEC_MEM_SM0_BUS_ACCESS << dev->sub_idx);
    putreg32(regval, reg_base + PEC_MEM_CFG_OFFSET);
#endif
    /* write instruction */
    putreg32(instr, reg_base + PEC_MEM_OFFSET + addr * sizeof(uint32_t));
#if defined(BL616L)
    /* enable state machine hardware access */
    regval = getreg32(reg_base + PEC_MEM_CFG_OFFSET);
    regval &= ~(PEC_MEM_SM0_BUS_ACCESS << dev->sub_idx);
    putreg32(regval, reg_base + PEC_MEM_CFG_OFFSET);
#endif
}

/****************************************************************************/ /**
 * @brief  write multiple instructions to memory
 *
 * @param  dev: device handle
 * @param  addr: start address
 * @param  instr: instruction pointor
 * @param  len: number of instructions
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_mem_write_multi(struct bflb_device_s *dev, uint8_t addr, uint32_t *instr, uint16_t len)
{
    uint32_t reg_base;
#if defined(BL616L)
    uint32_t regval;
#endif

    reg_base = dev->reg_base;
#if defined(BL616L)
    /* enable system bus access */
    regval = getreg32(reg_base + PEC_MEM_CFG_OFFSET);
    regval |= (PEC_MEM_SM0_BUS_ACCESS << dev->sub_idx);
    putreg32(regval, reg_base + PEC_MEM_CFG_OFFSET);
#endif
    /* write instruction */
    for (uint16_t i = 0; i < len; i++) {
        putreg32(instr[i], reg_base + PEC_MEM_OFFSET + (addr + i) * sizeof(uint32_t));
    }
#if defined(BL616L)
    /* enable hardware access */
    regval = getreg32(reg_base + PEC_MEM_CFG_OFFSET);
    regval &= ~(PEC_MEM_SM0_BUS_ACCESS << dev->sub_idx);
    putreg32(regval, reg_base + PEC_MEM_CFG_OFFSET);
#endif
}

/****************************************************************************/ /**
 * @brief  read an instruction from memory
 *
 * @param  dev: device handle
 * @param  addr: address, 0~255
 *
 * @return instruction
 *
*******************************************************************************/
uint32_t bflb_pec_mem_read(struct bflb_device_s *dev, uint8_t addr)
{
    uint32_t reg_base;
    uint32_t instruction;
#if defined(BL616L)
    uint32_t regval;
#endif

    reg_base = dev->reg_base;
#if defined(BL616L)
    /* enable system bus access */
    regval = getreg32(reg_base + PEC_MEM_CFG_OFFSET);
    regval |= (PEC_MEM_SM0_BUS_ACCESS << dev->sub_idx);
    putreg32(regval, reg_base + PEC_MEM_CFG_OFFSET);
#endif
    instruction = getreg32(reg_base + PEC_MEM_OFFSET + addr * sizeof(uint32_t));
#if defined(BL616L)
    /* enable hardware access */
    regval = getreg32(reg_base + PEC_MEM_CFG_OFFSET);
    regval &= ~(PEC_MEM_SM0_BUS_ACCESS << dev->sub_idx);
    putreg32(regval, reg_base + PEC_MEM_CFG_OFFSET);
#endif
    return instruction;
}

/****************************************************************************/ /**
 * @brief  store address of first instructions to internal software register
 *
 * @param  dev: device handle
 * @param  addr: start address
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_mem_store(struct bflb_device_s *dev, uint8_t addr)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SW_RESV_OFFSET);
    regval &= ~(0xFF << dev->sub_idx * 8);
    regval |= (addr << dev->sub_idx * 8);
    putreg32(regval, reg_base + PEC_SW_RESV_OFFSET);
}

/****************************************************************************/ /**
 * @brief  restore address of first instructions from internal software register
 *
 * @param  dev: device handle
 *
 * @return address of first instructions
 *
*******************************************************************************/
uint8_t bflb_pec_mem_restore(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SW_RESV_OFFSET);
    regval >>= (dev->sub_idx * 8);
    regval &= 0xFF;
    return (uint8_t)regval;
}

/****************************************************************************/ /**
 * @brief  set one bit of IPC register
 *
 * @param  dev: device handle
 * @param  bit: which bit to be set, 0~31
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_ipc_set_bit(struct bflb_device_s *dev, uint8_t bit)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = (1U << bit);
    putreg32(regval, reg_base + PEC_IPC_SET_OFFSET);
}

/****************************************************************************/ /**
 * @brief  clear one bit of IPC register
 *
 * @param  dev: device handle
 * @param  bit: which bit to be cleared, 0~31
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_ipc_clr_bit(struct bflb_device_s *dev, uint8_t bit)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = (1U << bit);
    putreg32(regval, reg_base + PEC_IPC_CLR_OFFSET);
}

/****************************************************************************/ /**
 * @brief  get one bit status of IPC register
 *
 * @param  dev: device handle
 * @param  bit: which bit to be set, 0~31
 *
 * @return status, 0 or 1
 *
*******************************************************************************/
uint8_t bflb_pec_ipc_get_bit(struct bflb_device_s *dev, uint8_t bit)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_IPC_CLR_OFFSET);
    if (regval & (1U << bit)) {
        return 1;
    } else {
        return 0;
    }
}

/****************************************************************************/ /**
 * @brief  set multiple bits to IPC register
 *
 * @param  dev: device handle
 * @param  mask: mask value, bits which is 1 will be set
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_ipc_set_mask(struct bflb_device_s *dev, uint32_t mask)
{
    uint32_t reg_base;

    reg_base = dev->reg_base;
    putreg32(mask, reg_base + PEC_IPC_SET_OFFSET);
}

/****************************************************************************/ /**
 * @brief  clear multiple bits to IPC register
 *
 * @param  dev: device handle
 * @param  mask: mask value, bits which is 1 will be cleared
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_ipc_clr_mask(struct bflb_device_s *dev, uint32_t mask)
{
    uint32_t reg_base;

    reg_base = dev->reg_base;
    putreg32(mask, reg_base + PEC_IPC_CLR_OFFSET);
}

/****************************************************************************/ /**
 * @brief  get status of IPC register
 *
 * @param  dev: device handle
 *
 * @return status of IPC register
 *
*******************************************************************************/
uint32_t bflb_pec_ipc_get(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_IPC_CLR_OFFSET);
    return regval;
}

/****************************************************************************/ /**
 * @brief  enable one bit of IPC to triger interrupt
 *
 * @param  dev: device handle
 * @param  bit: which bit to be enabled
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_ipc_int_enable_bit(struct bflb_device_s *dev, uint8_t bit)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_INT_IPC_EN_OFFSET);
    regval |= (1U << bit);
    putreg32(regval, reg_base + PEC_INT_IPC_EN_OFFSET);
}

/****************************************************************************/ /**
 * @brief  disable one bit of IPC to triger interrupt
 *
 * @param  dev: device handle
 * @param  bit: which bit to be disabled
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_ipc_int_disable_bit(struct bflb_device_s *dev, uint8_t bit)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_INT_IPC_EN_OFFSET);
    regval &= ~(1U << bit);
    putreg32(regval, reg_base + PEC_INT_IPC_EN_OFFSET);
}

/****************************************************************************/ /**
 * @brief  get status of that whether one bit of IPC to triger interrupt
 *
 * @param  dev: device handle
 * @param  bit: which bit to be enabled
 *
 * @return interrupt enable status of one bit
 *
*******************************************************************************/
uint8_t bflb_pec_ipc_int_enable_get_bit(struct bflb_device_s *dev, uint8_t bit)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_INT_IPC_EN_OFFSET);
    if (regval & (1U << bit)) {
        return 1;
    } else {
        return 0;
    }
}

/****************************************************************************/ /**
 * @brief  enable multiple bits of IPC to triger interrupt
 *
 * @param  dev: device handle
 * @param  mask: mask value, bits which is 1 will enable interrupt
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_ipc_int_enable_mask(struct bflb_device_s *dev, uint32_t mask)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_INT_IPC_EN_OFFSET);
    regval |= mask;
    putreg32(regval, reg_base + PEC_INT_IPC_EN_OFFSET);
}

/****************************************************************************/ /**
 * @brief  disable multiple bits of IPC to triger interrupt
 *
 * @param  dev: device handle
 * @param  mask: mask value, bits which is 1 will disable interrupt
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_ipc_int_disable_mask(struct bflb_device_s *dev, uint32_t mask)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_INT_IPC_EN_OFFSET);
    regval &= ~mask;
    putreg32(regval, reg_base + PEC_INT_IPC_EN_OFFSET);
}

/****************************************************************************/ /**
 * @brief  get status of that what bits of IPC to triger interrupt
 *
 * @param  dev: device handle
 *
 * @return interrupt enable status
 *
*******************************************************************************/
uint32_t bflb_pec_ipc_int_enable_get(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_INT_IPC_EN_OFFSET);
    return regval;
}

/****************************************************************************/ /**
 * @brief  enable 2-ff synchronizer bypass of one pin
 *
 * @param  dev: device handle
 * @param  pin: which pin to be bypassed, 0~63 is valid value
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_sync_bypass_enable_bit(struct bflb_device_s *dev, uint8_t pin)
{
    uint32_t reg_base;
    uint32_t regval;

    pin = pin % PEC_PIN_MAX;
    reg_base = dev->reg_base;
    reg_base += PEC_INPUT_SYNC_BYPASS_OFFSET;
    if (pin >= 32) {
        pin = pin % 32;
        reg_base += sizeof(uint32_t);
    }
    regval = getreg32(reg_base);
    regval |= (1U << pin);
    putreg32(regval, reg_base);
}

/****************************************************************************/ /**
 * @brief  disable 2-ff synchronizer bypass of one pin
 *
 * @param  dev: device handle
 * @param  pin: which pin to be not bypassed, 0~63 is valid value
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_sync_bypass_disable_bit(struct bflb_device_s *dev, uint8_t pin)
{
    uint32_t reg_base;
    uint32_t regval;

    pin = pin % PEC_PIN_MAX;
    reg_base = dev->reg_base;
    reg_base += PEC_INPUT_SYNC_BYPASS_OFFSET;
    if (pin >= 32) {
        pin = pin % 32;
        reg_base += sizeof(uint32_t);
    }
    regval = getreg32(reg_base);
    regval &= ~(1U << pin);
    putreg32(regval, reg_base);
}

/****************************************************************************/ /**
 * @brief  get 2-ff synchronizer bypass value of one pin
 *
 * @param  dev: device handle
 * @param  pin: which pin to be get, 0~63 is valid value
 *
 * @return 2-ff synchronizer bypass value
 *
*******************************************************************************/
uint8_t bflb_pec_sync_bypass_enable_get_bit(struct bflb_device_s *dev, uint8_t pin)
{
    uint32_t reg_base;
    uint32_t regval;

    pin = pin % PEC_PIN_MAX;
    reg_base = dev->reg_base;
    reg_base += PEC_INPUT_SYNC_BYPASS_OFFSET;
    if (pin >= 32) {
        pin = pin % 32;
        reg_base += sizeof(uint32_t);
    }
    regval = getreg32(reg_base);
    if (regval & (1U << pin)) {
        return 1;
    } else {
        return 0;
    }
}

/****************************************************************************/ /**
 * @brief  enable 2-ff synchronizer bypass of multiple pins
 *
 * @param  dev: device handle
 * @param  mask: which pins to be bypassed
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_sync_bypass_enable_mask(struct bflb_device_s *dev, uint64_t mask)
{
    uint32_t reg_base;
    uint32_t regval;
    uint32_t part;

    reg_base = dev->reg_base;
    reg_base += PEC_INPUT_SYNC_BYPASS_OFFSET;
    part = (uint32_t)mask & 0xFFFFFFFF;
    if (part) {
        regval = getreg32(reg_base);
        regval |= part;
        putreg32(regval, reg_base);
    }
    part = (uint32_t)(mask >> 32) & 0xFFFFFFFF;
    if (part) {
        reg_base += sizeof(uint32_t);
        regval = getreg32(reg_base);
        regval |= part;
        putreg32(regval, reg_base);
    }
}

/****************************************************************************/ /**
 * @brief  disable 2-ff synchronizer bypass of multiple pins
 *
 * @param  dev: device handle
 * @param  mask: which pins to be not bypassed
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_sync_bypass_disable_mask(struct bflb_device_s *dev, uint64_t mask)
{
    uint32_t reg_base;
    uint32_t regval;
    uint32_t part;

    reg_base = dev->reg_base;
    reg_base += PEC_INPUT_SYNC_BYPASS_OFFSET;
    part = (uint32_t)mask & 0xFFFFFFFF;
    if (part) {
        regval = getreg32(reg_base);
        regval &= ~part;
        putreg32(regval, reg_base);
    }
    part = (uint32_t)(mask >> 32) & 0xFFFFFFFF;
    if (part) {
        reg_base += sizeof(uint32_t);
        regval = getreg32(reg_base);
        regval &= ~part;
        putreg32(regval, reg_base);
    }
}

/****************************************************************************/ /**
 * @brief  get 2-ff synchronizer bypass value of all pins
 *
 * @param  dev: device handle
 *
 * @return 2-ff synchronizer bypass value
 *
*******************************************************************************/
uint64_t bflb_pec_sync_bypass_get(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval_0;
    uint32_t regval_1;

    reg_base = dev->reg_base;
    reg_base += PEC_INPUT_SYNC_BYPASS_OFFSET;
    regval_0 = getreg32(reg_base);
    reg_base += sizeof(uint32_t);
    regval_1 = getreg32(reg_base);
    return ((uint64_t)regval_1 << 32) | (uint64_t)regval_0;
}

/****************************************************************************/ /**
 * @brief  get tx_fifo deepth
 *
 * @param  dev: device handle
 *
 * @return tx_fifo deepth
 *
*******************************************************************************/
uint8_t bflb_pec_fifo_get_deepth_tx(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_CFG0_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    if (regval & PEC_CR_SM_FIFO_TX_JOIN_RX) {
        return 0;
    } else if (regval & PEC_CR_SM_FIFO_RX_JOIN_TX) {
#if defined(BL616L)
        if (dev->sub_idx == 0) {
            return (PEC_FIFO_DEEPTH * 4 + PEC_FIFO_DEEPTH * 2);
        } else {
            return (2 * PEC_FIFO_DEEPTH);
        }
    } else {
        if (dev->sub_idx == 0) {
            return (PEC_FIFO_DEEPTH * 4);
        } else {
            return PEC_FIFO_DEEPTH;
        }
#elif defined(BL616D)
        return (2 * PEC_FIFO_DEEPTH);
    } else {
        return PEC_FIFO_DEEPTH;
#endif
    }
}

/****************************************************************************/ /**
 * @brief  get rx_fifo deepth
 *
 * @param  dev: device handle
 *
 * @return rx_fifo deepth
 *
*******************************************************************************/
uint8_t bflb_pec_fifo_get_deepth_rx(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_CFG0_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    if (regval & PEC_CR_SM_FIFO_RX_JOIN_TX) {
        return 0;
    } else if (regval & PEC_CR_SM_FIFO_TX_JOIN_RX) {
#if defined(BL616L)
        if (dev->sub_idx == 0) {
            return (PEC_FIFO_DEEPTH * 4 + PEC_FIFO_DEEPTH * 2);
        } else {
            return (2 * PEC_FIFO_DEEPTH);
        }
    } else {
        if (dev->sub_idx == 0) {
            return (PEC_FIFO_DEEPTH * 2);
        } else {
            return PEC_FIFO_DEEPTH;
        }
#elif defined(BL616D)
        return (2 * PEC_FIFO_DEEPTH);
    } else {
        return PEC_FIFO_DEEPTH;
#endif
    }
}

/****************************************************************************/ /**
 * @brief  get tx_fifo level
 *
 * @param  dev: device handle
 *
 * @return tx_fifo level
 *
*******************************************************************************/
uint8_t bflb_pec_fifo_get_level_tx(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_FIFO_CTRL_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    regval &= PEC_ST_SM_TX_LEVEL_MASK;
    regval >>= PEC_ST_SM_TX_LEVEL_SHIFT;
    return (uint8_t)regval;
}

/****************************************************************************/ /**
 * @brief  get rx_fifo level
 *
 * @param  dev: device handle
 *
 * @return rx_fifo level
 *
*******************************************************************************/
uint8_t bflb_pec_fifo_get_level_rx(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_FIFO_CTRL_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    regval &= PEC_ST_SM_RX_LEVEL_MASK;
    regval >>= PEC_ST_SM_RX_LEVEL_SHIFT;
    return (uint8_t)regval;
}

/****************************************************************************/ /**
 * @brief  clear tx_fifo
 *
 * @param  dev: device handle
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_fifo_clr_tx(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_CFG0_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    regval |= PEC_CR_SM_TXF_CLR;
    putreg32(regval, reg_base + PEC_SM_CFG0_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
}

/****************************************************************************/ /**
 * @brief  clear rx_fifo
 *
 * @param  dev: device handle
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_fifo_clr_rx(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_CFG0_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    regval |= PEC_CR_SM_RXF_CLR;
    putreg32(regval, reg_base + PEC_SM_CFG0_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
}

/****************************************************************************/ /**
 * @brief  check whether tx_fifo is ready
 *
 * @param  dev: device handle
 *
 * @return 0: not ready, 1: ready
 *
*******************************************************************************/
uint8_t bflb_pec_fifo_is_rdy_tx(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_INT_RAW_OFFSET);
    regval &= (PEC_ST_INTR_SM_TXF_RDY << dev->sub_idx);
    if (regval) {
        return 1;
    } else {
        return 0;
    }
}

/****************************************************************************/ /**
 * @brief  check whether rx_fifo is ready
 *
 * @param  dev: device handle
 *
 * @return 0: not ready, 1: ready
 *
*******************************************************************************/
uint8_t bflb_pec_fifo_is_rdy_rx(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_INT_RAW_OFFSET);
    regval &= (PEC_ST_INTR_SM_RXF_RDY << dev->sub_idx);
    if (regval) {
        return 1;
    } else {
        return 0;
    }
}

/****************************************************************************/ /**
 * @brief  get tx_fifo address
 *
 * @param  dev: device handle
 *
 * @return tx_fifo address
 *
*******************************************************************************/
uint32_t bflb_pec_fifo_get_addr_tx(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = reg_base + PEC_TXF_OFFSET + dev->sub_idx * sizeof(uint32_t);
    return regval;
}

/****************************************************************************/ /**
 * @brief  get rx_fifo address
 *
 * @param  dev: device handle
 *
 * @return rx_fifo address
 *
*******************************************************************************/
uint32_t bflb_pec_fifo_get_addr_rx(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = reg_base + PEC_RXF_OFFSET + dev->sub_idx * sizeof(uint32_t);
    return regval;
}

/****************************************************************************/ /**
 * @brief  write a 32-bits value to tx_fifo
 *
 * @param  dev: device handle
 * @param  value: value to be written
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_fifo_write(struct bflb_device_s *dev, uint32_t value)
{
    uint32_t reg_base;

    reg_base = dev->reg_base;
    putreg32(value, reg_base + PEC_TXF_OFFSET + dev->sub_idx * sizeof(uint32_t));
}

/****************************************************************************/ /**
 * @brief  read a 32-bits value from rx_fifo
 *
 * @param  dev: device handle
 *
 * @return value
 *
*******************************************************************************/
uint32_t bflb_pec_fifo_read(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_RXF_OFFSET + dev->sub_idx * sizeof(uint32_t));
    return regval;
}

/****************************************************************************/ /**
 * @brief  write multi 32-bits value to tx_fifo
 *
 * @param  dev: device handle
 * @param  data: pointor of data to be written
 * @param  len: length of data to be written
 *
 * @return actual length of data written
 *
*******************************************************************************/
uint32_t bflb_pec_fifo_write_multi_32bits(struct bflb_device_s *dev, uint32_t *data, uint32_t len)
{
    uint32_t addr;
    uint8_t deepth, level, idx;

    deepth = bflb_pec_fifo_get_deepth_tx(dev);
    addr = dev->reg_base + PEC_TXF_OFFSET + dev->sub_idx * sizeof(uint32_t);
    level = bflb_pec_fifo_get_level_tx(dev);
    if (level >= deepth) {
        return 0;
    }
    level = deepth - level;
    level = (len > level) ? level : (uint8_t)len;
    for (idx = 0; idx < level; idx++) {
        putreg32(data[idx], addr);
    }
    return (uint32_t)idx;
}

/****************************************************************************/ /**
 * @brief  write multi 16-bits value to tx_fifo
 *
 * @param  dev: device handle
 * @param  data: pointor of data to be written
 * @param  len: length of data to be written
 *
 * @return actual length of data written
 *
*******************************************************************************/
uint32_t bflb_pec_fifo_write_multi_16bits(struct bflb_device_s *dev, uint16_t *data, uint32_t len)
{
    uint32_t addr;
    uint8_t deepth, level, idx;

    deepth = bflb_pec_fifo_get_deepth_tx(dev);
    addr = dev->reg_base + PEC_TXF_OFFSET + dev->sub_idx * sizeof(uint32_t);
    level = bflb_pec_fifo_get_level_tx(dev);
    if (level >= deepth) {
        return 0;
    }
    level = deepth - level;
    level = (len > level) ? level : (uint8_t)len;
    for (idx = 0; idx < level; idx++) {
        putreg32((uint32_t)(data[idx]), addr);
    }
    return (uint32_t)idx;
}

/****************************************************************************/ /**
 * @brief  write multi 8-bits value to tx_fifo
 *
 * @param  dev: device handle
 * @param  data: pointor of data to be written
 * @param  len: length of data to be written
 *
 * @return actual length of data written
 *
*******************************************************************************/
uint32_t bflb_pec_fifo_write_multi_8bits(struct bflb_device_s *dev, uint8_t *data, uint32_t len)
{
    uint32_t addr;
    uint8_t deepth, level, idx;

    deepth = bflb_pec_fifo_get_deepth_tx(dev);
    addr = dev->reg_base + PEC_TXF_OFFSET + dev->sub_idx * sizeof(uint32_t);
    level = bflb_pec_fifo_get_level_tx(dev);
    if (level >= deepth) {
        return 0;
    }
    level = deepth - level;
    level = (len > level) ? level : (uint8_t)len;
    for (idx = 0; idx < level; idx++) {
        putreg32((uint32_t)(data[idx]), addr);
    }
    return (uint32_t)idx;
}

/****************************************************************************/ /**
 * @brief  write multi 32-bits value to tx_fifo with blocking mode
 *
 * @param  dev: device handle
 * @param  data: pointor of data to be written
 * @param  len: length of data to be written
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_fifo_write_multi_32bits_blocking(struct bflb_device_s *dev, uint32_t *data, uint32_t len)
{
    uint32_t addr_fifo, addr_level, value;
    uint32_t index;
    uint8_t deepth, level;
    
    deepth = bflb_pec_fifo_get_deepth_tx(dev);
    addr_fifo = dev->reg_base + PEC_TXF_OFFSET + dev->sub_idx * sizeof(uint32_t);
    addr_level = dev->reg_base + PEC_SM_FIFO_CTRL_OFFSET + dev->sub_idx * PEC_SM_OFFSET;
    index = 0;
    while (1) {
        value = getreg32(addr_level);
        value &= PEC_ST_SM_TX_LEVEL_MASK;
        value >>= PEC_ST_SM_TX_LEVEL_SHIFT;
        level = (uint8_t)value;
        while (level < deepth && index < len) {
            putreg32(data[index], addr_fifo);
            index++;
            level++;
        }
        if (index >= len) {
            return;
        }
    }
}

/****************************************************************************/ /**
 * @brief  write multi 16-bits value to tx_fifo with blocking mode
 *
 * @param  dev: device handle
 * @param  data: pointor of data to be written
 * @param  len: length of data to be written
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_fifo_write_multi_16bits_blocking(struct bflb_device_s *dev, uint16_t *data, uint32_t len)
{
    uint32_t addr_fifo, addr_level, value;
    uint32_t index;
    uint8_t deepth, level;
    
    deepth = bflb_pec_fifo_get_deepth_tx(dev);
    addr_fifo = dev->reg_base + PEC_TXF_OFFSET + dev->sub_idx * sizeof(uint32_t);
    addr_level = dev->reg_base + PEC_SM_FIFO_CTRL_OFFSET + dev->sub_idx * PEC_SM_OFFSET;
    index = 0;
    while (1) {
        value = getreg32(addr_level);
        value &= PEC_ST_SM_TX_LEVEL_MASK;
        value >>= PEC_ST_SM_TX_LEVEL_SHIFT;
        level = (uint8_t)value;
        while (level < deepth && index < len) {
            putreg32((uint32_t)(data[index]), addr_fifo);
            index++;
            level++;
        }
        if (index >= len) {
            return;
        }
    }
}

/****************************************************************************/ /**
 * @brief  write multi 8-bits value to tx_fifo with blocking mode
 *
 * @param  dev: device handle
 * @param  data: pointor of data to be written
 * @param  len: length of data to be written
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_fifo_write_multi_8bits_blocking(struct bflb_device_s *dev, uint8_t *data, uint32_t len)
{
    uint32_t addr_fifo, addr_level, value;
    uint32_t index;
    uint8_t deepth, level;
    
    deepth = bflb_pec_fifo_get_deepth_tx(dev);
    addr_fifo = dev->reg_base + PEC_TXF_OFFSET + dev->sub_idx * sizeof(uint32_t);
    addr_level = dev->reg_base + PEC_SM_FIFO_CTRL_OFFSET + dev->sub_idx * PEC_SM_OFFSET;
    index = 0;
    while (1) {
        value = getreg32(addr_level);
        value &= PEC_ST_SM_TX_LEVEL_MASK;
        value >>= PEC_ST_SM_TX_LEVEL_SHIFT;
        level = (uint8_t)value;
        while (level < deepth && index < len) {
            putreg32((uint32_t)(data[index]), addr_fifo);
            index++;
            level++;
        }
        if (index >= len) {
            return;
        }
    }
}

/****************************************************************************/ /**
 * @brief  read multi 32-bits value from rx_fifo
 *
 * @param  dev: device handle
 * @param  data: pointor of memory to store read data
 * @param  len: length of data to be read
 *
 * @return actual length of data read
 *
*******************************************************************************/
uint32_t bflb_pec_fifo_read_multi_32bits(struct bflb_device_s *dev, uint32_t *data, uint32_t len)
{
    uint32_t addr;
    uint8_t level, idx;

    level = bflb_pec_fifo_get_level_rx(dev);
    addr = dev->reg_base + PEC_RXF_OFFSET + dev->sub_idx * sizeof(uint32_t);
    level = (len > level) ? level : (uint8_t)len;
    for (idx = 0; idx < level; idx++) {
        data[idx] = getreg32(addr);
    }
    return (uint32_t)idx;
}

/****************************************************************************/ /**
 * @brief  read multi 32-bits value from rx_fifo
 *
 * @param  dev: device handle
 * @param  data: pointor of memory to store read data
 * @param  len: length of data to be read
 *
 * @return actual length of data read
 *
*******************************************************************************/
uint32_t bflb_pec_fifo_read_multi_16bits(struct bflb_device_s *dev, uint16_t *data, uint32_t len)
{
    uint32_t addr;
    uint8_t level, idx;

    level = bflb_pec_fifo_get_level_rx(dev);
    addr = dev->reg_base + PEC_RXF_OFFSET + dev->sub_idx * sizeof(uint32_t);
    level = (len > level) ? level : (uint8_t)len;
    for (idx = 0; idx < level; idx++) {
        data[idx] = (uint16_t)getreg32(addr);
    }
    return (uint32_t)idx;
}

/****************************************************************************/ /**
 * @brief  read multi 8-bits value from rx_fifo
 *
 * @param  dev: device handle
 * @param  data: pointor of memory to store read data
 * @param  len: length of data to be read
 *
 * @return actual length of data read
 *
*******************************************************************************/
uint32_t bflb_pec_fifo_read_multi_8bits(struct bflb_device_s *dev, uint8_t *data, uint32_t len)
{
    uint32_t addr;
    uint8_t level, idx;

    level = bflb_pec_fifo_get_level_rx(dev);
    addr = dev->reg_base + PEC_RXF_OFFSET + dev->sub_idx * sizeof(uint32_t);
    level = (len > level) ? level : (uint8_t)len;
    for (idx = 0; idx < level; idx++) {
        data[idx] = (uint8_t)getreg32(addr);
    }
    return (uint32_t)idx;
}

/****************************************************************************/ /**
 * @brief  set fifo_tx threshold
 *
 * @param  dev: device handle
 * @param  threshold: threshold value
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_fifo_set_threshold_tx(struct bflb_device_s *dev, uint8_t threshold)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_FIFO_CTRL_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    regval &= ~PEC_CR_SM_TXF_THR_MASK;
    regval |= ((threshold & 0x1F) << PEC_CR_SM_TXF_THR_SHIFT);
    putreg32(regval, reg_base + PEC_SM_FIFO_CTRL_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
}

/****************************************************************************/ /**
 * @brief  set fifo_rx threshold
 *
 * @param  dev: device handle
 * @param  threshold: threshold value
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_fifo_set_threshold_rx(struct bflb_device_s *dev, uint8_t threshold)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_FIFO_CTRL_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    regval &= ~PEC_CR_SM_RXF_THR_MASK;
    regval |= ((threshold & 0x1F) << PEC_CR_SM_RXF_THR_SHIFT);
    putreg32(regval, reg_base + PEC_SM_FIFO_CTRL_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
}

/****************************************************************************/ /**
 * @brief  set side-pin base
 *
 * @param  dev: device handle
 * @param  base: side-pin base, 0~63 is valid value
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pin_side_base_set(struct bflb_device_s *dev, uint8_t base)
{
    uint32_t reg_base;
    uint32_t regval;

    if (base > 63) {
        return;
    }
    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_CFG1_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    regval &= ~PEC_CR_SM_SIDE_BASE_MASK;
    regval |= (base << PEC_CR_SM_SIDE_BASE_SHIFT);
    putreg32(regval, reg_base + PEC_SM_CFG1_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
}

/****************************************************************************/ /**
 * @brief  set side-pin count
 *
 * @param  dev: device handle
 * @param  count: side-pin count(including side_en), 0~7 is valid value
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pin_side_count_set(struct bflb_device_s *dev, uint8_t count)
{
    uint32_t reg_base;
    uint32_t regval;

    if (count > 7) {
        return;
    }
    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_CFG1_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    regval &= ~PEC_CR_SM_SIDE_COUNT_MASK;
    regval |= (count << PEC_CR_SM_SIDE_COUNT_SHIFT);
    putreg32(regval, reg_base + PEC_SM_CFG1_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
}

/****************************************************************************/ /**
 * @brief  set set-pin base
 *
 * @param  dev: device handle
 * @param  index: set-pin index, 0~3 is valid value
 * @param  base: set-pin base, 0~63 is valid value
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pin_set_base_set(struct bflb_device_s *dev, uint8_t index, uint8_t base)
{
    uint32_t reg_base;
    uint32_t regval;

    if (index > 3 || base > 63) {
        return;
    }
    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_SETPIN_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    switch (index) {
        case 0:
            regval &= ~PEC_CR_SM_SET_BASE_0_MASK;
            regval |= (base << PEC_CR_SM_SET_BASE_0_SHIFT);
            break;
        case 1:
            regval &= ~PEC_CR_SM_SET_BASE_1_MASK;
            regval |= (base << PEC_CR_SM_SET_BASE_1_SHIFT);
            break;
        case 2:
            regval &= ~PEC_CR_SM_SET_BASE_2_MASK;
            regval |= (base << PEC_CR_SM_SET_BASE_2_SHIFT);
            break;
        case 3:
            regval &= ~PEC_CR_SM_SET_BASE_3_MASK;
            regval |= (base << PEC_CR_SM_SET_BASE_3_SHIFT);
            break;
        default:
            return;
    }
    putreg32(regval, reg_base + PEC_SM_SETPIN_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
}

/****************************************************************************/ /**
 * @brief  set set-pin count
 *
 * @param  dev: device handle
 * @param  index: set-pin index, 0~3 is valid value
 * @param  count: set-pin count, 1~4 is valid value
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pin_set_count_set(struct bflb_device_s *dev, uint8_t index, uint8_t count)
{
    uint32_t reg_base;
    uint32_t regval;

    if (index > 3 || count == 0 || count > 4) {
        return;
    }
    count--;
    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_SETPIN_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    switch (index) {
        case 0:
            regval &= ~PEC_CR_SM_SET_COUNT_0_MASK;
            regval |= (count << PEC_CR_SM_SET_COUNT_0_SHIFT);
            break;
        case 1:
            regval &= ~PEC_CR_SM_SET_COUNT_1_MASK;
            regval |= (count << PEC_CR_SM_SET_COUNT_1_SHIFT);
            break;
        case 2:
            regval &= ~PEC_CR_SM_SET_COUNT_2_MASK;
            regval |= (count << PEC_CR_SM_SET_COUNT_2_SHIFT);
            break;
        case 3:
            regval &= ~PEC_CR_SM_SET_COUNT_3_MASK;
            regval |= (count << PEC_CR_SM_SET_COUNT_3_SHIFT);
            break;
        default:
            return;
    }
    putreg32(regval, reg_base + PEC_SM_SETPIN_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
}

/****************************************************************************/ /**
 * @brief  set out-pin base
 *
 * @param  dev: device handle
 * @param  index: out-pin index, 0~2 is valid value
 * @param  base: out-pin base, 0~63 is valid value
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pin_out_base_set(struct bflb_device_s *dev, uint8_t index, uint8_t base)
{
    uint32_t reg_base;
    uint32_t regval;

    if (index > 2 || base > 63) {
        return;
    }
    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_OUTPIN_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    switch (index) {
        case 0:
            regval &= ~PEC_CR_SM_OUT_BASE_0_MASK;
            regval |= (base << PEC_CR_SM_OUT_BASE_0_SHIFT);
            break;
        case 1:
            regval &= ~PEC_CR_SM_OUT_BASE_1_MASK;
            regval |= (base << PEC_CR_SM_OUT_BASE_1_SHIFT);
            break;
        case 2:
            regval &= ~PEC_CR_SM_OUT_BASE_2_MASK;
            regval |= (base << PEC_CR_SM_OUT_BASE_2_SHIFT);
            break;
        default:
            return;
    }
    putreg32(regval, reg_base + PEC_SM_OUTPIN_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
}

/****************************************************************************/ /**
 * @brief  set out-pin count
 *
 * @param  dev: device handle
 * @param  index: out-pin index, 0~2 is valid value
 * @param  count: out-pin count, 1~16 is valid when index==0, 1~32 is valid when index==1or2
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pin_out_count_set(struct bflb_device_s *dev, uint8_t index, uint8_t count)
{
    uint32_t reg_base;
    uint32_t regval;

    if (index > 2 || count == 0 || count > 32) {
        return;
    }
    if (index == 0 && count > 16) {
        return;
    }
    count--;    
    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_OUTPIN_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    switch (index) {
        case 0:
            regval &= ~PEC_CR_SM_OUT_COUNT_0_MASK;
            regval |= (count << PEC_CR_SM_OUT_COUNT_0_SHIFT);
            break;
        case 1:
            regval &= ~PEC_CR_SM_OUT_COUNT_1_MASK;
            regval |= (count << PEC_CR_SM_OUT_COUNT_1_SHIFT);
            break;
        case 2:
            regval &= ~PEC_CR_SM_OUT_COUNT_2_MASK;
            regval |= (count << PEC_CR_SM_OUT_COUNT_2_SHIFT);
            break;
        default:
            return;
    }
    putreg32(regval, reg_base + PEC_SM_OUTPIN_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
}

/****************************************************************************/ /**
 * @brief  set in-pin base
 *
 * @param  dev: device handle
 * @param  index: in-pin index, 0~4 is valid value
 * @param  base: in-pin base, 0~63 is valid value
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pin_in_base_set(struct bflb_device_s *dev, uint8_t index, uint8_t base)
{
    uint32_t reg_base;
    uint32_t regval;

    if (index > 4 || base > 63) {
        return;
    }
    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_INPIN_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    switch (index) {
        case 0:
            regval &= ~PEC_CR_SM_IN_BASE_0_MASK;
            regval |= (base << PEC_CR_SM_IN_BASE_0_SHIFT);
            break;
        case 1:
            regval &= ~PEC_CR_SM_IN_BASE_1_MASK;
            regval |= (base << PEC_CR_SM_IN_BASE_1_SHIFT);
            break;
        case 2:
            regval &= ~PEC_CR_SM_IN_BASE_2_MASK;
            regval |= (base << PEC_CR_SM_IN_BASE_2_SHIFT);
            break;
        case 3:
            regval &= ~PEC_CR_SM_IN_BASE_3_MASK;
            regval |= (base << PEC_CR_SM_IN_BASE_3_SHIFT);
            break;
        case 4:
            regval &= ~PEC_CR_SM_IN_BASE_4_MASK;
            regval |= (base << PEC_CR_SM_IN_BASE_4_SHIFT);
            break;
        default:
            return;
    }
    putreg32(regval, reg_base + PEC_SM_INPIN_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
}

/****************************************************************************/ /**
 * @brief  get side-pin base
 *
 * @param  dev: device handle
 *
 * @return side-pin base
 *
*******************************************************************************/
uint8_t bflb_pec_pin_side_base_get(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_CFG1_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    regval &= PEC_CR_SM_SIDE_BASE_MASK;
    regval >>= PEC_CR_SM_SIDE_BASE_SHIFT;
    return (uint8_t)regval;
}

/****************************************************************************/ /**
 * @brief  get side-pin count
 *
 * @param  dev: device handle
 *
 * @return side-pin count
 *
*******************************************************************************/
uint8_t bflb_pec_pin_side_count_get(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_CFG1_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    regval &= PEC_CR_SM_SIDE_COUNT_MASK;
    regval >>= PEC_CR_SM_SIDE_COUNT_SHIFT;
    return (uint8_t)regval;
}

/****************************************************************************/ /**
 * @brief  get set-pin base
 *
 * @param  dev: device handle
 * @param  index: set-pin index, 0~3 is valid value
 *
 * @return set-pin base
 *
*******************************************************************************/
uint8_t bflb_pec_pin_set_base_get(struct bflb_device_s *dev, uint8_t index)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_SETPIN_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    switch (index) {
        case 0:
            regval &= PEC_CR_SM_SET_BASE_0_MASK;
            regval <<= PEC_CR_SM_SET_BASE_0_SHIFT;
            break;
        case 1:
            regval &= PEC_CR_SM_SET_BASE_1_MASK;
            regval <<= PEC_CR_SM_SET_BASE_1_SHIFT;
            break;
        case 2:
            regval &= PEC_CR_SM_SET_BASE_2_MASK;
            regval <<= PEC_CR_SM_SET_BASE_2_SHIFT;
            break;
        case 3:
            regval &= PEC_CR_SM_SET_BASE_3_MASK;
            regval <<= PEC_CR_SM_SET_BASE_3_SHIFT;
            break;
        default:
            return 0xFF;
    }
    return (uint8_t)regval;
}

/****************************************************************************/ /**
 * @brief  get set-pin count
 *
 * @param  dev: device handle
 * @param  index: set-pin index, 0~3 is valid value
 *
 * @return set-pin count
 *
*******************************************************************************/
uint8_t bflb_pec_pin_set_count_get(struct bflb_device_s *dev, uint8_t index)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_SETPIN_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    switch (index) {
        case 0:
            regval &= PEC_CR_SM_SET_COUNT_0_MASK;
            regval <<= PEC_CR_SM_SET_COUNT_0_SHIFT;
            break;
        case 1:
            regval &= PEC_CR_SM_SET_COUNT_1_MASK;
            regval <<= PEC_CR_SM_SET_COUNT_1_SHIFT;
            break;
        case 2:
            regval &= PEC_CR_SM_SET_COUNT_2_MASK;
            regval <<= PEC_CR_SM_SET_COUNT_2_SHIFT;
            break;
        case 3:
            regval &= PEC_CR_SM_SET_COUNT_3_MASK;
            regval <<= PEC_CR_SM_SET_COUNT_3_SHIFT;
            break;
        default:
            return 0xFF;
    }
    return (uint8_t)(regval + 1);
}

/****************************************************************************/ /**
 * @brief  get out-pin base
 *
 * @param  dev: device handle
 * @param  index: out-pin index, 0~2 is valid value
 *
 * @return out-pin base
 *
*******************************************************************************/
uint8_t bflb_pec_pin_out_base_get(struct bflb_device_s *dev, uint8_t index)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_OUTPIN_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    switch (index) {
        case 0:
            regval &= PEC_CR_SM_OUT_BASE_0_MASK;
            regval <<= PEC_CR_SM_OUT_BASE_0_SHIFT;
            break;
        case 1:
            regval &= PEC_CR_SM_OUT_BASE_1_MASK;
            regval <<= PEC_CR_SM_OUT_BASE_1_SHIFT;
            break;
        case 2:
            regval &= PEC_CR_SM_OUT_BASE_2_MASK;
            regval <<= PEC_CR_SM_OUT_BASE_2_SHIFT;
            break;
        default:
            return 0xFF;
    }
    return (uint8_t)regval;
}

/****************************************************************************/ /**
 * @brief  get out-pin count
 *
 * @param  dev: device handle
 * @param  index: out-pin index, 0~2 is valid value
 *
 * @return out-pin count
 *
*******************************************************************************/
uint8_t bflb_pec_pin_out_count_get(struct bflb_device_s *dev, uint8_t index)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_OUTPIN_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    switch (index) {
        case 0:
            regval &= PEC_CR_SM_OUT_COUNT_0_MASK;
            regval <<= PEC_CR_SM_OUT_COUNT_0_SHIFT;
            break;
        case 1:
            regval &= PEC_CR_SM_OUT_COUNT_1_MASK;
            regval <<= PEC_CR_SM_OUT_COUNT_1_SHIFT;
            break;
        case 2:
            regval &= PEC_CR_SM_OUT_COUNT_2_MASK;
            regval <<= PEC_CR_SM_OUT_COUNT_2_SHIFT;
            break;
        default:
            return 0xFF;
    }
    return (uint8_t)(regval + 1);
}

/****************************************************************************/ /**
 * @brief  get in-pin base
 *
 * @param  dev: device handle
 * @param  index: in-pin index, 0~4 is valid value
 *
 * @return in-pin base
 *
*******************************************************************************/
uint8_t bflb_pec_pin_in_base_get(struct bflb_device_s *dev, uint8_t index)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_INPIN_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    switch (index) {
        case 0:
            regval &= PEC_CR_SM_IN_BASE_0_MASK;
            regval <<= PEC_CR_SM_IN_BASE_0_SHIFT;
            break;
        case 1:
            regval &= PEC_CR_SM_IN_BASE_1_MASK;
            regval <<= PEC_CR_SM_IN_BASE_1_SHIFT;
            break;
        case 2:
            regval &= PEC_CR_SM_IN_BASE_2_MASK;
            regval <<= PEC_CR_SM_IN_BASE_2_SHIFT;
            break;
        case 3:
            regval &= PEC_CR_SM_IN_BASE_3_MASK;
            regval <<= PEC_CR_SM_IN_BASE_3_SHIFT;
            break;
        case 4:
            regval &= PEC_CR_SM_IN_BASE_4_MASK;
            regval <<= PEC_CR_SM_IN_BASE_4_SHIFT;
            break;
        default:
            return 0xFF;
    }
    return (uint8_t)regval;
}

/****************************************************************************/ /**
 * @brief  enable output value inverse of one pin
 *
 * @param  dev: device handle
 * @param  pin: which pin to be inversed, 0~63 is valid value
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pad_o_inv_enable_bit(struct bflb_device_s *dev, uint8_t pin)
{
    uint32_t reg_base;
    uint32_t regval;

    pin = pin % PEC_PIN_MAX;
    reg_base = dev->reg_base;
    reg_base += PEC_PAD_O_INV_OFFSET;
    if (pin >= 32) {
        pin = pin % 32;
        reg_base += sizeof(uint32_t);
    }
    regval = getreg32(reg_base);
    regval |= (1U << pin);
    putreg32(regval, reg_base);
}

/****************************************************************************/ /**
 * @brief  disable output value inverse of one pin
 *
 * @param  dev: device handle
 * @param  pin: which pin to be inversed, 0~63 is valid value
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pad_o_inv_disable_bit(struct bflb_device_s *dev, uint8_t pin)
{
    uint32_t reg_base;
    uint32_t regval;

    pin = pin % PEC_PIN_MAX;
    reg_base = dev->reg_base;
    reg_base += PEC_PAD_O_INV_OFFSET;
    if (pin >= 32) {
        pin = pin % 32;
        reg_base += sizeof(uint32_t);
    }
    regval = getreg32(reg_base);
    regval &= ~(1U << pin);
    putreg32(regval, reg_base);
}

/****************************************************************************/ /**
 * @brief  get output value inverse status of one pin
 *
 * @param  dev: device handle
 * @param  pin: which pin to get, 0~63 is valid value
 *
 * @return output value inverse status
 *
*******************************************************************************/
uint8_t bflb_pec_pad_o_inv_enable_get_bit(struct bflb_device_s *dev, uint8_t pin)
{
    uint32_t reg_base;
    uint32_t regval;

    pin = pin % PEC_PIN_MAX;
    reg_base = dev->reg_base;
    reg_base += PEC_PAD_O_INV_OFFSET;
    if (pin >= 32) {
        pin = pin % 32;
        reg_base += sizeof(uint32_t);
    }
    regval = getreg32(reg_base);
    if (regval & (1U << pin)) {
        return 1;
    } else {
        return 0;
    }
}

/****************************************************************************/ /**
 * @brief  enable output value inverse of multiple pins
 *
 * @param  dev: device handle
 * @param  mask: which pins to be inversed, bits which is 1 will enable output value inverse
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pad_o_inv_enable_mask(struct bflb_device_s *dev, uint64_t mask)
{
    uint32_t reg_base;
    uint32_t regval;
    uint32_t part;

    reg_base = dev->reg_base;
    reg_base += PEC_PAD_O_INV_OFFSET;
    part = (uint32_t)mask & 0xFFFFFFFF;
    if (part) {
        regval = getreg32(reg_base);
        regval |= part;
        putreg32(regval, reg_base);
    }
    part = (uint32_t)(mask >> 32) & 0xFFFFFFFF;
    if (part) {
        reg_base += sizeof(uint32_t);
        regval = getreg32(reg_base);
        regval |= part;
        putreg32(regval, reg_base);
    }
}

/****************************************************************************/ /**
 * @brief  disable output value inverse of multiple pins
 *
 * @param  dev: device handle
 * @param  mask: which pins to be not inversed, bits which is 1 will disable output value inverse
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pad_o_inv_disable_mask(struct bflb_device_s *dev, uint64_t mask)
{
    uint32_t reg_base;
    uint32_t regval;
    uint32_t part;

    reg_base = dev->reg_base;
    reg_base += PEC_PAD_O_INV_OFFSET;
    part = (uint32_t)mask & 0xFFFFFFFF;
    if (part) {
        regval = getreg32(reg_base);
        regval &= ~part;
        putreg32(regval, reg_base);
    }
    part = (uint32_t)(mask >> 32) & 0xFFFFFFFF;
    if (part) {
        reg_base += sizeof(uint32_t);
        regval = getreg32(reg_base);
        regval &= ~part;
        putreg32(regval, reg_base);
    }
}

/****************************************************************************/ /**
 * @brief  get output value inverse status of multiple pins
 *
 * @param  dev: device handle
 *
 * @return output value inverse status
 *
*******************************************************************************/
uint64_t bflb_pec_pad_o_inv_enable_get(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval_0;
    uint32_t regval_1;

    reg_base = dev->reg_base;
    reg_base += PEC_PAD_O_INV_OFFSET;
    regval_0 = getreg32(reg_base);
    reg_base += sizeof(uint32_t);
    regval_1 = getreg32(reg_base);
    return ((uint64_t)regval_1 << 32) | (uint64_t)regval_0;
}

/****************************************************************************/ /**
 * @brief  enable output inverse of one pin
 *
 * @param  dev: device handle
 * @param  pin: which pin to be inversed, 0~63 is valid value
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pad_oe_inv_enable_bit(struct bflb_device_s *dev, uint8_t pin)
{
    uint32_t reg_base;
    uint32_t regval;

    pin = pin % PEC_PIN_MAX;
    reg_base = dev->reg_base;
    reg_base += PEC_PAD_OE_INV_OFFSET;
    if (pin >= 32) {
        pin = pin % 32;
        reg_base += sizeof(uint32_t);
    }
    regval = getreg32(reg_base);
    regval |= (1U << pin);
    putreg32(regval, reg_base);
}

/****************************************************************************/ /**
 * @brief  disable output inverse of one pin
 *
 * @param  dev: device handle
 * @param  pin: which pin to be inversed, 0~63 is valid value
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pad_oe_inv_disable_bit(struct bflb_device_s *dev, uint8_t pin)
{
    uint32_t reg_base;
    uint32_t regval;

    pin = pin % PEC_PIN_MAX;
    reg_base = dev->reg_base;
    reg_base += PEC_PAD_OE_INV_OFFSET;
    if (pin >= 32) {
        pin = pin % 32;
        reg_base += sizeof(uint32_t);
    }
    regval = getreg32(reg_base);
    regval &= ~(1U << pin);
    putreg32(regval, reg_base);
}

/****************************************************************************/ /**
 * @brief  get output inverse status of one pin
 *
 * @param  dev: device handle
 * @param  pin: which pin to get, 0~63 is valid value
 *
 * @return output inverse status
 *
*******************************************************************************/
uint8_t bflb_pec_pad_oe_inv_enable_get_bit(struct bflb_device_s *dev, uint8_t pin)
{
    uint32_t reg_base;
    uint32_t regval;

    pin = pin % PEC_PIN_MAX;
    reg_base = dev->reg_base;
    reg_base += PEC_PAD_OE_INV_OFFSET;
    if (pin >= 32) {
        pin = pin % 32;
        reg_base += sizeof(uint32_t);
    }
    regval = getreg32(reg_base);
    if (regval & (1U << pin)) {
        return 1;
    } else {
        return 0;
    }
}

/****************************************************************************/ /**
 * @brief  enable output inverse of multiple pins
 *
 * @param  dev: device handle
 * @param  mask: which pins to be inversed, bits which is 1 will enable output inverse
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pad_oe_inv_enable_mask(struct bflb_device_s *dev, uint64_t mask)
{
    uint32_t reg_base;
    uint32_t regval;
    uint32_t part;

    reg_base = dev->reg_base;
    reg_base += PEC_PAD_OE_INV_OFFSET;
    part = (uint32_t)mask & 0xFFFFFFFF;
    if (part) {
        regval = getreg32(reg_base);
        regval |= part;
        putreg32(regval, reg_base);
    }
    part = (uint32_t)(mask >> 32) & 0xFFFFFFFF;
    if (part) {
        reg_base += sizeof(uint32_t);
        regval = getreg32(reg_base);
        regval |= part;
        putreg32(regval, reg_base);
    }
}

/****************************************************************************/ /**
 * @brief  disable output inverse of multiple pins
 *
 * @param  dev: device handle
 * @param  mask: which pins to be inversed, bits which is 1 will disable output inverse
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pad_oe_inv_disable_mask(struct bflb_device_s *dev, uint64_t mask)
{
    uint32_t reg_base;
    uint32_t regval;
    uint32_t part;

    reg_base = dev->reg_base;
    reg_base += PEC_PAD_OE_INV_OFFSET;
    part = (uint32_t)mask & 0xFFFFFFFF;
    if (part) {
        regval = getreg32(reg_base);
        regval &= ~part;
        putreg32(regval, reg_base);
    }
    part = (uint32_t)(mask >> 32) & 0xFFFFFFFF;
    if (part) {
        reg_base += sizeof(uint32_t);
        regval = getreg32(reg_base);
        regval &= ~part;
        putreg32(regval, reg_base);
    }
}

/****************************************************************************/ /**
 * @brief  get output enable inverse status of multiple pins
 *
 * @param  dev: device handle
 *
 * @return output enable inverse status
 *
*******************************************************************************/
uint64_t bflb_pec_pad_oe_inv_enable_get(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval_0;
    uint32_t regval_1;

    reg_base = dev->reg_base;
    reg_base += PEC_PAD_OE_INV_OFFSET;
    regval_0 = getreg32(reg_base);
    reg_base += sizeof(uint32_t);
    regval_1 = getreg32(reg_base);
    return ((uint64_t)regval_1 << 32) | (uint64_t)regval_0;
}

/****************************************************************************/ /**
 * @brief  enable input value inverse of one pin
 *
 * @param  dev: device handle
 * @param  pin: which pin to be inversed, 0~63 is valid value
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pad_i_inv_enable_bit(struct bflb_device_s *dev, uint8_t pin)
{
    uint32_t reg_base;
    uint32_t regval;

    pin = pin % PEC_PIN_MAX;
    reg_base = dev->reg_base;
    reg_base += PEC_PAD_I_INV_OFFSET;
    if (pin >= 32) {
        pin = pin % 32;
        reg_base += sizeof(uint32_t);
    }
    regval = getreg32(reg_base);
    regval |= (1U << pin);
    putreg32(regval, reg_base);
}

/****************************************************************************/ /**
 * @brief  disable input value inverse of one pin
 *
 * @param  dev: device handle
 * @param  pin: which pin to be inversed, 0~63 is valid value
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pad_i_inv_disable_bit(struct bflb_device_s *dev, uint8_t pin)
{
    uint32_t reg_base;
    uint32_t regval;

    pin = pin % PEC_PIN_MAX;
    reg_base = dev->reg_base;
    reg_base += PEC_PAD_I_INV_OFFSET;
    if (pin >= 32) {
        pin = pin % 32;
        reg_base += sizeof(uint32_t);
    }
    regval = getreg32(reg_base);
    regval &= ~(1U << pin);
    putreg32(regval, reg_base);
}

/****************************************************************************/ /**
 * @brief  get input value inverse status of one pin
 *
 * @param  dev: device handle
 * @param  pin: which pin to get, 0~63 is valid value
 *
 * @return input value inverse status
 *
*******************************************************************************/
uint8_t bflb_pec_pad_i_inv_enable_get_bit(struct bflb_device_s *dev, uint8_t pin)
{
    uint32_t reg_base;
    uint32_t regval;

    pin = pin % PEC_PIN_MAX;
    reg_base = dev->reg_base;
    reg_base += PEC_PAD_I_INV_OFFSET;
    if (pin >= 32) {
        pin = pin % 32;
        reg_base += sizeof(uint32_t);
    }
    regval = getreg32(reg_base);
    if (regval & (1U << pin)) {
        return 1;
    } else {
        return 0;
    }
}

/****************************************************************************/ /**
 * @brief  enable input value inverse of multiple pins
 *
 * @param  dev: device handle
 * @param  mask: which pins to be inversed, bits which is 1 will enable input value inverse
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pad_i_inv_enable_mask(struct bflb_device_s *dev, uint64_t mask)
{
    uint32_t reg_base;
    uint32_t regval;
    uint32_t part;

    reg_base = dev->reg_base;
    reg_base += PEC_PAD_I_INV_OFFSET;
    part = (uint32_t)mask & 0xFFFFFFFF;
    if (part) {
        regval = getreg32(reg_base);
        regval |= part;
        putreg32(regval, reg_base);
    }
    part = (uint32_t)(mask >> 32) & 0xFFFFFFFF;
    if (part) {
        reg_base += sizeof(uint32_t);
        regval = getreg32(reg_base);
        regval |= part;
        putreg32(regval, reg_base);
    }
}

/****************************************************************************/ /**
 * @brief  disable input value inverse of multiple pins
 *
 * @param  dev: device handle
 * @param  mask: which pins to be not inversed, bits which is 1 will disable input value inverse
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pad_i_inv_disable_mask(struct bflb_device_s *dev, uint64_t mask)
{
    uint32_t reg_base;
    uint32_t regval;
    uint32_t part;

    reg_base = dev->reg_base;
    reg_base += PEC_PAD_I_INV_OFFSET;
    part = (uint32_t)mask & 0xFFFFFFFF;
    if (part) {
        regval = getreg32(reg_base);
        regval &= ~part;
        putreg32(regval, reg_base);
    }
    part = (uint32_t)(mask >> 32) & 0xFFFFFFFF;
    if (part) {
        reg_base += sizeof(uint32_t);
        regval = getreg32(reg_base);
        regval &= ~part;
        putreg32(regval, reg_base);
    }
}

/****************************************************************************/ /**
 * @brief  get input value inverse status of multiple pins
 *
 * @param  dev: device handle
 *
 * @return input value inverse status
 *
*******************************************************************************/
uint64_t bflb_pec_pad_i_inv_enable_get(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval_0;
    uint32_t regval_1;

    reg_base = dev->reg_base;
    reg_base += PEC_PAD_I_INV_OFFSET;
    regval_0 = getreg32(reg_base);
    reg_base += sizeof(uint32_t);
    regval_1 = getreg32(reg_base);
    return ((uint64_t)regval_1 << 32) | (uint64_t)regval_0;
}

/****************************************************************************/ /**
 * @brief  set output value of one pin
 *
 * @param  dev: device handle
 * @param  pin: which pin to be set, 0~63 is valid value
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pad_o_set_bit(struct bflb_device_s *dev, uint8_t pin)
{
    uint32_t reg_base;
    uint32_t regval;

    pin = pin % PEC_PIN_MAX;
    reg_base = dev->reg_base;
    reg_base += PEC_PAD_O_SET_OFFSET;
    if (pin >= 32) {
        pin = pin % 32;
        reg_base += sizeof(uint32_t);
    }
    regval = (1U << pin);
    putreg32(regval, reg_base);
}

/****************************************************************************/ /**
 * @brief  clear output value of one pin
 *
 * @param  dev: device handle
 * @param  pin: which pin to be cleared, 0~63 is valid value
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pad_o_clr_bit(struct bflb_device_s *dev, uint8_t pin)
{
    uint32_t reg_base;
    uint32_t regval;

    pin = pin % PEC_PIN_MAX;
    reg_base = dev->reg_base;
    reg_base += PEC_PAD_O_CLR_OFFSET;
    if (pin >= 32) {
        pin = pin % 32;
        reg_base += sizeof(uint32_t);
    }
    regval = (1U << pin);
    putreg32(regval, reg_base);
}


/****************************************************************************/ /**
 * @brief  get output value of one pin
 *
 * @param  dev: device handle
 * @param  pin: which pin to get, 0~63 is valid value
 *
 * @return pin output value
 *
*******************************************************************************/
uint8_t bflb_pec_pad_o_get_bit(struct bflb_device_s *dev, uint8_t pin)
{
    uint32_t reg_base;
    uint32_t regval;

    pin = pin % PEC_PIN_MAX;
    reg_base = dev->reg_base;
    reg_base += PEC_PAD_O_CLR_OFFSET;
    if (pin >= 32) {
        pin = pin % 32;
        reg_base += sizeof(uint32_t);
    }
    regval = getreg32(reg_base);
    if (regval & (1U << pin)) {
        return 1;
    } else {
        return 0;
    }
}

/****************************************************************************/ /**
 * @brief  set output value of multiple pins
 *
 * @param  dev: device handle
 * @param  mask: which pins to be set, bits which is 1 will be set
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pad_o_set_mask(struct bflb_device_s *dev, uint64_t mask)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    reg_base += PEC_PAD_O_SET_OFFSET;
    regval = (uint32_t)mask & 0xFFFFFFFF;
    if (regval) {
        putreg32(regval, reg_base);
    }
    regval = (uint32_t)(mask >> 32) & 0xFFFFFFFF;
    if (regval) {
        reg_base += sizeof(uint32_t);
        putreg32(regval, reg_base);
    }
}

/****************************************************************************/ /**
 * @brief  clear output value of multiple pins
 *
 * @param  dev: device handle
 * @param  mask: which pins to be cleared, bits which is 1 will be cleared
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pad_o_clr_mask(struct bflb_device_s *dev, uint64_t mask)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    reg_base += PEC_PAD_O_CLR_OFFSET;
    regval = (uint32_t)mask & 0xFFFFFFFF;
    if (regval) {
        putreg32(regval, reg_base);
    }
    regval = (uint32_t)(mask >> 32) & 0xFFFFFFFF;
    if (regval) {
        reg_base += sizeof(uint32_t);
        putreg32(regval, reg_base);
    }
}

/****************************************************************************/ /**
 * @brief  get output value of multiple pins
 *
 * @param  dev: device handle
 *
 * @return pins output value
 *
*******************************************************************************/
uint64_t bflb_pec_pad_o_get(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval_0;
    uint32_t regval_1;

    reg_base = dev->reg_base;
    reg_base += PEC_PAD_O_CLR_OFFSET;
    regval_0 = getreg32(reg_base);
    reg_base += sizeof(uint32_t);
    regval_1 = getreg32(reg_base);
    return ((uint64_t)regval_1 << 32) | (uint64_t)regval_0;
}

/****************************************************************************/ /**
 * @brief  set output enable of one pin
 *
 * @param  dev: device handle
 * @param  pin: which pin to be output, 0~63 is valid value
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pad_oe_set_bit(struct bflb_device_s *dev, uint8_t pin)
{
    uint32_t reg_base;
    uint32_t regval;

    pin = pin % PEC_PIN_MAX;
    reg_base = dev->reg_base;
    reg_base += PEC_PAD_OE_SET_OFFSET;
    if (pin >= 32) {
        pin = pin % 32;
        reg_base += sizeof(uint32_t);
    }
    regval = (1U << pin);
    putreg32(regval, reg_base);
}

/****************************************************************************/ /**
 * @brief  clear output enable of one pin
 *
 * @param  dev: device handle
 * @param  pin: which pin to be disable output, 0~63 is valid value
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pad_oe_clr_bit(struct bflb_device_s *dev, uint8_t pin)
{
    uint32_t reg_base;
    uint32_t regval;

    pin = pin % PEC_PIN_MAX;
    reg_base = dev->reg_base;
    reg_base += PEC_PAD_OE_CLR_OFFSET;
    if (pin >= 32) {
        pin = pin % 32;
        reg_base += sizeof(uint32_t);
    }
    regval = (1U << pin);
    putreg32(regval, reg_base);
}

/****************************************************************************/ /**
 * @brief  get output enable of one pin
 *
 * @param  dev: device handle
 * @param  pin: which pin to get output enable status, 0~63 is valid value
 *
 * @return pin output enable status
 *
*******************************************************************************/
uint8_t bflb_pec_pad_oe_get_bit(struct bflb_device_s *dev, uint8_t pin)
{
    uint32_t reg_base;
    uint32_t regval;

    pin = pin % PEC_PIN_MAX;
    reg_base = dev->reg_base;
    reg_base += PEC_PAD_OE_CLR_OFFSET;
    if (pin >= 32) {
        pin = pin % 32;
        reg_base += sizeof(uint32_t);
    }
    regval = getreg32(reg_base);
    if (regval & (1U << pin)) {
        return 1;
    } else {
        return 0;
    }
}

/****************************************************************************/ /**
 * @brief  set output enable of multiple pins
 *
 * @param  dev: device handle
 * @param  mask: which pins to be output, bits which is 1 will output
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pad_oe_set_mask(struct bflb_device_s *dev, uint64_t mask)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    reg_base += PEC_PAD_OE_SET_OFFSET;
    regval = (uint32_t)mask & 0xFFFFFFFF;
    if (regval) {
        putreg32(regval, reg_base);
    }
    regval = (uint32_t)(mask >> 32) & 0xFFFFFFFF;
    if (regval) {
        reg_base += sizeof(uint32_t);
        putreg32(regval, reg_base);
    }
}

/****************************************************************************/ /**
 * @brief  clear output enable of multiple pins
 *
 * @param  dev: device handle
 * @param  mask: which pins to be disable output, bits which is 1 will disable output
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_pad_oe_clr_mask(struct bflb_device_s *dev, uint64_t mask)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    reg_base += PEC_PAD_OE_CLR_OFFSET;
    regval = (uint32_t)mask & 0xFFFFFFFF;
    if (regval) {
        putreg32(regval, reg_base);
    }
    regval = (uint32_t)(mask >> 32) & 0xFFFFFFFF;
    if (regval) {
        reg_base += sizeof(uint32_t);
        putreg32(regval, reg_base);
    }
}

/****************************************************************************/ /**
 * @brief  get output enable status of multiple pins
 *
 * @param  dev: device handle
 *
 * @return pins output enable status
 *
*******************************************************************************/
uint64_t bflb_pec_pad_oe_get(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval_0;
    uint32_t regval_1;

    reg_base = dev->reg_base;
    reg_base += PEC_PAD_OE_CLR_OFFSET;
    regval_0 = getreg32(reg_base);
    reg_base += sizeof(uint32_t);
    regval_1 = getreg32(reg_base);
    return ((uint64_t)regval_1 << 32) | (uint64_t)regval_0;
}

/****************************************************************************/ /**
 * @brief  get interrupt raw status
 *
 * @param  dev: device handle
 * @param  int_mask: interrupt mask
 *
 * @return interrupt raw status
 *
*******************************************************************************/
uint16_t bflb_pec_int_status_get_raw(struct bflb_device_s *dev, uint16_t int_mask)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_INT_RAW_OFFSET);
    return (uint16_t)regval & int_mask;
}

/****************************************************************************/ /**
 * @brief  get interrupt status of irq0
 *
 * @param  dev: device handle
 * @param  int_mask: interrupt mask
 *
 * @return interrupt status
 *
*******************************************************************************/
uint16_t bflb_pec_int0_status_get(struct bflb_device_s *dev, uint16_t int_mask)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_IRQ0_INTS_OFFSET);
    return (uint16_t)regval & int_mask;
}

/****************************************************************************/ /**
 * @brief  enable interrupt of irq0
 *
 * @param  dev: device handle
 * @param  int_mask: interrupt mask
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_int0_enable(struct bflb_device_s *dev, uint16_t int_mask)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_IRQ0_INTE_OFFSET);
    regval |= int_mask;
    putreg32(regval, reg_base + PEC_IRQ0_INTE_OFFSET);
}

/****************************************************************************/ /**
 * @brief  disable interrupt of irq0
 *
 * @param  dev: device handle
 * @param  int_mask: interrupt mask
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_int0_disable(struct bflb_device_s *dev, uint16_t int_mask)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_IRQ0_INTE_OFFSET);
    regval &= ~(uint32_t)int_mask;
    putreg32(regval, reg_base + PEC_IRQ0_INTE_OFFSET);
}

/****************************************************************************/ /**
 * @brief  get interrupt enable statue of irq0
 *
 * @param  dev: device handle
 * @param  int_mask: interrupt mask
 *
 * @return interrupt enable statue
 *
*******************************************************************************/
uint16_t bflb_pec_int0_enable_get(struct bflb_device_s *dev, uint16_t int_mask)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_IRQ0_INTE_OFFSET);
    return ((uint16_t)regval & int_mask);
}

/****************************************************************************/ /**
 * @brief  get interrupt status of irq1
 *
 * @param  dev: device handle
 * @param  int_mask: interrupt mask
 *
 * @return interrupt status
 *
*******************************************************************************/
uint16_t bflb_pec_int1_status_get(struct bflb_device_s *dev, uint16_t int_mask)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_IRQ1_INTS_OFFSET);
    return (uint16_t)regval & int_mask;
}

/****************************************************************************/ /**
 * @brief  enable interrupt of irq1
 *
 * @param  dev: device handle
 * @param  int_mask: interrupt mask
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_int1_enable(struct bflb_device_s *dev, uint16_t int_mask)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_IRQ1_INTE_OFFSET);
    regval |= int_mask;
    putreg32(regval, reg_base + PEC_IRQ1_INTE_OFFSET);
}

/****************************************************************************/ /**
 * @brief  disable interrupt of irq1
 *
 * @param  dev: device handle
 * @param  int_mask: interrupt mask
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_int1_disable(struct bflb_device_s *dev, uint16_t int_mask)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_IRQ1_INTE_OFFSET);
    regval &= ~(uint32_t)int_mask;
    putreg32(regval, reg_base + PEC_IRQ1_INTE_OFFSET);
}

/****************************************************************************/ /**
 * @brief  get interrupt enable statue of irq1
 *
 * @param  dev: device handle
 * @param  int_mask: interrupt mask
 *
 * @return interrupt enable statue
 *
*******************************************************************************/
uint16_t bflb_pec_int1_enable_get(struct bflb_device_s *dev, uint16_t int_mask)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_IRQ1_INTE_OFFSET);
    return ((uint16_t)regval & int_mask);
}

/****************************************************************************/ /**
 * @brief  write a 32-bit value to state machine common register
 *
 * @param  dev: device handle
 * @param  idx: index of register, 0~5 is valid
 * @param  val: value to be written
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_sm_reg_write(struct bflb_device_s *dev, uint8_t idx, uint32_t val)
{
    uint32_t reg_base;

    reg_base = dev->reg_base;
    putreg32(val, reg_base + PEC_SM_R0_OFFSET + dev->sub_idx * PEC_SM_REG_OFFSET + idx * PEC_SM_REG_REG_OFFSET);
}

/****************************************************************************/ /**
 * @brief  read a 32-bit value from state machine common register
 *
 * @param  dev: device handle
 * @param  idx: index of register, 0~5 is valid
 *
 * @return register value
 *
*******************************************************************************/
uint32_t bflb_pec_sm_reg_read(struct bflb_device_s *dev, uint8_t idx)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_R0_OFFSET + dev->sub_idx * PEC_SM_REG_OFFSET + idx * PEC_SM_REG_REG_OFFSET);
    return regval;
}

/****************************************************************************/ /**
 * @brief  set one bit of state machine common register
 *
 * @param  dev: device handle
 * @param  idx: index of register, 0~5 is valid
 * @param  bit: bit to be set, 0~31 is valid
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_sm_reg_set_bit(struct bflb_device_s *dev, uint8_t idx, uint8_t bit)
{
    uint32_t reg_base;

    bit = bit % 32;
    reg_base = dev->reg_base;
    reg_base = reg_base + PEC_SM_R0L_RS_OFFSET + dev->sub_idx * PEC_SM_REG_OFFSET + idx * PEC_SM_REG_REG_OFFSET;
    if (bit >= 16) {
        bit = bit % 16;
        reg_base += sizeof(uint32_t);
    }
    putreg32((uint32_t)1U << bit, reg_base);
}

/****************************************************************************/ /**
 * @brief  clear one bit of state machine common register
 *
 * @param  dev: device handle
 * @param  idx: index of register, 0~5 is valid
 * @param  bit: bit to be cleared, 0~31 is valid
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_sm_reg_clr_bit(struct bflb_device_s *dev, uint8_t idx, uint8_t bit)
{
    uint32_t reg_base;

    bit = bit % 32;
    reg_base = dev->reg_base;
    reg_base = reg_base + PEC_SM_R0L_RS_OFFSET + dev->sub_idx * PEC_SM_REG_OFFSET + idx * PEC_SM_REG_REG_OFFSET;
    if (bit >= 16) {
        bit = bit % 16;
        reg_base += sizeof(uint32_t);
    }
    bit += 16;
    putreg32((uint32_t)1U << bit, reg_base);
}

/****************************************************************************/ /**
 * @brief  get one bit of state machine common register
 *
 * @param  dev: device handle
 * @param  idx: index of register, 0~5 is valid
 * @param  bit: bit to be set, 0~31 is valid
 *
 * @return bit value
 *
*******************************************************************************/
uint8_t bflb_pec_sm_reg_get_bit(struct bflb_device_s *dev, uint8_t idx, uint8_t bit)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_R0_OFFSET + dev->sub_idx * PEC_SM_REG_OFFSET + idx * PEC_SM_REG_REG_OFFSET);
    if (regval & (1U << bit)) {
        return 1;
    } else {
        return 0;
    }
}

/****************************************************************************/ /**
 * @brief  write value to state machine reset/set register[15:0]
 *
 * @param  dev: device handle
 * @param  idx: index of register, 0~5 is valid
 * @param  val: value to be written, 31~16 is for reset, 15~0 is for set(high priority)
 *
 * @return bit value
 *
*******************************************************************************/
void bflb_pec_sm_regl_rs(struct bflb_device_s *dev, uint8_t idx, uint32_t val)
{
    uint32_t reg_base;

    reg_base = dev->reg_base;
    putreg32(val, reg_base + PEC_SM_R0L_RS_OFFSET + dev->sub_idx * PEC_SM_REG_OFFSET + idx * PEC_SM_REG_REG_OFFSET);
}

/****************************************************************************/ /**
 * @brief  write value to state machine reset/set register[31:16]
 *
 * @param  dev: device handle
 * @param  idx: index of register, 0~5 is valid
 * @param  val: value to be written, 31~16 is for reset, 15~0 is for set(high priority)
 *
 * @return bit value
 *
*******************************************************************************/
void bflb_pec_sm_regh_rs(struct bflb_device_s *dev, uint8_t idx, uint32_t val)
{
    uint32_t reg_base;

    reg_base = dev->reg_base;
    putreg32(val, reg_base + PEC_SM_R0H_RS_OFFSET + dev->sub_idx * PEC_SM_REG_OFFSET + idx * PEC_SM_REG_REG_OFFSET);
}

/****************************************************************************/ /**
 * @brief  enable fifo_tx with dma
 *
 * @param  dev: device handle
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_dma_enable_tx(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_CFG1_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    regval |= PEC_CR_SM_DMA_TX_EN;
    putreg32(regval, reg_base + PEC_SM_CFG1_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
}

/****************************************************************************/ /**
 * @brief  enable fifo_rx with dma
 *
 * @param  dev: device handle
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_dma_enable_rx(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_CFG1_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    regval |= PEC_CR_SM_DMA_RX_EN;
    putreg32(regval, reg_base + PEC_SM_CFG1_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
}

/****************************************************************************/ /**
 * @brief  disable fifo_tx with dma
 *
 * @param  dev: device handle
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_dma_disable_tx(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_CFG1_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    regval &= ~PEC_CR_SM_DMA_TX_EN;
    putreg32(regval, reg_base + PEC_SM_CFG1_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
}

/****************************************************************************/ /**
 * @brief  disable fifo_rx with dma
 *
 * @param  dev: device handle
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_dma_disable_rx(struct bflb_device_s *dev)
{
    uint32_t reg_base;
    uint32_t regval;

    reg_base = dev->reg_base;
    regval = getreg32(reg_base + PEC_SM_CFG1_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
    regval &= ~PEC_CR_SM_DMA_RX_EN;
    putreg32(regval, reg_base + PEC_SM_CFG1_OFFSET + dev->sub_idx * PEC_SM_OFFSET);
}

/****************************************************************************/ /**
 * @brief  write an 32-bit value to any register
 *
 * @param  dev: device handle
 * @param  offset: address offset
 * @param  value: value will be written
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_reg_write(struct bflb_device_s *dev, uint32_t offset, uint32_t value)
{
    putreg32(value, dev->reg_base + offset);
}

/****************************************************************************/ /**
 * @brief  read an 32-bit value from any register
 *
 * @param  dev: device handle
 * @param  offset: address offset
 *
 * @return register value
 *
*******************************************************************************/
uint32_t bflb_pec_reg_read(struct bflb_device_s *dev, uint32_t offset)
{
    return getreg32(dev->reg_base + offset);
}
