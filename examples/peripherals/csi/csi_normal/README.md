# csi_normal


## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL808             |        |

## Compile

- BL808

```
make CHIP=bl808 BOARD=bl808dk CPU_ID=m0
make CHIP=bl808 BOARD=bl808dk CPU_ID=d0
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```