build_dirs += bflog
build_dirs += fatfs
build_dirs += freertos
build_dirs += helloworld
build_dirs += lua
build_dirs += lvgl
build_dirs += shell
build_dirs += peripherals/adc/adc_oneshot_1ch
build_dirs += peripherals/adc/adc_oneshot_1ch_dma
build_dirs += peripherals/adc/adc_oneshot_1ch_int
build_dirs += peripherals/adc/adc_oneshot_multich
build_dirs += peripherals/dac/dac_dma
build_dirs += peripherals/dac/dac_polling
build_dirs += peripherals/dma
build_dirs += peripherals/emac/emac_basic
build_dirs += peripherals/emac/lwip_udp
build_dirs += peripherals/gpio/gpio_interrupt
build_dirs += peripherals/gpio/gpio_output
build_dirs += peripherals/i2c/i2c_eeprom
build_dirs += peripherals/mtimer
build_dirs += peripherals/pwm/pwm_all_channel
build_dirs += peripherals/pwm/pwm_basic
build_dirs += peripherals/pwm/pwm_config_channel
build_dirs += peripherals/pwm/pwm_deadtime
build_dirs += peripherals/pwm/pwm_int
build_dirs += peripherals/rtc
build_dirs += peripherals/sec_eng/sec_eng_aes_sw_key
build_dirs += peripherals/sec_eng/sec_eng_sha
build_dirs += peripherals/sec_eng/trng_random
build_dirs += peripherals/spi/spi_dma
build_dirs += peripherals/spi/spi_poll
build_dirs += peripherals/timer
build_dirs += peripherals/uart/uart_auto_baudrate
build_dirs += peripherals/uart/uart_cts_rts
build_dirs += peripherals/uart/uart_dma
build_dirs += peripherals/uart/uart_error_interrupt
build_dirs += peripherals/uart/uart_feature_control
build_dirs += peripherals/uart/uart_fifo_interrupt
build_dirs += peripherals/uart/uart_lin
build_dirs += peripherals/uart/uart_poll
build_dirs += peripherals/uart/uart_rs485
build_dirs += peripherals/usbdev/usbd_cdc_acm
build_dirs += peripherals/usbdev/usbd_static_video
build_dirs += peripherals/usbhost

all: $(build_dirs)

$(build_dirs):
	make -C $@

.PHONY: $(build_dirs)