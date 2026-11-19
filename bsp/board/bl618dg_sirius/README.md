# BL618DG Sirius board support

Build with `BOARD=bl618dg_sirius CHIP=bl618dg`. Use the same board selection
when building the AP and NP images.

This directory provides the board GPIO overlay. Other startup and platform
support is inherited from the shared BL618DG DK board.

## GPIO mapping

| Function | GPIO assignment |
| --- | --- |
| UART0 console | TX 11, RX 42 |
| I2C0 | SDA 12, SCL 13 |
| CAN1 | RX 14, TX 15 |
| RF switch | 16; reserved for the application's RF configuration |
| ADC keys | 18, ADC0_CH3; `board_adc_gpio_init()` |
| Touch interrupt | 19; `board_touch_int_gpio_init()` |
| AuPWM | P 18, N 19; `board_audac_gpio_init()` |
| DSI backlight enable | 20, active high; call `board_lcd_backlight_gpio_init()` explicitly |
| DSI, two data lanes | DN0 23, DP0 24, DN1 25, DP1 26, CLKN 27, CLKP 28 |
| I2S | FS 29, DI 30, DO 31, BCLK 32 |
| WS2812 / optional touch reset / BOOT | 33 |
| USB | DP 40, DM 41 |
| UART1 RS485 | RX 43, TX 44 |
| I2C2 alternate function | SDA 43, SCL 44; shared with RS485 |
| SD card, 1-bit | CMD 45, CLK 46, DAT0 47 |
| Camera / codec MCLK | 48, chip clock output 0 |
| DVP data | D0-D3 0-3, D4-D7 7-10 |
| DVP synchronization | HSYNC 4, VSYNC 5, PCLK 6 |

GPIO34-39 are available to the Flash interface; the overlay leaves them
untouched and does not force dual-line Flash mode.

## Shared connections

- `board_dvp_gpio_init()` selects a 24 MHz camera clock on GPIO48.
  `board_i2s_codec_gpio_init()` selects the I2S reference clock on the same
  output. `board_i2s_gpio_init()` configures only GPIO29-32 and preserves MCLK.
  For concurrent camera and codec operation, use the camera's 24 MHz MCLK,
  call the pin-only I2S helper, and configure the I2S peripheral reference and
  codec registers in the application. Do not call the codec clock initializer
  afterward unless the camera also supports the selected MCLK.
- DVP uses BCLK / 2 for its internal clock and a separate 24 MHz sensor MCLK.
  Sensor format, FIFO threshold and capture buffers belong to the application.
- GPIO16 is connected to the RF switch. The GPIO helpers leave it untouched;
  the application must select the RF mode appropriate for the fitted module.
- The standalone NP console maps UART1 onto the console connector GPIO11/42.
  AP UART0 and NP UART1 cannot own these pins concurrently. UART1 is also the
  RS485 peripheral; a multicore application must assign console/RS485 ownership
  or redirect/disable NP console before enabling RS485.
- RS485 and I2C2 use the same GPIO43/44. The I2C2 helper only selects the
  alternate function; it does not disconnect the fitted RS485 circuitry.
- The SD helpers configure only CMD, CLK and DAT0. The SD-card stack defaults
  to `CONFIG_SDH_BUS_WIDTH_1LINE=y` when SD/SDIO host support is enabled.
  A separate host/device implementation must also select 1-bit operation.
  GPIO43/44 and GPIO48 are not SD data pins
  on this revision and are left untouched by SD initialization/deinitialization.
- GPIO33 is sampled as BOOT during reset and drives the WS2812 chain after
  startup. R132 is normally unpopulated and R133 pulls the screen reset high.
  `board_touch_reset_gpio_init()` can control screen reset only when R132 is
  fitted; that option shares reset with WS2812 and BOOT.
- GPIO20 backlight control shares Type-B DBI D/C. The backlight helper is
  explicit and must be used only with a compatible display connection.
- GPIO18/19 are shared by AuPWM, ADC keys and touch interrupt. Choose the
  matching resistor population and initialize only the function being used.
- I2C0 is shared by the camera, touch controller, ICM-42688-P, BMM150 and audio
  codecs. IMU and magnetometer acquisition uses polling; no interrupt GPIOs
  are assigned. Serialize transfers when several tasks share the bus.
- `AUDIO_CODEC_JY6316_ENABLE` names the SDK composite driver for the JY6311
  DAC and JY6166 ADC pair. Capture slot roles depend on the application's
  codec configuration; set the microphone/reference masks for that mode.
