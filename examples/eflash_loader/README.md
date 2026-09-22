# EFlash Loader

This example is a portable RAM-resident eFlash loader. The current port supports
the BL618DG B0 AP core. It keeps the
command frame and reply format used by `BLFlashCommand`/`bflb_eflash_loader`,
but replaces the UART transport with a JTAG-accessible memory mailbox.

## Operation

1. The JTAG host parses the standard BootROM RAM `.bin`, writes each segment to
   the address in its segment header, sets the RISC-V `pc` from the bootheader,
   and resumes the core. The startup code initializes `sp`.
2. The loader initializes the clock, halts NP/LP, probes the SPI flash from
   the BL618DG eFuse flash-pin setting, and waits for a host request.
3. The host writes a request to the data area and then writes `HRDY` to the
   flag word. The loader executes the request, writes the reply, and writes
   `SACK` to the flag word.

The JTAG tool uses the ordinary SDK RAM image:

```text
build/build_out/eflash_loader_bl618dg_ap.bin
```

For BL618DG this file contains a 256-byte BootROM header followed by one or
more 16-byte segment headers and their payloads. FlashCube validates the image
metadata and CRCs, strips the metadata logically, and writes only each segment
payload to its declared RAM address. The same standard BootROM RAM image is
used for BootROM and J-Link/OpenOCD loading.

## BL618DG B0 addresses

| Item | Address/value |
| --- | --- |
| JTAG load address | `0xA1000400` |
| AP entry/PC | `0xA1000400` |
| ABI descriptor | `0x2106AFE0` |
| Mailbox flag (non-cache AP RAM) | `0x2106AFF0` |
| Mailbox data | `0x2106B000` |
| Mailbox data capacity | `20 KiB` |
| Mailbox end | `0x21070000` |
| Host request flag | `0x59445248` (`HRDY`) |
| Loader response flag | `0x4B434153` (`SACK`) |

The link region is limited to `0x68000` bytes so that the mailbox remains
outside the AP image. Do not place application data in this mailbox range
while the loader is running.

The addresses are defined in `port/bl618dg/flashloader_port_config.h`, not
derived from the linked image size. Loader feature growth therefore does not
move the mailbox as long as the image remains below the reserved region.

The 16-byte descriptor is published before the loader accepts commands. It is
four little-endian words: magic `0x4A4C4642` (`BFLJ`), protocol version,
capability bits, and shared-buffer size. FlashCube checks it after starting the
image, so an incompatible loader fails before any flash operation begins.
BL618DG currently publishes `0xFF`: Flash, Flash parameters, XZ, Flash OTP,
eFuse, reset, real XIP reads, and host-consumed reset acknowledgement.

## Command protocol

The request is identical to the eflash-loader command frame:

```text
offset 0: command             uint8
offset 1: checksum             uint8
offset 2: payload length       uint16 little-endian
offset 4: payload              bytes
```

The checksum is the low byte of the sum of the two length bytes and every
payload byte. A checksum of zero disables checksum verification, matching the
existing loader behavior.

An ordinary successful command returns `OK`. A failed command returns:

```text
`FL` + error_code(uint16 little-endian)
```

A data-returning command returns:

```text
`OK` + data_length(uint16 little-endian) + data
```

The handshake is the same as the JTAG path in FlashCube: write one zero byte
to the data area, write `HRDY`, wait for `SACK`, and read two bytes (`OK`).
The mailbox flag is deliberately left at `SACK` while the host reads a reply;
the host starts the next transaction by writing new data and `HRDY`.

Implemented command IDs are:

```text
20 change rate       21 reset             22 clock set
23 option finish     30 erase             31 write
32 read              34 XIP read          35 switch bank (bank 0)
36 JEDEC ID          37 read status       38 write status
3A write check       3B set parameter     3C chip erase
3D SHA256            3E XIP SHA256        3F XZ/decompress write
40 eFuse write        41 eFuse read        42 read eFuse MAC
43 write eFuse MAC
60 XIP read start    61 XIP read finish
A0 OTP erase          A1 OTP write         A2 OTP read
A3 OTP set parameter  A4 OTP get parameter A5 OTP lock by index
A6 OTP lock by addr   A7 OTP erase index   A8 OTP write index
A9 OTP read index
```

Range erase and chip erase publish `PD` before entering a long erase operation;
the host then polls at a reduced rate until the final response. `0x42` returns
the six-byte eFuse MAC followed by its little-endian
CRC32, matching BLFlashCommand's optional MAC check.

`0x3B` applies the same packed setting used by BLFlashCommand: flash pin,
clock source/divider, I/O mode, and clock delay. Its optional flash parameter
block must be exactly the SDK's 84-byte `spi_flash_cfg_type`; any other size is
rejected.

`0x33 FLASH_BOOT` accepts either an empty payload or four zero bytes. Both
`0x21 RESET` and `0x33` reset through the BootROM after the reply is published.
When capability bit 7 is set, FlashCube acknowledges that it consumed the reply
by clearing the mailbox and the loader then resets. A one-second timeout keeps
the reset commands usable with older hosts that do not clear the mailbox.
`0x60` enables the Flash XIP/cache path; `0x34` and `0x3E` then read and hash
through that path until `0x61` restores the serial-Flash controller state.
Security/ECDH eFuse commands `0x80`/`0x81` are not supported; FlashCube uses
the direct `0x40`/`0x41` commands in JTAG mode. BL618DG bank 1 is deliberately
unsupported, so `0x35` accepts bank 0 only.

## Build

From this directory:

```bash
make -j8
make test
make package FLASH_CUBE_ROOT=/path/to/bouffalo_flash_cube
```

The host tests cover frame parsing, checksum handling, reply encoding, erase
pending callbacks, parameter dispatch, eFuse/OTP dispatch, the MAC/CRC response
format, and a complete protocol/transport link with only the Flash capability.
The target build requires the SDK toolchain and produces the ELF and standard
BootROM RAM `.bin` under `build/build_out/`.

### CI

`tools/CI/config/build_all.tab` lists this example for `618dgap`, so the SDK
build matrix compiles it as `build_eflash_loader_618dgap` with `CHIP=bl618dg
BOARD=bl618dgdk CPU_ID=ap` and the usual `CONFIG_STRICT=y`. Reproduce the CI
build locally with:

```bash
make BUILD_DIR=build_eflash_loader_618dgap CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CONFIG_STRICT=y
make CHIP=bl618dg BUILD_DIR=build_eflash_loader_618dgap CPU_ID=ap whole
```

The matrix runs a `whole` step after every build. This loader is loaded into RAM
over JTAG and is never programmed into flash, so it has no `flash_prog_cfg.ini`
and no whole-flash image. The example overrides the SDK's flash-only `whole`
rule and copies the linked RAM image to `$(BUILD_DIR)/whole_flash_data.bin`,
which is the file the CI artifact collector publishes. The archived `.bin`/`.elf`
are therefore the RAM image itself, not a flash layout.

The host tests under `tests/` also run locally through `make test`, but they are
not yet part of the Jenkins pipeline.

## Adding another chip

Add `port/<chip>/flashloader_port.c` and
`port/<chip>/flashloader_port_config.h`. The port implements initialization and
the APIs needed by its declared capabilities; host-ack reset additionally needs
the millisecond clock.
Its config fixes the mailbox/data addresses inside RAM excluded from that
chip's linker region and declares its capabilities and buffer sizes. Commands
and their XZ/OTP/eFuse/XIP dependencies are compiled only when enabled. Put
chip-specific Kconfig values such as RAM size or `CONFIG_XZ` in
`port/<chip>/flashloader.mk`. An optional `port/<chip>/CMakeLists.txt` can
validate the core and add extra port sources. The protocol, memory transport,
and flash commands remain shared.

## FlashCube usage

The matching FlashCube source uses:

```text
jtag_load_addr = 0xA1000400
jtag_run_addr  = 0xA1000400
shake_hand_addr = 0x2106AFF0
data_addr       = 0x2106B000
```

The matching FlashCube tree ships this standard image as the BL618DG default.
Use `--loadfile` to override it with a newly built image. JTAG mode always loads
the standard RAM image even when the UART-oriented chip config has
`load_function = 2`:

```text
BLFlashCommand.py --interface jlink --chipname bl618dg --port <J-Link SN> \
  --flash --write --file <firmware.bin>
```

For the Nuclei FTDI adapter, a non-destructive connectivity check is:

```text
BLFlashCommand.py --interface openocd --chipname bl618dg --baudrate 1000 \
  --flash --read --start 0x0 --len 0x10 --file bl618dg_bootheader.bin
```

Pass `--port <adapter-serial>` only when selecting one adapter from multiple
Nuclei FTDI probes.

The FlashCube OpenOCD path defaults to four-wire JTAG. When only the two-wire
NScan1 pins are connected, select cjtag with:

```text
BFLB_OPENOCD_SCAN_MODE=nscan1 BLFlashCommand.py \
  --interface openocd --chipname bl618dg --baudrate 15000 ...
```

For direct OpenOCD use, set the interface variable before sourcing the
interface file:

```text
openocd -c "set NUCLEI_NSCAN1 1" \
  -f interface/nuclei_ftdi.cfg -f target/bl618dg.cfg
```

Four-wire JTAG drives the adapter's `JTAG_SEL` signal high; cjtag drives it low.
Nuclei OpenOCD sets it from the selected mode. See `JTAG_BANDWIDTH.md` for the
protocol analysis and measured NScan1/four-wire performance.

The FlashCube JTAG loader obtains load addresses and PC from the image rather
than a fixed byte slice or the first two payload words. J-Link selects the
BL618DG AP TAP with `JTAGConf 5,1`. OpenOCD selects the AP by default; the SDK
target can select the NP for debugging with:

```text
openocd -f interface/nuclei_ftdi.cfg -c "set DEBUG_CPU np" \
  -f target/bl618dg.cfg
```

The scan-chain mapping verified on BL618DG B0 is `riscv0 = NP` and
`riscv1 = AP`. NP is normally held in reset after a chip reset; AP firmware
must release NP before OpenOCD can examine it. NP mode therefore attaches
without pulsing system reset. The FlashLoader must run on AP. CKLink remains
disabled until its AP TAP selector is verified.

## Validation status

The target ELF has been built and checked to have entry point `0xA1000400`;
the linked image ends below the reserved mailbox region. On BL618DG B0,
OpenOCD RAM loading, descriptor publication, handshake, JEDEC-ID read, and a
1 MiB range erase/write/XIP-SHA verification have been exercised with the
Nuclei FTDI adapter. eFuse/OTP operations have not been exercised.
