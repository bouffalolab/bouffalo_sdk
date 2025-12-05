# Nethub

## Overview

Nethub is a dual-stack solution example with host support, supporting multiple interfaces (SDIO interface by default). This solution implements WiFi to Ethernet network conversion, providing network connectivity capabilities for the host.

## Features

- WiFi to Ethernet conversion
- Dual-stack network protocol support (IPv4), IPv6 forwarded to host by default
- AT command control channel
- Customizable packet filtering rules
- Multiple interface support (SDIO, USB, SPI)

## Supported Chips

Note: **√** means supported; **×** means not supported; **○** means supported but not tested.

| Interface Type | bl616/bl618 | bl616d/bl618d/bl619d | bl616L  |
|:--------------:|:-----------:|:----------------------:|:-------:|
| SDIO           |    √(sdio2) |        √(sdio3)        |  √(sdio2)   |
| USB            | In Development |      In Development   |  In Development |
| SPI            | In Development |      In Development   |  In Development |

## Build Instructions

### bl616/bl618 Series

```bash
make CHIP=<chip_name> BOARD=<board_name>
```

Example:
```bash
make CHIP=bl616 BOARD=bl616dk
```

### bl616d Series

```bash
make CHIP=bl616d BOARD=bl616ddk CPU_ID=ap CONFIG_ROMAPI=n
```

## Hardware GPIO Configuration

| Function Pin | bl616/bl618 | bl616d/bl618d/bl619d | bl616L  |
|:------------:|:-----------:|:--------------------:|:-------:|
| SDIO_DAT2    |   GPIO10    | GPIO43               | GPIO6  |
| SDIO_DAT3    |   GPIO11    | GPIO44               | GPIO7  |
| SDIO_CMD     |   GPIO12    | GPIO45               | GPIO8  |
| SDIO_CLK     |   GPIO13    | GPIO46               | GPIO9  |
| SDIO_DATA0   |   GPIO14    | GPIO47               | GPIO10 |
| SDIO_DATA1   |   GPIO15    | GPIO48               | GPIO11 |

## Flash Instructions

### All Supported Chips

```bash
make flash CHIP=<chip_name> COMX=<serial_port>
```

Example:
```bash
make flash CHIP=bl616 COMX=/dev/ttyUSB0
```

## System Architecture

The Nethub solution consists of two parts:
- **Device Side**: Runs on Bouffalo chips, handles WiFi connections and data forwarding
- **Host Side**: Runs on the host machine, communicates with Device through drivers (named host driver, abbreviated as hd)

## Notes and Frequently Asked Questions

### Q1: What interfaces will appear on the host after hardware connection and driver loading?

After loading, the host will have at least 2 additional interfaces:

1. **hd_eth0** [Required] - Network interface, equivalent to STA to Ethernet functionality
2. **/dev/ttyhd0** [Required] - TTY control channel for sending and receiving commands like connection/disconnection, using standard AT command protocol
3. **/dev/ttyhd1** [Optional] - Dedicated data channel for users to transmit arbitrary data (not yet supported)

### Q2: Which side runs the DHCP client - host or device?

The DHCP client runs on the **device** side. The host obtains IP addresses through queries. After connecting to WiFi, a GOTIP event is received, then the host updates the IP address through queries.

Refer to the `connect_and_setip` script in the host for implementation details.

### Q3: Can users customize the packet filtering on the device side? What are the default rules?

**Yes, users can customize** the filtering by re-implementing the `eth_input_filter` function.

**Default filtering rules:**
1. **ARP packets**: Bidirectional transmission
2. **DHCPv4 and ICMPv4 packets**: Handled by device side by default
3. **Port filtering**: Users can define address space filtering using `CONFIG_NETHUB_FILTER_LOCAL_PORT_MIN` and `CONFIG_NETHUB_FILTER_LOCAL_PORT_MAX`
4. **Other packets**: Forwarded to host side by default

### Q4: What are the relationships and differences between nethub, hd_sdio, easyat, and connect_and_setip?

**nethub**: Firmware software running on the device side, implementing WiFi Station to Ethernet protocol conversion, supporting data packet filtering and forwarding rule management.

**hd_sdio**: Kernel driver module (.ko file), responsible for SDIO communication between host and device.

**easyat**: AT command encapsulation library for device control, providing convenient device operation interfaces. Developers can directly send/receive AT protocol bypassing this library.

**connect_and_setip**: Network configuration example tool, implementing the complete workflow of connecting to specified AP, obtaining IP information, and setting static IP.

**Architecture Relationship**:
```
User Application Layer
    ↓ connect_and_setip (Example Tool)
    ↓ easyat (AT Command Library)
Control Channel Layer (/dev/ttyhd0)
    ↓ hd_sdio (Kernel Driver)
Physical Layer (SDIO Interface)
    ↓ nethub (Device Firmware)
```

**Data Flow**:
- `nethub` is device-side firmware, handling WiFi protocol conversion and data forwarding
- `hd_sdio` is the underlying communication driver, responsible for physical connection between host and device
- `easyat` is user-space control library, encapsulating AT command operations
- `connect_and_setip` is application-layer example, showing complete usage workflow

### Q5: What are the special considerations for host development?

**Network Configuration Requirements:**

Due to the current design where DHCP client runs on the device side, the host side needs to follow these configuration requirements:

- **Must disable DHCP service for hd_eth0**: Use static IP configuration
- **Static IP configuration steps**:
  1. Use easyat command to query link status: `./userspace/easyat/src/easyat get_link_status`
  2. Configure host-side static IP address based on query results (using the same IP as device side)
  3. Ensure host side and device side use the same IP address

**Configuration Example**:
```bash
# 1. Query link status to get IP information
./userspace/easyat/src/easyat get_link_status

# 2. Configure static IP based on query results (example)
sudo ip addr add 192.168.1.100/24 dev hd_eth0
sudo ip link set hd_eth0 up
```

⚠️ **Important Reminder**: If DHCP service is enabled on the host side, it will conflict with the device-side DHCP client, causing network connection abnormalities.

## Features To Be Implemented

- [ ] Hot-plug SDIO functionality
- [ ] hd_sdio.ko dynamic load/unload functionality
- [ ] User private channel data transmission (/dev/ttyhd1)
- [ ] Performance optimization and stability improvements
- [ ] Add USB, SPI interface support

## License

MIT License

