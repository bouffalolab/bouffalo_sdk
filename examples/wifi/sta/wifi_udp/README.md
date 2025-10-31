# WiFi udp case


## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL602/BL616/BL618 |        |

## Compile

```bash
make CHIP=<chipname> BOARD=<boardname>
```

eg:
```
make CHIP=bl616 BOARD=bl616dk
```

## Flash

```
make flash CHIP=<chipname> COMX=xxx ## xxx is your com name, chipname is bl602/bl616...
```

## How use wifi udp test


```             
              ))                                 ____________
    \|/      )))         \|/  \|/               |            |
     |______           ___|____|___             |  Host pc   |
    |       |         |            |            |____________|
    | BLDev |         |   Router   |#<-------->#/            /#
    |_______|         |____________|           /____________/#   
   192.168.1.3         192.168.1.1              192.168.1.2    
                      SSID: BL_TEST
                    Password:12345678
```

### udp echo test

On BL Chips start udp server

```bash
bouffalolab />wifi_sta_connect BL_TEST 12345678
bouffalolab />wifi_udp_echo 3365
bouffalolab />udp server task start ...
Server ip Address : 0.0.0.0:3365
udp bind port success!
Press CTRL-C to exit.
recv from 192.168.1.2
recv:X
recv from 192.168.1.2
recv:X
recv from 192.168.1.2
recv:X
recv from 192.168.1.2
recv:X
recv from 192.168.1.2
recv:1234567890

```

On Linux Host pc using <nc> command connect server:

```bash
$ nc -uv 192.168.1.3 3365
Connection to 192.168.1.3 3365 port [udp/*] succeeded!
XXXX
1234567890  # enter something
1234567890  # echo received data

```

