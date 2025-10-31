# fatfs


## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |
|BL808             |  Only for M0 CPU      |
|BL628             |        |

## Compile

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

- BL808

```
make CHIP=bl808 BOARD=bl808dk CPU_ID=m0
```

- BL628

```
make CHIP=bl628 BOARD=bl628dk
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```