# USB WiFi
**注意**：usb_wifi 后期会被 nethub 示例取代，usbwifi 仅支持网络协议栈运行在host侧，device侧不运行tcpip协议栈，device仅充当一个网卡的功能。
nethub示例支持deivice同时运行网络协议栈，以及支持更广泛的接口，包括但不限于 SDIO、USB 和 SPI。

## 支持的芯片

|      芯片         | 备注 |
|:----------------:|:----:|
|bl616/bl618/bl616l |      |

## 设备编译和烧录

```bash
make CHIP=<芯片名称> BOARD=<开发板名称>
make flash CHIP=chip_name COMX=xxx # xxx 是您的串口名称
```

### bl616 系列

```bash
make CHIP=bl616 BOARD=bl616dk
make flash CHIP=bl616 COMX=/dev/ttyUSB0
```

### bl616l 系列

```bash
make CHIP=bl616l BOARD=bl616ldk
make flash CHIP=bl616l COMX=/dev/ttyUSB0
```

## 如何进行 USB WiFi STA 测试

设备上电后，将会虚拟化一个 ACM 串口和一个 enxxxx 网络接口。

观察串口输出。

```bash
cat /dev/ttyACM2
```

打开另一个终端并输入以下命令来控制 WiFi 网络接口卡。将其配置为 STA 模式。

```bash
echo -ne "AT+CWMODE=1\r\n" > /dev/ttyACM2
```

禁用网络接口卡的 DHCP 功能，让 HOST 端后续处理 DHCP。

```bash
echo -ne "AT+CWDHCP=0,1\r\n" > /dev/ttyACM2
```

我们使用 AT 命令控制网络接口卡连接到指定的接入点（AP）。

```bash
echo -ne "AT+CWJAP=\"8E88\",\"\"\r\n" > /dev/ttyACM2
```

假设当前网络接口卡名为 enxc4cc37a06d46，在 HOST 端输入以下命令：

```bash
sudo dhclient enxc4cc37a06d46
```

获取网络接口卡的 IP 地址。

## 如何进行 USB WiFi AP 测试

- 在设备上：

```bash
echo -ne "AT+CWMODE=2\r\n" > /dev/ttyACM1
echo -ne "AT+CWDHCP=0,2\r\n" > /dev/ttyACM1
echo -ne "AT+CWSAP=\"zephyr_ap\",\"12345678\",1,2,3,0\r\n" > /dev/ttyACM1
```

- 在主机PC上：

```bash
sudo apt install dhcpd
sudo vim /etc/udhcpd.conf
sudo ip link set enx18b905de866a up
sudo ip addr add 192.168.0.1/24 dev enx18b905de866a
sudo udhcpd -f /etc/udhcpd.conf
```
