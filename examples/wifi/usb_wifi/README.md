# USB WiFi

**Note**: The usb_wifi example will be replaced by the nethub example in future releases. usb_wifi only supports the network protocol stack running on the host side, while the device side does not run the TCP/IP protocol stack and only functions as a network card.

The nethub example supports the device running the network protocol stack simultaneously, and supports a wider range of interfaces including but not limited to SDIO, USB, and SPI.

## Supported Chips

|       Chip        | Notes |
|:-----------------:|:-----:|
| bl616/bl618/bl616l |       |

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

## How to Perform USB WiFi STA Testing

After the device is powered on, it will virtualize an ACM serial port and an enxxxx network interface.

* Disable device DHCP and connect to the specified WiFi

    Save the following script as a "connect" script, modify "/dev/ttyACM1" as needed, then execute ./connect ssid password

    ```expect
    #!/usr/bin/expect
    # =========== user config start ===============
    set timeout 10
    set tty /dev/ttyACM1       # User modify as needed, use whatever is virtualized
    # =========== user config end ===============
    spawn -open [open $tty "r+"]
    if {$argc < 1} {puts "Usage: $argv0 <ssid> \[password\]"; exit}
    set ssid [lindex $argv 0]
    set password [expr {$argc >= 2 ? [lindex $argv 1] : ""}]
    set serial_id $spawn_id
    # set mode, disable dhcp
    send -i $serial_id "AT\r\n"
    send -i $serial_id "AT+CWMODE=1\r\n"
    expect -i $serial_id "OK"
    send -i $serial_id "AT+CWDHCP=0,1\r\n"
    expect -i $serial_id "OK"
    # connect
    send -i $serial_id "AT+CWJAP=\"$ssid\",\"$password\"\r\n"
    expect {
        -i $serial_id "+CW:CONNECTED" {
        }
        -i $serial_id timeout {
            puts "✗ Timeout, did not receive expected response"
        }
    }
    close -i $serial_id
    ```

* Enable network card dhcp client

    Assuming the current network interface card name is enxc4cc37a06d46, enter the following command on the HOST side:

    ```bash
    sudo dhclient enxc4cc37a06d46
    ```

    Get the IP address of the network interface card.

## How to Perform USB WiFi AP Testing

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

