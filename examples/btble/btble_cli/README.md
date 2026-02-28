# btble_cli

## Support CHIP

| CHIP                      | Remark |
|:-------------------------:|:------:|
| BL602/BL616/BL618/BL618DG |        |

## Compile

- BL602 (BLE only)

```
make CHIP=bl602 BOARD=bl602dk
```
- BL616/BL618

```
make CHIP=bl616 BOARD=bl616dk
```
- BL618DG

```
make CHIP=bl618dg BOARD=bl618dgdk CPU_ID=ap
```

- BL616CL

```
make CHIP=bl616cl BOARD=bl616cldk
```
## Flash

```
make flash CHIP=chip_name COMX=xxx # xxx is your com name
```
## BTBLE CLI CMD guide

please refer to `docs/en/api_reference/ble.rst`
## CLI Commands

### BLE Host Commands

#### Device Initialization
- `ble_init` - Initialize BLE stack
- `ble_enable` - Enable BLE functionality
- `ble_disable` - Disable BLE functionality
- `ble_set_chan_map` - Set channel map
- `ble_set_device_name` - Set device name
- `ble_get_device_name` - Get device name
- `ble_read_local_address` - Read local address
- `ble_set_local_address` - Set local address

#### Scanning
- `ble_start_scan` - Start scanning
- `ble_stop_scan` - Stop scanning
- `ble_scan_filter_size` - Set scan filter size

#### Advertising
- `ble_start_adv` - Start advertising
- `ble_stop_adv` - Stop advertising
- `ble_set_adv_channel` - Set advertising channel
- `ble_start_multi_adv` - Start multi advertising
- `ble_stop_multi_adv` - Stop multi advertising

#### Connection Management
- `ble_connect` - Connect to remote device
- `ble_disconnect` - Disconnect from remote device
- `ble_select_conn` - Select specific connection
- `ble_auto_connect` - Auto connect
- `ble_whitelist_add` - Add to whitelist
- `ble_whitelist_rem` - Remove from whitelist
- `ble_whitelist_clear` - Clear whitelist

#### Connection Parameters
- `ble_conn_update` - Update connection parameters
- `ble_send_l2cap_conn_param_update_req` - Send L2CAP connection parameter update request

#### GATT Client Operations
- `ble_exchange_mtu` - Exchange MTU
- `ble_discover` - Discover services/characteristics
- `ble_read` - Read attribute
- `ble_write` - Write attribute
- `ble_write_without_rsp` - Write without response
- `ble_subscribe` - Subscribe to notifications/indications
- `ble_unsubscribe` - Unsubscribe from notifications/indications

#### Security
- `ble_security` - Start security procedure
- `ble_auth` - Register authentication callback
- `ble_auth_cancel` - Cancel authentication
- `ble_auth_passkey_confirm` - Confirm passkey
- `ble_auth_pairing_confirm` - Confirm pairing
- `ble_auth_passkey` - Input passkey

#### Data Operations
- `ble_set_data_len` - Set data length
- `ble_read_rssi` - Read RSSI
- `ble_get_all_conn_info` - Get all connection info

#### Throughput Testing
- `ble_tp_start` - Start throughput testing

#### PHY Configuration
- `ble_set_2M_phy` - Set 2M PHY
- `ble_set_coded_phy` - Set coded PHY
- `ble_set_default_phy` - Set default PHY

#### Power Control
- `ble_set_tx_pwr` - Set TX power

#### L2CAP Operations
- `ble_l2cap_send_test_data` - Send L2CAP test data
- `ble_l2cap_disconnect` - L2CAP disconnect
- `ble_register_test_psm` - Register test PSM
- `ble_connect_test_psm` - Connect test PSM

#### HID Operations
- `ble_hog_srv_notify` - HOG service notify

#### LE Testing
- `ble_le_tx_test` - LE TX test
- `ble_le_rx_test` - LE RX test
- `ble_le_enh_tx_test` - LE enhanced TX test
- `ble_le_enh_rx_test` - LE enhanced RX test
- `ble_test_end` - End LE test

#### Dynamic Service Operations
- `ble_add_spp_service` - Add SPP service
- `ble_del_spp_service` - Delete SPP service
- `ble_get_svc_info` - Get service info
- `ble_get_svc_char` - Get service characteristic
- `ble_get_svc_desp` - Get service descriptor

### Bluetooth Mesh Commands

#### Mesh Initialization
- `blemesh_init` - Initialize BLE mesh
- `blemesh_pvnr_init` - Initialize mesh provisioner

#### Device Information
- `blemesh_addr_get` - Get mesh address
- `blemesh_set_dev_uuid` - Set device UUID

#### Provisioning
- `blemesh_pb` - Enable/disable provisioning
- `blemesh_provision_adv` - Provision over advertising
- `blemesh_beacon_listen` - Listen for beacons
- `blemesh_provision` - Provision a device
- `blemesh_input_num` - Input number during provisioning
- `blemesh_input_str` - Input string during provisioning

#### Mesh Network Operations
- `blemesh_net_send` - Send network packet
- `blemesh_seg_send` - Send segmented message
- `blemesh_rpl_clr` - Clear replay protection list
- `blemesh_ivu_test` - IV update test mode
- `blemesh_iv_update` - IV update procedure
- `blemesh_reset` - Reset mesh state

#### Low Power Node
- `blemesh_lpn_set` - Enable/disable low power node
- `blemesh_lpn_poll` - LPN poll
- `blemesh_lpn_clear_friend_send` - Clear friend send
- `blemesh_lpn_friend_req` - Friend request
- `blemesh_lpn_group_add` - LPN group add
- `blemesh_lpn_group_del` - LPN group delete

#### Configuration Database
- `blemesh_cdb_create` - Create configuration database
- `blemesh_cdb_clear` - Clear configuration database
- `blemesh_cdb_show` - Show configuration database
- `blemesh_cdb_node_add` - Add node to CDB
- `blemesh_cdb_node_del` - Delete node from CDB
- `blemesh_cdb_subnet_add` - Add subnet to CDB
- `blemesh_cdb_subnet_del` - Delete subnet from CDB
- `blemesh_cdb_app_key_add` - Add app key to CDB
- `blemesh_cdb_app_key_del` - Delete app key from CDB

#### Configuration Model Operations
- `blemesh_net_key_add` - Add network key
- `blemesh_net_key_update` - Update network key
- `blemesh_net_key_get` - Get network key
- `blemesh_net_key_del` - Delete network key
- `blemesh_app_key_add` - Add application key
- `blemesh_app_key_update` - Update application key
- `blemesh_app_key_get` - Get application key
- `blemesh_app_key_del` - Delete application key
- `blemesh_kr_update` - Key refresh update
- `blemesh_mod_app_bind` - Bind model to app key
- `blemesh_mod_app_unbind` - Unbind model from app key
- `blemesh_mod_app_get` - Get model app bindings
- `blemesh_mod_sub_add` - Add model subscription
- `blemesh_mod_sub_ow` - Overwrite model subscription
- `blemesh_mod_sub_del` - Delete model subscription
- `blemesh_mod_sub_del_all` - Delete all model subscriptions
- `blemesh_mod_sub_add_va` - Add virtual address subscription
- `blemesh_mod_sub_ow_va` - Overwrite virtual address subscription
- `blemesh_mod_sub_del_va` - Delete virtual address subscription
- `blemesh_mod_sub_get` - Get model subscriptions
- `blemesh_mod_pub` - Set model publication
- `blemesh_mod_pub_va` - Set model publication to virtual address

#### Heartbeat Operations
- `blemesh_hb_sub` - Heartbeat subscription
- `blemesh_hb_pub` - Heartbeat publication

#### Configuration Server Operations
- `blemesh_krp_get` - Get key refresh phase
- `blemesh_krp_set` - Set key refresh phase
- `blemesh_cfg_bcn_get` - Get beacon configuration
- `blemesh_cfg_bcn_set` - Set beacon configuration
- `blemesh_cfg_dttl_get` - Get default TTL
- `blemesh_cfg_dttl_set` - Set default TTL
- `blemesh_cfg_gpxy_get` - Get GATT proxy state
- `blemesh_cfg_gpxy_set` - Set GATT proxy state
- `blemesh_friend` - Friend configuration
- `blemesh_relay` - Relay configuration
- `blemesh_node_identify` - Node identification
- `blemesh_node_reset` - Node reset
- `blemesh_network_trans` - Network transmit configuration
- `blemesh_lpn_timeout_get` - Get LPN timeout

#### Health Model Operations
- `blemesh_clhm_fault` - Health fault operations
- `blemesh_clhm_period` - Health period operations
- `blemesh_clhm_ats` - Health attention timer operations

#### Node Configuration
- `blemesh_get_comp` - Get composition data
- `blemesh_set_dst` - Set destination address
- `blemesh_node_cfg` - Configure node

#### Generic Model Client
- `blemesh_gen_oo_cli` - Generic onoff client operations

#### Light Model Client
- `blemesh_light_lgn_cli` - Light lightness client
- `blemesh_light_ctl_cli` - Light CTL client
- `blemesh_light_hsl_cli` - Light HSL client

#### Vendor Model Client
- `blemesh_vendor_cli` - Vendor model client operations

#### GATT Proxy
- `blemesh_proxy_identity` - Proxy identity operations

#### Mesh Synchronization
- `blemesh_sync_start` - Start mesh synchronization
- `blemesh_sync_stop` - Stop mesh synchronization

## Device Roles

### Central Role
- Scans for peripheral devices
- Initiates connections to peripherals
- Performs GATT discovery operations
- Can act as GATT client
- Supports connection parameter management

### Peripheral Role
- Advertises presence and services
- Accepts incoming connections from centrals
- Provides GATT server functionality
- Can act as GATT server with multiple characteristics
- Supports read/write/notify/indicate operations

### Mesh Role
- Node mode: Participates in mesh network as provisioned node
- Provisioner mode: Provisions and configures other nodes
- Supports both configuration and client model operations
- Handles network key management and security

## Usage Examples

### Basic BLE Operations
```bash
# Initialize and enable BLE
ble_init
ble_enable

# Set device name
ble_set_device_name "MyDevice"

# Start scanning
ble_start_scan 1 1 0080 0050

# Connect to device
ble_connect 0 112233AABBCC

# Discover services
ble_discover 0 1800 0001 0001 ffff

# Read characteristic
ble_read 0001 0000
```

### Mesh Network Operations
```bash
# Initialize mesh
blemesh_init

# Set device UUID
blemesh_set_dev_uuid "00112233445566778899AABBCCDD"

# Start provisioning
blemesh_pb 1 1

# Configure network key
blemesh_net_key_add 0000 0001 0000 0001

# Configure application key
blemesh_app_key_add 0000 0001 0000 0001 0001 00112233445566778899AABBCCDD

# Bind model to application key
blemesh_mod_app_bind 0000 0001 0001 0000 1000
```

## Configuration Parameters

- **Device Configuration**: Name, address, UUID settings
- **Connection Parameters**: Interval, latency, timeout settings
- **Security Settings**: Authentication, encryption, key management
- **Network Configuration**: Mesh provisioning and key management
- **Model Configuration**: Service, characteristic, and publication settings
