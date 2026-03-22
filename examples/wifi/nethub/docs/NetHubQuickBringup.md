# NetHub Quick Start

This document is for customers who are using NetHub for the first time. The
goal is to help you complete device and host bring-up as quickly as possible.

![NetHub SDIO Quick Start Demo](https://docs.bouffalolab.com/images/d/d4/Nethub_sdio_quickstart_demo.gif)

If you only care about the currently available solution, keep these points in
mind:

- The current primary path is `SDIO`
- The device runs `Wi-Fi backend (fhost / wl80211) + lwIP`
- `nethub` bridges device-side Wi-Fi capabilities to the host
- The control channel and `USER virtual channel` both run on the same host link

Active directories:

- device side
  - `components/net/nethub`
  - `examples/wifi/nethub`
- host side
  - `bsp/common/msg_router/linux_host/kernel`
  - `bsp/common/msg_router/linux_host/userspace/nethub`

## 1. Confirm Two Choices Before Bring-Up

### 1.1 Choose the Wi-Fi Backend

Default configuration:

- `CONFIG_WL80211` is disabled
- `fhost` is used

If you want to switch to `wl80211`, enable this option in
`examples/wifi/nethub/defconfig`:

```make
CONFIG_WL80211=y
```

Notes:

- `fhost` and `wl80211` are mutually exclusive
- The current `nethub` implementation supports both device-side Wi-Fi backends

### 1.2 Choose the Control Channel

device and host settings must match:

| device configuration | host startup |
| --- | --- |
| `CONFIG_NETHUB_AT_USE_VCHAN=n` | `./output/bflbwifid` or `./output/bflbwifid -c tty -p /dev/ttyAT0` |
| `CONFIG_NETHUB_AT_USE_VCHAN=y` | `./output/bflbwifid -c vchan` |

The default configuration is:

```make
CONFIG_NETHUB_AT_USE_VCHAN=n
```

That means the control channel uses `TTY` by default.

Additional notes:

- The host side always builds both `tty + vchan`
- The actual selection happens at `bflbwifid` runtime

## 2. Build and Flash the Device Firmware

```bash
cd examples/wifi/nethub

# BL616
make CHIP=bl616 BOARD=bl616dk

# BL616CL
make CHIP=bl616cl BOARD=bl616cldk

# BL618DG
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

Flash example:

```bash
cd examples/wifi/nethub
make flash CHIP=bl618dg COMX=/dev/ttyUSB0
```

The most relevant bring-up configuration is located in:

- `examples/wifi/nethub/defconfig`

Common options:

- `CONFIG_NETHUB=y`
- `CONFIG_MR_NETDEV=y`
- `CONFIG_MR_TTY=y`
- `CONFIG_MR_VIRTUALCHAN=y`
- `CONFIG_NETHUB_AT_USE_VCHAN=n`

## 3. Build and Start the Host Software

### 3.1 Build

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

### 3.2 Load the Kernel Module

```bash
cd bsp/common/msg_router/linux_host/userspace/nethub
sudo ./build.sh load
```

To unload:

```bash
sudo ./build.sh unload
```

### 3.3 Start the Host Daemon

Default TTY mode:

```bash
sudo ./output/bflbwifid
```

Explicit TTY mode:

```bash
sudo ./output/bflbwifid -c tty -p /dev/ttyAT0
```

VCHAN mode:

```bash
sudo ./output/bflbwifid -c vchan
```

## 4. Basic Verification Flow

Recommended sequence:

### 4.1 Check the Current Status

```bash
sudo ./output/bflbwifictrl status
```

### 4.2 Scan APs

```bash
sudo ./output/bflbwifictrl scan
```

### 4.3 Connect to an AP

```bash
sudo ./output/bflbwifictrl connect_ap "<ssid>" "<password>"
```

### 4.4 Check Status Again

```bash
sudo ./output/bflbwifictrl status
```

If the connection succeeds, you will usually see:

- the backend is connected
- the Wi-Fi state is `CONNECTED` or `GOTIP`
- `IP / Gateway / Netmask / DNS` after an address is assigned

### 4.5 Disconnect

```bash
sudo ./output/bflbwifictrl disconnect
```

## 5. Common Commands

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

## 6. SDIO Hardware Information

For the current customer bring-up path, use `SDIO` directly.
`USB / SPI` still keep architecture skeletons for now and are not the
recommended bring-up path.

### 6.1 Chip Support Matrix

| Interface | BL616 / BL618 | BL618DG | BL616CL |
| --- | --- | --- | --- |
| SDIO | Supported | Supported | Supported |
| USB | Planned | Planned | Planned |
| SPI | Planned | Planned | Planned |

### 6.2 SDIO Pins

| Pin Function | BL616 / BL618 | BL618DG | BL616CL |
| --- | --- | --- | --- |
| SDIO_DAT2 | GPIO10 | GPIO43 | GPIO6 |
| SDIO_DAT3 | GPIO11 | GPIO44 | GPIO7 |
| SDIO_CMD | GPIO12 | GPIO45 | GPIO8 |
| SDIO_CLK | GPIO13 | GPIO46 | GPIO9 |
| SDIO_DATA0 | GPIO14 | GPIO47 | GPIO10 |
| SDIO_DATA1 | GPIO15 | GPIO48 | GPIO11 |

## 7. USER Virtual Channel

If you need to transfer private messages between the host and device, use the
`USER virtual channel`.

Notes:

- The `USER` channel and the control channel are parallel logical channels
- It is not a separate physical interface; it is a logical channel carried on
  the current host link

### 7.1 device-side APIs

Header:

- `components/net/nethub/include/nethub_vchan.h`

Common APIs:

```c
int nethub_vchan_user_send(const void *data, uint16_t len);
int nethub_vchan_user_recv_register(nethub_vchan_recv_cb_t recv_cb, void *cb_arg);
```

### 7.2 host-side APIs

Header:

- `bsp/common/msg_router/linux_host/userspace/nethub/virtualchan/nethub_vchan.h`

Common APIs:

```c
int nethub_vchan_init(void);
int nethub_vchan_deinit(void);

int nethub_vchan_user_send(const void *data, size_t len);
int nethub_vchan_user_register_callback(nethub_vchan_recv_callback_t callback);

int nethub_vchan_get_state_snapshot(nethub_vchan_state_snapshot_t *snapshot);
```

Typical host-side usage order:

1. `nethub_vchan_init()`
2. `nethub_vchan_user_register_callback()`
3. `nethub_vchan_user_send()`
4. Call `nethub_vchan_deinit()` when finished

If you need to check whether the link is ready, you can optionally query:

- `link_state == NETHUB_VCHAN_LINK_UP`
  - the link is ready for TX/RX
- `host_state == NETHUB_VCHAN_HOST_STATE_DEVICE_RUN`
  - the host has completed the handshake with the device

## 8. Common Issues

### 8.1 `bflbwifid` Fails to Start

Check first:

- whether the device and host control backends match
- whether `/dev/ttyAT0` exists
- whether `mr_sdio.ko` has been loaded successfully
- whether the SDIO hardware link is working properly

### 8.2 `connect_ap` Succeeds but the Host Has No Network Access

Check first:

- whether `status` has reached `GOTIP`
- whether `mr_eth0` has an address
- whether services such as `NetworkManager` or `dhcpcd` override the NetHub
  configuration

Notes:

- `build.sh` builds userspace with `ENABLE_NETIF_AUTO_CONFIG=1` by default
- After the device reports `GOTIP`, the host automatically configures the IP,
  gateway, and DNS of `mr_eth0`

### 8.3 `build.sh unload` Reports That the Module Is Busy

Check first:

- whether `bflbwifid` is still running
- whether `nethub_vchan_app` is still running
