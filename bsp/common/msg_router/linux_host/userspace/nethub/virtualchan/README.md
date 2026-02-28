# NetHub VirtualChannel

NetHub Virtual Channel - High-efficiency data transmission between userspace and kernelspace based on Netlink mechanism.

## Features

- **Multiple Data Types**: Support for USER, AT, and SYSTEM data channels
- **Message Packet Transmission**: Transmits in message packets, receiving complete packets at the bottom layer (non-stream)
- **Maximum Data Length**: Maximum 1500 bytes per transmission
- **Callback Mechanism**: Support for registering data receive callbacks
- **Userspace API**: Simple C language interface

## Data Types

| Type | Value | Usage |
|------|-------|-------|
| USER | 0x01 | User private data |
| AT | 0x02 | WiFi related data/AT commands |
| SYSTEM | 0x03 | System related data |

## Quick Start

### Build

```bash
cd virtualchan/
make
```

Build artifact: `nethub_vchan_app`

### Run Test Program

```bash
# Requires kernel module mr_sdio.ko to be loaded
sudo ./nethub_vchan_app
```

### Command Reference

The test program supports the following commands:

| Command | Description | Example |
|---------|-------------|---------|
| `send <type> <data>` | Send data | `send user hello` |
| `test mtu` | MTU test | `test mtu` |
| `status` | View status | `status` |
| `help` | Display help | `help` |
| `quit/exit` | Exit program | `quit` |

**Data Type Parameters**:
- `user` - USER type data
- `at` - AT type data
- `system` - SYSTEM type data

### Usage Examples

```bash
# Send USER type data
nethub_vchan> send user test_message_123

# View status
nethub_vchan> status
Virtual Channel Status: Active
USER callback: registered
AT callback: not registered
SYSTEM callback: not registered

# MTU test
nethub_vchan> test mtu
Testing maximum transfer unit...
MTU test completed: max size=1500 bytes
```

## API Interface

### Initialization and Cleanup

```c
/* Initialize virtual channel */
int nethub_vchan_init(void);

/* Clean up virtual channel */
int nethub_vchan_deinit(void);
```

### Data Sending

```c
/* Send data (generic interface) */
int nethub_vchan_send(uint8_t type, const void *data, size_t len);

/* Send USER type data (convenience interface) */
int nethub_vchan_user_send(const void *data, size_t len);

/* Send AT type data (convenience interface) */
int nethub_vchan_at_send(const void *data, size_t len);
```

### Callback Registration

```c
/* Receive callback function type */
typedef void (*nethub_vchan_recv_callback_t)(const void *data, size_t len);

/* Register data receive callback (generic interface) */
int nethub_vchan_register_callback(uint8_t data_type,
                                    nethub_vchan_recv_callback_t callback);

/* Register USER type callback (convenience interface) */
int nethub_vchan_user_register_callback(nethub_vchan_recv_callback_t callback);

/* Register AT type callback (convenience interface) */
int nethub_vchan_at_register_callback(nethub_vchan_recv_callback_t callback);

/* Unregister callback */
int nethub_vchan_unregister_callback(uint8_t data_type);
```

### Code Example

```c
#include "nethub_vchan.h"

/* Data receive callback function */
void user_data_callback(const void *data, size_t len)
{
    printf("Received USER data: %.*s\n", (int)len, (char *)data);
}

int main(void)
{
    /* Initialize virtual channel */
    if (nethub_vchan_init() != 0) {
        fprintf(stderr, "Failed to initialize virtual channel\n");
        return -1;
    }

    /* Register USER data receive callback */
    nethub_vchan_user_register_callback(user_data_callback);

    /* Send USER data */
    const char *msg = "Hello from userspace!";
    nethub_vchan_user_send(msg, strlen(msg));

    /* Wait for data... */
    sleep(1);

    /* Cleanup */
    nethub_vchan_deinit();
    return 0;
}
```

## Build Options

### Enable Readline Support

If the readline library is installed on the system, command auto-completion and history features can be enabled:

```bash
make HAVE_READLINE=1
```

Install readline:
```bash
# Ubuntu/Debian
sudo apt-get install libreadline-dev

# CentOS/RHEL
sudo yum install readline-devel
```

### Clean Build Artifacts

```bash
make clean
```

## Directory Structure

```
virtualchan/
├── nethub_vchan.h          # Virtual channel header file (API interface)
├── nethub_vchan.c          # Virtual channel core implementation
├── nethub_vchan_app.c      # Test application
├── Makefile                # Build configuration
└── README_CN.md            # Chinese version of this document
```

## Error Codes

| Error Code | Macro Definition | Description |
|------------|------------------|-------------|
| 0 | NETHUB_VCHAN_OK | Success |
| -1 | NETHUB_VCHAN_ERROR | Generic error |
| -2 | NETHUB_VCHAN_ERROR_INIT | Initialization failed |
| -3 | NETHUB_VCHAN_ERROR_NOT_INIT | Not initialized |
| -4 | NETHUB_VCHAN_ERROR_PARAM | Invalid parameter |
| -5 | NETHUB_VCHAN_ERROR_BUSY | Busy state |
| -6 | NETHUB_VCHAN_ERROR_NOMEM | Out of memory |
| -7 | NETHUB_VCHAN_ERROR_IO | IO error |

## Dependencies

- **Kernel Module**: Requires `mr_sdio.ko` kernel driver to be loaded
- **System Libraries**:
  - Standard: libc, pthread
  - Optional: libreadline (for enhanced command-line interface)

## Limitations and Notes

1. **Data Length Limit**: Maximum 1500 bytes per transmission
2. **Root Privilege Required**: Netlink communication requires privileged user
3. **Depends on Kernel Module**: Must load `mr_sdio.ko` first
4. **Message Packet Integrity**: Guarantees packet-based reception without half-packets or sticky packets

## Application Scenarios

- **Private Data Channel**: User-defined data transmission
- **AT Command Channel**: WiFi configuration and control
- **System Message Channel**: Information exchange between kernelspace and userspace
