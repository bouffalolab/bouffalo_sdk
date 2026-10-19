# wifi_bz_coexm

Wi-Fi/BLE coexistence demo for BL616 and BL616CL, with additional BL618DG
BR/EDR support. See [COEX_GUIDE.md](COEX_GUIDE.md) for topology selection,
STA/AP examples, PS-PTA, Protection and application APIs.

## Build

Run from `examples/coex/wifi_bz_coexm`:

```sh
make CHIP=bl616 BOARD=bl616dk
make CHIP=bl616cl BOARD=bl616cldk
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

Choose the command matching the board. BL618DG uses the SDK's default PHYRF
library, including for standalone operation; no PHYRF CMake changes or library
override are needed.

### BL618DG Flash / PSRAM

Use separate build directories and a board with compatible PSRAM:

```sh
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap BUILD_DIR=build_flash \
    CONFIG_PSRAM=n CONFIG_PSRAM_COPY_CODE=n

make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap BUILD_DIR=build_psram \
    CONFIG_PSRAM=y CONFIG_PSRAM_COPY_CODE=y
```

`CONFIG_PSRAM_COPY_CODE=y` selects code execution from PSRAM; enabling PSRAM
alone does not. Use the artifacts from the selected build directory when
flashing. Keep `COEX_SPDT_EXPERIMENT` off for normal operation.

For integration testing on a real standalone dual-antenna board:

```sh
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap BUILD_DIR=build_dual_ant \
    COEX_DUAL_ANT_VALIDATION=ON
```

This enables the dual-antenna parallel hardware configuration, not PS-PTA.
It is disabled in the default build and requires board-level validation.

## Startup

On BL618DG, declare the board wiring, select its RF mode, then run `wifi_bt_init`:

| Hardware | Declare wiring | Prepare RF | Wi-Fi operation |
|---|---|---|---|
| Combo, no external SPDT | `wifi_coex_board_config combo` | `wifi_coex_init combo` | 2.4 GHz STA/AP |
| Standalone single antenna, SPDT + diplexer | `wifi_coex_board_config standalone_single_ant <gpio>` | `wifi_coex_init standalone` | 2.4/5 GHz STA/AP |
| Standalone dual antenna, no SPDT | `wifi_coex_board_config standalone_dual_ant` | `wifi_coex_init standalone` | 2.4/5 GHz STA/AP, validation build |

Declaration records immutable hardware facts without changing RF or GPIO. Supply
the actual SPDT control GPIO and verify the switch polarity against the BSP.
There is no default GPIO. BL616/BL616CL prepare RF during boot and start at
`wifi_bt_init`, without a topology command.

`wifi_coex_init` requires a declaration and accepts only `combo` or `standalone`.
Unsupported board/mode combinations are rejected before RF preparation; it does
not switch mode automatically. A successful prepare is not an already-running
hardware baseline. Use `wifi_coex_board_config show` to inspect both layers.

For example, on the tested single-antenna board whose GPIO18 high level selects BT:

```text
wifi_coex_board_config standalone_single_ant 18
wifi_coex_init standalone
wifi_bt_init
```

Wait for `CODE_WIFI_ON_MGMR_DONE` and successful Bluetooth initialization.
Then choose one Wi-Fi role, replacing the connection parameters as needed:

STA:

```text
wifi_sta_connect -b <AP_BSSID> -c 36 <SSID_5G> <password>
```

AP:

```text
wifi_ap_start -s COEX_5G_AP -k 12345678 -c 36 -I 192.168.169.1 -S 2 -L 100
```

Wait for STA connection/IP or `CODE_WIFI_ON_AP_STARTED`, then run
`wifi_coex_status`. Use a legal channel matching the target network. For 2.4 GHz,
change the STA parameters or AP channel, not the physical topology.

Hardware coexistence is automatic. Single-antenna SPDT uses dynamic PTA before
connecting and on 2.4 GHz; stable 5 GHz uses fixed-BT. Scan temporarily uses
dynamic PTA and restores the valid home configuration. Do not manually switch
GPIO/RF paths while operating; reboot before changing topology or after an RF
initialization failure.

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
coex hardware_configured=1 active=1 runtime=hardware_only ps_pta_running=0 band=5g duty=50 ms
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
`PORT` and `VIF`; use `VIF=0` for STA or `VIF=1` for AP. Start the receiver first.

| Direction | Receiver | Sender |
|---|---|---|
| TCP TX | PC: `iperf -s -B PC_IP -p PORT -i 1` | DUT: `iperf -c PC_IP -p PORT -t 30 -i 1 -I VIF` |
| TCP RX | DUT: `iperf -s -p PORT -i 1 -I VIF` | PC: `iperf -c DUT_IP -B PC_IP -p PORT -t 30 -i 1` |
| UDP TX | PC: `iperf -s -u -B PC_IP -p PORT -i 1` | DUT: `iperf -c PC_IP -p PORT -t 30 -i 1 -I VIF -u -b 80M -l 1470` |
| UDP RX | DUT: `iperf -s -p PORT -i 1 -I VIF -u` | PC: `iperf -c DUT_IP -B PC_IP -p PORT -t 30 -i 1 -u -b 80M -l 1470` |

For STA, use a PC Ethernet connection to the same router. For AP, connect the
PC Wi-Fi adapter to the DUT. Run `iperf stop` on the DUT between tests.
For UDP saturation tests, increase `-b` from 80M until receiver throughput
plateaus; record actual sender rate and loss. A high-loss plateau is not lossless
throughput. Compare concurrent and single-load tests under the same conditions
and over overlapping Wi-Fi/SPP windows.

For CPU statistics, add `CONFIG_PS_EXTEND=y` to the build. Run `ps_extend` once
to establish a reference, then sample at fixed intervals, such as every 5 seconds.
`Usage(x% Ns)` is CPU busy over that interval and already includes Trap time;
do not add Trap again. Keep sampling frequency consistent between tests because
the command allocates memory and prints a task table.

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

Historical measurements from `wifi_bz_coexm`, not a new validation of the current
checkout. DUT: `bl618dg-cp2102n-01`, standalone single antenna, GPIO18, default
PHYRF, PSRAM code-copy, PS-PTA/Protection off. Wi-Fi used 5 GHz channel 36;
STA used an ASUS AX88U and Ethernet PC, AP used MT7921U at 20 MHz with peer power
saving enabled. The SPP peer was the AIC Bluetooth controller.

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

Sample counts, loss, relative changes and test evidence are in the
[full test report](../../../docs/coex/validation/618dg_psram_sta_ap_bt_full_retest_20260916.md).
