# NetHub USER Virtual Channel

The USER virtual channel is a logical message channel between host software and
device software. It is carried by the selected NetHub host interface.

## 1. Current Scope

| Interface | USER virtual channel status |
| --- | --- |
| `SDIO` | Supported |
| `USB` | TODO for customer end-to-end USER channel |
| `SPI` | TODO |

Use the USER virtual channel for private application messages. Do not use it for
network packets.

## 2. Channel Types

| Type | Purpose |
| --- | --- |
| `USER` | customer application data |
| `AT` | control payloads |

## 3. Device API

Header:

```c
#include "nethub_vchan.h"
```

Common device-side APIs:

```c
int nethub_vchan_user_send(const void *data, uint16_t len);
int nethub_vchan_user_recv_register(nethub_vchan_recv_cb_t recv_cb, void *cb_arg);
```

## 4. Host API

Common host-side APIs:

```c
int nethub_vchan_init(void);
int nethub_vchan_deinit(void);
int nethub_vchan_user_send(const void *data, size_t len);
int nethub_vchan_user_register_callback(nethub_vchan_recv_callback_t callback);
```

Typical order:

1. initialize the virtual channel library
2. register the USER receive callback
3. send or receive USER messages
4. deinitialize the library when finished

## 5. Limits

- The maximum USER payload length is `1500` bytes.
- USER virtual channel messages are packet-oriented.
- Keep customer application traffic on `USER`; keep control traffic on `AT`.

## 6. Related Pages

- [NetHub.md](NetHub.md)
- [NetHubQuickBringup.md](NetHubQuickBringup.md)
- [NetHubArchitecture.md](NetHubArchitecture.md)
