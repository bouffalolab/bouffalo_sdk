# NetHub Architecture

This document summarizes the current NetHub architecture across the device-side module, the optional host control stack, and the current support boundaries.

## 1. High-Level Model

NetHub runs on the device and bridges already-available Wi-Fi capability onto one selected host interface.

![NetHub Architecture Overview](https://docs.bouffalolab.com/images/d/d1/Nethub_arch.gif)

```text
Wi-Fi backend + lwIP
       |
       v
     nethub
  +-----------+
  | data-path |
  | filter    |
  | ctrlpath  |
  | vchan     |
  +-----------+
       |
       v
host interface
(SDIO / USB / SPI)
```

Important boundary:

- `nethub` core does not directly depend on `ATModule`
- `ATModule` is an optional example consumer of the NetHub control path

## 2. Current Support Matrix

| Area | SDIO | USB | SPI |
| --- | --- | --- | --- |
| Device-side interface backend | yes | yes | yes |
| Device-side data-path | yes | yes, through `USB ECM` | no |
| Device-side control transport plumbing | yes | yes, through `USB ACM` | no |
| Current in-tree host Linux stack | yes | not yet the main host path | no |
| End-to-end USER virtual channel | yes | not yet aligned in-tree | no |
| Default interface today | yes | no | no |

## 3. Device-Side Layers

Public headers:

- `components/net/nethub/include/nethub.h`
- `components/net/nethub/include/nethub_vchan.h`
- `components/net/nethub/include/nethub_filter.h`

Internal layers:

- `bootstrap/`
- `core/`
- `profile/`
- `backend/wifi/`
- `backend/host/sdio/`
- `backend/host/usb/`
- `backend/host/spi/`

Main public APIs:

- `nethub_bootstrap()`
- `nethub_shutdown()`
- `nethub_get_status()`
- `nethub_set_active_wifi_channel()`
- `nethub_ctrl_upld_send()`
- `nethub_ctrl_dnld_register()`
- `nethub_vchan_user_send()`
- `nethub_vchan_user_recv_register()`
- `nethub_set_wifi_rx_filter()`

## 4. Data-Path

The data-path is the main NetHub function.

```text
host payload -> host interface backend -> nethub -> active Wi-Fi endpoint
Wi-Fi RX     -> nethub filter     -> local / host / both
```

Device-side Wi-Fi is already owned by the Wi-Fi backend. NetHub mainly wires the selected host interface to that Wi-Fi backend.

## 5. Control Path

NetHub exposes a stable control path facade:

```c
int nethub_ctrl_upld_send(uint8_t *data_buff, uint32_t data_size);
int nethub_ctrl_dnld_register(nethub_ctrl_rx_cb_t dnld_cb, void *cbpri_arg);
```

That facade is implemented per interface.

Current interpretation:

- `SDIO`: implemented in-tree
- `USB`: device-side ACM transport plumbing implemented
- `SPI`: not implemented

Main build-time switches related to this area:

- `CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE=y`
  - the example enables `ATModule` plus host `bflbwifid / bflbwifictrl`
- `CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE=n`
  - `ATModule` does not participate in the build

`CONFIG_NETHUB_AT_USE_VCHAN` should be documented carefully as an example or legacy SDIO-specific control path convention, not as a finished transport-wide selector.

## 6. Virtual Channel

NetHub also exposes a logical virtual channel layer for message-style traffic such as `USER`, `AT`, and internal coordination payloads.

Architecture role:

- device side uses `components/net/nethub/include/nethub_vchan.h`
- host side uses `bsp/common/msg_router/linux_host/userspace/nethub/virtualchan/nethub_vchan.h`
- the API is intentionally transport-neutral across host interfaces

Current implementation boundary:

- the current in-tree end-to-end USER virtual channel path follows the default `SDIO` interface
- the device-side implementation is still centered on the SDIO backend
- the host userspace `nethub_vchan` library is also SDIO-based today
- USB device-side ACM transport exists, but the host-side wrapper is not yet aligned as a finished end-to-end virtual channel path

For prerequisites, detailed APIs, usage order, and limits, read [NetHubVirtualChannel.md](NetHubVirtualChannel.md).

## 7. Host-Side Optional Stack

When the optional AT control solution is used, the typical host-side stack is:

```text
bflbwifictrl -> bflbwifid -> libbflbwifi -> tty or vchan backend
```

Notes:

- this stack is not mandatory for NetHub data-path-only products
- the in-tree host Linux stack is still centered on the SDIO path

## 8. Low-Power Scope

Current low-power support is `BL618DG` only.

The example configuration automatically disables `CONFIG_NETHUB_LOWPOWER_ENABLE` on other chips.

## 9. Recommended Reading

- [NetHub.md](NetHub.md)
- [NetHubQuickBringup.md](NetHubQuickBringup.md)
- [NetHubVirtualChannel.md](NetHubVirtualChannel.md)
