# MQuickJS Example

This example runs the `mquickjs` VM component on BL616/BL618 with LittleFS POSIX file APIs.

## Layout

- `lfs/tests`: bundled upstream JavaScript tests
- `lfs/tests2`: bundled ES5 test suite
- `pack_js_media.sh`: example-local media pack/flash helper

The normal SDK build generates `build/build_out/littlefs.bin` directly from the
example-local `lfs/` tree, and `flash_prog_cfg.ini` flashes it to the `media`
partition with `address = @partition`. On a 4MB BL616DK flash, the example now
uses the board default partition table directly.

## Build

```bash
cd examples/mquickjs
make CHIP=bl616 BOARD=bl616dk
```

## Flash

```bash
make flash CHIP=bl616 COMX=/dev/ttyUSB0
```

## Media Only

Regenerate the media image from the bundled example scripts:

```bash
./pack_js_media.sh
```

Flash only the media partition:

```bash
./pack_js_media.sh --flash /dev/ttyUSB0
```

Pack only selected directories from `lfs/`:

```bash
./pack_js_media.sh tests2
```

## Shell Commands

- `js_run <file>`: run JavaScript file from `/lfs`
- `js_eval "<expr>"`: evaluate an inline JavaScript expression
- `js_status`: show VM status
- `js_info`: show VM configuration
- `lfs_ls`, `lfs_cat`, `lfs_write`, `lfs_rm`, `lfs_mkdir`, `lfs_cd`, `lfs_pwd`, `lfs_info`: LittleFS helpers

## Example Test Commands

```text
js_run /lfs/tests/test_rect.js
js_run /lfs/tests2/run_es5.js
```

## One-Click Serial Validation

Run all bundled `tests/*.js` over serial, plus `tests2` as an ES5 suite by default:

```bash
./run_serial_tests.py /dev/ttyACM1
```

Useful options:

```bash
./run_serial_tests.py /dev/ttyACM1 --tests2-mode suite
./run_serial_tests.py /dev/ttyACM1 --tests2-mode individual
./run_serial_tests.py /dev/ttyACM1 --tests2-mode both
```

The script will:

- reset the board and wait for the shell prompt
- check `lfs_ls /lfs` and `js_status`
- run all `lfs/tests/*.js`
- run `lfs/tests2/run_es5.js` by default, or individual ES5 modules if requested
- write a full log under `build/serial_test_logs/`

## Notes

- `defconfig` enables `CONFIG_MQUICKJS`, `CONFIG_NEWLIB_LITTLEFS`, `CONFIG_LITTLEFS`, `CONFIG_FREERTOS`, and `CONFIG_SHELL`.
- `CONFIG_POST_BUILDS_GENERATE_LITTLEFS` generates `build/build_out/littlefs.bin`.
- LittleFS initialization tries partitions in order: `media`, then `PSM`.
