# Peripheral

This sample description ble peripheral function with READ, WRITE/WRITE NO RESPONSE, INDICATE and NOTIFY properties. Can be linked and communicated by ble app.

## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |

## Compile

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```