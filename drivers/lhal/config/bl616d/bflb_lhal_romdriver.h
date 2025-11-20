#ifndef __BFLB_LHAL_ROMDRIVER_H__
#define __BFLB_LHAL_ROMDRIVER_H__

#define BFLB_LHAL_ROMAPI_INDEX_MAX   (512)

typedef enum {
    BFLB_LHAL_ROMAPI_INDEX_VERSION      = 0,
    BFLB_LHAL_ROMAPI_INDEX_RSVD_0       = 1,
    BFLB_LHAL_ROMAPI_INDEX_RSVD_1       = 2,
    BFLB_LHAL_ROMAPI_INDEX_RSVD_LAST    = 3,

    BFLB_LHAL_ROMAPI_INDEX_bflb_cam_init                                     = 4,
    BFLB_LHAL_ROMAPI_INDEX_bflb_cam_start                                    = 5,
    BFLB_LHAL_ROMAPI_INDEX_bflb_cam_stop                                     = 6,
    BFLB_LHAL_ROMAPI_INDEX_bflb_cam_int_mask                                 = 7,
    BFLB_LHAL_ROMAPI_INDEX_bflb_cam_int_clear                                = 8,
    BFLB_LHAL_ROMAPI_INDEX_bflb_cam_crop_vsync                               = 9,
    BFLB_LHAL_ROMAPI_INDEX_bflb_cam_crop_hsync                               = 10,
    BFLB_LHAL_ROMAPI_INDEX_bflb_cam_pop_one_frame                            = 11,
    BFLB_LHAL_ROMAPI_INDEX_bflb_cam_get_frame_count                          = 12,
    BFLB_LHAL_ROMAPI_INDEX_bflb_cam_get_frame_info                           = 13,
    BFLB_LHAL_ROMAPI_INDEX_bflb_cam_get_intstatus                            = 14,
    BFLB_LHAL_ROMAPI_INDEX_bflb_cam_feature_control                          = 15,
    BFLB_LHAL_ROMAPI_INDEX_bflb_cks_reset                                    = 16,
    BFLB_LHAL_ROMAPI_INDEX_bflb_cks_set_endian                               = 17,
    BFLB_LHAL_ROMAPI_INDEX_bflb_cks_compute                                  = 18,
    BFLB_LHAL_ROMAPI_INDEX_bflb_peripheral_clock_get                         = 19,
    BFLB_LHAL_ROMAPI_INDEX_bflb_peripheral_clock_control                     = 20,
    BFLB_LHAL_ROMAPI_INDEX_bflb_peripheral_clock_status_get                  = 21,
    BFLB_LHAL_ROMAPI_INDEX_arch_strcmp                                       = 22,
    BFLB_LHAL_ROMAPI_INDEX_arch_memcpy                                       = 23,
    BFLB_LHAL_ROMAPI_INDEX_arch_memcpy4                                      = 24,
    BFLB_LHAL_ROMAPI_INDEX_arch_memcpy_fast                                  = 25,
    BFLB_LHAL_ROMAPI_INDEX_arch_memset                                       = 26,
    BFLB_LHAL_ROMAPI_INDEX_arch_memset4                                      = 27,
    BFLB_LHAL_ROMAPI_INDEX_arch_memcmp                                       = 28,
    BFLB_LHAL_ROMAPI_INDEX_bflb_check_cache_addr_aligned                     = 29,
    BFLB_LHAL_ROMAPI_INDEX_bflb_soft_crc16                                   = 30,
    BFLB_LHAL_ROMAPI_INDEX_bflb_soft_crc32_ex                                = 31,
    BFLB_LHAL_ROMAPI_INDEX_bflb_soft_crc32                                   = 32,
    BFLB_LHAL_ROMAPI_INDEX_bflb_get_app_version_from_efuse                   = 33,
    BFLB_LHAL_ROMAPI_INDEX_bflb_set_app_version_to_efuse                     = 34,
    BFLB_LHAL_ROMAPI_INDEX_bflb_get_boot2_version_from_efuse                 = 35,
    BFLB_LHAL_ROMAPI_INDEX_bflb_set_boot2_version_to_efuse                   = 36,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dac_init                                     = 37,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dac_channel_enable                           = 38,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dac_channel_disable                          = 39,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dac_link_txdma                               = 40,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dac_set_dma_format                           = 41,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dac_set_value                                = 42,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_init                                     = 43,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_deinit                                   = 44,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_qspi_set_addr                            = 45,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_send_cmd_data                            = 46,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_send_cmd_read_data                       = 47,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_send_cmd_pixel                           = 48,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_link_txdma                               = 49,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_txint_mask                               = 50,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_tcint_mask                               = 51,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_errint_mask                              = 52,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_get_intstatus                            = 53,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_int_clear                                = 54,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_feature_control                          = 55,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_init                             = 56,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_deinit                           = 57,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_lli_reload                       = 58,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_lli_link_head                    = 59,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_start                            = 60,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_stop                             = 61,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_isbusy                           = 62,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_tcint_mask                       = 63,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_get_tcint_status                 = 64,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_tcint_clear                      = 65,
    BFLB_LHAL_ROMAPI_INDEX_bflb_rx_cycle_dma_process                         = 66,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dma_feature_control                          = 67,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_busy                                 = 68,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_autoload_done                        = 69,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_set_para                             = 70,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_write_direct                         = 71,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_read_direct                          = 72,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_read_common_trim                     = 73,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_write_common_trim                    = 74,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_is_all_bits_zero                     = 75,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_get_byte_zero_cnt                    = 76,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_get_trim_parity                      = 77,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_init                                    = 78,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_deinit                                  = 79,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_output_value_mode_enable                = 80,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin0_31_output                          = 81,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin32_63_output                         = 82,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_set                                     = 83,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin0_31_set                             = 84,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin32_63_set                            = 85,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_reset                                   = 86,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin0_31_reset                           = 87,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin32_63_reset                          = 88,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_read                                    = 89,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin0_31_read                            = 90,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin32_63_read                           = 91,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_int_init                                = 92,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_int_mask                                = 93,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_get_intstatus                           = 94,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_get_intstatus_pin0_31                   = 95,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_get_intstatus_pin32_63                  = 96,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_int_clear                               = 97,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_int_clear_pin0_31                       = 98,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_int_clear_pin32_63                      = 99,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_uart_init                               = 100,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_feature_control                         = 101,
    BFLB_LHAL_ROMAPI_INDEX_bflb_i2c_init                                     = 102,
    BFLB_LHAL_ROMAPI_INDEX_bflb_i2c_deinit                                   = 103,
    BFLB_LHAL_ROMAPI_INDEX_bflb_i2c_link_txdma                               = 104,
    BFLB_LHAL_ROMAPI_INDEX_bflb_i2c_link_rxdma                               = 105,
    BFLB_LHAL_ROMAPI_INDEX_bflb_i2c_transfer                                 = 106,
    BFLB_LHAL_ROMAPI_INDEX_bflb_i2c_int_mask                                 = 107,
    BFLB_LHAL_ROMAPI_INDEX_bflb_i2c_int_clear                                = 108,
    BFLB_LHAL_ROMAPI_INDEX_bflb_i2c_get_intstatus                            = 109,
    BFLB_LHAL_ROMAPI_INDEX_bflb_i2c_feature_control                          = 110,
    BFLB_LHAL_ROMAPI_INDEX_bflb_i2s_init                                     = 111,
    BFLB_LHAL_ROMAPI_INDEX_bflb_i2s_deinit                                   = 112,
    BFLB_LHAL_ROMAPI_INDEX_bflb_i2s_link_txdma                               = 113,
    BFLB_LHAL_ROMAPI_INDEX_bflb_i2s_link_rxdma                               = 114,
    BFLB_LHAL_ROMAPI_INDEX_bflb_i2s_txint_mask                               = 115,
    BFLB_LHAL_ROMAPI_INDEX_bflb_i2s_rxint_mask                               = 116,
    BFLB_LHAL_ROMAPI_INDEX_bflb_i2s_errint_mask                              = 117,
    BFLB_LHAL_ROMAPI_INDEX_bflb_i2s_get_intstatus                            = 118,
    BFLB_LHAL_ROMAPI_INDEX_bflb_i2s_feature_control                          = 119,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ir_tx_init                                   = 120,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ir_send                                      = 121,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ir_swm_send                                  = 122,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ir_tx_enable                                 = 123,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ir_txint_mask                                = 124,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ir_txint_clear                               = 125,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ir_get_txint_status                          = 126,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ir_link_txdma                                = 127,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ir_get_txfifo_cnt                            = 128,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ir_txfifo_clear                              = 129,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ir_rx_init                                   = 130,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ir_receive                                   = 131,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ir_swm_receive                               = 132,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ir_rx_enable                                 = 133,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ir_rxint_mask                                = 134,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ir_rxint_clear                               = 135,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ir_get_rxint_status                          = 136,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ir_get_rxfifo_cnt                            = 137,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ir_rxfifo_clear                              = 138,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ir_feature_control                           = 139,
    BFLB_LHAL_ROMAPI_INDEX_bflb_irq_restore                                  = 140,
    BFLB_LHAL_ROMAPI_INDEX_bflb_irq_enable                                   = 141,
    BFLB_LHAL_ROMAPI_INDEX_bflb_irq_disable                                  = 142,
    BFLB_LHAL_ROMAPI_INDEX_bflb_irq_set_pending                              = 143,
    BFLB_LHAL_ROMAPI_INDEX_bflb_irq_clear_pending                            = 144,
    BFLB_LHAL_ROMAPI_INDEX_bflb_irq_set_nlbits                               = 145,
    BFLB_LHAL_ROMAPI_INDEX_bflb_irq_set_priority                             = 146,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_icache_enable                            = 147,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_icache_disable                           = 148,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_icache_invalid_all                       = 149,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_icache_invalid_range                     = 150,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_enable                            = 151,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_disable                           = 152,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_clean_all                         = 153,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_invalidate_all                    = 154,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_clean_invalidate_all              = 155,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_clean_range                       = 156,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_invalidate_range                  = 157,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_clean_invalidate_range            = 158,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_set_dqt_from_zigzag                    = 159,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_set_dqt_from_header                    = 160,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_set_dqt_from_quality                   = 161,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_set_dht_from_spec                      = 162,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_set_dht_from_header                    = 163,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_init                                   = 164,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_start                                  = 165,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_stop                                   = 166,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_push_jpeg                              = 167,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_tcint_mask                             = 168,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_get_intstatus                          = 169,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_int_clear                              = 170,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_get_frame_count                        = 171,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_pop_one_frame                          = 172,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_feature_control                        = 173,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_init                                   = 174,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_start                                  = 175,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_stop                                   = 176,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_sw_run                                 = 177,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_kick_run                               = 178,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_kick_stop                              = 179,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_kick                                   = 180,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_tcint_mask                             = 181,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_swapint_mask                           = 182,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_errint_mask                            = 183,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_get_intstatus                          = 184,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_int_clear                              = 185,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_get_frame_count                        = 186,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_pop_one_frame                          = 187,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_pop_swap_block                         = 188,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_get_swap_bit_count                     = 189,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_get_frame_info                         = 190,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_get_swap_block_info                    = 191,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_swap_is_block_full                     = 192,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_calculate_quantize_table               = 193,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_fill_quantize_table                    = 194,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_fill_jpeg_header_tail                  = 195,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_set_yuv420sp_cam_input                 = 196,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_update_input_output_buff               = 197,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_feature_control                        = 198,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mtimer_delay_us                              = 199,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mtimer_delay_ms                              = 200,
    BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_init                                  = 201,
    BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_deinit                                = 202,
    BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_set_period                            = 203,
    BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_start                                 = 204,
    BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_stop                                  = 205,
    BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_get_frequency                         = 206,
    BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_channel_init                          = 207,
    BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_channel_set_threshold                 = 208,
    BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_channel_positive_start                = 209,
    BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_channel_negative_start                = 210,
    BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_channel_positive_stop                 = 211,
    BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_channel_negative_stop                 = 212,
    BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_int_enable                            = 213,
    BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_int_clear                             = 214,
    BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_get_intstatus                         = 215,
    BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_feature_control                       = 216,
    BFLB_LHAL_ROMAPI_INDEX_bflb_rtc_disable                                  = 217,
    BFLB_LHAL_ROMAPI_INDEX_bflb_rtc_set_time                                 = 218,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_init                                     = 219,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_adma2_desc_init                          = 220,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_cmd_cfg                                  = 221,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_data_cfg                                 = 222,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_tranfer_start                            = 223,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_get_resp                                 = 224,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_sta_en                                   = 225,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_sta_en_get                               = 226,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_sta_int_en                               = 227,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_sta_int_en_get                           = 228,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_sta_get                                  = 229,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_sta_clr                                  = 230,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_feature_control                          = 231,
    BFLB_LHAL_ROMAPI_INDEX_bflb_aes_init                                     = 232,
    BFLB_LHAL_ROMAPI_INDEX_bflb_aes_deinit                                   = 233,
    BFLB_LHAL_ROMAPI_INDEX_bflb_aes_select_hwkey                             = 234,
    BFLB_LHAL_ROMAPI_INDEX_bflb_aes_set_mode                                 = 235,
    BFLB_LHAL_ROMAPI_INDEX_bflb_aes_setkey                                   = 236,
    BFLB_LHAL_ROMAPI_INDEX_bflb_aes_encrypt                                  = 237,
    BFLB_LHAL_ROMAPI_INDEX_bflb_aes_decrypt                                  = 238,
    BFLB_LHAL_ROMAPI_INDEX_bflb_aes_link_init                                = 239,
    BFLB_LHAL_ROMAPI_INDEX_bflb_aes_link_deinit                              = 240,
    BFLB_LHAL_ROMAPI_INDEX_bflb_aes_link_update                              = 241,
    BFLB_LHAL_ROMAPI_INDEX_bflb_group0_request_aes_access                    = 242,
    BFLB_LHAL_ROMAPI_INDEX_bflb_group1_request_aes_access                    = 243,
    BFLB_LHAL_ROMAPI_INDEX_bflb_group0_release_aes_access                    = 244,
    BFLB_LHAL_ROMAPI_INDEX_bflb_group1_release_aes_access                    = 245,
    BFLB_LHAL_ROMAPI_INDEX_bflb_aes_set_hwkey_source                         = 246,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha_init                                     = 247,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha1_start                                   = 248,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha256_start                                 = 249,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha512_start                                 = 250,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha1_update                                  = 251,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha1_once_padded                             = 252,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha256_update                                = 253,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha512_update                                = 254,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha1_finish                                  = 255,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha256_finish                                = 256,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha512_finish                                = 257,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha_link_init                                = 258,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha_link_deinit                              = 259,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha1_link_start                              = 260,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha256_link_start                            = 261,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha512_link_start                            = 262,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha1_link_update                             = 263,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha256_link_update                           = 264,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha512_link_update                           = 265,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha1_link_finish                             = 266,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha256_link_finish                           = 267,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha512_link_finish                           = 268,
    BFLB_LHAL_ROMAPI_INDEX_bflb_group0_request_sha_access                    = 269,
    BFLB_LHAL_ROMAPI_INDEX_bflb_group1_request_sha_access                    = 270,
    BFLB_LHAL_ROMAPI_INDEX_bflb_group0_release_sha_access                    = 271,
    BFLB_LHAL_ROMAPI_INDEX_bflb_group1_release_sha_access                    = 272,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sha_crc32_link_work                          = 273,
    BFLB_LHAL_ROMAPI_INDEX_bflb_trng_read                                    = 274,
    BFLB_LHAL_ROMAPI_INDEX_bflb_trng_readlen                                 = 275,
    BFLB_LHAL_ROMAPI_INDEX_bflb_group0_request_trng_access                   = 276,
    BFLB_LHAL_ROMAPI_INDEX_bflb_group1_request_trng_access                   = 277,
    BFLB_LHAL_ROMAPI_INDEX_bflb_group0_release_trng_access                   = 278,
    BFLB_LHAL_ROMAPI_INDEX_bflb_group1_release_trng_access                   = 279,
    BFLB_LHAL_ROMAPI_INDEX_bflb_spi_init                                     = 280,
    BFLB_LHAL_ROMAPI_INDEX_bflb_spi_deinit                                   = 281,
    BFLB_LHAL_ROMAPI_INDEX_bflb_spi_link_txdma                               = 282,
    BFLB_LHAL_ROMAPI_INDEX_bflb_spi_link_rxdma                               = 283,
    BFLB_LHAL_ROMAPI_INDEX_bflb_spi_poll_send                                = 284,
    BFLB_LHAL_ROMAPI_INDEX_bflb_spi_poll_exchange                            = 285,
    BFLB_LHAL_ROMAPI_INDEX_bflb_spi_txint_mask                               = 286,
    BFLB_LHAL_ROMAPI_INDEX_bflb_spi_rxint_mask                               = 287,
    BFLB_LHAL_ROMAPI_INDEX_bflb_spi_tcint_mask                               = 288,
    BFLB_LHAL_ROMAPI_INDEX_bflb_spi_rtoint_mask                              = 289,
    BFLB_LHAL_ROMAPI_INDEX_bflb_spi_errint_mask                              = 290,
    BFLB_LHAL_ROMAPI_INDEX_bflb_spi_get_intstatus                            = 291,
    BFLB_LHAL_ROMAPI_INDEX_bflb_spi_int_clear                                = 292,
    BFLB_LHAL_ROMAPI_INDEX_bflb_spi_isbusy                                   = 293,
    BFLB_LHAL_ROMAPI_INDEX_bflb_spi_feature_control                          = 294,
    BFLB_LHAL_ROMAPI_INDEX_bflb_timer_init                                   = 295,
    BFLB_LHAL_ROMAPI_INDEX_bflb_timer_deinit                                 = 296,
    BFLB_LHAL_ROMAPI_INDEX_bflb_timer_start                                  = 297,
    BFLB_LHAL_ROMAPI_INDEX_bflb_timer_stop                                   = 298,
    BFLB_LHAL_ROMAPI_INDEX_bflb_timer_set_preloadvalue                       = 299,
    BFLB_LHAL_ROMAPI_INDEX_bflb_timer_set_compvalue                          = 300,
    BFLB_LHAL_ROMAPI_INDEX_bflb_timer_get_compvalue                          = 301,
    BFLB_LHAL_ROMAPI_INDEX_bflb_timer_get_countervalue                       = 302,
    BFLB_LHAL_ROMAPI_INDEX_bflb_timer_compint_mask                           = 303,
    BFLB_LHAL_ROMAPI_INDEX_bflb_timer_get_compint_status                     = 304,
    BFLB_LHAL_ROMAPI_INDEX_bflb_timer_compint_clear                          = 305,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_init                                    = 306,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_deinit                                  = 307,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_enable                                  = 308,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_disable                                 = 309,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_link_txdma                              = 310,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_link_rxdma                              = 311,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_putchar                                 = 312,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_getchar                                 = 313,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_put                                     = 314,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_put_block                               = 315,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_get                                     = 316,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_wait_tx_done                            = 317,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_txready                                 = 318,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_txempty                                 = 319,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_rxavailable                             = 320,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_txint_mask                              = 321,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_rxint_mask                              = 322,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_errint_mask                             = 323,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_get_intstatus                           = 324,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_int_clear                               = 325,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_feature_control                         = 326,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wdg_init                                     = 327,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wdg_start                                    = 328,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wdg_stop                                     = 329,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wdg_get_countervalue                         = 330,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wdg_set_countervalue                         = 331,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wdg_reset_countervalue                       = 332,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wdg_compint_clear                            = 333,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wo_pin_init                                  = 334,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wo_init                                      = 335,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wo_enable                                    = 336,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wo_disable                                   = 337,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wo_get_fifo_available_cnt                    = 338,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wo_push_fifo                                 = 339,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wo_clear_fifo                                = 340,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wo_enable_dma                                = 341,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wo_disable_dma                               = 342,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wo_get_int_status                            = 343,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wo_int_mask                                  = 344,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wo_int_unmask                                = 345,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wo_int_clear                                 = 346,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_callapi_before                  = 347,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_callapi_after                   = 348,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_get_param                       = 349,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_region_foreach                  = 350,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_get_lockbits                    = 351,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_set_lockbits                    = 352,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_get_locked                      = 353,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_set_locked                      = 354,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_get_info_by_idx                 = 355,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_valid_by_idx                    = 356,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_read_by_idx                     = 357,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_write_by_idx                    = 358,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_erase_by_idx                    = 359,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_get_info                        = 360,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_valid                           = 361,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_read                            = 362,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_write                           = 363,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_erase                           = 364,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_init_ext_flash_gpio                   = 365,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_deinit_ext_flash_gpio                 = 366,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_init_flash_gpio                       = 367,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_init_flash2_gpio                      = 368,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_flash_identify                        = 369,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_get_flash_cfg_need_lock               = 370,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_flash_init                            = 371,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_sbus2_flash_init                      = 372,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_enable                               = 373,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_io_delay                         = 374,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_io_delay                         = 375,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_cs_clk_delay                     = 376,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_cs_clk_delay                     = 377,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_flash_io_cs_clk_delay            = 378,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_flash_io_cs_clk_delay            = 379,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_dqs_delay                        = 380,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_bank2_enable                         = 381,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_sbus2_hold_sram                      = 382,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_sbus2_release_sram                   = 383,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_sbus2_replace                        = 384,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_sbus2_revoke_replace                 = 385,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_sbus2_set_delay                      = 386,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_remap_set                            = 387,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_32bits_addr_en                       = 388,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_clock_delay                      = 389,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_clock_delay                      = 390,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_wrap_queue_value                 = 391,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_cmds_set                             = 392,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_select_pad                           = 393,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_sbus_select_bank                     = 394,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_owner                            = 395,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_owner                            = 396,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_owner_flag                       = 397,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_owner_flag                       = 398,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_disable                              = 399,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_enable_be                        = 400,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_enable_le                        = 401,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_region                       = 402,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_key                          = 403,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_key_be                       = 404,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_iv                           = 405,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_iv_be                        = 406,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_region_offset                = 407,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_xts_set_key                      = 408,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_xts_set_key_be                   = 409,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_xts_set_iv                       = 410,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_xts_set_iv_be                    = 411,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_mode                         = 412,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_enable                           = 413,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_disable                          = 414,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_is_aes_enable                        = 415,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_flash_image_offset               = 416,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_flash_image_offset               = 417,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_lock_flash_image_offset              = 418,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_select_clock                         = 419,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_sendcmd                              = 420,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_disable_wrap_access                  = 421,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_xip_set                              = 422,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_busy_state                       = 423,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_get_iv_be                        = 424,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_get_iv_le                        = 425,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_decrypt_region_be            = 426,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_decrypt_region_le            = 427,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_init                                  = 428,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_set_spi_mode                          = 429,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_read_reg                              = 430,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_write_reg                             = 431,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_read_reg_with_cmd                     = 432,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_write_reg_with_cmd                    = 433,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_busy                                  = 434,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_write_enable                          = 435,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_qspi_enable                           = 436,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_qspi_disable                          = 437,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_volatile_reg_write_enable             = 438,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_chip_erase                            = 439,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_sector_erase                          = 440,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_blk32_erase                           = 441,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_blk64_erase                           = 442,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_erase                                 = 443,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_get_uniqueid                          = 444,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_get_jedecid                           = 445,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_get_deviceid                          = 446,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_powerdown                             = 447,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_release_powerdown                     = 448,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_restore_from_powerdown                = 449,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_set_burst_wrap                        = 450,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_disable_burst_wrap                    = 451,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_set_32bits_addr_mode                  = 452,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_software_reset                        = 453,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_reset_continue_read                   = 454,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_set_xip_cfg                           = 455,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_xip_read_enable                       = 456,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_xip_read_disable                      = 457,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_rcv_enable                            = 458,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_erase_security_register               = 459,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_program_security_register             = 460,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_read_security_register                = 461,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_clear_status_register                 = 462,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_read                                  = 463,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_program                               = 464,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_state_save                        = 465,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_state_restore                     = 466,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_erase_need_lock                   = 467,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_write_need_lock                   = 468,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_read_need_lock                    = 469,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_get_jedecid_need_lock             = 470,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_get_deviceid_need_lock            = 471,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_get_uniqueid_need_lock            = 472,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_clear_status_register_need_lock   = 473,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_read_via_cache_need_lock          = 474,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_opt_enter                         = 475,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_opt_exit                          = 476,
    BFLB_LHAL_ROMAPI_INDEX_FUNC_EMPTY_START,

    BFLB_LHAL_ROMAPI_INDEX_FUNC_EMPTY_END    = (BFLB_LHAL_ROMAPI_INDEX_MAX - 1)

} BFLB_LHAL_ROMAPI_INDEX_e;

#ifndef BFLB_LHAL_ROMAPI_TABLE
#define BFLB_LHAL_ROMAPI_TABLE      (0x60f82800)
#endif
#define ROM_APITABLE                ((uint32_t *)BFLB_LHAL_ROMAPI_TABLE)


#define romapi_bflb_cam_init \
    ((void (*)(struct bflb_device_s *dev, const struct bflb_cam_config_s *config)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_cam_init])

#define romapi_bflb_cam_start \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_cam_start])

#define romapi_bflb_cam_stop \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_cam_stop])

#define romapi_bflb_cam_int_mask \
    ((void (*)(struct bflb_device_s *dev, uint32_t int_type, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_cam_int_mask])

#define romapi_bflb_cam_int_clear \
    ((void (*)(struct bflb_device_s *dev, uint32_t int_type)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_cam_int_clear])

#define romapi_bflb_cam_crop_vsync \
    ((void (*)(struct bflb_device_s *dev, uint16_t start_line, uint16_t end_line)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_cam_crop_vsync])

#define romapi_bflb_cam_crop_hsync \
    ((void (*)(struct bflb_device_s *dev, uint16_t start_pixel, uint16_t end_pixel)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_cam_crop_hsync])

#define romapi_bflb_cam_pop_one_frame \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_cam_pop_one_frame])

#define romapi_bflb_cam_get_frame_count \
    ((uint8_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_cam_get_frame_count])

#define romapi_bflb_cam_get_frame_info \
    ((uint32_t (*)(struct bflb_device_s *dev, uint8_t **pic)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_cam_get_frame_info])

#define romapi_bflb_cam_get_intstatus \
    ((uint32_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_cam_get_intstatus])

#define romapi_bflb_cam_feature_control \
    ((int (*)(struct bflb_device_s *dev, int cmd, size_t arg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_cam_feature_control])

#define romapi_bflb_cks_reset \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_cks_reset])

#define romapi_bflb_cks_set_endian \
    ((void (*)(struct bflb_device_s *dev, uint8_t endian)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_cks_set_endian])

#define romapi_bflb_cks_compute \
    ((uint16_t (*)(struct bflb_device_s *dev, uint8_t *data, uint32_t length)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_cks_compute])

#define romapi_bflb_peripheral_clock_get \
    ((uint32_t (*)(uint8_t peri)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_peripheral_clock_get])

#define romapi_bflb_peripheral_clock_control \
    ((int (*)(uint8_t peri, bool enable)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_peripheral_clock_control])

#define romapi_bflb_peripheral_clock_status_get \
    ((int (*)(uint8_t peri)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_peripheral_clock_status_get])

#define romapi_arch_strcmp \
    ((int (*)(const char *str1, const char *str2)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_arch_strcmp])

#define romapi_arch_memcpy \
    ((void * (*)(void *dst, const void *src, uint32_t n)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_arch_memcpy])

#define romapi_arch_memcpy4 \
    ((uint32_t * (*)(uint32_t *dst, const uint32_t *src, uint32_t n)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_arch_memcpy4])

#define romapi_arch_memcpy_fast \
    ((void * (*)(void *pdst, const void *psrc, uint32_t n)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_arch_memcpy_fast])

#define romapi_arch_memset \
    ((void * (*)(void *s, uint8_t c, uint32_t n)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_arch_memset])

#define romapi_arch_memset4 \
    ((uint32_t * (*)(uint32_t *dst, const uint32_t val, uint32_t n)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_arch_memset4])

#define romapi_arch_memcmp \
    ((int (*)(const void *s1, const void *s2, uint32_t n)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_arch_memcmp])

#define romapi_bflb_check_cache_addr_aligned \
    ((bool (*)(uintptr_t addr)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_check_cache_addr_aligned])

#define romapi_bflb_soft_crc16 \
    ((uint16_t (*)(void *in, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_soft_crc16])

#define romapi_bflb_soft_crc32_ex \
    ((uint32_t (*)(uint32_t initial, void *in, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_soft_crc32_ex])

#define romapi_bflb_soft_crc32 \
    ((uint32_t (*)(void *in, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_soft_crc32])

#define romapi_bflb_get_app_version_from_efuse \
    ((int32_t (*)(uint8_t *version)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_get_app_version_from_efuse])

#define romapi_bflb_set_app_version_to_efuse \
    ((int32_t (*)(uint8_t version)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_set_app_version_to_efuse])

#define romapi_bflb_get_boot2_version_from_efuse \
    ((int32_t (*)(uint8_t *version)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_get_boot2_version_from_efuse])

#define romapi_bflb_set_boot2_version_to_efuse \
    ((int32_t (*)(uint8_t version)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_set_boot2_version_to_efuse])

#define romapi_bflb_dac_init \
    ((void (*)(struct bflb_device_s *dev, uint8_t clk_div)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dac_init])

#define romapi_bflb_dac_channel_enable \
    ((void (*)(struct bflb_device_s *dev, uint8_t ch)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dac_channel_enable])

#define romapi_bflb_dac_channel_disable \
    ((void (*)(struct bflb_device_s *dev, uint8_t ch)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dac_channel_disable])

#define romapi_bflb_dac_link_txdma \
    ((void (*)(struct bflb_device_s *dev, bool enable)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dac_link_txdma])

#define romapi_bflb_dac_set_dma_format \
    ((void (*)(struct bflb_device_s *dev, uint8_t format)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dac_set_dma_format])

#define romapi_bflb_dac_set_value \
    ((void (*)(struct bflb_device_s *dev, uint8_t ch, uint16_t value)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dac_set_value])

#define romapi_bflb_dbi_init \
    ((void (*)(struct bflb_device_s *dev, const struct bflb_dbi_config_s *config)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_init])

#define romapi_bflb_dbi_deinit \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_deinit])

#define romapi_bflb_dbi_qspi_set_addr \
    ((void (*)(struct bflb_device_s *dev, uint8_t addr_byte_size, uint32_t addr_val)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_qspi_set_addr])

#define romapi_bflb_dbi_send_cmd_data \
    ((int (*)(struct bflb_device_s *dev, uint8_t cmd, uint8_t data_len, uint8_t *data_buff)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_send_cmd_data])

#define romapi_bflb_dbi_send_cmd_read_data \
    ((int (*)(struct bflb_device_s *dev, uint8_t cmd, uint8_t data_len, uint8_t *data_buff)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_send_cmd_read_data])

#define romapi_bflb_dbi_send_cmd_pixel \
    ((int (*)(struct bflb_device_s *dev, uint8_t cmd, uint32_t pixel_cnt, void *pixel_buff)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_send_cmd_pixel])

#define romapi_bflb_dbi_link_txdma \
    ((void (*)(struct bflb_device_s *dev, bool enable)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_link_txdma])

#define romapi_bflb_dbi_txint_mask \
    ((void (*)(struct bflb_device_s *dev, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_txint_mask])

#define romapi_bflb_dbi_tcint_mask \
    ((void (*)(struct bflb_device_s *dev, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_tcint_mask])

#define romapi_bflb_dbi_errint_mask \
    ((void (*)(struct bflb_device_s *dev, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_errint_mask])

#define romapi_bflb_dbi_get_intstatus \
    ((uint32_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_get_intstatus])

#define romapi_bflb_dbi_int_clear \
    ((void (*)(struct bflb_device_s *dev, uint32_t int_clear)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_int_clear])

#define romapi_bflb_dbi_feature_control \
    ((int (*)(struct bflb_device_s *dev, int cmd, size_t arg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dbi_feature_control])

#define romapi_bflb_dma_channel_init \
    ((void (*)(struct bflb_device_s *dev, const struct bflb_dma_channel_config_s *config)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_init])

#define romapi_bflb_dma_channel_deinit \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_deinit])

#define romapi_bflb_dma_channel_lli_reload \
    ((int (*)(struct bflb_device_s *dev, struct bflb_dma_channel_lli_pool_s *lli_pool, uint32_t max_lli_count, struct bflb_dma_channel_lli_transfer_s *transfer, uint32_t count)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_lli_reload])

#define romapi_bflb_dma_channel_lli_link_head \
    ((void (*)(struct bflb_device_s *dev, struct bflb_dma_channel_lli_pool_s *lli_pool, uint32_t used_lli_count)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_lli_link_head])

#define romapi_bflb_dma_channel_start \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_start])

#define romapi_bflb_dma_channel_stop \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_stop])

#define romapi_bflb_dma_channel_isbusy \
    ((bool (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_isbusy])

#define romapi_bflb_dma_channel_tcint_mask \
    ((void (*)(struct bflb_device_s *dev, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_tcint_mask])

#define romapi_bflb_dma_channel_get_tcint_status \
    ((bool (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_get_tcint_status])

#define romapi_bflb_dma_channel_tcint_clear \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_tcint_clear])

#define romapi_bflb_rx_cycle_dma_process \
    ((void (*)(struct bflb_rx_cycle_dma *rx_dma, bool in_dma_isr)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_rx_cycle_dma_process])

#define romapi_bflb_dma_feature_control \
    ((int (*)(struct bflb_device_s *dev, int cmd, size_t arg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_dma_feature_control])

#define romapi_bflb_ef_ctrl_busy \
    ((int (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_busy])

#define romapi_bflb_ef_ctrl_autoload_done \
    ((int (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_autoload_done])

#define romapi_bflb_ef_ctrl_set_para \
    ((int (*)(bflb_ef_ctrl_para_t *para)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_set_para])

#define romapi_bflb_ef_ctrl_write_direct \
    ((void (*)(struct bflb_device_s *dev, uint32_t offset, uint32_t *pword, uint32_t count, uint8_t program)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_write_direct])

#define romapi_bflb_ef_ctrl_read_direct \
    ((void (*)(struct bflb_device_s *dev, uint32_t offset, uint32_t *pword, uint32_t count, uint8_t reload)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_read_direct])

#define romapi_bflb_ef_ctrl_read_common_trim \
    ((void (*)(struct bflb_device_s *dev, char *name, bflb_ef_ctrl_com_trim_t *trim, uint8_t reload)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_read_common_trim])

#define romapi_bflb_ef_ctrl_write_common_trim \
    ((void (*)(struct bflb_device_s *dev, char *name, uint32_t value, uint8_t program)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_write_common_trim])

#define romapi_bflb_ef_ctrl_is_all_bits_zero \
    ((uint8_t (*)(uint32_t val, uint8_t start, uint8_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_is_all_bits_zero])

#define romapi_bflb_ef_ctrl_get_byte_zero_cnt \
    ((uint32_t (*)(uint8_t val)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_get_byte_zero_cnt])

#define romapi_bflb_ef_ctrl_get_trim_parity \
    ((uint8_t (*)(uint32_t val, uint8_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_get_trim_parity])

#define romapi_bflb_gpio_init \
    ((void (*)(struct bflb_device_s *dev, uint8_t pin, uint32_t cfgset)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_init])

#define romapi_bflb_gpio_deinit \
    ((void (*)(struct bflb_device_s *dev, uint8_t pin)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_deinit])

#define romapi_bflb_gpio_output_value_mode_enable \
    ((void (*)(struct bflb_device_s *dev, uint8_t pin)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_output_value_mode_enable])

#define romapi_bflb_gpio_pin0_31_output \
    ((void (*)(struct bflb_device_s *dev, uint32_t value)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin0_31_output])

#define romapi_bflb_gpio_pin32_63_output \
    ((void (*)(struct bflb_device_s *dev, uint32_t value)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin32_63_output])

#define romapi_bflb_gpio_set \
    ((void (*)(struct bflb_device_s *dev, uint8_t pin)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_set])

#define romapi_bflb_gpio_pin0_31_set \
    ((void (*)(struct bflb_device_s *dev, uint32_t value)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin0_31_set])

#define romapi_bflb_gpio_pin32_63_set \
    ((void (*)(struct bflb_device_s *dev, uint32_t value)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin32_63_set])

#define romapi_bflb_gpio_reset \
    ((void (*)(struct bflb_device_s *dev, uint8_t pin)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_reset])

#define romapi_bflb_gpio_pin0_31_reset \
    ((void (*)(struct bflb_device_s *dev, uint32_t value)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin0_31_reset])

#define romapi_bflb_gpio_pin32_63_reset \
    ((void (*)(struct bflb_device_s *dev, uint32_t value)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin32_63_reset])

#define romapi_bflb_gpio_read \
    ((bool (*)(struct bflb_device_s *dev, uint8_t pin)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_read])

#define romapi_bflb_gpio_pin0_31_read \
    ((uint32_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin0_31_read])

#define romapi_bflb_gpio_pin32_63_read \
    ((uint32_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin32_63_read])

#define romapi_bflb_gpio_int_init \
    ((void (*)(struct bflb_device_s *dev, uint8_t pin, uint8_t trig_mode)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_int_init])

#define romapi_bflb_gpio_int_mask \
    ((void (*)(struct bflb_device_s *dev, uint8_t pin, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_int_mask])

#define romapi_bflb_gpio_get_intstatus \
    ((bool (*)(struct bflb_device_s *dev, uint8_t pin)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_get_intstatus])

#define romapi_bflb_gpio_get_intstatus_pin0_31 \
    ((uint32_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_get_intstatus_pin0_31])

#define romapi_bflb_gpio_get_intstatus_pin32_63 \
    ((uint32_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_get_intstatus_pin32_63])

#define romapi_bflb_gpio_int_clear \
    ((void (*)(struct bflb_device_s *dev, uint8_t pin)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_int_clear])

#define romapi_bflb_gpio_int_clear_pin0_31 \
    ((void (*)(struct bflb_device_s *dev, uint32_t pins)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_int_clear_pin0_31])

#define romapi_bflb_gpio_int_clear_pin32_63 \
    ((void (*)(struct bflb_device_s *dev, uint32_t pins)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_int_clear_pin32_63])

#define romapi_bflb_gpio_uart_init \
    ((void (*)(struct bflb_device_s *dev, uint8_t pin, uint8_t uart_func)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_uart_init])

#define romapi_bflb_gpio_feature_control \
    ((int (*)(struct bflb_device_s *dev, int cmd, size_t arg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_feature_control])

#define romapi_bflb_i2c_init \
    ((void (*)(struct bflb_device_s *dev, uint32_t frequency)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_i2c_init])

#define romapi_bflb_i2c_deinit \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_i2c_deinit])

#define romapi_bflb_i2c_link_txdma \
    ((void (*)(struct bflb_device_s *dev, bool enable)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_i2c_link_txdma])

#define romapi_bflb_i2c_link_rxdma \
    ((void (*)(struct bflb_device_s *dev, bool enable)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_i2c_link_rxdma])

#define romapi_bflb_i2c_transfer \
    ((int (*)(struct bflb_device_s *dev, struct bflb_i2c_msg_s *msgs, int count)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_i2c_transfer])

#define romapi_bflb_i2c_int_mask \
    ((void (*)(struct bflb_device_s *dev, uint32_t int_type, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_i2c_int_mask])

#define romapi_bflb_i2c_int_clear \
    ((void (*)(struct bflb_device_s *dev, uint32_t int_clear)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_i2c_int_clear])

#define romapi_bflb_i2c_get_intstatus \
    ((uint32_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_i2c_get_intstatus])

#define romapi_bflb_i2c_feature_control \
    ((int (*)(struct bflb_device_s *dev, int cmd, size_t arg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_i2c_feature_control])

#define romapi_bflb_i2s_init \
    ((void (*)(struct bflb_device_s *dev, const struct bflb_i2s_config_s *config)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_i2s_init])

#define romapi_bflb_i2s_deinit \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_i2s_deinit])

#define romapi_bflb_i2s_link_txdma \
    ((void (*)(struct bflb_device_s *dev, bool enable)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_i2s_link_txdma])

#define romapi_bflb_i2s_link_rxdma \
    ((void (*)(struct bflb_device_s *dev, bool enable)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_i2s_link_rxdma])

#define romapi_bflb_i2s_txint_mask \
    ((void (*)(struct bflb_device_s *dev, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_i2s_txint_mask])

#define romapi_bflb_i2s_rxint_mask \
    ((void (*)(struct bflb_device_s *dev, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_i2s_rxint_mask])

#define romapi_bflb_i2s_errint_mask \
    ((void (*)(struct bflb_device_s *dev, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_i2s_errint_mask])

#define romapi_bflb_i2s_get_intstatus \
    ((uint32_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_i2s_get_intstatus])

#define romapi_bflb_i2s_feature_control \
    ((int (*)(struct bflb_device_s *dev, int cmd, size_t arg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_i2s_feature_control])

#define romapi_bflb_ir_tx_init \
    ((void (*)(struct bflb_device_s *dev, const struct bflb_ir_tx_config_s *config)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ir_tx_init])

#define romapi_bflb_ir_send \
    ((void (*)(struct bflb_device_s *dev, uint32_t *data, uint32_t length)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ir_send])

#define romapi_bflb_ir_swm_send \
    ((void (*)(struct bflb_device_s *dev, uint16_t *data, uint32_t length)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ir_swm_send])

#define romapi_bflb_ir_tx_enable \
    ((void (*)(struct bflb_device_s *dev, bool enable)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ir_tx_enable])

#define romapi_bflb_ir_txint_mask \
    ((void (*)(struct bflb_device_s *dev, uint8_t int_type, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ir_txint_mask])

#define romapi_bflb_ir_txint_clear \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ir_txint_clear])

#define romapi_bflb_ir_get_txint_status \
    ((uint32_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ir_get_txint_status])

#define romapi_bflb_ir_link_txdma \
    ((void (*)(struct bflb_device_s *dev, bool enable)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ir_link_txdma])

#define romapi_bflb_ir_get_txfifo_cnt \
    ((uint8_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ir_get_txfifo_cnt])

#define romapi_bflb_ir_txfifo_clear \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ir_txfifo_clear])

#define romapi_bflb_ir_rx_init \
    ((void (*)(struct bflb_device_s *dev, const struct bflb_ir_rx_config_s *config)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ir_rx_init])

#define romapi_bflb_ir_receive \
    ((uint16_t (*)(struct bflb_device_s *dev, uint64_t *data)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ir_receive])

#define romapi_bflb_ir_swm_receive \
    ((uint16_t (*)(struct bflb_device_s *dev, uint16_t *data, uint16_t length)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ir_swm_receive])

#define romapi_bflb_ir_rx_enable \
    ((void (*)(struct bflb_device_s *dev, bool enable)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ir_rx_enable])

#define romapi_bflb_ir_rxint_mask \
    ((void (*)(struct bflb_device_s *dev, uint8_t int_type, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ir_rxint_mask])

#define romapi_bflb_ir_rxint_clear \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ir_rxint_clear])

#define romapi_bflb_ir_get_rxint_status \
    ((uint32_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ir_get_rxint_status])

#define romapi_bflb_ir_get_rxfifo_cnt \
    ((uint8_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ir_get_rxfifo_cnt])

#define romapi_bflb_ir_rxfifo_clear \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ir_rxfifo_clear])

#define romapi_bflb_ir_feature_control \
    ((int (*)(struct bflb_device_s *dev, int cmd, size_t arg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_ir_feature_control])

#define romapi_bflb_irq_restore \
    ((void (*)(uintptr_t flags)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_irq_restore])

#define romapi_bflb_irq_enable \
    ((void (*)(int irq)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_irq_enable])

#define romapi_bflb_irq_disable \
    ((void (*)(int irq)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_irq_disable])

#define romapi_bflb_irq_set_pending \
    ((void (*)(int irq)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_irq_set_pending])

#define romapi_bflb_irq_clear_pending \
    ((void (*)(int irq)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_irq_clear_pending])

#define romapi_bflb_irq_set_nlbits \
    ((void (*)(uint8_t nlbits)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_irq_set_nlbits])

#define romapi_bflb_irq_set_priority \
    ((void (*)(int irq, uint8_t preemptprio, uint8_t subprio)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_irq_set_priority])

#define romapi_bflb_l1c_icache_enable \
    ((void (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_icache_enable])

#define romapi_bflb_l1c_icache_disable \
    ((void (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_icache_disable])

#define romapi_bflb_l1c_icache_invalid_all \
    ((void (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_icache_invalid_all])

#define romapi_bflb_l1c_icache_invalid_range \
    ((void (*)(void *addr, uint32_t size)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_icache_invalid_range])

#define romapi_bflb_l1c_dcache_enable \
    ((void (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_enable])

#define romapi_bflb_l1c_dcache_disable \
    ((void (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_disable])

#define romapi_bflb_l1c_dcache_clean_all \
    ((void (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_clean_all])

#define romapi_bflb_l1c_dcache_invalidate_all \
    ((void (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_invalidate_all])

#define romapi_bflb_l1c_dcache_clean_invalidate_all \
    ((void (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_clean_invalidate_all])

#define romapi_bflb_l1c_dcache_clean_range \
    ((void (*)(void *addr, uint32_t size)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_clean_range])

#define romapi_bflb_l1c_dcache_invalidate_range \
    ((void (*)(void *addr, uint32_t size)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_invalidate_range])

#define romapi_bflb_l1c_dcache_clean_invalidate_range \
    ((void (*)(void *addr, uint32_t size)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_clean_invalidate_range])

#define romapi_bflb_mjdec_set_dqt_from_zigzag \
    ((void (*)(struct bflb_device_s *dev, uint8_t *yy, uint8_t *uv)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_set_dqt_from_zigzag])

#define romapi_bflb_mjdec_set_dqt_from_header \
    ((uint8_t (*)(struct bflb_device_s *dev, uint8_t *header)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_set_dqt_from_header])

#define romapi_bflb_mjdec_set_dqt_from_quality \
    ((void (*)(struct bflb_device_s *dev, uint8_t quality)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_set_dqt_from_quality])

#define romapi_bflb_mjdec_set_dht_from_spec \
    ((uint8_t (*)(struct bflb_device_s *dev, struct bflb_mjdec_dht_s *dht)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_set_dht_from_spec])

#define romapi_bflb_mjdec_set_dht_from_header \
    ((uint8_t (*)(struct bflb_device_s *dev, uint8_t *header)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_set_dht_from_header])

#define romapi_bflb_mjdec_init \
    ((void (*)(struct bflb_device_s *dev, const struct bflb_mjdec_config_s *config)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_init])

#define romapi_bflb_mjdec_start \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_start])

#define romapi_bflb_mjdec_stop \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_stop])

#define romapi_bflb_mjdec_push_jpeg \
    ((void (*)(struct bflb_device_s *dev, void *frame)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_push_jpeg])

#define romapi_bflb_mjdec_tcint_mask \
    ((void (*)(struct bflb_device_s *dev, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_tcint_mask])

#define romapi_bflb_mjdec_get_intstatus \
    ((uint32_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_get_intstatus])

#define romapi_bflb_mjdec_int_clear \
    ((void (*)(struct bflb_device_s *dev, uint32_t int_clear)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_int_clear])

#define romapi_bflb_mjdec_get_frame_count \
    ((uint8_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_get_frame_count])

#define romapi_bflb_mjdec_pop_one_frame \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_pop_one_frame])

#define romapi_bflb_mjdec_feature_control \
    ((int (*)(struct bflb_device_s *dev, int cmd, size_t arg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjdec_feature_control])

#define romapi_bflb_mjpeg_init \
    ((void (*)(struct bflb_device_s *dev, const struct bflb_mjpeg_config_s *config)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_init])

#define romapi_bflb_mjpeg_start \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_start])

#define romapi_bflb_mjpeg_stop \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_stop])

#define romapi_bflb_mjpeg_sw_run \
    ((void (*)(struct bflb_device_s *dev, uint8_t frame_count)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_sw_run])

#define romapi_bflb_mjpeg_kick_run \
    ((void (*)(struct bflb_device_s *dev, uint16_t kick_count)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_kick_run])

#define romapi_bflb_mjpeg_kick_stop \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_kick_stop])

#define romapi_bflb_mjpeg_kick \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_kick])

#define romapi_bflb_mjpeg_tcint_mask \
    ((void (*)(struct bflb_device_s *dev, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_tcint_mask])

#define romapi_bflb_mjpeg_swapint_mask \
    ((void (*)(struct bflb_device_s *dev, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_swapint_mask])

#define romapi_bflb_mjpeg_errint_mask \
    ((void (*)(struct bflb_device_s *dev, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_errint_mask])

#define romapi_bflb_mjpeg_get_intstatus \
    ((uint32_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_get_intstatus])

#define romapi_bflb_mjpeg_int_clear \
    ((void (*)(struct bflb_device_s *dev, uint32_t int_clear)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_int_clear])

#define romapi_bflb_mjpeg_get_frame_count \
    ((uint8_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_get_frame_count])

#define romapi_bflb_mjpeg_pop_one_frame \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_pop_one_frame])

#define romapi_bflb_mjpeg_pop_swap_block \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_pop_swap_block])

#define romapi_bflb_mjpeg_get_swap_bit_count \
    ((uint32_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_get_swap_bit_count])

#define romapi_bflb_mjpeg_get_frame_info \
    ((uint32_t (*)(struct bflb_device_s *dev, uint8_t **pic)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_get_frame_info])

#define romapi_bflb_mjpeg_get_swap_block_info \
    ((uint8_t (*)(struct bflb_device_s *dev, uint8_t *idx)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_get_swap_block_info])

#define romapi_bflb_mjpeg_swap_is_block_full \
    ((uint8_t (*)(struct bflb_device_s *dev, uint8_t idx)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_swap_is_block_full])

#define romapi_bflb_mjpeg_calculate_quantize_table \
    ((void (*)(uint8_t quality, uint16_t *input_table, uint16_t *output_table)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_calculate_quantize_table])

#define romapi_bflb_mjpeg_fill_quantize_table \
    ((void (*)(struct bflb_device_s *dev, uint16_t *input_yy, uint16_t *input_uv)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_fill_quantize_table])

#define romapi_bflb_mjpeg_fill_jpeg_header_tail \
    ((void (*)(struct bflb_device_s *dev, uint8_t *header, uint32_t header_len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_fill_jpeg_header_tail])

#define romapi_bflb_mjpeg_set_yuv420sp_cam_input \
    ((void (*)(struct bflb_device_s *dev, uint8_t yy, uint8_t uv)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_set_yuv420sp_cam_input])

#define romapi_bflb_mjpeg_update_input_output_buff \
    ((void (*)(struct bflb_device_s *dev, void *input_buf0, void *input_buf1, void *output_buff, size_t output_buff_size)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_update_input_output_buff])

#define romapi_bflb_mjpeg_feature_control \
    ((int (*)(struct bflb_device_s *dev, int cmd, size_t arg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mjpeg_feature_control])

#define romapi_bflb_mtimer_delay_us \
    ((void (*)(uint32_t time)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mtimer_delay_us])

#define romapi_bflb_mtimer_delay_ms \
    ((void (*)(uint32_t time)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mtimer_delay_ms])

#define romapi_bflb_pwm_v2_init \
    ((void (*)(struct bflb_device_s *dev, const struct bflb_pwm_v2_config_s *config)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_init])

#define romapi_bflb_pwm_v2_deinit \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_deinit])

#define romapi_bflb_pwm_v2_set_period \
    ((void (*)(struct bflb_device_s *dev, uint16_t period)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_set_period])

#define romapi_bflb_pwm_v2_start \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_start])

#define romapi_bflb_pwm_v2_stop \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_stop])

#define romapi_bflb_pwm_v2_get_frequency \
    ((float (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_get_frequency])

#define romapi_bflb_pwm_v2_channel_init \
    ((void (*)(struct bflb_device_s *dev, uint8_t ch, struct bflb_pwm_v2_channel_config_s *config)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_channel_init])

#define romapi_bflb_pwm_v2_channel_set_threshold \
    ((void (*)(struct bflb_device_s *dev, uint8_t ch, uint16_t low_threhold, uint16_t high_threhold)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_channel_set_threshold])

#define romapi_bflb_pwm_v2_channel_positive_start \
    ((void (*)(struct bflb_device_s *dev, uint8_t ch)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_channel_positive_start])

#define romapi_bflb_pwm_v2_channel_negative_start \
    ((void (*)(struct bflb_device_s *dev, uint8_t ch)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_channel_negative_start])

#define romapi_bflb_pwm_v2_channel_positive_stop \
    ((void (*)(struct bflb_device_s *dev, uint8_t ch)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_channel_positive_stop])

#define romapi_bflb_pwm_v2_channel_negative_stop \
    ((void (*)(struct bflb_device_s *dev, uint8_t ch)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_channel_negative_stop])

#define romapi_bflb_pwm_v2_int_enable \
    ((void (*)(struct bflb_device_s *dev, uint32_t int_en, bool enable)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_int_enable])

#define romapi_bflb_pwm_v2_int_clear \
    ((void (*)(struct bflb_device_s *dev, uint32_t int_clear)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_int_clear])

#define romapi_bflb_pwm_v2_get_intstatus \
    ((uint32_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_get_intstatus])

#define romapi_bflb_pwm_v2_feature_control \
    ((int (*)(struct bflb_device_s *dev, int cmd, size_t arg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_pwm_v2_feature_control])

#define romapi_bflb_rtc_disable \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_rtc_disable])

#define romapi_bflb_rtc_set_time \
    ((void (*)(struct bflb_device_s *dev, uint64_t time)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_rtc_set_time])

#define romapi_bflb_sdh_init \
    ((int (*)(struct bflb_device_s *dev, struct bflb_sdh_config_s *cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_init])

#define romapi_bflb_sdh_adma2_desc_init \
    ((int (*)(struct bflb_device_s *dev, struct bflb_sdh_data_cfg_s *data_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_adma2_desc_init])

#define romapi_bflb_sdh_cmd_cfg \
    ((int (*)(struct bflb_device_s *dev, struct bflb_sdh_cmd_cfg_s *cmd_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_cmd_cfg])

#define romapi_bflb_sdh_data_cfg \
    ((int (*)(struct bflb_device_s *dev, struct bflb_sdh_data_cfg_s *data_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_data_cfg])

#define romapi_bflb_sdh_tranfer_start \
    ((int (*)(struct bflb_device_s *dev, struct bflb_sdh_cmd_cfg_s *cmd_cfg, struct bflb_sdh_data_cfg_s *data_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_tranfer_start])

#define romapi_bflb_sdh_get_resp \
    ((int (*)(struct bflb_device_s *dev, struct bflb_sdh_cmd_cfg_s *cmd_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_get_resp])

#define romapi_bflb_sdh_sta_en \
    ((void (*)(struct bflb_device_s *dev, uint32_t sta_bit, bool en)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_sta_en])

#define romapi_bflb_sdh_sta_en_get \
    ((uint32_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_sta_en_get])

#define romapi_bflb_sdh_sta_int_en \
    ((void (*)(struct bflb_device_s *dev, uint32_t sta_bit, bool en)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_sta_int_en])

#define romapi_bflb_sdh_sta_int_en_get \
    ((uint32_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_sta_int_en_get])

#define romapi_bflb_sdh_sta_get \
    ((uint32_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_sta_get])

#define romapi_bflb_sdh_sta_clr \
    ((void (*)(struct bflb_device_s *dev, uint32_t sta_bit)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_sta_clr])

#define romapi_bflb_sdh_feature_control \
    ((int (*)(struct bflb_device_s *dev, int cmd, uintptr_t arg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sdh_feature_control])

#define romapi_bflb_aes_init \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_aes_init])

#define romapi_bflb_aes_deinit \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_aes_deinit])

#define romapi_bflb_aes_select_hwkey \
    ((void (*)(struct bflb_device_s *dev, uint8_t keysel0, uint8_t keysel1)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_aes_select_hwkey])

#define romapi_bflb_aes_set_mode \
    ((void (*)(struct bflb_device_s *dev, uint8_t mode)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_aes_set_mode])

#define romapi_bflb_aes_setkey \
    ((void (*)(struct bflb_device_s *dev, const uint8_t *key, uint16_t keybits)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_aes_setkey])

#define romapi_bflb_aes_encrypt \
    ((int (*)(struct bflb_device_s *dev, const uint8_t *input, const uint8_t *iv, uint8_t *output, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_aes_encrypt])

#define romapi_bflb_aes_decrypt \
    ((int (*)(struct bflb_device_s *dev, const uint8_t *input, const uint8_t *iv, uint8_t *output, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_aes_decrypt])

#define romapi_bflb_aes_link_init \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_aes_link_init])

#define romapi_bflb_aes_link_deinit \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_aes_link_deinit])

#define romapi_bflb_aes_link_update \
    ((int (*)(struct bflb_device_s *dev, uint32_t link_addr, const uint8_t *input, uint8_t *output, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_aes_link_update])

#define romapi_bflb_group0_request_aes_access \
    ((int (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_group0_request_aes_access])

#define romapi_bflb_group1_request_aes_access \
    ((int (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_group1_request_aes_access])

#define romapi_bflb_group0_release_aes_access \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_group0_release_aes_access])

#define romapi_bflb_group1_release_aes_access \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_group1_release_aes_access])

#define romapi_bflb_aes_set_hwkey_source \
    ((void (*)(struct bflb_device_s *dev, uint8_t source)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_aes_set_hwkey_source])

#define romapi_bflb_sha_init \
    ((void (*)(struct bflb_device_s *dev, uint8_t mode)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha_init])

#define romapi_bflb_sha1_start \
    ((void (*)(struct bflb_device_s *dev, struct bflb_sha1_ctx_s *ctx)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha1_start])

#define romapi_bflb_sha256_start \
    ((void (*)(struct bflb_device_s *dev, struct bflb_sha256_ctx_s *ctx)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha256_start])

#define romapi_bflb_sha512_start \
    ((void (*)(struct bflb_device_s *dev, struct bflb_sha512_ctx_s *ctx)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha512_start])

#define romapi_bflb_sha1_update \
    ((int (*)(struct bflb_device_s *dev, struct bflb_sha1_ctx_s *ctx, const uint8_t *input, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha1_update])

#define romapi_bflb_sha1_once_padded \
    ((int (*)(struct bflb_device_s *dev, const uint8_t *input, uint8_t *output, uint32_t nblock)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha1_once_padded])

#define romapi_bflb_sha256_update \
    ((int (*)(struct bflb_device_s *dev, struct bflb_sha256_ctx_s *ctx, const uint8_t *input, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha256_update])

#define romapi_bflb_sha512_update \
    ((int (*)(struct bflb_device_s *dev, struct bflb_sha512_ctx_s *ctx, const uint8_t *input, uint64_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha512_update])

#define romapi_bflb_sha1_finish \
    ((void (*)(struct bflb_device_s *dev, struct bflb_sha1_ctx_s *ctx, uint8_t *output)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha1_finish])

#define romapi_bflb_sha256_finish \
    ((void (*)(struct bflb_device_s *dev, struct bflb_sha256_ctx_s *ctx, uint8_t *output)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha256_finish])

#define romapi_bflb_sha512_finish \
    ((void (*)(struct bflb_device_s *dev, struct bflb_sha512_ctx_s *ctx, uint8_t *output)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha512_finish])

#define romapi_bflb_sha_link_init \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha_link_init])

#define romapi_bflb_sha_link_deinit \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha_link_deinit])

#define romapi_bflb_sha1_link_start \
    ((void (*)(struct bflb_device_s *dev, struct bflb_sha1_link_ctx_s *ctx)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha1_link_start])

#define romapi_bflb_sha256_link_start \
    ((void (*)(struct bflb_device_s *dev, struct bflb_sha256_link_ctx_s *ctx, int is224)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha256_link_start])

#define romapi_bflb_sha512_link_start \
    ((void (*)(struct bflb_device_s *dev, struct bflb_sha512_link_ctx_s *ctx, int is384)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha512_link_start])

#define romapi_bflb_sha1_link_update \
    ((int (*)(struct bflb_device_s *dev, struct bflb_sha1_link_ctx_s *ctx, const uint8_t *input, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha1_link_update])

#define romapi_bflb_sha256_link_update \
    ((int (*)(struct bflb_device_s *dev, struct bflb_sha256_link_ctx_s *ctx, const uint8_t *input, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha256_link_update])

#define romapi_bflb_sha512_link_update \
    ((int (*)(struct bflb_device_s *dev, struct bflb_sha512_link_ctx_s *ctx, const uint8_t *input, uint64_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha512_link_update])

#define romapi_bflb_sha1_link_finish \
    ((void (*)(struct bflb_device_s *dev, struct bflb_sha1_link_ctx_s *ctx, uint8_t *output)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha1_link_finish])

#define romapi_bflb_sha256_link_finish \
    ((void (*)(struct bflb_device_s *dev, struct bflb_sha256_link_ctx_s *ctx, uint8_t *output)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha256_link_finish])

#define romapi_bflb_sha512_link_finish \
    ((void (*)(struct bflb_device_s *dev, struct bflb_sha512_link_ctx_s *ctx, uint8_t *output)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha512_link_finish])

#define romapi_bflb_group0_request_sha_access \
    ((int (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_group0_request_sha_access])

#define romapi_bflb_group1_request_sha_access \
    ((int (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_group1_request_sha_access])

#define romapi_bflb_group0_release_sha_access \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_group0_release_sha_access])

#define romapi_bflb_group1_release_sha_access \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_group1_release_sha_access])

#define romapi_bflb_sha_crc32_link_work \
    ((int (*)(struct bflb_device_s *dev, uint32_t addr, const uint8_t *in, uint32_t len, uint8_t *out)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sha_crc32_link_work])

#define romapi_bflb_trng_read \
    ((int (*)(struct bflb_device_s *dev, uint8_t data[32])) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_trng_read])

#define romapi_bflb_trng_readlen \
    ((int (*)(uint8_t *data, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_trng_readlen])

#define romapi_bflb_group0_request_trng_access \
    ((int (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_group0_request_trng_access])

#define romapi_bflb_group1_request_trng_access \
    ((int (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_group1_request_trng_access])

#define romapi_bflb_group0_release_trng_access \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_group0_release_trng_access])

#define romapi_bflb_group1_release_trng_access \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_group1_release_trng_access])

#define romapi_bflb_spi_init \
    ((void (*)(struct bflb_device_s *dev, const struct bflb_spi_config_s *config)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_spi_init])

#define romapi_bflb_spi_deinit \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_spi_deinit])

#define romapi_bflb_spi_link_txdma \
    ((void (*)(struct bflb_device_s *dev, bool enable)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_spi_link_txdma])

#define romapi_bflb_spi_link_rxdma \
    ((void (*)(struct bflb_device_s *dev, bool enable)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_spi_link_rxdma])

#define romapi_bflb_spi_poll_send \
    ((uint32_t (*)(struct bflb_device_s *dev, uint32_t data)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_spi_poll_send])

#define romapi_bflb_spi_poll_exchange \
    ((int (*)(struct bflb_device_s *dev, const void *txbuffer, void *rxbuffer, size_t nbytes)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_spi_poll_exchange])

#define romapi_bflb_spi_txint_mask \
    ((void (*)(struct bflb_device_s *dev, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_spi_txint_mask])

#define romapi_bflb_spi_rxint_mask \
    ((void (*)(struct bflb_device_s *dev, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_spi_rxint_mask])

#define romapi_bflb_spi_tcint_mask \
    ((void (*)(struct bflb_device_s *dev, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_spi_tcint_mask])

#define romapi_bflb_spi_rtoint_mask \
    ((void (*)(struct bflb_device_s *dev, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_spi_rtoint_mask])

#define romapi_bflb_spi_errint_mask \
    ((void (*)(struct bflb_device_s *dev, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_spi_errint_mask])

#define romapi_bflb_spi_get_intstatus \
    ((uint32_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_spi_get_intstatus])

#define romapi_bflb_spi_int_clear \
    ((void (*)(struct bflb_device_s *dev, uint32_t int_clear)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_spi_int_clear])

#define romapi_bflb_spi_isbusy \
    ((bool (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_spi_isbusy])

#define romapi_bflb_spi_feature_control \
    ((int (*)(struct bflb_device_s *dev, int cmd, size_t arg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_spi_feature_control])

#define romapi_bflb_timer_init \
    ((void (*)(struct bflb_device_s *dev, const struct bflb_timer_config_s *config)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_timer_init])

#define romapi_bflb_timer_deinit \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_timer_deinit])

#define romapi_bflb_timer_start \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_timer_start])

#define romapi_bflb_timer_stop \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_timer_stop])

#define romapi_bflb_timer_set_preloadvalue \
    ((void (*)(struct bflb_device_s *dev, uint32_t val)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_timer_set_preloadvalue])

#define romapi_bflb_timer_set_compvalue \
    ((void (*)(struct bflb_device_s *dev, uint8_t cmp_no, uint32_t val)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_timer_set_compvalue])

#define romapi_bflb_timer_get_compvalue \
    ((uint32_t (*)(struct bflb_device_s *dev, uint8_t cmp_no)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_timer_get_compvalue])

#define romapi_bflb_timer_get_countervalue \
    ((uint32_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_timer_get_countervalue])

#define romapi_bflb_timer_compint_mask \
    ((void (*)(struct bflb_device_s *dev, uint8_t cmp_no, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_timer_compint_mask])

#define romapi_bflb_timer_get_compint_status \
    ((bool (*)(struct bflb_device_s *dev, uint8_t cmp_no)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_timer_get_compint_status])

#define romapi_bflb_timer_compint_clear \
    ((void (*)(struct bflb_device_s *dev, uint8_t cmp_no)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_timer_compint_clear])

#define romapi_bflb_uart_init \
    ((void (*)(struct bflb_device_s *dev, const struct bflb_uart_config_s *config)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_uart_init])

#define romapi_bflb_uart_deinit \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_uart_deinit])

#define romapi_bflb_uart_enable \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_uart_enable])

#define romapi_bflb_uart_disable \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_uart_disable])

#define romapi_bflb_uart_link_txdma \
    ((void (*)(struct bflb_device_s *dev, bool enable)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_uart_link_txdma])

#define romapi_bflb_uart_link_rxdma \
    ((void (*)(struct bflb_device_s *dev, bool enable)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_uart_link_rxdma])

#define romapi_bflb_uart_putchar \
    ((int (*)(struct bflb_device_s *dev, int ch)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_uart_putchar])

#define romapi_bflb_uart_getchar \
    ((int (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_uart_getchar])

#define romapi_bflb_uart_put \
    ((int (*)(struct bflb_device_s *dev, uint8_t *data, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_uart_put])

#define romapi_bflb_uart_put_block \
    ((int (*)(struct bflb_device_s *dev, uint8_t *data, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_uart_put_block])

#define romapi_bflb_uart_get \
    ((int (*)(struct bflb_device_s *dev, uint8_t *data, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_uart_get])

#define romapi_bflb_uart_wait_tx_done \
    ((int (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_uart_wait_tx_done])

#define romapi_bflb_uart_txready \
    ((bool (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_uart_txready])

#define romapi_bflb_uart_txempty \
    ((bool (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_uart_txempty])

#define romapi_bflb_uart_rxavailable \
    ((bool (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_uart_rxavailable])

#define romapi_bflb_uart_txint_mask \
    ((void (*)(struct bflb_device_s *dev, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_uart_txint_mask])

#define romapi_bflb_uart_rxint_mask \
    ((void (*)(struct bflb_device_s *dev, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_uart_rxint_mask])

#define romapi_bflb_uart_errint_mask \
    ((void (*)(struct bflb_device_s *dev, bool mask)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_uart_errint_mask])

#define romapi_bflb_uart_get_intstatus \
    ((uint32_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_uart_get_intstatus])

#define romapi_bflb_uart_int_clear \
    ((void (*)(struct bflb_device_s *dev, uint32_t int_clear)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_uart_int_clear])

#define romapi_bflb_uart_feature_control \
    ((int (*)(struct bflb_device_s *dev, int cmd, size_t arg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_uart_feature_control])

#define romapi_bflb_wdg_init \
    ((void (*)(struct bflb_device_s *dev, const struct bflb_wdg_config_s *config)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_wdg_init])

#define romapi_bflb_wdg_start \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_wdg_start])

#define romapi_bflb_wdg_stop \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_wdg_stop])

#define romapi_bflb_wdg_get_countervalue \
    ((uint16_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_wdg_get_countervalue])

#define romapi_bflb_wdg_set_countervalue \
    ((void (*)(struct bflb_device_s *dev, uint16_t value)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_wdg_set_countervalue])

#define romapi_bflb_wdg_reset_countervalue \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_wdg_reset_countervalue])

#define romapi_bflb_wdg_compint_clear \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_wdg_compint_clear])

#define romapi_bflb_wo_pin_init \
    ((void (*)(struct bflb_device_s *dev, uint8_t pin, uint8_t mode)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_wo_pin_init])

#define romapi_bflb_wo_init \
    ((void (*)(struct bflb_device_s *dev, struct bflb_wo_cfg_s *cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_wo_init])

#define romapi_bflb_wo_enable \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_wo_enable])

#define romapi_bflb_wo_disable \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_wo_disable])

#define romapi_bflb_wo_get_fifo_available_cnt \
    ((uint32_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_wo_get_fifo_available_cnt])

#define romapi_bflb_wo_push_fifo \
    ((uint32_t (*)(struct bflb_device_s *dev, uint16_t *data, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_wo_push_fifo])

#define romapi_bflb_wo_clear_fifo \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_wo_clear_fifo])

#define romapi_bflb_wo_enable_dma \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_wo_enable_dma])

#define romapi_bflb_wo_disable_dma \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_wo_disable_dma])

#define romapi_bflb_wo_get_int_status \
    ((uint32_t (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_wo_get_int_status])

#define romapi_bflb_wo_int_mask \
    ((void (*)(struct bflb_device_s *dev, uint32_t int_type)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_wo_int_mask])

#define romapi_bflb_wo_int_unmask \
    ((void (*)(struct bflb_device_s *dev, uint32_t int_type)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_wo_int_unmask])

#define romapi_bflb_wo_int_clear \
    ((void (*)(struct bflb_device_s *dev, uint32_t int_type)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_wo_int_clear])

#define romapi_bflb_flash_secreg_callapi_before \
    ((int (*)(const spi_flash_cfg_type *flash_cfg, struct callapi_content *content)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_callapi_before])

#define romapi_bflb_flash_secreg_callapi_after \
    ((void (*)(const spi_flash_cfg_type *flash_cfg, struct callapi_content *content)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_callapi_after])

#define romapi_bflb_flash_secreg_get_param \
    ((int (*)(uint32_t jid, const bflb_flash_secreg_param_t **param)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_get_param])

#define romapi_bflb_flash_secreg_region_foreach \
    ((int (*)(bflb_flash_otp_config_t *otp_cfg, region_cb cb, void *data)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_region_foreach])

#define romapi_bflb_flash_secreg_get_lockbits \
    ((int (*)(bflb_flash_otp_config_t *otp_cfg, uint8_t *lockbits)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_get_lockbits])

#define romapi_bflb_flash_secreg_set_lockbits \
    ((int (*)(bflb_flash_otp_config_t *otp_cfg, uint8_t lockbits)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_set_lockbits])

#define romapi_bflb_flash_secreg_get_locked \
    ((int (*)(bflb_flash_otp_config_t *otp_cfg, uint8_t index)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_get_locked])

#define romapi_bflb_flash_secreg_set_locked \
    ((int (*)(bflb_flash_otp_config_t *otp_cfg, uint8_t index)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_set_locked])

#define romapi_bflb_flash_secreg_get_info_by_idx \
    ((int (*)(bflb_flash_otp_config_t *otp_cfg, uint8_t index, bflb_flash_secreg_region_info_t *info)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_get_info_by_idx])

#define romapi_bflb_flash_secreg_valid_by_idx \
    ((int (*)(bflb_flash_otp_config_t *otp_cfg, uint8_t index, uint32_t offset, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_valid_by_idx])

#define romapi_bflb_flash_secreg_read_by_idx \
    ((int (*)(bflb_flash_otp_config_t *otp_cfg, uint8_t index, uint32_t offset, void *data, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_read_by_idx])

#define romapi_bflb_flash_secreg_write_by_idx \
    ((int (*)(bflb_flash_otp_config_t *otp_cfg, uint8_t index, uint32_t offset, const void *data, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_write_by_idx])

#define romapi_bflb_flash_secreg_erase_by_idx \
    ((int (*)(bflb_flash_otp_config_t *otp_cfg, uint8_t index)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_erase_by_idx])

#define romapi_bflb_flash_secreg_get_info \
    ((int (*)(bflb_flash_otp_config_t *otp_cfg, uint32_t addr, bflb_flash_secreg_region_info_t *info)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_get_info])

#define romapi_bflb_flash_secreg_valid \
    ((int (*)(bflb_flash_otp_config_t *otp_cfg, uint32_t addr, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_valid])

#define romapi_bflb_flash_secreg_read \
    ((int (*)(bflb_flash_otp_config_t *otp_cfg, uint32_t addr, void *data, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_read])

#define romapi_bflb_flash_secreg_write \
    ((int (*)(bflb_flash_otp_config_t *otp_cfg, uint32_t addr, const void *data, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_write])

#define romapi_bflb_flash_secreg_erase \
    ((int (*)(bflb_flash_otp_config_t *otp_cfg, uint32_t addr, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_erase])

#define romapi_bflb_sf_cfg_init_ext_flash_gpio \
    ((int (*)(uint8_t ext_flash_pin)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_init_ext_flash_gpio])

#define romapi_bflb_sf_cfg_deinit_ext_flash_gpio \
    ((int (*)(uint8_t ext_flash_pin)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_deinit_ext_flash_gpio])

#define romapi_bflb_sf_cfg_init_flash_gpio \
    ((int (*)(uint8_t flash_pin_cfg, uint8_t restore_default)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_init_flash_gpio])

#define romapi_bflb_sf_cfg_init_flash2_gpio \
    ((int (*)(uint8_t swap)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_init_flash2_gpio])

#define romapi_bflb_sf_cfg_flash_identify \
    ((uint32_t (*)(uint8_t call_from_flash, uint8_t flash_pin_cfg, uint8_t restore_default, spi_flash_cfg_type *p_flash_cfg, uint8_t group, uint8_t bank)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_flash_identify])

#define romapi_bflb_sf_cfg_get_flash_cfg_need_lock \
    ((int (*)(uint32_t flash_id, spi_flash_cfg_type *p_flash_cfg, uint8_t group, uint8_t bank)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_get_flash_cfg_need_lock])

#define romapi_bflb_sf_cfg_flash_init \
    ((int (*)(uint8_t sel, const struct sf_ctrl_cfg_type *p_sf_ctrl_cfg, const struct sf_ctrl_bank2_cfg *p_bank2_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_flash_init])

#define romapi_bflb_sf_cfg_sbus2_flash_init \
    ((int (*)(uint8_t sel, const struct sf_ctrl_bank2_cfg *p_bank2_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_sbus2_flash_init])

#define romapi_bflb_sf_ctrl_enable \
    ((void (*)(const struct sf_ctrl_cfg_type *cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_enable])

#define romapi_bflb_sf_ctrl_set_io_delay \
    ((void (*)(uint8_t pad, uint8_t do_delay, uint8_t di_delay, uint8_t oe_delay)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_io_delay])

#define romapi_bflb_sf_ctrl_get_io_delay \
    ((void (*)(uint8_t pad, uint8_t *do_delay, uint8_t *di_delay, uint8_t *oe_delay)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_io_delay])

#define romapi_bflb_sf_ctrl_set_cs_clk_delay \
    ((void (*)(uint8_t pad, uint8_t cs_delay, uint8_t clk_delay)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_cs_clk_delay])

#define romapi_bflb_sf_ctrl_get_cs_clk_delay \
    ((void (*)(uint8_t pad, uint8_t *cs_delay, uint8_t *clk_delay)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_cs_clk_delay])

#define romapi_bflb_sf_ctrl_set_flash_io_cs_clk_delay \
    ((void (*)(struct bflb_sf_ctrl_io_cs_clk_delay_cfg cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_flash_io_cs_clk_delay])

#define romapi_bflb_sf_ctrl_get_flash_io_cs_clk_delay \
    ((void (*)(struct bflb_sf_ctrl_io_cs_clk_delay_cfg *cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_flash_io_cs_clk_delay])

#define romapi_bflb_sf_ctrl_set_dqs_delay \
    ((void (*)(uint8_t pad, uint8_t dodelay, uint8_t didelay, uint8_t oedelay)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_dqs_delay])

#define romapi_bflb_sf_ctrl_bank2_enable \
    ((void (*)(const struct sf_ctrl_bank2_cfg *bank2_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_bank2_enable])

#define romapi_bflb_sf_ctrl_sbus2_hold_sram \
    ((void (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_sbus2_hold_sram])

#define romapi_bflb_sf_ctrl_sbus2_release_sram \
    ((void (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_sbus2_release_sram])

#define romapi_bflb_sf_ctrl_sbus2_replace \
    ((void (*)(uint8_t pad)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_sbus2_replace])

#define romapi_bflb_sf_ctrl_sbus2_revoke_replace \
    ((void (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_sbus2_revoke_replace])

#define romapi_bflb_sf_ctrl_sbus2_set_delay \
    ((void (*)(uint8_t clk_delay, uint8_t rx_clk_invert)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_sbus2_set_delay])

#define romapi_bflb_sf_ctrl_remap_set \
    ((void (*)(uint8_t remap, uint8_t lock)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_remap_set])

#define romapi_bflb_sf_ctrl_32bits_addr_en \
    ((void (*)(uint8_t en32_bits_addr)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_32bits_addr_en])

#define romapi_bflb_sf_ctrl_get_clock_delay \
    ((uint8_t (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_clock_delay])

#define romapi_bflb_sf_ctrl_set_clock_delay \
    ((void (*)(uint8_t delay)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_clock_delay])

#define romapi_bflb_sf_ctrl_get_wrap_queue_value \
    ((uint8_t (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_wrap_queue_value])

#define romapi_bflb_sf_ctrl_cmds_set \
    ((void (*)(struct sf_ctrl_cmds_cfg *cmds_cfg, uint8_t bank)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_cmds_set])

#define romapi_bflb_sf_ctrl_select_pad \
    ((void (*)(uint8_t sel)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_select_pad])

#define romapi_bflb_sf_ctrl_sbus_select_bank \
    ((void (*)(uint8_t bank)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_sbus_select_bank])

#define romapi_bflb_sf_ctrl_set_owner \
    ((void (*)(uint8_t owner)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_owner])

#define romapi_bflb_sf_ctrl_get_owner \
    ((uint8_t (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_owner])

#define romapi_bflb_sf_ctrl_set_owner_flag \
    ((void (*)(uint8_t owner)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_owner_flag])

#define romapi_bflb_sf_ctrl_get_owner_flag \
    ((uint8_t (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_owner_flag])

#define romapi_bflb_sf_ctrl_disable \
    ((void (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_disable])

#define romapi_bflb_sf_ctrl_aes_enable_be \
    ((void (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_enable_be])

#define romapi_bflb_sf_ctrl_aes_enable_le \
    ((void (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_enable_le])

#define romapi_bflb_sf_ctrl_aes_set_region \
    ((void (*)(uint8_t region, uint8_t enable, uint8_t hw_key, uint32_t start_addr, uint32_t end_addr, uint8_t locked)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_region])

#define romapi_bflb_sf_ctrl_aes_set_key \
    ((void (*)(uint8_t region, uint8_t *key, uint8_t key_type)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_key])

#define romapi_bflb_sf_ctrl_aes_set_key_be \
    ((void (*)(uint8_t region, uint8_t *key, uint8_t key_type)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_key_be])

#define romapi_bflb_sf_ctrl_aes_set_iv \
    ((void (*)(uint8_t region, uint8_t *iv, uint32_t addr_offset)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_iv])

#define romapi_bflb_sf_ctrl_aes_set_iv_be \
    ((void (*)(uint8_t region, uint8_t *iv, uint32_t addr_offset)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_iv_be])

#define romapi_bflb_sf_ctrl_aes_set_region_offset \
    ((void (*)(uint8_t region, uint32_t addr_offset)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_region_offset])

#define romapi_bflb_sf_ctrl_aes_xts_set_key \
    ((void (*)(uint8_t region, uint8_t *key, uint8_t key_type)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_xts_set_key])

#define romapi_bflb_sf_ctrl_aes_xts_set_key_be \
    ((void (*)(uint8_t region, uint8_t *key, uint8_t key_type)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_xts_set_key_be])

#define romapi_bflb_sf_ctrl_aes_xts_set_iv \
    ((void (*)(uint8_t region, uint8_t *iv, uint32_t addr_offset)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_xts_set_iv])

#define romapi_bflb_sf_ctrl_aes_xts_set_iv_be \
    ((void (*)(uint8_t region, uint8_t *iv, uint32_t addr_offset)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_xts_set_iv_be])

#define romapi_bflb_sf_ctrl_aes_set_mode \
    ((void (*)(uint8_t mode)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_mode])

#define romapi_bflb_sf_ctrl_aes_enable \
    ((void (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_enable])

#define romapi_bflb_sf_ctrl_aes_disable \
    ((void (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_disable])

#define romapi_bflb_sf_ctrl_is_aes_enable \
    ((uint8_t (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_is_aes_enable])

#define romapi_bflb_sf_ctrl_set_flash_image_offset \
    ((void (*)(uint32_t addr_offset, uint8_t group, uint8_t bank)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_flash_image_offset])

#define romapi_bflb_sf_ctrl_get_flash_image_offset \
    ((uint32_t (*)(uint8_t group, uint8_t bank)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_flash_image_offset])

#define romapi_bflb_sf_ctrl_lock_flash_image_offset \
    ((void (*)(uint8_t lock)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_lock_flash_image_offset])

#define romapi_bflb_sf_ctrl_select_clock \
    ((void (*)(uint8_t sahb_sram_sel)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_select_clock])

#define romapi_bflb_sf_ctrl_sendcmd \
    ((void (*)(struct sf_ctrl_cmd_cfg_type *cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_sendcmd])

#define romapi_bflb_sf_ctrl_disable_wrap_access \
    ((void (*)(uint8_t disable)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_disable_wrap_access])

#define romapi_bflb_sf_ctrl_xip_set \
    ((void (*)(struct sf_ctrl_cmd_cfg_type *cfg, uint8_t cmd_valid)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_xip_set])

#define romapi_bflb_sf_ctrl_get_busy_state \
    ((uint8_t (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_busy_state])

#define romapi_bflb_sf_ctrl_aes_get_iv_be \
    ((void (*)(uint8_t region, uint8_t *iv)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_get_iv_be])

#define romapi_bflb_sf_ctrl_aes_get_iv_le \
    ((void (*)(uint8_t region, uint8_t *iv)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_get_iv_le])

#define romapi_bflb_sf_ctrl_aes_set_decrypt_region_be \
    ((int32_t (*)(struct bflb_sf_ctrl_decrypt_type *parm)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_decrypt_region_be])

#define romapi_bflb_sf_ctrl_aes_set_decrypt_region_le \
    ((int32_t (*)(struct bflb_sf_ctrl_decrypt_type *parm)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_decrypt_region_le])

#define romapi_bflb_sflash_init \
    ((void (*)(const struct sf_ctrl_cfg_type *p_sf_ctrl_cfg, const struct sf_ctrl_bank2_cfg *p_bank2_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_init])

#define romapi_bflb_sflash_set_spi_mode \
    ((int (*)(uint8_t mode)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_set_spi_mode])

#define romapi_bflb_sflash_read_reg \
    ((int (*)(spi_flash_cfg_type *flash_cfg, uint8_t reg_index, uint8_t *reg_value, uint8_t reg_len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_read_reg])

#define romapi_bflb_sflash_write_reg \
    ((int (*)(spi_flash_cfg_type *flash_cfg, uint8_t reg_index, uint8_t *reg_value, uint8_t reg_len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_write_reg])

#define romapi_bflb_sflash_read_reg_with_cmd \
    ((int (*)(spi_flash_cfg_type *flash_cfg, uint8_t read_reg_cmd, uint8_t *reg_value, uint8_t reg_len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_read_reg_with_cmd])

#define romapi_bflb_sflash_write_reg_with_cmd \
    ((int (*)(spi_flash_cfg_type *flash_cfg, uint8_t write_reg_cmd, uint8_t *reg_value, uint8_t reg_len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_write_reg_with_cmd])

#define romapi_bflb_sflash_busy \
    ((int (*)(spi_flash_cfg_type *flash_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_busy])

#define romapi_bflb_sflash_write_enable \
    ((int (*)(spi_flash_cfg_type *flash_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_write_enable])

#define romapi_bflb_sflash_qspi_enable \
    ((int (*)(spi_flash_cfg_type *flash_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_qspi_enable])

#define romapi_bflb_sflash_qspi_disable \
    ((int (*)(spi_flash_cfg_type *flash_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_qspi_disable])

#define romapi_bflb_sflash_volatile_reg_write_enable \
    ((void (*)(spi_flash_cfg_type *flash_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_volatile_reg_write_enable])

#define romapi_bflb_sflash_chip_erase \
    ((int (*)(spi_flash_cfg_type *flash_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_chip_erase])

#define romapi_bflb_sflash_sector_erase \
    ((int (*)(spi_flash_cfg_type *flash_cfg, uint32_t sector_num)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_sector_erase])

#define romapi_bflb_sflash_blk32_erase \
    ((int (*)(spi_flash_cfg_type *flash_cfg, uint32_t blk_num)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_blk32_erase])

#define romapi_bflb_sflash_blk64_erase \
    ((int (*)(spi_flash_cfg_type *flash_cfg, uint32_t blk_num)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_blk64_erase])

#define romapi_bflb_sflash_erase \
    ((int (*)(spi_flash_cfg_type *flash_cfg, uint32_t start_addr, uint32_t end_addr)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_erase])

#define romapi_bflb_sflash_get_uniqueid \
    ((void (*)(uint8_t *data, uint8_t id_len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_get_uniqueid])

#define romapi_bflb_sflash_get_jedecid \
    ((void (*)(spi_flash_cfg_type *flash_cfg, uint8_t *data)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_get_jedecid])

#define romapi_bflb_sflash_get_deviceid \
    ((void (*)(uint8_t *data, uint8_t is_32bits_addr)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_get_deviceid])

#define romapi_bflb_sflash_powerdown \
    ((void (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_powerdown])

#define romapi_bflb_sflash_release_powerdown \
    ((void (*)(spi_flash_cfg_type *flash_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_release_powerdown])

#define romapi_bflb_sflash_restore_from_powerdown \
    ((int (*)(spi_flash_cfg_type *flash_cfg, uint8_t flash_cont_read, uint8_t bank)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_restore_from_powerdown])

#define romapi_bflb_sflash_set_burst_wrap \
    ((void (*)(spi_flash_cfg_type *flash_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_set_burst_wrap])

#define romapi_bflb_sflash_disable_burst_wrap \
    ((void (*)(spi_flash_cfg_type *flash_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_disable_burst_wrap])

#define romapi_bflb_sflash_set_32bits_addr_mode \
    ((int (*)(spi_flash_cfg_type *flash_cfg, uint8_t en_32bits_addr)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_set_32bits_addr_mode])

#define romapi_bflb_sflash_software_reset \
    ((int (*)(spi_flash_cfg_type *flash_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_software_reset])

#define romapi_bflb_sflash_reset_continue_read \
    ((void (*)(spi_flash_cfg_type *flash_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_reset_continue_read])

#define romapi_bflb_sflash_set_xip_cfg \
    ((int (*)(spi_flash_cfg_type *flash_cfg, uint8_t io_mode, uint8_t cont_read, uint32_t addr, uint32_t len, uint8_t bank)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_set_xip_cfg])

#define romapi_bflb_sflash_xip_read_enable \
    ((int (*)(spi_flash_cfg_type *flash_cfg, uint8_t io_mode, uint8_t cont_read, uint8_t bank)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_xip_read_enable])

#define romapi_bflb_sflash_xip_read_disable \
    ((void (*)(void)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_xip_read_disable])

#define romapi_bflb_sflash_rcv_enable \
    ((int (*)(spi_flash_cfg_type *flash_cfg, uint8_t r_cmd, uint8_t w_cmd, uint8_t bit_pos)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_rcv_enable])

#define romapi_bflb_sflash_erase_security_register \
    ((int (*)(spi_flash_cfg_type *flash_cfg, struct sflash_sec_reg_cfg *p_sec_reg_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_erase_security_register])

#define romapi_bflb_sflash_program_security_register \
    ((int (*)(spi_flash_cfg_type *flash_cfg, struct sflash_sec_reg_cfg *p_sec_reg_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_program_security_register])

#define romapi_bflb_sflash_read_security_register \
    ((int (*)(struct sflash_sec_reg_cfg *p_sec_reg_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_read_security_register])

#define romapi_bflb_sflash_clear_status_register \
    ((int (*)(spi_flash_cfg_type *flash_cfg)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_clear_status_register])

#define romapi_bflb_sflash_read \
    ((int (*)(spi_flash_cfg_type *flash_cfg, uint8_t io_mode, uint8_t cont_read, uint32_t addr, uint8_t *data, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_read])

#define romapi_bflb_sflash_program \
    ((int (*)(spi_flash_cfg_type *flash_cfg, uint8_t io_mode, uint32_t addr, uint8_t *data, uint32_t len)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_program])

#define romapi_bflb_xip_sflash_state_save \
    ((int (*)(spi_flash_cfg_type *p_flash_cfg, uint32_t *offset, uint8_t group, uint8_t bank)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_state_save])

#define romapi_bflb_xip_sflash_state_restore \
    ((int (*)(spi_flash_cfg_type *p_flash_cfg, uint32_t offset, uint8_t group, uint8_t bank)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_state_restore])

#define romapi_bflb_xip_sflash_erase_need_lock \
    ((int (*)(spi_flash_cfg_type *p_flash_cfg, uint32_t start_addr, int len, uint8_t group, uint8_t bank)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_erase_need_lock])

#define romapi_bflb_xip_sflash_write_need_lock \
    ((int (*)(spi_flash_cfg_type *p_flash_cfg, uint32_t addr, uint8_t *data, uint32_t len, uint8_t group, uint8_t bank)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_write_need_lock])

#define romapi_bflb_xip_sflash_read_need_lock \
    ((int (*)(spi_flash_cfg_type *p_flash_cfg, uint32_t addr, uint8_t *data, uint32_t len, uint8_t group, uint8_t bank)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_read_need_lock])

#define romapi_bflb_xip_sflash_get_jedecid_need_lock \
    ((int (*)(spi_flash_cfg_type *p_flash_cfg, uint8_t *data, uint8_t group, uint8_t bank)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_get_jedecid_need_lock])

#define romapi_bflb_xip_sflash_get_deviceid_need_lock \
    ((int (*)(spi_flash_cfg_type *p_flash_cfg, uint8_t is_32bits_addr, uint8_t *data, uint8_t group, uint8_t bank)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_get_deviceid_need_lock])

#define romapi_bflb_xip_sflash_get_uniqueid_need_lock \
    ((int (*)(spi_flash_cfg_type *p_flash_cfg, uint8_t *data, uint8_t id_len, uint8_t group, uint8_t bank)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_get_uniqueid_need_lock])

#define romapi_bflb_xip_sflash_clear_status_register_need_lock \
    ((int (*)(spi_flash_cfg_type *p_flash_cfg, uint8_t group, uint8_t bank)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_clear_status_register_need_lock])

#define romapi_bflb_xip_sflash_read_via_cache_need_lock \
    ((int (*)(uint32_t addr, uint8_t *data, uint32_t len, uint8_t group, uint8_t bank)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_read_via_cache_need_lock])

#define romapi_bflb_xip_sflash_opt_enter \
    ((void (*)(uint8_t *aes_enable)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_opt_enter])

#define romapi_bflb_xip_sflash_opt_exit \
    ((void (*)(uint8_t aes_enable)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_opt_exit])

#endif
