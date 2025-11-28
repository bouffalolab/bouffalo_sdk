# NetHub Userspace Tools

This is the NetHub Linux host driver userspace toolset, providing a complete solution for communicating with WiFi modules through TTY channels. It includes both C language and Expect script implementations, supporting WiFi connection management, status queries, and network scanning.

## Features

- **WiFi Connection Management**: Connect to specified WiFi networks (supports encrypted and open networks)
- **AP Disconnection**: Disconnect from currently connected WiFi hotspots
- **Status Query**: Query current WiFi connection status and IP information
- **Network Scanning**: Scan for available WiFi networks in the vicinity
- **Dual Implementation Support**: C language version and Expect script version
- **One-Click Network Access**: Automated WiFi connection and static IP configuration
- **Hardware Abstraction**: Unified communication based on `/dev/ttyHD0` TTY interface

## Quick Start

### One-Click Network Access and Static IP Configuration (for testing only, users should preferably use easyat commands for manual connection and static IP configuration)
```bash
./connect_and_setip <ssid> [password]
```

### C Language Version Usage
```bash
cd easyat/src
make
./easyat/src/easyat connect MyWiFi password123
./easyat/src/easyat connect MyOpenWifi
./easyat/src/easyat disconnect_ap
./easyat/src/easyat get_link_status
./easyat/src/easyat wifi_scan
```

### Expect Script Version Usage
```bash
cd easyat/scripts
./connect MyWiFi password123
./disconnect_ap
./get_link_status
./wifi_scan
```

## Project Structure

```
userspace/
├── README.md                    # This document, project overview and quick start
├── connect_and_setip            # One-click network access and static IP configuration script (for testing only)
├── easyat/                      # Main project directory
│   ├── src/                     # C language implementation
│   │   ├── easyat.c            # Main program source code
│   │   └── Makefile            # C version build script
│   ├── scripts/                # Expect script implementation
│   │   ├── connect             # WiFi connection script
│   │   ├── disconnect_ap       # AP disconnection script
│   │   ├── get_link_status     # Status query script
│   │   └── wifi_scan           # WiFi scanning script
│   └── docs/                   # Detailed documentation
│       ├── c-implementation.md     # C implementation detailed description
│       ├── expect-implementation.md # Expect implementation detailed description
│       └── api-reference.md         # API and command reference
└── tools/                      # Other auxiliary tools
```

## Requirements

### Basic Requirements
- **Operating System**: Linux
- **Serial Device**: `/dev/ttyHD0`
- **Permissions**: Read/write permissions for serial device

### Additional Requirements for C Version
- GCC compiler
- Make tool

### Additional Requirements for Expect Version
- Expect interpreter
```bash
# Ubuntu/Debian installation
sudo apt-get install expect
```

## Permissions Setup

```bash
# Add user to dialout group for serial port permissions
sudo usermod -aG dialout $USER
newgrp dialout
```

## Implementation Comparison

| Feature | C Language Version | Expect Script Version |
|---------|-------------------|----------------------|
| Performance | High performance, compiled execution | Interpreted execution, moderate performance |
| Dependencies | Only standard C library required | Requires Expect interpreter |
| Debugging | Suitable for debugging and development | Suitable for quick testing |
| Deployment | Requires compilation | Direct execution |
| Maintenance | Clear code structure | Simple and easy to understand scripts |

**Recommended Scenarios**:
- **Production Environment**: Use C language version
- **Development and Debugging**: Use Expect script version
- **Quick Verification**: Use Expect script version

## Technical Specifications

- **Serial Configuration**: `/dev/ttyHD0`, 115200 8N1
- **Timeout Settings**:
  - WiFi connection: 20 seconds
  - Status query: 2 seconds
  - WiFi scan: 10 seconds
- **Response Detection**: Detect "GOTIP" (success) and "ERROR" (failure)

## Return Values

- `0`: Command executed successfully
- `1`: Command execution failed (serial error, timeout, parameter error, etc.)

## Build and Installation

### Build C Version
```bash
cd easyat/src
make
```

### Clean Build Files
```bash
cd easyat/src
make clean
```

### Install to System (Optional)
```bash
cd easyat/src
sudo make install
```

### Uninstall (Optional)
```bash
cd easyat/src
sudo make uninstall
```

## Usage Examples

### Basic Operations
```bash
# Connect to encrypted WiFi (user needs to configure static IP after connection)
./easyat/src/easyat connect MyWiFi password123

# Connect to open network (user needs to configure static IP after connection)
./easyat/src/easyat OpenWiFi

# Disconnect from AP
./easyat/src/easyat disconnect_ap

# Query connection status
./easyat/src/easyat get_link_status

# Scan networks
./easyat/scripts/wifi_scan
```

## Troubleshooting

### Common Issues

1. **Permission Denied**
   ```bash
   sudo usermod -aG dialout $USER
   newgrp dialout
   ```

2. **Device Does Not Exist**
   - Check if kernel module is loaded: `lsmod | grep hd_`
   - Check device node: `ls -l /dev/ttyHD0`

3. **Connection Timeout**
   - Check if WiFi name and password are correct
   - Confirm device is within signal range

4. **Compilation Failure**
   - Check if GCC toolchain is installed
   - Confirm Makefile path is correct

### Debugging Tips

- **View Serial Output**: Use `screen /dev/ttyHD0 115200` to monitor communication
- **Verbose Logging**: C version displays detailed send/receive information
- **Expect Debugging**: Add `exp_internal 1` at the beginning of scripts to show interaction process

## Related Documentation

- [C Implementation Detailed Description](easyat/docs/c-implementation.md)
- [Expect Implementation Detailed Description](easyat/docs/expect-implementation.md)
- [API and Command Reference](easyat/docs/api-reference.md)
- [Kernel Driver Documentation](../kernel/README.md)

## License

MIT License - See LICENSE file in project root directory for details
