# NetHub Documentation Entry

This directory is the device-side example entry point for NetHub.

If you only want to complete device build and flashing as quickly as possible,
this README is enough. If you also need to bring up the host side, understand
the architecture, or integrate the USER virtual channel, continue with the
documents under `docs/`.

## Current Conclusions

- The current primary path is `SDIO`.
- The device-side Wi-Fi backend supports `fhost / wl80211`.
- The host control backend is selected at runtime: `tty` or `vchan`.
- The control channel and `USER virtual channel` both run on the same host link.

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

Common options:

- `CONFIG_NETHUB=y`
- `CONFIG_MR_NETDEV=y`
- `CONFIG_MR_TTY=y`
- `CONFIG_MR_VIRTUALCHAN=y`
- `CONFIG_WL80211`
  - Enabled: use `wl80211`
  - Disabled: default to `fhost`
- `CONFIG_NETHUB_AT_USE_VCHAN`
  - `n`: the control channel uses `TTY`
  - `y`: the control channel uses `AT virtual channel`

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

Notes:

- The repository only maintains these Markdown documents.
- If you later want to publish to the online wiki, use these `docs/*.md`
  files as the source.

Recommended reading order:

1. `docs/NetHubQuickBringup.md`
2. `docs/NetHubArchitecture.md`
3. `docs/NetHubVirtualChannel.md`
