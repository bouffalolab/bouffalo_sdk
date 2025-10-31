# ir_swm_int


## Support CHIP

|      CHIP        | Remark                |
|:----------------:|:---------------------:|
|BL616             | Only support rx       |
|BL616L            | Only support rx       |
|BL616D            |                       |

## Compile

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

- BL616L

```
make CHIP=bl616l BOARD=bl616ldk
```

- BL616D

```
make CHIP=bl616d BOARD=bl616ddk CPU_ID=ap
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```
