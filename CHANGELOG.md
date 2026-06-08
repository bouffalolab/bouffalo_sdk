# CHANGELOG

## v2.3.27 — since v2.3.26 (2026-05-16 → 2026-06-07)

### New Features

- **BL618DG (B0 Silicon)**
  - Set default RAM size to maximum
  - Added GDB memory map support
  - Added SDIO3 low-power example
  - Improved SDIO recovery after host reload and low-power resume
  - Added audio PLL rate configuration for various sample rates
  - Added CPU PLL VCO frequencies 632.2176 MHz and 638.976 MHz
  - Added Bluetooth compilation and initialization support

- **BL616CL**
  - Added DCDC low power configuration support
  - Added low-power support in uart_wifi example
  - Updated PhyRF library

- **BL602**
  - Added MFG command support

- **Bluetooth / BLE**
  - Added AutoPTS support for BL618DG and BL616CL

- **WiFi**
  - Migrated buffer-related configurations to the configuration layer
  - Added CSA channel switch support
  - Added gcov code coverage dump support
  - PMK cache now compiled only for WFA builds (reduced code size)

- **Network**
  - Added DHCPD trace logging
  - Added netbus USB low-power support with SDIO/USB switching
  - Added nethub SDIO recovery after host reload and low-power resume

- **Audio**
  - Added `wave_player_set_record_pga` API

- **Peripherals**
  - Added CPU privilege mode support
  - Added SDH (SD Host) API
  - Added GPIO-based IPC performance measurement
  - Added RMII and MDIO port configuration from Makefile

- **Thread**
  - Added API to set Thread instance

- **Build System / Core**
  - Added gcov code coverage dump support
  - Added shell auto-list (tab-completion) support
  - Added rodata and text segment PMP alignment support

- **Tools**
  - Updated fw_post_proc to v1.4.3

### Bug Fixes

- **BL618DG (B0 Silicon)**
  - Fixed CPU LP mtimer issue on B0 silicon
  - Fixed flash margin adjustment at 80 MHz operation
  - Fixed LP firmware RAM address
  - Fixed LP core ID configuration
  - Fixed LP level2 IRQ handling
  - Fixed SPI3 clock domain to use MINI clock
  - Fixed eFUSE region1 programming and common trim offsets
  - Fixed arch_delay inaccuracy on B0 silicon
  - Fixed startup GPIO1 boot debug pull, now released after boot
  - Fixed smartconfigble support
  - Fixed low-power handling for nethub
  - Removed stale temporary partition file from btble_cli
  - Fixed 5G and Bluetooth coexistence issue
  - Updated Bluetooth hardware adaptation for B0 silicon

- **BL616CL**
  - Fixed missing headers in BTBLE examples
  - Updated PhyRF library

- **WiFi**
  - Fixed LD section synchronization and AMSDU bit alignment with driver
  - Fixed buffer config macro definitions for static array sizing in wl80211
  - Fixed disabled FTM task dispatch
  - Fixed RX workqueue CPU time usage for USB
  - Fixed race condition in URB resubmission paths
  - Fixed MP command expected response handling
  - Fixed BTSDU data port handling, added dump capability
  - Added SDIO interrupt polling fallback
  - Fixed BTSDU write done priority
  - Fixed cache line size alignment to use BFLB_CACHE_LINE_SIZE
  - Fixed weak function attribute usage in WLAN driver
  - Fixed WLAN driver compilation on kernel 3.x
  - Fixed firmware array weak linking to prevent xxd overwrite
  - Added BLMP full command set support

- **Bluetooth / BLE**
  - Fixed redundant unref on directed advertising
  - Fixed GATT server buffer heap overflow
  - Hardened BR/EDR profile receive paths and fixed latent bugs
  - Fixed feature macros and BR/EDR CLI/SPP bugs on BL618DG and BL616CL
  - Fixed stale pending advertising data not discarded on advertising stop
  - Temporarily switched RF path to combo mode for BL618DG

- **Low Power**
  - Optimized and refactored low-power configurations and macros across multiple modules
  - Removed useless LP firmware mstatus FS bit enable
  - Fixed L1C API to provide default value for low-power mode

- **Peripherals**
  - Fixed PTP message type shift error
  - Fixed UART de-glitch causing abnormal RX FIFO data
  - Added DMA burst limit checks and BL618DG DMA1 64-bit width support
  - Fixed eFUSE region1 programming
  - Fixed local common trim read/write on BL618DG
  - Fixed LHAL ROMAPI patch enable when CONFIG_ROMAPI is set

- **Network**
  - Fixed MQTT publish retain flag to use correct constant
  - Fixed AT module compile error in CI test
  - Added uart recovery support in AT module

- **Build System / Core**
  - Fixed ELF overlap in coredump data restoration
  - Fixed coredump.py handling of missing sections
  - Fixed mtvec address alignment to 2^8
  - Fixed LD script sequence for PSRAM, nocache_ram, RAM, and WRAM
  - Fixed IR clock setting bug
  - Merged POSIX n900 and normal libraries
  - Fixed backtrace support for all chips, fixed Python 3.6 compatibility
  - Fixed FreeRTOS POSIX warnings

- **Zigbee**
  - Enhanced minimum free heap protection
  - Fixed general command response handling in OTA server

- **Thread**
  - Skipped OpenThread logging source for Matter configuration

### Improvements

- Refactored low-power configurations and macros across multiple components
- Updated FreeRTOS POSIX for new compiler warnings
- Updated LD file for MFG RAM version
- Removed unused code in LWIP
- Updated BL616CL HToL test firmware
- Updated BL618DG toolchain documentation
- Updated board audio PLL configuration for BL618DG

## v2.3.26 — since v2.3.25 (2026-05-11 → 2026-05-16)

### New Features

- **BL618DG (B0 Silicon)**
  - Updated boot2 image to v8.2.4
  - Updated OpenOCD to support BL618DG
  - Updated ADC v3 driver and examples for B0 silicon
  - Added PDS LP wakeup example
  - Brought up B0 BT/BLE support

- **BL616CL**
  - Added LP firmware phyrf library

- **WiFi (AP Mode)**
  - Added maximum associate station limit
  - Added deauth station API
  - Added automatic detection and deauth of lost stations

- **Audio**
  - Added JY6316 codec support

- **Peripherals**
  - Added GPIO-simulated MDIO support

- **Network**
  - Added DHCP server API to remove client by flag

- **Thread**
  - Added mbedTLS V3 config options

- **Tools**
  - Updated FlashCube to v1.4.2

### Bug Fixes

- **BL618DG**
  - Fixed PSRAM base address
  - Fixed EMAC0 GPIO pin configuration
  - Fixed cache consistency issue
  - Fixed mtvec mode setup
  - Fixed TZC CPU group setting and eFUSE read/write lock
  - Fixed low-power clock restore
  - Fixed mini misc clock ID
  - Fixed UART RX pin restore after PDS wakeup
  - Fixed encryption and signing support for B0 silicon

- **BL616 / BL616CL**
  - Removed duplicate FPU initialization

- **WiFi**
  - Fixed WRAM malloc abnormal memory size and counter limit
  - Fixed print format specifier in debug output
  - Fixed BTSDU read/write handling to use event handler instead of ISR
  - Fixed BA creation gating on recent traffic to prevent spurious negotiation
  - Fixed Linux driver to default to BL616CL with AMSDU enabled
  - Fixed counter clearing for subsequent loops in Linux driver
  - Made customer1 the default WLAN customer configuration

- **Bluetooth / BLE**
  - Fixed extended advertising support on BL702L
  - Updated hardware adaptations for BL618DG B0
  - Increased ACL TX buffer count for BL618DG and BL616
  - Switched BL702L to use driver API for time retrieval

- **Thread**
  - Simplified mDNS TXT record publishing
  - Fixed HTTP server file setup

- **Low Power**
  - Removed unnecessary LP firmware mstatus FS bit enable

- **Build System**
  - Added `CONFIG_GCC_COMPILE_LTO` to control LTO at build time
  - Fixed USB driver warnings when LTO is enabled
  - Unified mtimer address definition in FreeRTOS configuration
  - Merged libc and libc_n900 into a single library
  - Disabled POSIX I/O symbols in open-amp to avoid conflicts

- **AUPWM**
  - Updated test case for BL618DG

### Improvements

- Refactored Thread build system to use `CONFIG_OT_*` options
- APM now uses `reason_code` instead of `status_code`
- Updated stdatomic and thread examples for SPI-WiFi build compatibility

## v2.3.25 — since v2.3.24 (2026-04-26 → 2026-05-11)

### New Features

- **BL618DG (B0 Silicon)**
  - Added toolchain B0 support for new silicon revision
  - Added B0 boot2 build support
  - Enabled ROM driver support
  - Added phyrf B0 library
  - Added PDS BOD example
  - Added PDS app recovery and unified A0/B0 low-power paths

- **BL616CL**
  - Released boot2 v8.2.1
  - Added LP firmware phyrf library

- **WiFi**
  - Added STA index notification on association/disassociation (AP mode)
  - Added custom TX rate profiler support
  - Added retry rate min/max constraint configuration
  - Added retry rate range CLI and API
  - Added AMSDU RX/TX support (host and MAC)

- **Thread / 802.15.4**
  - Added CSL TX/RX support and enhanced-ACK improvements
  - Added CSL IE generation support, debug traces and build fixes

- **DHCP Server**
  - Added support for disabling router advertisement
  - Added API for removing client by MAC address

- **System**
  - Added SYSINIT framework for unified system initialization across all chips
  - Added support for custom heap order list
  - Added network interface auto-configuration
  - Added AP/STA selection CLI when ATModule is disabled

- **Peripherals**
  - Added CANFD driver and example
  - Added DMA enable and FIFO threshold for PEC QSPI camera
  - Added USB control-path VCHAN API
  - Added USB statistics information

- **Audio**
  - Updated audio output clock mHz API

- **Examples**
  - Updated PDS RTC example to use unified LP API
  - Updated IPC example for BL618DG
  - Added audio output clock mHz test

- **Other**
  - Reduced ADC key polling interval
  - Replaced `BL618DG_VERSION_A0` with `CPU_MODEL_A0`
  - Updated fw_post_proc to v1.4.2

### Bug Fixes

- **BL618DG**
  - Fixed MFG test for BZ antenna switch and TX power setting
  - Brought up B0 silicon manufacturing test support
  - Fixed DBI issue for BL616CL and BL618DG (DRV_1)
  - Fixed NP core ROM driver support
  - Set unprotect all slaves as default
  - Fixed incorrect interrupt number
  - Fixed USB initialization blocking

- **BL616CL**
  - Disabled DCDC by default
  - Added AES rw lock for slot3 and slot4
  - Removed flash SW AES key function for A0 silicon
  - Fixed power level configuration
  - Optimized PDS GPIO config and added AON IO delay note

- **WiFi**
  - Updated TWT feature for PDS15 low-power mode
  - Fixed rate control when performing single-rate mode testing
  - Fixed out-of-bounds access in replay_counter buffer
  - Fixed undefined reference for fhost_rc_set_rate export
  - Fixed 2.4 GHz/11g mode rate table export
  - Fixed duplicate AddBA handling during BA delete
  - Fixed eFUSE read steps for BL616CL
  - Fixed host TL compilation error
  - Fixed blmp command over ioctl

- **Network**
  - Fixed AT module main task stack overflow when enabling AP mode
  - Fixed custreg feature compatibility on some platforms

- **Build / Tooling**
  - Fixed macOS build failure
  - Fixed errno standard symbol conflict
  - Fixed heap overlap with RAM code in common linker script
  - Fixed missing symbols in ram.ld
  - Fixed `CPU_MODEL` not taking effect at build time
  - Added missing config symbols (`CONFIG_PEC_V2`, `CONFIG_MULTIMEDIA_VEDIO`, `CONFIG_DVP_RASTER`)

- **Boot2**
  - Enforced anti-rollback check and reject malformed XZ images
  - Added anti-rollback support for BL616CL

- **Audio**
  - Fixed audio config to resample properly
  - Fixed ALSA param to set player buffer size

- **Manufacturing**
  - Fixed mfg test commands
  - Fixed race condition during Touchuan mfg upload
  - Fixed Touchuan mfg to use only one SDIO port

- **Examples**
  - Fixed AON WDT example
  - Fixed queue/tree file name for backwards compatibility

- **Linker**
  - Updated BL618DG common linker script for B0 flash compilation

- **Solution**
  - Updated image transmission IP configuration

- **LP Core**
  - Fixed BL618DG LP core build by skipping drivers/sys

### Improvements

- Refactored supplicant_api.c for independence
- Moved low-power code to common location for better reuse across chips
- Unified LMAC154 and Thread examples configuration and build
- Moved Matter MFD component to common location across SDK
- Split HCI UART transport into reusable layers (Bluetooth)
- Made WiFi6 CLI module conditional on `CONFIG_SHELL`
- Enabled stricter warning checks in WiFi6 build
- Added BL616CL wireless configuration and parameter test
- Improved GPIO test scheme in manufacturing
- Added BL616CL country code support in wireless parameters
- Brought up BL618DG B0 BT/BLE support
- Reduced HTTP/OTA code size
- Changed default optimization level to `-O2`

## v2.3.24 — since v2.3.23 (2026-04-17 → 2026-04-25)

### New Features

- **NetHub**
  - Added USB data path support
  - Added configuration macros to disable ATModule and low-power mode
  - Disabled invalid log output

- **PEC / Display**
  - Added PEC v2 simulation SRGB LCD support
  - Added PEC SRGB basic example
  - Updated PEC v2 library

- **Solution**
  - Added DPI display output support
  - Updated UVC/UAC solution

- **BL616CL / Low Power**
  - Added RAM and ATE manufacturing test codes
  - Added external DCDC auto control support in standard driver
  - Added IO wakeup support in low-power firmware
  - Added clock gate control in low-power firmware
  - Added DCDC voltage switching support in low-power firmware
  - Added per-pin HBN wakeup trigger configuration
  - Extended tickless low-power support for BL616CL and BL618DG

- **WiFi (802.11)**
  - Made WiFi log output configurable at runtime
  - Added rate control GI adaptive controller for improved link adaptation
  - Added compile-time STA bandwidth limit configuration
  - AP mode now supports configurable beacon interval
  - Auto-disconnect when DHCP fails to obtain IP within 15 seconds

- **Thread / 802.15.4**
  - Enhanced ACK timestamp handling and compatibility
  - Enhanced ACK timing and TX/RX state handling
  - Aligned monitor function implementations
  - Added product appearance attributes support in Matter

- **Multimedia**
  - Added miniAlsa example
  - Refined miniAlsa API and xcodec integration
  - Added mm_track feature
  - Added codec debug CLI commands

- **BL616**
  - Added API for fine-tuning AUPLL SDMIN

- **Manufacturing**
  - Updated common RF trim parameters for BL616CL

### Bug Fixes

- **NetHub / BL618DG**
  - Fixed BL618DG low-power mode for NetHub

- **PEC / Display**
  - Fixed bandwidth issue with PEC v2 SRGB mode

- **DBI / Multimedia**
  - Fixed YUV format errors in DBI interface
  - Reverted ADC v2 changes that introduced regressions
  - Fixed NV12/NV21 UV block count rounding for half-block chroma

- **WiFi**
  - Filtered duplicate AP management frames
  - Fixed default GI initialization by mode and MCS
  - Fixed rate control sample-aware checks gated with transient window

- **WLAN MAC**
  - Fixed lost packet counter not being cleared on valid RX data
  - Fixed connection loss timer not being cleared properly
  - Fixed duplicate ACK sent on saved messages
  - Removed BT TX path conflict; added hopping TX

- **Zigbee**
  - Fixed IAS zone status not triggered when zone is not enrolled
  - Added default handler for received ZCL server response frames
  - Fixed error in `zb_getGroupTableByGroupId()` API
  - Fixed compile error due to interface changes

- **Low Power (BL616CL)**
  - Refined external DCDC low-power flow
  - Aligned wakeup trigger mode lookup for BL616CL
  - Refined IO wakeup DCDC flow
  - Aligned IO wakeup examples with new trigger mode configuration

- **Manufacturing**
  - Fixed touch chip version confirmation not being set
  - Fixed manufacturing test compile errors

- **Build / Tooling**
  - Fixed FOTA compile failure due to undefined `CONFIG_SHELL`
  - Fixed `bl_crt_bundle` build failure
  - Fixed BL618DG A1 compilation error

### Improvements

- WiFi MQTT now supports auto test framework
- Updated multimedia library to support BL618DG A1
- Removed `CONFIG_STRICT` from app defconfig
- Fixed compile warnings across multiple components (lwIP, host_router, macsw, wifi4 firmware/manager, wifi6, bl602, bl616CL, BL618DG)
- LHAL: Renamed BL616D references to BL618DG

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
