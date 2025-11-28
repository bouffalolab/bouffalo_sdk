## nethub_host

This project implements kernel and userspace components. The kernel compiles to generate .ko files that virtualize multiple interfaces as Ethernet and TTY channels. The userspace component is an AT-based application built on TTY channels that primarily handles device connection, scanning, and querying functions.

## Features

- **Host Driver**: Virtualizes multiple interfaces as Ethernet and TTY channels
- **WiFi Connection**: Connect to specified WiFi networks, query WiFi status, scan available WiFi networks
- **Dual Implementations**: C language version and Expect script version

## Project Structure

- kernel: Kernel components that compile to *.ko files
- userspace: Applications that compile to executable files or directly execute expect scripts

## Quick Start

### Build hd_sdio.ko and application firmware
```bash
./build.sh build
```

### Load hd_sdio.ko
```bash
sudo ./build.sh load
```

### Connect to network and configure static IP (this script is for testing only, see details below)
```bash
./userspace/connect_and_setip <ssid> [password]
```

### WiFi Connection
```bash
# Connect to WiFi
./userspace/easyat/src/easyat connect MyWiFi password123

# Connect to open network
./userspace/easyat/src/easyat connect OpenWiFi
```

### WiFi Disconnection
```bash
./userspace/easyat/src/easyat disconnect_ap
```

### Status Query
```bash
./userspace/easyat/src/easyat get_link_status
```

### Network Scan
```bash
./userspace/easyat/src/easyat wifi_scan
```

## Notes and Common Questions

### Q1: What interfaces appear on the host after hardware connection and driver loading?

After loading, the host will have at least 2 additional interfaces:

1. **hd_eth0** [Required] - Network interface, equivalent to STA to Ethernet functionality
2. **/dev/ttyhd0** [Required] - TTY control channel, connection/disconnection commands are sent and received through this channel using standard AT command protocol
3. **/dev/ttyhd1** [Optional] - Data channel exclusively for users, users can use this interface to transmit arbitrary data (currently not supported)

### Q2: Where does the DHCP client run between host and device?

The DHCP client runs on the **device** side. The host obtains IP addresses through a query mechanism. After connecting to WiFi, a GOTIP event is received, then the host updates the IP address through querying.

You can refer to the `connect_and_setip` script implementation in the host.

### Q3: Can users customize the packet filter on the device side? What are the default rules?

**Yes, it can be customized**. Users can complete filter customization by re-implementing the `eth_input_filter` function on the device side.

**Default filtering rules:**
1. **ARP packets**: Bidirectional transmission
2. **DHCPv4 and ICMPv4 packets**: Default to device-side processing, not delivered to host side
3. **Port filtering**: Users can complete address space filtering by defining `CONFIG_NETHUB_FILTER_LOCAL_PORT_MIN` and `CONFIG_NETHUB_FILTER_LOCAL_PORT_MAX`
4. **Other packets**: Default to host-side processing

### Q4: What are the relationships and differences between nethub, hd_sdio, easyat, and connect_and_setip?

**nethub**: Firmware software running on the device side, implements WiFi Station to Ethernet protocol conversion, supports packet filtering and forwarding rule management.

**hd_sdio**: Kernel driver module (.ko file), responsible for SDIO communication between host and device.

**easyat**: AT command encapsulation library for users to control devices, providing convenient device operation interfaces. Developers can directly send/receive AT protocols bypassing this library.

**connect_and_setip**: Network configuration example tool that implements the complete process of connecting to specified AP, obtaining IP information, and setting static IP.

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

**Data Flow Direction**:
- `nethub` is device-side firmware that handles WiFi protocol conversion and data forwarding
- `hd_sdio` is the underlying communication driver responsible for physical connection between host and device
- `easyat` is a userspace control library that encapsulates AT command operations
- `connect_and_setip` is an application layer example showing the complete usage process

### Q5: What are special considerations for host development?

**Network Configuration Requirements:**

Since the current DHCP client design is on the device side, the host side needs to follow these configuration requirements:

- **Must disable DHCP service for hd_eth0**: Use static IP configuration
- **Static IP Configuration Steps**:
  1. Use easyat command to query link status: `./userspace/easyat/src/easyat get_link_status`
  2. Configure static IP address on host side based on query results (same IP as device side)
  3. Ensure host side and device side use the same IP address

**Configuration Example:**
```bash
# 1. Query link status to get IP information
./userspace/easyat/src/easyat get_link_status

# 2. Configure static IP based on query results (example)
sudo ip addr add 192.168.1.100/24 dev hd_eth0
sudo ip link set hd_eth0 up
```

⚠️ **Important Reminder**: If DHCP service is enabled on the host side, it will conflict with the DHCP client on the device side, causing network connection abnormalities.

## Features to be Implemented

- [ ] Hot-plug SDIO functionality
- [ ] hd_sdio.ko dynamic loading/unloading functionality
- [ ] User private channel data transmission (/dev/ttyhd1)
- [ ] Performance optimization and stability improvements
- [ ] Add USB and SPI related interface support

## License

MIT License

