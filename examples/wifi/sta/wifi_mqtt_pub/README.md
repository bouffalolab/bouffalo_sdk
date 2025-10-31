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

## How use wifi mqtt pub test

1. Find some mqtt server or your deployed a mqtt server first. This case default used `test.mosquitto.org:1883` server test; or used EMQX mqtt public server test.
2. Use mqtt client to test if the server is working. You can install `MQTTBox` on windows for testing, you can install it from Microsoft Store. Or use EMQX's online mqtt client for testing.
3. If the server works fine, then you can continue with the next test.
4. connect your WiFi.
5. connect MQTT server. command like `mqtt_pub` or `mqtt_pub <server domain name or server ip> <server port> <topic>`.
6. If the command is executed successfully, a message will be uploaded every 3 seconds.

Log:

```bash
bouffalolab />wifi_sta_connect BL_TEST 12345678
bouffalolab />mqtt_pub
bouffalolab />mqtt_pub is ready to begin publishing the time.
Press ENTER to publish the current time.
Press CTRL-C to exit.
mqtt_pub published : "{"hello mqtt by bl616/8 !"}
Now time is 2023-06-16 00:00:00"
mqtt_pub published : "{"hello mqtt by bl616/8 !"}
Now time is 2023-06-16 00:00:03"
mqtt_pub stop publish to test.mosquitto.org
^C
```

