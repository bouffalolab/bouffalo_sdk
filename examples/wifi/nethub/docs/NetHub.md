# NetHub

NetHub exposes the device-side Wi-Fi function to an external host through one
selected host interface.

The current customer-facing scope covers `BL616`, `BL616CL`, and `BL618DG` with
`SDIO` or `USB`.

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
| `BL616CL` | `SDIO` | Build-only | TODO | SDIO end-to-end validation is still pending. |
| `BL616CL` | `USB` | Supported | TODO | Keep low power disabled. |
| `BL618DG` | `SDIO` | Supported | TODO | Keep low power disabled. |
| `BL618DG` | `USB` | Supported | TODO | Keep low power disabled. |

## Configuration Summary

Select exactly one host interface in one firmware build.

| Target | Main options |
| --- | --- |
| `SDIO` | `CONFIG_NETHUB_PROFILE_SDIO=y`, `CONFIG_NETHUB_PROFILE_USB=n` |
| `USB` | `CONFIG_NETHUB_PROFILE_USB=y`, `CONFIG_NETHUB_PROFILE_SDIO=n` |
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

- `SDIO` is the default interface in the example configuration.
- `USB` uses ECM for the network data path and ACM for control or message traffic.
- `SPI` is not supported in the current NetHub customer bring-up flow.
