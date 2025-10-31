# sdio_wifi


## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |

## Device Compile


- BL616/BL618

```
# 使用编译脚本make, 默认为 ldo 1.1v 模式, lpfw 不带 log:
make
# 注意, 如果修改了脚本参数，需要先执行 make clean 清除后再重新执行。
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

## Host Compile

在host（如树梅派等） sdk目录下执行（host sdk在components/net/netbus/host_router的host_linux目录）**./bl_install**命令，编译成功会有**blctl version 1.0** log出现。

## Host控制程序

BL616/8和host使用sdio连接后，BL616/8上电，上电后会打印**device ready** log，此时host和device已经握手成功，便可使用blctl发送控制命令，进入userspace目录：

（1）连接路由器

```basic
blctl connect_ap <ssid> [password]
```

其中SSID为必须提供的参数，password可选。对于不加密的AP，不提供password参数。

（2） 断开和路由器的连接

```basic
blctl disconnect_ap
```

（3）获得和路由器的连接状态

```basic
blctl get_link_status
```

（4）请求device执行Wi-Fi扫描

```plain
blctl wifi_scan
```

此命令执行后需使用wifi_scan_results命令获得扫描结果。建议的操作顺序是：wifi_scan；sleep 6s; wifi_scan_results。注意扫描结果有15s的有效期。

（5）获得Wi-Fi扫描结果

```plain
blctl wifi_scan_results
```

（6）启动AP

```plain
blctl start_ap <ssid> [password]
```

其中SSID为要启动的AP的SSID。如要开启无加密的AP，则不指定password；如要开启加密的AP，则应当指定password，且password为8到63个常见字符。

本透传方案不支持AP/STA共存，只支持STA/AP的分时复用。因此用户应当确保在连接路由器前AP已被关闭；开启AP前确保已经和路由器断开连接。

如果启用了Firmware DHCP Server，在开启了AP后需配置Linux接口地址：

```plain
ifconfig bleth0 192.168.169.1
```

（7）关闭AP

```plain
blctl stop_ap
```

（8）OTA

```plain
blctl ota <path to ota bin>
```

此命令需要的唯一参数为OTA bin。OTA bin是BL616编译时产生的，如：examples/wifi/sdio_wifi/build/build_out下面的sdio_wifi_bl616.bin.ota和sdio_wifi_bl616.xz.ota，前者是非压缩的ota bin，后者是压缩的。

（9）用户自定义命令demo

```plain
blctl user_ext
```

此命令演示了host和device传送用户自定义命令。用户可基于此添加功能（该命令不推荐使用）。

```
blctl user_ext_rsp
```

此命令演示了host主动发送命令给device，同时device回复给host。用户可基于此添加应用功能。

```
blctl user_ext_no_rsp
```

此命令演示了host主动发送命令给device，device收到命令后将内容打印出来，不会回复host。用户可基于此添加应用功能。

以上3条命令分别对应BF1B_CMD_USER_EXT，BF1B_CMD_USER_EXT_RSP，BF1B_CMD_USER_EXT_NO_RSP这3个CMD，demo里面前两条命令效果是一样，这里建议客户使用后面两条命令，并且可以进行应用扩展。上述3条命令都是host主动发送给device的，如果希望device主动发送消息给host端，device端可以使用rnms_user_send_extension函数进行发送。

**PS：**
host端应用扩展请在userspace/msg_handlers.c中修改。

device端应用扩展请在dev_net_mgmr/src/rnm_user_ext.c中修改。

## **DHCP**

如果启用Firmware处理DHCP，Host需要启动blctld程序来取得连接路由器后的IP地址信息，命令格式如下：

```plain
sudo blctld /fullpath/to/sta.sh
```

其中sta.sh为blctld收到如IP地址信息后执行的脚本，默认功能是设置网络接口地址。用户可能需要设置开机自动加载驱动和启动blctld。注意blctld需在驱动加载后启动，脚本路径应为全路径。（bl_install里默认会执行上述命令）

## 关于双协议栈分流

分流有两条处理路径，一个是eth_input_hook()，另一个是lwip中的input hook。

eth_input_hook()中会把EAPOL(BL616）过滤到本地处理，这对于连接路由器的过程是必要的，因此用户不应修改；ARP包会复制一份，输入到device和host的协议栈，一般用户也不应修改。用户可以添加自己的过滤逻辑来决定哪些包输入到device，哪些输入到host。

lwip中的input hook主要为了实现input数据包的自动分流，实现的方法是先将Wi-Fi收到的包输入到lwip，如果lwip没有对应的连接，再输入给host。这样以来，用户可以不配置eth_input_hook()中通过端口区分两端的流量。

eth_input_hook()会先执行，然后lwip中的input hook才会执行。eth_input_hook()中通过端口区分两端的流量可能带来一些性能的改善。

应当避免出现eth_input_hook()决定将包输入到两端，然后lwip中的hook又决定输入到host的情形出现，这种情形下device固件可能crash。在一般的应用场景下，此种情形不会出现。

分流主要的风险在于两端socket应用可能会端口冲撞：如果两端作为client使用相同的端口连接同一个server（IP:PORT），则连接input的包会输入到device。如果两端作为server绑定到同一端口，则input的包会被输入到device，即embedded优先。如果两端连接的server（IP:PORT）不同，则没问题。两端socket可以通过绑定到不同的端口上来规避此风险。
