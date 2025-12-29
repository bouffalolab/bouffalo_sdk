# timer_gpio_clock


## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL702L/BL704L     |        |
|BL616/BL618       |        |
|BL808             |        |
|BL616D            |        |
|BL616L/BL618L     |        |

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

- BL616D

```
make CHIP=bl616d BOARD=bl616ddk
```

- BL616L/BL618L

```
make CHIP=bl616l BOARD=bl616ldk
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```