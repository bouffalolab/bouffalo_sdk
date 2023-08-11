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

## How use wifi https test

### https test get a page

On BL616 board, using <wifi_sta_connect> command connect your WiFi router

```bash
bouffalolab />wifi_sta_connect BL_TEST 12345678
bouffalolab />wifi_https_test
bouffalolab />dst_addr is F23265B4
[I:  52207293][bl_https.c:126]    HTTP/S> bl connect fd = 0xa800ee60
[I:  52221230][bl_https.c:136]    HTTP/S> ret = -28
[I:  54080149][bl_https.c:156]    HTTP/S> ret = 0
[I:  54080658][bl_https.c:160]    HTTP/S> total time:1884447 us
[I:  54095499][bl_https.c:168]    HTTP/S> rcv_ret = 1179
[I:  54096043][bl_https.c:172]    HTTP/S> proc_head_r 0, status_code 200, body_start_off 400
[I:  54096479][bl_https.c:180]    HTTP/S> Copy to resp @off 0, len 779, 1st char 3C
blTcpSslDisconnect end
[I:  54104728][bl_https.c:264]    HTTP/S> total time:1908498 us
[I:  54105226][bl_https.c:265]    HTTP/S> test_1: status_code 200, resp_len 779
[I:  54105654][bl_https.c:267]    HTTP/S> resp body: <!DOCTYPE html>
......

```

### http post 

TODO


