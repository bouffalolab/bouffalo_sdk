# Matter 集成与工厂数据

以下描述当前 checkout；SDK 外的完整 Matter 协议栈和应用需单独核对版本与来源。

`CONFIG_MATTER` 默认关闭；当前 `components/wireless/CMakeLists.txt` 启用后引入 `matter/mfd` 工厂数据组件，并不因此包含完整 Matter 应用。当前 repo manifest 包含独立的 MFD 仓库，另一个 manifest 声明 `connectedhomeip`，但当前工作区没有检出该项目。`examples/wifi/sta/matter/` 的 README 说明它只为 BL616 Matter 应用编译部分模块、生成库，且不应对外发布；其 main 初始化 Wi-Fi 和网络，不是配网演示。

`components/wireless/matter/mfd/include/bflb_mfd.h` 提供 `mfd_init()` 和 DAC、PAI、passcode、discriminator、SPAKE2 等访问接口；`mfd_init` 读取活动 `MFD` 分区并检查最小数据集合。最小集合包含 DAC 私钥，但不包含 DAC 证书，调用成功不等于认证所需资料完整。接口还提供私钥复制和直接指针，实际访问权限与生命周期需按实现及产品威胁模型处理。

MFD 格式支持明文和密文区域、CRC；允许零长度密文供开发/测试，不能从存在 MFD 推断数据已加密或具有密码学完整性保护。证书、私钥、passcode 及日志应按产品流程保护，不能写入示例文档。芯片专用解密实现、硬件密钥、eFuse 和工厂分区需匹配实际产品；源码和 CMake 目标差异不能仅凭 README 支持表推断兼容。

完整验证应分别覆盖配置/构建、MFD 分区和证书、完整协议栈、Wi-Fi 或 Thread 传输、发现与配网、控制命令及设备恢复；只构建该库生成示例不证明端到端 Matter 或认证通过。

本工作区用 GCC 14.3.0 的 BL616 库生成示例构建通过；将同一示例指定为 BL618DG AP 时编译失败，MFD 源码未选择 BL618DG 平台头，报告 `bool`、`uint8_t` 等未知类型。该示例原本仅声明 BL616，用这个结果不能推断其他独立集成的行为。
