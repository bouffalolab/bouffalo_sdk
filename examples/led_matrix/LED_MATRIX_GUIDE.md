# LED Matrix Driver Guide

## Overview

This case supports BL616CL, BL616, and BL618DG (AP core). A circular LLI list writes GPIO low and high output words for continuous multiplexed scanning.

| Chip | Pacing peripheral | DMA resource |
|---|---|---|
| BL616CL | TIMER0 compare0 | DMA0 channel 0 |
| BL616 | SPI0 TX | DMA0 channel 0 |
| BL618DG | SPI0 TX | DMA0 channel 0 |

BL616 and BL618DG do not support timer DMA requests. Their backend uses SPI internally; no SPI pins or external SPI device are needed. Reserve the listed peripheral and DMA channel exclusively for this driver.

Current defaults:

- Matrix: 8 rows by 12 columns
- Brightness: 100 linear PWM levels
- Blanking: 4 extra time slots per row
- Target refresh: 240 Hz; BL616CL calculated refresh is about 240.38 Hz
- GPIO mode: common-anode logic (`LED_MATRIX_COMMON_CATHODE=0`)

## Hardware and scan model

The matrix uses dynamic row scanning. One row is selected at a time while column states encode the pixels for that row.

Default pin mappings are defined by `board_led_matrix_gpio_config` in each board's `board_gpio.c`:

| Board | Rows 0-7 | Columns 0-11 |
|---|---|---|
| bl616cldk | 12, 13, 14, 15, 16, 17, 3, 18 | 6, 11, 1, 26, 27, 28, 29, 30, 31, 23, 24, 25 |
| bl616dk | 0, 1, 2, 3, 4, 5, 6, 7 | 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 |
| bl618dgdk | 0, 1, 2, 3, 4, 5, 6, 7 | 8, 9, 10, 13, 14, 15, 16, 17, 18, 19, 20, 21 |

The BSP driver contains no fixed row or column pins. The example calls
`board_led_matrix_gpio_config_get()` and passes the result to
`led_matrix_init(config)`. The driver validates that every pin is in range
and that no row or column shares a GPIO. Initialization returns a negative
error for invalid configuration or when scanning is already active.
The configuration must remain valid while the driver is in use; the default
board configuration is static and stored in read-only memory.

For another board or wiring layout, provide its own configuration in
`board_gpio.c`. When using `board_gpio_overlay.h/.c`, implement
`board_led_matrix_gpio_config_get()` in the overlay, following the existing
board GPIO override mechanism. Matrix geometry and scan parameters remain
in `bflb_led_matrix.h`; update the board arrays to match any geometry change.

BL616 leaves console GPIO21/22 available. BL618DG leaves AP console GPIO11 and GPIO12/42 available. Check package pin availability, board connections, and other enabled peripherals before wiring. The driver preserves other GPIO output latch values captured during initialization; other software must not change those latches while scanning.

For each row, the driver emits 4 blanking slots followed by 100 brightness slots. A pixel is active while its brightness value is greater than the current brightness index. The current common-anode path selects the active row low, keeps columns high during blanking, and drives active columns low.

For BL616CL, the timer request rate is:

```text
DMA request rate = refresh_rate * rows * (brightness_levels + blanking_levels) * 2
compare0_ticks   = round(40 MHz / DMA request rate)
actual_refresh   = 40 MHz / (compare0_ticks * rows *
                             (brightness_levels + blanking_levels) * 2)
```

With the defaults, compare0 is 100 ticks, one row takes 520 us, each slot takes 5 us, and blanking takes 20 us.

For BL616/BL618DG, each scan slot consumes one 8-bit SPI frame:

```text
slot_rate          = refresh_rate * R * (B + K)
requested_spi_hz   = slot_rate * 8
spi_half_period    = clamp(round(spi_source_hz / (2 * requested_spi_hz)), 1, 256)
nominal_refresh    = spi_source_hz / (2 * spi_half_period * 8 * R * (B + K))
```

The source clock comes from the board clock configuration. For an 80 MHz SPI source, the default request is 1,597,440 Hz, rounded to 1.6 MHz (nominal refresh 240.38 Hz). Continuous SPI mode keeps transfers flowing; the actual GPIO slot timing also depends on SPI frame spacing and DMA latency. Confirm it with a logic analyzer on hardware.

## DMA and LLI operation

Each scan slot contains two 32-bit GPIO words:

```text
dma_scan_data[2 * slot + 0] = GPIO low output word
dma_scan_data[2 * slot + 1] = GPIO high output word
```

BL616CL uses one descriptor per slot to transfer 8 bytes to the GPIO output registers, paced by two timer requests.

BL616/BL618DG use two descriptors per slot in the same DMA channel: first transfer the two 32-bit GPIO words, then transfer one dummy byte to the SPI TX FIFO. Both use the SPI TX request. The FIFO request remains asserted during the GPIO writes; writing the dummy byte fills the vacancy and releases the request until SPI consumes another byte. This avoids arbitration between separate DMA channels sharing the same request. The SPI FIFO is prefilled before starting to avoid an initial burst of scan slots. RX data is ignored.

The final descriptor links back to the first descriptor. The entire ring is built and made visible to DMA before loading the first descriptor into the channel registers. Neither backend requires a CPU interrupt per scan slot. `led_matrix_off()` disables the pacing peripheral and DMA, and restarting resets the FIFO or timer state.

`led_matrix_update()` rebuilds `dma_scan_data` and cleans the data cache while DMA is running. The current implementation has no double buffer or frame-boundary synchronization, so an update can briefly expose mixed old and new frame data.

## Memory sizing

Let `R` be rows, `C` be columns, `B` be brightness levels, and `K` be blanking levels:

```text
scan_slots       = R * (B + K)
DMA scan data    = 8 * R * (B + K) bytes
LLI pool (timer) = 16 * R * (B + K) bytes
LLI pool (SPI)   = 32 * R * (B + K) bytes
pixel buffer     = R * C + 1 bytes
total (timer)    = 24 * R * (B + K) + R * C + 1 bytes
total (SPI)      = 40 * R * (B + K) + R * C + 2 bytes (includes dummy byte)
```

With `B=100` and `K=4`, the timer total is `2496*R + R*C + 1` bytes; the SPI total is `4160*R + R*C + 2` bytes. The following table describes the timer backend.

| Matrix | Scan data | LLI pool | Pixel buffer | Main static data |
|---|---:|---:|---:|---:|
| 8 x 12 | 6,656 B | 13,312 B | 97 B | 20,065 B (19.6 KiB) |
| 16 x 16 | 13,312 B | 26,624 B | 257 B | 40,193 B (39.3 KiB) |
| 32 x 32 | 26,624 B | 53,248 B | 1,025 B | 80,897 B (79.0 KiB) |

For the default 8 x 12 matrix, the SPI backend uses 33,378 bytes (32.6 KiB) for these buffers, including a 26,624-byte LLI pool.

These figures exclude code, constants, stack, other globals, and linker alignment. They also do not prove that GPIO count, DMA throughput, LED current, or refresh quality is sufficient.

## Driver API

| Function | Purpose |
|---|---|
| `led_matrix_init(config)` | Validate board GPIO configuration, then initialize GPIO, pacing peripheral, DMA, and scan data |
| `led_matrix_on()` | Start continuous DMA scanning |
| `led_matrix_off()` | Stop scanning and deactivate all LEDs |
| `led_matrix_clear()` | Set every pixel to zero brightness |
| `led_matrix_set_pixel()` | Set one pixel brightness from 0 to 100 |
| `led_matrix_get_pixel()` | Read one pixel brightness |
| `led_matrix_set_row()` | Set one row from an array |
| `led_matrix_set_column()` | Set one column from an array |
| `led_matrix_set_rectangle()` | Set a rectangular region |
| `led_matrix_set_number()` | Draw a 3 x 5 digit glyph |
| `led_matrix_update()` | Rebuild the DMA scan data |

## Application commands

Enter each command in the UART shell and press Enter. Use `help` to list commands.
In the syntax below, `<...>` is a required argument and `[...]` is optional;
do not type the brackets. Coordinates start at zero: rows are 0-7 and columns
are 0-11 for the default 8 x 12 matrix. Brightness values are integers from
0 (off) to 100 (maximum configured brightness).

| Command | Behavior and parameters | Example |
|---|---|---|
| `timer_start [seconds]` | Start an increasing seconds counter at 0-99; default is 0. Display two digits and wrap to 00 after 99. | `timer_start 30` |
| `stop` | Stop the current timer or animation and clear all pixels. DMA scanning remains enabled. | `stop` |
| `number <value>` | Display an integer from 0 to 99 at full brightness. Values below 10 use one digit; other values use two digits. Clear the previous image. | `number 42` |
| `pixel <row> <col> <brightness>` | Set one pixel, preserving all other pixels. | `pixel 2 3 50` |
| `all <brightness>` | Set every pixel to the same brightness. | `all 100` |
| `pwm_gradient` | Show a static gradient: brightness is `min(row * columns + col, 100)`. The default matrix ranges from 0 at (0,0) to 95 at (7,11). | `pwm_gradient` |
| `palindrome_start [brightness]` | Start a pattern mirrored across the vertical center, updated every 250 ms. | `palindrome_start 50` |
| `scan_start [brightness]` | Start a vertical light band moving left and right, with dimmer adjacent columns; update every 70 ms. | `scan_start 80` |
| `rain_start [brightness]` | Start falling drops with dimmer tails; update every 110 ms. | `rain_start 60` |
| `expand_start [brightness]` | Start a filled region expanding from the center and then shrinking; update every 120 ms. | `expand_start 100` |

Only one timer or animation mode runs at a time. Starting another replaces
the current mode. `number`, `pixel`, `all`, and `pwm_gradient` stop the active
timer or animation and leave a static image. `stop` also clears that image.

When animation brightness is omitted, the initial value is 100. Later calls
to `palindrome_start` reuse its own previous brightness. `scan_start`,
`rain_start`, and `expand_start` share a previous brightness value, so setting
one also determines the default for the other two. An explicit brightness
of 0 keeps the animation running with all its pixels off.

For example, enter these commands one at a time to clear the matrix, light
one pixel at half brightness, start rain, and then stop:

```text
all 0
pixel 2 3 50
rain_start 60
stop
```

## Build and validation notes

Build the case with:

```sh
make CHIP=bl616cl BOARD=bl616cldk BUILD_DIR=build_bl616cl
make CHIP=bl616 BOARD=bl616dk BUILD_DIR=build_bl616
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap BUILD_DIR=build_bl618dg
```

The case does not measure CPU utilization automatically. Both DMA backends avoid per-slot CPU interrupt work, but initialization, scan-data rebuilds, shell processing, and animations still run on the CPU.

Build verification does not establish electrical behavior. On hardware, check refresh timing, brightness at 0/50/100, row transitions, repeated off/on cycles, and continued UART command handling.
