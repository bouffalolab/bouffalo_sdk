/**
  ******************************************************************************
  * @file    mjdec_v2_reg.h
  * @version V1.0
  * @date    2025-09-09
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
#ifndef  __MJDEC_REG_H__
#define  __MJDEC_REG_H__

/****************************************************************************
 * Pre-processor Definitions
****************************************************************************/

/* Register offsets *********************************************************/

#define MJDEC_CONFIG_OFFSET                                     (0x0)/* mjdec_config */
#define MJDEC_JPEG_STREAM_BASE_OFFSET                           (0x4)/* jpeg_stream_base */
#define MJDEC_OUT_Y_BASE_OFFSET                                 (0x8)/* out_y_base */
#define MJDEC_OUT_UV_BASE_OFFSET                                (0xC)/* out_uv_base */
#define MJDEC_INT_CFG_OFFSET                                    (0x10)/* mjdec_int_cfg */
#define MJDEC_STAT_OFFSET                                       (0x14)/* mjdec_stat */
#define MJDEC_PAUSE_ROW_OFFSET                                  (0x18)/* pause_row */
#define MJDEC_LOOP_BUFFER_OFFSET                                (0x1C)/* loop_buffer */
#define MJDEC_HDER_INFO_OFFSET                                  (0x20)/* hder_info */
#define MJDEC_HDER_IMAGE_SIZE_OFFSET                            (0x24)/* hder_image_size */
#define MJDEC_PARSE_STATE_OFFSET                                (0x28)/* parse_state */
#define MJDEC_SCAN_STATE_OFFSET                                 (0x2C)/* scan_state */
#define MJDEC_SCAN_BIT_BUFFER_OFFSET                            (0x30)/* scan_bit_buffer */
#define MJDEC_DHT_STATE_OFFSET                                  (0x34)/* dht_state */
#define MJDEC_OUTPUT_STATE_OFFSET                               (0x38)/* output_state */
#define MJDEC_OUT_BUFFS_STATE_OFFSET                            (0x3C)/* out_buffs_state */
#define MJDEC_VERSION_OFFSET                                    (0x7C)/* mjdec_version */

/* Register Bitfield definitions *****************************************************/

/* 0x0 : mjdec_config */
#define MJDEC_EN                                                (1<<0U)
#define MJDEC_START_DEC                                         (1<<1U)
#define MJDEC_SKIP_HDER                                         (1<<2U)
#define MJDEC_STREAM_NO_HDER                                    (1<<3U)
#define MJDEC_IN_BURST_TYPE_SHIFT                               (4U)
#define MJDEC_IN_BURST_TYPE_MASK                                (0x3<<MJDEC_IN_BURST_TYPE_SHIFT)
#define MJDEC_OUT_BURST_TYPE_SHIFT                              (6U)
#define MJDEC_OUT_BURST_TYPE_MASK                               (0x3<<MJDEC_OUT_BURST_TYPE_SHIFT)
#define MJDEC_PIXEL_FORMAT_SHIFT                                (8U)
#define MJDEC_PIXEL_FORMAT_MASK                                 (0x3<<MJDEC_PIXEL_FORMAT_SHIFT)
#define MJDEC_SEMI_PLAN_TYPE                                    (1<<10U)
#define MJDEC_YUV_INTLV_TYPE_SHIFT                              (12U)
#define MJDEC_YUV_INTLV_TYPE_MASK                               (0x3<<MJDEC_YUV_INTLV_TYPE_SHIFT)
#define MJDEC_RGB_TYPE_SHIFT                                    (16U)
#define MJDEC_RGB_TYPE_MASK                                     (0x7<<MJDEC_RGB_TYPE_SHIFT)
#define MJDEC_ALPHA_VALUE_SHIFT                                 (24U)
#define MJDEC_ALPHA_VALUE_MASK                                  (0xff<<MJDEC_ALPHA_VALUE_SHIFT)

/* 0x4 : jpeg_stream_base */
#define MJDEC_IMAGE_BASE_SHIFT                                  (3U)
#define MJDEC_IMAGE_BASE_MASK                                   (0x1fffffff<<MJDEC_IMAGE_BASE_SHIFT)

/* 0x8 : out_y_base */
#define MJDEC_OUT_Y_BASE_SHIFT                                  (3U)
#define MJDEC_OUT_Y_BASE_MASK                                   (0x1fffffff<<MJDEC_OUT_Y_BASE_SHIFT)

/* 0xC : out_uv_base */
#define MJDEC_OUT_UV_BASE_SHIFT                                 (3U)
#define MJDEC_OUT_UV_BASE_MASK                                  (0x1fffffff<<MJDEC_OUT_UV_BASE_SHIFT)

/* 0x10 : mjdec_int_cfg */
#define MJDEC_FIN_INTEN                                         (1<<0U)
#define MJDEC_ROW_INTEN                                         (1<<1U)
#define MJDEC_BUSERR_INTEN                                      (1<<2U)
#define MJDEC_PARSE_ERR_INTEN                                   (1<<3U)
#define MJDEC_DHT_FAIL_INTEN                                    (1<<4U)
#define MJDEC_INT_ROW_NUM_SHIFT                                 (16U)
#define MJDEC_INT_ROW_NUM_MASK                                  (0xfff<<MJDEC_INT_ROW_NUM_SHIFT)

/* 0x14 : mjdec_stat */
#define MJDEC_FIN_INT                                           (1<<0U)
#define MJDEC_ROW_INT                                           (1<<1U)
#define MJDEC_BUSERR_INT                                        (1<<2U)
#define MJDEC_PARSE_ERR_INT                                     (1<<3U)
#define MJDEC_DHT_FAIL_INT                                      (1<<4U)
#define MJDEC_BUSY                                              (1<<16U)
#define MJDEC_PAUSING                                           (1<<17U)

/* 0x18 : pause_row */
#define MJDEC_PAUSE_ROW_NUM_SHIFT                               (0U)
#define MJDEC_PAUSE_ROW_NUM_MASK                                (0xfff<<MJDEC_PAUSE_ROW_NUM_SHIFT)

/* 0x1C : loop_buffer */
#define MJDEC_OUT_Y_END_ROW_SHIFT                               (0U)
#define MJDEC_OUT_Y_END_ROW_MASK                                (0xfff<<MJDEC_OUT_Y_END_ROW_SHIFT)

/* 0x20 : hder_info */
#define MJDEC_IMAGE_Y_H_SAMP_SHIFT                              (0U)
#define MJDEC_IMAGE_Y_H_SAMP_MASK                               (0x3<<MJDEC_IMAGE_Y_H_SAMP_SHIFT)
#define MJDEC_IMAGE_Y_V_SAMP_SHIFT                              (2U)
#define MJDEC_IMAGE_Y_V_SAMP_MASK                               (0x3<<MJDEC_IMAGE_Y_V_SAMP_SHIFT)
#define MJDEC_IMAGE_CB_H_SAMP_SHIFT                             (4U)
#define MJDEC_IMAGE_CB_H_SAMP_MASK                              (0x3<<MJDEC_IMAGE_CB_H_SAMP_SHIFT)
#define MJDEC_IMAGE_CB_V_SAMP_SHIFT                             (6U)
#define MJDEC_IMAGE_CB_V_SAMP_MASK                              (0x3<<MJDEC_IMAGE_CB_V_SAMP_SHIFT)
#define MJDEC_IMAGE_CR_H_SAMP_SHIFT                             (8U)
#define MJDEC_IMAGE_CR_H_SAMP_MASK                              (0x3<<MJDEC_IMAGE_CR_H_SAMP_SHIFT)
#define MJDEC_IMAGE_CR_V_SAMP_SHIFT                             (10U)
#define MJDEC_IMAGE_CR_V_SAMP_MASK                              (0x3<<MJDEC_IMAGE_CR_V_SAMP_SHIFT)
#define MJDEC_HDER_LENGTH_SHIFT                                 (16U)
#define MJDEC_HDER_LENGTH_MASK                                  (0xffff<<MJDEC_HDER_LENGTH_SHIFT)

/* 0x24 : hder_image_size */
#define MJDEC_IMAGE_HEIGHT_SHIFT                                (0U)
#define MJDEC_IMAGE_HEIGHT_MASK                                 (0xfff<<MJDEC_IMAGE_HEIGHT_SHIFT)
#define MJDEC_IMAGE_WIDTH_SHIFT                                 (16U)
#define MJDEC_IMAGE_WIDTH_MASK                                  (0xfff<<MJDEC_IMAGE_WIDTH_SHIFT)

/* 0x28 : parse_state */
#define MJDEC_ST_PARSE_CUR_SHIFT                                (0U)
#define MJDEC_ST_PARSE_CUR_MASK                                 (0xf<<MJDEC_ST_PARSE_CUR_SHIFT)
#define MJDEC_ST_SCAN_CUR_SHIFT                                 (4U)
#define MJDEC_ST_SCAN_CUR_MASK                                  (0xf<<MJDEC_ST_SCAN_CUR_SHIFT)
#define MJDEC_ST_BUS_CUR_SHIFT                                  (8U)
#define MJDEC_ST_BUS_CUR_MASK                                   (0x3<<MJDEC_ST_BUS_CUR_SHIFT)
#define MJDEC_IBUF_NUM_SHIFT                                    (16U)
#define MJDEC_IBUF_NUM_MASK                                     (0x3f<<MJDEC_IBUF_NUM_SHIFT)

/* 0x2C : scan_state */
#define MJDEC_SCAN_MCU_CNT_SHIFT                                (0U)
#define MJDEC_SCAN_MCU_CNT_MASK                                 (0xffff<<MJDEC_SCAN_MCU_CNT_SHIFT)
#define MJDEC_SCAN_BIT_CNT_SHIFT                                (16U)
#define MJDEC_SCAN_BIT_CNT_MASK                                 (0x3f<<MJDEC_SCAN_BIT_CNT_SHIFT)

/* 0x30 : scan_bit_buffer */
#define MJDEC_SCAN_BIT_BUFFER_SHIFT                             (0U)
#define MJDEC_SCAN_BIT_BUFFER_MASK                              (0xffffffff<<MJDEC_SCAN_BIT_BUFFER_SHIFT)

/* 0x34 : dht_state */
#define MJDEC_DHT_LOOKUP_BITS_SHIFT                             (0U)
#define MJDEC_DHT_LOOKUP_BITS_MASK                              (0xffff<<MJDEC_DHT_LOOKUP_BITS_SHIFT)
#define MJDEC_DHT_LOOKUP_TABLE_ID_SHIFT                         (16U)
#define MJDEC_DHT_LOOKUP_TABLE_ID_MASK                          (0x3<<MJDEC_DHT_LOOKUP_TABLE_ID_SHIFT)
#define MJDEC_DHT_LOOKUP_SIZE_SHIFT                             (18U)
#define MJDEC_DHT_LOOKUP_SIZE_MASK                              (0x1f<<MJDEC_DHT_LOOKUP_SIZE_SHIFT)
#define MJDEC_DHT_LOOKUP_SYMBOL_SHIFT                           (24U)
#define MJDEC_DHT_LOOKUP_SYMBOL_MASK                            (0xff<<MJDEC_DHT_LOOKUP_SYMBOL_SHIFT)

/* 0x38 : output_state */
#define MJDEC_ST_OUT_CUR_SHIFT                                  (0U)
#define MJDEC_ST_OUT_CUR_MASK                                   (0xf<<MJDEC_ST_OUT_CUR_SHIFT)
#define MJDEC_IMAGE_HMCU_CNT_SHIFT                              (4U)
#define MJDEC_IMAGE_HMCU_CNT_MASK                               (0x1ff<<MJDEC_IMAGE_HMCU_CNT_SHIFT)
#define MJDEC_IMAGE_VMCU_CNT_SHIFT                              (16U)
#define MJDEC_IMAGE_VMCU_CNT_MASK                               (0x1ff<<MJDEC_IMAGE_VMCU_CNT_SHIFT)

/* 0x3C : out_buffs_state */
#define MJDEC_OBUFF_BLK_NUM_SHIFT                               (0U)
#define MJDEC_OBUFF_BLK_NUM_MASK                                (0x3f<<MJDEC_OBUFF_BLK_NUM_SHIFT)
#define MJDEC_CBUFF_FULL                                        (1<<6U)
#define MJDEC_CBUFF_EMPT                                        (1<<7U)
#define MJDEC_OUTSTANDING_CNT_SHIFT                             (8U)
#define MJDEC_OUTSTANDING_CNT_MASK                              (0x7<<MJDEC_OUTSTANDING_CNT_SHIFT)
#define MJDEC_DBUFF_FULL                                        (1<<12U)
#define MJDEC_DBUFF_EMPT                                        (1<<13U)

/* 0x7C : mjdec_version */
#define MJDEC_RESV_FOR_SOFTWARE_SHIFT                           (0U)
#define MJDEC_RESV_FOR_SOFTWARE_MASK                            (0xffffff<<MJDEC_RESV_FOR_SOFTWARE_SHIFT)
#define MJDEC_VERSION_ID_SHIFT                                  (24U)
#define MJDEC_VERSION_ID_MASK                                   (0xff<<MJDEC_VERSION_ID_SHIFT)


#endif  /* __MJDEC_REG_H__ */
