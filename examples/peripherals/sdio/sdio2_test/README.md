# sdio2_test


## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL602/BL604       |        |
|BL616/BL618       |        |

## Compile

- BL602/BL604

```
make CHIP=bl602 BOARD=bl602dk
```

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```