# BL618DG MStar board

Select `CHIP=bl618dg BOARD=bl618dg_mstar CPU_ID=ap CPU_MODEL=b0`.
Like `bl616cl_ave_v10`, this board keeps its hardware differences in
`board_gpio_overlay.c` and `Kconfig.defconfig` while reusing the BL618DG DK
memory, RF and boot setup. The J7 JY6166 ADC/JY6311 DAC wiring below is shared
by applications that select this board.

- I2C SDA/SCL: GPIO4/5.
- I2S MCLK/DO/BCLK/FS/DI: GPIO6/7/8/9/10.
- Audio-board enable: GPIO48, asserted before codec probing.
- Four DSP/TDM slots: playback reference, unused, unused microphone input,
  onboard microphone. The valid microphone mask is `0x08`, reference `0x01`.
- USB Host uses GPIO40 and GPIO41.
- TF/microSD supports the native **1-bit SDH** interface: CMD GPIO45,
  CLK GPIO46 and DAT0 GPIO47. This board defines `CONFIG_SDH_BUS_WIDTH_1LINE`
  for all applications; no application-specific bus-width setting is needed.
  Call `board_sdh_gpio_init()` before initializing the SDH driver or mounting
  the card (for FatFs, call `fatfs_sdh_driver_register()` and then `f_mount()`).
  `board_sdh_gpio_init()` leaves GPIO43, GPIO44 and GPIO48 unchanged; DAT1-3
  are not used for this board's TF transfers. GPIO48 remains available for the
  audio-board enable above. Simultaneous TF and audio operation needs separate
  application validation. Four-bit TF mode is unsupported on this wiring.

The SDK names this external codec JY6316. The shared codec configuration
provides four capture/bus slots; this board supplies its actual valid inputs.
The generic JY6316 dual-microphone mask remains `0x0c` for other hardware.

Applications receive the layout with each interleaved capture frame. Select
external I2S audio in applications that also support the built-in codec.
Other audio-board wiring needs a matching board profile.
