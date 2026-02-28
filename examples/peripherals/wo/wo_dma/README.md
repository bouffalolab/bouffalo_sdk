# wo_dma


## Support CHIP

| CHIP        | Remark |
|:-----------:|:------:|
| BL616/BL618 |        |
| BL618DG     |        |
| BL616CL     |        |

## Compile

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
