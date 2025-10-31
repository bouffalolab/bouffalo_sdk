# WiFi AP STA bridge

## Support CHIP

|      CHIP        | Remark |
|:----------------:|:------:|
|BL616/BL618       |        |

## Compile

- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```

## Flash

```bash
make flash COMX=xxx ## xxx is your com name
```

## AP STA Bridge Topology

```             
     ________________            _________________            ___________
    |                |          |                 |          |           |
    | End Device STA | ........ | AP--bridge--STA | ........ | AP Router |
    |________________|          |_________________|          |___________|
```

### AP STA Bridge Test

#### Connect Bridge STA to the Router
```bash
wifi_sta_connect -D {SSID of Router} {Password of the Router}

-D - do not start DHCP for the STA interface
```

#### Start Bridge AP

```bash
wifi_ap_start -s tiny_ap -d 0 -c 6

where
    -s tiny_ap  -   the SSID of bridge AP.
    -d 0        -   do not start dhcpd for this AP. 
    -c 6        -   the channel this AP is working, which should be the same with that of STA interface.
```

#### Create A Bridge Interface

```bash
netifd bridge create -b br3
```

#### Add STA And AP Interface To The Bridge

```bash
netifd bridge add -b br3 -i wl1
netifd bridge add -b br3 -i wl2
```

Now use an end device, say, your phone, to connect AP tiny_ap, once the WiFi connection is established, the end device will get a network address from the router soon, and now you can connect the end device to the Internet. During the whole process, the transparent bridge forwards the traffic between its STA interface and AP interface.

### Debug And Statistics
Type the following command to check the debug and statistics of MAC Address Translation.
```bash
mat
```

### Note
Bridge STA can disconnect itself from the router AP for various reasons, the end devices will lost connection to the Internet either. In such case, we need to re-connect the bridge STA to target router again.
Assure bridge STA and AP interface work in the same frequency channel.
