# NetHub Virtual Channel Demo Guide

## Overview

This directory contains demo code for NetHub Virtual Channel. It shows how to
use `nethub_vchan_user_send` and `nethub_vchan_user_recv_register`.

## Design Principles

1. **Single command entry**: all functions are accessed through the `vchanuser`
   command.
2. **Minimal static memory usage**: only 1 byte of static memory is used, for
   the `g_recv_callback_registered` flag.
3. **Register callbacks on demand**: no callback is registered by default; the
   user enables it explicitly through commands.
4. **Temporary memory allocation**: tests use `pvPortMalloc` temporarily and
   free the memory immediately after the test completes.

## Command Format

```text
vchanuser <subcmd> [args...]
```

## Subcommands

### 1. Send Data

```bash
# Send an ASCII string
vchanuser send hello

# Send hexadecimal data
vchanuser send hex 0102030405
```

### 2. Receive Callback Control

```bash
# Register the receive callback
vchanuser recv on

# Unregister the receive callback
vchanuser recv off
```

### 3. Flow Test

```bash
# Send 100 packets, each 512 bytes
vchanuser test flow 100 512
```

Notes:

- Automatically registers the receive callback if it is not already registered
- Uses the first 4 bytes of each packet as a big-endian sequence number
- Prints progress every 100 packets
- Waits 1 second so the receive path can complete
- Prints TX/RX statistics at the end

### 4. MTU Test

```bash
# Test a 1024-byte MTU
vchanuser test mtu 1024
```

Notes:

- Automatically registers the receive callback if it is not already registered
- Sends test data with the requested length
- Fills the payload with an incremental pattern (`0x00`, `0x01`, `0x02`, ...)

### 5. Statistics

```bash
vchanuser stats
```

Shows:

- receive callback registration state
- sent/received packet counts and byte counts

### 6. Help

```bash
vchanuser help
```

## Usage Examples

### Basic Send/Receive Test

```bash
# 1. Enable receive
vchanuser recv on

# 2. Send test data
vchanuser send test123

# 3. Check the receive result
#    The device side prints the received payload automatically.

# 4. Disable receive
vchanuser recv off
```

### Flow Test

```bash
# Send 1000 packets, each 1024 bytes
vchanuser test flow 1000 1024

# Show statistics
vchanuser stats
```

### MTU Probing

```bash
# Try different sizes to find the maximum MTU
vchanuser test mtu 512
vchanuser test mtu 1024
vchanuser test mtu 1500
```

## API Examples

The demo shows how to use the following APIs:

```c
// Send data
int ret = nethub_vchan_user_send(data_buf, data_len);

// Register a receive callback
int ret = nethub_vchan_user_recv_register(callback_fn, NULL);

// Unregister the receive callback by passing NULL
int ret = nethub_vchan_user_recv_register(NULL, NULL);
```

## File Layout

```text
virtualchan/
|- app_vchan.c       # main implementation
|- app_vchan.h       # header
`- README.md         # this document
```

## Notes

1. The maximum payload length is 1500 bytes (`NETHUB_VCHAN_MAX_DATA_LEN`).
2. Hex strings must have an even length, for example `"0102"` instead of `"010"`.
3. The flow test registers the receive callback automatically.
4. Statistics are only valid for the current run and are cleared after reset.
