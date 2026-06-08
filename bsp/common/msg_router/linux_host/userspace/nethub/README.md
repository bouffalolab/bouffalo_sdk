# NetHub Host User Guide

This document explains the current host-side Linux stack under
`bsp/common/msg_router/linux_host/userspace/nethub`.

If this is your first NetHub bring-up, read these first:

- `examples/wifi/nethub/README.md`
- `examples/wifi/nethub/docs/NetHubQuickBringup.md`

## 1. Scope of This Host Stack

This directory describes the current in-tree Linux host stack.
Today that stack is primarily the SDIO flow.

Current status:

- `SDIO`
  - current host stack for the default `SDIO` interface
- `USB`
  - device-side NetHub backend exists, but this userspace stack is not
    yet the main in-tree USB host stack
- `SPI`
  - not implemented

Also note:

- `bflbwifid / bflbwifictrl` are only needed when you use the optional
  AT-style control solution
- data-path-only products do not need this full host control stack

## 2. Components

Main host-side components:

- `mr_sdio.ko`
  - kernel communication base for the current SDIO host stack
- `bflbwifid`
  - control path daemon
- `bflbwifictrl`
  - command-line tool
- `libbflbwifi.a`
  - Wi-Fi control library
- `libnethub_vchan.a`
  - host USER virtual channel library for the current SDIO path
- `nethub_vchan_app`
  - optional debug tool

Supported control backends in this host stack:

- `tty`
- `vchan`

Notes:

- the host build keeps both `tty` and `vchan` backends available
- `vchan` in this repository is still aligned with the SDIO path
- do not read this guide as a finished USB host guide

## 3. Quick Bring-Up

Build:

```bash
cd bsp/common/msg_router/linux_host/userspace/nethub
./build.sh build
```

Artifacts include:

- `output/mr_sdio.ko`
- `output/bflbwifid`
- `output/bflbwifictrl`
- `output/libnethub_vchan.a`
- `output/nethub_vchan_app`

Load the kernel module:

```bash
sudo ./build.sh load
```

Unload it:

```bash
sudo ./build.sh unload
```

## 4. Start the Daemon

Default mode:

```bash
sudo ./output/bflbwifid
```

Explicit TTY device:

```bash
sudo ./output/bflbwifid -c tty -p /dev/ttyAT0
```

VCHAN mode:

```bash
sudo ./output/bflbwifid -c vchan
```

Optional path arguments:

- `-s <socket_path>`
- `-P <pid_file>`
- `-l <log_file>`

## 5. Use the CLI

Check status:

```bash
sudo ./output/bflbwifictrl status
```

Scan APs:

```bash
sudo ./output/bflbwifictrl scan
```

Connect to an AP:

```bash
sudo ./output/bflbwifictrl connect_ap "<ssid>" "<password>"
```

Disconnect:

```bash
sudo ./output/bflbwifictrl disconnect
```

## 6. Control-Plane Notes

The device-side `nethub` core does not directly depend on `ATModule`.
This host stack is relevant when the optional example control solution is
used.

Important configuration note:

- `CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE=y`
  - example enables the AT control stack
- `CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE=n`
  - the project can stay data-path-focused and skip this stack

About `CONFIG_NETHUB_AT_USE_VCHAN`:

- it still appears in the example configuration and older docs
- treat it as an example or legacy SDIO control path convention
- do not interpret it as proof that USB and SPI already share the same
  finished host control path

## 7. USER Virtual Channel

If you need private data exchange between host and device, use the USER
virtual channel.

Header:

- `virtualchan/nethub_vchan.h`

Common APIs:

```c
int nethub_vchan_init(void);
int nethub_vchan_deinit(void);
int nethub_vchan_user_send(const void *data, size_t len);
int nethub_vchan_user_register_callback(nethub_vchan_recv_callback_t callback);
int nethub_vchan_get_state_snapshot(nethub_vchan_state_snapshot_t *snapshot);
```

Current implementation:

- the in-tree host USER virtual channel path is SDIO-based today
- host-side `nethub_vchan` is not yet the generic wrapper for USB ACM
- for more detail, read `virtualchan/README.md`

## 8. Automatic Network Configuration

`build.sh` builds userspace with `ENABLE_NETIF_AUTO_CONFIG=1` by default.

After the device reports `GOTIP`, the host tries to configure the Linux
network interface selected by `NETHUB_NETIF_NAME`. If the environment
variable is not set, it defaults to `mr_eth0`.

- the IP address and netmask
- the default gateway
- DNS

If `connect_ap` succeeds but the host still cannot reach the network,
check:

- whether `status` has reached `GOTIP`
- whether the selected interface, for example `mr_eth0` or `usb0`, has
  an address
- whether services such as NetworkManager or `dhcpcd` override the
  NetHub configuration

## 9. Related Documents

- `HOST_ARCHITECTURE.md`
- `virtualchan/README.md`
- `bflbwifictrl/README.md`
- `examples/wifi/nethub/docs/NetHubQuickBringup.md`
