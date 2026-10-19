# ir_tx_dma


## Support CHIP

| CHIP          | Remark          |
|:-------------:|:---------------:|
| BL702L/BL704L | Only support tx |
| BL618DG       |                 |

## Compile

- BL702L/BL704L

```
make CHIP=bl702l BOARD=bl702ldk
```

- BL618DG

```
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=np
```
## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```
