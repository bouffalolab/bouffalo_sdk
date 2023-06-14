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

## How use wifi mqtt sub test

1. Find some mqtt server or your deployed a mqtt server first. This case default used `test.mosquitto.org:1883` server test; or used EMQX mqtt public server test.
2. Use mqtt client to test if the server is working. You can install `MQTTBox` on windows for testing, you can install it from Microsoft Store. Or use EMQX's online mqtt client for testing.
3. If the server works fine, then you can continue with the next test.
4. connect your WiFi.
5. connect MQTT server. command like `mqtt_sub` or `mqtt_sub <server domain name or server ip> <server port> <topic>`

Log:

```bash
bouffalolab />wifi_sta_connect BL_TEST 12345678
bouffalolab />mqtt_sub
bouffalolab />mqtt_sub listening for 'bl618' messages.
Press CTRL-C to exit.
Received publish('bl618'): {"hello mqtt"}
Received publish('bl618'): {"hello mqtt"}
Received publish('bl618'): {"hello mqtt"}
mqtt_sub disconnecting from test.mosquitto.org
^C
```

use broker.emqx.io mqtt public server test log:
```bash
bouffalolab />wifi_sta_connect BL_TEST 12345678
bouffalolab />mqtt_sub broker.emqx.io 1883 bl618
bouffalolab />mqtt_sub listening for 'bl618' messages.
Press CTRL-C to exit.
Received publish('bl618'):  {"hello mqtt"}
Received publish('bl618'):  {"hello mqtt"}
mqtt_sub disconnecting from broker.emqx.io
^C
```

