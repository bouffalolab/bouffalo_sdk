# Zigbee 应用与 Thread 共存

## 组件和示例

`CONFIG_ZIGBEE` 选择 `components/wireless/zigbee/`。该组件包括 MAC/PHY 适配、ZCL、应用和部分栈源码，也按 `CONFIG_ZBSTACK_DEBUG` 选择 `zbstack/lib/libzbstackex[d].a`；预编译库内部不在当前源码审查范围。`examples/zigbee/zigbee_light`、`zigbee_coordinator`、`zigbee_thread` 的 README 列出 BL616/BL618，其他芯片必须核对目标发布版本和实际构建，不能凭通用 Kconfig 开关推断支持。

`zigbee_light` 例子注册 endpoint 1、灯的多个 ZCL server 和 OTA client，未入网时选择 router；`zigbee_coordinator` 选择 coordinator，配置对应 client cluster。初始化涉及板、MTD、可选 LittleFS、RF、`zb_stackInit()`、注册应用以及 FreeRTOS 调度。示例代码的具体错误处理方式不应替代应用对失败的处理。

## 入网、恢复和持久化

`zb_setRole()` 应在 `zb_start()` 之前调用。头文件说明 factory-new coordinator 建网，router/end device 扫描并加入开放网络；非 factory-new 恢复先前网络。`zb_stackInit()` 可从 Flash 恢复网络信息，所以 `zb_isJoined()` 可能在 `zb_start()` 前已为真。关注 `ZB_EVT_STARTUP_COMPLETE` 的状态和角色、离网以及认证等事件，成功调用启动函数本身不能证明已入网或认证成功。BDB 提供 network formation 和 steering 等接口，完成结果通过回调报告；实际 CLI 是否启用由配置决定。

网络参数、计数器和密钥属于敏感持久数据，检查目标分区和备份策略。示例 factory reset 会离网、清理部分用户 Flash 并复位，异常处理也可能触发 reset；调试前确认对已有网络及数据的影响，不能将恢复出厂当成无副作用的常规步骤。

## Zigbee + Thread

`examples/zigbee/zigbee_thread` 同时启用 `CONFIG_ZIGBEE`、`CONFIG_THREAD`、`CONFIG_LMAC154` 和 `CONFIG_ZB_USE_LMAC154`；Thread 和 Zigbee 分别注册 LMAC154 的 stack 1、stack 2，示例将 Thread dataset 的信道传给 Zigbee，属于该示例的同信道安排。该工程明确不包含 Wi-Fi/BT，不能据此声称四协议并发受支持。LMAC 双栈回调与芯片边界见 [23-ieee802154-thread.md](23-ieee802154-thread.md)。

验证应分别记录固件构建、设备烧录、coordinator 建网、设备加入和认证、ZCL 操作、重启恢复、离网及双栈并发；双端设备和真实网络未运行时不能将构建成功描述为 Zigbee 功能通过。
