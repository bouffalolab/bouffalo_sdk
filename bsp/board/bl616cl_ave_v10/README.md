# BL616CL AVE V10 board

Build examples with `CHIP=bl616cl BOARD=bl616cl_ave_v10`.
This board reuses `bl616cldk/board.c`; its board-specific pin functions are in
`board_gpio_overlay.c`.

## Audio defaults

`Kconfig.defconfig` supplies two playback PCM channels and two interleaved
capture DMA channels for the onboard ES8388. Microphone slot 1 and playback
reference slot 0 are described by masks `0x02` and `0x01`. Normal I2S uses
automatic bus slots. Explicit project configuration may override defaults.
Selecting an external JY6316 profile instead uses its shared four-channel
capture and fixed four-slot DSP defaults; its physical wiring must match.

## Camera and Ethernet mode

The schematic shows H1 `STATUS_SW` driving
external analog switches. GPIO23-31 are routed to **either** the camera DVP
connector **or** the Ethernet RMII PHY. The two interfaces cannot operate at
the same time. Firmware cannot read or set H1; choosing `BOARD` only sets SoC
GPIO functions and does not move the external switches.

| SoC GPIO | Camera DVP | Ethernet RMII |
| --- | --- | --- |
| 23 | VSYNC | MDIO |
| 24 | DAT4 | MDC |
| 25 | DAT5 | TX_EN |
| 26 | DAT0 | TXD1 |
| 27 | DAT1 | TXD0 |
| 28 | DAT2 | REF_CLK |
| 29 | DAT3 | RXD1 |
| 30 | DAT6 | RXD0 |
| 31 | DAT7 | RX_DV |

For a camera test, set H1 `STATUS_SW` to the **DVP** connection, then run a
normal camera example such as `cam_int`. For an Ethernet test, set H1 to the
**RMII** connection, then run `emac_basic`. The GPIO initialization functions
print reminders, but they cannot confirm H1's physical position. If a sensor
is detected over I2C but no frames arrive, or if Ethernet has no link or PHY
response, check H1 and the peripheral cabling before changing software.

Camera MCLK uses GPIO3, PCLK GPIO21, and HSYNC GPIO22. The camera connector's
I2C0 uses GPIO11 and GPIO20. Those signals do not replace the H1 selection
for the shared DVP/RMII pins.
