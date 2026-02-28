# WiFi macsw test


## Support CHIP

| CHIP        | Remark |
|:-----------:|:------:|
| BL616/BL618 |        |

## Compile

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

## Flash

```bash
make flash COMX=xxx ## xxx is your com name
```

### Test commands

```bash
macsw_scan
```

