# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Component Overview

**wl80211** is the WiFi driver component for Bouffalo Lab SDK, providing IEEE 802.11 wireless LAN driver support for BL616/BL618 series chips. It implements a full WiFi stack with Station (STA) and Access Point (AP) modes, WPA/WPA2/WPA3 authentication, and lwIP TCP/IP stack integration.

## Build System

This component supports two build modes:

### SDK Build Mode (Default)
Uses precompiled libraries from `lib/` directory:
- `libwl80211_bl616.a` - BL616 chip
- `libwl80211_bl618dg.a` - BL618DGDK board
- `libwl80211_bl616cl.a` - BL616CL chip

The `bouffalo.mk` makefile defines the component integration:
- **Public includes**: `include/` (exported to project level)
- **Private includes**: `macsw/` (internal only)
- **Source files**: wl80211.c, timeout.c, rtos_al.c, cntrl.c, lwip.c, wifi_mgmr.c, macsw/*.c

### Source Build Mode
When `src/` directory is present, builds from source using `src/CMakeLists.txt`:
- Toolchain: RISC-V (`riscv64-unknown-elf-gcc`)
- Compiler flags: `-Os -march=rv32imafcpzpsfoperand_xtheade -mabi=ilp32f -mtune=e907`
- Strict compilation enabled: `-Werror -Wstrict-prototypes`
- Dependencies: PHYRF, MACSW, SUPPLICANT components

To build from source:
```bash
cd src && cmake -DBOARD=bl616dk . && make
```

## Architecture

### Layer Structure

```
+-------------------+     +------------------+
|   Application     |     |  wifi_mgmr API   |
|       Layer       |     |  (WiFi Manager)  |
+-------------------+     +------------------+
            |                       |
            v                       v
+----------------------------------------------+
|              wl80211 Driver API              |
|          (wl80211.h - control interface)     |
+----------------------------------------------+
                        |
    +-------------------+-------------------+
    |                                       |
+-----------------+               +------------------+
|  MACSW Layer    |               |  net80211 Layer  |
|  (macsw/)       |               |  (net80211/)     |
+-----------------+               +------------------+
    |                                       |
    +-------------------+-------------------+
                        |
            +---------------------------+
            |  bl_wpa_supplicant       |
            |  (WPA/WPA2/WPA3 auth)    |
            +---------------------------+
```

### Key Components

1. **wl80211.c** - Main driver initialization and control
2. **wifi_mgmr.c** - WiFi manager (high-level API wrapper)
3. **macsw/** - MAC software layer implementation
   - `ap.c` - AP mode operations
   - `connect.c` / `disconnect.c` - Connection handling
   - `scan.c` - Scanning functionality
   - `supplicant_api.c` - WPA supplicant integration
   - `rx.c` / `tx.c` - Data path
4. **cntrl.c** - Control command dispatcher (`wl80211_cntrl`)
5. **lwip.c** - lwIP TCP/IP stack integration
6. **wl80211_platform.c** - Platform abstraction (FreeRTOS)
7. **country.c** - Regulatory domain and country code support

## Public APIs

### WiFi Manager API (`wifi_mgmr.h`) - Recommended
High-level wrapper API, most commonly used by applications:

```c
// Initialization
void wifi_mgmr_init(void);

// STA operations
int wifi_mgmr_sta_connect(const wifi_mgmr_sta_connect_params_t *conn_param);
int wifi_mgmr_sta_disconnect(void);
int wifi_mgmr_sta_state_get(void);      // 1=connected, 0=disconnected
int wifi_mgmr_sta_get_bssid(uint8_t bssid[6]);
int wifi_mgmr_sta_rssi_get(int *rssi);
int wifi_mgmr_sta_channel_get(uint8_t *channel);

// Scan operations
int wifi_mgmr_sta_scan(const wifi_mgmr_scan_params_t *config);
uint32_t wifi_mgmr_sta_scanlist_nums_get(void);
int wifi_mgmr_scan_ap_all(void *env, void *arg, scan_item_cb_t cb);

// Power saving
void wifi_mgmr_sta_ps_enter(void);
void wifi_mgmr_sta_ps_exit(void);

// Country/regulatory
int wifi_mgmr_set_country_code(char *country_code);
```

### Driver API (`wl80211.h`) - Lower Level
Direct driver control interface:

```c
// Control API (varargs-based dispatcher)
int wl80211_cntrl(int cmd, ...);

// Commands: WL80211_CTRL_SCAN, WL80211_CTRL_STA_CONNECT,
//          WL80211_CTRL_STA_DISCONNECT, WL80211_CTRL_STA_SET_PS_MODE,
//          WL80211_CTRL_AP_START, WL80211_CTRL_AP_STOP

// Inline helpers
int wl80211_sta_connect(struct wl80211_connect_params *param);
int wl80211_sta_disconnect(void);
int wl80211_sta_is_connected(void);
int wl80211_sta_set_ps(int mode);
int wl80211_ap_start(struct wl80211_ap_settings *ap_settings);
int wl80211_ap_stop(void);
int wl80211_set_country_code(const char *country_code);

// Channel/frequency utilities
int wl80211_channel_to_freq(int channel);
int wl80211_freq_to_channel(int freq);
```

### Async Events
WiFi state changes are posted via async event system:

```c
#define EV_WIFI ((uintptr_t)wifi_mgmr_init)

// Event codes
#define CODE_WIFI_ON_INIT_DONE        1
#define CODE_WIFI_ON_CONNECTED        4
#define CODE_WIFI_ON_DISCONNECT       5
#define CODE_WIFI_ON_GOT_IP           7
#define CODE_WIFI_ON_SCAN_DONE        9
#define CODE_WIFI_ON_AP_STARTED       11
#define CODE_WIFI_ON_AP_STA_ADD       21
```

## Dependencies

### Required Components
- **lwIP** (`components/net/lwip`) - TCP/IP stack
- **FreeRTOS** - RTOS primitives (semaphores, mutexes, tasks)
- **macsw** (`components/wireless/macsw`) - MAC hardware abstraction (v6.10.0.0)
- **bl_wpa_supplicant** (`components/wireless/bl_wpa_supplicant`) - WPA authentication
- **async_event** - Event posting mechanism
- **log** - Logging framework

### Optional Components
- **dhcp_server** - For AP mode DHCP functionality
- **shell** - For CLI command support

## Platform Abstraction

The `wl80211_platform.c/h` layer provides RTOS abstraction:
- Time functions: `rtos_ms2tick()`, `rtos_now()`
- Critical sections: `rtos_lock()`, `rtos_unlock()`
- Synchronization: semaphores, mutexes
- MAC address: `platform_get_mac()`

To support a new RTOS, implement the platform abstraction interface in `wl80211_platform.c`.

## Country/Regulatory Support

Country codes are defined in `country.c` with channel lists for 2.4GHz and 5GHz bands. Supported regions include CN, US, JP, EU, and others. The country code affects available channels and TX power limits.

## CLI Commands

When shell component is included, WiFi CLI commands are available via `wifi_mgmr_cli.c`:
- `wifi_sta_connect` - Connect to AP
- `wifi_sta_disconnect` - Disconnect
- `wifi_scan` - Scan for networks
- `wifi_info` - Display WiFi connection information
- `wifi_ap_start` - Start AP mode
- `wifi_ap_stop` - Stop AP mode
- `wifi_sta_list` - List connected STAs (AP mode)
- `wifi_sta_autoconnect_enable` - Enable auto reconnect
- `wifi_sta_autoconnect_disable` - Disable auto reconnect
- `wifi_country_code_set` - Set regulatory country code

---

## wl80211 CLI Compatibility with fhost

### Design Goal
**Purpose:** Maintain CLI command compatibility between wl80211 (BL616/BL618) and fhost (WiFi6) implementations to ensure automated test scripts work across both platforms.

### User Directives
1. "你需要参考fhost版本的wifi_scan命令，在wl80211版本上来实现同样的cli命令功能"
2. "并且保持命令的打印和fhost版本的api一致（因为要兼容自动化测试脚本）"
3. "注意wl80211版本下不可以调用fhost的API"
4. "增加wifi_info命令，要求打印一致"
5. "在接下来的工作中，把你掌握的指导性的信息（包括我的指令以及代码设计思路和目的）记录在wl80211下面的CLAUDE.md中，随时更新，随时阅读"

### Implementation Strategy

#### Key Differences Between Platforms
| Aspect | fhost (WiFi6) | wl80211 (BL616) |
|--------|---------------|-----------------|
| Print function | `fhost_printf()` | `wl80211_printf()` / `printf()` |
| getopt API | `utils_al_getopt()` | `utils_getopt()` |
| Parse functions | `utils_al_parse_number_adv()` | `utils_parse_number_adv()` |
| Command export | Inline in fhost | Via `wifi_mgmr_cli.c` + `main.c` |

#### Files Modified
1. **`wifi_mgmr_cli.c`** - WiFi manager CLI commands library
   - **All WiFi CLI commands are centralized in this file**
   - Contains CLI command implementations and SHELL_CMD_EXPORT_ALIAS macros
   - Commands are conditionally compiled with `CONFIG_SHELL`

2. **`examples/wifi/macsw_bare/main.c`** - Application entry point
   - Does NOT contain WiFi CLI commands (all moved to wifi_mgmr_cli.c)
   - Only contains application-specific commands (macsw_*, monitor_*)

### Implemented Commands

#### wifi_scan Command
**Location:** `main.c:wifi_scan_cmd()`

**Implementation:**
```c
static void wifi_scan_cmd(int argc, char **argv)
{
    int opt, ret;
    getopt_env_t getopt_env;
    wifi_mgmr_scan_params_t config;
    memset(&config, 0, sizeof(wifi_mgmr_scan_params_t));
    utils_getopt_init(&getopt_env, 0);

    while ((opt = utils_getopt(&getopt_env, argc, argv, "s:c:b:t:p:")) >= 0) {
        switch (opt) {
        case 's': // SSID filter
            memcpy(config.ssid_array, getopt_env.optarg, MGMR_SSID_LEN);
            config.ssid_length = strlen(getopt_env.optarg);
            printf("ssid: %s len: %d\r\n", config.ssid_array, config.ssid_length);
            break;
        case 'c': // Channel list (comma-separated)
            utils_parse_number_adv(getopt_env.optarg, ',', config.channels,
                                   MAX_FIXED_CHANNELS_LIMIT, 10, &config.channels_cnt);
            printf("scan channels: ");
            for (int i = 0; i < config.channels_cnt; i++) {
                printf("%d ", config.channels[i]);
            }
            printf("\r\n");
            break;
        case 'b': // BSSID filter
            config.bssid_set_flag = 1;
            utils_parse_number(getopt_env.optarg, ':', config.bssid, 6, 16);
            printf("bssid: %s, mac:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
                    getopt_env.optarg, MAC_ADDR_LIST(config.bssid));
            break;
        case 't': // Duration
            config.duration = atoi(getopt_env.optarg);
            break;
        case 'p': // Probe count
            config.probe_cnt = atoi(getopt_env.optarg);
            break;
        }
    }
    ret = wifi_mgmr_sta_scan(&config);
    if (ret < 0) {
        printf("scan failed\r\n");
    }
}
```

**Usage:**
```bash
wifi_scan                    # Scan all channels
wifi_scan -s MyNetwork       # Scan for specific SSID
wifi_scan -c 1,6,11          # Scan specific channels
wifi_scan -b AA:BB:CC:DD:EE:FF  # Scan for specific BSSID
wifi_scan -t 100 -p 2        # Set duration and probe count
```

#### wifi_info Command
**Location:** `main.c:wifi_info_cmd()`

**Implementation:**
```c
static void wifi_info_cmd(int argc, char **argv)
{
    wifi_mgmr_connect_ind_stat_info_t wifi_mgmr_ind_stat = {0};

    printf("\r\n");
    printf("STA Information:\r\n");
    printf("-------------------------------------\r\n");
    wifi_mgmr_sta_connect_ind_stat_get(&wifi_mgmr_ind_stat);
    printf("-------------------------------------\r\n");
}
```

**Output Format (when connected):**
```
STA Information:
-------------------------------------
Connected
ssid:            MyNetwork
passphr:         ********
bssid:           AA:BB:CC:DD:EE:FF
aid:             1
channel:         6
band:            2.4G
security:        3
-------------------------------------
```

**Output Format (when disconnected):**
```
STA Information:
-------------------------------------
No Connect
-------------------------------------
```

### Print Format Compatibility

The `wifi_mgmr_sta_info_dump()` function in `wifi_mgmr.c` already produces fhost-compatible output:
- Uses same field names and order
- Same password masking ("********" for secured, "Open" for open networks)
- Same format for BSSID (uppercase hex with colons)
- Same band display ("2.4G" or "5G")

#### wifi_sta_autoconnect_enable/disable Commands

**Location:** `wifi_mgmr_cli.c:wifi_sta_autoconnect_enable_cmd()`, `wifi_sta_autoconnect_disable_cmd()`

**Usage:**
```bash
wifi_sta_autoconnect_enable
wifi_sta_autoconnect_disable
```

**Output:**
```
Enable Auto Reconnect
Disable Auto Reconnect
```

### Autoconnect Implementation (Updated 2026-01-28)

**Design Decision:** Per user directive "不要在 wl80211_glb 中添加 disable_autoconnect 标志，把这个功能放在wifi_mgmr层中", the autoconnect functionality is implemented in the wifi_mgmr layer rather than the driver layer.

**Implementation Details:**

1. **State Management** (`wifi_mgmr.c:29`):
   ```c
   static int wifi_mgmr_disable_autoconnect = 1;  /* Default: disabled */
   ```

2. **Disconnect Event Handler** - Uses async_event instead of creating new task:
   ```c
   case WL80211_EVT_STA_DISCONNECTED:
       connect_ind_dump((ev->value >> 16) & 0xFF, ev->value & 0xFF);
       async_post_event(EV_WIFI, CODE_WIFI_ON_DISCONNECT, 0);
       handler = disconnect_tsk;
       /* Trigger autoconnect event if enabled */
       if (!wifi_mgmr_disable_autoconnect && wl80211_glb.last_connect_params) {
           async_post_event(EV_WIFI, CODE_WIFI_CMD_RECONNECT, 0);
       }
       break;
   ```

3. **Reconnect Event Handler** (`wifi_mgmr.c`):
   ```c
   case CODE_WIFI_CMD_RECONNECT: {
       if (!wifi_mgmr_disable_autoconnect && wl80211_glb.last_connect_params) {
           wl80211_printf("Autoconnect: Reconnecting to %s\r\n",
                          wl80211_glb.last_connect_params->ssid);
           wl80211_sta_connect(wl80211_glb.last_connect_params);
       }
   } break;
   ```

4. **Enable/Disable Functions** (`wifi_mgmr.c`):
   ```c
   int wifi_mgmr_sta_autoconnect_enable(void)
   {
       wifi_mgmr_disable_autoconnect = 0;
       wl80211_printf("Enable Auto Reconnect\r\n");
       return 0;
   }

   int wifi_mgmr_sta_autoconnect_disable(void)
   {
       wifi_mgmr_disable_autoconnect = 1;
       wl80211_printf("Disable Auto Reconnect\r\n");
       return 0;
   }
   ```

**Key Features:**
- Uses `wl80211_glb.last_connect_params` to retrieve saved connection parameters
- Uses async_event system for non-blocking reconnection (no task creation overhead)
- Checks both autoconnect state and saved parameters before reconnecting
- Default state is disabled (requires explicit enable)

#### wifi_country_code_set Command
**Location:** `main.c:wifi_country_code_set_cmd()`

**Usage:**
```bash
wifi_country_code_set CN
wifi_country_code_set US
wifi_country_code_set JP
```

**Output:**
```
Success: Country code set to CN
```

#### wifi_ap_start Command

**Location:** `wifi_mgmr_cli.c:wifi_ap_start_cmd()`

**Usage:**
```bash
# Open AP (no password)
wifi_ap_start -s MyAP

# Encrypted AP (with password)
wifi_ap_start -s MyAP -k mypassword

# With optional channel
wifi_ap_start -s MyAP -k mypassword -c 6
```

**Output:**
```
Starting AP: SSID=MyAP, Open
or
Starting AP: SSID=MyAP, Password=***
```

**Parameters:**
- `-s <ssid>` - AP SSID (required)
- `-k <password>` - AP password (optional, WPA2-PSK if provided)
- `-c <channel>` - WiFi channel 1-14 (optional, default 11)

**Default Settings:**
- Channel: 11
- Channel width: 20MHz
- Beacon interval: 100
- TX power: 0x14

#### wifi_ap_stop Command

**Location:** `wifi_mgmr_cli.c:wifi_ap_stop_cmd()`

**Usage:**
```bash
wifi_ap_stop
```

**Output:**
```
AP stopped
```

#### wifi_sta_list Command

**Location:** `wifi_mgmr_cli.c:wifi_sta_list_cmd()`

**Usage:**
```bash
wifi_sta_list
```

**Output (when STAs are connected):**
```
AP Station List:
========================================
STA 1:
  MAC: AA:BB:CC:DD:EE:FF
  AID: 1
  sta_idx: 1
STA 2:
  MAC: 11:22:33:44:55:66
  AID: 2
  sta_idx: 2
Total: 2 station(s)
========================================
```

**Output (when no STAs connected):**
```
AP Station List:
========================================
No stations connected
========================================
```

**Output (when AP mode is not enabled):**
```
AP mode is not enabled
```

**Implementation Details:**
- Uses `wl80211_glb.aid_list[4]` array to get connected STA information
- Each entry contains: MAC address, AID, sta_idx, and used flag
- Maximum 4 STAs can be connected simultaneously


### API Modifications

The following modifications were made to `wifi_mgmr.c` and `wifi_mgmr.h`:

#### wifi_mgmr.c Changes
1. **Added autoconnect state variable** - `wifi_mgmr_disable_autoconnect` (default: disabled)
2. **Modified `WL80211_EVT_STA_DISCONNECTED` handler** - Posts `CODE_WIFI_CMD_RECONNECT` event when enabled
3. **Added `CODE_WIFI_CMD_RECONNECT` event handler** - Performs reconnection using saved parameters
4. **Implemented `wifi_mgmr_sta_autoconnect_enable()`** - Enables autoconnect by setting flag to 0
5. **Implemented `wifi_mgmr_sta_autoconnect_disable()`** - Disables autoconnect by setting flag to 1
6. **Fixed `wifi_mgmr_get_country_code()`** - Fixed return type (was returning NULL instead of -1)

#### wifi_mgmr.h Changes
1. **Added declaration** for `wifi_mgmr_sta_autoconnect_enable()`
2. **Updated comments** for autoconnect functions - No longer marked as stub

### Build Verification

**Primary Build Command:**
```bash
make -C examples/wifi/macsw_bare CHIP=bl616l BOARD=bl616ldk
```

**Status:** ✅ Compiles successfully

### Important Notes

1. **SHELL_CMD_EXPORT_ALIAS Conflicts**: Do not export the same command symbol in both `wifi_mgmr_cli.c` and `main.c`. The implementation should live in `main.c` while `wifi_mgmr_cli.c` provides the shared function definitions.

2. **API Compatibility**: wl80211 cannot call fhost-specific APIs. Use only wl80211 APIs from `wifi_mgmr.h`.

3. **Print Functions**: Use `printf()` in main.c instead of `wl80211_printf()` for consistency.

4. **Macro Definitions**: Add `MAC_ADDR_LIST` macro to main.c for BSSID formatting.

5. **Test Script Compatibility**: The output format must match fhost exactly for automated test scripts to work correctly.

---

## Important Notes

- The `src/` directory contains optional source code (not built by default)
- Precompiled libraries in `lib/` are used for production builds
- This is BL616/BL618 specific; other chips use different WiFi drivers
- MAC layer is split between `macsw/` (symlink to shared component) and `net80211/`

---

## FreeBSD net80211 Porting Status

### Context
The `src/net80211/` directory contains a port of FreeBSD's net80211 WiFi stack to FreeRTOS for BL616. This is an ongoing effort to compile the FreeBSD 802.11 code on a bare-metal/RTOS environment.

### Primary Build Command
```bash
# Always use this command to test net80211 compilation
make -C examples/wifi/macsw_bare CHIP=bl616 BOARD=bl616dk
```

### Compilation Status (Latest - 2026-01-27)

**BUILD SUCCESS: ✅ All net80211 files compiled successfully!**

| File | Status | Notes |
|------|--------|-------|
| ieee80211.c | ✅ Compiled | Requires NO_SYSCTL and NO_IFMEDIA defined |
| ieee80211_crypto.c | ✅ Compiled | Forward declaration conflict resolved |
| ieee80211_input.c | ✅ Compiled | Requires extensive mbuf/Ethernet/LLC stubs |
| ieee80211_output.c | ✅ Compiled | BPF structures moved outside #if 0 block |
| ieee80211_node.c | ✅ Compiled | Fixed switch statement, time_uptime replaced |
| ieee80211_proto.c | ✅ Compiled | Fixed KASSERT redefinition |
| ieee80211_scan.c | ✅ Compiled | |
| ieee80211_scan_sta.c | ✅ Compiled | Fixed isocmp signedness warnings |
| ieee80211_scan_sw.c | ✅ Compiled | Fixed taskqueue function signatures |
| ieee80211_regdomain.c | ✅ Compiled | Macro expansion issue resolved |
| ieee80211_ratectl.c | ✅ Compiled | Added NO_SYSCTL guards |
| ieee80211_power.c | ✅ Compiled | Fixed MALLOC_DEFINE, added panic() stub |
| ieee80211_phy.c | ✅ Compiled | Fixed uninitialized variables |
| ieee80211_sta.c | ✅ Compiled | Type visibility fixed, time_uptime replaced |
| ieee80211_ioctl.c | ❌ Skipped | Per user request |
| ieee80211_hostap.c | ✅ Compiled | |
| ieee80211_adhoc.c | ✅ Compiled | |
| ieee80211_monitor.c | ✅ Compiled | |
| ieee80211_wds.c | ✅ Compiled | |
| ieee80211_acl.c | ✅ Compiled | |
| ieee80211_ht.c | ✅ Compiled | HT functionality enabled (user requested) |
| ieee80211_action.c | ✅ Compiled | |
| ieee80211_crypto_wep.c | ✅ Compiled | |
| ieee80211_crypto_tkip.c | ✅ Compiled | |
| ieee80211_crypto_ccmp.c | ✅ Compiled | |
| ieee80211_crypto_gcmp.c | ✅ Compiled | |
| ieee80211_crypto_none.c | ✅ Compiled | |
| ieee80211_xauth.c | ✅ Compiled | |
| ieee80211_amrr.c | ✅ Compiled | |
| ieee80211_rssadapt.c | ✅ Compiled | |
| ieee80211_ratectl_none.c | ✅ Compiled | |
| ieee80211_superg.c | ✅ Compiled | |
| ieee80211_ageq.c | ✅ Compiled | |
| ieee80211_hwmp.c | ✅ Compiled | |
| ieee80211_ddb.c | ✅ Compiled | time_uptime replaced |
| ieee80211_freebsd.c | ✅ Compiled | SYSCTL interfaces stubbed |
| ieee80211_alq.c | ❌ Skipped | ALQ not supported in IoT |
| ieee80211_radiotap.c | ❌ Skipped | BPF/Radiotap not supported in IoT |
| ieee80211_mesh.c | ❌ Skipped | Mesh networking (advanced feature) |
| ieee80211_tdma.c | ❌ Skipped | TDMA (advanced feature) |
| ieee80211_vht.c | ❌ Skipped | VHT (advanced feature) |
| ieee80211_dfs.c | ❌ Skipped | DFS (advanced feature) |

**Progress: Build completed successfully (100%)**

**Build Command:**
```bash
make -C examples/wifi/macsw_bare CHIP=bl616 BOARD=bl616dk
```

**Build Result:**
```
[100%] Linking C executable build_out/macsw_test_bl616.elf
[100%] Built target macsw_test_bl616.elf
```

---

### Compilation Status (Latest - 2026-01-30)

**BUILD SUCCESS: ✅ All net80211 files compiled successfully including mesh, vht, and dfs!**

| File | Status | Notes |
|------|--------|-------|
| ieee80211.c | ✅ Compiled | |
| ieee80211_crypto.c | ✅ Compiled | |
| ieee80211_input.c | ✅ Compiled | |
| ieee80211_output.c | ✅ Compiled | |
| ieee80211_node.c | ✅ Compiled | |
| ieee80211_proto.c | ✅ Compiled | |
| ieee80211_scan.c | ✅ Compiled | |
| ieee80211_scan_sta.c | ✅ Compiled | |
| ieee80211_scan_sw.c | ✅ Compiled | |
| ieee80211_regdomain.c | ✅ Compiled | |
| ieee80211_ratectl.c | ✅ Compiled | |
| ieee80211_power.c | ✅ Compiled | |
| ieee80211_phy.c | ✅ Compiled | |
| ieee80211_sta.c | ✅ Compiled | |
| ieee80211_hostap.c | ✅ Compiled | |
| ieee80211_adhoc.c | ✅ Compiled | |
| ieee80211_monitor.c | ✅ Compiled | |
| ieee80211_wds.c | ✅ Compiled | |
| ieee80211_mesh.c | ✅ Compiled | Fixed SYSCTL, IEEE80211_IOC_MESH_* constants, ALIGN |
| ieee80211_acl.c | ✅ Compiled | |
| ieee80211_ht.c | ✅ Compiled | |
| ieee80211_vht.c | ✅ Compiled | |
| ieee80211_dfs.c | ✅ Compiled | Fixed MALLOC_DEFINE, callout_schedule, ppsratecheck |
| ieee80211_action.c | ✅ Compiled | |
| ieee80211_crypto_wep.c | ✅ Compiled | |
| ieee80211_crypto_tkip.c | ✅ Compiled | |
| ieee80211_crypto_ccmp.c | ✅ Compiled | |
| ieee80211_crypto_gcm.c | ✅ Compiled | |
| ieee80211_crypto_gcmp.c | ✅ Compiled | |
| ieee80211_crypto_none.c | ✅ Compiled | |
| ieee80211_xauth.c | ✅ Compiled | |
| ieee80211_amrr.c | ✅ Compiled | |
| ieee80211_rssadapt.c | ✅ Compiled | |
| ieee80211_ratectl_none.c | ✅ Compiled | |
| ieee80211_superg.c | ✅ Compiled | |
| ieee80211_ageq.c | ✅ Compiled | |
| ieee80211_hwmp.c | ✅ Compiled | |
| ieee80211_ddb.c | ✅ Compiled | |
| ieee80211_freebsd.c | ✅ Compiled | |
| ieee80211_ioctl.c | ❌ Skipped | Per user request |
| ieee80211_alq.c | ❌ Skipped | ALQ not supported in IoT |
| ieee80211_radiotap.c | ❌ Skipped | BPF/Radiotap not supported in IoT |
| ieee80211_tdma.c | ❌ Skipped | TDMA not needed for basic IoT |

**Progress: Build completed successfully (100%)**

**Build Command:**
```bash
make -C examples/wifi/macsw_bare CHIP=bl616 BOARD=bl616dk
```

**Build Result:**
```
[100%] Linking C executable build_out/macsw_test_bl616.elf
[100%] Built target macsw_test_bl616.elf
```

**Recent Fixes (2026-01-30):**
1. **ieee80211_mesh.c**:
   - Fixed `SYSCTL_NODE` - stubbed in sys/sysctl.h
   - Added `IEEE80211_IOC_MESH_*` constants to ieee80211_freebsd.h (were inside `#ifdef __FreeBSD__` in ieee80211_ioctl.h)
   - Fixed `ALIGN(&rt[1])` - added explicit `(uintptr_t)` cast
   - Added `#include <net80211/ieee80211_ioctl.h>` for mesh ioctl constants
   - Commented out SYSCTL macro calls

2. **ieee80211_dfs.c**:
   - Fixed `static MALLOC_DEFINE` - removed `static` keyword (MALLOC_DEFINE expands to struct definition)
   - Added `callout_schedule()` stub to ieee80211_freebsd.h
   - Fixed `ppsratecheck()` signature - changed 2nd param from `const struct timeval *` to `int *`

3. **ieee80211_freebsd.h**:
   - Added `IEEE80211_IOC_MESH_*` constants (lines 2421-2433)
   - Added `if_getmtu()` stub returning 1500
   - Added `if_getcounter()` stub returning 0
   - Added `callout_schedule()` stub
   - Added `ppsratecheck()` stub with correct signature
   - Extended `IFCOUNTER_*` constants

---

### Previous Issues (RESOLVED)

#### Type Visibility Issue (RESOLVED - 2026-01-26)

**Problem:** Type definitions in `ieee80211_freebsd.h` were not visible to headers included by `ieee80211_var.h`

**Solution Applied (Option 1):**
1. Moved all critical type definitions to top of `ieee80211_freebsd.h` (lines 47-95)
2. Added early type definitions for:
   - `struct mtx`
   - `counter_u64_t`
   - `ieee80211_com_lock_t`
   - `ieee80211_tx_lock_t`
   - `ieee80211_ff_lock_t`
   - `ieee80211_node_lock_t`
   - `ieee80211_psq_lock_t`
   - `ieee80211_ageq_lock_t`
3. Commented out original typedef definitions to avoid redefinition

**Files Modified:**
- `ieee80211_freebsd.h`: Added early type definitions
- `ieee80211_sta.c`: Now compiles successfully
- `ieee80211_node.c`: Now compiles successfully

**Status:** ✅ RESOLVED - ieee80211_sta.c and ieee80211_node.c now compile

### BPF/Radiotap Removal

**Status:** ✅ COMPLETED

Per user request: "去掉bpf相关的功能，这个功能在IoT场景不需要"

**Changes Made:**
1. Moved `struct ieee80211_bpf_params` and `struct ieee80211_tx_params` OUTSIDE `#if 0` block in `ieee80211_freebsd.h` (lines 1888-1920)
2. Moved `BPF_MTAP` macro outside `#if 0` block (line 814)
3. Provided stub implementations for BPF functions:
```c
static inline int ieee80211_add_xmit_params(struct mbuf *m, const struct ieee80211_bpf_params *p)
    { (void)m; (void)p; return 0; }
static inline int ieee80211_get_xmit_params(struct mbuf *m, struct ieee80211_bpf_params *p)
    { (void)m; (void)p; return 0; }
// ... other stubs
```
4. Commented out `ieee80211_radiotap.c` in CMakeLists.txt:
```cmake
#target_sources(${LIBRARY_NAME} PRIVATE net80211/net80211/ieee80211_radiotap.c)  /* BPF/Radiotap not supported in IoT */
```

### Header Management Strategy

**CRITICAL LESSON:** Do NOT create stub headers that shadow toolchain headers!

**Files Modified (sys/socket.h removal):**
Per user request: "我认为你应该直接把net80211里对sys/socket.h的依赖去除"

Removed all `#include <sys/socket.h>` from net80211 source files and added definitions directly to `ieee80211_freebsd.h`:
```c
/* Socket address structure - from sys/socket.h */
struct sockaddr {
    uint8_t  sa_len;        /* Total length */
    uint8_t  sa_family;     /* Address family */
    char     sa_data[14];   /* Address value */
};

#define AF_UNSPEC       0
#define AF_UNIX         1
#define AF_INET         2
#define AF_INET6        3
#define AF_IEEE80211    32
#define AF_MAX          40

typedef size_t socklen_t;
```

**Files Modified (ieee80211_regdomain.c):**
Removed local `struct mtx` definitions that were conflicting with `ieee80211_freebsd.h`.

### FreeBSD Compatibility Layer (ieee80211_freebsd.h)

This file provides stub definitions to allow FreeBSD code to compile without actual FreeBSD kernel.

**IMPORTANT - Early Type Definitions (Lines 47-95):**
To resolve type visibility issues, critical types are defined at the TOP of the file, immediately after basic includes:

```c
/* Basic mutex structure - must be defined early */
struct mtx {
    int dummy;
};

/* Counter type */
typedef uint64_t counter_u64_t;

/* Lock types - full definitions for early visibility */
typedef struct {
    char name[16];
    struct mtx mtx;
} ieee80211_com_lock_t;

typedef struct {
    char name[16];
    struct mtx mtx;
} ieee80211_tx_lock_t;

typedef struct {
    char name[16];
    struct mtx mtx;
} ieee80211_ff_lock_t;

typedef struct {
    char name[16];
    struct mtx mtx;
} ieee80211_node_lock_t;

/* Simple typedefs for lock types */
typedef struct mtx ieee80211_psq_lock_t;
typedef struct mtx ieee80211_ageq_lock_t;
```

**Note:** The original typedef definitions are commented out later in the file (around lines 1230-1356) to avoid redefinition.

**Lock Macros (Lines 1238-1250):**
```c
#define IEEE80211_LOCK_INIT(_ic, _name) ...
#define IEEE80211_LOCK_OBJ(_ic)      (&(_ic)->ic_comlock.mtx)
#define IEEE80211_LOCK_DESTROY(_ic)  mtx_destroy(IEEE80211_LOCK_OBJ(_ic))
#define IEEE80211_LOCK(_ic)          mtx_lock(IEEE80211_LOCK_OBJ(_ic))
#define IEEE80211_UNLOCK(_ic)        mtx_unlock(IEEE80211_LOCK_OBJ(_ic))
#define IEEE80211_LOCK_ASSERT(_ic)   mtx_assert(IEEE80211_LOCK_OBJ(_ic), MA_OWNED)
#define IEEE80211_UNLOCK_ASSERT(_ic) mtx_assert(IEEE80211_LOCK_OBJ(_ic), MA_NOTOWNED)
#define IEEE80211_IS_LOCKED(_ic)     mtx_owned(IEEE80211_LOCK_OBJ(_ic))
```

**Mutex Functions (Lines 823-828):**
```c
static inline void mtx_init(struct mtx *m, const char *name, const char *type, int opts);
static inline void mtx_destroy(struct mtx *m);
static inline void mtx_lock(struct mtx *m);
static inline void mtx_unlock(struct mtx *m);
static inline void mtx_assert(struct mtx *m, int what);
static inline int mtx_owned(struct mtx *m);
```

/* BPF structures (defined but functions are stubs) */
struct ieee80211_bpf_params {
    uint8_t ibp_vers;
#define IEEE80211_BPF_VERSION 0
    uint8_t ibp_len;
    uint8_t ibp_flags;
#define IEEE80211_BPF_SHORTPRE 0x01
#define IEEE80211_BPF_NOACK    0x02
#define IEEE80211_BPF_CRYPTO   0x04
    // ... more fields
};

struct ieee80211_tx_params {
    struct ieee80211_bpf_params params;
};
```

**BPF Stub Implementations:**
```c
#define BPF_MTAP(ifp, m) do { (void)(ifp); (void)(m); } while (0)

static inline int ieee80211_add_xmit_params(struct mbuf *m, const struct ieee80211_bpf_params *p)
    { (void)m; (void)p; return 0; }
static inline int ieee80211_get_xmit_params(struct mbuf *m, struct ieee80211_bpf_params *p)
    { (void)m; (void)p; return 0; }
static inline int ieee80211_add_rx_params(struct mbuf *m, const struct ieee80211_rx_stats *rxs)
    { (void)m; (void)rxs; return 0; }
static inline int ieee80211_get_rx_params(struct mbuf *m, struct ieee80211_rx_stats *rxs)
    { (void)m; (void)rxs; return 0; }
static inline const struct ieee80211_rx_stats *ieee80211_get_rx_params_ptr(struct mbuf *m)
    { (void)m; return NULL; }
```

### Known Issues and Resolutions

#### 1. ieee80211_ioctl.c - Skipped Per User Request
**Status:** ✅ SKIPPED - User explicitly stated: "ieee80211_ioctl.c不需要参与编译"

#### 2. Type Visibility Issue (CURRENT BLOCKER)
**Status:** ⏸️ BLOCKED - See "Current Blocking Issue" section above for details

#### 3. BPF Structure Visibility (RESOLVED)
**Problem:** `struct ieee80211_bpf_params` was inside `#if 0` block, making it invisible
**Solution:** Moved structure definitions outside `#if 0` while keeping only function declarations inside
**Status:** ✅ RESOLVED

#### 4. BPF_MTAP Macro Visibility (RESOLVED)
**Problem:** `BPF_MTAP` macro was inside `#if 0` block
**Solution:** Moved macro definition outside `#if 0` block
**Status:** ✅ RESOLVED

#### 5. time_uptime Undefined (RESOLVED - 2026-01-26)
**Problem:** `time_uptime` variable not defined for bare-metal build
**Symptoms:**
```c
error: 'time_uptime' undeclared (first use in this function)
```
**Solution Applied (Per User Request):**
User requested: "time_uptime这个东西用0替代，你在相关代码处加一个//FIXME就行了"

Replaced all uses of `time_uptime` with 0 and added FIXME comments:
- `ieee80211_sta.c:1671`: `ni->ni_jointime = 0;  // FIXME: replace with time_uptime when available`
- `ieee80211_node.c:2520`: `ni->ni_jointime = 0;  // FIXME: replace with time_uptime when available`
- `ieee80211_ddb.c:234`: `(unsigned long)(0 - ni->ni_jointime), ni->ni_challenge);  // FIXME: replace with time_uptime when available`

Also removed time_uptime declaration and definition from:
- `ieee80211_freebsd.h`: Commented out `extern int time_uptime;`
- `ieee80211_freebsd.c`: Removed `int time_uptime = 0;`

**Status:** ✅ RESOLVED

#### 6. endian.h Include Path (RESOLVED - 2026-01-26)
**Problem:** `#include <endian.h>` not found in bare-metal build
**Solution:** Changed to `#include <sys/endian.h>` in `ieee80211_input.h:78`
**Status:** ✅ RESOLVED

#### 7. Missing mbuf Flags (RESOLVED - 2026-01-26)
**Problem:** Missing mbuf flags `M_AMPDU_MPDU`, `M_AMPDU`, `M_WEP`
**Solution:** Added flag definitions to `sys/mbuf.h`:
```c
#define M_AMPDU_MPDU 0x00000100  /* A-MPDU subframe */
#define M_AMPDU      0x00000200  /* A-MPDU */
#define M_WEP        0x00000400  /* WEP encrypted */
```
**Status:** ✅ RESOLVED

#### 8. Structure Redefinition (RESOLVED - 2026-01-26)
**Problem:** `struct sysctl_req` and `struct mbuf` redefinition between stub headers and `ieee80211_freebsd.h`
**Solution:** Added `#ifndef` guards:
- `_SYSCTL_REQ_DEFINED_` for `struct sysctl_req`
- `_MBUF_STRUCT_DEFINED_` for `struct mbuf`
- Updated stub headers to define these guards
**Status:** ✅ RESOLVED

### Directory Structure

```
src/net80211/
├── CMakeLists.txt              # Build configuration
├── net80211/
│   ├── ieee80211_freebsd.h     # FreeBSD compatibility layer (STUB definitions)
│   ├── ieee80211_var.h         # Main net80211 header (INCLUDES ieee80211_freebsd.h)
│   ├── ieee80211.h             # Core definitions
│   ├── ieee80211.c             # Main implementation
│   ├── ieee80211_output.c      # TX path
│   ├── ieee80211_input.c       # RX path
│   ├── ieee80211_node.c        # Node management
│   ├── ieee80211_regdomain.c   # Regulatory domain
│   └── ... (many more files)
└── sys/                         # Stub headers
    ├── socket.h                 # NOT USED - definitions in ieee80211_freebsd.h
    └── ... (other stub headers)
```

### Testing and Validation

**Current phase:** Compilation only (not functional)

**Completed:**
- ✅ Successfully compiled 13 out of ~45 net80211 files (29%)
- ✅ Removed sys/socket.h dependencies per user request
- ✅ Removed BPF/Radiotap functionality per user request
- ✅ Fixed BPF structure visibility issues

**Blocked:**
- ⏸️ Type visibility issue between ieee80211_freebsd.h and included headers

**Next Steps:**
1. Resolve type visibility issue (see suggested solutions above)
2. Continue compilation of remaining net80211 files
3. Implement actual FreeBSD compatibility layer functions
4. Integrate with MACSW layer
5. Test basic WiFi operations

### User Feedback Summary

**Directives:**
- "我认为你应该直接把net80211里对sys/socket.h的依赖去除" - Remove sys/socket.h dependencies
- "去掉bpf相关的功能，这个功能在IoT场景不需要" - Remove BPF functionality
- "不要注释掉net80211/net80211/ieee80211_freebsd.c这个文件的编译" - Don't comment out ieee80211_freebsd.c compilation
- "ieee80211_ioctl.c不需要参与编译" - ieee80211_ioctl.c doesn't need to be compiled
- "那就按Option 1来执行，注意一点，我要求能编译通过，与FreeBSD紧耦合的部分，比如对#include <net/xxx.h>，还有ddb等相关的功能全部注释掉，我只关注net80211 softmac协议栈的基础功能，OS的功能我后面会再适配，强耦合的地方我也是要去除的。" - Execute Option 1, remove FreeBSD-coupled parts
- "time_uptime这个东西用0替代，你在相关代码处加一个//FIXME就行了。" - Replace time_uptime with 0 and add FIXME comments
- "更新最新信息到CLAUDE.md吧，剩下我来处理" - Update CLAUDE.md with latest progress

**General Guidance:**
- Focus on compilation, not over-engineering
- Use standard C library functions
- Remove unnecessary FreeBSD kernel features
- Keep stub implementations simple

### Build Commands Reference

**Primary Build Command:**
```bash
make -C examples/wifi/macsw_bare CHIP=bl616 BOARD=bl616dk
```

**Full Clean Build:**
```bash
make -C examples/wifi/macsw_bare clean
make -C examples/wifi/macsw_bare CHIP=bl616 BOARD=bl616dk
```

### References

- FreeBSD net80211 source: `sys/net80211/` in FreeBSD source tree
- FreeBSD 802.11 implementation: https://cgit.freebsd.org/src/tree/sys/net80211
- BL616 Reference Manual: For hardware-specific details

---

## Work Log (2026-01-26)

### Session Summary
This session focused on resolving the type visibility blocking issue that was preventing `ieee80211_sta.c` and other net80211 files from compiling.

### Completed Tasks

1. **Type Visibility Issue (Option 1 Implementation)** ✅
   - Added early type definitions to `ieee80211_freebsd.h` (lines 47-95)
   - Types moved: `struct mtx`, `counter_u64_t`, all lock types
   - Commented out original typedef definitions to avoid redefinition
   - Result: `ieee80211_sta.c` now compiles successfully

2. **Structure Redefinition Fix** ✅
   - Added `_SYSCTL_REQ_DEFINED_` guard for `struct sysctl_req`
   - Added `_MBUF_STRUCT_DEFINED_` guard for `struct mbuf`
   - Updated stub headers to define these guards

3. **endian.h Include Path** ✅
   - Changed `#include <endian.h>` to `#include <sys/endian.h>` in `ieee80211_input.h`

4. **Missing mbuf Flags** ✅
   - Added `M_AMPDU_MPDU`, `M_AMPDU`, `M_WEP` to `sys/mbuf.h`

5. **time_uptime Replacement** ✅
   - Replaced all uses with 0 and added FIXME comments
   - Files modified: `ieee80211_sta.c`, `ieee80211_node.c`, `ieee80211_ddb.c`
   - Removed declaration from `ieee80211_freebsd.h` and definition from `ieee80211_freebsd.c`

### Current Issue (INVESTIGATION)

**Macro Expansion Problem:**
- `ieee80211_regdomain.c` fails with implicit declaration errors for lock macros
- Macros are defined in `ieee80211_freebsd.h` but not expanded during preprocessing
- Preprocessed output shows macros are used but not defined
- Investigating stub headers interference or include scope issues

**Error Messages:**
```
error: implicit declaration of function 'IEEE80211_LOCK_ASSERT'
error: implicit declaration of function 'IEEE80211_LOCK'
error: implicit declaration of function 'IEEE80211_UNLOCK'
```

### Files Modified Today

1. `components/wireless/wl80211/src/net80211/net80211/ieee80211_freebsd.h`
   - Lines 47-95: Added early type definitions
   - Lines 406-408: Added `_SYSCTL_REQ_DEFINED_` guard
   - Lines 533-543: Added `_MBUF_STRUCT_DEFINED_` guard
   - Lines 968: Commented out time_uptime declaration
   - Lines 1230-1356: Commented out original typedef definitions
   - Lines 2187-2215: Added if_clone_addreq, IFC_F_AUTOUNIT, ifc_attach_cloner, ifc_detach_cloner, if_getbroadcastaddr stubs
   - Lines 2242-2249: Fixed SYSCTL_NODE_CHILDREN return type (removed extra pointer)
   - Lines 2292: Fixed ALIGN macro to use explicit uintptr_t casts
   - Lines 2337-2340: Added IF_LLADDR_CONST for const struct ifnet *
   - Lines 2371-2377: Added if_setdrvflagbits stub function
   - Lines 2379-2380: Fixed EVENTHANDLER_REGISTER to return eventhandler_tag

2. `components/wireless/wl80211/src/net80211/sys/sysctl.h`
   - Lines 45-47: Added `_SYSCTL_REQ_DEFINED_` guard

3. `components/wireless/wl80211/src/net80211/sys/mbuf.h`
   - Lines 28-46: Added `_MBUF_STRUCT_DEFINED_` guard
   - Lines 20: Added `M_AMPDU_MPDU`, `M_AMPDU`, `M_WEP` flags

4. `components/wireless/wl80211/src/net80211/net80211/ieee80211_input.h`
   - Line 78: Changed `<endian.h>` to `<sys/endian.h>`

5. `components/wireless/wl80211/src/net80211/net80211/ieee80211_sta.c`
   - Line 1671: Replaced time_uptime with 0, added FIXME

6. `components/wireless/wl80211/src/net80211/net80211/ieee80211_node.c`
   - Line 2520: Replaced time_uptime with 0, added FIXME

7. `components/wireless/wl80211/src/net80211/net80211/ieee80211_ddb.c`
   - Line 234: Replaced time_uptime with 0, added FIXME

8. `components/wireless/wl80211/src/net80211/net80211/ieee80211_freebsd.c`
   - Lines 77-78: Removed time_uptime definition
   - Lines 1073-1075: Changed IF_LLADDR to IF_LLADDR_CONST for const ifp
   - Lines 1087-1089: Changed IF_LLADDR to IF_LLADDR_CONST for const ifp
   - Lines 247-256: Converted ieee80211_sysctl_vattach to stub (SYSCTL not supported)
   - Lines 305-312: Converted ieee80211_sysctl_vdetach to stub
   - Lines 175-237: Converted all SYSCTL handler functions to stubs
   - Line 262: Removed & operator from SYSCTL_ADD_NODE call
   - Line 483: Fixed ALIGN macro usage with explicit uintptr_t cast

9. `components/wireless/wl80211/src/CMakeLists.txt`
   - Line 146: Commented out ieee80211_alq.c (ALQ not supported in IoT)

10. `components/wireless/wl80211/src/net80211/net80211/ieee80211_alq.c`
    - Lines 44-47: Commented out sys/pcpu.h and related FreeBSD-specific headers

11. `components/wireless/wl80211/CLAUDE.md`
    - Updated with latest progress and status

### Session Summary (2026-01-27)

**BUILD SUCCESS: ✅ net80211 compilation completed successfully!**

This session successfully completed the net80211 compilation by:

1. **if_clone Interface Stubs** - Added proper stubs for FreeBSD interface cloning:
   - `struct if_clone_addreq` with correct function signatures
   - `IFC_F_AUTOUNIT` constant
   - `ifc_attach_cloner()`, `ifc_detach_cloner()`, `if_getbroadcastaddr()` functions

2. **EVENTHANDLER Macro Fix** - Changed `EVENTHANDLER_REGISTER` to return `(eventhandler_tag)0` instead of `NULL`

3. **SYSCTL Interface Removal** - Per user request, all SYSCTL functionality was stubbed:
   - `ieee80211_sysctl_vattach()` - stubbed
   - `ieee80211_sysctl_vdetach()` - stubbed
   - All SYSCTL handler functions - stubbed

4. **ALQ Removal** - Commented out `ieee80211_alq.c` compilation (ALQ is FreeBSD-specific debugging)

5. **Minor Fixes:**
   - Fixed `SYSCTL_NODE_CHILDREN` return type
   - Fixed `ALIGN` macro with explicit `uintptr_t` casts
   - Added `IF_LLADDR_CONST` for const-correctness
   - Added `if_setdrvflagbits()` stub function

### Next Steps

The net80211 softmac codebase now compiles successfully. The next phase would be:
1. Integration testing with MACSW layer
2. Runtime functionality testing
3. Performance optimization
4. Memory usage analysis

---

## Work Log (2026-01-28)

### Session Summary

Completed implementation of WiFi autoconnect functionality in the wifi_mgmr layer per user directive: "不要在 wl80211_glb 中添加 disable_autoconnect 标志，把这个功能放在wifi_mgmr层中".

### Completed Tasks

1. **Autoconnect Implementation** ✅
   - Added `wifi_mgmr_disable_autoconnect` static state variable in wifi_mgmr layer
   - Created `wifi_mgmr_sta_autoconnect_task()` FreeRTOS task for non-blocking reconnection
   - Implemented `wifi_mgmr_sta_autoconnect_enable()` and `wifi_mgmr_sta_autoconnect_disable()` functions
   - Modified `WL80211_EVT_STA_DISCONNECTED` event handler to trigger autoconnect when enabled
   - Uses existing `wl80211_glb.last_connect_params` for saved connection parameters

2. **Build Verification** ✅
   - Verified build with: `make -C examples/wifi/macsw_bare CHIP=bl616l BOARD=bl616ldk`
   - Build completed successfully

3. **Documentation Update** ✅
   - Updated CLAUDE.md to reflect actual autoconnect implementation (no longer stub)
   - Added detailed implementation documentation with code snippets
   - Updated API Modifications section

### Files Modified

1. `components/wireless/wl80211/wifi_mgmr.c`
   - Line 29: Added `wifi_mgmr_disable_autoconnect` state variable
   - Lines 228-230: Modified disconnect event handler to post reconnect event (optimized - no task creation)
   - Lines 275-281: Added `CODE_WIFI_CMD_RECONNECT` event handler for reconnection
   - Lines 744-756: Implemented enable/disable functions

**Optimization (2026-01-28):**
- Replaced xTaskCreate with async_post_event to avoid task creation overhead
- User feedback: "必须要在重连的时候创建一个新的task吗？" led to optimization using existing event system

2. `components/wireless/wl80211/include/wifi_mgmr.h`
   - Added declaration for `wifi_mgmr_sta_autoconnect_enable()`

3. `components/wireless/wl80211/CLAUDE.md`
   - Updated autoconnect documentation with actual implementation details
   - Updated API Modifications section

---

## Work Log (2026-01-28 - Session 2)

### Session Summary

Added AP mode CLI commands (`wifi_ap_start` and `wifi_ap_stop`) compatible with fhost version.

### Completed Tasks

1. **wifi_ap_start Command** ✅
   - Supports open AP: `wifi_ap_start -s <ssid>`
   - Supports encrypted AP: `wifi_ap_start -s <ssid> -k <password>`
   - Optional channel parameter: `-c <channel>`
   - Uses wl80211 driver API directly

2. **wifi_ap_stop Command** ✅
   - Stops AP mode
   - Simple command without parameters

3. **Build Verification** ✅
   - Verified build with: `make -C examples/wifi/macsw_bare CHIP=bl616l BOARD=bl616ldk`
   - Build completed successfully

4. **Documentation Update** ✅
   - Added command documentation to CLAUDE.md
   - Updated CLI Commands section

### Files Modified

1. `components/wireless/wl80211/wifi_mgmr_cli.c`
   - Added `wifi_ap_start_cmd()` function (lines 230-290)
   - Added `wifi_ap_stop_cmd()` function (lines 292-302)
   - Added SHELL_CMD_EXPORT_ALIAS macros for both commands

2. `components/wireless/wl80211/CLAUDE.md`
   - Updated CLI Commands section to list new commands
   - Added detailed documentation for wifi_ap_start and wifi_ap_stop commands

### Command Details

**wifi_ap_start:**
- Required: `-s <ssid>` for SSID
- Optional: `-k <password>` for WPA2-PSK encryption
- Optional: `-c <channel>` for channel selection (1-14)
- Defaults: channel 11, 20MHz width, beacon interval 100

**wifi_ap_stop:**
- Stops AP mode
- No parameters required

---

## Work Log (2026-01-28 - Session 3)

### Session Summary

Added `wifi_sta_list` command for AP mode to display connected stations list.

### Completed Tasks

1. **wifi_sta_list Command** ✅
   - Lists all connected STAs in AP mode
   - Shows MAC address, AID, and sta_idx for each STA
   - Checks AP mode status before displaying
   - Uses `wl80211_glb.aid_list[4]` array

2. **Build Verification** ✅
   - Verified build with: `make -C examples/wifi/macsw_bare CHIP=bl616l BOARD=bl616ldk`
   - Build completed successfully

3. **Documentation Update** ✅
   - Added command documentation to CLAUDE.md
   - Updated CLI Commands section

### Files Modified

1. `components/wireless/wl80211/wifi_mgmr_cli.c`
   - Added `wifi_sta_list_cmd()` function (lines 304-342)
   - Added SHELL_CMD_EXPORT_ALIAS macro

2. `components/wireless/wl80211/CLAUDE.md`
   - Updated CLI Commands section
   - Added detailed documentation for wifi_sta_list command

### Command Details

**wifi_sta_list:**
- No parameters required
- Displays up to 4 connected STAs
- Shows: MAC address, AID, sta_idx
- Handles cases: AP not enabled, no STAs connected
