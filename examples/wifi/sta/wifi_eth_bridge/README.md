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
| bl618dg     | Two EMAC ports on the AP core |

## Compile

- bl616/bl618

```text
make
```

## Flash

```text
make flash COMX=xxx
```

### Four-address STA bridge (BL618DG)

Topology: downstream device -- Ethernet -- BL618DG STA -- upstream AP/router.
Configure four-address WDS before connecting the STA. The upstream AP must
support compatible four-address WDS; auto mode uses Multi-AP backhaul discovery.
MAT does not translate traffic while four-address WDS is enabled.

Build the AP-core firmware:

```bash
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap CONFIG_ROMAPI=n
```

On a peer running this example, configure and start the AP:

```bash
wifi_wds config ap auto
wifi_ap_start -s WDS_WPA2 -k bridge-wpa2 -a WPA2 -c 149 -b 2 -g 0 -d 0
```

On the STA board, configure the bridge first, then connect:

```bash
wifi_wds config sta auto
wifi_sta_connect -2 -D WDS_WPA2 bridge-wpa2
wifi_wds status
netifd status
```

For static peers, use `wifi_wds config sta <AP-BSSID>` and pass the same BSSID
to `wifi_sta_connect -2 -D -b <AP-BSSID> <ssid> <password>`. Configure the AP's
peer as the STA MAC. Stop both Wi-Fi roles before changing WDS configuration.

`wifi_wds config` attaches Ethernet and the selected Wi-Fi interface to the
bridge. Do not create a second bridge or add the STA after association.
`-D` disables DHCP on the STA port; `-g 0 -d 0` disables the AP port's gateway
and DHCP service. If the board itself needs an IP address, start DHCP on the
bridge interface shown by `netifd status`, for example:

```bash
netifd dhcp start -i br4
```

Connect the downstream device to Ethernet. Renew its DHCP lease or assign an
unused static address in the upstream network, then test traffic in both
directions. The board itself has no IPv4 address until you configure the bridge
interface or start its DHCP client.

For diagnostics, run `wifi_wds status`, `netifd status`, `br_fdb`, and
`emac_status`. Check `addr4=1`, `active=1`, and `data_ready=1` with Ethernet
connected. `rx_4addr_seen=1` confirms a four-address frame has been received;
association alone does not prove end-to-end forwarding.

### BL618DG dual EMAC

The BL618DG defconfig sets `CONFIG_EMAC_DEV_COUNT=2`. Both physical ports use
`app/src/apeth_bridge/apeth_bridge_emac.c`; the BSP `lwip_emac_start.c` and
`lwip_netif_emac.c` paths must be disabled. BL616/BL618 and BL616CL keep one
EMAC port. BL618DG can also be built with `CONFIG_EMAC_DEV_COUNT=1` for a
single-PHY board.

Both ports share the following size/count macros in `apeth_bridge_emac.h`:

| Setting | EMAC0 | EMAC1 |
| --- | --- | --- |
| RX buffers (`LWIP_EMAC_RX_BUFF_CNT`) | 128 | 128 |
| RX capacity / aligned stride | 1524 / 1536 bytes | 1524 / 1536 bytes |
| RX storage / DMA address | PSRAM / non-cache alias | PSRAM / non-cache alias |
| TX buffers (`LWIP_EMAC_TX_BUFF_CNT`) | 24 | 24 |
| TX capacity / aligned stride | 1524 / 1536 bytes | 1524 / 1536 bytes |
| TX storage | Internal non-cache RAM | Internal non-cache RAM |

Every port has its own DMA descriptor rings, RX task, queues, PHY control,
link state, counters and buffer pool. Descriptors remain in internal non-cache
RAM. The combined RX allocation is 393216 bytes; TX allocation is 73728 bytes.
The BL618DG AP PSRAM partition is 4 MiB (`CONFIG_PSRAM_FOR_AP_SIZE=0x400000`).

GPIO initialization uses `board_emac_rmii_gpio_init(port)` and
`board_emac_mdio_gpio_init(port)` for ports 0 and 1. Each port scans its own
MDIO bus. The board must have two PHYs wired to those mappings in
`bsp/board/bl618dgdk/board_gpio.c`; if either PHY fails initialization, startup
reports the failed port and rolls back the already initialized interfaces.
EMAC1 receives a distinct locally administered MAC derived from the board MAC.

`wifi_wds config sta auto` (or the AP/static-peer variant) attaches both
Ethernet interfaces and the selected Wi-Fi interface to one bridge. Interface
names are assigned by lwIP; read them from `netifd status`. Either Ethernet
port being linked permits Wi-Fi forwarding. A link loss on either port flushes
the FDB, including when the other Ethernet port remains connected.

```text
emac_status       # Both ports
emac_status 0     # EMAC0
emac_status 1     # EMAC1
wifi_wds status
netifd status
br_fdb
```

`wifi_wds status` reports `ethernet_link_mask`: bit 0 is EMAC0, bit 1 is EMAC1
(0: neither, 1: EMAC0, 2: EMAC1, 3: both). `ethernet_link` remains a boolean
indicating that at least one Ethernet link is up.
