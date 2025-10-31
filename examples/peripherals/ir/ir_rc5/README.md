# ir_rc5


## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL602/BL604       |        |
|BL702/BL704/BL706 |        |
|BL702L/BL704L     | Only support tx       |
|BL616/BL618       | Only support rx       |
|BL808             |        |
|BL616D            |        |

## Compile

- BL602/BL604

```
make CHIP=bl602 BOARD=bl602dk
```

- BL702/BL704/BL706

```
make CHIP=bl702 BOARD=bl702dk
```

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
make CHIP=bl616d BOARD=bl616ddk CPU_ID=ap
make CHIP=bl616d BOARD=bl616ddk CPU_ID=np
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```