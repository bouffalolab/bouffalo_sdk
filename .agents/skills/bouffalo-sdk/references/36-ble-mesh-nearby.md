# BLE Mesh 与 Nearby Fast Pair

路径相对 SDK 根目录。两者是不同协议，均依赖对应 BLE host/controller 与板级资源；构建通过并不证明手机互通或认证。

## BLE Mesh

`components/wireless/bluetooth/CMakeLists.txt` 在启用 `CONFIG_BT_MESH` 后引入 `blemesh`，`CONFIG_BT_MESH_MODEL` 再引入标准模型。核心包括 provisioning、network、transport、access、beacon 和配置，Friend、LPN、Proxy、PB-ADV/PB-GATT、Provisioner 等按配置选择；Generic/Lighting/Sensor/Time-Scene 模型以对应模型 CMake 和实际应用为准。

`examples/btble/blemesh_simple/` 包含 Configuration Server 和 Generic OnOff server/client，恢复持久配置；未配网时启用 PB-GATT。`blemesh_cli/README.md` 展示配网、AppKey 绑定、组地址及 Provisioner 的使用。存储、密钥、节点地址和模型绑定是设备运行时状态，不应将示例默认值视为产品配置。Simple 的配置明确标记 Friend 不支持，不能只根据 Kconfig 默认开关判断节点能力。

当前主配置入口为 `components/wireless/bluetooth/Kconfig`；`blemesh/src/Kconfig` 中额外依赖是否被具体构建引入须实际核查。旧 `bouffalo.mk` 与当前 CMake 对 Mesh 1.1、BLOB/DFU 等源文件的接线不同，不能仅凭宏认定功能完整。Simple 的 UUID 构造两次复制在 index 9 重叠、index 15 未由这两次复制填充；设备身份影响未实测，产品适配应检查 UUID 的完整性和唯一性。

## Nearby

`CONFIG_BT_NEARBY` 接入 SDK 的 embedded Fast Pair BLE 适配，示例为 `examples/btble/nearby/`。当前 CMake 采用 mbedTLS 与 BLE-only，关闭 battery notification、additional data 和 message stream；若需要扩展功能，应逐项检查平台适配，不要由上游目录存在推断已接入。

示例需与平台注册的名称、24-bit model ID 和防伪私钥匹配，当前头文件是空名称、零 ID 和空密钥。密钥属于敏感材料，不要放进共享文档、命令输出或提交记录。适配层部分地址旋转、能力查询、音频和电池接口仍有 TODO，是否影响具体产品应按启用功能验证。持久化依赖 LittleFS KV、分区和 Flash；随机数实现与产品安全要求需另行评估。

两个示例 README 声明 BL616/BL618，其他芯片有代码分支不等于已验证支持。配网、密钥恢复、手机互通与断电恢复需对应设备验证。
