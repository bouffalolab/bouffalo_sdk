# Wi-Fi 4：BL602 构建、连接与事件

## 选择正确的栈

`examples/wifi/defconfig` 在 `CHIP=bl602` 时选择 `CONFIG_WIFI4=y`。Wi-Fi 4 由 `components/wireless/wifi4/` 的 firmware、manager、内置 supplicant、crypto 和适配层组成；不要将 Wi-Fi 6 的 FHOST 或 wl80211 配置结构直接套用到该路径。此处仅描述当前 BL602 示例分支，BL604 等其他芯片的独立构建目标及实际支持需另查当前发布版本。

MAC/manager/supplicant 有源码编译入口；BL602 默认 PHY/RF CMake 链接预编译库，文件路径带旧 GCC 版本名不代表项目当前构建使用旧编译器，也不代表已检查库内部实现。仍使用 SDK 规定的工具链并实际核对构建。`examples/wifi/sta/wifi_http/` 和 `wifi_tcp/` 支持 BL602；前者的 shell 连接示例需要人工发起连接。

## 初始化和 STA

BL602 `wifi_http` 先板级及 RF 初始化、`tcpip_init()`，在任务中注册 `EV_WIFI` 回调、创建 Wi-Fi 任务，随后等待 `CODE_WIFI_ON_INIT_DONE` 再调用 `wifi_mgmr_task_start()`；`CODE_WIFI_ON_MGMR_DONE` 表示 manager 启动阶段完成。该工程中 `fhost_init()` 在 Wi-Fi 4 manager 的实现直接返回 0；`wifi_tcp` 的 BL602 分支则不调用它，日志文字本身不能证明使用了 FHOST。

BL602 的声明位于 `wifi4/manager/bl60x_wifi_driver/include/wifi_mgmr_ext.h`：`wifi_mgmr_sta_enable()` 后使用 `wifi_mgmr_sta_connect(...)` 发起请求。该路径的 connect 实现第 3 个字符串参数是 passphrase、第 4 个是预计算 PSK，而头文件参数名与实现不一致；以实现及本芯片示例为准，不要混用 Wi-Fi 6 的单结构体接口。默认请求启用 DHCP；返回成功表示请求提交，分别等待 `CODE_WIFI_ON_CONNECTED` 和 `CODE_WIFI_ON_GOT_IP`，并处理 `CODE_WIFI_ON_DISCONNECT` 的状态码。源码中 DHCP 获取超时路径会断连，具体重连受 profile 与配置控制。

Wi-Fi 4 的 async 分发经 FreeRTOS timer daemon 处理，执行回调时暂停调度；回调应尽快返回，不执行阻塞操作，并在需要异步处理时复制数据而不是保留事件对象指针。密码相关 CLI 及 `wifi_mgmr_sta_connect_ind_stat_get()` 的当前实现会打印 passphrase；采集和共享日志前应检查并脱敏。后者对 32 字节 SSID 的终止符写入存在静态边界风险，使用前需结合实际代码核实；尚无运行复现。

`CONFIG_STA_MAX`、`CONFIG_VIRT_DEV_MAX`、`CONFIG_WIFI4_SCAN_ITEMS_MAX` 是 Wi-Fi 4 相关配置，后者在 NETBUS 情况有不同固定限制；其数值不是吞吐量或无线连接能力的实测保证。认证枚举不等于目标芯片、固件和接入点的每种组合都已验证。

## 排查和验证

先确认 `CONFIG_WIFI4`、设备固件和 RF 初始化，再按扫描、关联/认证、IP 获取、DNS 和应用连接区分问题。Wi-Fi 4 的 `wifi_mgmr_sta_connect_ind_stat_get`、SSID 边界及日志敏感内容要特别留意。实际射频、认证、DHCP、断线恢复和业务收发需要对应板卡和网络，编译通过不能替代这些验证。
