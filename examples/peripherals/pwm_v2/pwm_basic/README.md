# pwm_basic


## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL702L/BL704L     |        |
|BL616/BL618       |        |
|BL808             |        |

## Compile

- BL702L/BL704L

```
make CHIP=bl702l BOARD=bl702ldk
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