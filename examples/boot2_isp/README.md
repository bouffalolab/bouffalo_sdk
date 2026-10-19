# boot2_isp

## Support CHIP

|       CHIP       |     Remark     |
| :---------------: | :-------------: |
|    BL602/BL604    |                |
| BL702/BL704/BL706 |                |
|   BL702L/BL704L   |                |
|    BL616/BL618    |                |
|      BL618DG      | Only for ap CPU |

## Compile

- BL602/BL604

```
make CHIP=bl602 BOARD=bl602dk
```

- BL702/BL704/BL706

```
make CHIP=bl702 BOARD=bl702dk
```

- BL702L/BL704L

```
make CHIP=bl702l BOARD=bl702ldk
```

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

- BL618DG

```
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

`CONFIG_APP_ENCRYPT_AFTER_SIGN` enables the `ONL1` image format used by
`APP AES Key per device`. Boot2 configures the APP eFuse AES
region before hashing, verifies the decrypted plaintext, and leaves images
without the `ONL1` marker on the existing path.

The build output must still be packaged with Boot credentials matching the
target's Secure Boot eFuse policy before it is programmed at address zero.

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```
