# NetHub Virtual Channel Guide

If you are new to NetHub bringup, read these first:

- `examples/wifi/nethub/README.md`
- `examples/wifi/nethub/docs/NetHubQuickBringup.md`
- `bsp/common/msg_router/linux_host/userspace/nethub/README.md`

## 1. Overview

NetHub Virtual Channel is a logical message channel carried on the active host link.

It is not a separate physical interface. It reuses the current `SDIO / USB / SPI`
transport and multiplexes different payload types into different logical channels.

The relevant channel types are:

- `USER`
  - private application data
- `AT`
  - host control-plane payload
- `SYSTEM`
  - internal coordination

For customer integrations, the main concern is usually the `USER` channel.

## 2. Data Types

| Type | Value | Usage |
| --- | --- | --- |
| USER | `0x01` | private user payload |
| AT | `0x02` | AT / control payload |
| SYSTEM | `0x03` | internal system payload |

## 3. Prerequisites

Before using the host-side USER virtual channel, ensure that:

1. device is built with:
   - `CONFIG_NETHUB=y`
   - `CONFIG_MR_VIRTUALCHAN=y`
2. host has loaded `mr_sdio.ko`
3. the underlying host-device link is already working

Notes:

- USER and AT are parallel logical channels
- if host control already runs over `vchan`, it uses the `AT` type
- customer private traffic should use the `USER` type

## 4. Build

### 4.1 Build from the host top-level entry

Recommended:

```bash
cd bsp/common/msg_router/linux_host/userspace/nethub
./build.sh build
```

Artifacts include:

- `output/libnethub_vchan.a`
- `output/nethub_vchan_app`

### 4.2 Build virtualchan only

```bash
cd bsp/common/msg_router/linux_host/userspace/nethub/virtualchan
make
```

## 5. Host Public API

Header:

- `bsp/common/msg_router/linux_host/userspace/nethub/virtualchan/nethub_vchan.h`

### 5.1 Init and deinit

```c
int nethub_vchan_init(void);
int nethub_vchan_deinit(void);
```

### 5.2 USER channel

```c
typedef void (*nethub_vchan_recv_callback_t)(const void *data, size_t len);

int nethub_vchan_user_send(const void *data, size_t len);
int nethub_vchan_user_register_callback(nethub_vchan_recv_callback_t callback);
```

### 5.3 Optional state query

If the application needs to know whether virtual channel is ready, it can use:

```c
typedef struct {
    nethub_vchan_link_state_t link_state;
    nethub_vchan_host_state_t host_state;
} nethub_vchan_state_snapshot_t;

int nethub_vchan_get_state_snapshot(nethub_vchan_state_snapshot_t *snapshot);
```

Typical checks:

- `link_state == NETHUB_VCHAN_LINK_UP`
  - link is ready for data transfer
- `host_state == NETHUB_VCHAN_HOST_STATE_DEVICE_RUN`
  - host-side state machine has completed handshake with device

### 5.4 Optional link event callback

If the application needs asynchronous notification when the link goes up or down,
it can register:

```c
int nethub_vchan_register_link_event_callback(
    nethub_vchan_link_event_callback_t callback,
    void *user_data);
```

For most customer integrations, the minimal flow is still:

- `nethub_vchan_init()`
- `nethub_vchan_user_register_callback()`
- `nethub_vchan_user_send()`
- `nethub_vchan_deinit()`

State snapshot and link event callback are only needed when the application
wants stricter readiness checks or passive link-state tracking.

## 6. Typical Usage

Typical flow:

1. `nethub_vchan_init()`
2. `nethub_vchan_user_register_callback()`
3. `nethub_vchan_user_send()`
4. `nethub_vchan_deinit()`

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

## 7. Test Utility

`nethub_vchan_app` is an optional debug tool. It is not required for customer
integration of the USER channel.

If a quick link check is needed:

```bash
cd bsp/common/msg_router/linux_host/userspace/nethub/virtualchan
sudo ./nethub_vchan_app
```

## 8. Limits and Notes

1. Maximum payload length per message is `1500` bytes.
2. USER channel is packet-oriented, not a byte stream.
3. The kernel module must be loaded before use.
4. If device does not enable `CONFIG_MR_VIRTUALCHAN`, the host USER channel will not work.
5. If host control-plane and USER traffic coexist, keep customer private traffic on `USER` and do not reuse the `AT` type.

## 9. Related Docs

- `examples/wifi/nethub/README.md`
- `examples/wifi/nethub/docs/NetHub.md`
- `examples/wifi/nethub/docs/NetHubVirtualChannel.md`
- `bsp/common/msg_router/linux_host/userspace/nethub/README.md`
