# README

# coex_wifi_ble

This is a Wi-Fi & BLE coexistence example.

## Support CHIP

|CHIP|Remark|
| :---: | :----: |
|BL602||

## Compile

- BL602

```
make CHIP=bl602 BOARD=bl602dk
```

## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```

## Test

* WiFi connect

```
wifi_sta_connect <ssid> <passwd>
```

* Enable coexistence mode

```
wifi_sta_ps_on
```

* Set the Wi-Fi duty cycle to 50%

```
wifi_sta_ps_set 50
```

* Enable BLE advertising

```
ble_start_adv 0 0 0x80 0x80
```

‍

‍
