# CHANGELOG

## v2.3.23 — since v2.3.22 (2026-04-11 → 2026-04-17)

### New Features

- **WiFi**
  - Added WiFi AP intra-BSS forwarding support (stations in the same AP can communicate directly)
  - Added random MAC address support for STA mode
  - Made WiFi log output configurable at runtime
  - Made WNM/MBO/RRM features conditionally compiled under `MACSW_WFA` flag for WFA certification builds

- **iperf**
  - Added UDP jitter and packet loss statistics reporting

- **Low Power**
  - Added low-power support to NetHub component
  - Updated LP wait timeout handling

- **BL616CL**
  - Added support for 96M clock flash configuration on bl616cldk board
  - Added support for ZB25WQ80A 1 MB flash
  - Updated ADC calibration process for A0 silicon
  - Added external DCDC support for PDS mode

- **Thread / Zigbee**
  - Enhanced Thread and Zigbee examples with lmac154 improvements
  - Enhanced lmac154 frame parsing and restructured for multi-platform support

- **Multimedia**
  - Refined miniAlsa API

- **BL602**
  - Added `reboot` shell command on bl602dk board

- **Camera**
  - Added PEC QSPI camera demo

### Bug Fixes

- **WiFi**
  - Enabled `LWIP_NETIF_TX_SINGLE_PBUF` to improve TCP transmit performance
  - Fixed cipher mismatch handling and optimized cipher selection logic
  - Fixed 2.4 GHz-only chips incorrectly reporting 5 GHz channel support
  - Fixed keyram not being cleared on disconnection (security hardening)
  - Fixed early null-pointer dereference when checking connected AP pointer in Linux driver
  - Improved Linux driver build compatibility
  - Fixed invalid VIF/STA ID used in BAR frame immediately after disconnection

- **Low Power**
  - Optimized keyram dump procedure and corrected 256-bit key display
  - Fixed missing macsw register configuration macros for LP mode
  - Fixed SF configuration value `0x3F` not handled before flash power-down
  - Fixed SF configuration value `0x3F` not handled during low-power flash recovery

- **Bluetooth / BLE**
  - Added `get_mac` API support in ADV-only controller mode
  - Fixed MTU and data-length exchange serialization for OAD and ATVV profiles
  - Fixed static RAM usage in BLE stack to prevent crash

- **NetBus / USB WiFi**
  - Fixed host unable to receive data when operating in USB WiFi AP mode

- **Zigbee**
  - Applied customer-reported bug fixes

- **Build / Tooling**
  - Fixed invalid filename handling when checking out on Windows
  - Fixed RAM startup address failure

### Improvements

- Unified three separate OTA code paths into a single shared implementation (affects lhal, bluetooth, netbus, and host_router)
- Optimized Wi-Fi TX path cache affinity for improved throughput
- Thread component aligned with latest lmac154 API changes

## v2.3.22

### New Features

- **BL616CL**
  - Added timeout interrupt support
  - Added I2C EEPROM DMA example
  - Added PSRAM hybrid sleep support
  - Added PSRAM PDS RTC test
  - Updated phy_rf library

- **Memory Management**
  - Added heap5 support
  - Added PSRAM hybrid sleep mode

- **WiFi**
  - Added packet count control in MFG firmware for transmission testing
  - Added beacon-only mode via controller API
  - Optimized WiFi/lwIP throughput

- **Bluetooth/BLE**
  - Added beacon-only controller API
  - Added EM free API

- **LCD**
  - Added ST7789V DBI driver

- **RTOS**
  - Added FreeRTOS POSIX support

- **Tools**
  - Added `mem_region_analyze.py` for memory map file analysis
  - Added common ADC key component with demo

### Bug Fixes

- **Low Power**
  - Fixed LP firmware keyram restore for SEC 256 configuration
  - Fixed missing LP keyram restore path

- **Flash**
  - Fixed 2-line boot failure

- **Memory**
  - Fixed PMP entry configuration for address 0xF0000000
  - Fixed PSRAM heap initialization

- **WiFi Security**
  - Fixed state machine transition during Group Key Rekey
  - Hardened bounds checks in multiple WiFi driver paths (FT IE, DHCP, message handlers)
  - Fixed 32-bit/64-bit compile compatibility in Linux driver

- **Linker**
  - Fixed stack section placement (moved to noinit region)

- **Debug / Crash Analysis**
  - Fixed backtrace before scheduler starts
  - Fixed backtrace for last instruction of a function

- **Bluetooth/BLE**
  - Fixed BR/EDR slave-to-master role switch failure
  - Fixed BLE MFG log output routing (SDIO/USB)

- **wl80211**
  - Added STA connect/disconnect event notification in AP mode

### Improvements

- Reduced code size of image transmission module
- Split low power modules into separate components for better maintainability
- Removed internal shell commands from library
- Reduced macsw log overhead
