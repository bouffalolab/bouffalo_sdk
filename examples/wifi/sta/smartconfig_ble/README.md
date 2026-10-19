# wifi6


## Support CHIP

| CHIP        | Remark |
|:-----------:|:------:|
| BL616/BL618 |        |

## Compile

- BL616/BL618/BL618DG

```
make CHIP=bl616 BOARD=bl616dk
```

```
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

## Flash

```
make flash COMX=xxx ## xxx is your com name
```

