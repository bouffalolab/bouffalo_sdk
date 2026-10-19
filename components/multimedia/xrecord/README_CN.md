# xrecord

[English](README.md) | 简体中文

xrecord 将麦克风或 ringbuffer 数据传送到录音/流式输出目标，例如 WebSocket 服务器。

## SDK 集成

启用 `CONFIG_MULTIMEDIA=y`、`CONFIG_XRECORD=y` 和所需的 MSP OS 适配。
公开接口见 [msp/record.h](include/msp/record.h)。数据源及输出 URI 的实现位于
`recio/`；选择麦克风输入时需配置音频采集后端，选择 WebSocket 输出时需启用
`CONFIG_XWEBSOCKET=y`、网络及 POSIX 线程支持。

使用顺序为 `record_register()` → 可选块大小/数据就绪回调配置 →
`record_start()` → `record_stop()` → `record_unregister()`。
注册失败返回 `NULL`，开始、停止及注销应检查负错误码。
异步传输期间保持输入数据、ringbuffer 和回调上下文有效，停止并释放服务后再回收。
参考[多媒体总览](../README_CN.md)了解音频硬件和媒体配置。

## 接口

### 注册

```C
rec_hdl_t record_register(const char *from, const char *to);
```

* 参数
  * from: 数据来源，例如：rbuf://handle=403913384
  * to: 数据存储的地方，例如：rbuf://handle=403913384；ws://192.168.1.102:9090/mic.pcm
* 返回值
  * `rec_hdl_t`：成功
  * NULL: 失败

### 设置数据ready的回调函数

```C
void record_set_data_ready_cb(rec_hdl_t hdl, data_ready_func cb, void *arg);
```

* 参数
  * hdl: `rec_hdl_t`句柄
  * cb: 回调函数
  * arg: 回调函数的参数
* 返回值
  * 无

### 设置一次读取的块大小

```C
void record_set_chunk_size(rec_hdl_t hdl, int chunk_size);
```

* 参数
  * hdl: `rec_hdl_t`句柄
  * chunk_size: 块大小
* 返回值
  * 无

### 开始录制

```C
int record_start(rec_hdl_t hdl);
```

* 参数
  * hdl: `rec_hdl_t`句柄
* 返回值
  * 0: 成功
  * < 0: 失败

### 停止录制

```C
int record_stop(rec_hdl_t hdl);
```

* 参数
  * hdl: `rec_hdl_t`句柄
* 返回值
  * 0: 成功
  * < 0: 失败

### 注销，释放资源

```C
int record_unregister(rec_hdl_t hdl);
```

* 参数
  * hdl: `rec_hdl_t`句柄
* 返回值
  * 0: 成功
  * < 0: 失败


## 示例

以下片段使用已有的 `x_ringbuf_t` 缓冲区上传数据，`destination` 为有效的 `ws://` URL。
调用前需完成 OS 和网络初始化；生产者负责向 ringbuffer 填入数据。
当前 URI 分发支持 `mic://`、`rbuf://`、`ws://` 和 `null://`，
`mem://` 与 `sdbuffer://` 路径尚未接入。更多用法见 [cli/cli_record.c](cli/cli_record.c)。

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

按应用供数方式设置数据就绪回调，避免无数据时忙等。结束时先停止生产者并协调回调退出，
再检查 `record_stop()` 和 `record_unregister()` 返回值，最后释放 ringbuffer 及上下文。
