# ir_swm_int


## Support CHIP

| CHIP    | Remark          |
|:-------:|:---------------:|
| BL616   | Only support rx |
| BL618DG |                 |

## Compile

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
