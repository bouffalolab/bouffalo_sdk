# pec_dpi_basic


## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616D/BL618D     |        |
|BL616L/BL618L     |        |

## Compile

- BL616D/BL618D

```
make CHIP=bl616d BOARD=bl616ddk CPU_ID=ap
```

- BL616L/BL618L

```
make CHIP=bl616l BOARD=bl616ldk
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```