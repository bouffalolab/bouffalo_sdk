# Netbus 与 ATModule

## 组件与构建

`CONFIG_NETBUS` 启用 `components/net/netbus/`，其中包含 host router、可选 netifd 和 ATModule；`CONFIG_ATMODULE` 按 `ATMODULE_PATH` 引入 AT 实现。NetHub 是另一组件，可将 ATModule 作为可选控制通道，通过 TTY 适配连接；数据桥接与传输边界见 [26-nethub.md](26-nethub.md)。

ATModule 默认使用 `at_core/v1.0`，`CONFIG_ATMODULE_V2_0=y` 选择 v2.0；UART、USB、NX SPI、SPI sync、TTY 是传输 choice。网络命令依赖 work queue，HTTP/MQTT 依赖网络命令配置；具体命令集以芯片、核心版本及生成配置为准。示例在 `components/net/netbus/atmodule/examples/{uart_wifi,spi_wifi,usb_wifi}`。仓库 README 的芯片/接口表是 CI 构建覆盖，不是所有组合的运行兼容保证。

如遇 `kmin_free_size` 链接错误，检查目标内存组件配置及 `CONFIG_MM_ENABLE_MIN_FREE_TRACKING`；编译通过不代表 AT 命令已在设备上验证。

示例的 `components/net/netbus/atmodule/examples/sdk.mk` 在 SDK 不存在时可能 clone，使用内嵌 SDK 且版本不符时可能 fetch/checkout。执行构建前显式指向已核实的 `BL_SDK_BASE`，检查工作区和构建目标。工程启用 LittleFS 镜像打包时，当前绝对 `BUILD_DIR` 有路径拼接问题，隔离构建方法见 NetHub 文档。

## 命令处理和扩展（v2.0）

v2 解析器要求 `AT` 命令以 CRLF 结束、命令名不区分大小写；执行、查询 `?`、测试 `=?` 和设置 `=...` 分派到 `at_cmd_struct` 的不同回调。`at_cmd_register()` 保存命令表指针，表须持续有效且以空名称结尾；`para_num_min/max` 在 setup 分支检查。当前头文件限制输入 256 字节、最多 17 个解析项（含命令名）及 12 个注册表；语法和不同版本差异以所选 `at_core` 源码为准。

解析器原地修改输入字符串，`argv` 位于解析调用栈且指向复用输入缓冲；回调返回后要异步使用的参数必须复制。输出 callback 的数据也可能来自临时或栈缓冲，异步保存应复制。`at_module_func(char *cmd, ...)` 要求可写命令缓冲，返回值是输入处理剩余长度，不能当作命令执行状态。

回显默认开启，调试日志可能打印参数；Wi-Fi 等命令可能携带密码。诊断与日志共享前应检查并脱敏，避免把真实凭据写入示例或记录。实际命令、设备类型和详细格式分别查仓库 `docs/{zh,en}/samples/atmoudle/` 的对应版本及构建后的功能开关。

## 验证

分别核对编译配置、固件烧录、物理传输、AT 往返、网络命令和断线恢复。USB 枚举、UART 波特率/引脚、SPI 主机及固件版本必须与目标板匹配；编译或解析器源码检查不证明真实命令交互或网络连接成功。
