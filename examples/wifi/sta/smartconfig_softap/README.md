# SoftAP WiFi Configuration Demo

BL602 creates a WiFi AP on boot. Connect to it and configure WiFi via web page.

## Features

- Auto-starts AP on boot (SSID: `BL602_Config`, no password)
- Web interface at `http://192.168.169.1`
- Scan and connect to WiFi networks

## Build

```bash
cd examples/wifi/sta/smartconfig_softap
make CHIP=bl602 BOARD=bl602dk
```

## Flash

```bash
make flash CHIP=bl602 COMX=/dev/ttyUSB0
```

## Usage

1. Power on the device
2. Connect phone/PC to `BL602_Config` WiFi
3. Open browser: `http://192.168.169.1`
4. Click "Scan" to find networks
5. Select network and enter password
6. Click "Connect"

## API

- `/wifiscan` - Scan WiFi networks
- `/wificonfig?ssid=xxx&password=xxx` - Start connection
- `/wifiget` - Get connection status
