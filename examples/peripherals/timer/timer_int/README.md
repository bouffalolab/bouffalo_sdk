# timer_int


## Support CHIP

| CHIP              | Remark |
|:-----------------:|:------:|
| BL602/BL604       |        |
| BL702/BL704/BL706 |        |
| BL702L/BL704L     |        |
| BL616/BL618       |        |
| BL618DG           |        |
| BL616CL           |        |

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

- BL618DG

```
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=np
```

- BL616CL

```
make CHIP=bl616cl BOARD=bl616cldk
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```
