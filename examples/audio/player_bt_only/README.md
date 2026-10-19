# i2s_codec


## Support CHIP

| CHIP        | Remark |
|:-----------:|:------:|
| BL616/BL618 |        |
| BL618DG     |        |
| BL616L      |        |

## Compile

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

- BL618DG

```
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

- BL616L

```
make CHIP=bl616l BOARD=bl616ldk
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```
