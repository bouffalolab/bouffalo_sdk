# WiFi Dualcore OTA Example

## Support CHIP

| CHIP    | Remark |
|:-------:|:------:|
| BL618DG |        |

## Compile

- BL618DG

```bash
make
```

## Flash

- BL618DG

```bash
make CHIP=bl618dg flash COMX=xxx ## xxx is your com name
```

## Generate OTA File

After building, run the following command from this directory:

```bash
SDK_BASE=$(cd ../../../.. && pwd) && \
$SDK_BASE/tools/bflb_tools/bflb_fw_post_proc/bflb_fw_post_proc-ubuntu \
  --chipname=bl618dg \
  --imgfile=build/wifi_bl618dg.bin \
  --appkeys=shared \
  --brdcfgdir=$SDK_BASE/bsp/board/bl616dk/config
```

The OTA file will be generated at: `build/wifi_bl618dg.bin.ota`

## Output Files

| File | Description |
|------|-------------|
| `build/wifi_bl618dg.bin` | Combined firmware binary (AP + NP) |
| `build/wifi_bl618dg.bin.ota` | OTA firmware file with header |
| `build/wifi_bl618dg.xz` | Compressed firmware |
| `build/wifi_bl618dg.xz.ota` | OTA compressed firmware |

## OTA File Format

The OTA file contains:
- 512-byte OTA header (version info, SHA256, etc.)
- Firmware binary data
