# wifi_eth_bridge

## Commands

After boot the example attaches a three-address Ethernet bridge. The default role is AP and WDS enable is 0. `CONFIG_WIFI_MAT` defaults on with `CONFIG_WIFI_WDS` and translates only in that three-address state. Set `CONFIG_WIFI_MAT=n` to omit translation. On the STA board, select the role before starting Wi-Fi.

```text
wifi_wds config ap
wifi_wds config sta
wifi_wds config ap auto
wifi_wds config sta auto
wifi_wds config ap <peer-mac>
wifi_wds config sta <peer-mac>
wifi_wds status
wifi_wds release
```

`config <ap|sta>` keeps three-address mode. `auto` or a peer MAC enables four-address WDS.

For a four-address test, start the AP on channel 149 at 80 MHz. `-b` values 0, 1, and 2 select 20, 40, and 80 MHz. Omitting `-b` leaves the native command at 20 MHz. Pass `-g 0 -d 0`.

```text
wifi_ap_start -s WDS_WPA2 -k bridge-wpa2 -a WPA2 -c 149 -b 2 -g 0 -d 0
wifi_ap_stop
```

The STA must pass `-D`. The WPA2 example below also passes `-2`. Other authentication modes keep the existing Wi-Fi command arguments. For a static peer, add `-b` with the same MAC given to `wifi_wds config`. Auto mode does not add its own authentication or cipher restriction. An open network does not use a password.

```text
wifi_sta_connect -2 -D WDS_WPA2 bridge-wpa2
wifi_sta_disconnect
```

Open-network four-address auto example. On the AP board:

```text
wifi_wds config ap auto
wifi_ap_start -s WDS_OPEN -a OPEN -c 149 -b 2 -g 0 -d 0
```

On the STA board, omit the password and `-2`:

```text
wifi_wds config sta auto
wifi_sta_connect -D WDS_OPEN
wifi_wds status
```

WDS still checks the Multi-AP backhaul capability, the peer identity, and the authorized port. An encrypted network follows the existing Wi-Fi authentication flow. The available authentication and cipher types depend on the stack and the build configuration.

`CONFIG_MACSW_SELECT=bridge` keeps `CONFIG_WIFI_WDS` at its default y. Writing n or `# CONFIG_WIFI_WDS is not set` does not turn the bridge library off. Delete `CONFIG_WIFI_ETH_BRIDGE_4ADDR`; a value of y fails as an undefined symbol, while `=n` and `# is not set` are ignored. Three-address is the boot state. Four-address starts only after `wifi_wds config <ap|sta> auto` or a peer MAC. `WIFI_MAT` translates MAC addresses while four-address WDS is off. Four-address mode does not translate them. A three-address STA bridge needs MAT. `CONFIG_WIFI_MAT=n` leaves translation out, and the AP drops a unicast whose destination is not an associated station.

`wifi_wds config` creates this bridge through netifd. Interface names come from the current interface. Do not create a second bridge with `netifd bridge create`.

`running` becomes 1 after the AP has started or the STA has associated. Stop Wi-Fi before changing roles. Do not use `wifi_eth_bridge` or `wifi_wds_diag`.

## Support CHIP

| CHIP        | Remark |
|:-----------:|:------:|
| bl616/bl618 |        |

## Compile

- bl616/bl618

```text
make
```

## Flash

```text
make flash COMX=xxx
```
