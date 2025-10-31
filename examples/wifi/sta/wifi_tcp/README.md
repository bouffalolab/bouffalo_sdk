# WiFi tcp case


## Support CHIP

|      CHIP               | Remark |
|:-----------------------:|:------:|
|BL602/BL616/BL618/BL616d |        |

## Compile

- BL616/BL618/BL602

```bash
make CHIP=<chipname> BOARD=<boardname>
```

eg:
```bash
make CHIP=bl616 BOARD=bl616dk
```

- BL616d

```bash
make CHIP=bl616d BOARD=bl616ddk CPU_ID=ap CONFIG_ROMAPI=n
```

## Flash

- BL60x/BL61x

```bash
make flash CHIP=<chipname> COMX=xxx ## xxx is your com name, chipname is bl602/bl616...
```

## How use wifi tcp test


example
```             
              ))    (                            ____________
    \|/      )))    ((   \|/  \|/               |            |
     |______           ___|____|___             |  Host pc   |
    |       |         |            |            |____________|
    | BLDev |         |   Router   |#<-------->#/            /#
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

On BL Chips, using <wifi_sta_connect> command connect your WiFi router

```bash
bouffalolab />wifi_sta_connect BL_TEST 12345678
bouffalolab />wifi_tcp_test 192.168.1.2 3365
bouffalolab />tcp client task start ...
Server ip Address : 192.168.1.2:3365
TCP client connect server success!
Press CTRL-C to exit.

```
if connect success, Linux Host pc have receive data the BL Chips send.

### tcp server echo test

On BL Chips start tcp server

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

