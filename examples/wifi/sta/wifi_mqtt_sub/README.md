# WiFi MQTT Subscriber Example

This example demonstrates how to subscribe to MQTT topics over a WiFi connection using Bouffalo SDK. It supports both plain TCP and TLS (MQTTS) connections.

## Features

- MQTT v3.1.1 protocol support
- WiFi station mode connectivity
- TLS/MQTTS support using mbedTLS (optional)
- Shell command interface for easy testing
- Non-blocking socket I/O with FreeRTOS
- Configurable MQTT broker, port, and topic

## Supported Chips

| CHIP    | Board      | Remark |
|:-------:|:----------:|:------:|
| BL602   | bl602dk    |        |
| BL616   | bl616dk    |        |
| BL618   | bl618dk    |        |

## Build

### BL602
```bash
make CHIP=bl602 BOARD=bl602dk
```

### BL616/BL618
```bash
make CHIP=bl616 BOARD=bl616dk
```

### Using Ninja
```bash
make ninja CHIP=bl616 BOARD=bl616dk
```

## Flash

```bash
make flash CHIP=bl616 COMX=/dev/ttyUSB0
```

## Configuration

### TLS/SSL Support
To enable MQTT over TLS (MQTTS), uncomment the following line in `CMakeLists.txt`:
```cmake
sdk_add_compile_options(-DMQTT_USE_MBEDTLS)
```

### Default CA Certificate
For TLS connections, update the `test_ca_crt` in `wifi_mqtt_sub.c` with your broker's CA certificate.

## Usage

### Prerequisites

1. Find or deploy an MQTT broker server
2. Test the server with an MQTT client (e.g., MQTTBox, EMQX online client)

### Shell Commands

```bash
# Connect to WiFi first
wifi_sta_connect <SSID> <PASSWORD>

# Subscribe with default settings (test.mosquitto.org:1883, topic: bl618)
mqtt_sub

# Subscribe with custom broker
mqtt_sub <broker_address> <port> <topic>
```

### Example Log Output

#### Using test.mosquitto.org (default)
```bash
bouffalolab />wifi_sta_connect MyNetwork MyPassword
bouffalolab />mqtt_sub
bouffalolab />Addr:test.mosquitto.org  Port:1883  Topic:bl618
mqtt_sub listening for 'bl618' messages.
Press CTRL-C to exit.
Received publish('bl618'): {"hello mqtt"}
Received publish('bl618'): {"hello mqtt"}
mqtt_sub disconnecting from test.mosquitto.org
^C
```

#### Using broker.emqx.io
```bash
bouffalolab />wifi_sta_connect MyNetwork MyPassword
bouffalolab />mqtt_sub broker.emqx.io 1883 mytopic
bouffalolab />Addr:broker.emqx.io  Port:1883  Topic:mytopic
mqtt_sub listening for 'mytopic' messages.
Press CTRL-C to exit.
Received publish('mytopic'): {"hello mqtt"}
mqtt_sub disconnecting from broker.emqx.io
^C
```

## Code Structure

| File            | Description                                  |
|:----------------|:---------------------------------------------|
| `main.c`        | Application entry point, WiFi initialization |
| `wifi_mqtt_sub.c` | MQTT client implementation, shell commands   |
| `FreeRTOSConfig.h` | FreeRTOS configuration                      |
| `lwipopts_user.h` | lwIP network stack configuration            |
| `mbedtls_sample_config.h` | mbedTLS configuration              |

## Dependencies

- **FreeRTOS**: Real-time operating system
- **lwIP**: TCP/IP stack
- **mbedTLS**: TLS/SSL library (optional, for MQTTS)
- **MQTT-C**: MQTT client library

## Notes

- Ensure WiFi is connected before running `mqtt_sub`
- Press CTRL-C to exit the subscription
- The client uses clean session mode
- Default QoS level is 0 (at most once)
