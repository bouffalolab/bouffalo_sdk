# bl808_triplecore


## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL808             |  Only for LP CPU      |

## Compile

- BL808

```
make CHIP=bl808 BOARD=bl808dk CPU_ID=lp
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```

## UART of LP

It's defined in 'console_init' of bl808dk/board.c.
- GPIO 18 : TX
- GPIO 19 : RX
