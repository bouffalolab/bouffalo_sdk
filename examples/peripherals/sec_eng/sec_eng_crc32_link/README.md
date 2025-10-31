# sec_eng_crc32_link


## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |
|BL616L            |        |
|BL616D            |        |
|BL808             |        |

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

- BL808

```
make CHIP=bl808 BOARD=bl808dk CPU_ID=m0
make CHIP=bl808 BOARD=bl808dk CPU_ID=d0
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```
