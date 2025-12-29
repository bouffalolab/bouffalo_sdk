# i2s_es8388


## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |
|BL616L/BL618L     |        |

## Compile

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

- BL616L/BL618L

```
make CHIP=bl616l BOARD=bl616ldk
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```