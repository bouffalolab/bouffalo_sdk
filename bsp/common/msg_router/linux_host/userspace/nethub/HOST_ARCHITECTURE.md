# NetHub Host Architecture Guide

This document describes the current host-side software architecture under
`bsp/common/msg_router/linux_host/userspace/nethub`.

It focuses on two things:

1. what the current in-tree Linux host stack actually supports
2. how the host-side modules are layered today

If you have not finished basic bring-up yet, read these first:

- `examples/wifi/nethub/README.md`
- `examples/wifi/nethub/docs/NetHubQuickBringup.md`
- `bsp/common/msg_router/linux_host/userspace/nethub/README.md`

## 1. Support Matrix

| Area | SDIO | USB | SPI |
| --- | --- | --- | --- |
| Current in-tree Linux host stack | yes | no | no |
| `mr_sdio.ko` based data path | yes | no | no |
| `bflbwifid / bflbwifictrl` host control stack | yes | not the main in-tree flow | no |
| `nethub_vchan` host library | yes | not aligned in-tree yet | no |

Interpretation:

- this host stack follows the default `SDIO` interface
- USB device-side NetHub support exists, but the in-tree host reference
  documentation and libraries are not yet flattened into the same model
- SPI is not implemented

## 2. Design Goals

The current host stack solves three separate needs:

- control path
  - optional AT-style control through daemon and CLI
- data-path
  - host networking through the kernel netdev path
- user extension channel
  - USER virtual channel messages through `nethub_vchan`

Core principles:

- keep data-path and control path separate
- keep CLI and daemon stable for the optional AT solution
- keep the host virtual channel library separate from the Wi-Fi control
  library

## 3. User View

```text
bflbwifictrl -> bflbwifid -> libbflbwifi -> tty or vchan backend
                                             |
                                             v
                                      mr_sdio.ko + msg_router
                                             |
                                             v
                                        device firmware

Linux network stack <-> mr_eth0 <-> mr_sdio.ko
nethub_vchan_app    <-> libnethub_vchan <-> netlink <-> mr_sdio.ko
```

Notes:

- `tty` and `vchan` are control backends within the current host stack
- `USER` virtual channel is a separate logical service on the same SDIO
  link used by the default interface
- data-only products do not need the full AT control chain

## 4. Main Modules

- `build.sh`
  - build, load, and unload entry point
- `mr_sdio.ko`
  - host-kernel communication base for the SDIO path
- `bflbwifid`
  - control path daemon
- `bflbwifictrl`
  - command-line entry
- `libbflbwifi.a`
  - host Wi-Fi control library
- `libnethub_vchan.a`
  - host USER virtual channel library
- `nethub_vchan_app`
  - optional debug utility

## 5. Control Stack

The AT-style control stack is optional from a NetHub product perspective.
When used, the layering is:

- CLI layer
  - `bflbwifictrl`
- daemon layer
  - `bflbwifid`
- library layer
  - `libbflbwifi`
- backend layer
  - `tty` or `vchan`

Important boundary:

- this stack is not proof that `nethub` core depends on `ATModule`
- `ATModule` is only one optional control path solution that can sit on
  top of the NetHub control facade

`CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE` is therefore best documented as
an example composition switch, not as a NetHub core requirement.

## 6. USER Virtual Channel

The host-side USER virtual channel is exposed through:

- `virtualchan/nethub_vchan.h`

Current implementation:

- the host library is aligned with the SDIO kernel virtual channel path
- the public API is reusable, but the in-tree backend is still SDIO-based
- do not document this library as a finished USB ACM wrapper yet

## 7. Build Notes

Build from the top-level host entry:

```bash
cd bsp/common/msg_router/linux_host/userspace/nethub
./build.sh build
```

This host build keeps both control backends available:

- `tty`
- `vchan`

The actual choice is made when `bflbwifid` starts.

## 8. Recommended Wording

When describing the current repository state, use these statements:

- the in-tree Linux host stack follows the default `SDIO` interface
- USB device-side NetHub support exists, but the host Linux reference
  stack is not yet centered on USB
- SPI is not implemented
- the host USER virtual channel library is currently SDIO-backed
- the AT control stack is optional from NetHub core's perspective

## 9. Related Documents

- `README.md`
- `virtualchan/README.md`
- `bflbwifictrl/README.md`
- `examples/wifi/nethub/docs/NetHubArchitecture.md`
