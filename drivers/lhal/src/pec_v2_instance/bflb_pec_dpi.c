#include "bflb_pec_v2.h"
#include "bflb_pec_v2_instance.h"
#include "bflb_pec_common.h"

/*** note
    +-----------+-----------+-----------+---------+
BIT:| 31 ... 21 | 20 ... 16 | 15 ... 10 | 9 ... 0 |
    +-----------+-----------+-----------+---------+
R0: |         VACT          |         HACT        |
    +-----------+-----------+-----------+---------+
R1: |    HFP    |          HBP          |  HSYNC  |
    +-----------+-----------------------+---------+
R2: |    VFP    |          VBP          |  VSYNC  |
    +-----------+-----------------------+---------+
R3: |           recurse for horizontal            |
    +---------------------------------------------+
R4: |            recurse for vertical             |
    +---------------------------------------------+
*******************************************************************************/

#define PIN_SET_VSYNC (PEC_INSTR_SET_DST_PINVAL_0)
#define PIN_SET_HSYNC (PEC_INSTR_SET_DST_PINVAL_1)
#define PIN_SET_DE    (PEC_INSTR_SET_DST_PINVAL_2)
#define PIN_SET_LAT   (PEC_INSTR_SET_DST_PINVAL_3)

#define PIN_SET_VSYNC_IDX (0)
#define PIN_SET_HSYNC_IDX (1)
#define PIN_SET_DE_IDX    (2)
#define PIN_SET_LAT_IDX   (3)

#define PARA_ACTURAL (PEC_R0) /* used for store width and hegigh of actural picture */
#define PARA_H_CFG   (PEC_R1) /* used for store hsync/hbp/hfp */
#define PARA_V_CFG   (PEC_R2) /* used for store vsync/vbp/vfp */
#define PARA_H_RECUR (PEC_R3) /* used for recurse of horizontal */
#define PARA_V_RECUR (PEC_R4) /* used for recurse of vertical */

#define PARA_BIT_LEN_SYNC (10)
#define PARA_BIT_LEN_BP   (11)
#define PARA_BIT_LEN_FP   (11)
#define PARA_BIT_POS_SYNC (0)
#define PARA_BIT_POS_BP   (PARA_BIT_POS_SYNC + PARA_BIT_LEN_SYNC)
#define PARA_BIT_POS_FP   (PARA_BIT_POS_BP + PARA_BIT_LEN_BP)

#define PARA_BIT_LEN_H (16)
#define PARA_BIT_LEN_V (16)
#define PARA_BIT_POS_H (0)
#define PARA_BIT_POS_V (PARA_BIT_POS_H + PARA_BIT_LEN_H)

/****************************************************************************/ /**
 * @brief  initialize dpi ip
 *
 * @param  dev: device handle
 * @param  dpi: pointor of dpi configuration structure
 *
 * @return status of initialization
 *
*******************************************************************************/
int bflb_pec_dpi_init(struct bflb_device_s *dev, struct bflb_pec_dpi_s *dpi)
{
    struct bflb_pec_cfg_s cfg;
    uint8_t mem;
    int ret;

    ret = bflb_pec_memory_size_check(dev, dpi->mem, 64, &mem);
    if (ret != PEC_STS_OK) {
        return ret;
    }

    bflb_pec_disable(dev);
    cfg.clkdiv = dpi->div;                             /*!< clock divisor, frequency = clock freq / (clkdiv + 1) */
    cfg.pc_origin = mem + 0;                           /*!< beginning of the program for state machine, 0~255 is available */
    cfg.side_en = 1;                                   /*!< allow instructions to perform sideset optionally, rather than on every instruction */
    cfg.side_pindir = 0;                               /*!< side data is asserted to pin OEs or pin values */
    cfg.fifo_join = PEC_FIFO_RX_JOIN_TX;               /*!< if rx_fifo and tx_fifo join */
    cfg.out_dir = PEC_SHIFT_DIR_TO_RIGHT;              /*!< shift OSR direction */
    cfg.in_dir = PEC_SHIFT_DIR_TO_RIGHT;               /*!< shift ISR direction */
    cfg.auto_pull = 0;                                 /*!< pull automatically when the output shift register is emptied */
    cfg.auto_push = 0;                                 /*!< push automatically when the input shift register is filled */
    cfg.wrap_top = mem + 255;                          /*!< after reaching this address, execution is wrapped to wrap_bottom */
    cfg.wrap_bottom = mem + 0;                         /*!< ater reaching wrap_top, execution is wrapped to this address */
    cfg.dma_tx = dpi->dma_enable;                      /*!< enable signal of dma_tx_req/ack interface */
    cfg.dma_rx = 0;                                    /*!< enable signal of dma_rx_req/ack interface */
    cfg.thresh_bits_pull = 0;                          /*!< number of bits shifted out of TXSR before autopull or conditional pull, 1~31, 0 present 32 */
    cfg.thresh_bits_push = 0;                          /*!< number of bits shifted into RXSR before autopush or conditional push, 1~31, 0 present 32 */
    cfg.thresh_fifo_tx = dpi->fifo_threshold;          /*!< threshold of TX FIFO for interrupt and DMA, 0~15 if no join, 0~31 if rx_join_tx */
    cfg.thresh_fifo_rx = 0;                            /*!< threshold of RX FIFO for interrupt and DMA, 0~15 if no join, 0~31 if tx_join_rx */
    bflb_pec_init(dev, &cfg);

    /* pin pclk */
    bflb_pec_pin_side_base_set(dev, dpi->pin_pclk % PEC_PIN_MAX);
    bflb_pec_pin_side_count_set(dev, 2);
    bflb_pec_pad_o_clr_bit(dev, dpi->pin_pclk % PEC_PIN_MAX);
    bflb_pec_pad_oe_set_bit(dev, dpi->pin_pclk % PEC_PIN_MAX);
    /* pin vsync */
    bflb_pec_pin_set_base_set(dev, PIN_SET_VSYNC_IDX, dpi->pin_vsync % PEC_PIN_MAX);
    bflb_pec_pin_set_count_set(dev, PIN_SET_VSYNC_IDX, 1);
    if (dpi->level_vsync) {
        bflb_pec_pad_o_clr_bit(dev, dpi->pin_vsync % PEC_PIN_MAX);
    }  else {
        bflb_pec_pad_o_set_bit(dev, dpi->pin_vsync % PEC_PIN_MAX);
    }
    bflb_pec_pad_oe_set_bit(dev, dpi->pin_vsync % PEC_PIN_MAX);
    /* pin hsync */
    bflb_pec_pin_set_base_set(dev, PIN_SET_HSYNC_IDX, dpi->pin_hsync % PEC_PIN_MAX);
    bflb_pec_pin_set_count_set(dev, PIN_SET_HSYNC_IDX, 1);
    if (dpi->level_hsync) {
        bflb_pec_pad_o_clr_bit(dev, dpi->pin_hsync % PEC_PIN_MAX);
    }  else {
        bflb_pec_pad_o_set_bit(dev, dpi->pin_hsync % PEC_PIN_MAX);
    }
    bflb_pec_pad_oe_set_bit(dev, dpi->pin_hsync % PEC_PIN_MAX);
    /* pin de */
    bflb_pec_pin_set_base_set(dev, PIN_SET_DE_IDX, dpi->pin_de % PEC_PIN_MAX);
    bflb_pec_pin_set_count_set(dev, PIN_SET_DE_IDX, 1);
    if (dpi->level_de) {
        bflb_pec_pad_o_clr_bit(dev, dpi->pin_de % PEC_PIN_MAX);
    }  else {
        bflb_pec_pad_o_set_bit(dev, dpi->pin_de % PEC_PIN_MAX);
    }
    bflb_pec_pad_oe_set_bit(dev, dpi->pin_de % PEC_PIN_MAX);
    /* pin data */
    for (uint8_t group = 0; group < sizeof(dpi->pin_data_count) / sizeof(dpi->pin_data_count[0]); group++) {
        if (dpi->pin_data_count[group] == 0) {
            break;
        }
        bflb_pec_pin_out_base_set(dev, group, dpi->pin_data_start[group] % PEC_PIN_MAX);
        bflb_pec_pin_out_count_set(dev, group, dpi->pin_data_count[group]);
        for (uint8_t count = 0; count < dpi->pin_data_count[group]; count++) {
            bflb_pec_pad_o_clr_bit(dev, (dpi->pin_data_start[group] + count) % PEC_PIN_MAX);
            bflb_pec_pad_oe_set_bit(dev, (dpi->pin_data_start[group] + count) % PEC_PIN_MAX);
        }
    }

    dpi->level_vsync = dpi->level_vsync ? 1 : 0;
    dpi->level_hsync = dpi->level_hsync ? 1 : 0;
    dpi->level_de = dpi->level_de ? 1 : 0;

    dpi->height = ((dpi->height ? dpi->height : 1) > 65535) ? 65535 : dpi->height;
    dpi->width = ((dpi->width ? dpi->width : 1) > 65535) ? 65535 : dpi->width;
    dpi->hsync = ((dpi->hsync ? dpi->hsync : 1) > 1024) ? 1024 : dpi->hsync;
    dpi->vsync = ((dpi->vsync ? dpi->vsync : 1) > 1024) ? 1024 : dpi->vsync;
    dpi->hbp = ((dpi->hbp ? dpi->hbp : 1) > 2048) ? 2048 : dpi->hbp;
    dpi->vbp = ((dpi->vbp ? dpi->vbp : 1) > 2048) ? 2048 : dpi->vbp;
    dpi->hfp = ((dpi->hfp ? dpi->hfp : 1) > 2048) ? 2048 : dpi->hfp;
    dpi->vfp = ((dpi->vfp ? dpi->vfp : 1) > 2048) ? 2048 : dpi->vfp;
    bflb_pec_sm_reg_write(dev, PARA_ACTURAL, ((dpi->height - 1) << PARA_BIT_POS_V) | ((dpi->width - 1) << PARA_BIT_POS_H));
    bflb_pec_sm_reg_write(dev, PARA_H_CFG, ((dpi->hsync - 1) << PARA_BIT_POS_SYNC) | ((dpi->hbp - 1) << PARA_BIT_POS_BP) | ((dpi->hfp - 1) << PARA_BIT_POS_FP));
    bflb_pec_sm_reg_write(dev, PARA_V_CFG, ((dpi->vsync - 1) << PARA_BIT_POS_SYNC) | ((dpi->vbp - 1) << PARA_BIT_POS_BP) | ((dpi->vfp - 1) << PARA_BIT_POS_FP));

    /* VSync */
    bflb_pec_mem_write(dev, mem, pec_instr_MOV(PARA_V_RECUR, PARA_V_CFG, PARA_BIT_POS_SYNC, PARA_BIT_LEN_SYNC, pec_instr_side(0, 0, 2))); mem++; /* get vsync length */
    bflb_pec_mem_write(dev, mem, pec_instr_SET(PIN_SET_VSYNC, dpi->level_vsync, pec_instr_side(2, 0, 2))); mem++; /* set vsync pin level */
    /* VSync: HSync */
    bflb_pec_mem_write(dev, mem, pec_instr_MOV(PARA_H_RECUR, PARA_H_CFG, PARA_BIT_POS_SYNC, PARA_BIT_LEN_SYNC, pec_instr_side(0, 0, 2))); mem++; /* get hsync length */
    bflb_pec_mem_write(dev, mem, pec_instr_SET(PIN_SET_HSYNC, dpi->level_hsync, pec_instr_side(2, 1, 2))); mem++; /* set hsync pin level */
    bflb_pec_mem_write(dev, mem, pec_instr_JRNZ(mem - 1, PARA_H_RECUR, PEC_INSTR_CMP_DEC, pec_instr_side(3, 1, 2))); mem++; /* recurse of hsync */
    /* VSync: HBP */
    bflb_pec_mem_write(dev, mem, pec_instr_MOV(PARA_H_RECUR, PARA_H_CFG, PARA_BIT_POS_BP, PARA_BIT_LEN_BP, pec_instr_side(0, 0, 2))); mem++; /* get hbp length */
    bflb_pec_mem_write(dev, mem, pec_instr_SET(PIN_SET_HSYNC, !(dpi->level_hsync), pec_instr_side(2, 1, 2))); mem++; /* set hsync pin level toggle */
    bflb_pec_mem_write(dev, mem, pec_instr_JRNZ(mem - 1, PARA_H_RECUR, PEC_INSTR_CMP_DEC, pec_instr_side(3, 1, 2))); mem++; /* recurse of hbp */
    /* VSync: HACT */
    bflb_pec_mem_write(dev, mem, pec_instr_MOV(PARA_H_RECUR, PARA_ACTURAL, PARA_BIT_POS_H, PARA_BIT_LEN_H, pec_instr_side(0, 0, 2))); mem++; /* get picture width */
    bflb_pec_mem_write(dev, mem, pec_instr_NOP(PEC_R0, pec_instr_side(2, 1, 2))); mem++; /* nop */
    bflb_pec_mem_write(dev, mem, pec_instr_JRNZ(mem - 1, PARA_H_RECUR, PEC_INSTR_CMP_DEC, pec_instr_side(3, 1, 2))); mem++; /* recurse of picture width */
    /* VSync: HFP */
    bflb_pec_mem_write(dev, mem, pec_instr_MOV(PARA_H_RECUR, PARA_H_CFG, PARA_BIT_POS_FP, PARA_BIT_LEN_FP, pec_instr_side(0, 0, 2))); mem++; /* get hfp length */
    bflb_pec_mem_write(dev, mem, pec_instr_NOP(PEC_R0, pec_instr_side(2, 1, 2))); mem++; /* nop */
    bflb_pec_mem_write(dev, mem, pec_instr_JRNZ(mem - 1, PARA_H_RECUR, PEC_INSTR_CMP_DEC, pec_instr_side(3, 1, 2))); mem++; /* recurse of hfp */
    /* VSync recurse */
    bflb_pec_mem_write(dev, mem, pec_instr_JRNZ(mem - 12, PARA_V_RECUR, PEC_INSTR_CMP_DEC, pec_instr_side(0, 0, 2))); mem++; /* recurse of vsync, jump to 'VSync: HSync' */

    /* VBP */
    bflb_pec_mem_write(dev, mem, pec_instr_MOV(PARA_V_RECUR, PARA_V_CFG, PARA_BIT_POS_BP, PARA_BIT_LEN_BP, pec_instr_side(0, 0, 2))); mem++; /* get vbp length */
    bflb_pec_mem_write(dev, mem, pec_instr_SET(PIN_SET_VSYNC, !(dpi->level_vsync), pec_instr_side(2, 0, 2))); mem++; /* set vsync pin level toggle */
    /* VBP: HSync */
    bflb_pec_mem_write(dev, mem, pec_instr_MOV(PARA_H_RECUR, PARA_H_CFG, PARA_BIT_POS_SYNC, PARA_BIT_LEN_SYNC, pec_instr_side(0, 0, 2))); mem++; /* get hsync length */
    bflb_pec_mem_write(dev, mem, pec_instr_SET(PIN_SET_HSYNC, dpi->level_hsync, pec_instr_side(2, 1, 2))); mem++; /* set hsync pin level */
    bflb_pec_mem_write(dev, mem, pec_instr_JRNZ(mem - 1, PARA_H_RECUR, PEC_INSTR_CMP_DEC, pec_instr_side(3, 1, 2))); mem++; /* recurse of hsync */
    /* VBP: HBP */
    bflb_pec_mem_write(dev, mem, pec_instr_MOV(PARA_H_RECUR, PARA_H_CFG, PARA_BIT_POS_BP, PARA_BIT_LEN_BP, pec_instr_side(0, 0, 2))); mem++; /* get hbp length */
    bflb_pec_mem_write(dev, mem, pec_instr_SET(PIN_SET_HSYNC, !(dpi->level_hsync), pec_instr_side(2, 1, 2))); mem++; /* set hsync pin level toggle */
    bflb_pec_mem_write(dev, mem, pec_instr_JRNZ(mem - 1, PARA_H_RECUR, PEC_INSTR_CMP_DEC, pec_instr_side(3, 1, 2))); mem++; /* recurse of hbp */
    /* VBP: HACT */
    bflb_pec_mem_write(dev, mem, pec_instr_MOV(PARA_H_RECUR, PARA_ACTURAL, PARA_BIT_POS_H, PARA_BIT_LEN_H, pec_instr_side(0, 0, 2))); mem++; /* get picture width */
    bflb_pec_mem_write(dev, mem, pec_instr_NOP(PEC_R0, pec_instr_side(2, 1, 2))); mem++; /* nop */
    bflb_pec_mem_write(dev, mem, pec_instr_JRNZ(mem - 1, PARA_H_RECUR, PEC_INSTR_CMP_DEC, pec_instr_side(3, 1, 2))); mem++; /* recurse of picture width */
    /* VBP: HFP */
    bflb_pec_mem_write(dev, mem, pec_instr_MOV(PARA_H_RECUR, PARA_H_CFG, PARA_BIT_POS_FP, PARA_BIT_LEN_FP, pec_instr_side(0, 0, 2))); mem++; /* get hfp length */
    bflb_pec_mem_write(dev, mem, pec_instr_NOP(PEC_R0, pec_instr_side(2, 1, 2))); mem++; /* nop */
    bflb_pec_mem_write(dev, mem, pec_instr_JRNZ(mem - 1, PARA_H_RECUR, PEC_INSTR_CMP_DEC, pec_instr_side(3, 1, 2))); mem++; /* recurse of hfp */
    /* VBP recurse */
    bflb_pec_mem_write(dev, mem, pec_instr_JRNZ(mem - 12, PARA_V_RECUR, PEC_INSTR_CMP_DEC, pec_instr_side(0, 0, 2))); mem++; /* recurse of vbp, jump to 'VBP: HSync' */

    /* VACT */
    bflb_pec_mem_write(dev, mem, pec_instr_MOV(PARA_V_RECUR, PARA_ACTURAL, PARA_BIT_POS_V, PARA_BIT_LEN_V, pec_instr_side(0, 0, 2))); mem++; /* get picture height */
    /* VACT: HSync */
    bflb_pec_mem_write(dev, mem, pec_instr_MOV(PARA_H_RECUR, PARA_H_CFG, PARA_BIT_POS_SYNC, PARA_BIT_LEN_SYNC, pec_instr_side(0, 0, 2))); mem++; /* get hsync length */
    bflb_pec_mem_write(dev, mem, pec_instr_SET(PIN_SET_HSYNC, dpi->level_hsync, pec_instr_side(2, 1, 2))); mem++; /* set hsync pin level */
    bflb_pec_mem_write(dev, mem, pec_instr_JRNZ(mem - 1, PARA_H_RECUR, PEC_INSTR_CMP_DEC, pec_instr_side(3, 1, 2))); mem++; /* recurse of hsync */
    /* VACT: HBP */
    bflb_pec_mem_write(dev, mem, pec_instr_MOV(PARA_H_RECUR, PARA_H_CFG, PARA_BIT_POS_BP, PARA_BIT_LEN_BP, pec_instr_side(0, 0, 2))); mem++; /* get hbp length */
    bflb_pec_mem_write(dev, mem, pec_instr_SET(PIN_SET_HSYNC, !(dpi->level_hsync), pec_instr_side(2, 1, 2))); mem++; /* set hsync pin level toggle */
    bflb_pec_mem_write(dev, mem, pec_instr_JRNZ(mem - 1, PARA_H_RECUR, PEC_INSTR_CMP_DEC, pec_instr_side(3, 1, 2))); mem++; /* recurse of hbp */
    /* VACT: HACT */
    bflb_pec_mem_write(dev, mem, pec_instr_MOV(PARA_H_RECUR, PARA_ACTURAL, PARA_BIT_POS_H, PARA_BIT_LEN_H, pec_instr_side(0, 0, 2))); mem++; /* get picture width */
    bflb_pec_mem_write(dev, mem, pec_instr_SET(PIN_SET_DE, dpi->level_de, pec_instr_side(0, 0, 2))); mem++; /* set de pin level */
    bflb_pec_mem_write(dev, mem, pec_instr_PULLB(pec_instr_side(0, 0, 2))); mem++;
    bflb_pec_mem_write(dev, mem, pec_instr_OUTPINVAL(0, dpi->pin_data_count[0], pec_instr_side(2, 1, 2))); mem++; /* output all data in only period */
    bflb_pec_mem_write(dev, mem, pec_instr_JRNZ(mem - 2, PARA_H_RECUR, PEC_INSTR_CMP_DEC, pec_instr_side(3, 0, 2))); mem++; /* recurse of picture width */
    /* VACT: HFP */
    bflb_pec_mem_write(dev, mem, pec_instr_MOV(PARA_H_RECUR, PARA_H_CFG, PARA_BIT_POS_FP, PARA_BIT_LEN_FP, pec_instr_side(0, 0, 2))); mem++; /* get hfp length */
    bflb_pec_mem_write(dev, mem, pec_instr_SET(PIN_SET_DE, !(dpi->level_de), pec_instr_side(2, 1, 2))); mem++; /* set de pin level toggle */
    bflb_pec_mem_write(dev, mem, pec_instr_JRNZ(mem - 1, PARA_H_RECUR, PEC_INSTR_CMP_DEC, pec_instr_side(3, 1, 2))); mem++; /* recurse of hfp */
    /* VACT recurse */
    bflb_pec_mem_write(dev, mem, pec_instr_JRNZ(mem - 14, PARA_V_RECUR, PEC_INSTR_CMP_DEC, pec_instr_side(0, 0, 2))); mem++; /* recurse of vact, jump to 'VACT: HSync' */

    /* VFP */
    bflb_pec_mem_write(dev, mem, pec_instr_MOV(PARA_V_RECUR, PARA_V_CFG, PARA_BIT_POS_FP, PARA_BIT_LEN_FP, pec_instr_side(0, 0, 2))); mem++; /* get vfp length */
    /* VFP: HSync */
    bflb_pec_mem_write(dev, mem, pec_instr_MOV(PARA_H_RECUR, PARA_H_CFG, PARA_BIT_POS_SYNC, PARA_BIT_LEN_SYNC, pec_instr_side(0, 0, 2))); mem++; /* get hsync length */
    bflb_pec_mem_write(dev, mem, pec_instr_SET(PIN_SET_HSYNC, dpi->level_hsync, pec_instr_side(2, 1, 2))); mem++; /* set hsync pin level */
    bflb_pec_mem_write(dev, mem, pec_instr_JRNZ(mem - 1, PARA_H_RECUR, PEC_INSTR_CMP_DEC, pec_instr_side(3, 1, 2))); mem++; /* recurse of hsync */
    /* VFP: HBP */
    bflb_pec_mem_write(dev, mem, pec_instr_MOV(PARA_H_RECUR, PARA_H_CFG, PARA_BIT_POS_BP, PARA_BIT_LEN_BP, pec_instr_side(0, 0, 2))); mem++; /* get hbp length */
    bflb_pec_mem_write(dev, mem, pec_instr_SET(PIN_SET_HSYNC, !(dpi->level_hsync), pec_instr_side(2, 1, 2))); mem++; /* set hsync pin level toggle */
    bflb_pec_mem_write(dev, mem, pec_instr_JRNZ(mem - 1, PARA_H_RECUR, PEC_INSTR_CMP_DEC, pec_instr_side(3, 1, 2))); mem++; /* recurse of hbp */
    /* VFP: HACT */
    bflb_pec_mem_write(dev, mem, pec_instr_MOV(PARA_H_RECUR, PARA_ACTURAL, PARA_BIT_POS_H, PARA_BIT_LEN_H, pec_instr_side(0, 0, 2))); mem++; /* get picture width */
    bflb_pec_mem_write(dev, mem, pec_instr_NOP(PEC_R0, pec_instr_side(2, 1, 2))); mem++; /* nop */
    bflb_pec_mem_write(dev, mem, pec_instr_JRNZ(mem - 1, PARA_H_RECUR, PEC_INSTR_CMP_DEC, pec_instr_side(3, 1, 2))); mem++; /* recurse of picture width */
    /* VFP: HFP */
    bflb_pec_mem_write(dev, mem, pec_instr_MOV(PARA_H_RECUR, PARA_H_CFG, PARA_BIT_POS_FP, PARA_BIT_LEN_FP, pec_instr_side(0, 0, 2))); mem++; /* get hfp length */
    bflb_pec_mem_write(dev, mem, pec_instr_NOP(PEC_R0, pec_instr_side(2, 1, 2))); mem++; /* nop */
    bflb_pec_mem_write(dev, mem, pec_instr_JRNZ(mem - 1, PARA_H_RECUR, PEC_INSTR_CMP_DEC, pec_instr_side(3, 1, 2))); mem++; /* recurse of hfp */
    /* VFP recurse */
    bflb_pec_mem_write(dev, mem, pec_instr_JRNZ(mem - 12, PARA_V_RECUR, PEC_INSTR_CMP_DEC, pec_instr_side(0, 0, 2))); mem++; /* recurse of vfp, jump to 'VFP: HSync' */

    /* one frame comlete, jump to origin */
    bflb_pec_mem_write(dev, mem, pec_instr_GOTO(cfg.pc_origin, pec_instr_side(0, 0, 2))); mem++;

    bflb_pec_enable(dev);
    return PEC_STS_OK;
}

/****************************************************************************/ /**
 * @brief  deinit dpi ip
 *
 * @param  dev: device handle
 *
 * @return NONE
 *
*******************************************************************************/
void bflb_pec_dpi_deinit(struct bflb_device_s *dev)
{
    bflb_pec_disable(dev);
}
