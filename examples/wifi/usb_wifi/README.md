# USB WiFi case

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

## How USB WiFi STA test

When the device is powered on, it will virtualize an ACM serial port and an enxxxx Ethernet network interface.

Observe the output from the serial port.


```bash
cat /dev/ttyACM2
```

Open another terminal and enter the following commands to control the WiFi network interface card. Configure it in STA mode.

```bash
echo -ne "AT+CWMODE=1\r\n" > /dev/ttyACM2
```

Disable the DHCP function of the network interface card, and let the HOST side handle DHCP afterward.

```bash
echo -ne "AT+CWDHCP=0,1\r\n" > /dev/ttyACM2
```

We use AT commands to control the network interface card to connect to a specified Access Point (AP).

```bash
echo -ne "AT+CWJAP=\"8E88\",\"\"\r\n" > /dev/ttyACM2
```

Assuming the current network interface card is named enxc4cc37a06d46, enter the following on the HOST side:

```bash
sudo dhclient enxc4cc37a06d46
```

Obtain the IP address of the network interface card.

## How USB WiFi AP test

- soc

```bash
echo -ne "AT+CWMODE=2\r\n" > /dev/ttyACM1
echo -ne "AT+CWDHCP=0,2\r\n" > /dev/ttyACM1
echo -ne "AT+CWSAP=\"zephyr_ap\",\"12345678\",1,2,3,0\r\n" > /dev/ttyACM1
```

- pc

```bash
sudo apt install dhcpd
sudo vim /etc/udhcpd.conf
sudo ip link set enx18b905de866a up
sudo ip addr add 192.168.0.1/24 dev enx18b905de866a
sudo udhcpd -f /etc/udhcpd.conf
```

