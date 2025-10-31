# README

# Uart WiFi Configuration and Usage

## How to Connect an HOST Board and a BFLB Board Using Jumper Wires

The hardware wiring diagram between the HOST and BFLB is shown in the figure.

### Jumper Wires

If you are using the BFLB development board, please connect the jumpers as follows：

| BFLB        | HOST      |
|-------------|-----------|
| GPIO14(TX)  | UART_RX   |
| GPIO15(RX)  | UART_TX   |
| GND         | GND       |

Baudrate: 2000000

### Compile

Compile uartwifi porject:

```bash
make CONFIG_USE_LPAPP=0 CONFIG_ANTENNA_CONTROL=0
```

### Flashing

To flash your project onto the target device, use the following command, where `xxx` is your serial port name:

```bash
make flash COMX=xxx # xxx is your com port name
```
