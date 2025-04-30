# wifi6

## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|bl616/bl618       |        |

## Compile

- bl616/bl618

```
make
```

## Flash

```
make flash COMX=xxx ## xxx is your com name
```

###  Start bridge server in BL616 shell

* Start the WiFi hotspot, configure the bridge-related services, and finally add the Ethernet and AP to the bridge.

```bash
# ap start without ipconfig and dhcp
wifi_ap_start -s WiFiSoc -k 12345678 -g 0 -d 0

# bridge create 
netifd bridge create -b br4
netifd up -i br4

# bridge add ethernet and ap
netifd bridge add -b br4 -i wl3
netifd bridge add -b br4 -i ex1
```

* Now you can connect to the network where the Ethernet is located by connecting your phone to the hotspot with the SSID <wifisoc>.

