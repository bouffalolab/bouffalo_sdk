# LED Matrix Solution

This solution demonstrates the common BSP LED matrix driver on BL616CL, BL616, and BL618DG. A circular DMA list writes GPIO output data, paced by TIMER0 on BL616CL and SPI0 TX on BL616/BL618DG.

## Supported target

| Chip | Board | CPU |
|:---:|:---:|:---:|
| BL616CL | bl616cldk | - |
| BL616 | bl616dk | - |
| BL618DG | bl618dgdk | AP |

## Build

```sh
make CHIP=bl616cl BOARD=bl616cldk
make CHIP=bl616 BOARD=bl616dk
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

## Flash

```sh
make flash CHIP=bl616cl BOARD=bl616cldk COMX=/dev/ttyUSB0
```

## Serial commands

```text
timer_start [seconds]
stop
number <0-99>
pixel <row> <col> <brightness>
all <brightness>
pwm_gradient
palindrome_start [brightness]
scan_start [brightness]
rain_start [brightness]
expand_start [brightness]
```

Rows are 0-7, columns are 0-11, and brightness is 0-100. Use `help` to list shell commands.

## Driver configuration

Enable `CONFIG_BSP_LED_MATRIX` to build the driver in `bsp/common/led_matrix`.
The current defaults are 8 rows, 12 columns, 100 brightness levels, 4 blanking
slots, and a target refresh rate of 240 Hz. Both backends reserve DMA0
channel 0. BL616CL also reserves TIMER0; BL616/BL618DG reserve SPI0 without
using SPI pins. GPIO mappings are defined in each board's `board_gpio.c`; see
[LED_MATRIX_GUIDE.md](LED_MATRIX_GUIDE.md).
Adapt the board pin configuration to the connected matrix. Verify the wiring and polarity before running the example.

The main application provides the serial commands above.
See [LED_MATRIX_GUIDE.md](LED_MATRIX_GUIDE.md) for the
scan timing, memory formulas, API, and current update limitations.

The original chip test remains under `chiptest/bl616l/led_matrix` and is not changed by this solution.
