# psram


## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL702/BL704/BL706 |        |
|BL616/BL618       |        |
|BL808             |        |

## Compile

- BL702/BL704/BL706

```
make CHIP=bl702 BOARD=bl702dk
```

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

- BL808

```
make CHIP=bl808 BOARD=bl808dk CPU_ID=m0
make CHIP=bl808 BOARD=bl808dk CPU_ID=d0
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```