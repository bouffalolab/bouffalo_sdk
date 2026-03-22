# gpio_validation_cli

GPIO validation case for command-driven chip validation. DUT and tester share the same `gpiov_*` CLI protocol:

- DUT mode: default build, uses native MCU GPIOs
- tester mode: `CONFIG_GPIO_TESTER=y`, uses PCA9698 I/O expanders over I2C

## Support CHIP

| CHIP              | Remark |
|:-----------------:|:------:|
| BL616/BL618       | Verified as v1 target |

## Build

### DUT build

```bash
make CHIP=bl616 BOARD=bl616dk
```

### Tester build

Tester mode maps logical tester GPIOs like this:

- `I2C0 + 0x20` -> tester GPIO `0..39`
- `I2C1 + 0x20` -> tester GPIO `40..79`
- both PCA9698 devices use `AD0/AD1/AD2 = 0`

Example build:

```bash
make CHIP=bl616 BOARD=bl616dk \
  CONFIG_GPIO_TESTER=y \
  CONFIG_GPIO_TESTER_I2C0_SDA_PIN=11 \
  CONFIG_GPIO_TESTER_I2C0_SCL_PIN=14 \
  CONFIG_GPIO_TESTER_I2C1_SDA_PIN=16 \
  CONFIG_GPIO_TESTER_I2C1_SCL_PIN=17
```

Available tester-mode config items:

- `CONFIG_GPIO_TESTER`
- `CONFIG_GPIO_TESTER_I2C_FREQ`
- `CONFIG_GPIO_TESTER_I2C0_SDA_PIN`
- `CONFIG_GPIO_TESTER_I2C0_SCL_PIN`
- `CONFIG_GPIO_TESTER_I2C1_SDA_PIN`
- `CONFIG_GPIO_TESTER_I2C1_SCL_PIN`
- `CONFIG_GPIO_TESTER_I2C0_ADDR`
- `CONFIG_GPIO_TESTER_I2C1_ADDR`

Default values:

- `CONFIG_GPIO_TESTER_I2C_FREQ=400000`
- `CONFIG_GPIO_TESTER_I2C0_ADDR=0x20`
- `CONFIG_GPIO_TESTER_I2C1_ADDR=0x20`

## Flash

```bash
make flash CHIP=bl616 COMX=/dev/ttyUSB0
```

## DUT/tester behavior

- DUT mode supports input, output and irq validation.
- tester mode supports input/output only.
- In tester mode:
  - `gpiov_config ... irq` and all `gpiov_irq_*` commands return `GPIOV FAIL reason=irq_not_supported`
  - `pull=...`, `drive=...`, `trig=...` are accepted for protocol compatibility
  - unsupported tester-side options are ignored and returned as `GPIOV OK warning=ignored_*`

The host validation script sends the same `gpiov_*` commands to both serial ports.

## Validation Configuration

This case now includes:

- `run_full_validation.py`
- `gpio_validation_config.example.json`

The configuration is centralized in one JSON file. It contains:

- DUT build/flash configuration
- DUT test serial settings
- Tester board serial settings
- DUT pin group to tester pin group mapping
- Output scenario configuration
- Input scenario configuration
- Interrupt scenario configuration

Default one-click config file:

```text
examples/peripherals/gpio/gpio_validation_cli/gpio_validation_config.example.json
```

Scenario fields are explicit in the JSON:

- `scenarios.output`: `enabled`, `settle_ms`, `dut_drive`, `dut_pull`
- `scenarios.input`: `enabled`, `settle_ms`, `dut_pull`
- `scenarios.interrupt`: `enabled`, `settle_ms`, `restore_settle_ms`, `active_hold_ms`, `irq_read_chunk_size`, `irq_read_retry_max`, `irq_read_retry_delay_ms`, `timeout_ms`, `expected_delta`, `dut_pull`, `triggers`

Run the one-click flow directly like this:

```bash
python3 run_full_validation.py --config gpio_validation_config.example.json
```

This wrapper does:

- DUT build
- DUT flash
- tester / DUT run-mode reset via serial control lines
- output / input / interrupt validation

By default it creates:

- an Excel report under `<output_dir>/gpio_validation_report.xlsx`
- a detailed serial command log under `<output_dir>/gpio_ci_test.log`
- a machine-readable summary under `<output_dir>/gpio_validation_results.json`

During execution, the wrapper also prints real-time progress to the terminal for:

- each validation stage
- each pin group / pull / trigger combination
- each individual pin result

`dut_pull` supports either a single string or an ordered array. Example:

```json
"dut_pull": ["float", "pull_up", "pull_down"]
```

When an array is used, `run_full_validation.py` runs the whole pin group once per pull mode, in the listed order:

1. all target pins under `float`
2. all target pins under `pull_up`
3. all target pins under `pull_down`

Tester-side `drive` and `pull` are no longer exposed as JSON config items. The tester board only needs to read DUT levels or drive explicit `0/1` levels into DUT. The script uses fixed tester-side settings internally.

For interrupt testing:

- `idle_level` means the steady level before the interrupt stimulus.
- `active_level` means the driven level during the interrupt stimulus.
- `active_hold_ms` controls how long the tester keeps the active interrupt level before restoring idle.
- For `sync_low`, `sync_high`, `async_low`, and `async_high`, the script restores idle first and then reads back IRQ counts, so level-triggered sources do not stay asserted during result checking.
- When `active_hold_ms` is not set on an edge trigger item, edge-trigger validation keeps using `restore_settle_ms` as the default pulse width.
- `irq_read_chunk_size` controls how many pins are included in each `gpiov_irq_get_many` readback chunk.
- `irq_read_retry_max` controls how many times the script retries a malformed or incomplete IRQ readback chunk before marking that pin as failed.
- `irq_read_retry_delay_ms` controls the delay between IRQ readback retries.
- Level-trigger IRQ counts are read back in smaller chunks to avoid oversized serial responses when testing large pin groups, and each chunk retry count is recorded in the final interrupt report.

Typical examples:

- `trigger=sync_rise`: `idle_level=0`, `active_level=1`
- `trigger=sync_fall`: `idle_level=1`, `active_level=0`

## Validation Logs

- `run_full_validation.py` writes a detailed step-by-step serial command log to:

```text
<output_dir>/gpio_ci_test.log
```

This log records:

- every DUT command
- every tester command
- every returned `GPIOV OK` / `GPIOV FAIL`
- current scenario, target pin and group name

The same high-level progress is also mirrored into the wrapper log, so you can check stage and per-pin status even while the test is still running.

So when a failure happens, you can see exactly which side sent which command and which response failed.

## CLI interface

All commands return one line beginning with `GPIOV OK` or `GPIOV FAIL`, so the validation script can parse them directly.

### Basic commands

```text
gpiov_ping
gpiov_help
gpiov_status <pin>
gpiov_status_many <pins_csv>
gpiov_deinit <pin>
```

### Single-pin commands

```text
gpiov_config <pin> <in|out|irq> [pull=up|down|float] [drive=0|1|2|3] [init=0|1] [trig=sync_fall|sync_rise|sync_low|sync_high|sync_both|async_fall|async_rise|async_low|async_high]
gpiov_write <pin> <0|1>
gpiov_read <pin>
gpiov_read_many <pins_csv>
gpiov_pulse <pin> <count> <high_ms> <low_ms>
gpiov_irq_get <pin>
gpiov_irq_get_many <pins_csv>
gpiov_irq_clear <pin>
gpiov_irq_wait <pin> <expected_count> <timeout_ms>
```

### Strict multi-pin commands

These are the commands intended for rigorous chip validation:

```text
gpiov_config_many <pins_csv> <in|out|irq> [pull=up|down|float] [drive=0|1|2|3] [init=0|1] [trig=...]
gpiov_write_pattern <pins_csv> <target_pin> <target_level> <other_level>
gpiov_read_expect <pins_csv> <target_pin> <target_level> <other_level>
gpiov_irq_clear_many <pins_csv>
gpiov_irq_expect <pins_csv> <target_pin> <expected_delta> <timeout_ms>
```

In tester mode, the main host-facing commands are:

- `gpiov_config_many`
- `gpiov_write_pattern`
- `gpiov_read_expect`
- `gpiov_status`
- `gpiov_status_many`
- `gpiov_deinit`

Example pin group:

```text
0,1,2,23,24
```

## Notes for validation use

- `bl616dk` console uses GPIO21/22 and `bl616cldk` console uses GPIO34/35. This case protects those pins from reconfiguration.
- Exclude boot, console, JTAG, flash and any board-sensitive pins from the validation pin list.
- The external tester board should connect to DUT pins one-to-one and be able to switch each tester pin between input and output.
- For rigorous validation, do not judge only whether the target pin responds. Judge whether only the target pin responds and every non-target pin stays at the expected state.

## Recommended validation topology

- DUT runs this `gpio_validation_cli` firmware.
- Tester board should already be programmed with the tester firmware variant of `gpio_validation_cli`.
- Host PC sends shell commands to DUT and to the external tester board.
- External tester board controls or samples all linked GPIOs in parallel.
- DUT firmware is responsible for deterministic pin configuration and local checks.
- Tester board is responsible for electrical observation and final end-to-end pass/fail orchestration.

## GPIO output validation

Goal:

- Verify the target DUT GPIO can drive `0` and `1`.
- Verify every non-target DUT GPIO keeps the opposite expected level.
- Verify there is no pin swap, mirror response or shorted behavior.

Preparation:

```text
gpiov_config_many 0,1,2,23,24 out pull=float drive=0 init=1
```

Per target pin `X`, run two patterns.

Case A: target outputs `0`, others output `1`

```text
gpiov_write_pattern 0,1,2,23,24 X 0 1
```

Tester board expectation:

- Only DUT pin `X` reads low.
- Every other pin in the group reads high.

Case B: target outputs `1`, others output `0`

```text
gpiov_write_pattern 0,1,2,23,24 X 1 0
```

Tester board expectation:

- Only DUT pin `X` reads high.
- Every other pin in the group reads low.

Recommended sequence:

```text
gpiov_write_pattern 0,1,2,23,24 0 0 1
gpiov_write_pattern 0,1,2,23,24 0 1 0
gpiov_write_pattern 0,1,2,23,24 1 0 1
gpiov_write_pattern 0,1,2,23,24 1 1 0
...
```

This sequence matches the strict pattern you described: each round selects exactly one target pin and forces the rest of the group to the opposite level.

## GPIO input validation

Goal:

- Verify the target DUT GPIO reads the level driven by the tester board.
- Verify every non-target DUT GPIO reads the expected opposite level.
- Verify there is no wrong-pin response.

Preparation:

```text
gpiov_config_many 0,1,2,23,24 in pull=float
```

Per target pin `X`, the tester board drives two patterns.

Case A: tester drives target `X` high, others low

- Tester drives: `X=1`, others `0`
- DUT checks:

```text
gpiov_read_expect 0,1,2,23,24 X 1 0
```

Case B: tester drives target `X` low, others high

- Tester drives: `X=0`, others `1`
- DUT checks:

```text
gpiov_read_expect 0,1,2,23,24 X 0 1
```

Pass condition:

- DUT returns `GPIOV OK`.
- Returned `levels=` field shows only the target pin at the target level and all other pins at the opposite level.

Fail examples caught by this command:

- Target pin level is wrong.
- Another pin follows the target pin unexpectedly.
- One or more non-target pins stay at the wrong level.

## GPIO interrupt validation

Goal:

- Verify the target DUT GPIO interrupt can be triggered.
- Verify non-target DUT GPIO interrupts do not trigger during the same stimulus.
- Verify there is no cross-response between interrupt pins.

Preparation for rising-edge validation:

```text
gpiov_config_many 0,1,2,23,24 irq pull=down trig=sync_rise
```

Per target pin `X`:

1. Clear the whole group interrupt counters.

```text
gpiov_irq_clear_many 0,1,2,23,24
```

2. Tester board injects one rising edge only on pin `X`. Other pins stay quiet.

3. DUT checks:

```text
gpiov_irq_expect 0,1,2,23,24 X 1 100
```

Pass condition:

- Target pin delta count is exactly `1`.
- Every non-target pin delta count is exactly `0`.

Fail examples caught by this command:

- Target pin does not trigger.
- Another pin also triggers.
- Target pin triggers more than once because of glitch or bounce.

For falling-edge validation, reconfigure once and repeat the same pattern:

```text
gpiov_config_many 0,1,2,23,24 irq pull=up trig=sync_fall
```

## Suggested automation flow

For each validation group:

1. Output test: DUT sets strict output patterns, tester samples all lines.
2. Input test: tester drives strict input patterns, DUT checks all lines.
3. Interrupt test: tester injects one interrupt source at a time, DUT verifies exact pin ownership.

This split gives two layers of confidence:

- Electrical behavior is checked by the tester board.
- Logical ownership of the response is checked by DUT-side strict commands.
