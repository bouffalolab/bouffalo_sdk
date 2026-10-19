# sec_eng_gmac_link


## Support CHIP

| CHIP          | Remark |
|:-------------:|:------:|
| BL602/BL604   |        |
| BL702L/BL704L |        |
| BL616/BL618   |        |
| BL616CL       |        |
| BL618DG       |        |

## Compile

- BL602/BL604

```
make CHIP=bl602 BOARD=bl602dk
```

- BL702L/BL704L

```
make CHIP=bl702l BOARD=bl702ldk
```

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

- BL616CL

```
make CHIP=bl616cl BOARD=bl616cldk
```

- BL618DG

```
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```
## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```
