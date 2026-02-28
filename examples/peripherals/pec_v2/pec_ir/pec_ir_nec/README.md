# pec_ir_nec


## Support CHIP

| CHIP    | Remark |
|:-------:|:------:|
| BL618DG |        |
| BL616CL |        |

## Compile

- BL618DG

```
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

- BL616CL

```
make CHIP=bl616cl BOARD=bl616cldk
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```
