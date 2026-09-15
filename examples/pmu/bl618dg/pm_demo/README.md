# BL618DG PM demo

This example combines the BL618DG PDS/HBN entry paths behind one shell
command. GPIO, BOD, keyscan, and RTC wakeup paths can be selected independently
with Kconfig. GPIO, BOD, and RTC wakeup are enabled by default.

## Build

```sh
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0
```

## Command

```text
app_pm_enter <PDS|HBN> <level> <wake_ms> [rtc|timer]
app_pm_bod <0-7>
app_pm_gpio <gpio> <falling|rising|low|high> <none|up|down>
app_pm_gpio reset
```

Supported levels:

- PDS: `1`, `2`, `3`, `7`, `15`
- HBN: `0`

When `CONFIG_EXAMPLE_PM_RTC_WAKEUP` is enabled, `wake_ms` is the relative timer
wakeup delay in milliseconds. Set it to `0` to disable the timer for one sleep
entry. The optional last argument selects the timer wakeup channel: `rtc`
(default) uses the HBN RTC through `PDS_WAKEUP_BY_HBN_IRQ_OUT`, while `timer`
uses the PDS timer through `PDS_WAKEUP_BY_PDS_TIMER` (PDS mode only; HBN has no
PDS timer). When RTC wakeup is compiled out, `wake_ms` must be `0`.
The conversion uses the RTC's nominal 32.768 kHz rate. Because this example
selects RC32K, the measured delay varies with its trim accuracy; use XTAL32K or
an application-specific fine-trim procedure when precise timing is required.

Examples:

```text
app_pm_enter PDS 15 5000
app_pm_enter PDS 15 5000 timer
app_pm_enter HBN 0 5000
app_pm_bod 5
app_pm_gpio reset
app_pm_gpio 6 falling up
app_pm_gpio 9 rising down
```

The following options are available through `make menuconfig`:

- `CONFIG_EXAMPLE_PM_GPIO_WAKEUP`: enable board-configured GPIO wakeup
- `CONFIG_EXAMPLE_PM_BOD_WAKEUP`: enable BOD wakeup for PDS and HBN
- `CONFIG_EXAMPLE_PM_KEYSCAN_WAKEUP`: enable keyscan wakeup for PDS
- `CONFIG_EXAMPLE_PM_KEYSCAN_POLL_MODE`: use keyscan polling instead of the
  default interrupt mode while waiting for the startup key presses
- `CONFIG_EXAMPLE_PM_RTC_WAKEUP`: enable RTC wakeup for PDS and HBN

Wake GPIO pins, pulls, and the trigger mode of each eight-pin GPIO group are
configured by `board_lp_gpio_wakeup_config` in the board's `board_gpio.c`, just
like the keyscan matrix mapping. The BL618DGDK default uses GPIO6 with an
internal pull-up and asynchronous falling-edge trigger, plus GPIO9 with an
internal pull-down and asynchronous rising-edge trigger. GPIO6 can wake PDS
and HBN; GPIO9 can wake PDS only. These pins do not overlap the default
keyscan matrix, so the default `defconfig` enables keyscan and GPIO wakeup
together. Enabled wake GPIOs must not overlap the enabled board keyscan matrix
or another board function.

The BOD selector maps to approximately 2.05 V, 2.10 V, 2.15 V, 2.20 V,
2.25 V, 2.30 V, 2.35 V, and 2.40 V for selector values 0 through 7,
respectively. The default selector is 7. `app_pm_bod` changes the threshold
immediately for the current boot and reports the selected voltage and current
BOD output state.

The application copies the const board GPIO wakeup configuration into RAM at
startup. `app_pm_gpio` updates and applies that RAM copy immediately. Repeated
GPIO commands add or update pins; `app_pm_gpio reset` clears the complete RAM
configuration, including all board defaults. To build a different set from
scratch, run `reset` first and then add each pin. The async trigger mode is
shared by each eight-pin group, so the command rejects a trigger that conflicts
with another enabled pin in the same group.

The enabled wake GPIO, BOD, normal keyscan controller, and static PDS key
detector parameters are configured during startup and may be updated by the
test CLI. Each sleep entry only clears stale wake status, arms the selected
wake paths, and loads the relative RTC delay. PDS15 additionally applies its
level-specific keyscan pad settings before sleep and restores the normal
keyscan matrix after wakeup. When keyscan is enabled, startup waits for two
keyscan results and prints each result before initializing the shell. The
subsequent `app_pm_enter` command enters the selected power mode directly.
Interrupt mode is the default; enabling
`CONFIG_EXAMPLE_PM_KEYSCAN_POLL_MODE` uses the driver's blocking query API.

For PDS, `PDS_WAKEUP_BY_HBN_IRQ_OUT` is unmasked for BOD, RTC, or a configured
GPIO0-GPIO7 wakeup. GPIO8-GPIO52 use `PDS_WAKEUP_BY_PDS_GPIO_INT`, while
`PDS_WAKEUP_BY_KEYSCAN` is controlled independently by the keyscan option. The
`timer` argument instead routes the timer wakeup through
`PDS_WAKEUP_BY_PDS_TIMER`, which the PDS entry configures directly from
`wake_ms`.
Disabling all applicable sources causes the selected power mode to wait
indefinitely for another reset condition.

HBN wakeup resets the CPU, so its wake reason is printed near the start of the
next boot. PDS wakeup restores execution and prints its wake sources before
returning to the shell. Runtime CLI settings are not retained across HBN reset;
the next boot reloads board and Kconfig defaults. HBN GPIO wake reporting reads
all GPIO0-GPIO7 status bits so a runtime-configured pin is still identified.

## Test Walkthrough

The default board config enables GPIO6 (pull-up, async falling edge) and GPIO9
(pull-down, async rising edge) as wake pins, plus the 8x8 keyscan matrix. GPIO6
can wake PDS and HBN; GPIO9 can wake PDS only. The steps below assume the
default `defconfig` (GPIO, BOD, keyscan, and RTC wakeup all enabled).

### Wakeup source bits

`PDS wakeup sources: 0x%08lx` reports the raw PDS wakeup source register. The
bits used by this demo are:

| Bit | Value    | Source                        | Detail print            |
|-----|----------|-------------------------------|-------------------------|
| 0   | 0x000001 | `PDS_WAKEUP_BY_PDS_TIMER`     | `PDS timer`             |
| 1   | 0x000002 | `PDS_WAKEUP_BY_HBN_IRQ_OUT`   | `HBN RTC` / `HBN BOD` / `HBN GPIO0-7` |
| 3   | 0x000008 | `PDS_WAKEUP_BY_PDS_GPIO_INT`  | `PDS GPIO8-52`          |
| 8   | 0x000100 | `PDS_WAKEUP_BY_KEYSCAN`       | `keyscan row=.. col=..` |

### 1. Confirm the program is running

After power-on or reset the console prints:

```text
PM demo starting...
PM demo cold boot
Current RTC time: 0.000
Current RTC time: 0.500
PM demo wakeup sources: GPIO-mask=0x... BOD PDS-keyscan RTC
PM demo keyscan: press 2 keys before shell startup
```

Wait about 1 second, then press any two keys on the keyscan matrix with at
least 1 second between presses. Each press prints its key number and row/col.
After the second press the shell starts:

```text
PM demo keyscan: key_num=1 key[0]=.. row=.. col=..
PM demo keyscan press count: 1/2
PM demo keyscan: key_num=1 key[0]=.. row=.. col=..
PM demo keyscan press count: 2/2
PM demo keyscan: 2 key presses detected
PM demo ready: app_pm_enter <PDS|HBN> <level> <wake_ms> [rtc|timer]
```

### 2. PDS RTC timer wakeup

Works for every PDS level (`1`, `2`, `3`, `7`, `15`); only the power depth
differs. Example with level 1 and a 2000 ms delay:

```text
app_pm_enter PDS 1 2000
```

Console output:

```text
Enter PDS1: RTC delay=2000 ms (0=disabled)
```
about 2 seconds later:
```text
PDS wakeup sources: 0x00000002
  HBN RTC
```

Repeat for levels `2`, `3`, `7`, `15` with the same command, replacing the
level number.

### 3. PDS timer wakeup

Same as above but routes the timer through `PDS_WAKEUP_BY_PDS_TIMER` instead of
the HBN RTC:

```text
app_pm_enter PDS 1 2000 timer
```

Console output:

```text
Enter PDS1: PDS-timer delay=2000 ms (0=disabled)
```
about 2 seconds later:
```text
PDS wakeup sources: 0x00000001
  PDS timer
```

Repeat for levels `2`, `3`, `7`, `15`.

### 4. PDS keyscan wakeup

Keyscan is a PDS-only wake source. Pass `wake_ms 0` so no timer is armed, then
press any key on the matrix while the chip sleeps:

```text
app_pm_enter PDS 1 0
```

Press a key on the matrix:

```text
PDS wakeup sources: 0x00000100
  keyscan row=.. col=.. row_gpio=.. col_gpio=..
```

Works for levels `2`, `3`, `7`, `15` as well. Level `15` additionally applies
its level-specific keyscan pad settings before sleep.

### 5. PDS GPIO wakeup

Pass `wake_ms 0`, then toggle the configured GPIO.

GPIO6 (pull-up, falling edge) wakes through `HBN_IRQ_OUT`:

```text
app_pm_enter PDS 1 0
```
drive GPIO6 low:
```text
PDS wakeup sources: 0x00000002
  HBN GPIO6
```

GPIO9 (pull-down, rising edge) wakes through `PDS_GPIO_INT`:

```text
app_pm_enter PDS 1 0
```
drive GPIO9 high:
```text
PDS wakeup sources: 0x00000008
  PDS GPIO9
```

Works for levels `2`, `3`, `7`, `15`. A runtime pin added with `app_pm_gpio`
behaves the same way; GPIO0-7 report as `HBN GPIOx`, GPIO8-52 report as
`PDS GPIOx`.

### 6. PDS BOD wakeup

Pass `wake_ms 0` and let the supply dip below the configured BOD threshold
(adjust it first with `app_pm_bod <0-7>`):

```text
app_pm_bod 3
app_pm_enter PDS 1 0
```

When the supply drops below the threshold:

```text
PDS wakeup sources: 0x00000002
  HBN BOD
```

### 7. HBN RTC wakeup

HBN resets the CPU, so the wake reason is printed on the next boot:

```text
app_pm_enter HBN 0 2000
```

Console:

```text
Enter HBN0: RTC delay=2000 ms (0=disabled)
```
about 2 seconds later the chip resets and prints:
```text
PM demo starting...
PM demo HBN wakeup sources:
  HBN RTC
```

HBN only supports the RTC timer; passing `timer` is rejected.

### 8. HBN GPIO wakeup

Pass `wake_ms 0`, then toggle GPIO6 (pull-up, falling edge). GPIO9 cannot wake
HBN because GPIO8-52 wake only PDS:

```text
app_pm_enter HBN 0 0
```
drive GPIO6 low:
```text
PM demo starting...
PM demo HBN wakeup sources:
  HBN GPIO6
```

### 9. HBN BOD wakeup

Pass `wake_ms 0` and let the supply dip below the BOD threshold:

```text
app_pm_enter HBN 0 0
```

When the supply drops below the threshold:

```text
PM demo starting...
PM demo HBN wakeup sources:
  HBN BOD
```
