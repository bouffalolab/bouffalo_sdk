/**
  ******************************************************************************
  * @file    canfd_reg.h
  * @version V1.0
  * @date    2025-04-03
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
#ifndef __HARDWARE_CANFD_H__
#define __HARDWARE_CANFD_H__

/****************************************************************************
 * Pre-processor Definitions
****************************************************************************/

/* Register offsets *********************************************************/

#define CANFD_RBUF_OFFSET                                       (0x00)
#define CANFD_RBUF_ID_OFFSET                                    (0x00)
#define CANFD_RBUF_CTRL_OFFSET                                  (0x04)
#define CANFD_RBUF_DATA_OFFSET                                  (0x08)
#define CANFD_RBUF_RTS_OFFSET                                   (0x48)
#define CANFD_TBUF_OFFSET                                       (0x50)
#define CANFD_TBUF_ID_OFFSET                                    (0x50)
#define CANFD_TBUF_CTRL_OFFSET                                  (0x54)
#define CANFD_TBUF_DATA_OFFSET                                  (0x58)
#define CANFD_TTS_OFFSET                                        (0x98)
#define CANFD_CTRL_OFFSET                                       (0xA0)
#define CANFD_INT_OFFSET                                        (0xA4)
#define CANFD_S_TIMING_OFFSET                                   (0xA8)
#define CANFD_F_TIMING_OFFSET                                   (0xAC)
#define CANFD_ERROR_OFFSET                                      (0xB0)
#define CANFD_ACFCTRL_OFFSET                                    (0xB4)
#define CANFD_ACF_X_OFFSET                                      (0xB8)
#define CANFD_VERSION_OFFSET                                    (0xBC)

#define CANFD_TIMECFG_OFFSET                                    (0xB4)
#define CANFD_TTCFG_OFFSET                                      (0xBC)
#define CANFD_REF_MSG_OFFSET                                    (0xC0)
#define CANFD_TRIG_CFG_OFFSET                                   (0xC4)
#define CANFD_TT_WTRIG_OFFSET                                   (0xC8)
#define CANFD_MEM_ES_OFFSET                                     (0xCC)
#define CANFD_SCFG_OFFSET                                       (0xD0)

/* Register Bitfield definitions *****************************************************/

#define CANFD_RBUF_ID_ESI                                       (1 << 31U)
#define CANFD_TBUF_ID_TTSEN                                     (1 << 31U)
#define CANFD_ID_STANDARD_MASK                                  (0x7FF)
#define CANFD_ID_EXTENDED_MASK                                  (0x1FFFFFFF)

#define CANFD_RBUF_CTRL_DLC_SHIFT                               (0U)
#define CANFD_RBUF_CTRL_DLC_MASK                                (0xF << CANFD_RBUF_CTRL_DLC_SHIFT)
#define CANFD_RBUF_CTRL_BRS                                     (1 << 4U)
#define CANFD_RBUF_CTRL_FDF                                     (1 << 5U)
#define CANFD_RBUF_CTRL_RTR                                     (1 << 6U)
#define CANFD_RBUF_CTRL_IDE                                     (1 << 7U)

#define CANFD_TBUF_CTRL_DLC_SHIFT                               (0U)
#define CANFD_TBUF_CTRL_DLC_MASK                                (0xF << CANFD_TBUF_CTRL_DLC_SHIFT)
#define CANFD_TBUF_CTRL_BRS                                     (1 << 4U)
#define CANFD_TBUF_CTRL_FDF                                     (1 << 5U)
#define CANFD_TBUF_CTRL_RTR                                     (1 << 6U)
#define CANFD_TBUF_CTRL_IDE                                     (1 << 7U)

/* 0xA0 : Configuration and Status Register CFG_STAT */
#define CANFD_CFG_STAT_BUSOFF                                   (1 << 0U)
#define CANFD_CFG_STAT_TACTIVE                                  (1 << 1U)
#define CANFD_CFG_STAT_RACTIVE                                  (1 << 2U)
#define CANFD_CFG_STAT_TSSS                                     (1 << 3U)
#define CANFD_CFG_STAT_TPSS                                     (1 << 4U)
#define CANFD_CFG_STAT_LBMI                                     (1 << 5U)
#define CANFD_CFG_STAT_LBME                                     (1 << 6U)
#define CANFD_CFG_STAT_RESET                                    (1 << 7U)

/* 0xA1 : Command Register TCMD */
#define CANFD_TCMD_TSA                                          (1 << 8U)
#define CANFD_TCMD_TSALL                                        (1 << 9U)
#define CANFD_TCMD_TSONE                                        (1 << 10U)
#define CANFD_TCMD_TPA                                          (1 << 11U)
#define CANFD_TCMD_TPE                                          (1 << 12U)
#define CANFD_TCMD_STBY                                         (1 << 13U)
#define CANFD_TCMD_LOM                                          (1 << 14U)
#define CANFD_TCMD_TBSEL                                        (1 << 15U)

/* 0xA2 : Transmit Control Register TCTRL */
#define CANFD_TCTRL_TSSTAT_SHIFT                                (16U)
#define CANFD_TCTRL_TSSTAT_MASK                                 (0x3 << CANFD_TCTRL_TSSTAT_SHIFT)
#define CANFD_TCTRL_TTTBM                                       (1 << 20U)
#define CANFD_TCTRL_TSMODE                                      (1 << 21U)
#define CANFD_TCTRL_TSNEXT                                      (1 << 22U)
#define CANFD_TCTRL_FD_ISO                                      (1 << 23U)

/* 0xA3 : Receive Control Register RCTRL */
#define CANFD_RCTRL_RSTAT_SHIFT                                 (24U)
#define CANFD_RCTRL_RSTAT_MASK                                  (0x3 << CANFD_RCTRL_RSTAT_SHIFT)
#define CANFD_RCTRL_RBALL                                       (1 << 27U)
#define CANFD_RCTRL_RREL                                        (1 << 28U)
#define CANFD_RCTRL_ROV                                         (1 << 29U)
#define CANFD_RCTRL_ROM                                         (1 << 30U)
#define CANFD_RCTRL_SACK                                        (1 << 31U)

/* 0xA4 : Receive and Transmit Interrupt Enable Register RTIE */
#define CANFD_RTIE_TSFF                                         (1 << 0U)
#define CANFD_RTIE_EIE                                          (1 << 1U)
#define CANFD_RTIE_TSIE                                         (1 << 2U)
#define CANFD_RTIE_TPIE                                         (1 << 3U)
#define CANFD_RTIE_RAFIE                                        (1 << 4U)
#define CANFD_RTIE_RFIE                                         (1 << 5U)
#define CANFD_RTIE_ROIE                                         (1 << 6U)
#define CANFD_RTIE_RIE                                          (1 << 7U)

/* 0xA5 : Receive and Transmit Interrupt Flag Register RTIF */
#define CANFD_RTIF_AIF                                          (1 << 8U)
#define CANFD_RTIF_EIF                                          (1 << 9U)
#define CANFD_RTIF_TSIF                                         (1 << 10U)
#define CANFD_RTIF_TPIF                                         (1 << 11U)
#define CANFD_RTIF_RAFIF                                        (1 << 12U)
#define CANFD_RTIF_RFIF                                         (1 << 13U)
#define CANFD_RTIF_ROIF                                         (1 << 14U)
#define CANFD_RTIF_RIF                                          (1 << 15U)

/* 0xA6 : Error Interrupt Enable and Flag Register ERRINT */
#define CANFD_ERRINT_BEIF                                       (1 << 16U)
#define CANFD_ERRINT_BEIE                                       (1 << 17U)
#define CANFD_ERRINT_ALIF                                       (1 << 18U)
#define CANFD_ERRINT_ALIE                                       (1 << 19U)
#define CANFD_ERRINT_EPIF                                       (1 << 20U)
#define CANFD_ERRINT_EPIE                                       (1 << 21U)
#define CANFD_ERRINT_EPASS                                      (1 << 22U)
#define CANFD_ERRINT_RWARN                                      (1 << 23U)

/* 0xA7 : Warning Limits Register LIMIT */
#define CANFD_LIMIT_EWL_SHIFT                                   (24U)
#define CANFD_LIMIT_EWL_MASK                                    (0xF << CANFD_LIMIT_EWL_SHIFT)
#define CANFD_LIMIT_AFWL_SHIFT                                  (28U)
#define CANFD_LIMIT_AFWL_MASK                                   (0xF << CANFD_LIMIT_AFWL_SHIFT)

/* 0xA8 : Bit Timing Register S_Seg_1 */
#define CANFD_S_SEG_1_SHIFT                                     (0U)
#define CANFD_S_SEG_1_MASK                                      (0xFF << CANFD_S_SEG_1_SHIFT)

/* 0xA9 : Bit Timing Register S_Seg_2 */
#define CANFD_S_SEG_2_SHIFT                                     (8U)
#define CANFD_S_SEG_2_MASK                                      (0x7F << CANFD_S_SEG_2_SHIFT)

/* 0xAA : Bit Timing Register S_SJW */
#define CANFD_S_SJW_SHIFT                                       (16U)
#define CANFD_S_SJW_MASK                                        (0x7F << CANFD_S_SJW_SHIFT)

/* 0xAB : Prescaler Registers S_PRESC */
#define CANFD_S_PRESC_SHIFT                                     (24U)
#define CANFD_S_PRESC_MASK                                      (0xFF << CANFD_S_PRESC_SHIFT)

/* 0xAC : Bit Timing Register F_Seg_1 */
#define CANFD_F_SEG_1_SHIFT                                     (0U)
#define CANFD_F_SEG_1_MASK                                      (0x1F << CANFD_F_SEG_1_SHIFT)

/* 0xAD : Bit Timing Register F_Seg_2 */
#define CANFD_F_SEG_2_SHIFT                                     (8U)
#define CANFD_F_SEG_2_MASK                                      (0xF << CANFD_F_SEG_2_SHIFT)

/* 0xAE : Bit Timing Register F_SJW */
#define CANFD_F_SJW_SHIFT                                       (16U)
#define CANFD_F_SJW_MASK                                        (0xF << CANFD_F_SJW_SHIFT)

/* 0xAF : Prescaler Registers F_PRESC */
#define CANFD_F_PRESC_SHIFT                                     (24U)
#define CANFD_F_PRESC_MASK                                      (0xFF << CANFD_F_PRESC_SHIFT)

/* 0xB0 : Error and Arbitration Lost Capture Register */
#define CANFD_EALCAP_ALC_SHIFT                                  (0U)
#define CANFD_EALCAP_ALC_MASK                                   (0x1F << CANFD_EALCAP_ALC_SHIFT)
#define CANFD_EALCAP_KOER_SHIFT                                 (5U)
#define CANFD_EALCAP_KOER_MASK                                  (0x7 << CANFD_EALCAP_KOER_SHIFT)

/* 0xB1 : Transmitter Delay Compensation Register TDC */
#define CANFD_TDC_SSPOFF_SHIFT                                  (8U)
#define CANFD_TDC_SSPOFF_MASK                                   (0x7F << CANFD_TDC_SSPOFF_SHIFT)
#define CANFD_TDC_TDCEN                                         (1 << 15U)

/* 0xB2 : Error Counter Registers RECNT */
#define CANFD_RECNT_RECNT_SHIFT                                 (16U)
#define CANFD_RECNT_RECNT_MASK                                  (0xFF << CANFD_RECNT_RECNT_SHIFT)

/* 0xB3 : Error Counter Registers TECNT */
#define CANFD_TECNT_TECNT_SHIFT                                 (24U)
#define CANFD_TECNT_TECNT_MASK                                  (0xFF << CANFD_TECNT_TECNT_SHIFT)

/* 0xB4 : Acceptance Filter Control Register ACFCTRL */
#define CANFD_ACFCTRL_ACFADR_SHIFT                              (0U)
#define CANFD_ACFCTRL_ACFADR_MASK                               (0xF << CANFD_ACFCTRL_ACFADR_SHIFT)
#define CANFD_ACFCTRL_SELMASK                                   (1 << 5U)

/* 0xB5 : CiA 603 Time Stamping TIMECFG */
#define CANFD_TIMECFG_TIMEEN                                    (1 << 8U)
#define CANFD_TIMECFG_TIMEPOS                                   (1 << 9U)

/* 0xB6~0xB7 : Acceptance Filter Enable ACF_EN */
#define CANFD_ACF_EN_SHIFT                                      (16U)
#define CANFD_ACF_EN_MASK                                       (0xFFFF << CANFD_ACF_EN_SHIFT)

/* 0xB8 : Acceptance filter code and mask Register */
#define CANFD_ACF_X_AIDE                                        (1 << 29U)
#define CANFD_ACF_X_AIDEE                                       (1 << 30U)

/* 0xBC : version */
#define CANFD_VERSION_SHIFT                                     (0U)
#define CANFD_VERSION_MASK                                      (0xFFFF << CANFD_VERSION_SHIFT)

/* 0xBE : TTCAN: TB Slot Pointer TBSLOT */
#define CANFD_TBSLOT_TBPTR_SHIFT                                (16U)
#define CANFD_TBSLOT_TBPTR_MASK                                 (0x3F << CANFD_TBSLOT_TBPTR_SHIFT)
#define CANFD_TBSLOT_TBF                                        (1 << 22U)
#define CANFD_TBSLOT_TBE                                        (1 << 23U)

/* 0xBF : TTCAN: Time Trigger Configuration TTCFG */
#define CANFD_TTCFG_TTEN                                        (1 << 24U)
#define CANFD_TTCFG_T_PRESC_SHIFT                               (25U)
#define CANFD_TTCFG_T_PRESC_MASK                                (0x3 << CANFD_TTCFG_T_PRESC_SHIFT)
#define CANFD_TTCFG_TTIF                                        (1 << 27U)
#define CANFD_TTCFG_TTIE                                        (1 << 28U)
#define CANFD_TTCFG_TEIF                                        (1 << 29U)
#define CANFD_TTCFG_WTIF                                        (1 << 30U)
#define CANFD_TTCFG_WTIE                                        (1 << 31U)

/* 0xC0 : TTCAN: Reference Message REF_MSG_0 to REF_MSG_ 3 */
#define CANFD_REF_MSG_REF_ID_SHIFT                              (0U)
#define CANFD_REF_MSG_REF_ID_MASK                               (0x1FFFFFFF << CANFD_REF_MSG_REF_ID_SHIFT)
#define CANFD_REF_MSG_REF_IDE                                   (1 << 31U)

/* 0xC4 : TTCAN: Trigger Configuration TRIG_CFG_0 */
#define CANFD_TRIG_CFG_0_TTPTR_SHIFT                            (0U)
#define CANFD_TRIG_CFG_0_TTPTR_MASK                             (0x3F << CANFD_TRIG_CFG_0_TTPTR_SHIFT)

/* 0xC5 : TTCAN: Trigger Configuration TRIG_CFG_1 */
#define CANFD_TRIG_CFG_1_TTYPE_SHIFT                            (8U)
#define CANFD_TRIG_CFG_1_TTYPE_MASK                             (0x7 << CANFD_TRIG_CFG_1_TTYPE_SHIFT)
#define CANFD_TRIG_CFG_1_TEW_SHIFT                              (12U)
#define CANFD_TRIG_CFG_1_TEW_MASK                               (0xF << CANFD_TRIG_CFG_1_TEW_SHIFT)

/* 0xC6 : TTCAN: Trigger Time TT_TRIG_0 and TT_TRIG_1 */
#define CANFD_TT_TRIG_SHIFT                                     (16U)
#define CANFD_TT_TRIG_MASK                                      (0xFFFF << CANFD_TT_TRIG_SHIFT)

/* 0xC8 : TTCAN: Watch Trigger Time TT_WTRIG_0 and TT_WTRIG_1 */
#define CANFD_TT_WTRIG_SHIFT                                    (0U)
#define CANFD_TT_WTRIG_MASK                                     (0xFFFF << CANFD_TT_WTRIG_SHIFT)

/* 0xCA : MEM_PROT: Memory Protection */
#define CANFD_MEM_PROT_MPEN                                     (1 << 0U)
#define CANFD_MEM_PROT_MDWIE                                    (1 << 1U)
#define CANFD_MEM_PROT_MDWIF                                    (1 << 2U)
#define CANFD_MEM_PROT_MDEIF                                    (1 << 3U)
#define CANFD_MEM_PROT_MAEIF                                    (1 << 4U)

/* 0xCB : MEM_STAT: Memory Status */
#define CANFD_MEM_STAT_ACFA                                     (1 << 0U)
#define CANFD_MEM_STAT_TXS                                      (1 << 1U)
#define CANFD_MEM_STAT_TXB                                      (1 << 2U)
#define CANFD_MEM_STAT_HELOC_SHIFT                              (3U)
#define CANFD_MEM_STAT_HELOC_MASK                               (0x3 << CANFD_MEM_STAT_HELOC_SHIFT)

/* 0xCC : MEM_ES_0: Memory Error Stimulation 0 */
#define CANFD_MEM_ES_0_MEBP1_SHIFT                              (0U)
#define CANFD_MEM_ES_0_MEBP1_MASK                               (0x3F << CANFD_MEM_ES_0_MEBP1_SHIFT)
#define CANFD_MEM_ES_0_ME1EE                                    (1 << 6U)
#define CANFD_MEM_ES_0_MEAEE                                    (1 << 7U)

/* 0xCD : MEM_ES_1: Memory Error Stimulation 1 */
#define CANFD_MEM_ES_1_MEBP2_SHIFT                              (0U)
#define CANFD_MEM_ES_1_MEBP2_MASK                               (0x3F << CANFD_MEM_ES_1_MEBP2_SHIFT)
#define CANFD_MEM_ES_1_ME2EE                                    (1 << 6U)

/* 0xCE : MEM_ES_2: Memory Error Stimulation 2 */
#define CANFD_MEM_ES_2_MEEEC_SHIFT                              (0U)
#define CANFD_MEM_ES_2_MEEEC_MASK                               (0xF << CANFD_MEM_ES_2_MEEEC_SHIFT)
#define CANFD_MEM_ES_2_MENEC_SHIFT                              (4U)
#define CANFD_MEM_ES_2_MENEC_MASK                               (0xF << CANFD_MEM_ES_2_MENEC_SHIFT)

/* 0xCF : MEM_ES_3: Memory Error Stimulation 3 */
#define CANFD_MEM_ES_3_MEL_SHIFT                                (0U)
#define CANFD_MEM_ES_3_MEL_MASK                                 (0x3 << CANFD_MEM_ES_3_MEL_SHIFT)
#define CANFD_MEM_ES_3_MES                                      (1 << 2U)

/* 0xD0 : SCFG: Safety Configuration */
#define CANFD_SCFG_XMREN                                        (1 << 0U)
#define CANFD_SCFG_SEIF                                         (1 << 1U)
#define CANFD_SCFG_SWIE                                         (1 << 2U)
#define CANFD_SCFG_SWIF                                         (1 << 3U)
#define CANFD_SCFG_FSTIM_SHIFT                                  (4U)
#define CANFD_SCFG_FSTIM_MASK                                   (0x7 << CANFD_SCFG_FSTIM_SHIFT)

#endif /* __HARDWARE_CANFD_H__ */
