# 通用运行时与工具组件

路径相对 SDK 根目录。FreeRTOS 内存、日志和文件系统分别见其他专题；本页覆盖系统初始化、shell 和通用工具。

## 初始化和 C 运行时

`components/sysinit/sysinit.h` 的 `SYSINIT(name, sub, order, fn, arg)` 将初始化项放在 `.sysinit.<sub>.<order>.<name>`，优先级是两位十六进制字符串。链接脚本需要保留并按 section 名排序；`sysinit_run_all()` 逐项调用 `void` 回调，不提供错误返回传播。各芯片启动顺序须看对应 `SystemInit` 和链接脚本。

当前 libc 使用 Picolibc 适配；`components/libc` 的文件系统后端由配置决定，没有相应后端时 nosys 调用可能返回 `ENOSYS`。启动 TLS 与 FreeRTOS 每任务 TLS 见 `docs/standard_riscv_toolchain.md`；printf 浮点、POSIX 等配置需按项目启用，不应只凭包含标准头文件推断可用。

## Shell

`components/shell/shell.h` 提供 `shell_init()`、`shell_handler(uint8_t)`、`shell_exec()` 与 `SHELL_CMD_EXPORT`、`SHELL_CMD_EXPORT_ALIAS`。导出宏放入 `FSymTab`，目标链接脚本必须保留符号表。`examples/shell/shell_no_os/` 轮询串口并逐字节送入 handler；`examples/shell/shell_os/` 使用 `shell_init_with_task()`。是否运行、任务栈、串口资源及命令本身的副作用要按目标配置核对。

## Ring buffer 与事件

`components/utils/ring_buffer/ring_buffer.h` 的 `Ring_Buffer_Init` 接受由调用者持有的 `Ring_Buffer_Type` 和存储区，以及可选的 lock/unlock 回调；Init 和 Reset 返回 `void`。读写 API 返回实际字节数；Force 写与普通写的覆盖语义不同，不能假设写入请求总能完整接收。零拷贝消费可使用 `Get_Read_Index` 和 `Advance_Read`，DMA 缓冲生命周期、环绕分段及生产者竞争仍由集成方处理。多上下文访问需提供适合任务/中断场景的同步，不应仅凭 `volatile` 假定并发安全。

`components/utils/async_event` 在 FreeRTOS 配置下构建。`async_post_general_event` 按 event 的 size 分配并复制事件，完成过滤器调用和可选 finish 回调后释放副本；包含的指针仍需由应用保证其指向对象的生命周期。取消注册先标记删除，后续事件循环才调用清理回调并释放过滤器；队列头插入，不应假设 FIFO。

## XZ/LZMA2 解码

`components/lzma/xz/` 是 XZ/LZMA2 解码源码；`components/lzma/Kconfig` 说明用于 boot2 和镜像工具，`examples/boot2_isp/defconfig` 与 `examples/eflash_loader/defconfig` 是配置入口。

## 数据与资源

- cJSON 位于 `components/utils/cjson/`。检查解析、创建、查找与类型检查的返回值；`cJSON_Parse` 产生的树由调用者 `cJSON_Delete`，打印生成的字符串用匹配分配器的 `cJSON_free` 释放。对象所有权转移、引用和 detach 的细节以 `cJSON.h` 为准。
- `components/utils/bflb_block_pool/` 使用调用者提供的池内存；未归还所有块时 delete 失败。参数检查依赖调试配置，应用需自身保证块、池及锁回调的有效性。
- `components/utils/math/include/` 提供 base64、CRC 和 hex，`components/utils/getopt/` 提供解析；以真实头文件和输出缓冲容量为准。

可从 `examples/shell/` 构建裸机和 FreeRTOS 示例；`components/utils/ring_buffer/test.c` 可通过 `RING_BUFFER_HOST_TEST` 在主机做带锁并发压力检查。主机测试不证明 MCU 的中断、DMA 和缓存一致性。
