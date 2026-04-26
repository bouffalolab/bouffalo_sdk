# NetHub USER Virtual Channel

This document describes the current NetHub USER virtual channel support.

It focuses on USER virtual channel usage, prerequisites, APIs, and limits. For the full NetHub architecture and the broader interface support matrix, read [NetHubArchitecture.md](NetHubArchitecture.md).

## 1. Concept

USER virtual channel is a logical message channel carried on the active host interface.

It is not a separate physical interface. The public API is transport-neutral, while the current in-tree implementation is still centered on the default `SDIO` interface.

Common logical types:

| Type | Meaning |
| --- | --- |
| `USER` | private customer or application data |
| `AT` | control payloads |
| `SYSTEM` | internal coordination messages |

## 2. Current Scope

Current implementation summary:

- the public API is transport-neutral by design
- the current in-tree end-to-end USER virtual channel path follows the default `SDIO` interface
- the device-side implementation is currently in `backend/host/sdio/virtualchan.c`
- the host userspace library is currently aligned with `mr_sdio.ko + netlink`
- USB device-side ACM transport exists, but in-tree host `nethub_vchan` alignment is not finished
- SPI is not supported

## 3. Prerequisites

Before using the current in-tree USER virtual channel path, confirm:

- device side is built with `CONFIG_NETHUB=y`
- device side enables `CONFIG_MR_VIRTUALCHAN=y`
- the selected bring-up path uses the default `SDIO` interface
- host side has loaded `mr_sdio.ko`
- the host-device link is already working

Notes:

- USER and AT are parallel logical channels
- customer private data should stay on the `USER` type
- documentation should not describe USB and SPI as if they already share a finished end-to-end virtual channel stack

## 4. Device-Side API

Header:

- `components/net/nethub/include/nethub_vchan.h`

Common APIs:

```c
int nethub_vchan_user_send(const void *data, uint16_t len);
int nethub_vchan_user_recv_register(nethub_vchan_recv_cb_t recv_cb, void *cb_arg);
```

## 5. Host-Side API

Header:

- `bsp/common/msg_router/linux_host/userspace/nethub/virtualchan/nethub_vchan.h`

Initialization and cleanup:

```c
int nethub_vchan_init(void);
int nethub_vchan_deinit(void);
```

USER channel:

```c
typedef void (*nethub_vchan_recv_callback_t)(const void *data, size_t len);

int nethub_vchan_user_send(const void *data, size_t len);
int nethub_vchan_user_register_callback(nethub_vchan_recv_callback_t callback);
```

Optional state query:

```c
typedef struct {
    nethub_vchan_link_state_t link_state;
    nethub_vchan_host_state_t host_state;
} nethub_vchan_state_snapshot_t;

int nethub_vchan_get_state_snapshot(nethub_vchan_state_snapshot_t *snapshot);
```

Optional link event callback:

```c
int nethub_vchan_register_link_event_callback(
    nethub_vchan_link_event_callback_t callback,
    void *user_data);
```

## 6. Typical Usage Order

1. `nethub_vchan_init()`
2. `nethub_vchan_user_register_callback()`
3. `nethub_vchan_user_send()`
4. `nethub_vchan_deinit()` when finished

Example:

```c
#include <stdio.h>
#include <string.h>

#include "nethub_vchan.h"

static void user_rx_cb(const void *data, size_t len)
{
    printf("recv USER data: %.*s\n", (int)len, (const char *)data);
}

int main(void)
{
    const char *msg = "hello from host";

    if (nethub_vchan_init() != 0) {
        return -1;
    }

    nethub_vchan_user_register_callback(user_rx_cb);
    nethub_vchan_user_send(msg, strlen(msg));

    nethub_vchan_deinit();
    return 0;
}
```

## 7. Limits and Notes

- maximum payload length per message is `1500` bytes
- USER virtual channel is packet-oriented, not a byte stream
- the current in-tree end-to-end path is SDIO-based
- if the device side does not enable `CONFIG_MR_VIRTUALCHAN`, the current in-tree USER virtual channel path will not work
- if control traffic and USER traffic coexist, keep private application traffic on `USER` and do not reuse the `AT` type

## 8. Related Pages

- [NetHub.md](NetHub.md)
- [NetHubQuickBringup.md](NetHubQuickBringup.md)
- [NetHubArchitecture.md](NetHubArchitecture.md)
