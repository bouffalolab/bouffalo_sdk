# NetHub Device Module Guide

This document explains the role and entry points of the device-side module in
`components/net/nethub`.

If this is your first NetHub bring-up, start with:

- `examples/wifi/nethub/README.md`
- `examples/wifi/nethub/docs/NetHubQuickBringup.md`

Online wiki:

- <https://docs.bouffalolab.com/index.php?title=NetHub>
- <https://docs.bouffalolab.com/index.php?title=NetHubQuickBringup>

## 1. Module Role

`components/net/nethub` is the active device-side NetHub implementation.

Its responsibilities are:

- bridge device-side Wi-Fi data onto the current host link
- split Wi-Fi RX packets between local handling and host forwarding
- provide a logical control channel on the same host link
- provide an optional USER virtual channel

It does not handle:

- the Wi-Fi protocol stack itself
- the host-side daemon or CLI
- product-level arbitration for multiple host links running at the same time

## 2. Current Architecture

The current data and control model can be summarized as:

```text
Wi-Fi + lwIP
    |
    v
  nethub
    |- data bridge
    |- packet filter
    |- ctrlpath
    |- virtual channel
    |
    v
Host Link (SDIO / USB / SPI)
```

The current primary path is `SDIO`.

The control plane and USER channel are not separate physical interfaces. They
are logical channels carried on the current host link.

## 3. Public Headers

The stable public headers are:

- `include/nethub.h`
- `include/nethub_vchan.h`
- `include/nethub_filter.h`

Additional notes:

- Most integrations only need `include/nethub.h`.
- `include/nethub_vchan.h`
  - Include this directly only if you need the host-link logical virtual
    channel API.
- `include/nethub_filter.h`
  - Include this directly only if you need to fully replace the built-in Wi-Fi
    RX filter.
- `include/nethub_defs.h`
  - Include this directly only when you need shared public types. Most
    application code does not need it separately.

### 3.1 Core APIs

`nethub.h`:

- `nethub_bootstrap()`
- `nethub_shutdown()`
- `nethub_get_status()`
- `nethub_set_active_wifi_channel()`
- `nethub_ctrl_upld_send()`
- `nethub_ctrl_dnld_register()`

`nethub_vchan.h`:

- `nethub_vchan_user_send()`
- `nethub_vchan_user_recv_register()`
- `nethub_vchan_at_send()`
- `nethub_vchan_at_recv_register()`

`nethub_filter.h`:

- `nethub_set_wifi_rx_filter()`

Notes:

- `nethub_ctrl_upld_send()` / `nethub_ctrl_dnld_register()`
  - map to the logical control channel on the host link
- `nethub_vchan_user_*`
  - map to the logical USER channel on the host link
- `nethub_set_wifi_rx_filter()`
  - replaces the built-in Wi-Fi RX filter
  - must be called before `nethub_bootstrap()`

## 4. Directory Overview

- `include/`
  - public headers
- `core/`
  - hub lifecycle, endpoint, filter, runtime, and ctrlpath
- `profile/`
  - fixed topology selection for the current build configuration
- `bootstrap/`
  - module bootstrap entry
- `backend/wifi/`
  - Wi-Fi backend adaptation
- `backend/host/sdio/`
  - backend for the current primary path
- `backend/host/usb/`
  - USB skeleton
- `backend/host/spi/`
  - SPI skeleton
- `diag/`
  - internal diagnostics

## 5. Key Configuration

The most relevant NetHub bring-up configuration is usually in:

- `examples/wifi/nethub/defconfig`

Common options:

- `CONFIG_NETHUB=y`
- `CONFIG_MR_NETDEV=y`
- `CONFIG_MR_TTY=y`
- `CONFIG_MR_VIRTUALCHAN=y`
- `CONFIG_NETHUB_AT_USE_VCHAN=n`

Wi-Fi backend selection:

- Enable `CONFIG_WL80211=y`
  - use `wl80211`
- Disable `CONFIG_WL80211`
  - default to `fhost`

Control channel selection:

- `CONFIG_NETHUB_AT_USE_VCHAN=n`
  - the control plane uses `TTY`
- `CONFIG_NETHUB_AT_USE_VCHAN=y`
  - the control plane uses `AT virtual channel`

## 6. Initialization Entry

In the current example project, NetHub is initialized in:

- `examples/wifi/nethub/app/src/app_user.c`

The main sequence is:

1. RF parameter initialization
2. `tcpip_init()`
3. `easyflash_init()`
4. `nethub_bootstrap()`
5. `app_wifi_init()`

If `CONFIG_MR_VIRTUALCHAN` is enabled, the example also initializes the USER
virtual channel demo code.

## 7. Related Documents

- `examples/wifi/nethub/README.md`
- `examples/wifi/nethub/docs/NetHub.md`
- `examples/wifi/nethub/docs/NetHubQuickBringup.md`
- `components/net/nethub/ARCHITECTURE.md`
- `bsp/common/msg_router/linux_host/userspace/nethub/README.md`
