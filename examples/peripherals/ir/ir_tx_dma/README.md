# ir_tx_dma


## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL702L/BL704L     | Only support tx       |
|BL808             |        |

## Compile

- BL702L/BL704L

```
make CHIP=bl702l BOARD=bl702ldk
```

- BL808

```
make CHIP=bl808 BOARD=bl808dk CPU_ID=m0
make CHIP=bl808 BOARD=bl808dk CPU_ID=d0
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```