# NetHub Host User Guide

This document is for host-side users and explains how to bring up NetHub on a
Linux host.

If this is your first full NetHub bring-up, start with:

- `examples/wifi/nethub/README.md`
- `examples/wifi/nethub/docs/NetHubQuickBringup.md`

Online wiki:

- <https://docs.bouffalolab.com/index.php?title=NetHub>
- <https://docs.bouffalolab.com/index.php?title=NetHubQuickBringup>

Active host-side directories:

- `bsp/common/msg_router/linux_host/kernel`
- `bsp/common/msg_router/linux_host/userspace/nethub`

The current primary path is `SDIO`.
`tty / vchan / USER virtual channel` are all carried on this same host link.
`tty / vchan` select the control-plane backend, while `USER virtual channel`
is for private application data.

## 1. Components

Main host-side components:

- `mr_sdio.ko`
  - host kernel module
- `bflbwifid`
  - control-plane daemon
- `bflbwifictrl`
  - command-line tool
- `libbflbwifi.a`
  - Wi-Fi control library
- `libnethub_vchan.a`
  - user-space virtual channel library
- `nethub_vchan_app`
  - optional debug tool

Supported control backends:

- `tty`
- `vchan`

Notes:

- The host build always includes both `tty + vchan`.
- The actual backend is selected when `bflbwifid` starts.

## 2. Quick Bring-Up

### 2.1 Build

```bash
cd bsp/common/msg_router/linux_host/userspace/nethub
./build.sh build
```

Artifacts:

- `output/mr_sdio.ko`
- `output/bflbwifid`
- `output/bflbwifictrl`
- `output/libnethub_vchan.a`
- `output/nethub_vchan_app`

### 2.2 Load the Kernel Module

```bash
sudo ./build.sh load
```

To unload:

```bash
sudo ./build.sh unload
```

### 2.3 Start the Daemon

#### Default TTY mode

```bash
sudo ./output/bflbwifid
```

#### Explicit TTY device

```bash
sudo ./output/bflbwifid -c tty -p /dev/ttyAT0
```

#### VCHAN mode

```bash
sudo ./output/bflbwifid -c vchan
```

Optional path arguments:

- `-s <socket_path>`
- `-P <pid_file>`
- `-l <log_file>`

### 2.4 Use the CLI

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

## 3. Control Backend Selection

The host backend must match the device configuration:

| Device configuration | Host startup |
| --- | --- |
| `CONFIG_NETHUB_AT_USE_VCHAN=n` | `./output/bflbwifid` or `./output/bflbwifid -c tty -p /dev/ttyAT0` |
| `CONFIG_NETHUB_AT_USE_VCHAN=y` | `./output/bflbwifid -c vchan` |

By default, the device control channel uses `TTY`, so the usual host startup
command is:

```bash
sudo ./output/bflbwifid
```

## 4. Common Commands

The public CLI surface is fixed to 9 commands:

- `connect_ap`
- `disconnect`
- `scan`
- `status`
- `version`
- `reboot`
- `ota`
- `start_ap`
- `stop_ap`

Examples:

```bash
sudo ./output/bflbwifictrl version
sudo ./output/bflbwifictrl reboot
sudo ./output/bflbwifictrl start_ap "<ssid>" "<password>"
sudo ./output/bflbwifictrl stop_ap
```

Detailed command documentation is currently in:

- `bflbwifictrl/COMMANDS_CN.md`

## 5. Automatic Network Configuration

`build.sh` builds userspace with `ENABLE_NETIF_AUTO_CONFIG=1` by default.

After the device reports `GOTIP`, the host will try to configure:

- the IP address of `mr_eth0`
- the default gateway
- DNS

If `connect_ap` succeeds but the host still cannot reach the network, check:

- whether `status` has reached `GOTIP`
- whether `mr_eth0` has an address
- whether services such as NetworkManager or `dhcpcd` override the NetHub
  configuration

## 6. USER Virtual Channel

If you need to transfer private data between the host and device, use the USER
Virtual Channel.

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

For more details, see:

- `virtualchan/README.md`

If an application needs to check whether the virtual channel is ready before
sending, the following snapshot fields are the usual checks:

- `link_state == NETHUB_VCHAN_LINK_UP`
  - the link is ready for transfer
- `host_state == NETHUB_VCHAN_HOST_STATE_DEVICE_RUN`
  - the host has completed the handshake with the device

## 7. FAQ

### 7.1 `bflbwifid` fails to start

Check first:

- whether the backend selection matches the device configuration
- whether `/dev/ttyAT0` exists
- whether `mr_sdio.ko` loaded successfully
- whether the hardware link is healthy

### 7.2 `build.sh unload` reports that the module is busy

Check first:

- whether `bflbwifid` is still running
- whether `nethub_vchan_app` is still running

Usually you can just run:

```bash
sudo ./build.sh unload
```

The script will first stop known holders and then unload the module.

## 8. Related Documents

- `examples/wifi/nethub/README.md`
- `examples/wifi/nethub/docs/NetHub.md`
- `examples/wifi/nethub/docs/NetHubQuickBringup.md`
- `HOST_ARCHITECTURE.md`
- `bflbwifictrl/README.md`
- `bflbwifictrl/COMMANDS_CN.md`
- `virtualchan/README.md`
