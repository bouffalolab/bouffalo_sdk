        # NetHub Virtual Channel Guide

        If you are new to NetHub bring-up, read these first:

        - `examples/wifi/nethub/README.md`
        - `examples/wifi/nethub/docs/NetHubQuickBringup.md`
        - `bsp/common/msg_router/linux_host/userspace/nethub/README.md`

        ## 1. Overview

        NetHub virtual channel is a logical message channel carried on the active
        host link.

        The public API is transport-neutral by design, but the current in-tree
        host implementation is still the SDIO reference path built on
        `mr_sdio.ko + netlink`.

        Relevant logical types:

        - `USER`
          - private application data
        - `AT`
          - control payloads
        - `SYSTEM`
          - internal coordination

        ## 2. Current Support Status

        | Area | Current status |
        | --- | --- |
        | Host public API | available |
        | Current in-tree backend | SDIO-based |
        | End-to-end reference path | `SDIO` |
        | USB | device-side ACM transport exists, host wrapper alignment not finished |
        | SPI | not supported |

        ## 3. Data Types

        | Type | Value | Usage |
        | --- | --- | --- |
        | USER | `0x01` | private user payload |
        | AT | `0x02` | AT or control payload |
        | SYSTEM | `0x03` | internal system payload |

        ## 4. Prerequisites

        Before using the host-side USER virtual channel, ensure that:

        1. device is built with:
           - `CONFIG_NETHUB=y`
           - `CONFIG_MR_VIRTUALCHAN=y`
        2. the selected path is the current SDIO reference flow
        3. host has loaded `mr_sdio.ko`
        4. the underlying host-device link is working

        Notes:

        - USER and AT are parallel logical channels
        - if host control already uses `vchan`, it uses the `AT` type
        - customer private traffic should stay on the `USER` type

        ## 5. Build

        Build from the top-level host entry:

        ```bash
        cd bsp/common/msg_router/linux_host/userspace/nethub
        ./build.sh build
        ```

        Artifacts include:

        - `output/libnethub_vchan.a`
        - `output/nethub_vchan_app`

        Build this directory only:

        ```bash
        cd bsp/common/msg_router/linux_host/userspace/nethub/virtualchan
        make
        ```

        ## 6. Host Public API

        Header:

        - `bsp/common/msg_router/linux_host/userspace/nethub/virtualchan/nethub_vchan.h`

        Init and deinit:

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

        ## 7. Typical Usage

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
            printf("recv USER data: %.*s
", (int)len, (const char *)data);
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

        ## 8. Test Utility

        `nethub_vchan_app` is an optional debug tool. It is not required for
        customer integration.

        Quick check:

        ```bash
        cd bsp/common/msg_router/linux_host/userspace/nethub/virtualchan
        sudo ./nethub_vchan_app
        ```

        ## 9. Limits and Notes

        - maximum payload length per message is `1500` bytes
        - USER virtual channel is packet-oriented, not a byte stream
        - the kernel module must be loaded before use
        - current in-tree host virtual-channel support is SDIO-based
        - if device does not enable `CONFIG_MR_VIRTUALCHAN`, the current in-tree
          USER virtual-channel path will not work
        - do not reuse the `AT` type for private USER payloads

        ## 10. Related Docs

        - `../README.md`
        - `../HOST_ARCHITECTURE.md`
        - `examples/wifi/nethub/docs/NetHubVirtualChannel.md`
