# BL616CL PM demo

This example combines the BL616CL PDS and HBN entry paths behind one shell
command. GPIO, BOD, RTC, and the HBN-domain watchdog can be selected
independently with Kconfig. BL616CL does not support the `bflb_kys_v2`
peripheral, so the BL618DG keyscan demonstration is intentionally omitted.

## Build

```sh
make CHIP=bl616cl BOARD=bl616cldk
```

## Commands

```text
app_pm_enter <PDS|HBN> <level> <wake_ms> [rtc|timer]
app_pm_bod <0-7>
app_pm_gpio <gpio> <falling|rising|low|high> <none|up|down>
app_pm_gpio reset
```

Supported levels are PDS `1`, `2`, `3`, `7`, `11`, and `15`, plus HBN `0`
and `1`. HBN2 is not exposed because the BL616CL HBN2 path powers off the HBN
RTC domain and the existing GPIO wakeup example only validates HBN0/HBN1.

`wake_ms` is a relative delay in milliseconds. For PDS, `rtc` (the default)
uses the HBN RTC through `PDS_WAKEUP_BY_HBN_IRQ_OUT`; `timer` uses the native
PDS timer. HBN only accepts `rtc`. A delay of zero disables the selected timer
for that entry. The conversion assumes a nominal 32.768 kHz RTC clock, so RC32K
accuracy affects the measured delay.

The default board configuration enables:

- GPIO0: pull-up, asynchronous falling edge, PDS and HBN wakeup
- GPIO12: pull-down, asynchronous rising edge, PDS-only wakeup

GPIO0-GPIO5 are HBN AON pads and can wake both PDS and HBN. GPIO6-GPIO36 use
the PDS GPIO detector and can only wake PDS. Unlike BL618DG, each BL616CL GPIO
has an independent trigger mode. Change the const
`board_lp_gpio_wakeup_config` in `bsp/board/bl616cldk/board_gpio.c` for the
product board wiring.

At startup, the application copies the board configuration to RAM and applies
it. `app_pm_gpio` adds or updates a pin in that RAM copy. `app_pm_gpio reset`
clears every RAM wake GPIO, including the board defaults. Runtime settings are
lost after HBN reset and the next boot reloads the board defaults.

The BOD selector maps as follows:

| Selector | Voltage |
|----------|---------|
| 0 | 2.2 V |
| 1 | 2.3 V |
| 2 | 2.4 V |
| 3 | 2.5 V |
| 4 | 2.6 V |
| 5 | 2.7 V |
| 6 | 2.8 V |
| 7 | 2.9 V |

The default selector is 5 (2.7 V). `app_pm_bod` changes it immediately for the
current boot and prints the current BOD output state.

The following options are available through `make menuconfig`:

- `CONFIG_EXAMPLE_PM_GPIO_WAKEUP`
- `CONFIG_EXAMPLE_PM_BOD_WAKEUP`
- `CONFIG_EXAMPLE_PM_BOD_THRESHOLD`
- `CONFIG_EXAMPLE_PM_RTC_WAKEUP`
- `CONFIG_EXAMPLE_PM_HBN_WDT`

The HBN watchdog is enabled by default with a one-second timeout and is fed
every 250 ms in the active shell loop. This short timeout is intentional for
automated reset testing. Disable `CONFIG_EXAMPLE_PM_HBN_WDT` when manually
testing GPIO or BOD wakeup without a strict response time.

## Test Walkthrough

### 1. Startup and active watchdog feed

After flashing, the console should contain:

```text
PM demo starting...
PM demo cold boot
PM demo wakeup sources: GPIO-mask=0x0000000000001001 BOD RTC
PM demo HBN WDT configured: timeout=1000 ms, feed interval=250 ms
PM demo ready: app_pm_enter <PDS|HBN> <level> <wake_ms> [rtc|timer]
```

Leave the shell idle for at least five seconds. It must remain alive, proving
that the active loop feeds the watchdog.

### 2. PDS RTC wakeup

Use a delay shorter than the default watchdog timeout:

```text
app_pm_enter PDS 1 500
```

Expected wake information:

```text
PDS wakeup sources: 0x00000002
  HBN RTC
```

Repeat with PDS levels `2`, `3`, `7`, `11`, and `15`.

### 3. PDS timer wakeup

```text
app_pm_enter PDS 15 500 timer
```

Expected wake information:

```text
PDS wakeup sources: 0x00000001
  PDS timer
```

### 4. PDS GPIO wakeup

For an unrestricted manual test, disable the watchdog in Kconfig. Enter PDS
with no timer and drive GPIO0 low:

```text
app_pm_enter PDS 15 0
```

Expected output:

```text
PDS wakeup sources: 0x00000002
  HBN GPIO0
```

Enter PDS again and drive GPIO12 high. Expected output:

```text
PDS wakeup sources: 0x00000008
  PDS GPIO12
```

The runtime helper can build a different setup without changing the board:

```text
app_pm_gpio reset
app_pm_gpio 1 falling up
app_pm_gpio 13 rising down
```

### 5. PDS BOD wakeup

Disable the watchdog, select a threshold appropriate for the power source, and
enter PDS without a timer:

```text
app_pm_bod 3
app_pm_enter PDS 15 0
```

Dropping the supply below 2.5 V should report the HBN IRQ path and `HBN BOD`.

### 6. HBN RTC and GPIO wakeup

HBN wakeup resets the CPU, so its source is printed during the next boot. With
the default watchdog enabled, keep the RTC delay below one second:

```text
app_pm_enter HBN 0 500
```

The next boot should report `HBN RTC`. Test HBN1 in the same way. For GPIO,
disable the watchdog, use a zero delay, and drive GPIO0 low:

```text
app_pm_enter HBN 0 0
```

The next boot should report `HBN GPIO0`. GPIO6-GPIO36 cannot wake HBN.

### 7. Watchdog during PDS and HBN

First verify normal PDS recovery with a 500 ms timer as above. Then request a
five-second sleep:

```text
app_pm_enter PDS 15 5000 timer
```

If the HBN watchdog remains active in PDS15, the chip resets after about one
second and the next boot prints:

```text
PM demo reset source: HBN WDT
```

For HBN, first use the 500 ms RTC test as the normal-wakeup control, then run:

```text
app_pm_enter HBN 0 5000
```

`PM demo reset source: HBN WDT` means the watchdog remains active in HBN0. If
`HBN RTC` appears after about five seconds instead, the watchdog did not run in
that HBN level. Repeat with HBN1 to document the silicon behavior.
