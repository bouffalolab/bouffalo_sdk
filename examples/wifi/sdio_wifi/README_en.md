# sdio_wifi


## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |

## Device Compile


- BL616/BL618

```
# Use the compilation script make; the default is ldo 1.1v mode, and lpfw does not include logs:
make
# Note: If you modify the script parameters, you need to execute make clean first to clear them before re-executing.
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```

## Memory

| Start Address   | Size（bytes）  | Description                                                         |      |
| ---------- | -------------- | ------------------------------------------------------------ | ---- |
| 0x23010000 | 0x16800（90K） | WiFi (bss, common, data, etc.). If CONFIG_SDIO_HIGH_PERFORMANCE is defined,more RAM will be consumed for sdio tx to improve sdio tx throughput. |      |
| 0x23026800 | 0x9800（38K）  | 	Shared by LP firmware and sdio tx buffer                          |      |
| 0x23030000 | 0x8000（32K）  | Used as EM area for Bluetooth if Bluetooth is initialized                   |      |

## Host Compile

Execute the ./bl_install command in the host (such as Raspberry Pi) SDK directory (the host SDK is located in the host_linux directory under components/net/netbus/host_router). 
If the compilation is successful, the log blctl version 1.0 will appear.
## Host Control Program

After BL616/8 and the host are connected via SDIO, when BL616/8 is powered on, the log device ready will be printed. 
At this point, the host and the device have completed the handshake successfully, and you can use blctl to send control commands. Enter the userspace directory:

（1）Connect to the router

```basic
blctl connect_ap <ssid> [password]
```

Among them, SSID is a mandatory parameter, and password is optional. For unencrypted APs, the password parameter is not required.

（2） Disconnect from the router

```basic
blctl disconnect_ap
```

（3）Get the connection status with the router

```basic
blctl get_link_status
```

（4）Request the device to perform a Wi-Fi scan

```plain
blctl wifi_scan
```

After executing this command, you need to use the wifi_scan_results command to obtain the scan results. 
The recommended sequence of operations is: wifi_scan → sleep 6s → wifi_scan_results. Note that the scan results are valid for 15 seconds.

（5）Obtain Wi-Fi scan results

```plain
blctl wifi_scan_results
```

（6）Start AP

```plain
blctl start_ap <ssid> [password]
```

The SSID parameter specifies the SSID of the AP to be started. To start an open AP, do not specify the password parameter. 
To start a secured AP, the password parameter must be specified, which should be 8-63 common characters in length.
This transparent transmission solution does not support simultaneous AP/STA operation, only time-division multiplexing between STA and AP modes. 
Therefore, users must ensure the following:
Turn off the AP before connecting to a router
Disconnect from the router before starting the AP
If the Firmware DHCP Server is enabled, after starting the AP, you need to configure the Linux interface address:

```plain
ifconfig bleth0 192.168.169.1
```

（7）Stop AP

```plain
blctl stop_ap
```

（8）OTA

```plain
blctl ota <path to ota bin>
```

The only parameter required by this command is the OTA bin. The OTA bin is generated during the compilation of BL616, for example:

sdio_wifi_bl616.bin.ota (uncompressed OTA binary)
sdio_wifi_bl616.xz.ota (compressed OTA binary)
These files are located under examples/wifi/sdio_wifi/build/build_out directory.

（9）User-defined command demo

```plain
blctl user_ext
```

This command demonstrates the transmission of user-defined commands between the host and the device. Users can add functions based on this (this command is not recommended for use).

```
blctl user_ext_rsp
```

This command demonstrates that the host actively sends commands to the device, and the device replies to the host. Users can add application functions based on this.

```
blctl user_ext_no_rsp
```

This command demonstrates that the host actively sends a command to the device, and the device prints out the content after receiving the command without replying to the host. Users can add application functions based on this.
The above three commands correspond to the three CMDs: BF1B_CMD_USER_EXT, BF1B_CMD_USER_EXT_RSP, and BF1B_CMD_USER_EXT_NO_RSP respectively. The first two commands in the demo have the same effect. 
It is recommended that customers use the latter two commands and can perform application expansion. All the above three commands are actively sent by the host to the device. 
If you want the device to actively send messages to the host, the device can use the rnms_user_send_extension function to send.
**PS：**
For host-side application expansion, please modify in userspace/msg_handlers.c.


For device-side application expansion, please modify in dev_net_mgmr/src/rnm_user_ext.c.
## **DHCP**

If Firmware is enabled to handle DHCP, the Host needs to start the blctld program to obtain the IP address information after connecting to the router. The command format is as follows:

```plain
sudo blctld /fullpath/to/sta.sh
```

Among them, sta.sh is a script executed by blctld after receiving information such as an IP address. Its default function is to set the network interface address. 
Users may need to configure the automatic loading of the driver and the startup of blctld at boot. 
Note that blctld must be started after the driver is loaded, and the script path should be a full path. (The above commands are executed by default in bl_install.)
## About Dual-Protocol Stack Traffic Offloading

There are two processing paths for traffic offloading: one is eth_input_hook(), and the other is the input hook in lwIP.

In eth_input_hook(), EAPOL (BL616) packets are filtered for local processing, which is necessary for the process of connecting to the router, so users should not modify this part. 
ARP packets are copied and input to both the device and host protocol stacks, and users should generally not modify this either. 
Users can add their own filtering logic to determine which packets are input to the device and which are input to the host.
The input hook in lwIP is mainly used to implement automatic offloading of input data packets. The implementation method is to first input packets received via Wi-Fi to lwIP; 
if lwIP has no corresponding connection, the packets are then input to the host. This way, users do not need to configure eth_input_hook() to distinguish traffic between the two ends through ports.
eth_input_hook() is executed first, followed by the input hook in lwIP. Distinguishing traffic between the two ends through ports in eth_input_hook() may bring some performance improvements.
It should be avoided that eth_input_hook() decides to input packets to both ends, and then the hook in lwIP decides to input them to the host. In such cases, the device firmware may crash. 
However, this situation will not occur in general application scenarios.
The main risk of traffic offloading is the possibility of port conflicts between socket applications on both ends:
If both ends act as clients and connect to the same server (IP:PORT) using the same port, the input packets of the connection will be sent to the device.
If both ends act as servers and bind to the same port, the input packets will be sent to the device (i.e., the embedded side has priority).
If the servers (IP:PORT) connected by the two ends are different, there will be no problem.
Sockets on both ends can avoid this risk by binding to different ports.