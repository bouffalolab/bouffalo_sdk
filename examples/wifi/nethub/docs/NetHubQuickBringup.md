# NetHub Quick Start

This document is for customers using NetHub for the first time. The goal is to get you to the current default interface as quickly as possible.

![NetHub SDIO Quick Start Demo](https://docs.bouffalolab.com/images/d/d4/Nethub_sdio_quickstart_demo.gif)

## 1. Current Support Matrix

| Item | Current status |
| --- | --- |
| Default interface | `SDIO` |
| USB interface | device-side backend implemented with `USB ECM + USB ACM` |
| SPI interface | not implemented |
| USER virtual channel | default interface is `SDIO` |
| Optional AT control solution | available, but not mandatory |
| Low power | currently `BL618DG` only |

Read this table literally:

- if you want the shortest successful path today, choose `SDIO`
- if you are evaluating the USB interface, focus on the device-side backend and USB descriptors first
- do not plan on `SPI` bring-up yet

## 2. Main Configuration Switches

For most users, the main configuration changes are these:

```make
CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE =y
CONFIG_NETHUB_LOWPOWER_ENABLE          =y
CONFIG_NETHUB_PROFILE_USB              =n
CONFIG_NETHUB_PROFILE_SDIO             =y
```

Notes:

- the config symbol names still use `PROFILE`, but they select the active host interface
- set `CONFIG_NETHUB_PROFILE_SDIO=y` for the current default interface
- set `CONFIG_NETHUB_PROFILE_USB=y` only when you want to evaluate the USB interface
- `CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE=n` means `ATModule` will not participate in the build
- `CONFIG_NETHUB_LOWPOWER_ENABLE=y` is currently meaningful only on `BL618DG`

Additional current default facts in `examples/wifi/nethub/defconfig`:

- `CONFIG_MR_VIRTUALCHAN=y`
- `CONFIG_NETHUB_AT_USE_VCHAN=n`
- `CONFIG_MR_TTY=n`

## 3. Wi-Fi Backend Selection

Default configuration:

- `CONFIG_WL80211` disabled
- `fhost` selected

If you want `wl80211`, enable this in `examples/wifi/nethub/defconfig`:

```make
CONFIG_WL80211=y
```

Notes:

- `fhost` and `wl80211` are mutually exclusive
- current NetHub supports both device-side Wi-Fi backends

## 4. Device Build and Flash

Build:

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

## 5. Hardware Interface Pins

The pin tables below are the current default mappings used by the in-tree BSP board helpers.

Reference sources in the repository:

- `bsp/board/bl616dk/board_gpio.c`
- `bsp/board/bl616cldk/board_gpio.c`
- `bsp/board/bl618dgdk/board_gpio.c`

### 5.1 Default SDIO Pins

These are the current `board_sdh_gpio_init()` mappings used by the BSP.

| Pin Function | BL616 / BL618 (`bl616dk`) | BL618DG (`bl618dgdk`) | BL616CL (`bl616cldk`) |
| --- | --- | --- | --- |
| `SDIO_DAT2` | `GPIO10` | `GPIO43` | `GPIO6` |
| `SDIO_DAT3` | `GPIO11` | `GPIO44` | `GPIO7` |
| `SDIO_CMD` | `GPIO12` | `GPIO45` | `GPIO8` |
| `SDIO_CLK` | `GPIO13` | `GPIO46` | `GPIO9` |
| `SDIO_DAT0` | `GPIO14` | `GPIO47` | `GPIO10` |
| `SDIO_DAT1` | `GPIO15` | `GPIO48` | `GPIO11` |

Notes:

- this table shows the current default 4-bit SDIO wiring used by the NetHub SDIO data-path
- 1-line mode is also supported; if you use 1-line mode, keep `CMD`, `CLK`, and `DAT0` first
- the BSP configures these pins with `GPIO_FUNC_SDH | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1`

### 5.2 Default USB Pins

The current USB interface uses the board's default USB differential pair.

| Chip / Board family | Default USB pins in current BSP | Current BSP behavior | Notes |
| --- | --- | --- | --- |
| BL616 / BL618 (`bl616dk`) | board default USB port path | current BSP enables USB clock in `board.c`, but does not expose a dedicated `board_usb_gpio_init()` helper | treat this as the board's fixed default USB routing in the current example |
| BL616CL (`bl616cldk`) | `GPIO32`, `GPIO33` | `board_usb_gpio_init()` configures the pair as analog pins | current BSP documents the pair, not separate `DP` / `DM` labels |
| BL618DG (`bl618dgdk`) | `GPIO40`, `GPIO41` | `board_usb_gpio_init()` configures the pair as analog pins | current BSP documents the pair, not separate `DP` / `DM` labels |

Notes:

- for `BL616CL` and `BL618DG`, the BSP helper only tells us the USB pair used by the board, not which pin is `D+` and which pin is `D-`
- if you are wiring a custom board and need explicit `DP` / `DM` polarity, confirm it from the board schematic or chip or package documentation

## 6. Host Bring-Up Guidance

For the current in-tree host Linux stack, use:

- `bsp/common/msg_router/linux_host/userspace/nethub/README.md`

Today that host stack follows the default `SDIO` interface.

For USB projects, keep expectations aligned with current code status:

- device-side `ECM + ACM` backend exists
- host-side transport flattening into the same `nethub_vchan` behavior as SDIO is not fully documented or aligned in-tree yet

## 7. USER Virtual Channel

If you need private application messages between host and device, read:

- [NetHubVirtualChannel.md](NetHubVirtualChannel.md)

Current implementation:

- the current in-tree end-to-end USER virtual channel follows the default `SDIO` interface
- the public device API is transport-neutral by design
- the in-tree implementation behind it is still SDIO-backed today

## 8. Low-Power Note

Low power is currently supported only on `BL618DG`.

For other chips, `examples/wifi/nethub/defconfig` automatically turns `CONFIG_NETHUB_LOWPOWER_ENABLE` off. In practice, you normally keep this option enabled only for `BL618DG` builds.

## 9. Common Questions

### 9.1 `bflbwifid` Fails to Start

This question is relevant only when you use the optional host control stack.

Check first:

- whether the device and host control backends match
- whether `/dev/ttyAT0` exists
- whether `mr_sdio.ko` has been loaded successfully
- whether the SDIO hardware link is working properly

### 9.2 `connect_ap` Succeeds but the Host Has No Network Access

Check first:

- whether `status` has reached `GOTIP`
- whether `mr_eth0` has an address
- whether services such as `NetworkManager` or `dhcpcd` override the NetHub configuration

Notes:

- `build.sh` builds userspace with `ENABLE_NETIF_AUTO_CONFIG=1` by default
- after the device reports `GOTIP`, the host automatically configures the IP, gateway, and DNS of `mr_eth0`

### 9.3 `build.sh unload` Reports That the Module Is Busy

Check first:

- whether `bflbwifid` is still running
- whether `nethub_vchan_app` is still running

### 9.4 How Do I Change the Host-Side Virtual Interface Name `mr_eth0`?

The default host-side virtual network interface name comes from:

- `bsp/common/msg_router/linux_host/kernel/netdev/mr_netdev.c`

The macro is:

```c
#define ETH_DRV_NAME "mr_eth%d"
```

The first created interface therefore appears as `mr_eth0`.

To change the interface name, update `ETH_DRV_NAME` to the pattern you want. For example:

```c
#define ETH_DRV_NAME "nethub%d"
```

Notes:

- keep `%d` if you want Linux to allocate names such as `nethub0`, `nethub1`, and so on
- rebuild the host software after the change and reload the host-side kernel module
- if any local scripts or service configuration still refer to `mr_eth0`, update them accordingly

### 9.5 Can the Host Expose `AP` and `STA` at the Same Time?

No. Due to the current software implementation, this is not supported yet.

Current behavior:

- the host side exposes only one virtual interface at a time
- the default active Wi-Fi channel is `STA`

If you need to switch between `AP` and `STA`, call `nethub_set_active_wifi_channel()` directly:

- use `NETHUB_CHANNEL_WIFI_AP` to switch to `AP`
- use `NETHUB_CHANNEL_WIFI_STA` to switch to `STA`
- the default value is `NETHUB_CHANNEL_WIFI_STA`

### 9.6 What Should the Host Side Handle When `CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE=n`?

When `CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE=n`, the optional AT-based host control solution is not part of the build.

What you need to handle yourself:

- you need your own private control path to carry required messages between host and device
- this includes, but is not limited to, host netdev link-up handling
- this also includes host-side IP address and netmask configuration

In short, NetHub still provides the data-path, but the host-side control workflow becomes your own responsibility.

### 9.7 Why Does `ifconfig` Not Show the Virtual Interface?

Check first:

- use a command such as `ifconfig -a` to list interfaces that are currently down
- find the virtual interface and bring it up manually, for example: `sudo ifconfig mr_eth0 up`

Notes:

- some Linux systems, including newer Raspberry Pi OS releases, run `NetworkManager` by default
- because of that, the exact behavior depends on your host environment and may need host-specific analysis

### 9.8 For Dual-Stack Traffic, Can Users Customize the Filter? Is There Any Difference Between USB and SDIO?

Yes. The default configuration already covers most scenarios, but users can still customize the Wi-Fi RX filter if needed.

Important points:

- the filter is not bound to one interface type; there is no special filter split between `USB` and `SDIO`
- the current built-in policy is shared by both `SDIO` and `USB`
- you can refer to `components/net/nethub/profile/nh_profile_sdio.c` and `components/net/nethub/profile/nh_profile_usb.c`; both use the same built-in Wi-Fi RX policy

Default built-in policy reference:

- `components/net/nethub/profile/nh_profile_builtin.c`
- `NH_FILTER_MATCH_8021X`: handled locally and not delivered to the host
- `NH_FILTER_MATCH_ARP`: handled locally and also delivered to the host
- `NH_FILTER_MATCH_DHCP4`: handled locally
- `NH_FILTER_MATCH_ICMP4`: handled locally
- other packets: delivered to the host by default

If the default policy does not match your requirement:

- you can call `nethub_set_wifi_rx_filter(nethub_wifi_rx_filter_cb_t filter_cb, void *user_ctx)` from `components/net/nethub/include/nethub_filter.h`
- for the callback behavior, refer to `nh_filter_apply_policy()` in `components/net/nethub/core/nh_filter.c`
- if you need a more complex policy, it is also fine to contact us directly

### 9.9 Why Can the Host Not Ping Other Devices on the Same LAN?

This is expected with the current default policy.

Reason:

- `ICMP` packets are handled locally on the device side by default
- they are not delivered to the host by default

So, by default:

- other LAN devices pinging the device can work
- host-side ping visibility to other LAN devices is not part of the default behavior

## 10. Where to Go Next

- overall entry: [NetHub.md](NetHub.md)
- architecture: [NetHubArchitecture.md](NetHubArchitecture.md)
- USER virtual channel: [NetHubVirtualChannel.md](NetHubVirtualChannel.md)
