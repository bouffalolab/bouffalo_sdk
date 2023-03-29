# wifi6


## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |

## Compile

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

## Flash

```
make flash COMX=xxx ## xxx is your com name
```

## How use wifi http test

### http test get some page

On BL616 board, using <wifi_sta_connect> command connect your WiFi router

```bash
bouffalolab />wifi_sta_connect BL_TEST 12345678
bouffalolab />wifi_http_test www.gov.cn 
bouffalolab />Http client task start ...
Host:www.gov.cn, Server ip Address : 61.136.232.149:80
Http client connect server success!
Press CTRL-C to exit.
HTTP/1.1 200 OK
......

```

### http post 

TODO


