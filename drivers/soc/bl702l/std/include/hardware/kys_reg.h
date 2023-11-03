/**
 ******************************************************************************
 * @file    kys_reg.h
 * @version V1.0
 * @date    2022-05-10
 * @brief   This file is the description of.IP register
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 *modification, are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *notice, this list of conditions and the following disclaimer in the
 *documentation and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
#ifndef __KYS_REG_H__
#define __KYS_REG_H__

#include "bl702l.h"

/* 0x0 : ks_ctrl */
#define KYS_KS_CTRL_OFFSET (0x0)
#define KYS_KS_EN          KYS_KS_EN
#define KYS_KS_EN_POS      (0U)
#define KYS_KS_EN_LEN      (1U)
#define KYS_KS_EN_MSK      (((1U << KYS_KS_EN_LEN) - 1) << KYS_KS_EN_POS)
#define KYS_KS_EN_UMSK     (~(((1U << KYS_KS_EN_LEN) - 1) << KYS_KS_EN_POS))
#define KYS_FIFO_MODE      KYS_FIFO_MODE
#define KYS_FIFO_MODE_POS  (1U)
#define KYS_FIFO_MODE_LEN  (1U)
#define KYS_FIFO_MODE_MSK  (((1U << KYS_FIFO_MODE_LEN) - 1) << KYS_FIFO_MODE_POS)
#define KYS_FIFO_MODE_UMSK (~(((1U << KYS_FIFO_MODE_LEN) - 1) << KYS_FIFO_MODE_POS))
#define KYS_GHOST_EN       KYS_GHOST_EN
#define KYS_GHOST_EN_POS   (2U)
#define KYS_GHOST_EN_LEN   (1U)
#define KYS_GHOST_EN_MSK   (((1U << KYS_GHOST_EN_LEN) - 1) << KYS_GHOST_EN_POS)
#define KYS_GHOST_EN_UMSK  (~(((1U << KYS_GHOST_EN_LEN) - 1) << KYS_GHOST_EN_POS))
#define KYS_DEG_EN         KYS_DEG_EN
#define KYS_DEG_EN_POS     (3U)
#define KYS_DEG_EN_LEN     (1U)
#define KYS_DEG_EN_MSK     (((1U << KYS_DEG_EN_LEN) - 1) << KYS_DEG_EN_POS)
#define KYS_DEG_EN_UMSK    (~(((1U << KYS_DEG_EN_LEN) - 1) << KYS_DEG_EN_POS))
#define KYS_DEG_CNT        KYS_DEG_CNT
#define KYS_DEG_CNT_POS    (4U)
#define KYS_DEG_CNT_LEN    (4U)
#define KYS_DEG_CNT_MSK    (((1U << KYS_DEG_CNT_LEN) - 1) << KYS_DEG_CNT_POS)
#define KYS_DEG_CNT_UMSK   (~(((1U << KYS_DEG_CNT_LEN) - 1) << KYS_DEG_CNT_POS))
#define KYS_RC_EXT         KYS_RC_EXT
#define KYS_RC_EXT_POS     (8U)
#define KYS_RC_EXT_LEN     (2U)
#define KYS_RC_EXT_MSK     (((1U << KYS_RC_EXT_LEN) - 1) << KYS_RC_EXT_POS)
#define KYS_RC_EXT_UMSK    (~(((1U << KYS_RC_EXT_LEN) - 1) << KYS_RC_EXT_POS))
#define KYS_ROW_NUM        KYS_ROW_NUM
#define KYS_ROW_NUM_POS    (16U)
#define KYS_ROW_NUM_LEN    (3U)
#define KYS_ROW_NUM_MSK    (((1U << KYS_ROW_NUM_LEN) - 1) << KYS_ROW_NUM_POS)
#define KYS_ROW_NUM_UMSK   (~(((1U << KYS_ROW_NUM_LEN) - 1) << KYS_ROW_NUM_POS))
#define KYS_COL_NUM        KYS_COL_NUM
#define KYS_COL_NUM_POS    (20U)
#define KYS_COL_NUM_LEN    (3U)
#define KYS_COL_NUM_MSK    (((1U << KYS_COL_NUM_LEN) - 1) << KYS_COL_NUM_POS)
#define KYS_COL_NUM_UMSK   (~(((1U << KYS_COL_NUM_LEN) - 1) << KYS_COL_NUM_POS))

/* 0x10 : ks_int_en */
#define KYS_KS_INT_EN_OFFSET    (0x10)
#define KYS_KS_DONE_INT_EN      KYS_KS_DONE_INT_EN
#define KYS_KS_DONE_INT_EN_POS  (7U)
#define KYS_KS_DONE_INT_EN_LEN  (1U)
#define KYS_KS_DONE_INT_EN_MSK  (((1U << KYS_KS_DONE_INT_EN_LEN) - 1) << KYS_KS_DONE_INT_EN_POS)
#define KYS_KS_DONE_INT_EN_UMSK (~(((1U << KYS_KS_DONE_INT_EN_LEN) - 1) << KYS_KS_DONE_INT_EN_POS))
#define KYS_KEYFIFO_INT_EN      KYS_KEYFIFO_INT_EN
#define KYS_KEYFIFO_INT_EN_POS  (8U)
#define KYS_KEYFIFO_INT_EN_LEN  (4U)
#define KYS_KEYFIFO_INT_EN_MSK  (((1U << KYS_KEYFIFO_INT_EN_LEN) - 1) << KYS_KEYFIFO_INT_EN_POS)
#define KYS_KEYFIFO_INT_EN_UMSK (~(((1U << KYS_KEYFIFO_INT_EN_LEN) - 1) << KYS_KEYFIFO_INT_EN_POS))
#define KYS_GHOST_INT_EN        KYS_GHOST_INT_EN
#define KYS_GHOST_INT_EN_POS    (12U)
#define KYS_GHOST_INT_EN_LEN    (1U)
#define KYS_GHOST_INT_EN_MSK    (((1U << KYS_GHOST_INT_EN_LEN) - 1) << KYS_GHOST_INT_EN_POS)
#define KYS_GHOST_INT_EN_UMSK   (~(((1U << KYS_GHOST_INT_EN_LEN) - 1) << KYS_GHOST_INT_EN_POS))

/* 0x14 : ks_int_sts */
#define KYS_KS_INT_STS_OFFSET     (0x14)
#define KYS_KEYCODE_DONE          KYS_KEYCODE_DONE
#define KYS_KEYCODE_DONE_POS      (7U)
#define KYS_KEYCODE_DONE_LEN      (1U)
#define KYS_KEYCODE_DONE_MSK      (((1U << KYS_KEYCODE_DONE_LEN) - 1) << KYS_KEYCODE_DONE_POS)
#define KYS_KEYCODE_DONE_UMSK     (~(((1U << KYS_KEYCODE_DONE_LEN) - 1) << KYS_KEYCODE_DONE_POS))
#define KYS_KEYFIFO_FULL          KYS_KEYFIFO_FULL
#define KYS_KEYFIFO_FULL_POS      (8U)
#define KYS_KEYFIFO_FULL_LEN      (1U)
#define KYS_KEYFIFO_FULL_MSK      (((1U << KYS_KEYFIFO_FULL_LEN) - 1) << KYS_KEYFIFO_FULL_POS)
#define KYS_KEYFIFO_FULL_UMSK     (~(((1U << KYS_KEYFIFO_FULL_LEN) - 1) << KYS_KEYFIFO_FULL_POS))
#define KYS_KEYFIFO_HALF          KYS_KEYFIFO_HALF
#define KYS_KEYFIFO_HALF_POS      (9U)
#define KYS_KEYFIFO_HALF_LEN      (1U)
#define KYS_KEYFIFO_HALF_MSK      (((1U << KYS_KEYFIFO_HALF_LEN) - 1) << KYS_KEYFIFO_HALF_POS)
#define KYS_KEYFIFO_HALF_UMSK     (~(((1U << KYS_KEYFIFO_HALF_LEN) - 1) << KYS_KEYFIFO_HALF_POS))
#define KYS_KEYFIFO_QUARTER       KYS_KEYFIFO_QUARTER
#define KYS_KEYFIFO_QUARTER_POS   (10U)
#define KYS_KEYFIFO_QUARTER_LEN   (1U)
#define KYS_KEYFIFO_QUARTER_MSK   (((1U << KYS_KEYFIFO_QUARTER_LEN) - 1) << KYS_KEYFIFO_QUARTER_POS)
#define KYS_KEYFIFO_QUARTER_UMSK  (~(((1U << KYS_KEYFIFO_QUARTER_LEN) - 1) << KYS_KEYFIFO_QUARTER_POS))
#define KYS_KEYFIFO_NONEMPTY      KYS_KEYFIFO_NONEMPTY
#define KYS_KEYFIFO_NONEMPTY_POS  (11U)
#define KYS_KEYFIFO_NONEMPTY_LEN  (1U)
#define KYS_KEYFIFO_NONEMPTY_MSK  (((1U << KYS_KEYFIFO_NONEMPTY_LEN) - 1) << KYS_KEYFIFO_NONEMPTY_POS)
#define KYS_KEYFIFO_NONEMPTY_UMSK (~(((1U << KYS_KEYFIFO_NONEMPTY_LEN) - 1) << KYS_KEYFIFO_NONEMPTY_POS))
#define KYS_GHOST_DET             KYS_GHOST_DET
#define KYS_GHOST_DET_POS         (12U)
#define KYS_GHOST_DET_LEN         (1U)
#define KYS_GHOST_DET_MSK         (((1U << KYS_GHOST_DET_LEN) - 1) << KYS_GHOST_DET_POS)
#define KYS_GHOST_DET_UMSK        (~(((1U << KYS_GHOST_DET_LEN) - 1) << KYS_GHOST_DET_POS))

/* 0x18 : keycode_clr */
#define KYS_KEYCODE_CLR_OFFSET    (0x18)
#define KYS_KS_DONE_CLR           KYS_KS_DONE_CLR
#define KYS_KS_DONE_CLR_POS       (7U)
#define KYS_KS_DONE_CLR_LEN       (1U)
#define KYS_KS_DONE_CLR_MSK       (((1U << KYS_KS_DONE_CLR_LEN) - 1) << KYS_KS_DONE_CLR_POS)
#define KYS_KS_DONE_CLR_UMSK      (~(((1U << KYS_KS_DONE_CLR_LEN) - 1) << KYS_KS_DONE_CLR_POS))
#define KYS_KEYFIFO_FULL_CLR      KYS_KEYFIFO_FULL_CLR
#define KYS_KEYFIFO_FULL_CLR_POS  (8U)
#define KYS_KEYFIFO_FULL_CLR_LEN  (1U)
#define KYS_KEYFIFO_FULL_CLR_MSK  (((1U << KYS_KEYFIFO_FULL_CLR_LEN) - 1) << KYS_KEYFIFO_FULL_CLR_POS)
#define KYS_KEYFIFO_FULL_CLR_UMSK (~(((1U << KYS_KEYFIFO_FULL_CLR_LEN) - 1) << KYS_KEYFIFO_FULL_CLR_POS))
#define KYS_GHOST_CLR             KYS_GHOST_CLR
#define KYS_GHOST_CLR_POS         (12U)
#define KYS_GHOST_CLR_LEN         (1U)
#define KYS_GHOST_CLR_MSK         (((1U << KYS_GHOST_CLR_LEN) - 1) << KYS_GHOST_CLR_POS)
#define KYS_GHOST_CLR_UMSK        (~(((1U << KYS_GHOST_CLR_LEN) - 1) << KYS_GHOST_CLR_POS))

/* 0x30 : keyfifo_idx */
#define KYS_KEYFIFO_IDX_OFFSET (0x30)
#define KYS_KEYFIFO_HEAD       KYS_KEYFIFO_HEAD
#define KYS_KEYFIFO_HEAD_POS   (0U)
#define KYS_KEYFIFO_HEAD_LEN   (3U)
#define KYS_KEYFIFO_HEAD_MSK   (((1U << KYS_KEYFIFO_HEAD_LEN) - 1) << KYS_KEYFIFO_HEAD_POS)
#define KYS_KEYFIFO_HEAD_UMSK  (~(((1U << KYS_KEYFIFO_HEAD_LEN) - 1) << KYS_KEYFIFO_HEAD_POS))
#define KYS_KEYFIFO_TAIL       KYS_KEYFIFO_TAIL
#define KYS_KEYFIFO_TAIL_POS   (8U)
#define KYS_KEYFIFO_TAIL_LEN   (3U)
#define KYS_KEYFIFO_TAIL_MSK   (((1U << KYS_KEYFIFO_TAIL_LEN) - 1) << KYS_KEYFIFO_TAIL_POS)
#define KYS_KEYFIFO_TAIL_UMSK  (~(((1U << KYS_KEYFIFO_TAIL_LEN) - 1) << KYS_KEYFIFO_TAIL_POS))

/* 0x34 : keyfifo_value */
#define KYS_KEYFIFO_VALUE_OFFSET (0x34)
#define KYS_KEYFIFO_VALUE        KYS_KEYFIFO_VALUE
#define KYS_KEYFIFO_VALUE_POS    (0U)
#define KYS_KEYFIFO_VALUE_LEN    (8U)
#define KYS_KEYFIFO_VALUE_MSK    (((1U << KYS_KEYFIFO_VALUE_LEN) - 1) << KYS_KEYFIFO_VALUE_POS)
#define KYS_KEYFIFO_VALUE_UMSK   (~(((1U << KYS_KEYFIFO_VALUE_LEN) - 1) << KYS_KEYFIFO_VALUE_POS))

/* 0x3C : ks_ctrl2 */
#define KYS_KS_CTRL2_OFFSET (0x3C)
#define KYS_COL_START       KYS_COL_START
#define KYS_COL_START_POS   (0U)
#define KYS_COL_START_LEN   (4U)
#define KYS_COL_START_MSK   (((1U << KYS_COL_START_LEN) - 1) << KYS_COL_START_POS)
#define KYS_COL_START_UMSK  (~(((1U << KYS_COL_START_LEN) - 1) << KYS_COL_START_POS))
#define KYS_ROW_START       KYS_ROW_START
#define KYS_ROW_START_POS   (20U)
#define KYS_ROW_START_LEN   (2U)
#define KYS_ROW_START_MSK   (((1U << KYS_ROW_START_LEN) - 1) << KYS_ROW_START_POS)
#define KYS_ROW_START_UMSK  (~(((1U << KYS_ROW_START_LEN) - 1) << KYS_ROW_START_POS))

struct kys_reg {
    /* 0x0 : ks_ctrl */
    union {
        struct {
            uint32_t ks_en          : 1; /* [    0],        r/w,        0x0 */
            uint32_t fifo_mode      : 1; /* [    1],        r/w,        0x0 */
            uint32_t ghost_en       : 1; /* [    2],        r/w,        0x0 */
            uint32_t deg_en         : 1; /* [    3],        r/w,        0x0 */
            uint32_t deg_cnt        : 4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t rc_ext         : 2; /* [ 9: 8],        r/w,        0x3 */
            uint32_t reserved_10_15 : 6; /* [15:10],       rsvd,        0x0 */
            uint32_t row_num        : 3; /* [18:16],        r/w,        0x7 */
            uint32_t reserved_19    : 1; /* [   19],       rsvd,        0x0 */
            uint32_t col_num        : 3; /* [22:20],        r/w,        0x7 */
            uint32_t reserved_23_31 : 9; /* [31:23],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } ks_ctrl;

    /* 0x4  reserved */
    uint8_t RESERVED0x4[12];

    /* 0x10 : ks_int_en */
    union {
        struct {
            uint32_t reserved_0_6   : 7;  /* [ 6: 0],       rsvd,        0x0 */
            uint32_t ks_done_int_en : 1;  /* [    7],        r/w,        0x1 */
            uint32_t keyfifo_int_en : 4;  /* [11: 8],        r/w,        0x0 */
            uint32_t ghost_int_en   : 1;  /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31 : 19; /* [31:13],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } ks_int_en;

    /* 0x14 : ks_int_sts */
    union {
        struct {
            uint32_t reserved_0_6     : 7;  /* [ 6: 0],       rsvd,        0x0 */
            uint32_t ks_done          : 1;  /* [    7],          r,        0x0 */
            uint32_t keyfifo_full     : 1;  /* [    8],          r,        0x0 */
            uint32_t keyfifo_half     : 1;  /* [    9],          r,        0x0 */
            uint32_t keyfifo_quarter  : 1;  /* [   10],          r,        0x0 */
            uint32_t keyfifo_nonempty : 1;  /* [   11],          r,        0x0 */
            uint32_t ghost_det        : 1;  /* [   12],          r,        0x0 */
            uint32_t reserved_13_31   : 19; /* [31:13],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } ks_int_sts;

    /* 0x18 : keycode_clr */
    union {
        struct {
            uint32_t reserved_0_6   : 7;  /* [ 6: 0],       rsvd,        0x0 */
            uint32_t ks_done_clr    : 1;  /* [    7],        w1c,        0x0 */
            uint32_t keyfifo_clr    : 1;  /* [    8],        w1c,        0x0 */
            uint32_t reserved_9_11  : 3;  /* [11: 9],       rsvd,        0x0 */
            uint32_t ghost_clr      : 1;  /* [   12],        w1c,        0x0 */
            uint32_t reserved_13_31 : 19; /* [31:13],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } keycode_clr;

    /* 0x1C : reserved */
    uint8_t RESERVED0x1C[20];

    /* 0x30 : keyfifo_idx */
    union {
        struct {
            uint32_t keyfifo_head   : 3;  /* [ 2: 0],          r,        0x0 */
            uint32_t reserved_3_7   : 5;  /* [ 7: 3],       rsvd,        0x0 */
            uint32_t keyfifo_tail   : 3;  /* [10: 8],          r,        0x0 */
            uint32_t reserved_11_31 : 21; /* [31:11],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } keyfifo_idx;

    /* 0x34 : keyfifo_value */
    union {
        struct {
            uint32_t keyfifo_value : 8;  /* [ 7: 0],          r,       0xff */
            uint32_t reserved_8_31 : 24; /* [31: 8],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } keyfifo_value;
};

typedef volatile struct kys_reg kys_reg_t;

#endif /* __KYS_REG_H__ */
