# NetHub Device Module Guide

This document explains the current device-side NetHub module in
`components/net/nethub`.

If you are doing first-time bring-up, start with:

- `examples/wifi/nethub/README.md`
- `examples/wifi/nethub/docs/NetHubQuickBringup.md`

Online wiki:

- <https://docs.bouffalolab.com/index.php?title=NetHub>
- <https://docs.bouffalolab.com/index.php?title=NetHubQuickBringup>

## 1. Module Role

`components/net/nethub` is the active device-side NetHub implementation.

Its responsibilities are:

- bridge device-side Wi-Fi traffic onto one selected host interface
- split Wi-Fi RX packets between local handling and host forwarding
- keep the currently active Wi-Fi endpoint selection, `STA` or `AP`
- expose a logical control path facade through `nethub_ctrl_*`
- expose a logical virtual channel API through `nethub_vchan_*`

It does not do the following:

- implement the Wi-Fi protocol stack itself
- require `ATModule` as a hard dependency
- run multiple host interfaces at the same time in one build

The important architectural boundary is:

- `nethub` core does not directly depend on `ATModule`
- `ATModule` is only one optional control path composition used by the
  example project
- data-only products may keep NetHub and drop the AT control solution

## 2. Current Support Matrix

Only one `CONFIG_NETHUB_PROFILE_*` interface may be selected in one build.

| Interface | Device-side data path | Device-side control facade | USER virtual channel | Current status |
| --- | --- | --- | --- | --- |
| `SDIO` | implemented | implemented through AT virtual channel | implemented end-to-end in-tree | current default interface |
| `USB` | implemented through `USB ECM` | implemented through AT virtual channel over `USB ACM` | device-side ACM vchan path exists, but in-tree host `nethub_vchan` is not yet aligned with USB | device-side backend available |
| `SPI` | not implemented | not implemented | not implemented | interface stub only |

Notes:

- `SDIO` is still the default bring-up interface today.
- `USB` is no longer just a skeleton on the device side.
- `SPI` currently returns `NETHUB_ERR_NOT_SUPPORTED` in the transport
  backend and should be documented as unsupported.

## 3. Public Headers and APIs

The public headers are:

- `include/nethub.h`
- `include/nethub_vchan.h`
- `include/nethub_filter.h`
- `include/nethub_defs.h`

Typical integrations only need `include/nethub.h`.

`nethub.h` exports the main facade:

- `nethub_bootstrap()`
- `nethub_shutdown()`
- `nethub_get_status()`
- `nethub_set_active_wifi_channel()`
- `nethub_ctrl_upld_send()`
- `nethub_ctrl_dnld_register()`

`nethub_vchan.h` exports the logical virtual channel API:

- `nethub_vchan_send()`
- `nethub_vchan_recv_register()`
- `nethub_vchan_user_send()`
- `nethub_vchan_user_recv_register()`
- `nethub_vchan_at_send()`
- `nethub_vchan_at_recv_register()`

`nethub_filter.h` exports:

- `nethub_set_wifi_rx_filter()`

Notes:

- `nethub_ctrl_*` is the stable control path facade layered on
  `NETHUB_VCHAN_TYPE_AT`.
- `nethub_vchan.h` is designed as a transport-neutral API surface. SDIO
  and USB provide device-side vchan backends; the current in-tree
  end-to-end USER virtual channel flow is still SDIO-centered because the
  host userspace wrapper is not yet aligned with USB ACM.
- `nethub_set_wifi_rx_filter()` replaces the built-in Wi-Fi RX policy and
  must be called before `nethub_bootstrap()`.

## 4. Directory Overview

- `bootstrap/`
  - bring-up and shutdown entry points
- `core/`
  - hub lifecycle, endpoint registration, runtime state, filter, and
    control path facade
- `profile/`
  - build-time interface selection and built-in policy wiring
- `backend/wifi/`
  - Wi-Fi bridge and Wi-Fi backend adapters
- `backend/host/sdio/`
  - current in-tree default backend, including netdev and virtual
    channel integration
- `backend/host/usb/`
  - device-side USB backend, including ECM data transport and ACM-backed
    virtual channel plumbing
- `backend/host/spi/`
  - interface stub, not implemented yet
- `diag/`
  - optional diagnostics and shell helpers

## 5. Key Configuration

The main bring-up configuration lives in:

- `examples/wifi/nethub/defconfig`

Important options:

- `CONFIG_NETHUB=y`
  - enable NetHub
- `CONFIG_NETHUB_PROFILE_SDIO=y`
- `CONFIG_NETHUB_PROFILE_USB=y`
- `CONFIG_NETHUB_PROFILE_SPI=y`
  - config symbol names use `PROFILE`, but choose exactly one host interface per build
- `CONFIG_WL80211=y`
  - use the `wl80211` Wi-Fi backend
- `CONFIG_WL80211` unset
  - use the default `fhost` Wi-Fi backend
- `CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE=y`
  - example composition enables `ATModule` plus host `bflbwifid /
    bflbwifictrl`
- `CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE=n`
  - example can be reduced to a data-path-focused build; `nethub_ctrl_*`
    returns `NETHUB_ERR_NOT_SUPPORTED`, while the data path and
    `nethub_vchan_*` remain available
- `CONFIG_MR_VIRTUALCHAN=y`
  - required for the current in-tree SDIO virtual channel path
- `CONFIG_NETHUB_LOWPOWER_ENABLE=y`
  - currently enabled by default for `BL616`; `BL616CL` and `BL618DG`
    keep it off by default in the NetHub example

About `CONFIG_NETHUB_AT_USE_VCHAN`:

- it still exists in the example configuration
- treat it as an example or legacy control path convention for the SDIO
  host-control flow
- do not document it as the core selector that makes all backends share
  one finished control implementation

Current default example facts in `examples/wifi/nethub/defconfig`:

- `CONFIG_NETHUB_PROFILE_SDIO=y`
- `CONFIG_NETHUB_PROFILE_USB=n`
- `CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE=y`
- `CONFIG_MR_VIRTUALCHAN=y`
- `CONFIG_NETHUB_AT_USE_VCHAN=n`
- `CONFIG_MR_TTY=n`

## 6. Initialization Flow

In the example project, NetHub is initialized from:

- `examples/wifi/nethub/app/src/app_user.c`

The usual sequence is:

1. RF parameter initialization
2. `tcpip_init()`
3. `easyflash_init()`
4. `nethub_bootstrap()`
5. `app_wifi_init()`

If `CONFIG_MR_VIRTUALCHAN` is enabled, the example also builds the local
USER virtual channel demo under `examples/wifi/nethub/app/src/virtualchan/`.

## 7. Low-Power Scope

Low power is not a generic NetHub capability across all chips today.

Current status:

- `BL616`: low-power flow is supported by the NetHub example
- `BL616CL` and `BL618DG`: the example configuration auto-disables
  `CONFIG_NETHUB_LOWPOWER_ENABLE`

For the per-chip and per-interface validation state, see
`examples/wifi/nethub/docs/NetHub.md`, "Current Support Matrix".

## 8. Related Documents

- `components/net/nethub/ARCHITECTURE.md`
- `examples/wifi/nethub/README.md`
- `examples/wifi/nethub/docs/NetHub.md`
- `examples/wifi/nethub/docs/NetHubQuickBringup.md`
- `examples/wifi/nethub/docs/NetHubArchitecture.md`
- `examples/wifi/nethub/docs/NetHubVirtualChannel.md`
- `bsp/common/msg_router/linux_host/userspace/nethub/README.md`
