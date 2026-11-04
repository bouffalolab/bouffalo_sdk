# SPI-NAND + FatFs

Uses the Bouffalo SPI-NAND driver and Daniel Beer Dhara FTL from `components/spi_nand`.
The example writes `/flash/spi_nand_test.bin`, overwrites an unaligned region,
syncs/unmounts, reinitializes the NAND, then checks the stored file.

The driver detects the model from its three-byte JEDEC ID:

| Model | JEDEC ID | Main capacity | Main + spare bytes/page | Pages/block |
| --- | --- | --- | --- | --- |
| Winbond W25N01KVZEIR | EF AE 21 | 128 MiB | 2048 + 96 | 64 |
| FORESEE F35SQA512M | CD 70 70 | 64 MiB | 2048 + 64 | 64 |

Device profiles select geometry, writable feature bits, ECC error decoding and
factory bad-block marker locations. F35SQA512M checks the first spare byte on
both the first and second pages of each block; ECC status 10 and 11 both report
uncorrectable errors. Its 64-byte spare area is host accessible, with ECC parity
stored separately. Support follows the F35SQA512M datasheet Rev. 1.2.
The example uses detected device information and the same FatFs flow for both
models. Logical capacity is smaller than main capacity because Dhara reserves
space for garbage collection, wear leveling and bad blocks.

BL618DGDK pinmux uses CS=16, CLK=17, MISO=18, MOSI=19. BL616CLDK uses
CS=12, CLK=13, MISO=18, MOSI=19. Configure WP/IO2 and HOLD/IO3 high externally
for SIO operation.

```sh
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap BUILD_DIR=build_bl618dg
make CHIP=bl616cl BOARD=bl616cldk BUILD_DIR=build_bl616cl
```

The default build does not format. For first use on a blank NAND, explicitly
set `CONFIG_SPI_NAND_EXAMPLE_FORMAT=y` when building. This permits `f_mkfs`
when mounting reports no filesystem, and may overwrite existing contents.
Turn it off for later runs. No automatic whole-chip erase is performed.

For a FreeRTOS build, also pass `CONFIG_FREERTOS=y` and use a separate build
folder. NAND initialization runs in a task after scheduler start.

The ID output depends on the detected device, for example `NAND ID ef ae21`
for W25N01KV. Successful verification prints
`NAND FatFs rewrite + reinitialize: PASS (0)`. This verifies reinitialization;
physical power-cycle testing and actual-device measurements remain necessary.
