# wo_uart


## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |
|BL808             |        |
|BL616D            |        |

## Compile

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

- BL808

```
make CHIP=bl808 BOARD=bl808dk CPU_ID=m0
make CHIP=bl808 BOARD=bl808dk CPU_ID=d0
```

- BL616D

```
make CHIP=bl616d BOARD=bl616ddk CPU_ID=ap
make CHIP=bl616d BOARD=bl616ddk CPU_ID=np
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```