# USB WiFi

**Note**: The USB WiFi example will be replaced by the nethub example. The usb_wifi only supports the network protocol stack running on the host side, while the device side does not run the tcpip protocol stack - the device only acts as a network card function. The nethub example supports the device to simultaneously run the network protocol stack, and supports a wider range of interfaces, including but not limited to SDIO, USB, and SPI.

## Supported Chips

|      Chip         | Remark |
|:----------------:|:------:|
|bl616/bl618/bl616l |        |

## Device Compilation and Flashing

```bash
make CHIP=<chip_name> BOARD=<board_name>
make flash CHIP=chip_name COMX=xxx # xxx is your serial port name
```

### bl616 Series

```bash
make CHIP=bl616 BOARD=bl616dk
make flash CHIP=bl616 COMX=/dev/ttyUSB0
```

### bl616l Series

```bash
make CHIP=bl616l BOARD=bl616ldk
make flash CHIP=bl616l COMX=/dev/ttyUSB0
```

## USB WiFi STA Testing

When the device powers on, it will create a virtual ACM serial port and an enxxxx Ethernet network interface.

**Note**: The actual ACM serial port number (e.g., /dev/ttyACM0, /dev/ttyACM1, /dev/ttyACM2) may vary depending on your system. Please check the actual device name on your system.

Observe the output from the serial port:

```bash
cat /dev/ttyACM2
```

Open another terminal and enter the following commands to control the WiFi network interface card. Configure it in STA mode:

```bash
echo -ne "AT+CWMODE=1\r\n" > /dev/ttyACM2
```

Disable the DHCP function of the network interface card, and let the HOST handle DHCP afterward:

```bash
echo -ne "AT+CWDHCP=0,1\r\n" > /dev/ttyACM2
```

Use AT commands to control the network interface card to connect to a specified Access Point (AP):

```bash
echo -ne "AT+CWJAP=\"8E88\",\"\"\r\n" > /dev/ttyACM2
```

Assuming the current network interface card is named enxc4cc37a06d46, enter the following on the HOST side:

```bash
sudo dhclient enxc4cc37a06d46
```

This will obtain the IP address of the network interface card.

## USB WiFi AP Testing

**Note**: In the following commands, the ACM serial port number and network interface name may vary depending on your system. Please adjust them according to your actual device names.

- On the device:

```bash
echo -ne "AT+CWMODE=2\r\n" > /dev/ttyACM1
echo -ne "AT+CWDHCP=0,2\r\n" > /dev/ttyACM1
echo -ne "AT+CWSAP=\"zephyr_ap\",\"12345678\",1,2,3,0\r\n" > /dev/ttyACM1
```

- On the host PC:

```bash
sudo apt install dhcpd
sudo vim /etc/udhcpd.conf
sudo ip link set enx18b905de866a up
sudo ip addr add 192.168.0.1/24 dev enx18b905de866a
sudo udhcpd -f /etc/udhcpd.conf
```

