# canfd_filter


## Support CHIP

| CHIP              | Remark |
|:-----------------:|:------:|
| BL618DG           |        |

## Compile

- BL618DG

```
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CPU_MODEL=b0
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=np CPU_MODEL=b0
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```
