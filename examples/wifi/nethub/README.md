# NetHub Documentation Entry

This directory is the example entry point for NetHub.

If you only need the fastest device build and flash flow, this README is
enough. For architecture, host bring-up, or USER virtual channel details,
continue with the documents under `docs/`.

## Current Support Snapshot

| Item | Current status |
| --- | --- |
| Default interface | `SDIO` |
| USB interface | device-side backend implemented with `ECM + ACM` |
| SPI interface | not implemented |
| USER virtual channel | default interface is `SDIO` |
| Optional AT control solution | available through example composition, not a core NetHub dependency |
| Low power | see `docs/NetHub.md`, "Current Support Matrix" |

Important notes:

- `nethub` core does not directly depend on `ATModule`.
- If you only need the data-path, you can ignore the AT control solution.
- The current host Linux stack under
  `bsp/common/msg_router/linux_host/userspace/nethub` follows the default
  `SDIO` interface.

## Device Build

```bash
cd examples/wifi/nethub

# BL616
make CHIP=bl616 BOARD=bl616dk

# BL616CL
make CHIP=bl616cl BOARD=bl616cldk

# BL618DG
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

## Device Flash

```bash
cd examples/wifi/nethub
make flash CHIP=bl618dg COMX=/dev/ttyUSB0
```

## Key Configuration

Configuration entry:

- `examples/wifi/nethub/defconfig`

Important options:

- `CONFIG_NETHUB=y`
- `CONFIG_NETHUB_PROFILE_SDIO=y`
- `CONFIG_NETHUB_PROFILE_USB=y`
- `CONFIG_NETHUB_PROFILE_SPI=y`
  - config symbol names use `PROFILE`, but choose exactly one interface per build
- `CONFIG_WL80211=y`
  - use the `wl80211` Wi-Fi backend
- `CONFIG_WL80211` unset
  - use the default `fhost` Wi-Fi backend
- `CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE=y`
  - enable the optional example AT control solution
- `CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE=n`
  - disable the optional AT control solution; `nethub_ctrl_*` returns
    unsupported and the host must provide any required private control flow
- `CONFIG_MR_VIRTUALCHAN=y`
  - required for the current in-tree SDIO USER virtual channel path
- `CONFIG_NETHUB_LOWPOWER_ENABLE=y`
  - enabled by default for `BL616`; `BL616CL` and `BL618DG` keep it off by default in the NetHub example

About `CONFIG_NETHUB_AT_USE_VCHAN`:

- it still exists in the example configuration
- treat it as an example or legacy SDIO control path convention
- do not treat it as the selector that makes all backends share one
  finished control implementation

Current default example values include:

- `CONFIG_NETHUB_PROFILE_SDIO=y`
- `CONFIG_NETHUB_PROFILE_USB=n`
- `CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE=y`
- `CONFIG_MR_VIRTUALCHAN=y`
- `CONFIG_NETHUB_AT_USE_VCHAN=n`
- `CONFIG_MR_TTY=n`

## Documentation Index

In-repo Markdown documents:

- `docs/NetHub.md`
- `docs/NetHubQuickBringup.md`
- `docs/NetHubArchitecture.md`
- `docs/NetHubVirtualChannel.md`

Online wiki:

- <https://docs.bouffalolab.com/index.php?title=NetHub>
- <https://docs.bouffalolab.com/index.php?title=NetHubQuickBringup>
- <https://docs.bouffalolab.com/index.php?title=NetHubArchitecture>
- <https://docs.bouffalolab.com/index.php?title=NetHubVirtualChannel>

Recommended reading order:

1. `docs/NetHubQuickBringup.md`
2. `docs/NetHub.md`
3. `docs/NetHubArchitecture.md`
4. `docs/NetHubVirtualChannel.md`
