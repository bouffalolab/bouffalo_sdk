# BFLB WiFi 命令列表

## CLI 命令 → AT 指令

### 连接 AP
```bash
# Open 网络
./bflbwifictrl connect_ap <ssid>

# 加密网络
./bflbwifictrl connect_ap <ssid> <password>
```
→ `AT+CWJAP="<ssid>","<password>"`

### 断开连接
```bash
./bflbwifictrl disconnect
```
→ `AT+CWQAP`

### 扫描 AP
```bash
./bflbwifictrl scan
```
→ `AT+CWLAP`

### 查询状态
```bash
./bflbwifictrl status
```
→ `AT+CWJAP?`

### 查询版本
```bash
./bflbwifictrl version
```
→ `AT+GMR`

### 重启模块
```bash
./bflbwifictrl reboot
```
→ `AT+RST`

### 启动 SoftAP
```bash
# Open 网络
./bflbwifictrl start_ap <ssid>

# 加密网络
./bflbwifictrl start_ap <ssid> <password>
```
→ 按顺序执行以下命令：
```
AT+CWMODE=2
AT+CIPAP="192.168.4.1","192.168.4.1","255.255.255.0"
AT+CWDHCPS=1,3,"192.168.4.50","192.168.4.200"
AT+CWSAP="<ssid>","<password>",11,4,4,0
```
**说明**：
- 默认通道：11
- 默认加密：WPA2
- AP IP：192.168.4.1
- DHCP 范围：192.168.4.50 - 192.168.4.200

### 停止 SoftAP
```bash
./bflbwifictrl stop_ap
```
→ `AT+CWMODE=0` (切换到 IDLE 模式)

### OTA 固件升级
```bash
./bflbwifictrl ota <firmware.bin>
```
```
AT+OTASTART?
AT+OTASTART=1
循环发送:
  AT+OTASEND=512
  [512字节原始二进制数据]
  等待 "SEND OK"
AT+OTAFIN
```

**注意事项**：
- OTA 期间接收线程暂停，TTY 被独占访问
- OTA 期间其他命令将被拒绝（返回错误码 -12）
- OTA 完成后模块会自动重启
- 进度每发送一个数据块（512 字节）打印一次
- OTA 失败会自动恢复状态

---

## 返回值

| 结果 | 说明 |
|------|------|
| `OK` | 成功 |
| `ERROR <code> <msg>` | 失败，见错误码 |

---

## 错误码

| 代码 | 说明 |
|------|------|
| 0 | 成功 |
| -1 | 一般错误 |
| -2 | TTY 错误 |
| -3 | 超时 |
| -4 | 参数无效 |
| -7 | 密码错误 |
| -8 | 找不到 AP |
| -12 | 设备忙碌（OTA 进行中） |
