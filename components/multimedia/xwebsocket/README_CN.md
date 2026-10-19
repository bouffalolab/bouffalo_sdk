# xwebsocket (librws)

[English](README.md) | 简体中文

xwebsocket 将 librws WebSocket 客户端接入 SDK，提供连接、发送文本/二进制数据、
接收回调及后台线程处理。公开接口位于 [include/librws.h](include/librws.h)。

## SDK 集成

启用 `CONFIG_MULTIMEDIA=y` 和 `CONFIG_XWEBSOCKET=y`，并提供网络及 POSIX
线程/互斥锁支持。线程适配见 [src/port/rws_thread.c](src/port/rws_thread.c)。
本 SDK 使用 CMake 构建组件，英文文档中的 CocoaPods 安装方法属于上游平台用法。
录音流集成可参考 [xrecord](../xrecord/README_CN.md)，依赖配置见[多媒体总览](../README_CN.md)。

## 使用流程

1. 用 `rws_socket_create()` 创建句柄，检查是否为 `NULL`。
2. 配置 scheme、host、port、path，并注册回调；断开回调为必需项。
3. 调用 `rws_socket_connect()` 发起连接，连接成功回调后再发送数据。
4. 通过 `rws_socket_send_text()` 等接口发送；检查相应返回值。
5. 使用 `rws_socket_disconnect_and_release()` 断开并释放，随后停止使用句柄。

```c
#include <stdio.h>
#include <librws.h>

static void on_disconnected(rws_socket socket)
{
    rws_error error = rws_socket_get_error(socket);
    if (error != NULL) {
        printf("WebSocket error: %s\n", rws_error_get_description(error));
    }
}

static void on_text(rws_socket socket, const char *text,
                    const unsigned int length, bool is_finished)
{
    (void)socket;
    (void)is_finished;
    printf("%.*s\n", (int)length, text);
}
```

将上述函数通过 `rws_socket_set_on_disconnected()` 和
`rws_socket_set_on_received_text()` 注册到句柄；完整连接流程见英文版示例。
文本和二进制接收回调包含 `bool is_finished`，用于标识消息分片是否结束。
按回调长度读取数据，不要假定 payload 以零结尾。
回调在 socket 处理上下文执行，应尽快返回，并在释放前协调应用中的句柄访问。
断开时通过 `rws_socket_get_error()` 查询连接或传输错误。

## 上游与许可证

上游项目为 [librws](https://github.com/OlehKulykov/librws)。许可证及版权声明如下。

The MIT License (MIT)

Copyright (c) 2014 - 2019 Oleh Kulykov <info@resident.name>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
