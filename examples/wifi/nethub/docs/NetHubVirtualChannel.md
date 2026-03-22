# NetHub USER Virtual Channel

This document describes how to use the NetHub `USER virtual channel`.

## 1. Concept

`USER virtual channel` is a logical message channel carried on the current host
link.

It is not a separate physical interface. It reuses the current
`SDIO / USB / SPI` transport and multiplexes different payload types into
separate logical channels.

Commonly used types:

- `USER`
  - private customer or application data
- `AT`
  - host control-plane data
- `SYSTEM`
  - internal system coordination messages

For most customer integrations, only the `USER` type matters.

## 2. Bring-Up Prerequisites

Before using it, confirm:

- device side is built with `CONFIG_NETHUB=y`
- device side is built with `CONFIG_MR_VIRTUALCHAN=y`
- host side has already loaded `mr_sdio.ko`
- the underlying host-device link is already working

Additional notes:

- The `USER` channel and the control channel are parallel channels
- If the host control plane itself uses `vchan`, it uses the `AT` type
- Customer private messages should stay on the `USER` type

## 3. device-side Interfaces

Header:

- `components/net/nethub/include/nethub_vchan.h`

Common APIs:

```c
int nethub_vchan_user_send(const void *data, uint16_t len);
int nethub_vchan_user_recv_register(nethub_vchan_recv_cb_t recv_cb, void *cb_arg);
```

## 4. host-side Interfaces

Header:

- `bsp/common/msg_router/linux_host/userspace/nethub/virtualchan/nethub_vchan.h`

### 4.1 Initialization and Cleanup

```c
int nethub_vchan_init(void);
int nethub_vchan_deinit(void);
```

### 4.2 USER Channel

```c
typedef void (*nethub_vchan_recv_callback_t)(const void *data, size_t len);

int nethub_vchan_user_send(const void *data, size_t len);
int nethub_vchan_user_register_callback(nethub_vchan_recv_callback_t callback);
```

### 4.3 Optional State Query

If you need to know whether the virtual channel is ready, you can optionally
use:

```c
typedef struct {
    nethub_vchan_link_state_t link_state;
    nethub_vchan_host_state_t host_state;
} nethub_vchan_state_snapshot_t;

int nethub_vchan_get_state_snapshot(nethub_vchan_state_snapshot_t *snapshot);
```

Typical checks:

- `link_state == NETHUB_VCHAN_LINK_UP`
  - the link is ready for TX/RX
- `host_state == NETHUB_VCHAN_HOST_STATE_DEVICE_RUN`
  - the host has completed the handshake with the device

### 4.4 Optional Link Event Callback

If the application wants passive notification when the link goes up or down, it
can optionally register:

```c
int nethub_vchan_register_link_event_callback(
    nethub_vchan_link_event_callback_t callback,
    void *user_data);
```

## 5. Typical Usage Order

1. `nethub_vchan_init()`
2. `nethub_vchan_user_register_callback()`
3. `nethub_vchan_user_send()`
4. Call `nethub_vchan_deinit()` when finished

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

## 6. Limits and Notes

- Maximum payload length per message is `1500` bytes
- The `USER` channel is packet-oriented, not a byte stream
- The kernel module must be loaded before use
- If the device side does not enable `CONFIG_MR_VIRTUALCHAN`, the host-side
  `USER` channel will not work
- If host control-plane traffic and `USER` traffic coexist, do not reuse the
  `AT` type for customer private data

## 7. Related Pages

- [NetHubQuickBringup.md](NetHubQuickBringup.md)
- [NetHubArchitecture.md](NetHubArchitecture.md)
- [NetHub.md](NetHub.md)
