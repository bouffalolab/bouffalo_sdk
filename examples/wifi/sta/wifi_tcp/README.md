# wifi6 tcp case


## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |

## Compile

- BL616/BL618

```bash
make CHIP=bl616 BOARD=bl616dk
```

## Flash

```bash
make flash COMX=xxx ## xxx is your com name
```

## How use wifi tcp test


example
```             
              ))    (                            ____________
    \|/      )))    ((   \|/  \|/               |            |
     |______           ___|____|___             |  Host pc   |
    |       |         |            |            |____________|
    | BL616 |         |   Router   |#<-------->#/            /#
    |_______|         |____________|           /____________/#   
   192.168.1.3         192.168.1.1              192.168.1.2    
                      SSID: BL_TEST
                    Password:12345678
```

### tcp client test

On Linux Host pc run netcat command, listen localhost 3365 port

```bash
$ nc -lp 3365
```

On BL616 board, using <wifi_sta_connect> command connect your WiFi router

```bash
bouffalolab />wifi_sta_connect BL_TEST 12345678
bouffalolab />wifi_tcp_test 192.168.1.2 3365
bouffalolab />tcp client task start ...
Server ip Address : 192.168.1.2:3365
TCP client connect server success!
Press CTRL-C to exit.

```
if connect success, Linux Host pc have receive data the BL616 send.

### tcp server echo test

On BL616 Board start tcp server

```bash
bouffalolab />wifi_sta_connect BL_TEST 12345678
bouffalolab />wifi_tcp_echo_test 3365
bouffalolab />tcp server task start ...
Server ip Address : 0.0.0.0:3365
# host pc connected log
new client connected from (192.168.1.2, 57480)
Press CTRL-C to exit.
recv 7 len data

```

On Linux Host pc using <nc> command connect server:

```bash
$ nc -v 192.168.1.3 3365
Connection to 192.168.1.3 3365 port [tcp/*] succeeded!
123456  # enter something
123456  # echo received data

```

