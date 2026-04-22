## How use wifi mqtt pub test

1. Find some mqtt server or your deployed a mqtt server first. This case default used `test.mosquitto.org:1883` server test; or used EMQX mqtt public server test.
2. Use mqtt client to test if the server is working. You can install `MQTTBox` on windows for testing, you can install it from Microsoft Store. Or use EMQX's online mqtt client for testing.
3. If the server works fine, then you can continue with the next test.
4. connect your WiFi.
5. connect MQTT server. command like `mqtt_pub` or `mqtt_pub <server ip> <server port>`.
6. If the command is executed successfully, a message will be uploaded every 3 seconds.

Log:

In linux, start broker and subscribe.
```bash
linux-bash  /> mosquitto -p 8883
linux-bash  /> mosquitto_sub -h 127.0.0.1 -p 8883 -t mqtt_test
```

```bash
bouffalolab />wifi_sta_connect Your_SSID 12345678
bouffalolab />mqtt_pub 192.168.1.143 8883
bouffalolab />mqtt_pub is ready to begin publishing the time.
Press ENTER to publish the current time.
Press CTRL-C to exit.
mqtt_pub published : "{"hello mqtt !"}
Now time is 2023-06-16 00:00:00"
mqtt_pub published : "{"hello mqtt !"}
Now time is 2023-06-16 00:00:03"
mqtt_pub stop publish to test.mosquitto.org
```
