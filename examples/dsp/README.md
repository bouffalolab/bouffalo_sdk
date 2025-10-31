## CSI DSP Test

## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |
|BL808             | m0     |

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