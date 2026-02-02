# WiFi MQTT Publisher Example

## Overview

This example demonstrates how to use Bouffalo SDK to publish messages to an MQTT broker over WiFi. It showcases:

- WiFi Station mode connection
- MQTT client implementation using MQTT-C library
- TLS/SSL support for secure MQTT connections (MQTTS)
- FreeRTOS task management
- Shell command interface

## Features

- **WiFi Station Mode**: Connect to WiFi networks with WPA/WPA2 authentication
- **MQTT Publishing**: Publish messages to MQTT topics with configurable QoS
- **TLS Support**: Optional TLS/SSL connection using mbedTLS for secure communication
- **Shell Commands**: Interactive command-line interface for MQTT operations
- **Configurable Parameters**: Server address, port, and topic can be specified

## Support CHIP

| CHIP        | Remark |
|:-----------:|:------:|
| BL602       |        |
| BL616/BL618 |        |

## Hardware Requirements

- BL602/BL616/BL618 development board
- WiFi network with internet access
- MQTT broker (public or private)

## Software Components

- **MQTT-C**: Lightweight MQTT client library
- **mbedTLS**: TLS/SSL implementation for secure connections
- **FreeRTOS**: Real-time operating system
- **LwIP**: TCP/IP stack
- **Shell**: Command-line interface

## Compile

### BL602

```bash
make CHIP=bl602 BOARD=bl602dk
```

### BL616/BL618

```bash
make CHIP=bl616 BOARD=bl616dk
```

## Flash

```bash
make flash COMX=xxx  # xxx is your com port name
```

## Configuration

### TLS/SSL Configuration

To enable TLS support, uncomment the following line in `CMakeLists.txt`:

```cmake
sdk_add_compile_options(-DMQTT_USE_MBEDTLS)
```

The example includes a sample CA certificate embedded in `wifi_mqtt_pub.c`. Replace it with your broker's CA certificate for production use.

## How to Use

### 1. Set Up MQTT Broker

First, find an MQTT broker or deploy your own. For testing, you can use:
- **Public test broker**: `test.mosquitto.org:1883` (non-SSL)
- **EMQX public broker**: `broker.emqx.io:1883`
- **Your own broker**: Use your deployed MQTT server

### 2. Verify Broker Connection

Use an MQTT client to test if the server is working:
- **Windows**: Install `MQTTBox` from Microsoft Store
- **Web**: Use EMQX's online MQTT client
- **Linux**: Use `mosquitto_pub` and `mosquitto_sub` command-line tools

### 3. Connect to WiFi

Use the shell command to connect to your WiFi network:

```bash
bouffalolab />wifi_sta_connect <SSID> <PASSWORD>
```

Example:
```bash
bouffalolab />wifi_sta_connect MyNetwork MyPassword
```

Wait for the IP assignment confirmation before proceeding.

### 4. Publish to MQTT Broker

#### Basic Usage (Default Settings)

```bash
bouffalolab />mqtt_pub
```

This connects to `test.mosquitto.org:1883` and publishes to topic `bl618`.

#### Custom Server, Port, and Topic

```bash
bouffalolab />mqtt_pub <server> <port> <topic>
```

Examples:
```bash
# Connect to custom broker
bouffalolab />mqtt_pub broker.emqx.io 1883 my/topic

# Connect to IP address
bouffalolab />mqtt_pub 192.168.1.100 1883 sensors/data
```

### 5. Expected Output

Once connected, the device will publish a message every 3 seconds:

```
mqtt_pub is ready to begin publishing the time.
Press CTRL-C to exit.
mqtt_pub published : "{\"hello mqtt by bl616/8 !\"}
Now time is now"
mqtt_pub published : "{\"hello mqtt by bl616/8 !\"}
Now time is now"
...
```

Press `CTRL-C` to stop publishing and disconnect.

## Shell Commands

| Command | Description |
|---------|-------------|
| `wifi_sta_connect <SSID> <PASSWORD>` | Connect to WiFi network |
| `mqtt_pub` | Publish to default broker (test.mosquitto.org:1883) |
| `mqtt_pub <server> <port> <topic>` | Publish to custom broker and topic |

## Code Structure

- **main.c**: Application entry point, WiFi initialization, event handling
- **wifi_mqtt_pub.c**: MQTT client implementation and shell commands
- **CMakeLists.txt**: Build configuration
- **Kconfig**: SDK configuration

## Key Functions

| Function | Description |
|----------|-------------|
| `cmd_mqtt_publisher()` | Shell command handler for MQTT publishing |
| `example_mqtt()` | Main MQTT publishing task |
| `initserver_tls()` | Initialize TLS connection for MQTTS |
| `client_refresher()` | Background task for handling MQTT traffic |
| `publish_callback_1()` | Callback for received publish messages |

## MQTT Message Format

Published messages have the following format:

```json
{"hello mqtt by bl616/8 !"}
Now time is now
```

## Troubleshooting

### WiFi Connection Fails
- Verify SSID and password are correct
- Check WiFi signal strength
- Ensure the router supports 2.4GHz (BL602/BL616 only support 2.4GHz)

### MQTT Connection Fails
- Verify the broker address and port are correct
- Check if the broker requires authentication
- For TLS connections, verify the CA certificate is valid
- Test the broker with a known-working MQTT client first

### TLS Handshake Fails
- Ensure `MQTT_USE_MBEDTLS` is defined in CMakeLists.txt
- Verify the CA certificate matches your broker
- Check mbedTLS debug output for detailed error information

## Resources

- **MQTT-C Library**: https://github.com/LiamBindle/MQTT-C
- **mbedTLS Documentation**: https://tls.mbed.org/
- **MQTT Protocol**: https://mqtt.org/
- **Public MQTT Brokers**:
  - https://test.mosquitto.org/
  - https://www.emqx.com/en/mqtt/public-mqtt5-broker

## License

Licensed to the Apache Software Foundation (ASF) under one or more contributor license agreements. See the NOTICE file distributed with this work for additional information regarding copyright ownership.
