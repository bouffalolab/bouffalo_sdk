# boot2_isp


## Support CHIP

| CHIP              | Remark          |
|:-----------------:|:---------------:|
| BL602/BL604       |                 |
| BL702/BL704/BL706 |                 |
| BL702L/BL704L     |                 |
| BL616/BL618       |                 |
| BL618DG           | Only for ap CPU |

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
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```
