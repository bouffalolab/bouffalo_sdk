# adc_watchdog


## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616D/BL618D     |        |

## Compile

- BL616D/BL618D

```
make CHIP=bl616d BOARD=bl616ddk CPU_ID=ap
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```