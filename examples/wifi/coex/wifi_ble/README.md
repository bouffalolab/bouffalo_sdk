# README

# coex_wifi_ble

This is a Wi-Fi & BLE coexistence example.

## Support CHIP

| CHIP    | Remark |
|:-------:|:------:|
| BL602   |        |
| BL616   |        |
| BL616CL |        |
| BL618DG |        |

## Compile

- BL602

```
make CHIP=bl602 BOARD=bl602dk
```

- BL616

```
make CHIP=bl616 BOARD=bl616dk
```

- BL616CL

```
make CHIP=bl616cl BOARD=bl616cldk
```

- BL618DG

```
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
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

* Start coexistence using PS-PTA

```
wifi_coex_start
```

* Set the Wi-Fi duty cycle to 50%

```
wifi_coex_duty_set 50
```

Use `wifi_coex_status` to query the activation and `wifi_coex_stop` to stop it.
The `wifi_sta_ps_on`, `wifi_sta_ps_off`, and `wifi_sta_ps_set` commands control
ordinary STA power save; they do not activate coexistence.

* Enable BLE advertising

```
ble_start_adv 0 0 0x80 0x80
```

‍

‍
