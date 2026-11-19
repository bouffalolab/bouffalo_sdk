# IEEE 802.15.4 与 OpenThread

## LMAC154

`components/wireless/lmac154/lmac154/include/lmac154.h` 提供 802.15.4 低层接口，`CONFIG_LMAC154` 控制组件。当前源码 CMake 有 BL702、BL702L、BL616、BL618DG 分支，BL616CL 分支明确不支持；是否适用于某个板还需结合实际 PHY、栈和构建配置。CMake 优先查预编译库，否则使用构建目录中的 LMAC 产物。

接口分 legacy 和双栈路径：前者以 `lmac154_getInterruptHandler()`、`lmac154_triggerTx()` 和全局事件回调工作；后者用 `lmac154_getInterruptCallback()`、参数化 TX 和 `lmac154_registerEventCallback()` 注册接收回调。后者只有 `LMAC154_STACK_1/2` 两个槽，重复注册会覆盖该槽；当前 OpenThread 使用槽 1，Zigbee 的相关实现使用槽 2。头文件注释称注册函数返回中断处理器，实际签名返回 `bool`，IRQ 处理器需另取。回调涉及中断上下文，应遵循对应接口的上下文要求和数据生命周期。示例见 `examples/lmac154/`。

## OpenThread 模式和配置

`examples/thread/` 有 `openthread_cli`、`openthread_ncp`、Wi-Fi BR、Ethernet BR；各自 README 列出 BL616/BL618，不能推广为全部芯片支持矩阵。CLI 默认 FTD 与 shell；可选择 MTD。NCP 工程默认 RCP，也可配置 NCP FTD/MTD：RCP 提供 radio 协处理，主机仍需对应 Thread 栈；NCP 运行更多协议栈功能。CLI 与 NCP/RCP 互斥，FTD/MTD、NCP/RCP 各自互斥；当前移植要求 LittleFS 保存 OpenThread settings，详见 `components/wireless/thread/openthread_port/CMakeLists.txt`。

OpenThread 核心有默认版本，也可由工程项目配置覆盖；不要把所有固件固定描述为 Thread 1.2。特性配置查工程的 `openthread-core-proj-config.h` 和最终编译宏。`otrStart()` 创建 FreeRTOS 任务，任务初始化实例并通过 `otrInitUser(otInstance *)` 钩子执行应用初始化，随后处理 tasklet 等事件；示例用 `otDatasetIsCommissioned`、`otIp6SetEnabled` 和 `otThreadSetEnabled`，请检查返回结果与线程同步要求。

## 边界路由与验证

Wi-Fi BR 示例描述单芯片 Wi-Fi/802.15.4 共享 RF 时间片，高 DTIM 可能增加时延，SSED 能力有文档限制；Ethernet BR 使用不同的基础网络。示例文档描述当前 OTBR 为无状态 IPv6 地址配置；不要将其扩展成所有网络配置已支持。NCP/RCP 与主机间示例使用 Spinel/HDLC，UART 和波特率以该工程板级配置为准。

在 GCC 14.3 环境下可分别为 CLI FTD、RCP 指定独立 `BUILD_DIR` 并编译 BL616，具体配置见各示例 README；不要让不同角色共享残留 menuconfig。真实验证至少需要对应固件、设备与对端，按 dataset、角色形成、IPv6 ping、断线恢复逐项记录；BR 还需网络侧和对端测试。仅构建不能证明射频通信、组网、互操作或边界转发成功。
