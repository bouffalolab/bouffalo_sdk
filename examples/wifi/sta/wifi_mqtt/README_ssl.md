## Prepare to check
1. Append content to your /etc/hosts, this server.local is COMMON NAME of certificate.
```
127.0.0.1   server.local
```
2. Start Borker backend
```
mosquitto -c ssl/mos.conf
```
3. Subscribe test title
```
mosquitto_sub -h server.local -p 8883 --cafile ssl/ca_1.crt -t test
```
4. Publish hello message for test title
```
mosquitto_pub -h server.local -p 8883 --cafile ssl/ca_1.crt -t test -m hello
```
5. connect is ok, can receive hello from subscribe.

## DEMO as subscribe
1. stop previous mosquitto task, mosquitto_sub task and mosquitto_pub task.
2. Start Borker backend
```
mosquitto -c ssl/mos.conf
```
3. Connect your ssid, and subscribe listen.
```
bouffalolab /> wifi_sta_connect your_ssid 12345678
bouffalolab /> mqtts_sub 192.168.1.143 8883
tcp client connect 192.168.1.143:8883
tcp_client_connect fd:1
[MBEDTLS] Performing the SSL/TLS handshake ... 
[MBEDTLS] ...... Verifying peer X.509 certificate ... 
[MBEDTLS] ssl connect ok
mqtts_sub listening for 'mqtt_test' messages.
```
4. Publish hello message for test title, and receive
```
mosquitto_pub -h server.local -p 8883 --cafile ssl/ca_1.crt -t mqtt_test -m hello

Received publish('mqtt_test'): hello
Received publish('mqtt_test'): hello
```

## DEMO as publish
1. stop previous mosquitto task, mosquitto_sub task and mosquitto_pub task.
2. Start Borker backend, and subscribe
```
mosquitto -c ssl/mos.conf
mosquitto_sub -h server.local -p 8883 --cafile ssl/ca_1.crt -t mqtt_test 
```
3. Connect your ssid, and loop publish
```
bouffalolab /> wifi_sta_connect your_ssid 12345678
bouffalolab />mqtts_pub 192.168.1.143 8883
tcp client connect 192.168.1.143:8883
tcp_client_connect fd:1
[MBEDTLS] Performing the SSL/TLS handshake ... 
[MBEDTLS] ...... Verifying peer X.509 certificate ... 
[MBEDTLS] ssl connect ok
mqtts_pub is ready to begin publishing the time.
mqtts_pub published : "{"hello mqtt !"}
mqtts_pub published : "{"hello mqtt !"}
mqtts_pub published : "{"hello mqtt !"}
```
4. Get message from your subcribe
```
{"hello mqtt !"}
{"hello mqtt !"}
{"hello mqtt !"}
{"hello mqtt !"}
{"hello mqtt !"}
{"hello mqtt !"}
{"hello mqtt !"}
```
