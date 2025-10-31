# pwm_foc_hall


## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |
|BL808             |        |
|BL616D            |        |
|BL616L/BL618L     |        |

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

- BL616L/BL618L

```
make CHIP=bl616l BOARD=bl616ldk
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```