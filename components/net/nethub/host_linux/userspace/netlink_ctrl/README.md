# Netlink Control Library

A Netlink userspace library and application for communication with the kernel Netlink module.

## Features

- **Type-based callbacks**: Register separate callbacks for different data types
- **Multi-process support**: Multiple processes can receive data simultaneously
- **Data type support**: SYSTEM, WIFI, BLE, USER, LOG
- **Flow control**: Built-in credit-based flow control
- **Easy to use**: Simple API and interactive CLI

## Building

```bash
cd netlink_ctrl
make
```

## Quick Start

### Starting the Application

```bash
./netlink_app
```

### Interactive Commands

```
netlink> send user 12345678
Sent 8 bytes (type=USER)

netlink> send wifi "wifitestdata"
Sent 14 bytes (type=WIFI)

netlink> send ble "bletestdata"
Sent 13 bytes (type=BLE)

netlink> test flow 1000 512
[FLOW_TEST] Starting flow control test: count=1000, size=512
[FLOW_TEST] Received 100 packets, 51200 bytes, 0 lost
[FLOW_TEST] Received 200 packets, 102400 bytes, 0 lost
...
[FLOW_TEST] Received 1000 packets, 512000 bytes, 0 lost

netlink> test mtu 1024
[MTU_TEST] Testing MTU: size=1024 bytes
[MTU_TEST] Successfully sent 1024 bytes

[Received] 7 bytes (type=USER): Hello from device

netlink> status
Connection: Active
Sent: 1003 packets
Received: 1008 packets

netlink> help
Available commands:
  send [type] <string>   Send data with optional type
                         Types: system, wifi, ble, user, log
  test flow <cnt> <sz>   Flow control test (send count packets)
  test mtu <size>        MTU test (send data with specified size)
  status                 Show connection status
  help                   Show this help message
  quit/exit              Exit program

netlink> quit
Bye!
```

## Prerequisites

Before using, ensure the kernel Netlink module is loaded:

```bash
# Load module
sudo insmod hd_netlink.ko

# Check dmesg
dmesg | grep Netlink
```

Expected output:
```
Netlink: Netlink module initialized successfully
Netlink: Device Ready! Credit Limit=3
```

## Installation

```bash
# Install to system
sudo make install

# Uninstall
sudo make uninstall
```

## Usage as a Library

### API Overview

```c
#include "netlink_ctrl.h"
#include "netlink_protocol.h"

/* Initialize */
netlink_handle_t handle;
netlink_init(&handle);

/* Register callbacks for specific data types */
netlink_register_callback(handle, NETLINK_DATA_TYPE_WIFI, wifi_callback);
netlink_register_callback(handle, NETLINK_DATA_TYPE_BLE, ble_callback);

/* Send data */
const char *data = "Hello";
netlink_send(handle, NETLINK_DATA_TYPE_USER, data, strlen(data));

/* Receive data (callback will be invoked) */
void wifi_callback(const void *data, size_t len) {
    // Handle WiFi data
}

/* Cleanup */
netlink_deinit(handle);
```

### Example Program

```c
#include "netlink_ctrl.h"
#include "netlink_protocol.h"

static void wifi_handler(const void *data, size_t len) {
    printf("WiFi: Received %zu bytes\n", len);
    // Process WiFi data
}

static void ble_handler(const void *data, size_t len) {
    printf("BLE: Received %zu bytes\n", len);
    // Process BLE data
}

int main() {
    netlink_handle_t handle;

    /* Initialize */
    netlink_init(&handle);

    /* Register callbacks */
    netlink_register_callback(handle, NETLINK_DATA_TYPE_WIFI, wifi_handler);
    netlink_register_callback(handle, NETLINK_DATA_TYPE_BLE, ble_handler);

    /* Send data */
    netlink_send(handle, NETLINK_DATA_TYPE_WIFI, "scan", 4);

    /* Receive data in main loop or thread */
    // ...

    /* Cleanup */
    netlink_deinit(handle);
    return 0;
}
```

## Data Types

| Type | Value | Description |
|------|-------|-------------|
| **SYSTEM** | 0x01 | System messages |
| **WIFI** | 0x02 | WiFi related data |
| **BLE** | 0x03 | BLE related data |
| **USER** | 0x04 | User data (default) |
| **LOG** | 0x05 | Log data |

## Multi-Process Support

Multiple processes can run and receive data simultaneously:

```bash
# Terminal 1
./netlink_app

# Terminal 2
./netlink_app

# Both terminals will receive data sent from device
```

## File Structure

```
netlink_ctrl/
├── netlink_protocol.h    # Protocol definitions (data types, message header)
├── netlink_ctrl.h        # API header file
├── netlink_ctrl.c        # Core Netlink communication library
├── netlink_app.c         # Interactive CLI application
├── Makefile               # Build script
└── README.md             # This file
```

## Data Format

All data sent through Netlink includes a type header:

```
+----------+------------+--------+
| data_type| reserved   | data[] |
| 1 byte   | 3 bytes    | N bytes|
+----------+------------+--------+
```

- `data_type`: Specifies the data type (SYSTEM/WIFI/BLE/USER/LOG)
- `reserved[3]`: Reserved for future expansion
- `data[]`: Actual user data

## Testing

### Flow Control Test

The flow control test verifies that the credit-based flow control mechanism prevents packet loss during continuous data transmission.

**Host Command:**
```bash
netlink> test flow <count> <size>
```

**Device Command:**
```bash
netlink test flow <count> <size>
```

**Parameters:**
- `count`: Number of packets to send (e.g., 1000)
- `size`: Size of each packet in bytes (1-1024)

**How it works:**
1. Transmitter sends packets with 4-byte sequence numbers
2. Receiver checks sequence continuity and tracks lost packets
3. Progress is printed every 100 packets

**Example:**
```bash
# Host side
netlink> test flow 1000 512
[FLOW_TEST] Starting flow control test: count=1000, size=512
[FLOW_TEST] Received 100 packets, 51200 bytes, 0 lost
[FLOW_TEST] Received 200 packets, 102400 bytes, 0 lost
...
[FLOW_TEST] Received 1000 packets, 512000 bytes, 0 lost

# Device side (simultaneously)
netlink test flow 1000 512
[FLOW_TEST] Starting flow control test: count=1000, size=512
[FLOW_TEST] Received 100 packets, 51200 bytes, 0 lost
...
```

**Expected Result:** If flow control works correctly, lost packets should be 0.

---

### MTU Test

The MTU test verifies the maximum transmission unit by sending data of specified sizes up to 1024 bytes.

**Host Command:**
```bash
netlink> test mtu <size>
```

**Device Command:**
```bash
netlink test mtu <size>
```

**Parameters:**
- `size`: Data size in bytes (1-1024)

**How it works:**
1. Transmitter sends data filled with test pattern (0x00, 0x01, 0x02...)
2. Receiver verifies data integrity

**Example:**
```bash
# Test maximum MTU
netlink> test mtu 1024
[MTU_TEST] Testing MTU: size=1024 bytes
[MTU_TEST] Successfully sent 1024 bytes

# Test different sizes
netlink> test mtu 64
[MTU_TEST] Testing MTU: size=64 bytes
[MTU_TEST] Successfully sent 64 bytes
```

**Expected Result:** All sizes from 1 to 1024 bytes should be sent and received successfully.

---

### Test Scenarios

**Scenario 1: Bidirectional Flow Control Test**
```
# Terminal 1 (Host)
netlink> test flow 1000 512

# Terminal 2 (Device)
netlink test flow 1000 512
```
Both sides send 1000 packets of 512 bytes simultaneously to verify flow control.

**Scenario 2: MTU Boundary Test**
```bash
# Test various sizes
netlink> test mtu 1      # Minimum
netlink> test mtu 64     # Small
netlink> test mtu 512    # Medium
netlink> test mtu 1024   # Maximum
```

---

## Capabilities Required

### For Multicast Reception
To receive multicast messages from the kernel, the application needs:

```bash
# Option 1: Run with sudo
sudo ./netlink_app

# Option 2: Grant CAP_NET_RAW capability
sudo setcap cap_net_raw+ep ./netlink_app
```

### For Sending
No special privileges required. Any userspace process can send data.

## Troubleshooting

### Failed to initialize Netlink
```
Error: Failed to initialize netlink: -1
```
**Solution**: Ensure kernel module is loaded:
```bash
sudo insmod hd_netlink.ko
```

### Permission denied
```
bind: Permission denied
```
**Solution**: Run with sudo or grant CAP_NET_RAW capability (see above)

### No data received
**Check**:
1. Is kernel module loaded? (`lsmod | grep hd_netlink`)
2. Is callback registered? (Application should show "Registered" message)
3. Is device sending data? (Check kernel logs: `dmesg | tail -20`)

## License

See individual files for license information.

## Contact

For issues or questions, please contact the development team.
