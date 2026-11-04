# Wi-Fi Direct (P2P) Example

## Overview

This example provides Wi-Fi Direct (P2P) shell commands for peer discovery, connection setup, group management, and data-plane testing. It supports:

- P2P initialization, discovery, listen, and group operations on `wl2`.
- Group Owner (GO) and Group Client (GC) roles selected through GO Negotiation.
- PBC, device-generated PINs, fixed PINs, and PINs displayed by the peer.
- Autonomous GO, persistent groups, Provision Discovery, and Invitation.
- Autonomous GO SSIDs include the configured P2P device name when no explicit
  `p2p_ssid_postfix` is configured.
- P2P events for peer discovery, GO Negotiation, group formation, group removal, and IP assignment.
- Automatic GO configuration at `192.168.49.1/24` with a DHCP server, and automatic DHCP client startup for a GC.
- Automatic Keyram dumps after a GO or GC connection is established for encrypted-frame debugging.
- TCP and iperf commands for data-plane validation.

Run `p2p_init` before discovery, listen, connection, or group-management commands. `p2p_join` is the exception: it initializes P2P itself and performs discovery, peer selection, automatic connection, and completion polling as one operation. When `peer` or `name` identifies a target, discovery stops as soon as that target appears in the discovered-peer cache. It uses `P2P_CONNECT ... pbc auto`: an existing GO is joined as a GC; a peer that is not running a GO triggers GO Negotiation, so either device may become GO.

## Supported Platforms

| CHIP | BOARD | Notes |
| --- | --- | --- |
| BL616 | `bl616dk` | Build verified |
| BL618DG | `bl618dgdk` | AP core with PSRAM; build and autonomous-GO startup verified |

## Build

Run the following commands from the directory containing this README.

Build for BL616:

```bash
make CHIP=bl616 BOARD=bl616dk
```

Build for the BL618DG AP core:

```bash
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

When changing CHIP, BOARD, or CPU_ID, clean the previous build first:

```bash
make clean
```

## Flash

The SDK generates the flash configuration in the build directory during
configuration. The flash target uses that generated configuration; no
example-local INI file is required.

Replace `/dev/ttyUSB0` with the actual serial port.

BL616:

```bash
make flash CHIP=bl616 BOARD=bl616dk COMX=/dev/ttyUSB0 BAUDRATE=2000000
```

BL618DG AP core:

```bash
make flash CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap COMX=/dev/ttyUSB0 BAUDRATE=2000000
```

After flashing and resetting the board, wait for both events before running P2P commands:

```text
CODE_WIFI_ON_INIT_DONE
CODE_WIFI_ON_MGMR_DONE
```

## Command Conventions

- `channel` is an IEEE channel number such as `6` or `36`; `freq` is a frequency in MHz such as `2437` or `5180`.
- MAC arguments are P2P Device Addresses in `xx:xx:xx:xx:xx:xx` format. A P2P Device Address is used for discovery and connection setup and may differ from the P2P Interface Address assigned after group formation.
- A CLI return value of `0` only means that the supplicant accepted the command. It does not mean that the asynchronous P2P operation has completed successfully.
- Use `[APP] [EVT] [P2P]` events together with `p2p_status` to determine the final result.
- If `p2p_init` has no channel argument, it reuses the active STA channel when possible and otherwise uses channel 1.
- Before using 5 GHz, check the country code, peer capabilities, and regulatory restrictions. A DFS or NO_IR channel may be listed for the country but still reject P2P initiation. Channel 52 is a common example.
- The firmware help output is the authoritative reference for the command syntax implemented by the current build:

```text
p2p_help
p2p_help p2p_connect
```

## Quick Start

The following workflows use channel 6 and PBC.

### 1. Initialize P2P and Discover a Peer

```text
p2p_init 6
p2p_name bl-p2p-demo
p2p_find 15 freq=2437
p2p_peers discovered
p2p_peer <peer-device-address>
```

The first positional argument of `p2p_find` is the discovery timeout in seconds. `freq` restricts discovery to one frequency, and `dev_id` specifies a target P2P Device Address. For example:

```text
p2p_find 30 freq=5180 dev_id=02:11:22:33:44:55
```

`p2p_peers` prints all cached peers. Adding `discovered` limits the output to peers obtained through P2P discovery.

### 2. Start GO Negotiation from the Device

Keep the phone on its Wi-Fi Direct discovery screen, obtain its P2P Device Address, and run:

```text
p2p_connect <peer-device-address> pbc
```

Without `join`, the two devices use GO Negotiation to select their roles. Use `go_intent=0..15` to influence role selection; a larger value makes the local device more likely to become the GO:

```text
p2p_connect <peer-device-address> pbc go_intent=7 freq=2437
```

Wait for `GO_NEG_SUCCESS`, `GROUP_FORMATION_SUCCESS`, and `GROUP_STARTED`. If the local device becomes a GC, also wait for `GOT_IP`. Then run:

```text
p2p_status
```

A successful GO normally reports:

```text
p2p_state=GROUP_ACTIVE
wpa_state=COMPLETED
mode=P2P GO
```

A successful GC normally reports:

```text
p2p_state=GROUP_ACTIVE
wpa_state=COMPLETED
mode=P2P CLIENT
```

### 3. Accept a Connection Initiated by a Phone

Put the device into listen mode:

```text
p2p_init 6
p2p_listen 120 6
```

When the phone initiates a connection, the device reports `GO_NEG_REQUEST` with the peer address, GO intent, and WPS method. This example does not accept the request automatically. After the application approves the peer, authorize the request with the same WPS method. For a PBC request, run:

```text
p2p_connect <peer-device-address> pbc auth
```

To reject the request, do not authorize it. To cancel negotiation that is already in progress, run:

```text
p2p_cancel
```

### 4. Create an Autonomous GO

```text
p2p_init 36
p2p_name bl618-p2p
p2p_group_add freq=5180 ht40 vht he passphrase=12345678
```

For a newly created autonomous GO, `p2p_name` is appended to the standard
random prefix. The commands above create an SSID such as
`DIRECT-QQ-bl618-p2p`. An explicitly configured `p2p_ssid_postfix` remains
authoritative, and reinvoking `persistent=<id>` preserves the saved SSID.

After receiving `GROUP_STARTED`, run:

```text
p2p_status
p2p_wps_pbc
```

`p2p_wps_pbc` opens the PBC window. The phone must join while that window is active.

`passphrase=` accepts 8 to 63 printable characters directly. The equivalent
`passphrase_hex=` form remains available for values that need hexadecimal
transport. For example, `P9XKZRjI` is encoded as:

```text
p2p_group_add freq=2437 passphrase_hex=5039584b5a526a49
```

Runtime validation on BL618DG AP (`bl618dgdk`) confirmed that the plaintext
command is accepted, `GROUP_STARTED` reports GO mode at 5180 MHz, and
`p2p_status` reports `ssid=DIRECT-QQ-bl618-p2p`, `mode=P2P GO`, and
`wpa_state=COMPLETED`.

After the GO starts, its address is `192.168.49.1/24` and its DHCP pool is `192.168.49.2` through `192.168.49.100`.

### 5. Create and Reinvoke a Persistent Group

Persistent profiles are stored in the PSM partition and restored whenever
`p2p_init` recreates the P2P interface. Create a persistent GO and wait for
`GROUP_STARTED`:

```text
p2p_init 6
p2p_group_add persistent freq=2437
```

List the saved profile and note its network ID:

```text
list_networks
```

After removing the active group, reinvoke that profile by ID without generating
new credentials:

```text
p2p_group_remove
p2p_group_add persistent=<id> freq=2437
```

The same profile remains available after `p2p_stop`, P2P interface recreation,
or a device restart. After restarting, run `p2p_init` before `list_networks` or
reinvocation.

When this device is the saved GO, the peer Device Address is required and the
peer must already be in the supplicant peer cache. Discover the peer before
sending the Invitation:

```text
p2p_find 15 freq=2437 dev_id=<peer-device-address>
p2p_stop_find
p2p_invite persistent=<id> peer=<peer-device-address> freq=2437
```

When this device is the saved client and the saved peer is the GO, `peer` may
be omitted. A successful persistent group reports `[PERSISTENT]` in the
`GROUP_STARTED` event. `P2P-INVITATION-RESULT` follows standard supplicant
semantics and contains `status` plus an optional BSSID; peer, frequency, and
network ID are not part of that result event.

Delete an obsolete profile with:

```text
remove_network <id>
```

Deletion is written to PSM immediately. Erasing or replacing the PSM partition
removes all persistent profiles. The feature is controlled by
`CONFIG_WIFI_P2P_PERSISTENT_GROUP` and is enabled in this example's defconfig.

### 6. Join a Peer with Automatic Role Selection

When the peer is already a GO with PBC active, run:

```text
p2p_init 6
p2p_find 10 freq=2437
p2p_connect <go-device-address> pbc join freq=2437
p2p_status
```

Alternatively, use `p2p_join` to perform the complete sequence. Specify `peer` whenever possible to avoid selecting another discovered device:

```text
p2p_join peer=<peer-device-address> freq=2437 go_intent=7 find=60 wait=35
```

`find` and `wait` are in seconds and default to 60 and 35, respectively. When `peer` or `name` is specified, `p2p_join` checks the discovered-peer cache during discovery and issues `P2P_STOP_FIND` immediately after a match; without either target, it waits for the complete discovery period before selecting a peer. Specify either `freq` or `channel`. If both are present, they must identify the same channel. The peer must keep WPS PBC active during the connection attempt. If it is not a GO, the supplicant emits `P2P-FALLBACK-TO-GO-NEG` and starts GO Negotiation automatically.

`go_intent` is optional and accepts 0 through 15. It affects only GO Negotiation after the automatic connection falls back; it has no effect when joining a peer that is already a GO. Omitting it uses the supplicant-configured default, which is 7 in the current configuration. Higher values increase the local preference to become GO. A value of 15 requires the local device to become GO, and negotiation fails if both peers use 15.

When `peer` is present, it selects the target directly and `name` is ignored. With only `name=<substring>`, the command selects the first peer whose device name contains that string. If both `peer` and `name` are omitted, the command selects the first device in the discovery list. Device names may not be unique, and the first discovered device may not be the intended GO, so `peer=<go-device-address>` is preferred. `no_find` skips discovery and should only be used when the peer is already cached.

## PIN Connections

### Generate a PIN on the Device and Enter It on the Peer

```text
p2p_connect <peer-device-address> pin display
```

After the command is accepted, the generated PIN is printed as:

```text
p2p: generated_pin=<PIN>
```

### Use a Fixed PIN on the Device and Enter It on the Peer

```text
p2p_connect <peer-device-address> 12345670 display
```

### Display a PIN on the Peer and Enter It on the Device

```text
p2p_connect <peer-device-address> 12345670 keypad
```

`display` means that the PIN is provided by the local device and entered on the peer. `keypad` means that the PIN is provided by the peer and included in the local command. A PIN must contain 4 or 8 decimal digits. Four-digit PIN support depends on the peer; use a valid eight-digit WPS PIN for interoperability testing when possible.

## Command Reference

| Command | Arguments | Description |
| --- | --- | --- |
| `p2p_help` | `[command]` | Show all commands or detailed help for one command. |
| `p2p_init` | `[channel]` | Initialize the `wl2` P2P interface. With no channel, reuse the active STA channel or fall back to channel 1. |
| `p2p_channel` | `<channel>` | Set the operating, listen, and preferred channels. Not allowed during negotiation or while a group is active. |
| `p2p_config` | `<channel> [no_group_iface]` | Update the channel configuration after initialization. `no_group_iface` must currently be `1`. |
| `p2p_name` | `<device-name>` | Set the 1-to-32-byte P2P/WPS device name. New autonomous GO SSIDs include this name unless an explicit SSID postfix is configured. |
| `p2p_find` | `[timeout] [freq=<MHz>] [dev_id=<MAC>]` | Start discovery with optional duration, frequency, and target-device restrictions. |
| `p2p_stop_find` | None | Stop the current discovery or listen operation while preserving the interface and peer cache. |
| `p2p_listen` | `[seconds] [channel]` | Listen for connection requests on the selected channel. The default duration is 30 seconds. |
| `p2p_peers` | `[discovered]` | List cached P2P Device Addresses. `discovered` includes only peers obtained through P2P discovery. |
| `p2p_flush` | None | Stop discovery and clear peer and Provision Discovery caches without removing an active group. |
| `p2p_peer` | `<peer-mac>` | Print parsed information for one cached peer. |
| `p2p_peer_raw` | `<peer-mac>` | Print the raw `P2P_PEER` response from the supplicant for diagnostics. |
| `p2p_connect` | `<peer-mac> {pbc \| pin [display] \| <PIN> [display\|keypad]} [join\|auto\|auth] [go_intent=<0..15>] [freq=<MHz>]` | Start GO Negotiation, join an existing group, or authorize a peer request. `auto` joins a running GO or falls back to GO Negotiation. `join`, `auto`, and `auth` are mutually exclusive. |
| `p2p_cancel` | None | Cancel in-progress GO Negotiation, join, or invitation work without removing an active group. |
| `p2p_group_add` | `[persistent\|persistent=<id>] [freq=<MHz>] [max_oper_chwidth=<20\|40\|80>] [ht40] [vht] [he] [passphrase=<8..63 chars>] [passphrase_hex=<16..126 hex chars>]` | Create an autonomous or persistent GO, or reinvoke a saved profile by ID. Plaintext and decoded passphrases must contain 8 to 63 printable characters. |
| `list_networks` | None | List saved persistent profiles and their network IDs. Profiles restored from PSM become visible after `p2p_init`. |
| `remove_network` | `<id>` | Delete one saved persistent profile and immediately update PSM. |
| `p2p_group_remove` | None | Remove the active `wl2` group while preserving the initialized P2P interface. |
| `p2p_wps_pbc` | `[peer-mac]` | Open WPS PBC on the local GO, optionally restricted to one peer. |
| `p2p_prov_disc` | `<peer-mac> [pbc\|pin\|display\|keypad] [join\|auto]` | Start Provision Discovery. `join` and `auto` are mutually exclusive. |
| `p2p_invite` | `group=wl2 peer=<MAC> [go_dev_addr=<MAC>]` | Invite a cached peer to the active group. |
| `p2p_invite` | `persistent=<id> [peer=<MAC>] [freq=<MHz>] [pref=<MHz>] [max_oper_chwidth=<20\|40\|80>] [ht40] [vht] [he]` | Reinvoke a saved persistent group. `peer` is required when this device is the saved GO and may be omitted when the saved peer is GO. The specified peer must already be cached. Unsupported channel capabilities are rejected by the supplicant. |
| `p2p_join` | `[peer=<MAC>\|<MAC>] [name=<text>] [freq=<MHz>] [channel=<channel>] [go_intent=<0..15>] [find=<seconds>] [wait=<seconds>] [no_find]` | Automatically join a running GO or negotiate a GO/GC role with PBC. A requested `peer` or `name` stops discovery as soon as it is found; `go_intent` controls the local role preference only during GO Negotiation. |
| `p2p_stop` | None | Stop P2P completely, remove the active group, clear cached state, and release `wl2`. |
| `p2p_status` | None | Show the local P2P lifecycle state and the supplicant connection state. |

## Lifecycle and Stop Commands

The normal P2P lifecycle is:

```text
STOPPED -> IDLE -> FINDING/LISTENING -> NEGOTIATING -> GROUP_ACTIVE
```

The stop-related commands have different scopes:

| Command | Stop discovery/listen | Clear peer cache | Remove active group | Release `wl2` |
| --- | --- | --- | --- | --- |
| `p2p_stop_find` | Yes | No | No | No |
| `p2p_flush` | Yes | Yes | No | No |
| `p2p_cancel` | No | No | No | No |
| `p2p_group_remove` | No | No | Yes | No |
| `p2p_stop` | Yes | Yes | Yes | Yes |

## Data-Plane Validation

Before testing data transfer, verify that `p2p_status` reports `GROUP_ACTIVE` and `COMPLETED`.
This example uses a conservative lwIP profile for protocol validation. For peak
TCP/UDP throughput measurement, build `examples/wifi/p2p/wifi_p2p_high_perf`
instead; it reuses this application code with the `wifi_tcp_high_perf` TCP
profile.

Start a TCP echo server on the device:

```text
wifi_tcp_echo_test 3365
```

Connect the peer to the device P2P IP address on port `3365`, send data, and verify the echo. A local GO uses `192.168.49.1`. For a local GC, obtain its address from the `GOT_IP` log or the peer GO's DHCP lease table.

Run the device as a TCP client:

```text
wifi_tcp_test <peer-ip> 3365
```

Show the available iperf arguments:

```text
iperf -h
```

UDP server example:

```text
iperf -s -u -i 1
```

UDP client example:

```text
iperf -c <peer-ip> -u -b 50M -t 30 -i 1
```

## Events and Logs

Monitor these events during P2P operations:

| Event | Meaning |
| --- | --- |
| `DEVICE_FOUND` / `DEVICE_LOST` | A peer was added to or removed from the discovery cache. |
| `FIND_STOPPED` | Discovery or listen stopped, including natural expiration. |
| `GO_NEG_REQUEST` | A peer requested GO Negotiation; the application must decide whether to authorize it. |
| `GO_NEG_SUCCESS` / `GO_NEG_FAILURE` | GO Negotiation result. |
| `GROUP_FORMATION_SUCCESS` / `GROUP_FORMATION_FAILURE` | Group formation result. |
| `GROUP_STARTED` | The group is active; the log includes the interface, role, SSID, and frequency. |
| `GOT_IP` | The GC obtained an address through DHCP. |
| `GROUP_REMOVED` | The group was removed; the lifecycle should return to `IDLE`. |
| `PROV_DISC_*` | Provision Discovery PIN, PBC, or failure events. |
| `INVITATION_*` | Invitation receipt, acceptance, or result events. |

Keyram dumps contain session keys. Use them only for controlled testing and issue analysis. Do not store or transmit them in public logs or locations without appropriate access control.

## Troubleshooting

### `p2p_init` Returns `not supported`

First verify that `CODE_WIFI_ON_MGMR_DONE` has been received. Then check whether the channel satisfies the country-code rules, hardware capabilities, peer capabilities, and P2P initiation restrictions. Do not assume that every channel in the country channel list is valid for initiation. For routine interoperability tests, prefer non-DFS channels such as 1, 6, 11, 36, 40, 44, or 48.

### `p2p_find` Cannot Find a Phone

- Keep the phone on its Wi-Fi Direct discovery screen.
- Verify that both devices use compatible channels and bands. Use `freq=<MHz>` to restrict discovery when needed.
- Run `p2p_stop_find` and start discovery again. Use `p2p_flush` if stale cached state is suspected.
- Compare `p2p_peers` with `p2p_peers discovered` to distinguish all cached peers from peers obtained through discovery.

### `p2p_connect` Returns 0 but No Group Is Created

A return value of 0 only means that the supplicant accepted the command. Continue monitoring `GO_NEG_*`, `GROUP_FORMATION_*`, and `GROUP_STARTED`, and use `p2p_status` to inspect the local lifecycle and `wpa_state`. The asynchronous operation may fail if the peer does not confirm within the WPS window, the PIN methods do not match, or the frequencies are incompatible.

### The State Remains `NEGOTIATING`

Run `p2p_status` and compare the local lifecycle with the supplicant state. If group formation has not completed, run `p2p_cancel`. If the state still does not recover, run `p2p_stop` and initialize P2P again. An unauthorized `GO_NEG_REQUEST` or a failed negotiation should return the lifecycle to `IDLE`; preserve the complete serial event log if it does not.

### A Phone-Initiated Connection Does Not Continue Automatically

This is expected. The example reports `GO_NEG_REQUEST` but does not authorize an unknown peer automatically. After application-level approval, call `p2p_connect` with `auth`. The PBC, display, or keypad method must match the WPS method reported by the event.

### STA and P2P Channels Conflict

On a single-radio device, STA and P2P should use the same channel when they operate concurrently. If `p2p_init` has no `channel` and `p2p_connect` has no `freq`, the example attempts to reuse the active STA channel. Explicitly selecting another channel may prevent P2P from using the requested channel or forming a group.

## Cleanup

Remove only the active group and keep the P2P interface initialized:

```text
p2p_group_remove
```

Stop P2P completely and release its resources:

```text
p2p_stop
```
