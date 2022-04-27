请使用 10.2 版本以上 T-head 工具链编译该 demo

```bash

$ make APP=ble_peripheral BOARD=bl706_iot SUPPORT_FLOAT=y

The Ble OTA function is enable by add command "set(CONFIG_BT_OAD_SERVER 1) " , in the script file “ble_flags.cmake”

```