# crash_v1

## Support only linux

## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|bl616/bl618       |        |

## Compile

- bl616/bl618

```
make CHIP=bl616 BOARD=bl616dk
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```

## coredump in flash usage

1. Enter crash in cli, wait log print. At the same time, a coredump will be generated in the core partition.

2. Get bin format crash binary, need to enter burning mode(CHIP_EN + RESET) first

```
(bl616dk)
./tools/bflb_tools/bouffalo_flash_cube/BLFlashCommand-ubuntu --chip bl616 --read --flash --start 0x2f8000 --len 0x80000 --file crash.bin --port /dev/ttyACM0

(bl616dk CONFIG_PSRAM = 1)
./tools/bflb_tools/bouffalo_flash_cube/BLFlashCommand-ubuntu --chip bl616 --read --flash --start 0x6e0000 --len 0x480000 --file crash.bin --port /dev/ttyACM0
```

3. Start Debug
./tools/byai/coredump.py crash.bin examples/crash/build/build_out/crash_bl616.elf

## coredump in log usage

1. Start Debug
./tools/byai/coredump.py your.log examples/crash/build/build_out/crash_bl616.elf

2. If your want split multiple coredump from log with `tools/byai/crash_capture.py`:

```
../../tools/byai/crash_capture.py 1.log
[crash_capture] find 1 coredump
....
[crash_capture] find n coredump
```

## note

- `tools/byai/crash_capture.py` writes split coredumps as plain UTF-8 text with Unix `\n` line endings.
- `tools/byai/coredump.py` reuses `tools/byai/crash_capture.py` parsing logic and always loads the last valid coredump from the selected file.
