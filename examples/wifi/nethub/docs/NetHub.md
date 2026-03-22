# NetHub

NetHub bridges device-side Wi-Fi capabilities to the host.

The current solution targets Wi-Fi SoCs such as `BL616 / BL618 / BL616CL / BL618DG`,
and the recommended bring-up path is currently `SDIO`.

## Documentation Guide

- [NetHubQuickBringup.md](NetHubQuickBringup.md)
  - For customers doing first-time bring-up
  - Covers key device/host choices, build steps, module loading, AP connection,
    and common issues
- [NetHubArchitecture.md](NetHubArchitecture.md)
  - For developers who need to understand module boundaries and APIs
  - Covers device/host layering, public interfaces, and key data flows
- [NetHubVirtualChannel.md](NetHubVirtualChannel.md)
  - For customers or application developers who need to transfer private data
  - Covers USER channel prerequisites, APIs, and typical usage

## Current Conclusions

- The current primary path is `SDIO`
- The device-side Wi-Fi backend supports `fhost / wl80211`
- The host control backend is selected at runtime: `tty` or `vchan`
- The control channel and `USER virtual channel` both run on the same host link

## In-Repo Reference Directories

- `examples/wifi/nethub`
- `components/net/nethub`
- `bsp/common/msg_router/linux_host/userspace/nethub`

## Local Documentation Source

- Markdown documents:
  - `examples/wifi/nethub/docs/`

If you need to publish to the online wiki, use these four Markdown documents
as the source.

## Online Wiki

- [NetHub](https://docs.bouffalolab.com/index.php?title=NetHub)
- [NetHubQuickBringup](https://docs.bouffalolab.com/index.php?title=NetHubQuickBringup)
- [NetHubArchitecture](https://docs.bouffalolab.com/index.php?title=NetHubArchitecture)
- [NetHubVirtualChannel](https://docs.bouffalolab.com/index.php?title=NetHubVirtualChannel)
