# Examples

This directory contains the official Bouffalo SDK example projects. They
demonstrate SDK features and provide reference code that can be adapted for
applications.

## Example Layout

Examples are grouped by feature area. A category may contain one or more
standalone projects; build a project from the directory that contains its
`Makefile`.

| Directory | Description |
| --- | --- |
| [`audio`](audio) | Audio codec, playback, and ALSA examples. |
| [`bflb_block_pool`](bflb_block_pool) | BFLB block-pool memory-management example. |
| [`bflog`](bflog) | BFLB logging examples. |
| [`bl3141`](bl3141) | BL3141 peripheral examples. |
| [`bl618dg_dualcore`](bl618dg_dualcore) | BL618DG AP/NP dual-core examples. |
| [`boot2_isp`](boot2_isp) | Boot2 ISP examples. |
| [`btble`](btble) | Bluetooth Classic, Bluetooth LE, Mesh, and related examples. |
| [`cherryusb`](cherryusb) | CherryUSB examples. |
| [`coex`](coex) | Wireless coexistence examples. |
| [`coremark_v1.01`](coremark_v1.01) | CoreMark benchmark. |
| [`crash`](crash) | Crash handling and diagnostic examples. |
| [`dhrystone`](dhrystone) | Dhrystone benchmark. |
| [`dsp`](dsp) | Digital signal processing examples. |
| [`easyflash`](easyflash) | EasyFlash examples. |
| [`fatfs`](fatfs) | FAT file-system examples. |
| [`freertos`](freertos) | FreeRTOS examples. |
| [`helloworld`](helloworld) | Minimal getting-started example. |
| [`littlefs`](littlefs) | LittleFS file-system examples. |
| [`lmac154`](lmac154) | Low-MAC IEEE 802.15.4 examples. |
| [`log_dbg_assert`](log_dbg_assert) | Log, debug, and assertion examples. |
| [`lua`](lua) | Lua runtime examples. |
| [`lvgl`](lvgl) | LVGL graphics and UI examples. |
| [`mbedtls`](mbedtls) | Mbed TLS security examples. |
| [`memheap`](memheap) | Memory heap examples. |
| [`memtester`](memtester) | Memory test examples. |
| [`mfg_test`](mfg_test) | Manufacturing-test examples. |
| [`mquickjs`](mquickjs) | QuickJS runtime examples. |
| [`openamp`](openamp) | OpenAMP inter-core communication examples. |
| [`partition`](partition) | Flash partition examples. |
| [`peripherals`](peripherals) | Driver examples for on-chip peripherals. |
| [`pmu`](pmu) | Power-management and low-power examples. |
| [`posix`](posix) | POSIX compatibility examples. |
| [`printf`](printf) | Formatted-output example. |
| [`shell`](shell) | Command-line shell examples. |
| [`solution`](solution) | Integrated application solutions. |
| [`thread`](thread) | OpenThread examples. |
| [`tpjdec_disp`](tpjdec_disp) | Tiny JPEG decoder display example. |
| [`wifi`](wifi) | Wi-Fi connectivity, protocol, coexistence, and test examples. |
| [`wlan`](wlan) | WLAN firmware examples. |
| [`zigbee`](zigbee) | Zigbee coordinator, device, and Thread integration examples. |

## Application Index

The following index lists every source example with a tracked `Makefile`
build entry point. Links identify the directory from which the project can be
built. Generated `build/` directories and CMake-only component directories
are intentionally not listed as applications.

### General Applications

| Directory | Applications |
| --- | --- |
| [`audio`](audio) | [`codec`](audio/codec), [`codec_HFP`](audio/codec_HFP), [`minialsa`](audio/minialsa), [`player`](audio/player), [`player_bt`](audio/player_bt), [`player_es8388`](audio/player_es8388), [`player_mp3`](audio/player_mp3), [`player_wifi`](audio/player_wifi) |
| [`bflb_block_pool`](bflb_block_pool) | [`bflb_block_pool`](bflb_block_pool) |
| [`bflog`](bflog) | [`barebone_sync`](bflog/barebone_sync), [`freertos_async`](bflog/freertos_async) |
| [`bl3141`](bl3141) | [`bl3141`](bl3141/bl3141), [`bl3141_cli`](bl3141/bl3141_cli) |
| [`bl618dg_dualcore`](bl618dg_dualcore) | [`bl618dg_dualcore`](bl618dg_dualcore), [`helloworld_ap`](bl618dg_dualcore/helloworld_ap), [`helloworld_np`](bl618dg_dualcore/helloworld_np) |
| [`boot2_isp`](boot2_isp) | [`boot2_isp`](boot2_isp) |
| [`btble`](btble) | [`a2dp_player`](btble/a2dp_player), [`adv`](btble/adv), [`bl616cl_rom_test`](btble/bl616cl_rom_test), [`bl702_demo_ble_peripheral`](btble/bl702_demo_ble_peripheral), [`blemesh_cli`](btble/blemesh_cli), [`blemesh_simple`](btble/blemesh_simple), [`btble_autopts`](btble/btble_autopts), [`btble_cli`](btble/btble_cli), [`btblecontroller_test`](btble/btblecontroller_test), [`btble_host_tl`](btble/btble_host_tl), [`btble_nimble`](btble/btble_nimble), [`btble_pts`](btble/btble_pts), [`btble_vela`](btble/btble_vela), [`central`](btble/central), [`nearby`](btble/nearby), [`peripheral`](btble/peripheral), [`remote_control`](btble/remote_control), [`yls_bl616_passthrough`](btble/yls_bl616_passthrough) |
| [`cherryusb`](cherryusb) | [`cherryusb_cli`](cherryusb/cherryusb_cli) |
| [`coex`](coex) | [`wifi_a2dpsource`](coex/wifi_a2dpsource), [`wifi_bz_coexm`](coex/wifi_bz_coexm) |
| [`coremark_v1.01`](coremark_v1.01) | [`coremark_v1.01`](coremark_v1.01) |
| [`crash`](crash) | [`crash`](crash) |
| [`dhrystone`](dhrystone) | [`dhrystone`](dhrystone) |
| [`dsp`](dsp) | [`dsp`](dsp) |
| [`easyflash`](easyflash) | [`easyflash`](easyflash) |
| [`fatfs`](fatfs) | [`fatfs`](fatfs) |
| [`freertos`](freertos) | [`freertos`](freertos) |
| [`helloworld`](helloworld) | [`helloworld`](helloworld) |
| [`littlefs`](littlefs) | [`littlefs`](littlefs) |
| [`lmac154`](lmac154) | [`lmac154`](lmac154/lmac154), [`lmac154_cli`](lmac154/lmac154_cli) |
| [`log_dbg_assert`](log_dbg_assert) | [`log_dbg_assert`](log_dbg_assert) |
| [`lua`](lua) | [`lua`](lua) |
| [`lvgl`](lvgl) | [`lvgl_v8`](lvgl/lvgl_v8), [`lvgl_v8_with_osd`](lvgl/lvgl_v8_with_osd), [`lvgl_v9`](lvgl/lvgl_v9), [`lvgl_v9_with_osd`](lvgl/lvgl_v9_with_osd) |
| [`mbedtls`](mbedtls) | [`mbedtls`](mbedtls) |
| [`memheap`](memheap) | [`memheap`](memheap) |
| [`memtester`](memtester) | [`memtester`](memtester), [`memtester/memtester`](memtester/memtester) |
| [`mfg_test`](mfg_test) | [`mfg_test`](mfg_test) |
| [`mquickjs`](mquickjs) | [`mquickjs`](mquickjs) |
| [`openamp`](openamp) | [`openamp`](openamp), [`openamp_ap`](openamp/openamp_ap), [`openamp_np`](openamp/openamp_np) |
| [`partition`](partition) | [`partition`](partition) |
| [`posix`](posix) | [`freertos`](posix/freertos), [`freertos_posix`](posix/freertos_posix) |
| [`printf`](printf) | [`printf`](printf) |
| [`shell`](shell) | [`shell_no_os`](shell/shell_no_os), [`shell_os`](shell/shell_os) |
| [`thread`](thread) | [`openthread_cli`](thread/openthread_cli), [`openthread_eth_br`](thread/openthread_eth_br), [`openthread_ncp`](thread/openthread_ncp), [`openthread_wifi_br`](thread/openthread_wifi_br) |
| [`tpjdec_disp`](tpjdec_disp) | [`tpjdec_disp`](tpjdec_disp) |
| [`wlan`](wlan) | [`wlan_fw`](wlan/wlan_fw) |
| [`zigbee`](zigbee) | [`zigbee_coordinator`](zigbee/zigbee_coordinator), [`zigbee_light`](zigbee/zigbee_light), [`zigbee_thread`](zigbee/zigbee_thread) |

### Wi-Fi Applications

| Directory | Applications |
| --- | --- |
| [`wifi`](wifi) | [`ap_sta_bridge`](wifi/ap_sta_bridge), [`macsw_bare`](wifi/macsw_bare), [`nethub`](wifi/nethub), [`rf_ota`](wifi/rf_ota), [`sdh_wifi`](wifi/sdh_wifi), [`sdio_wifi`](wifi/sdio_wifi), [`spi_wifi`](wifi/spi_wifi), [`uart_wifi`](wifi/uart_wifi), [`usb_wifi`](wifi/usb_wifi), [`wfa`](wifi/wfa), [`wfa_dualcore`](wifi/wfa_dualcore) |
| [`wifi/coex`](wifi/coex) | [`wifi_ble`](wifi/coex/wifi_ble) |
| [`wifi/p2p`](wifi/p2p) | [`wifi_p2p`](wifi/p2p/wifi_p2p), [`wifi_p2p_high_perf`](wifi/p2p/wifi_p2p_high_perf) |
| [`wifi/sta`](wifi/sta) | [`aws_iot`](wifi/sta/aws_iot), [`dualcore`](wifi/sta/dualcore), [`wifi_ap`](wifi/sta/dualcore/wifi_ap), [`wifi_np`](wifi/sta/dualcore/wifi_np), [`http_restful_api`](wifi/sta/http_restful_api), [`matter`](wifi/sta/matter), [`smartconfig_ble`](wifi/sta/smartconfig_ble), [`smartconfig_softap`](wifi/sta/smartconfig_softap), [`websocket`](wifi/sta/websocket), [`wifi_eth_bridge`](wifi/sta/wifi_eth_bridge), [`wifi_gcov_dump`](wifi/sta/wifi_gcov_dump), [`wifi_http`](wifi/sta/wifi_http), [`wifi_https`](wifi/sta/wifi_https), [`wifi_mqtt`](wifi/sta/wifi_mqtt), [`wifi_ota_by_http`](wifi/sta/wifi_ota_by_http), [`wifi_tcp`](wifi/sta/wifi_tcp), [`wifi_tcp_high_perf`](wifi/sta/wifi_tcp_high_perf), [`wifi_udp`](wifi/sta/wifi_udp) |
| [`wifi/wfa_dualcore`](wifi/wfa_dualcore) | [`wfa_ap`](wifi/wfa_dualcore/wfa_ap), [`wfa_np`](wifi/wfa_dualcore/wfa_np) |

### Peripheral Applications

| Directory | Applications |
| --- | --- |
| [`peripherals`](peripherals) | [`anti_rollback`](peripherals/anti_rollback), [`mtimer`](peripherals/mtimer), [`psram`](peripherals/psram), [`ram_speed`](peripherals/ram_speed), [`rtc`](peripherals/rtc) |
| [`peripherals/adc`](peripherals/adc) | [`adc_dma`](peripherals/adc/adc_dma), [`adc_int`](peripherals/adc/adc_int), [`adc_poll`](peripherals/adc/adc_poll), [`adc_poll_diff_mode`](peripherals/adc/adc_poll_diff_mode), [`adc_poll_onechan`](peripherals/adc/adc_poll_onechan), [`adc_tsen`](peripherals/adc/adc_tsen), [`adc_vbat`](peripherals/adc/adc_vbat) |
| [`peripherals/adc_key`](peripherals/adc_key) | [`adc_key_basic`](peripherals/adc_key/adc_key_basic) |
| [`peripherals/adc_v2`](peripherals/adc_v2) | [`adc_basic`](peripherals/adc_v2/adc_basic), [`adc_diff`](peripherals/adc_v2/adc_diff), [`adc_dma`](peripherals/adc_v2/adc_dma), [`adc_inject`](peripherals/adc_v2/adc_inject), [`adc_inject_dma`](peripherals/adc_v2/adc_inject_dma), [`adc_pwm`](peripherals/adc_v2/adc_pwm), [`adc_scan`](peripherals/adc_v2/adc_scan), [`adc_tsen`](peripherals/adc_v2/adc_tsen), [`adc_vbat`](peripherals/adc_v2/adc_vbat) |
| [`peripherals/adc_v3`](peripherals/adc_v3) | [`adc_basic`](peripherals/adc_v3/adc_basic), [`adc_cooperate`](peripherals/adc_v3/adc_cooperate), [`adc_diff`](peripherals/adc_v3/adc_diff), [`adc_dma`](peripherals/adc_v3/adc_dma), [`adc_inject`](peripherals/adc_v3/adc_inject), [`adc_pwm`](peripherals/adc_v3/adc_pwm), [`adc_saturation`](peripherals/adc_v3/adc_saturation), [`adc_scan`](peripherals/adc_v3/adc_scan), [`adc_tsen`](peripherals/adc_v3/adc_tsen), [`adc_vbat`](peripherals/adc_v3/adc_vbat), [`adc_watchdog`](peripherals/adc_v3/adc_watchdog) |
| [`peripherals/audio`](peripherals/audio) | [`auadc_audac_loopback`](peripherals/audio/auadc_audac_loopback), [`auadc_record_to_mem`](peripherals/audio/auadc_record_to_mem), [`audac_play_from_mem`](peripherals/audio/audac_play_from_mem), [`audio_dac_adc`](peripherals/audio/audio_dac_adc), [`audio_dac_i2s_rec`](peripherals/audio/audio_dac_i2s_rec) |
| [`peripherals/cam`](peripherals/cam) | [`cam_crop`](peripherals/cam/cam_crop), [`cam_int`](peripherals/cam/cam_int), [`cam_lcd`](peripherals/cam/cam_lcd), [`cam_lcd_mipi`](peripherals/cam/cam_lcd_mipi), [`cam_normal`](peripherals/cam/cam_normal), [`cam_yuv420`](peripherals/cam/cam_yuv420) |
| [`peripherals/canfd`](peripherals/canfd) | [`canfd_basic`](peripherals/canfd/canfd_basic), [`canfd_cia603`](peripherals/canfd/canfd_cia603), [`canfd_filter`](peripherals/canfd/canfd_filter), [`canfd_interrupt`](peripherals/canfd/canfd_interrupt), [`canfd_loopback`](peripherals/canfd/canfd_loopback), [`canfd_tt_master`](peripherals/canfd/canfd_tt_master), [`canfd_tt_slave`](peripherals/canfd/canfd_tt_slave) |
| [`peripherals/cks`](peripherals/cks) | [`cks_dma`](peripherals/cks/cks_dma), [`cks_normal`](peripherals/cks/cks_normal) |
| [`peripherals/dac`](peripherals/dac) | [`dac_dma`](peripherals/dac/dac_dma), [`dac_polling`](peripherals/dac/dac_polling) |
| [`peripherals/dbi`](peripherals/dbi) | [`lcd_type_b`](peripherals/dbi/lcd_type_b) |
| [`peripherals/dma`](peripherals/dma) | [`dma_normal`](peripherals/dma/dma_normal), [`dma_normal_cycle`](peripherals/dma/dma_normal_cycle), [`dma_reduce_or_add`](peripherals/dma/dma_reduce_or_add) |
| [`peripherals/dpi`](peripherals/dpi) | [`dpi_framebuffer_with_osd`](peripherals/dpi/dpi_framebuffer_with_osd), [`dpi_framebuffer_without_osd`](peripherals/dpi/dpi_framebuffer_without_osd), [`dpi_test_pattern_with_osd`](peripherals/dpi/dpi_test_pattern_with_osd), [`dpi_test_pattern_without_osd`](peripherals/dpi/dpi_test_pattern_without_osd) |
| [`peripherals/efuse`](peripherals/efuse) | [`efuse_getinfo`](peripherals/efuse/efuse_getinfo), [`efuse_rw`](peripherals/efuse/efuse_rw), [`efuse_trim`](peripherals/efuse/efuse_trim) |
| [`peripherals/emac`](peripherals/emac) | [`emac_basic`](peripherals/emac/emac_basic), [`lwip_emac`](peripherals/emac/lwip_emac) |
| [`peripherals/flash`](peripherals/flash) | [`flash_dma`](peripherals/flash/flash_dma), [`flash_get_image_hash`](peripherals/flash/flash_get_image_hash), [`flash_iomode`](peripherals/flash/flash_iomode), [`flash_read_write`](peripherals/flash/flash_read_write), [`flash_secreg`](peripherals/flash/flash_secreg), [`flash_secreg_lock`](peripherals/flash/flash_secreg_lock), [`flash_secure_read_write`](peripherals/flash/flash_secure_read_write), [`flash_secure_read_write_hw_key_ctr_128`](peripherals/flash/flash_secure_read_write_hw_key_ctr_128), [`flash_xip_read`](peripherals/flash/flash_xip_read) |
| [`peripherals/gpio`](peripherals/gpio) | [`gpio_input_output`](peripherals/gpio/gpio_input_output), [`gpio_interrupt`](peripherals/gpio/gpio_interrupt), [`gpio_output_with_input`](peripherals/gpio/gpio_output_with_input), [`gpio_validation_cli`](peripherals/gpio/gpio_validation_cli) |
| [`peripherals/i2c`](peripherals/i2c) | [`i2c_10_bit`](peripherals/i2c/i2c_10_bit), [`i2c_ds3231`](peripherals/i2c/i2c_ds3231), [`i2c_eeprom`](peripherals/i2c/i2c_eeprom), [`i2c_eeprom_dma`](peripherals/i2c/i2c_eeprom_dma), [`i2c_eeprom_interrupt`](peripherals/i2c/i2c_eeprom_interrupt), [`i2c_lsm6ds3`](peripherals/i2c/i2c_lsm6ds3), [`i2c_pca8553`](peripherals/i2c/i2c_pca8553) |
| [`peripherals/i2s`](peripherals/i2s) | [`i2s_codec`](peripherals/i2s/i2s_codec), [`i2s_dma`](peripherals/i2s/i2s_dma) |
| [`peripherals/ipc`](peripherals/ipc) | [`ipc_thrcore`](peripherals/ipc/ipc_thrcore), [`ipc_ap`](peripherals/ipc/ipc_thrcore/ipc_ap), [`ipc_np`](peripherals/ipc/ipc_thrcore/ipc_np), [`ipc_trig`](peripherals/ipc/ipc_trig) |
| [`peripherals/ir`](peripherals/ir) | [`ir_nec`](peripherals/ir/ir_nec), [`ir_rc5`](peripherals/ir/ir_rc5), [`ir_swm`](peripherals/ir/ir_swm), [`ir_swm_int`](peripherals/ir/ir_swm_int), [`ir_tx_dma`](peripherals/ir/ir_tx_dma) |
| [`peripherals/mjdec`](peripherals/mjdec) | [`mjdec_dht`](peripherals/mjdec/mjdec_dht), [`mjdec_normal`](peripherals/mjdec/mjdec_normal) |
| [`peripherals/mjdec_v2`](peripherals/mjdec_v2) | [`mjdec_normal`](peripherals/mjdec_v2/mjdec_normal), [`mjdec_pingpang`](peripherals/mjdec_v2/mjdec_pingpang) |
| [`peripherals/mjpeg`](peripherals/mjpeg) | [`mjpeg_cam_crop`](peripherals/mjpeg/mjpeg_cam_crop), [`mjpeg_cam_normal`](peripherals/mjpeg/mjpeg_cam_normal), [`mjpeg_cam_swap`](peripherals/mjpeg/mjpeg_cam_swap), [`mjpeg_no_camera`](peripherals/mjpeg/mjpeg_no_camera), [`mjpeg_recv_from_uart`](peripherals/mjpeg/mjpeg_recv_from_uart) |
| [`peripherals/pec_v2`](peripherals/pec_v2) | [`pec_dpi_basic`](peripherals/pec_v2/pec_dpi/pec_dpi_basic), [`pec_dpi_srgb`](peripherals/pec_v2/pec_dpi/pec_dpi_srgb), [`pec_dpi_srgb_mjdec`](peripherals/pec_v2/pec_dpi/pec_dpi_srgb_mjdec), [`pec_dvp_cam_basic`](peripherals/pec_v2/pec_dvp/pec_dvp_cam_basic), [`pec_dvp_cam_lcd`](peripherals/pec_v2/pec_dvp/pec_dvp_cam_lcd), [`pec_i2c_eeprom`](peripherals/pec_v2/pec_i2c/pec_i2c_eeprom), [`pec_ir_nec`](peripherals/pec_v2/pec_ir/pec_ir_nec), [`pec_ir_rc5`](peripherals/pec_v2/pec_ir/pec_ir_rc5), [`pec_ir_rx_nec`](peripherals/pec_v2/pec_ir/pec_ir_rx_nec), [`pec_ir_rx_rc5`](peripherals/pec_v2/pec_ir/pec_ir_rx_rc5), [`pec_pwm_basic`](peripherals/pec_v2/pec_pwm/pec_pwm_basic), [`pec_qspi_cam`](peripherals/pec_v2/pec_qspi/pec_qspi_cam), [`pec_spi_loopback`](peripherals/pec_v2/pec_spi/pec_spi_loopback), [`pec_srgb_basic`](peripherals/pec_v2/pec_srgb/pec_srgb_basic), [`pec_uart_loopback`](peripherals/pec_v2/pec_uart/pec_uart_loopback), [`pec_uart_loopback_dma`](peripherals/pec_v2/pec_uart/pec_uart_loopback_dma), [`pec_uart_rx_echo`](peripherals/pec_v2/pec_uart/pec_uart_rx_echo), [`pec_uart_tx_dma`](peripherals/pec_v2/pec_uart/pec_uart_tx_dma), [`pec_uart_tx_freerun`](peripherals/pec_v2/pec_uart/pec_uart_tx_freerun) |
| [`peripherals/pwm_v1`](peripherals/pwm_v1) | [`pwm_all_channels`](peripherals/pwm_v1/pwm_all_channels), [`pwm_basic`](peripherals/pwm_v1/pwm_basic), [`pwm_int`](peripherals/pwm_v1/pwm_int) |
| [`peripherals/pwm_v2`](peripherals/pwm_v2) | [`pwm_6step`](peripherals/pwm_v2/pwm_6step), [`pwm_all_channels`](peripherals/pwm_v2/pwm_all_channels), [`pwm_basic`](peripherals/pwm_v2/pwm_basic), [`pwm_config_channel`](peripherals/pwm_v2/pwm_config_channel), [`pwm_deadtime`](peripherals/pwm_v2/pwm_deadtime), [`pwm_foc_curr`](peripherals/pwm_v2/pwm_foc_curr), [`pwm_foc_hall`](peripherals/pwm_v2/pwm_foc_hall), [`pwm_int`](peripherals/pwm_v2/pwm_int), [`pwm_spwm`](peripherals/pwm_v2/pwm_spwm), [`pwm_svpwm`](peripherals/pwm_v2/pwm_svpwm) |
| [`peripherals/sdio`](peripherals/sdio) | [`sdio2_test`](peripherals/sdio/sdio2_test), [`sdio3_test`](peripherals/sdio/sdio3_test), [`sdio_msg_router`](peripherals/sdio/sdio_msg_router) |
| [`peripherals/sec_dbg`](peripherals/sec_dbg) | [`sec_dbg_password`](peripherals/sec_dbg/sec_dbg_password) |
| [`peripherals/sec_eng`](peripherals/sec_eng) | [`sec_eng_aes_link_sw_key`](peripherals/sec_eng/sec_eng_aes_link_sw_key), [`sec_eng_aes_sw_key`](peripherals/sec_eng/sec_eng_aes_sw_key), [`sec_eng_aes_sw_key_costtime`](peripherals/sec_eng/sec_eng_aes_sw_key_costtime), [`sec_eng_crc32_link`](peripherals/sec_eng/sec_eng_crc32_link), [`sec_eng_dsa`](peripherals/sec_eng/sec_eng_dsa), [`sec_eng_ecdh`](peripherals/sec_eng/sec_eng_ecdh), [`sec_eng_ecdh_384`](peripherals/sec_eng/sec_eng_ecdh_384), [`sec_eng_ecdsa`](peripherals/sec_eng/sec_eng_ecdsa), [`sec_eng_ecdsa_384`](peripherals/sec_eng/sec_eng_ecdsa_384), [`sec_eng_gmac_link`](peripherals/sec_eng/sec_eng_gmac_link), [`sec_eng_sha`](peripherals/sec_eng/sec_eng_sha), [`sec_eng_sha_costtime`](peripherals/sec_eng/sec_eng_sha_costtime), [`sec_eng_sha_link`](peripherals/sec_eng/sec_eng_sha_link), [`trng_random`](peripherals/sec_eng/trng_random) |
| [`peripherals/spi`](peripherals/spi) | [`spi_dma`](peripherals/spi/spi_dma), [`spi_flash`](peripherals/spi/spi_flash), [`spi_int`](peripherals/spi/spi_int), [`spi_poll`](peripherals/spi/spi_poll) |
| [`peripherals/timer`](peripherals/timer) | [`timer_capture`](peripherals/timer/timer_capture), [`timer_clksource_check`](peripherals/timer/timer_clksource_check), [`timer_gpio_clock`](peripherals/timer/timer_gpio_clock), [`timer_int`](peripherals/timer/timer_int) |
| [`peripherals/touch`](peripherals/touch) | [`touch_int`](peripherals/touch/touch_int), [`touch_oneshot`](peripherals/touch/touch_oneshot) |
| [`peripherals/uart`](peripherals/uart) | [`uart_auto_baudrate`](peripherals/uart/uart_auto_baudrate), [`uart_cts`](peripherals/uart/uart_cts), [`uart_dma`](peripherals/uart/uart_dma), [`uart_dma_rto`](peripherals/uart/uart_dma_rto), [`uart_end_interrupt`](peripherals/uart/uart_end_interrupt), [`uart_error_interrupt`](peripherals/uart/uart_error_interrupt), [`uart_feature_control`](peripherals/uart/uart_feature_control), [`uart_fifo_interrupt`](peripherals/uart/uart_fifo_interrupt), [`uart_ir`](peripherals/uart/uart_ir), [`uart_lin`](peripherals/uart/uart_lin), [`uart_poll`](peripherals/uart/uart_poll), [`uart_rs485`](peripherals/uart/uart_rs485), [`uart_sw_rts`](peripherals/uart/uart_sw_rts) |
| [`peripherals/wdg`](peripherals/wdg) | [`wdg_clksource_check`](peripherals/wdg/wdg_clksource_check), [`wdg_int`](peripherals/wdg/wdg_int), [`wdg_reset`](peripherals/wdg/wdg_reset) |
| [`peripherals/wo`](peripherals/wo) | [`wo_console`](peripherals/wo/wo_console), [`wo_dma`](peripherals/wo/wo_dma), [`wo_int`](peripherals/wo/wo_int), [`wo_uart`](peripherals/wo/wo_uart), [`wo_ws2812`](peripherals/wo/wo_ws2812) |

### Power-Management Applications

| Directory | Applications |
| --- | --- |
| [`pmu`](pmu) | [`bl616cl_lp_fw`](pmu/bl616cl_lp_fw), [`bl616_lp_fw`](pmu/bl616_lp_fw), [`wl_ble_lp`](pmu/wl_ble_lp), [`wl_lp`](pmu/wl_lp) |
| [`pmu/bl616`](pmu/bl616) | [`hbn_acomp`](pmu/bl616/hbn_acomp), [`hbn_io_wakeup`](pmu/bl616/hbn_io_wakeup), [`hbn_rtc`](pmu/bl616/hbn_rtc), [`pds_io_wakeup`](pmu/bl616/pds_io_wakeup), [`pds_rtc`](pmu/bl616/pds_rtc) |
| [`pmu/bl616cl`](pmu/bl616cl) | [`hbn_io_wakeup`](pmu/bl616cl/hbn_io_wakeup), [`hbn_rtc`](pmu/bl616cl/hbn_rtc), [`hbn_watchdog`](pmu/bl616cl/hbn_watchdog), [`pds_bod`](pmu/bl616cl/pds_bod), [`pds_io_wakeup`](pmu/bl616cl/pds_io_wakeup), [`pds_rtc`](pmu/bl616cl/pds_rtc), [`pds_timer`](pmu/bl616cl/pds_timer) |
| [`pmu/bl618dg`](pmu/bl618dg) | [`aon_wdt`](pmu/bl618dg/aon_wdt), [`hbn_io_wakeup`](pmu/bl618dg/hbn_io_wakeup), [`hbn_rtc`](pmu/bl618dg/hbn_rtc), [`mini_sys`](pmu/bl618dg/mini_sys), [`pds_bod`](pmu/bl618dg/pds_bod), [`pds_io_wakeup`](pmu/bl618dg/pds_io_wakeup), [`pds_lp_wakeup`](pmu/bl618dg/pds_lp_wakeup), [`pds_lp_wakeup/ap`](pmu/bl618dg/pds_lp_wakeup/ap), [`pds_lp_wakeup/lp`](pmu/bl618dg/pds_lp_wakeup/lp), [`pds_mini`](pmu/bl618dg/pds_mini), [`pds_rtc`](pmu/bl618dg/pds_rtc), [`pds_timer`](pmu/bl618dg/pds_timer), [`sdio3_lowpower`](pmu/bl618dg/sdio3_lowpower) |
| [`pmu/bl702`](pmu/bl702) | [`hbn_acomp`](pmu/bl702/hbn_acomp), [`hbn_rtc`](pmu/bl702/hbn_rtc), [`pds_io_wakeup`](pmu/bl702/pds_io_wakeup), [`pds_rtc`](pmu/bl702/pds_rtc) |
| [`pmu/lp_fw`](pmu/lp_fw) | [`bl618dg`](pmu/lp_fw/bl618dg) |

### Solution Applications

| Directory | Applications |
| --- | --- |
| [`solution`](solution) | [`mimiclaw`](solution/mimiclaw), [`OpenAI_bl618`](solution/OpenAI_bl618) |
| [`solution/solution_bl616`](solution/solution_bl616) | [`image_transmission`](solution/solution_bl616/image_transmission) |
| [`solution/solution_dualcore`](solution/solution_dualcore) | [`solution_dualcore`](solution/solution_dualcore), [`wifi_ap`](solution/solution_dualcore/wifi_ap), [`wifi_np`](solution/solution_dualcore/wifi_np) |
| [`solution/TvBox`](solution/TvBox) | [`ble_hid_cli_sender`](solution/TvBox/ble_hid_cli_sender), [`usb_ecm_audio-notify`](solution/TvBox/usb_ecm_audio-notify) |

Most standalone projects contain the following files. The exact contents vary
by example.

| File | Purpose |
| --- | --- |
| `Makefile` | SDK build entry point. |
| `CMakeLists.txt` | CMake build description, where applicable. |
| `main.c` | Application entry point. |
| `proj.conf` | Project configuration. |
| `README.md` | Project-specific chip support, build, and usage information. |

## Using Examples

Set up the SDK toolchain as described in the top-level
[README](../README.md#environment-setup). Then enter an example project
directory and build it with the target chip and board. For example:

```bash
cd examples/helloworld
make CHIP=bl616 BOARD=bl616dk
```

The supported chips and required configuration vary by project. Check the
project's `README.md` before building, and use the board names available under
[`bsp/board`](../bsp/board). For multi-core projects, specify `CPU_ID` as
documented by that example.

To flash a built image, run the following command from the same project
directory, replacing the chip and serial port as needed:

```bash
make flash CHIP=bl616 COMX=/dev/ttyUSB0
```

For configuration, cleanup, and Ninja builds, see the top-level
[README](../README.md#command-line-development).
