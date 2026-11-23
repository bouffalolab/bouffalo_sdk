# wifi_bz_coexm

Wi-Fi/BLE coexistence demo for BL616 and BL616CL, with additional BL618DG
BR/EDR support. See [COEX_GUIDE.md](COEX_GUIDE.md) for topology selection,
STA/AP examples, PS-PTA, Protection and application APIs.

## Build

Run the command for your board from `examples/coex/wifi_bz_coexm`.

```sh
make CHIP=bl616 BOARD=bl616dk
make CHIP=bl616cl BOARD=bl616cldk
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

BL618DG uses the SDK's default PHYRF library, including for standalone operation.

### BL618DG Coex Board Configuration

The BL618DG SYSINIT `WIRELESS` record uses the board topology to prepare RF.
Select a topology that matches your board and is supported by the released
MACSW library. Use the `Wi-Fi/Bluetooth coexistence` menu or the
`ifeq ($(CHIP),bl618dg)` block in this example's `defconfig`.

| Board wiring | Kconfig choice | Notes |
|---|---|---|
| Combo, no external SPDT | `CONFIG_COEX_BOARD_TOPOLOGY_COMBO` | combo RF path, 2.4 GHz coexistence |
| Standalone single antenna, SPDT + diplexer | `CONFIG_COEX_BOARD_TOPOLOGY_STANDALONE_SINGLE_ANT_SPDT` | also set `CONFIG_COEX_BOARD_SPDT_GPIO` |
| Standalone dual antenna, no SPDT | `CONFIG_COEX_BOARD_TOPOLOGY_STANDALONE_DUAL_ANT` | requires dual-antenna coexistence support in the released MACSW library |

The checked-in defconfig selects the single-antenna SPDT topology with GPIO18.
Verify the GPIO and switch polarity against the schematic and BSP.

```sh
make menuconfig CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

Use the same `BUILD_DIR` for menuconfig and the build. Once menuconfig is saved,
that build directory uses its saved configuration. Use a fresh build directory
to apply defconfig and command-line overrides. The default selection in
`defconfig` is as follows.

```make
CONFIG_COEX_BOARD_TOPOLOGY_STANDALONE_SINGLE_ANT_SPDT =y
CONFIG_COEX_BOARD_SPDT_GPIO                          =18
```

If no topology is selected on a BL618DG `CONFIG_COEX` build, the build
intentionally fails with a diagnostic instead of starting with an unknown RF
path.

### BL618DG Flash / PSRAM

Use separate build directories and a board with compatible PSRAM.

```sh
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap BUILD_DIR=build_flash \
    CONFIG_PSRAM=n CONFIG_PSRAM_COPY_CODE=n

make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap BUILD_DIR=build_psram \
    CONFIG_PSRAM=y CONFIG_PSRAM_COPY_CODE=y CONFIG_PSRAM_XIP=n
```

`CONFIG_PSRAM_COPY_CODE=y` copies the configured code sections to PSRAM at boot
for execution there. Flash the artifacts from the selected build directory.

### Standalone Bluetooth Controller Configuration

If the standalone Bluetooth controller build reports missing FreeRTOS defines
such as `CONFIG_FREERTOS_MAX_PRIORITIES`, pass it the generated FreeRTOS
configuration. The following Bash example uses the PSRAM build directory.

```bash
sdk_root=$(cd ../../.. && pwd)
build_dir=build_psram
build_args=(CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap BUILD_DIR="$build_dir"
    CONFIG_PSRAM=y CONFIG_PSRAM_COPY_CODE=y CONFIG_PSRAM_XIP=n)
make cmake_cache "${build_args[@]}"
rtos_config="$PWD/$build_dir/generated/freertos-standalone.h"
awk '/^#define CONFIG_FREERTOS([[:space:]]|_)/' \
    "$build_dir/generated/autoconfig.h" > "$rtos_config"
"$sdk_root/tools/cmake/bin/cmake" \
    -S "$sdk_root/components/wireless/bluetooth/btblecontroller" \
    -B "$build_dir/build_btblecontroller" "-DCMAKE_C_FLAGS:STRING=-include $rtos_config"
make "${build_args[@]}"
```

## Startup

The SDK starts Wi-Fi and its manager in the SYSINIT `WIRELESS` record before
`main()`. Wait for the shell prompt and, on BL618DG, the `RF is ready` boot
log. The shell can be ready before the application registers its event logger,
so `CODE_WIFI_ON_MGMR_DONE` may be absent. Use
`wifi_coex_debug_board_config show` to inspect the BL618DG topology in the
running firmware.

For Bluetooth on any supported chip, run `wifi_bt_init` once after the shell
is ready and wait for the `BD_ADDR` log.

```text
wifi_bt_init
```

These STA and AP commands use 5 GHz on BL618DG. For 2.4 GHz examples, see the
[coexistence guide](COEX_GUIDE.md). Replace the connection parameters and, in
STA mode, wait for the link and IP address.

```text
wifi_sta_connect -b <AP_BSSID> -c 36 <SSID_5G> <password>
```

For a 5 GHz AP, set the channel and requested bandwidth.

```text
wifi_ap_start -s COEX_5G_AP -a WPA2 -k <passphrase> -c 36 -b 1 -I 192.168.169.1
```

`-b 0`, `-b 1` and `-b 2` request 20, 40 and 80 MHz respectively. Keep WMM
enabled for wide channels, and select a channel supported by the board and
local regulations. Wait for `AP-ENABLED`, check the operating channel and
bandwidth, then verify that the peer obtains an IP address and can reach the AP.
Use `wifi_coex_status` once the link is ready.

Hardware coexistence is automatic. Single-antenna SPDT uses dynamic PTA before
connecting and on 2.4 GHz; stable 5 GHz uses fixed-BT. Scan temporarily uses
dynamic PTA and restores the valid home configuration. Do not manually switch
GPIO/RF paths while operating. Changing the board topology requires changing the
Kconfig selection, rebuilding, and reflashing; reboot after an RF initialization
failure.

## Coex Commands

| Command | Purpose |
|---|---|
| `wifi_coex_start` | Explicitly enable PS-PTA on a supported, connected 2.4 GHz single STA |
| `wifi_coex_stop` | Disable PS-PTA; retain the hardware baseline and Wi-Fi link |
| `wifi_coex_status` | Query hardware readiness, active configuration and PS-PTA runtime |
| `wifi_coex_duty_set <10-90>` | Set Wi-Fi active-window duration in milliseconds |
| `wifi_coex_protection <0\|1>` | Independently disable/enable connection-stage Protection |

No hardware-only start is needed. Start takes no mode argument and is not used
for AP, STA+AP, 5 GHz or dual-antenna parallel operation. Before PS-PTA start,
ordinary STA PS must be off. PS-PTA is not automatically restarted after reconnect.

Protection defaults to off. Enable it after stack initialization and before a
supported scan/connect operation if the application needs it. Stop does not
disable Protection. See the Guide for its topology/band policy.

`active=1` does not mean PS-PTA is running: check `ps_pta_running`. A healthy
5 GHz hardware-only connection reports, for example:

```text
coex hardware_configured=1 active=1 runtime=hardware_only ps_pta_running=0 band=5g duty=10 ms
```

The duty value is the saved setting, not evidence of active time slicing.
Check command errors before proceeding; a failed status query is not a valid
snapshot of the previous configuration.

## Bluetooth SPP

BL618DG BR/EDR SPP is separate from Wi-Fi and Coex initialization. After
`wifi_bt_init` completes:

```text
bredr_init
bredr_connectable 1
bredr_discoverable 1
```

Pair an SPP-capable peer and open its SPP connection. ACL connection alone is
not an SPP data connection. Re-enable connectable/discoverable before reconnecting.
Once SPP is connected, DUT TX can be started with:

```text
bredr_discoverable 0
spp_tp_start all 672 0
```

Stop the sender with `spp_tp_stop all`. For DUT RX, send from the peer instead.
BL616/BL616CL use application BLE services, not these BR/EDR commands.

## Throughput and CPU Measurement

Use PC **iperf2**. TX/RX is relative to the DUT. Replace `PC_IP`, `DUT_IP`,
`PORT`, `VIF` and `UDP_RATE`. Use `VIF=0` for STA or `VIF=1` for AP. Start the
receiver first.

| Direction | Receiver | Sender |
|---|---|---|
| TCP TX | PC: `iperf -s -B PC_IP -p PORT -i 1` | DUT: `iperf -c PC_IP -p PORT -t 60 -I VIF` |
| TCP RX | DUT: `iperf -s -p PORT -I VIF` | PC: `iperf -c DUT_IP -B PC_IP -p PORT -t 60 -i 1` |
| UDP TX | PC: `iperf -s -u -B PC_IP -p PORT -i 1` | DUT: `iperf -c PC_IP -p PORT -t 60 -I VIF -u -b UDP_RATE -l 1470` |
| UDP RX | DUT: `iperf -s -p PORT -I VIF -u` | PC: `iperf -c DUT_IP -B PC_IP -p PORT -t 60 -i 1 -u -b UDP_RATE -l 1470` |

For STA, use a PC Ethernet connection to the same router. For AP, connect the
PC Wi-Fi adapter to the DUT. Run `iperf stop` on the DUT between tests. Run each
direction twice and use the receiver summary for throughput. For UDP saturation
tests, increase `-b` until receiver throughput plateaus. Record the requested rate,
actual sender rate and receiver loss. For concurrent Wi-Fi and SPP tests, compare
overlapping traffic windows and use the same link conditions as the single-load test.

For CPU statistics, add `CONFIG_PS_EXTEND=y` to the build. In a separate profiling
run, call `ps_extend` once to establish a reference and sample with a fixed
interval. `Usage(x% Ns)` is CPU busy over that interval, including Trap time.
The command allocates memory and prints a task table. Collect diagnostic
snapshots between throughput runs to avoid UART output during the measurements.

## BL618DG AP40 Throughput (2026-11-20)

- Firmware from SDK `4c3d239b9`, WiFi6 `752bdfe2`, MACSW `04abe84c` and lwIP
  `eca87730`, with `CONFIG_PSRAM=y CONFIG_PSRAM_COPY_CODE=y CONFIG_PSRAM_XIP=n`.
  The build used the default MACSW profile, the `CPU_MODEL=b0` selector for
  the B1 board, and the generated FreeRTOS configuration for the standalone
  Bluetooth controller.
- DUT `bl618dg-b1-single_ant_ft2232-01` in AP mode, standalone single antenna
  with SPDT on GPIO18. Peer `mt7921u-capture-01` in STA mode, with power saving
  disabled. Both devices were in RF box A.
- 5 GHz channel 36, 40 MHz, primary frequency 5180 MHz and center frequency
  5190 MHz as read back during the test. Bluetooth was initialized and idle.
  Coexistence reported `hardware_only`, with PS-PTA and Protection off.
- IPv4 single stream, **60 seconds × 2** per direction. UDP used 1470-byte
  datagrams with requested rates of `320M` (DUT TX) and `240M` (DUT RX).

AP command for the open network in the RF box.

```text
wifi_ap_start -s BL618DG_AP40_B1_CURRENT -c 36 -b 1 -I 192.168.169.1 -S 2 -L 100
```

| Direction | Run 1 (Mbit/s) | Run 2 (Mbit/s) | Mean (Mbit/s) | UDP loss, both runs |
|---|---:|---:|---:|---:|
| TCP TX | 56.5 | 55.1 | 55.8 | — |
| TCP RX | 89.7 | 90.1 | 89.9 | — |
| UDP TX | 146.0 | 147.0 | 146.5 | 0 / 1,493,437 (0%) |
| UDP RX | 109.0 | 110.0 | 109.5 | 2 / 1,117,653 (0.000179%) |

TCP RX exceeded 80 Mbit/s in both runs. In the snapshots between tests, TCP TX
raised the cumulative lwIP heap peak to 96,228 of 98,304 bytes and the
allocation-failure count to 291. The measured heap in use returned to 76 bytes,
and the cumulative failure count stayed at 291 through the remaining tests.

## Diagnostics

The demo enables the debug CLI. These are optional inspections, not setup steps:

```text
wifi_coex_debug_status
wifi_coex_debug_context
wifi_coex_debug_board_config show
wifi_coex_debug_resolve hardware_only
wifi_coex_debug_resolve ps_pta
```

Resolver output does not apply a hardware configuration. `spdt_2g` and `spdt_bt`
are isolated board diagnostics and are blocked while the hardware is managed.
PS-PTA stop does not release that hardware ownership. Do not use raw RF/register
commands to bypass an initialization or configuration error.

## PSRAM Reference Results (2026-09-16)

Historical measurements from `wifi_bz_coexm` on `bl618dg-cp2102n-01` with a
standalone single antenna, GPIO18, default PHYRF, PSRAM code copy, and
PS-PTA/Protection off. Wi-Fi used 5 GHz channel 36. For DUT STA, the peers were
an ASUS AX88U and an Ethernet PC. For DUT AP, the MT7921U peer used 20 MHz with
power saving enabled. The SPP peer was the AIC Bluetooth controller.

Rates are receiver measurements, normally averaged over **2 x 30 seconds**.
BT-only retains an idle Wi-Fi connection/AP; it does not mean Wi-Fi radio off.
CPU is mean busy from `ps_extend`, normally eight valid samples per row.

### Single Load

| Test | STA rate | STA CPU % | AP rate | AP CPU % |
|---|---:|---:|---:|---:|
| UDP TX, Mbit/s | 178.00 | 92.65 | 85.75 | 43.72 |
| UDP RX, Mbit/s | 277.00 | 98.86 | 78.80 | 52.51 |
| TCP TX, Mbit/s | 38.45 | 71.23 | 34.30 | 62.77 |
| TCP RX, Mbit/s | 85.20 | 77.99 | 50.20 | 60.66 |
| BT TX, KiB/s | 123.36 | 6.47 | 123.20 | 6.57 |
| BT RX, KiB/s | 123.45 | 22.19 | 122.84 | 22.13 |

### Concurrent Load

Wi-Fi rates are Mbit/s; BT rates are KiB/s (1024 bytes/s).

| Test | STA Wi-Fi | STA BT | STA CPU % | AP Wi-Fi | AP BT | AP CPU % |
|---|---:|---:|---:|---:|---:|---:|
| UDP TX + BT TX | 153.50 | 111.96 | 88.30 | 82.80 | 120.78 | 52.70 |
| UDP TX + BT RX | 133.50 | 118.12 | 93.68 | 82.05 | 110.27 | 66.32 |
| UDP RX + BT TX | 259.00* | 75.32* | 99.56* | 71.15 | 61.33 | 51.07 |
| UDP RX + BT RX | 206.50 | 121.71 | 99.61 | 77.80 | 47.08 | 61.89 |
| TCP TX + BT TX | 34.90 | 87.19 | 73.56 | 32.80 | 105.93 | 71.26 |
| TCP TX + BT RX | 31.85 | 118.58 | 84.59 | 30.75 | 73.18 | 75.12 |
| TCP RX + BT TX | 79.40 | 82.93 | 81.08 | 43.15 | 76.39 | 58.12 |
| TCP RX + BT RX | 71.55 | 116.03 | 89.12 | 47.15 | 39.28 | 66.54 |

`*` STA UDP RX + BT TX has one valid 30-second run and two CPU samples.

- UDP offered-load arguments: STA TX/RX `300M`, AP TX `160M`, AP RX `80M`.
  STA UDP RX loss was 11.6163% alone, 17.4912% with BT TX, and 34.2106% with
  BT RX. These are saturation results, not recommended lossless operating rates.
- UDP TX had zero recorded loss; AP UDP RX loss was below 0.001%. AP RX values
  reflect this peer/link setup, not a demonstrated absolute DUT limit.
- AP and STA use different peers and link conditions; their difference cannot
  be attributed solely to the Wi-Fi role. These short windows do not establish
  long-term stability or guaranteed product throughput.
