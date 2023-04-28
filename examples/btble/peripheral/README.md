# Peripheral

This sample description ble peripheral function with READ, WRITE/WRITE NO RESPONSE, INDICATE and NOTIFY properties. Can be linked and communicated by ble app.

## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |
|BL808             |  Only for M0 CPU      |

## Compile

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

- BL808

```
make CHIP=bl808 BOARD=bl808dk CPU_ID=m0
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```