# NetHub Virtual Channel Demo Guide

## Overview

This directory contains demo code for the device-side NetHub USER virtual
channel. It shows how to use `nethub_vchan_user_send()` and
`nethub_vchan_user_recv_register()`.

Current scope note:

- this demo exercises the current in-tree SDIO-backed virtual-channel path
- it should not be documented as a finished USB or SPI virtual-channel
  demo today

## Design Principles

1. Single command entry: all functions are accessed through the
   `vchanuser` command.
2. Minimal static memory usage: only 1 byte of static memory is used for
   the callback registration flag.
3. Register callbacks on demand: no callback is registered by default.
4. Temporary memory allocation: tests allocate temporary buffers and free
   them immediately after the test completes.

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

- automatically registers the receive callback if needed
- uses the first 4 bytes of each packet as a big-endian sequence number
- prints progress periodically
- waits briefly so the receive path can complete
- prints TX and RX statistics at the end

### 4. MTU Test

```bash
# Test a 1024-byte MTU
vchanuser test mtu 1024
```

Notes:

- automatically registers the receive callback if needed
- sends test data with the requested length
- fills the payload with an incremental byte pattern

### 5. Statistics

```bash
vchanuser stats
```

Shows:

- receive callback registration state
- sent and received packet counts
- sent and received byte counts

### 6. Help

```bash
vchanuser help
```

## API Examples

```c
int ret = nethub_vchan_user_send(data_buf, data_len);
int ret = nethub_vchan_user_recv_register(callback_fn, NULL);
int ret = nethub_vchan_user_recv_register(NULL, NULL);
```

## File Layout

```text
virtualchan/
|- app_vchan.c
|- app_vchan.h
`- README.md
```

## Notes

1. Maximum payload length is `1500` bytes (`NETHUB_VCHAN_MAX_DATA_LEN`).
2. Hex strings must have an even length.
3. The flow test registers the receive callback automatically.
4. Statistics are valid only for the current run.
