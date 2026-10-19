# hbn_io_wakeup


## Support CHIP

| CHIP    | Remark |
|:-------:|:------:|
| BL616CL |        |

## Compile

- BL616CL

```
make CHIP=bl616cl BOARD=bl616cldk
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```

## Validation Script

This case now includes:

- `run_hbn_wakeup_validation.py`
- `hbn_wakeup_validation_config.example.json`

The JSON configuration centralizes:

- DUT build and flash parameters
- DUT serial port settings
- DFT serial port settings
- HBN modes under test
- DUT GPIO to DFT GPIO mapping
- Trigger modes under test
- Command retry count
- Trigger timing and wakeup timeout

Default example config path:

```text
examples/pmu/bl616cl/hbn_io_wakeup/hbn_wakeup_validation_config.example.json
```

Run the one-click flow like this:

```bash
python3 run_hbn_wakeup_validation.py --config hbn_wakeup_validation_config.example.json
```

The script does:

- DUT build
- DUT flash
- DUT and DFT serial open and optional run-mode reset
- DFT uses the pre-flashed tester firmware and is not rebuilt or reflashed by this script
- DFT idle/active GPIO drive per configured wakeup trigger
- DUT `hbn <hbn_mode> <test_io> <trig_type> <pull>` validation
- Excel report generation

Optional flags:

```bash
python3 run_hbn_wakeup_validation.py --config hbn_wakeup_validation_config.example.json --skip-build
python3 run_hbn_wakeup_validation.py --config hbn_wakeup_validation_config.example.json --skip-flash
python3 run_hbn_wakeup_validation.py --config hbn_wakeup_validation_config.example.json --skip-test
```

Artifacts are written under:

```text
examples/pmu/bl616cl/hbn_io_wakeup/validation_runs/<timestamp>/
```

The generated files are:

- `hbn_wakeup_validation.log`
- `hbn_wakeup_validation_results.json`
- `hbn_wakeup_validation_report.xlsx`

Excel report rules:

- one sheet per HBN mode, for example `hbn0`, `hbn1`
- first column is GPIO, merged across that GPIO's trigger rows
- second column is wakeup mode
- third column is `PASS` or `FAIL`
- `PASS` cell background is green
- `FAIL` cell background is red
