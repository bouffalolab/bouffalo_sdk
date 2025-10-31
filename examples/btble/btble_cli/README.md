# btble_cli

## Support CHIP

|      CHIP               | Remark |
|:-----------------------:|:------:|
|BL602/BL616/BL618/BL616D/BL618D|        |

## Compile

- BL602 (BLE only)

```
make CHIP=bl602 BOARD=bl602dk
```
- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```
- BL616D/BL618D

```
make CHIP=bl616d BOARD=bl616ddk CPU_ID=ap
```
## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```
## BTBLE CLI CMD guide

please refer to `docs/en/api_reference/ble.rst`