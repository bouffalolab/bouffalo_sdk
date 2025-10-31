/**
  ******************************************************************************
  * @file    ipc_reg.h
  * @version V1.0
  * @date    2024-09-11
  * @brief   This file is the description of.IP register
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#ifndef  __IPC_REG_H__
#define  __IPC_REG_H__

/****************************************************************************
 * Pre-processor Definitions
****************************************************************************/

/* Register offsets *********************************************************/

#define IPC_AP2NP_TRIGGER_OFFSET                                (0x0)/* ap2np_trigger */
#define IPC_NP2AP_RAW_STATUS_OFFSET                             (0x4)/* np2ap_raw_status */
#define IPC_NP2AP_ACK_OFFSET                                    (0x8)/* np2ap_ack */
#define IPC_NP2AP_UNMASK_SET_OFFSET                             (0xC)/* np2ap_unmask_set */
#define IPC_NP2AP_UNMASK_CLEAR_OFFSET                           (0x10)/* np2ap_unamsk_clear */
#define IPC_NP2AP_LINE_SEL_LOW_OFFSET                           (0x14)/* np2ap_line_sel_low */
#define IPC_NP2AP_LINE_SEL_HIGH_OFFSET                          (0x18)/* np2ap_line_sel_high */
#define IPC_NP2AP_STATUS_OFFSET                                 (0x1C)/* np2ap_status */
#define IPC_NP2AP_TRIGGER_OFFSET                                (0x20)/* np2ap_trigger */
#define IPC_AP2NP_RAW_STATUS_OFFSET                             (0x24)/* ap2np_raw_status */
#define IPC_AP2NP_ACK_OFFSET                                    (0x28)/* ap2np_ack */
#define IPC_AP2NP_UNMASK_SET_OFFSET                             (0x2C)/* ap2np_unmask_set */
#define IPC_AP2NP_UNMASK_CLEAR_OFFSET                            (0x30)/* ap2np_umask_clear */
#define IPC_AP2NP_LINE_SEL_LOW_OFFSET                           (0x34)/* ap2np_line_sel_low */
#define IPC_AP2NP_LINE_SEL_HIGH_OFFSET                          (0x38)/* ap2np_line_sel_high */
#define IPC_AP2NP_STATUS_OFFSET                                 (0x3C)/* ap2np_status */

/* Register Bitfield definitions *****************************************************/

/* 0x0 : ap2np_trigger */
#define IPC_AP2NP_TRIGGER_SHIFT                                 (0U)
#define IPC_AP2NP_TRIGGER_MASK                                  (0xffffffff<<IPC_AP2NP_TRIGGER_SHIFT)

/* 0x4 : np2ap_raw_status */
#define IPC_NP2AP_RAW_STATUS_SHIFT                              (0U)
#define IPC_NP2AP_RAW_STATUS_MASK                               (0xffffffff<<IPC_NP2AP_RAW_STATUS_SHIFT)

/* 0x8 : np2ap_ack */
#define IPC_NP2AP_ACK_SHIFT                                     (0U)
#define IPC_NP2AP_ACK_MASK                                      (0xffffffff<<IPC_NP2AP_ACK_SHIFT)

/* 0xC : np2ap_unmask_set */
#define IPC_NP2AP_UNMASK_SET_SHIFT                              (0U)
#define IPC_NP2AP_UNMASK_SET_MASK                               (0xffffffff<<IPC_NP2AP_UNMASK_SET_SHIFT)

/* 0x10 : np2ap_unamsk_clear */
#define IPC_NP2AP_UNAMSK_CLEAR_SHIFT                            (0U)
#define IPC_NP2AP_UNAMSK_CLEAR_MASK                             (0xffffffff<<IPC_NP2AP_UNAMSK_CLEAR_SHIFT)

/* 0x14 : np2ap_line_sel_low */
#define IPC_NP2AP_LINE_SEL_LOW_SHIFT                            (0U)
#define IPC_NP2AP_LINE_SEL_LOW_MASK                             (0xffffffff<<IPC_NP2AP_LINE_SEL_LOW_SHIFT)

/* 0x18 : np2ap_line_sel_high */
#define IPC_NP2AP_LINE_SEL_HIGH_SHIFT                           (0U)
#define IPC_NP2AP_LINE_SEL_HIGH_MASK                            (0xffffffff<<IPC_NP2AP_LINE_SEL_HIGH_SHIFT)

/* 0x1C : np2ap_status */
#define IPC_NP2AP_STATUS_SHIFT                                  (0U)
#define IPC_NP2AP_STATUS_MASK                                   (0xffffffff<<IPC_NP2AP_STATUS_SHIFT)

/* 0x20 : np2ap_trigger */
#define IPC_NP2AP_TRIGGER_SHIFT                                 (0U)
#define IPC_NP2AP_TRIGGER_MASK                                  (0xffffffff<<IPC_NP2AP_TRIGGER_SHIFT)

/* 0x24 : ap2np_raw_status */
#define IPC_AP2NP_RAW_STATUS_SHIFT                              (0U)
#define IPC_AP2NP_RAW_STATUS_MASK                               (0xffffffff<<IPC_AP2NP_RAW_STATUS_SHIFT)

/* 0x28 : ap2np_ack */
#define IPC_AP2NP_ACK_SHIFT                                     (0U)
#define IPC_AP2NP_ACK_MASK                                      (0xffffffff<<IPC_AP2NP_ACK_SHIFT)

/* 0x2C : ap2np_unmask_set */
#define IPC_AP2NP_UNMASK_SET_SHIFT                              (0U)
#define IPC_AP2NP_UNMASK_SET_MASK                               (0xffffffff<<IPC_AP2NP_UNMASK_SET_SHIFT)

/* 0x30 : ap2np_umask_clear */
#define IPC_AP2NP_UMASK_CLEAR_SHIFT                             (0U)
#define IPC_AP2NP_UMASK_CLEAR_MASK                              (0xffffffff<<IPC_AP2NP_UMASK_CLEAR_SHIFT)

/* 0x34 : ap2np_line_sel_low */
#define IPC_AP2NP_LINE_SEL_LOW_SHIFT                            (0U)
#define IPC_AP2NP_LINE_SEL_LOW_MASK                             (0xffffffff<<IPC_AP2NP_LINE_SEL_LOW_SHIFT)

/* 0x38 : ap2np_line_sel_high */
#define IPC_AP2NP_LINE_SEL_HIGH_SHIFT                           (0U)
#define IPC_AP2NP_LINE_SEL_HIGH_MASK                            (0xffffffff<<IPC_AP2NP_LINE_SEL_HIGH_SHIFT)

/* 0x3C : ap2np_status */
#define IPC_AP2NP_STATUS_SHIFT                                  (0U)
#define IPC_AP2NP_STATUS_MASK                                   (0xffffffff<<IPC_AP2NP_STATUS_SHIFT)

#endif  /* __IPC_REG_H__ */
