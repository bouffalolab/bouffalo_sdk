# Websocket case


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

## How use websocket tls test


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


On Linux Host pc run netcat command, listen localhost 3365 port

```bash
websocat --pkcs12-der certificate.pfx --pkcs12-passwd "" -s 0.0.0.0:8888
```

On BL Chips, using <wifi_sta_connect> command connect your WiFi router

```bash
bouffalolab />websocket_test 192.168.28.141 8888 /
Set host: 192.168.28.141 port: 8888 path: /
websocket connecting to ws://192.168.28.141:8888/ ...
TLS Context Ininting ...
WS:start to connect to host
socket created: 0
WS: rws connect ok
WS:connect to host ok
Start Handshake
HandShake OK
TLS version: TLSv1.2
Cipher: TLS-ECDHE-RSA-WITH-AES-128-GCM-SHA256
WS:
send buffer:GET / HTTP/1.1
Host: 192.168.28.141:8888
Upgrade: websocket
Connection: Upgrade
Origin: wss://192.168.28.141
Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==
Sec-WebSocket-Protocol: chat, superchat
Sec-WebSocket-Version: 13
websocket connect successfully
bouffalolab />WS:rws_socket_process_received_frame: frame->opcode:10WS:rws_socket_process_received_frame: rws_opcode_pong

```
if connect success, Linux Host pc have receive ping
