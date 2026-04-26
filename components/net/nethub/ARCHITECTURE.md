# NetHub Device Architecture Guide

This document describes the current device-side NetHub architecture.

If you are doing first-time bring-up, start with:

- `examples/wifi/nethub/README.md`
- `examples/wifi/nethub/docs/NetHubQuickBringup.md`
- `components/net/nethub/README.md`

Online wiki:

- <https://docs.bouffalolab.com/index.php?title=NetHub>
- <https://docs.bouffalolab.com/index.php?title=NetHubArchitecture>

## 1. Design Goals

`nethub` targets device-side Wi-Fi bridging on Wi-Fi SoCs such as
`BL616 / BL618 / BL616CL / BL618DG`.

The core goals are:

- expose device-side Wi-Fi connectivity through one selected host interface
- keep the data-path independent from the optional AT control solution
- provide a stable public facade for control traffic and virtual channels
- hide per-interface differences behind interface-specific backends

The current architecture is build-time interface selection, not runtime
multi-link arbitration. Exactly one `CONFIG_NETHUB_PROFILE_*` backend is
selected per firmware build.

## 2. Architecture Summary

Functional view:

![NetHub Architecture Overview](https://docs.bouffalolab.com/images/d/d1/Nethub_arch.gif)

```text
Wi-Fi backend (fhost / wl80211) + lwIP
                |
                v
             nethub
      +-------------------+
      | Wi-Fi RX filter   |
      | active Wi-Fi      |
      | ctrlpath facade   |
      | vchan facade      |
      +-------------------+
                |
                v
     host interface (SDIO / USB / SPI)
```

Important boundaries:

- the data-path is the primary function of NetHub
- control traffic is a logical channel carried by the active host interface
- USER virtual channel is also a logical channel carried by the active
  host interface
- `ATModule` is not a mandatory dependency of `nethub`; it is only one
  optional control path composition used by the example project

## 3. Current Support Matrix

| Interface backend | Host interface | Data-path | Control facade | USER virtual channel | Current state |
| --- | --- | --- | --- | --- | --- |
| `nh_profile_sdio.c` | `SDIO` | implemented | implemented | implemented in-tree | current default interface |
| `nh_profile_usb.c` | `USB` | implemented through `USB ECM` | implemented transport plumbing through `USB ACM` | API intent only; in-tree host alignment not finished | device-side backend available |
| `nh_profile_spi.c` | `SPI` | not implemented | not implemented | not implemented | stub only |

Additional notes:

- `SDIO` is the only end-to-end in-tree default interface today.
- `USB` is no longer just an interface placeholder. The device-side backend
  in `backend/host/usb/` provides ECM data transport and ACM control
  transport plumbing.
- `SPI` currently returns `NETHUB_ERR_NOT_SUPPORTED` and should be
  documented as unsupported.

## 4. Public API Surface

Public headers:

- `components/net/nethub/include/nethub.h`
- `components/net/nethub/include/nethub_vchan.h`
- `components/net/nethub/include/nethub_filter.h`
- `components/net/nethub/include/nethub_defs.h`

Main facade in `nethub.h`:

- `nethub_bootstrap()`
- `nethub_shutdown()`
- `nethub_get_status()`
- `nethub_set_active_wifi_channel()`
- `nethub_ctrl_upld_send()`
- `nethub_ctrl_dnld_register()`

Virtual-channel API in `nethub_vchan.h`:

- `nethub_vchan_send()`
- `nethub_vchan_recv_register()`
- `nethub_vchan_user_send()`
- `nethub_vchan_user_recv_register()`
- `nethub_vchan_at_send()`
- `nethub_vchan_at_recv_register()`

Filter override in `nethub_filter.h`:

- `nethub_set_wifi_rx_filter()`

API reality note:

- `nethub_vchan.h` is intentionally transport-neutral as a public API
- the current in-tree implementation behind that API is still SDIO-based
- documentation should therefore distinguish API intent from current
  backend coverage

## 5. Internal Layers

- `bootstrap/`
  - brings the selected interface backend online and shuts it down
- `profile/`
  - resolves the active host interface and built-in Wi-Fi RX policy
- `core/nh_hub.c`
  - lifecycle and runtime status
- `core/nh_runtime.c`
  - active Wi-Fi selection and statistics
- `core/nh_filter.c`
  - built-in Wi-Fi RX filtering policy
- `core/nh_ctrlpath.c`
  - interface-agnostic control path facade
- `backend/wifi/`
  - common Wi-Fi bridge and Wi-Fi backend adapters
- `backend/host/*`
  - per-interface host interface implementations

## 6. Data-Path

The data-path is the most mature part of NetHub.

Conceptually:

```text
host packet -> host interface backend -> nethub -> active Wi-Fi endpoint
Wi-Fi RX    -> nethub filter     -> local / host / both
```

Key points:

- packet routing is not a generic programmable engine; it is a fixed
  topology with a replaceable Wi-Fi RX filter policy
- `nethub_set_active_wifi_channel()` chooses which Wi-Fi endpoint is used
  for host-to-Wi-Fi forwarding
- `nethub_set_wifi_rx_filter()` allows a project to replace the built-in
  RX filter before bootstrap

## 7. Control Path

The control path is intentionally split into two layers:

1. core facade
   - `nethub_ctrl_upld_send()`
   - `nethub_ctrl_dnld_register()`
2. interface-specific implementation
   - selected by `nh_profile_get_ctrlpath_ops()`

This keeps `nethub` decoupled from any specific control consumer.

Current backend reality:

- `SDIO`
  - implemented through the SDIO virtual channel path
- `USB`
  - device-side ACM transport plumbing exists in the USB backend
- `SPI`
  - not implemented

Example composition:

- `CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE=y`
  - enables the example-level `ATModule` control solution
- `CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE=n`
  - keeps NetHub usable as a data-path-focused build

`CONFIG_NETHUB_AT_USE_VCHAN` still appears in the example configuration,
but it should be described as an example or legacy SDIO control path
convention, not as the core selector for all interfaces.

## 8. Virtual Channel

The logical virtual channel API multiplexes payload types such as
`USER`, `AT`, and `SYSTEM` on the active host interface.

Current implementation status:

- device-side implementation lives in
  `components/net/nethub/backend/host/sdio/virtualchan.c`
- host userspace `nethub_vchan` is also aligned with the SDIO msg_router
  and netlink path today
- therefore, end-to-end USER virtual channel is currently an SDIO
  capability on the default interface
- USB ACM is the likely long-term carrier for a USB control or vchan-like
  path, but the in-tree host wrapper is not yet aligned with that path

## 9. USB-Specific Notes

`backend/host/usb/` now contains a real backend rather than a skeleton.

Current USB device-side behavior:

- data-path is exposed through `USB CDC ECM`
- control transport plumbing is exposed through `USB CDC ACM`
- the backend is selected through `CONFIG_NETHUB_PROFILE_USB`

What is not yet finished end-to-end:

- a transport-neutral host-side `nethub_vchan` implementation that hides
  the USB ACM stream details the same way the SDIO path hides netlink

## 10. Low-Power Scope

NetHub low power is currently platform-limited.

In the example configuration:

- `CONFIG_NETHUB_LOWPOWER_ENABLE` is supported only on `BL618DG`
- non-`BL618DG` builds automatically disable it

Do not document low power as universally available across all NetHub
target chips.

## 11. Recommended Documentation Language

When documenting the current codebase, use these statements:

- `nethub` core does not directly depend on `ATModule`
- `SDIO` is the current in-tree default interface
- `USB` device-side backend is implemented with `ECM + ACM`
- `SPI` is not implemented yet
- end-to-end USER virtual channel is currently available on `SDIO`
- low power is currently `BL618DG`-only

## 12. Related Documents

- `components/net/nethub/README.md`
- `examples/wifi/nethub/docs/NetHub.md`
- `examples/wifi/nethub/docs/NetHubQuickBringup.md`
- `examples/wifi/nethub/docs/NetHubArchitecture.md`
- `examples/wifi/nethub/docs/NetHubVirtualChannel.md`
- `bsp/common/msg_router/linux_host/userspace/nethub/HOST_ARCHITECTURE.md`
