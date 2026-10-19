# NetHub

NetHub exposes the device-side Wi-Fi function to an external host through one
selected host interface.

The current customer-facing scope covers `BL616`, `BL616CL`, and `BL618DG` with
`SDIO` or `USB`. Boot-only SDIO/USB dual-profile autodetect is supported on
`BL616` and `BL618DG`.

## Documents

| Document | Purpose |
| --- | --- |
| [NetHubQuickBringup.md](NetHubQuickBringup.md) | build, flash, interface selection, and basic bring-up |
| [NetHubArchitecture.md](NetHubArchitecture.md) | NetHub architecture and host-interface model |
| [NetHubVirtualChannel.md](NetHubVirtualChannel.md) | USER virtual channel scope and APIs |

## Current Support Matrix

Status values:

- `Supported`: available for customer bring-up and validated end to end.
- `Build-only`: firmware builds, but end-to-end hardware validation is still pending.
- `TODO`: not ready for customer use.

| Chip | Interface | NetHub control/data | Low power | Customer guidance |
| --- | --- | --- | --- | --- |
| `BL616` | `SDIO` | Supported | Supported | Recommended default bring-up path. |
| `BL616` | `USB` | Supported | Supported | Use the USB profile when the host connection is USB. |
| `BL616` | `SDIO + USB dual` | Supported | Supported | Use `CONFIG_NETHUB_PROFILE_DUAL=y`; the BL616 SDIO download pool uses 15 frames to preserve WRAM margin. |
| `BL616CL` | `SDIO` | Build-only | TODO | SDIO end-to-end validation is still pending. |
| `BL616CL` | `USB` | Supported | TODO | Keep low power disabled. |
| `BL618DG` | `SDIO` | Supported | TODO | Keep low power disabled. |
| `BL618DG` | `USB` | Supported | TODO | Keep low power disabled. |
| `BL618DG` | `SDIO + USB dual` | Supported | TODO | Use `CONFIG_NETHUB_PROFILE_DUAL=y` for boot-only host autodetect; it locks the first effective host candidate and keeps low power disabled. |

## Configuration Summary

Select one single-host profile, or use the BL616/BL618DG dual profile when one
production firmware must support both SDIO and USB hardware variants.

| Target | Main options |
| --- | --- |
| `SDIO` | `CONFIG_NETHUB_PROFILE_SDIO=y`, `CONFIG_NETHUB_PROFILE_USB=n` |
| `USB` | `CONFIG_NETHUB_PROFILE_USB=y`, `CONFIG_NETHUB_PROFILE_SDIO=n` |
| `BL616 / BL618DG dual` | `CONFIG_NETHUB_PROFILE_DUAL=y`; SDIO and USB are compiled together and the first effective host candidate is locked as active host |
| Low power enabled | `CONFIG_NETHUB_LOWPOWER_ENABLE=y` |
| Low power disabled | `CONFIG_NETHUB_LOWPOWER_ENABLE=n` |

Low-power guidance:

- enable low power only for the `BL616` combinations listed as supported
- keep low power disabled for `BL616CL` and `BL618DG`

The AT-based control channel is optional:

| Setting | Behavior |
| --- | --- |
| `CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE=y` | Use the example AT control channel over the selected interface. |
| `CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE=n` | Build without the example AT control channel. The data path remains available, but product software must provide any required private control flow. |

## Notes

- `SDIO` is the default interface in the example configuration and remains the primary in-tree Host bring-up and USER virtual-channel reference path.
- Dual profile is optional on BL616/BL618DG and must be enabled explicitly with `CONFIG_NETHUB_PROFILE_DUAL=y`.
- `USB` uses ECM for the network data path and ACM for control or message traffic.
- `CONFIG_NETHUB_PROFILE_DUAL=y` is boot-only autodetect: before lock, NetHub keeps probing until the first effective SDIO or USB host candidate arrives; after the host is locked, it does not hot-switch or fail over, and the selected backend follows its single-profile reconnect behavior.
- The dual selector is shared by BL616 and BL618DG. Chip-specific memory and low-power settings remain independent of the selector behavior.
- `SPI` is not supported in the current NetHub customer bring-up flow.
