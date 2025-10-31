#ifndef __BFLB_LHAL_ROMDRIVER_H__
#define __BFLB_LHAL_ROMDRIVER_H__

#define BFLB_LHAL_ROMAPI_INDEX_MAX   (512)

typedef enum {
    BFLB_LHAL_ROMAPI_INDEX_VERSION      = 0,
    BFLB_LHAL_ROMAPI_INDEX_RSVD_0       = 1,
    BFLB_LHAL_ROMAPI_INDEX_RSVD_1       = 2,
    BFLB_LHAL_ROMAPI_INDEX_RSVD_LAST    = 3,

    BFLB_LHAL_ROMAPI_INDEX_arch_strcmp                                       = 4,
    BFLB_LHAL_ROMAPI_INDEX_arch_memcpy                                       = 5,
    BFLB_LHAL_ROMAPI_INDEX_arch_memcpy4                                      = 6,
    BFLB_LHAL_ROMAPI_INDEX_arch_memcpy_fast                                  = 7,
    BFLB_LHAL_ROMAPI_INDEX_arch_memset                                       = 8,
    BFLB_LHAL_ROMAPI_INDEX_arch_memset4                                      = 9,
    BFLB_LHAL_ROMAPI_INDEX_arch_memcmp                                       = 10,
    BFLB_LHAL_ROMAPI_INDEX_bflb_check_cache_addr_aligned                     = 11,
    BFLB_LHAL_ROMAPI_INDEX_bflb_soft_crc16                                   = 12,
    BFLB_LHAL_ROMAPI_INDEX_bflb_soft_crc32_ex                                = 13,
    BFLB_LHAL_ROMAPI_INDEX_bflb_soft_crc32                                   = 14,
    BFLB_LHAL_ROMAPI_INDEX_bflb_get_app_version_from_efuse                   = 15,
    BFLB_LHAL_ROMAPI_INDEX_bflb_set_app_version_to_efuse                     = 16,
    BFLB_LHAL_ROMAPI_INDEX_bflb_get_boot2_version_from_efuse                 = 17,
    BFLB_LHAL_ROMAPI_INDEX_bflb_set_boot2_version_to_efuse                   = 18,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_init                             = 19,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_deinit                           = 20,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_lli_reload                       = 21,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_lli_link_head                    = 22,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_start                            = 23,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_stop                             = 24,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_isbusy                           = 25,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_tcint_mask                       = 26,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_get_tcint_status                 = 27,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dma_channel_tcint_clear                      = 28,
    BFLB_LHAL_ROMAPI_INDEX_bflb_rx_cycle_dma_process                         = 29,
    BFLB_LHAL_ROMAPI_INDEX_bflb_dma_feature_control                          = 30,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_busy                                 = 31,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_autoload_done                        = 32,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_set_para                             = 33,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_write_direct                         = 34,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_read_direct                          = 35,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_read_common_trim                     = 36,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_write_common_trim                    = 37,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_is_all_bits_zero                     = 38,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_get_byte_zero_cnt                    = 39,
    BFLB_LHAL_ROMAPI_INDEX_bflb_ef_ctrl_get_trim_parity                      = 40,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_init                                    = 41,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_deinit                                  = 42,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_output_value_mode_enable                = 43,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin0_31_output                          = 44,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin32_63_output                         = 45,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_set                                     = 46,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin0_31_set                             = 47,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin32_63_set                            = 48,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_reset                                   = 49,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin0_31_reset                           = 50,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin32_63_reset                          = 51,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_read                                    = 52,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin0_31_read                            = 53,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_pin32_63_read                           = 54,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_int_init                                = 55,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_int_mask                                = 56,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_get_intstatus                           = 57,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_get_intstatus_pin0_31                   = 58,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_get_intstatus_pin32_63                  = 59,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_int_clear                               = 60,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_int_clear_pin0_31                       = 61,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_int_clear_pin32_63                      = 62,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_uart_init                               = 63,
    BFLB_LHAL_ROMAPI_INDEX_bflb_gpio_feature_control                         = 64,
    BFLB_LHAL_ROMAPI_INDEX_bflb_irq_restore                                  = 65,
    BFLB_LHAL_ROMAPI_INDEX_bflb_irq_enable                                   = 66,
    BFLB_LHAL_ROMAPI_INDEX_bflb_irq_disable                                  = 67,
    BFLB_LHAL_ROMAPI_INDEX_bflb_irq_set_pending                              = 68,
    BFLB_LHAL_ROMAPI_INDEX_bflb_irq_clear_pending                            = 69,
    BFLB_LHAL_ROMAPI_INDEX_bflb_irq_set_nlbits                               = 70,
    BFLB_LHAL_ROMAPI_INDEX_bflb_irq_set_priority                             = 71,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_icache_enable                            = 72,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_icache_disable                           = 73,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_icache_invalid_all                       = 74,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_icache_invalid_range                     = 75,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_enable                            = 76,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_disable                           = 77,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_clean_all                         = 78,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_invalidate_all                    = 79,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_clean_invalidate_all              = 80,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_clean_range                       = 81,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_invalidate_range                  = 82,
    BFLB_LHAL_ROMAPI_INDEX_bflb_l1c_dcache_clean_invalidate_range            = 83,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mtimer_delay_us                              = 84,
    BFLB_LHAL_ROMAPI_INDEX_bflb_mtimer_delay_ms                              = 85,
    BFLB_LHAL_ROMAPI_INDEX_bflb_rtc_disable                                  = 86,
    BFLB_LHAL_ROMAPI_INDEX_bflb_rtc_set_time                                 = 87,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_init                                    = 88,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_deinit                                  = 89,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_enable                                  = 90,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_disable                                 = 91,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_link_txdma                              = 92,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_link_rxdma                              = 93,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_putchar                                 = 94,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_getchar                                 = 95,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_put                                     = 96,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_put_block                               = 97,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_get                                     = 98,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_wait_tx_done                            = 99,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_txready                                 = 100,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_txempty                                 = 101,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_rxavailable                             = 102,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_txint_mask                              = 103,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_rxint_mask                              = 104,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_errint_mask                             = 105,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_get_intstatus                           = 106,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_int_clear                               = 107,
    BFLB_LHAL_ROMAPI_INDEX_bflb_uart_feature_control                         = 108,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wdg_init                                     = 109,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wdg_start                                    = 110,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wdg_stop                                     = 111,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wdg_get_countervalue                         = 112,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wdg_set_countervalue                         = 113,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wdg_reset_countervalue                       = 114,
    BFLB_LHAL_ROMAPI_INDEX_bflb_wdg_compint_clear                            = 115,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_callapi_before                  = 116,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_callapi_after                   = 117,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_get_param                       = 118,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_region_foreach                  = 119,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_get_lockbits                    = 120,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_set_lockbits                    = 121,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_get_locked                      = 122,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_set_locked                      = 123,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_get_info_by_idx                 = 124,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_valid_by_idx                    = 125,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_read_by_idx                     = 126,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_write_by_idx                    = 127,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_erase_by_idx                    = 128,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_get_info                        = 129,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_valid                           = 130,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_read                            = 131,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_write                           = 132,
    BFLB_LHAL_ROMAPI_INDEX_bflb_flash_secreg_erase                           = 133,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_init_ext_flash_gpio                   = 134,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_deinit_ext_flash_gpio                 = 135,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_init_flash_gpio                       = 136,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_init_flash2_gpio                      = 137,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_flash_identify                        = 138,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_get_flash_cfg_need_lock               = 139,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_flash_init                            = 140,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_cfg_sbus2_flash_init                      = 141,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_enable                               = 142,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_io_delay                         = 143,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_io_delay                         = 144,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_cs_clk_delay                     = 145,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_cs_clk_delay                     = 146,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_flash_io_cs_clk_delay            = 147,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_flash_io_cs_clk_delay            = 148,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_dqs_delay                        = 149,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_bank2_enable                         = 150,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_sbus2_hold_sram                      = 151,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_sbus2_release_sram                   = 152,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_sbus2_replace                        = 153,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_sbus2_revoke_replace                 = 154,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_sbus2_set_delay                      = 155,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_remap_set                            = 156,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_32bits_addr_en                       = 157,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_clock_delay                      = 158,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_clock_delay                      = 159,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_wrap_queue_value                 = 160,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_cmds_set                             = 161,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_select_pad                           = 162,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_sbus_select_bank                     = 163,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_owner                            = 164,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_owner                            = 165,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_owner_flag                       = 166,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_owner_flag                       = 167,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_disable                              = 168,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_enable_be                        = 169,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_enable_le                        = 170,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_region                       = 171,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_key                          = 172,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_key_be                       = 173,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_iv                           = 174,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_iv_be                        = 175,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_region_offset                = 176,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_xts_set_key                      = 177,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_xts_set_key_be                   = 178,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_xts_set_iv                       = 179,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_xts_set_iv_be                    = 180,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_mode                         = 181,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_enable                           = 182,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_disable                          = 183,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_is_aes_enable                        = 184,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_set_flash_image_offset               = 185,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_flash_image_offset               = 186,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_lock_flash_image_offset              = 187,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_select_clock                         = 188,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_sendcmd                              = 189,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_disable_wrap_access                  = 190,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_xip_set                              = 191,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_get_busy_state                       = 192,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_get_iv_be                        = 193,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_get_iv_le                        = 194,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_decrypt_region_be            = 195,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sf_ctrl_aes_set_decrypt_region_le            = 196,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_init                                  = 197,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_set_spi_mode                          = 198,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_read_reg                              = 199,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_write_reg                             = 200,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_read_reg_with_cmd                     = 201,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_write_reg_with_cmd                    = 202,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_busy                                  = 203,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_write_enable                          = 204,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_qspi_enable                           = 205,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_qspi_disable                          = 206,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_volatile_reg_write_enable             = 207,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_chip_erase                            = 208,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_sector_erase                          = 209,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_blk32_erase                           = 210,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_blk64_erase                           = 211,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_erase                                 = 212,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_get_uniqueid                          = 213,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_get_jedecid                           = 214,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_get_deviceid                          = 215,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_powerdown                             = 216,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_release_powerdown                     = 217,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_restore_from_powerdown                = 218,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_set_burst_wrap                        = 219,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_disable_burst_wrap                    = 220,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_set_32bits_addr_mode                  = 221,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_software_reset                        = 222,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_reset_continue_read                   = 223,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_set_xip_cfg                           = 224,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_xip_read_enable                       = 225,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_xip_read_disable                      = 226,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_rcv_enable                            = 227,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_erase_security_register               = 228,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_program_security_register             = 229,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_read_security_register                = 230,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_clear_status_register                 = 231,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_read                                  = 232,
    BFLB_LHAL_ROMAPI_INDEX_bflb_sflash_program                               = 233,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_state_save                        = 234,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_state_restore                     = 235,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_erase_need_lock                   = 236,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_write_need_lock                   = 237,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_read_need_lock                    = 238,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_get_jedecid_need_lock             = 239,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_get_deviceid_need_lock            = 240,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_get_uniqueid_need_lock            = 241,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_clear_status_register_need_lock   = 242,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_read_via_cache_need_lock          = 243,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_opt_enter                         = 244,
    BFLB_LHAL_ROMAPI_INDEX_bflb_xip_sflash_opt_exit                          = 245,
    BFLB_LHAL_ROMAPI_INDEX_FUNC_EMPTY_START,

    BFLB_LHAL_ROMAPI_INDEX_FUNC_EMPTY_END    = (BFLB_LHAL_ROMAPI_INDEX_MAX - 1)

} BFLB_LHAL_ROMAPI_INDEX_e;

#ifndef BFLB_LHAL_ROMAPI_TABLE
#define BFLB_LHAL_ROMAPI_TABLE      (0x90018800)
#endif
#define ROM_APITABLE                ((uint32_t *)BFLB_LHAL_ROMAPI_TABLE)


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

#define romapi_bflb_mtimer_delay_us \
    ((void (*)(uint32_t time)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mtimer_delay_us])

#define romapi_bflb_mtimer_delay_ms \
    ((void (*)(uint32_t time)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_mtimer_delay_ms])

#define romapi_bflb_rtc_disable \
    ((void (*)(struct bflb_device_s *dev)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_rtc_disable])

#define romapi_bflb_rtc_set_time \
    ((void (*)(struct bflb_device_s *dev, uint64_t time)) \
    ROM_APITABLE[BFLB_LHAL_ROMAPI_INDEX_bflb_rtc_set_time])

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
