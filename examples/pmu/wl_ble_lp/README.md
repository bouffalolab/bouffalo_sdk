## BLE Low Power：

### BLE ADV:

1.Reset board

2\.

ADV = 2S: Enter the command "ble\_start\_adv 0 0 0x0c80 0x0c80 " in the command line.

OR

ADV = 500MS: Enter the command "ble\_start\_adv 0 0 0x0320 0x0320" in the command line.

3.Enter the command "pm_enter_lp " in the command line.

### BLE CONNECTION:

1.Reset board

2.Enter ADV connectable mode: Enter the command "ble\_start\_adv 0 0 0x80 0x80 " in the command line.

3.Use Phone or other device connect IUT.

4. When connection established enter the command "pm_enter_lp " in the command line.

### BLE Benchmark：

The following test data reduced the power of BLE TX. The modification is as follows: Change "pwr\_table\_ble = <13> to pwr\_table\_ble = <0>;" in the file "bsp/board/qcc743\_lp\_dk/config/qcc74x\_factory\_params\_IoTKitA\_auto.dts".

| Mode        | uA   |
| :---------- | :--- |
| Deep sleep  | 50   |
| ADV = 2S    | 428  |
| ADV = 500ms | 1430 |



## WiFi Low Power：

### WIFI only Compile

Enter the build command line:

```bash
make CONFIG_BLE_ENABLE=0
```

### Test DTIM:

1.Reset board

2.Set dtim.

DTIM = 1
```bash
wifi_lp_set_dtim 1
```

DTIM = 3
```bash
wifi_lp_set_dtim 3
```

DTIM = 10
```bash
wifi_lp_set_dtim 10
```

2.Connect wifi, and got ip.

3.Enter lowpower

```bash
pm_enter_lp <rx_bcast>
```

Parameter:

rx_bcast = 0 — Disable receiving broadcast/multicast while in low power.
STA will not wake at DTIM for broadcast/multicast delivery; expect lower power.

rx_bcast = 1 — Enable receiving broadcast/multicast while in low power.
STA wakes at DTIM to receive buffered broadcast/multicast; expect higher power.

# Enter low power, ignore broadcast in LP
pm_enter_lp 0

# Enter low power, receive broadcast in LP
pm_enter_lp 1


### WiFi Benchmark：

| Mode       | uA  |
| :--------- | :-- |
| Deep sleep | 67  |
| DTIM = 1   | 810 |
| DTIM = 3   | 319 |
| DTIM = 10  | 154 |


## BLE And WiFi Dual Low Power
BLE and Wi-Fi dual low power mode, which can maintain Wi-Fi keep-alive while also maintaining BLE advertising and BLE connection.

### Test Step

1.Reset board
2.Connect wifi, and got ip.
3.Enter ADV connectable mode: Enter the command "ble\_start\_adv 0 0 0x320 0x320" in the command line.
4.DTIM = 10: Enter the command "tickless 10" in the command line.



## USB Host for MSC
Start usbh msc test with a specified time duration, during which perform write and read operation on a usb msc device.

Please be noticed, the USB Host feature in this demo is only for purpose of Wi-Fi performance verification when USB traffic is on. QCC743 cannot go to sleep once there is a USB device connected, so this demo only works in active mode.

### Build Command

```bash
make CONFIG_USBH_MSC=1
```

### Test Step

1.Make sure QCC743 is not in sleep mode during this test

2.Plug in a usb msc device

3.Initialize usb host: Enter the command "usbh\_init" in the command line.

4.Enter the test command with a time duration in seconds as argument (uint32_t type), e.g. "usbh\_msc\_test 60" in the command line.
