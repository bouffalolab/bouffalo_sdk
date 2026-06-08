# NetHub Architecture

NetHub runs on the device and bridges the device-side Wi-Fi function to one
selected host interface.

![NetHub Architecture Overview](https://docs.bouffalolab.com/images/d/d1/Nethub_arch.gif)

```text
Host
  |
  | SDIO or USB
  v
NetHub host interface backend
  |
  v
NetHub core
  |
  v
Wi-Fi backend + lwIP
```

## 1. Interface Model

NetHub uses build-time interface selection. Select one interface in one firmware
build.

| Interface | Network data path | Control channel | USER virtual channel |
| --- | --- | --- | --- |
| `SDIO` | Supported | Supported | Supported |
| `USB` | Supported through ECM | Supported through ACM | TODO |
| `SPI` | TODO | TODO | TODO |

For per-chip status and low-power support, see
[NetHub.md](NetHub.md), "Current Support Matrix".

## 2. Main Blocks

| Block | Role |
| --- | --- |
| NetHub core | lifecycle, active Wi-Fi endpoint, data routing, control facade, virtual channel facade |
| Host interface backend | SDIO or USB transport implementation |
| Wi-Fi backend | device-side Wi-Fi connection and packet handling |
| Optional control module | customer or example control flow carried over the selected interface |

## 3. Data Path

The data path forwards packets between the host interface and the active Wi-Fi
endpoint.

```text
host packet -> NetHub -> Wi-Fi
Wi-Fi packet -> NetHub -> host
```

The default Wi-Fi RX policy handles common local packets on the device and
forwards host traffic as required by the NetHub profile.

## 4. Control Path

NetHub provides a control facade for software that needs a control channel
between host and device.

The example configuration can use an AT-based control channel:

| Setting | Behavior |
| --- | --- |
| `CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE=y` | enable the example AT control channel |
| `CONFIG_NETHUB_CTRLCHANNEL_USE_ATMODULE=n` | build without the example AT control channel |

When the example AT control channel is disabled, the NetHub data path still
exists. Product software must provide any required private control flow.

## 5. Virtual Channel

Virtual channels carry message-style payloads over the active host interface.

| Type | Purpose |
| --- | --- |
| `AT` | control payloads |
| `USER` | customer application messages |

See [NetHubVirtualChannel.md](NetHubVirtualChannel.md) for USER channel scope
and APIs.

## 6. Low Power

Low-power support is platform-limited.

| Chip | NetHub low power |
| --- | --- |
| `BL616` | Supported |
| `BL616CL` | TODO |
| `BL618DG` | TODO |

Keep `CONFIG_NETHUB_LOWPOWER_ENABLE=n` for platforms marked `TODO`.
