# NetHub

NetHub bridges device-side Wi-Fi capability to a selected host interface.

The current solution targets Wi-Fi SoCs such as `BL616 / BL618 / BL616CL / BL618DG`.

## Current Status

| Item | Current status |
| --- | --- |
| Default interface | `SDIO` |
| USB data-path | device-side `USB ECM` backend implemented |
| USB control transport | device-side `USB ACM` transport plumbing implemented |
| SPI | not implemented |
| USER virtual channel | default interface is `SDIO` |
| Low power | currently `BL618DG` only |
| Optional ATModule control solution | available, but not mandatory |

## Document Guide

Use these documents according to what you need:

| Document | Focus |
| --- | --- |
| [NetHubQuickBringup.md](NetHubQuickBringup.md) | default interface, main configuration switches, build and flash, SDIO and USB pins |
| [NetHubArchitecture.md](NetHubArchitecture.md) | internal layering, public APIs, support matrix, and interface differences |
| [NetHubVirtualChannel.md](NetHubVirtualChannel.md) | USER virtual channel scope, prerequisites, APIs, and limits |

## Key Points

- `SDIO` is the current default interface for the in-tree bring-up path.
- `USB` device-side backend is implemented with `ECM + ACM`.
- `SPI` is still a stub and is not ready for bring-up.
- `nethub` core exposes data-path, control path, and virtual channel facades without directly depending on `ATModule`.
- The optional `ATModule + bflbwifid + bflbwifictrl` flow is available, but not mandatory.

## In-Repo Reference Directories

- `examples/wifi/nethub`
- `components/net/nethub`
- `bsp/common/msg_router/linux_host/userspace/nethub`

## Local Documentation Source

The in-repo documentation source is:

- `examples/wifi/nethub/docs/`

## Online Wiki

- [NetHub](https://docs.bouffalolab.com/index.php?title=NetHub)
- [NetHubQuickBringup](https://docs.bouffalolab.com/index.php?title=NetHubQuickBringup)
- [NetHubArchitecture](https://docs.bouffalolab.com/index.php?title=NetHubArchitecture)
- [NetHubVirtualChannel](https://docs.bouffalolab.com/index.php?title=NetHubVirtualChannel)
