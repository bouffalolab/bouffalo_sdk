# BLE 基础：host、controller、GAP 与 GATT

先确认目标芯片、controller 库 variant、host 栈及工程配置。普通非 `CONFIG_GEN_ROM` 构建中，`components/wireless/bluetooth/CMakeLists.txt` 对 BL602/BL702 选择 `blecontroller`，其他分支选择 `btblecontroller`；各自可选预编译库或构建目录产物。host 未禁用且非 beacon-only 时，`CONFIG_NIMBLE=y` 选择 NimBLE，否则选择 `blestack`。`CONFIG_BLUETOOTH` 是菜单入口，实际编译还受 controller、host 和其他配置影响。

`blestack` 提供 Zephyr 风格的 host API；NimBLE 是另一套 host 和 API，二者不能直接混用。controller variant 白名单和 host 连接数等参数需与目标芯片匹配；看 `bluetooth/ble_common_readonly.cmake`、`bluetooth/Kconfig` 和工程 `defconfig`。存在源码目录或 README 构建命令不代表某组合已通过实测。Mesh/Nearby 和无线共存另查对应模块。

controller 库 variant 的用途见 `components/wireless/bluetooth/README.md`；BLE API 文档入口为 `docs/zh/api_reference/ble.rst`，实际适用 host 和芯片以工程为准。

## 默认 blestack：初始化和广播

`examples/btble/peripheral/` 展示板级、controller、host、广播和 GATT 服务入口；初始化和广播的具体条件以该工程配置为准。

`bt_le_adv_start` 的长度参数是 `bt_data` 数组元素数。成功调用只证明请求被接受；是否实际广播、可发现及可连接需用对端设备验证。断连后的重新广播应按当前配置和事件处理。

## GATT 服务

默认 blestack 的服务由 `bt_gatt_attr` 属性数组和 `BT_GATT_SERVICE` 构成；`BT_GATT_PRIMARY_SERVICE`、`BT_GATT_CHARACTERISTIC` 与 `BT_GATT_CCC` 的使用见 `examples/btble/peripheral/ble_tp_svc.c`。不能把不存在的 `.uuid/.chars` 字段直接放进 `struct bt_gatt_service`。读写回调需要正确处理偏移、长度、权限和返回值；notify/indicate 需要对应属性和 CCC；可指定目标连接，也可按头文件约定以 NULL 向已启用 CCC 的对端发送。indicate 参数与数据的生命周期应满足异步调用要求。

## 默认 blestack：扫描、连接和客户端

`examples/btble/central/` 展示 active scan、解析广播、停止扫描和发起连接；其 `bt_le_scan_start` 回调为 `(addr, rssi, adv_type, struct net_buf_simple *buf)`，该栈的 `bt_conn_create_le(addr, params)` 返回连接对象引用，按 `conn.h` 管理 `bt_conn_ref/unref`。连接回调的 `err` 非零表示建立失败。其他 host 中同名或类似函数签名可能不同。

`bt_gatt_discover` 需要指定类型、handle 范围、回调和保持有效的参数；异步发现完成时 `attr == NULL`，除非回调主动停止。发现回调提供的只读 attr 不可不复制就长期缓存。`bt_gatt_read/write` 也是异步操作，参数及相关数据须按头文件契约保持有效；读回调的 `data == NULL` 表示结束，写回调报告 ATT 错误。

订阅须根据发现结果填写 characteristic value handle、实际 CCC descriptor handle、通知回调和值；不要假定 CCC 总在 characteristic 的固定偏移。显式 `bt_gatt_unsubscribe` 应检查返回值，并按通知回调 `data == NULL` 的生命周期约定处理参数；当前实现可能在调用内部同步回调，不要假定回调一定发生在函数返回后。头文件对通知回调内主动停止时是否再回调 NULL 的说明与当前实现路径存在差异，具体重入行为尚未运行验证。`central/` 示例对第一个 characteristic 和 CCC 偏移有假设，且示例未显式设置 `value_handle`，不能当作通用发现与订阅流程。

## 配对、安全与配置

`bt_conn_set_security`、认证回调和 I/O 能力受 `CONFIG_BT_SMP`、controller、密钥存储及应用配置影响。Passkey 输入只应在相应认证流程中提交；LE Secure Connections 的 OOB 使用 `bt_le_oob_get_local` 和 `bt_le_oob_set_sc_data` 等当前栈接口，后者须遵循 OOB 请求回调约定。不要把配对成功、链路加密和业务权限当成同一事件。

`examples/btble/btble_cli/` 可用于按当前配置查看蓝牙 shell 接口；命令导出受 `CONFIG_BT_SMP`、`CONFIG_BT_GATT_CLIENT` 等条件限制，应以 `blestack/src/cli_cmds/ble_cli_cmds.c` 和实际固件为准。`examples/btble/btble_nimble/` 演示 NimBLE，使用其自身的 GAP/GATT API 与 controller 配置；先检查对应芯片的 variant 白名单，再构建。

排查时分别记录 controller/host 启动、广播或扫描、连接状态、配对/加密、GATT 发现和读写/通知。构建、烧录、双端连接和业务通信是不同证据；连接数、吞吐量和功耗需在具体配置及硬件上测量。
