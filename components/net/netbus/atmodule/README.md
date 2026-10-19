# AT Module

Bouffalo Lab's AT module provides an AT command interface for adding wireless
connectivity to a host device. This repository contains the AT component and
UART, SPI, and USB Wi-Fi examples.

## Introduction

AT commands cover Wi-Fi, TCP/IP, TLS, HTTP, MQTT, Bluetooth LE, OTA, and
low-power functions. Available commands depend on the chip and firmware
configuration.

## Supported Chips and Interfaces

The following example combinations are verified by CI builds:

| Chip | UART | SPI | USB |
| --- | --- | --- | --- |
| BL602 | Verified | — | — |
| BL616 | Verified | Verified | Verified |
| BL616CL | Verified | — | Verified |

A dash indicates a combination without CI build coverage.

## Get Started

Install the build tools and RISC-V toolchain required by Bouffalo SDK, then
build the default BL616 UART example from this repository's root:

```bash
make CHIP=bl616 BOARD=bl616dk
```

The build detects an existing SDK or downloads it automatically. See the
example guides below for wiring, configuration, and flashing.

## Documentation

- [English AT Command Reference](docs/en/samples/atmoudle/atmoudle.rst)
- [中文 AT 命令参考](docs/zh/samples/atmoudle/atmoudle.rst)

To build the documentation locally:

```bash
python3 -m pip install -r docs/requirements.txt
./docs/build_all.sh
```

## Resources

- [UART Wi-Fi Example](examples/uart_wifi/README.md)
- [SPI Wi-Fi Example](examples/spi_wifi/README.md)
- [USB Wi-Fi Example](examples/usb_wifi/README.md)
- [Bouffalo SDK](https://github.com/bouffalolab/bouffalo_sdk)
