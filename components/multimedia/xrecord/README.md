# xrecord

English | [简体中文](README_CN.md)

xrecord routes microphone or ringbuffer data to recording/streaming destinations, including a WebSocket server.

## SDK Integration

Enable `CONFIG_MULTIMEDIA=y`, `CONFIG_XRECORD=y`, and the required MSP OS port.
The public API is [msp/record.h](include/msp/record.h); source and destination URI
implementations are in `recio/`. Microphone input requires an audio capture
backend. WebSocket output requires `CONFIG_XWEBSOCKET=y`, networking, and POSIX
thread support.

Use `record_register()` → optional chunk size/data-ready callback configuration
→ `record_start()` → `record_stop()` → `record_unregister()`.
Registration returns `NULL` on failure; check negative errors from start, stop,
and unregister. Keep input data, ringbuffers, and callback context alive during
asynchronous transfers; stop and release the service before freeing them.
See the [multimedia guide](../README.md) for audio and media setup.

## Interfaces

### Registration

```C
rec_hdl_t record_register(const char *from, const char *to);
```

* Parameters
  * from: Data source. For example:rbuf://handle=403913384
  * to: Data destination. For example:rbuf://handle=403913384;ws://192.168.1.102:9090/mic.pcm
* Return Values
  * `rec_hdl_t`:Success
  * NULL: Failure

### Set Data Ready Callback Function

```C
void record_set_data_ready_cb(rec_hdl_t hdl, data_ready_func cb, void *arg);
```

* Parameters
  * hdl: `rec_hdl_t`handle
  * cb: Callback function
  * arg:  User data passed to the callback function
* Return Values
  * None

### Set the Block Size for Each Read Operation

```C
void record_set_chunk_size(rec_hdl_t hdl, int chunk_size);
```

* Parameters
  * hdl: `rec_hdl_t`handle
  * chunk_size: Block size
* Return Values
  * None

### Start Recording

```C
int record_start(rec_hdl_t hdl);
```

* Parameters
  * hdl: `rec_hdl_t`handle
* Return Values
  * 0: Success
  * < 0: Failure

### Stop Recording

```C
int record_stop(rec_hdl_t hdl);
```

* Parameters
  * hdl: `rec_hdl_t`handle
* Return Values
  * 0: Success
  * < 0: Failure

### Deregister and Release Resources

```C
int record_unregister(rec_hdl_t hdl);
```

* Parameters
  * hdl: `rec_hdl_t`handle
* Return Values
  * 0: Success
  * < 0: Failure


## Example

This fragment uploads from an existing `x_ringbuf_t`; `destination` is a valid
`ws://` URL. Initialize the OS and networking first; a producer supplies data to
the ringbuffer. Current URI dispatch supports `mic://`, `rbuf://`, `ws://`, and
`null://`; the `mem://` and `sdbuffer://` paths are not connected. See
[cli/cli_record.c](cli/cli_record.c) for further integration examples.

```c
#include <inttypes.h>
#include <stdio.h>
#include <msp/record.h>
#include <xutils/xringbuffer.h>

rec_hdl_t start_ringbuffer_upload(x_ringbuf_t *ringbuffer,
                                 const char *destination)
{
    char source[64];
    snprintf(source, sizeof(source), "rbuf://handle=%" PRIuPTR,
             (uintptr_t)ringbuffer);
    rec_hdl_t handle = record_register(source, destination);
    if (handle == NULL) {
        return NULL;
    }
    record_set_chunk_size(handle, 2560);
    if (record_start(handle) < 0) {
        record_unregister(handle);
        return NULL;
    }
    return handle;
}
```

Set a data-ready callback appropriate to the producer to avoid polling an empty
buffer. On shutdown, stop the producer and coordinate callback completion, check
`record_stop()` and `record_unregister()`, then free the ringbuffer and context.
