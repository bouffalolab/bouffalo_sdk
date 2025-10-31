# wifi_a2dpsource

## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |

## Device Compile

- BL616/BL618

```
make
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```

## Memory

| 起始地址   | 大小（bytes）  | 说明                                                         |      |
| ---------- | -------------- | ------------------------------------------------------------ | ---- |
| 0x23010000 | 0x16800（90K） | wifi （bss、common、data等），如果定义了CONFIG_SDIO_HIGH_PERFORMANCE，<br />则消耗更多的ram给sdio tx使用，提高sdio tx吞吐。 |      |
| 0x23026800 | 0x9800（38K）  | LP firmware和sdio tx buffer复用                              |      |
| 0x23030000 | 0x8000（32K）  | 如果初始化了蓝牙，则作为EM区域供蓝牙使用                     |      |

## Test

### WiFi命令参考
wifi_sta_connect [-b <bssid>] [-2/3] [-f 0/1/2] [-q] [-w] [-D] [-c <ch_idx>] <ssid> [password]

### bt命令参考

`bt [init|scan|connect <device>|disconnect|a2dpstart|a2dpstop]`

### 共存测试
```bash
# 连接WiFi
wifi_sta_connect ssid password

# BT 初始化，连接指定device
bt init
bt scan
bt connect D44BB67A9991

# A2DP启动和停止
bt a2dpstart
bt a2dpresume
bt a2dpstop

# BT 主动断开和反初始化
bt disconnect
bt deinit
```

