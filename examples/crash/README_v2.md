# crash_v2

## Support linux/win/macos

## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|bl616/bl618       |        |

## Compile

- bl616/bl618

```
enable set(CONFIG_COREDUMP_V2 1) in proj.conf.

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
(bl616)
../../tools/bflb_tools/bouffalo_flash_cube/BLFlashCommand-ubuntu --chip bl616 --read --start 0x2f8000 --len 0x80000 --file crash.bin --port /dev/ttyACM0
mv ../../tools/bflb_tools/bouffalo_flash_cube/crash.bin .

(bl616 CONFIG_PSRAM = 1)
../../tools/bflb_tools/bouffalo_flash_cube/BLFlashCommand-ubuntu --chip bl616 --read --start 0x6e0000 --len 0x480000 --file crash.bin --port /dev/ttyACM0
mv ../../tools/bflb_tools/bouffalo_flash_cube/crash.bin .
```

3. Start gdb server emulator

```
../../tools/crash_tools/v2/bugkiller-ubuntu
```

4. Start debug

```
riscv64-unknown-elf-gdb -x gdb.init
```

## coredump in log usage

1. Enter crash in cli, wait log print.

2. Save log to 1.log file, and capture all coredump from 1.log file

```
../../tools/crash_tools/crash_capture_v2.pl 1.log
mv ../../tools/crash_tools/output/coredump1 crash.bin
```

3. Start gdb server emulator

```
../../tools/crash_tools/v2/bugkiller-ubuntu
```


4. Start debug

```
riscv64-unknown-elf-gdb -x gdb.init
```
