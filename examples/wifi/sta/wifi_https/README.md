# wifi6


## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL602             |        |
|BL616/BL618       |        |

## Compile

- BL602

```
make CHIP=bl602 BOARD=bl602dk
```

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

## Flash

```
make flash COMX=xxx ## xxx is your com name
```

## How use wifi https test

### https test get a page

On BL616 board, using <wifi_sta_connect> command connect your WiFi router

```bash
bouffalolab />wifi_sta_connect bouffalolab 12345678
bouffalolab />wifi_https_test https://www.gov.cn

...

Http client GET request server success

...

Http client POST request server success
```

