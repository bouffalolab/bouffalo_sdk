# crash_v1

## Support only linux

## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|bl616/bl618       |        |

## Compile

- bl616/bl618
- comment CONFIG_COREDUMP_V2, default is v1

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
../../tools/bflb_tools/bouffalo_flash_cube/BLFlashCommand-ubuntu --chip bl616 --read --start 0x2f8000 --len 0x80000 --file crash.bin --port /dev/ttyACM0
mv ../../tools/bflb_tools/bouffalo_flash_cube/crash.bin .

(bl616dk CONFIG_PSRAM = 1)
../../tools/bflb_tools/bouffalo_flash_cube/BLFlashCommand-ubuntu --chip bl616 --read --start 0x6e0000 --len 0x480000 --file crash.bin --port /dev/ttyACM0
mv ../../tools/bflb_tools/bouffalo_flash_cube/crash.bin .
```

3. Start bugkiller server

```
../../tools/crash_tools/v1/bugkiller_linux_amd64 -d crash.bin -e build/build_out/crash_bl616.elf -p 1234
```

4. Start debug

comment `restore build/build_out/crash_bl616.elf` and `restore crash.bin` in gdb.init file.
```
riscv64-unknown-elf-gdb -x gdb.init -se build/build_out/crash_bl616.elf
```

## coredump in log usage

1. Enter crash in cli, wait log print.

2. Save log to 1.log file, and capture all coredump from 1.log file

```
../../tools/crash_tools/crash_capture.pl 1.log
[crash_capture] find 1 coredump
....
[crash_capture] find n coredump
```

3. Step 2 will generate n files, cordump1 ~ coredumpn, select the crash you are interested in.

```
path/to/your/bugkiller_dump -bin crash.bin -log ../../tools/crash_tools/output/coredump1
path/to/your/bugkiller -d crash.bin -e build/build_out/crash_bl616.elf -p 1234
```

4. Start debug

comment `restore build/build_out/crash_bl616.elf` and `restore crash.bin` in gdb.init file.
```
riscv64-unknown-elf-gdb -x gdb.init -se build/build_out/crash_bl616.elf
```

## note

lease kill bugkiller_linux_amd64 process after debug.
