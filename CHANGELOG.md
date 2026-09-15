# CHANGELOG

## v2.3.34 — since v2.3.33 (2026-08-31 → 2026-09-15)

### New Features

- **WiFi / Networking**
  - Added scan-result reporting for access-point operating bandwidth and per-channel utilization
  - Added SoftAP authentication-failure reporting, including the station identity and failure reason
  - Added runtime configuration of a SoftAP static IP address
  - Added host heartbeat detection for NetHub connections

- **BL618DG**
  - Added regulatory power-limit support for 2.4 GHz and 5 GHz WiFi operation, including 80 MHz bandwidth
  - Added a low-power wakeup demonstration and preserved wakeup status across PDS recovery
  - Added a dual-core OpenAMP throughput stress-test example
  - Added Boya FQ256ES flash support and a keyscan v2 driver with example

- **Ethernet**
  - Added Ethernet PHY and network-interface control APIs, including independent EMAC receive and transmit shutdown

- **Security / Manufacturing**
  - Added selectable public-key hash support for BL618DG Boot2 image verification
  - Added BL616CL RF parameter storage in flash OTP for manufacturing flows
  - Added Kconfig options for selecting manufacturing-test features

- **Bluetooth / Zigbee**
  - Added a BL702L Bluetooth controller configuration with reduced memory use and expanded transmit buffering
  - Added Zigbee cluster extension support for additional customer implementations

- **Display / Memory**
  - Added DPI pixel-clock sampling-edge selection and display board-mode configuration
  - Added optional minimum-free-heap tracking

- **Clock / Power**
  - Added board-specific RTC 32K-source initialization for low-power applications
  - Added startup eFuse-trim diagnostics on supported boards

- **Tools / Build**
  - Extended the Web Serial flasher to support BL602, BL616, and BL616CL targets
  - Added Bluetooth configuration menus and support for appending menuconfig settings to defconfig files
  - Added an option to suppress MACSW log output

### Bug Fixes

- **WiFi / Networking**
  - Fixed scan entries expiring or retaining stale metadata when a matching access point is seen with a weaker signal
  - Validated SoftAP channel, bandwidth, security, and command-line parameters against device and country restrictions
  - Fixed iperf failures and assertions when the destination is unreachable, and restored compatibility with older iperf2 versions
  - Fixed receive-buffer handling for fragmented WiFi traffic
  - Preserved NetHub virtual-channel message types over USB and fixed BL618DG NetHub low-power wakeup

- **Bluetooth**
  - Fixed BR/EDR inquiry reporting after FHS-only results and protected ACL transmit buffers from concurrent access
  - Fixed A2DP connection setup, source-configuration failure handling, duplicate discovery, and Sniff-mode timing races
  - Corrected the BL618DG Bluetooth channel-assessment threshold
  - Fixed Bluetooth configuration when BR/EDR is enabled

- **Thread / Zigbee / 802.15.4**
  - Fixed active-scan transmission on the requested 802.15.4 channel and delivery of destination-less beacon frames
  - Corrected physical channel reporting to OpenThread and improved BL702L Zigbee frequency-offset calibration

- **Platform / Power**
  - Fixed BL618DG flash hangs caused by PSRAM clock handling and corrected PSRAM clock configuration
  - Restored USB device role before software reset and fixed low-power wakeup handling in BL616CL and BL618DG flows
  - Loaded RC32K eFuse trim data during startup for BL616, BL616CL, and BL618DG
  - Fixed BL618DG Boot2 ISP reset handling and IPC shared-RAM build configuration

- **Security**
  - Fixed BL618DG NP hardware-accelerated AES and SHA operations used by decryption and OTA requests

- **Linux WiFi Driver / USB**
  - Fixed scan-result handling on affected platforms and bootrom ID error reporting
  - Fixed USB zero-length-packet handling and Bluetooth coexistence cleanup

- **Display**
  - Corrected DSI v2 PLL settings and RGB565 framebuffer cache maintenance

### Improvements

- **WiFi Performance**
  - Improved BL618DG TCP benchmark throughput through WiFi, lwIP, and cache-layout tuning

- **RF / Low Power**
  - Updated BL618DG and BL616CL RF PHY libraries and low-power firmware
  - Updated BL618DG clock and display-PLL drivers for current silicon revisions

- **Build / Configuration**
  - Unified board lookup, firmware packaging, and flash-image configuration across CMake projects
  - Packaged Zigbee linker fragments for component-based builds
  - Enabled no-relax compilation for WiFi 6 builds
  - Avoided rewriting generated configuration files when their content is unchanged

- **Tools**
  - Updated the firmware post-processing tool to v1.4.7

## v2.3.33 — since v2.3.32 (2026-08-18 → 2026-08-31)

### New Features

- **WiFi / Coexistence**
  - Added runtime WiFi, Bluetooth, BLE, and 802.15.4 coexistence controls, including Bluetooth RF-path configuration
  - Added WPS push-button configuration for a target BSSID and operating frequency
  - Added monitor-mode channel switching and beacon/probe-response frame callbacks
  - Enabled SoftAP in the TCP benchmark profile

- **BL618DG**
  - Added dual-EMAC support with independent network interfaces and diagnostics
  - Added BLE low-power firmware support and wireless wake scheduling
  - Added manufacturing AT commands for eFuse and GPIO testing

- **Bluetooth**
  - Added Bluetooth coexistence priority APIs and BR/EDR sniff mode support
  - Added a BL618DG NimBLE example

- **Display**
  - Added AXS15231E JX371 and EK79007 WKS70WSV114 MIPI DSI LCD drivers

- **Flash / Security**
  - Added automatic two-line flash fallback and an anti-rollback enable query API
  - Added Boot2 support for per-device application encryption after signing
  - Added a browser and command-line Web Serial flasher for BL616 whole-image binaries

- **Zigbee / 802.15.4**
  - Added a Zigbee API to update attribute configuration
  - Added support for honoring the software frame-pending bit in 802.15.4 frames

- **Memory**
  - Added an API to query the largest contiguous free memory block

### Bug Fixes

- **WiFi / Networking**
  - Fixed connecting to a requested SSID when scan results contain multiple matching networks
  - Corrected default network-interface selection and country configuration handling
  - Fixed UDP iperf server buffer allocation and critical WiFi transmissions using unsupported legacy rates
  - Fixed NetHub host commands
  - Preserved HTTP connection handles and drained pending TLS data before polling

- **Bluetooth**
  - Fixed AVDTP OPEN failures when the transaction ID wraps
  - Fixed HFP SCO codec handling and added A2DP sink statistics

- **Thread / Zigbee**
  - Fixed OpenThread Border Router forwarding, mDNS, NAT64, REST, UDP, locking, and settings issues
  - Corrected Zigbee APS data indication RSSI and LQI values
  - Fixed a Touchlink crash

- **USB / Peripherals**
  - Added support for additional USB Audio Class sample rates
  - Improved USB console logging efficiency
  - Fixed PCA8553 symbol display in the I2C example

- **Platform**
  - Fixed two-line flash wrapping on BL618DG
  - Fixed Linux WiFi driver compatibility with newer kernels

### Improvements

- **Build / Configuration**
  - Centralized SDK, board, and component configuration handling

- **WiFi**
  - Prioritized WiFi-affinity code placement to improve performance

- **Solution Components**
  - Consolidated reusable audio, camera, display, USB, and streaming utilities into SDK solution components

## v2.3.32 — since v2.3.31 (2026-08-10 → 2026-08-17)

### New Features

- **Build / Configuration**
  - Integrated menuconfig and added component configuration menus for file system, graphics, multimedia, and AT module

- **Peripherals / Drivers**
  - Added DAC reference voltage configuration API
  - Added PEC infrared (IR) receiver support

- **USB**
  - Added USB console logging for cases where the serial TX/RX is occupied

- **Networking**
  - Added mDNS configuration to the uart_wifi example
  - Reconstructed iperf with TCP/UDP raw and socket backends and updated documentation

- **Bluetooth**
  - Added per-device BR/EDR inquiry result callback
  - Enhanced BR/EDR diagnostics and media control

- **Clock / Power**
  - Added predictive coarse calibration with half-MSB support for RC32K, with half-MSB state restored on low-power wakeup

- **Display**
  - Updated LVGL OSD example and added ST77922, ST7701P, and ST7102 LCD drivers

- **MFG**
  - Added BL618DG RF calibration flash, OTP, and eFuse operations

- **Flash**
  - Added IS25LP01GJ flash support for BL616

### Bug Fixes

- **WiFi**
  - Fixed rate control time integer wrap-around
  - Rejected duplicate Block ACK frames before upload
  - Exported channel switch state
  - Fixed TX stalls after channel scans
  - Extra information elements are now propagated for scan and association
  - Firmware status is now checked before TX
  - Fixed AP DHCPD client release when a station disconnects
  - Fixed SDU RX indication triggering a heavy load loop

- **Networking**
  - Fixed multiple DHCPD issues in lwIP

- **System / RTOS**
  - Fixed tick compensation overflow during long RTC sleep
  - boot2 ISP now uses chip-specific bootrom versions

- **Power**
  - Removed invalid BLE EM override in the low-power example
  - Adapted the RTC driver for BL616CL

- **BL618DG**
  - Fixed SDH timeout judgment
  - Fixed MFG SPDT configuration

- **Zigbee / 802.15.4**
  - Fixed BL702L private rate hardware ACK reception issue

- **Flash**
  - Added support for large-capacity JEDEC IDs
  - Normalized flash pin configuration for BL616CL/BL618DG

- **Linux Driver**
  - Fixed ioctl path handling on newer kernels
  - Fixed MFG TCAL setting and USB/SDIO PID debug

- **MFG**
  - Fixed BL616CL temperature calibration USB PID

### Improvements

- **WiFi / Networking**
  - Added a high-performance TCP demo
  - Increased TCP benchmark RX buffers

- **Bluetooth**
  - Updated Nimble throughput handling for BL618DG

- **Drivers**
  - Unified the UART driver into a single runtime-configurable driver

- **SDH**
  - Removed the workaround for the SDH hardware issue

- **MFG**
  - BL616CL MFG build now supports an autoboot flag

## v2.3.31 — since v2.3.30 (2026-07-27 → 2026-08-10)

### New Features

- **OTA / FOTA**
  - HTTP/HTTPS/TCP OTA commands now support a reboot option, allowing the current firmware to keep running after a successful update
  - Active OTA partition is marked immediately, so multiple updates can be installed before rebooting
  - Fast OTA mode can now be enabled or disabled at build time

- **USB**
  - Updated USB protocol stack to v1.6.1
  - Expanded USB CLI demo with device/host templates, Ethernet adapters, and documentation
  - Added USB port unregister support

- **Camera / Display**
  - Added 720P GC2145 camera to MIPI DSI LCD example
  - Added GC0308 camera with ST77926 LCD support to camera/LCD example

- **Networking**
  - Nethub on BL616/BL618DG now automatically selects the SDIO or USB interface

- **Power**
  - Added runtime power mode configuration for BL616CL, including matching LPFW voltage parameter updates

- **BL618DG**
  - Added SPDT switch control support
  - Added BLE LP firmware (LPFW) API

- **System / RTOS**
  - Added a configurable threshold to flag tasks that remain in ready state for too long

- **Flash**
  - Added IS25LP01GJ flash support for BL618DG

### Bug Fixes

- **OTA / FOTA**
  - OTA start now rejects duplicate invocations
  - Serialized SHA mode switching during OTA
  - Fixed BL602 OTA stability by reworking RAM layout and keeping SHA link descriptors in OCRAM

- **WiFi**
  - Fixed BL618DG WFA memory overlap by isolating Wi-Fi buffers in dedicated RAM with linker checks
  - Disabled high ISR stack for BL618DG WFA builds
  - Fixed reflected station packets being processed after deaggregation
  - Fixed BL616 iperf assert and TCP RX resource lifetime issue
  - Fixed retry attempts when the virtual interface is disabled or disconnected
  - Fixed scan adding entries from the wrong PHY channel

- **BL616CL**
  - Fixed MFG temperature sensor and PDS API issues
  - Fixed MFG SDIO/USB compile errors
  - Fixed MFG Wi-Fi TX and RF temperature calibration loop
  - Fixed temperature sensor offset values on BL616CL and BL618DG
  - Fixed temperature sensor issues in ADC v2/v3 examples

- **BL618DG**
  - Disabled Wi-Fi PLL fine-tuning on B0 silicon
  - Fixed CPU PLL clock calculation using floating-point
  - Fixed EMAC clock invert bit
  - Fixed channel data leftover handling

- **Bluetooth**
  - BLE stack now uses runtime local ATT MTU for GATT exchange

- **Zigbee / 802.15.4**
  - Fixed TCLK callback being invoked while erasing TCLK
  - Corrected BL702 monitor timeout calculation

- **USB**
  - Updated USB descriptor API usage in AT module
  - Removed redundant USB host serial source

- **Demo / Examples**
  - Enabled EasyFlash support in nethub demo

### Improvements

- **WiFi / Networking**
  - Optimized nethub Wi-Fi throughput with hot-code placement and build tuning
  - Updated Wi-Fi MAC default configuration for BL618DG
  - Gated nested IRQ handling on high ISR stack
  - Tuned combo memory usage to free more RAM for other components
  - Updated Wi-Fi MAC integration for the new ADC temperature sensor API on BL616CL

- **Bluetooth**
  - Allowed external control of BLE simple logging on BL702L
  - BLE now tracks whether the next wake is for event programming
  - Resolved BLE stack symbol name conflicts

- **RF**
  - Updated RF PHY libraries for BL616 and BL616CL

- **BL618DG**
  - Unified PDS and jump app paths in low-power firmware example

- **System**
  - Moved small read-only data to TCM to reduce RAM usage

- **Tools**
  - Added support for Zephyr toolchain
  - Updated firmware post-processing tool to v1.4.4

## v2.3.30 — since v2.3.29 (2026-07-15 → 2026-07-27)

### New Features

- **WiFi**
  - Added retry count limit support
  - Added MAC TX and RX test support in MFG mode
  - Added RX/TX flow control dump for debugging
  - Added BL616CL CCA threshold and AC1 REC adaptive control

- **Display / LVGL**
  - Unified LVGL display layer and pin initialization across boards
  - Added ACM as data source and ECM for BL618DG LVGL demo
  - Added PEC DVP camera to MIPI DSI LCD example

- **Camera**
  - Merged PEC DVP CAM HSYNC sampling mode into hsync_dly

- **Zigbee / 802.15.4**
  - Updated Zigbee example for Zigbee component release
  - Added 802.15.4 and MAC TRX to configuration

- **BL618DG**
  - Added BLE LP firmware info recording and query API
  - Implemented hardware K 32K interface
  - Added xtal 32K counter APIs

- **Debug / Tools**
  - Reduced backtrace cost stack size using binary search in unwind table

- **Flash**
  - Added XM25UH32D flash support for BL616CL

- **RF OTA**
  - Added RF OTA test firmware example

### Bug Fixes

- **WiFi**
  - Fixed memory leak when WPA send fails
  - Fixed Nethub SDIO low UDP TX throughput and zero data drops
  - Fixed off-by-one when disabling LWIP TX single pbuf
  - Fixed credits handling in TX flow control
  - Fixed empty HT MCS map handling in rate control
  - Fixed BL618DG errno TLS issue in coexistence CLI
  - Fixed AP country IE advertisement and deferred stop event
  - Fixed VHT beamforming capability
  - Fixed join scan not finding BSSID in first round
  - Fixed CCA offset for SRRC compliance
  - Fixed hardware MAC overwriting register on reset
  - Fixed TX policy preservation across A-MPDU transitions
  - Fixed RTS protection rate selection per RC step
  - Fixed internal frame fallback rates initialization
  - Fixed NULL notification for 70ms duration

- **Low Power**
  - Fixed low power assert

- **BL616CL**
  - Fixed power compensation API error

- **Audio**
  - Fixed audio example issues for BL618DG and BL616CL

- **Camera**
  - Updated cam_lcd for multi-chip compatibility

- **Bluetooth**
  - Fixed crash in BLE PDS init for BL702L
  - Fixed BLE/BT power minimum value for BL616L and BL618DG

### Improvements

- **Networking**
  - Optimized TCP TX memory efficiency
  - Added LWIP_TCPIP_FORCE_TX_COPY configuration option
  - Fixed netstat TCP memory usage reporting

- **WiFi**
  - WPA hexdump now always prints for easier debugging
  - Raised WiFi connect task priorities
  - Optimized zero-copy RX path for pbuf handling

- **BL618DG**
  - MFG mode now supports negative BT/BLE TX power values
  - Updated RF switch GPIO for EVT2 hardware
  - Removed unused temporary RF code

- **System**
  - Cleaned up AT module code for improved robustness
  - Removed LP monitor context sections from linker scripts
  - Added DVP I2C configuration macros for board GPIO headers

## v2.3.29 — since v2.3.28 (2026-06-26 → 2026-07-15)

### New Features

- **BL618DG**
  - Consolidated BZ coexistence demo

- **Camera**
  - Added BF3901 sensor and PEC DVP CAM capture support

- **WiFi**
  - Added BLE WiFi coexistence tests (WFA)
  - Added P2P (WiFi Direct) feature

- **Networking**
  - Optimized IPv6 support for low power scenarios

- **Display**
  - Improved LVGL demo cases and DSI screen support

- **BL616CL**
  - Added GPIO interrupt watchdog test case

- **Debug / Tools**
  - Updated OpenOCD to support CJTAG
  - Board serial log output can now be disabled

- **Manufacturing**
  - Added TX power compensation support in MFG mode

### Bug Fixes

- **WiFi**
  - Adapted SPI WiFi fake write API
  - Fixed TWT tick calculation
  - Fixed WFA IRQ priority setup for BL618DG B0
  - Fixed UART signal issue after boot2 log output
  - Fixed support for more chip variants in WiFi low power flash config

- **Audio**
  - Fixed loop playback crash

- **Low Power**
  - Refined RC32K coarse calibration

- **Build System**
  - Fixed `make` and `ninja` first-time build failure
  - Fixed compilation issues for SDIO/IPC on BL602 and BL618DG
  - Fixed compilation issue for BL616CL
  - Fixed various build failures

- **BL618DG**
  - Fixed WiFi-BLE switching in MFG mode
  - Fixed power offset issue when no channel switch occurs in MFG

- **WS2812**
  - Fixed WS2812 LEDs only displaying blue

### Improvements

- **WiFi**
  - Consolidated P2P code paths, retired legacy `CONFIG_WL80211_P2P` config
  - Optimized WiFi TCP throughput
  - Added WPA failure point logging for easier debugging
  - Removed legacy "FW0" residue references throughout codebase

- **BL618DG**
  - Updated MFG version to 0.5
  - Added RCAL RF parameter
  - Separated lpfw_private library for ILP32D
  - Demo now selects standalone or combo path based on macro

- **System**
  - Updated hibooster startup method
  - Updated linker flash rodata object patterns

## v2.3.28 — since v2.3.27 (2026-06-07 → 2026-06-26)

### New Features

- **BL616CL**
  - Added USB enumeration as EMAC, HID, and UAC (USB Audio Class)
  - Updated PhyRF library

- **BL618DG**
  - Added integrated LP firmware entry point
  - Added PSRAM hybrid sleep support
  - Added PSRAM PDS recovery and RTC test
  - Updated PhyRF library

- **Display / Graphics**
  - Added DSI v2 panel framework and LVGL v9 OSD demo
  - Updated PEC QSPI camera example

- **Audio**
  - Added AC101 codec driver
  - Added AUADC peripheral support and example

- **WiFi**
  - Coexistence examples now support BL616, BL616CL, and BL618DG
  - Added P2P configuration header in MACSW
  - Added log output when AP deauthenticates a station

- **Bluetooth / BLE**
  - SPP now supports up to 7 concurrent connections (was 1)
  - Added continuous TX codes 0xC0-0xC7 for testing

- **Thread / 802.15.4**
  - Updated debug and neighbor functions for BL618DG

- **Matter**
  - Added API to retrieve eFUSE AES IV via MFD

- **Build / Core**
  - Added firmware boot success marker in partition support
  - coredump.py now supports BKCD binary format
  - `settimeofday` marked as weak symbol in libc
  - Added CANFD CIA603 and TTCAN examples

- **Manufacturing**
  - Added ADC temperature sensor function for HToL test

### Bug Fixes

- **BL618DG**
  - Fixed PDS sleep time calculation
  - Fixed PDS GPIO interrupt trigger types
  - Fixed HBN IRQ status preservation during PDS enable
  - Fixed HBN DCDC and power configuration
  - Fixed WL LP IO wakeup support
  - Fixed LP softirq restore after wakeup
  - Disabled all interrupts before B0 silicon sleep
  - Fixed forwarded IRQ masking at boot (unmasked on attach)
  - Removed NP JTAG pinmux to avoid conflicts
  - Fixed BLE RX failure at sniff anchor point
  - Fixed BLE EBQ issues on B0 silicon
  - Enhanced Bluetooth and 5G coexistence

- **BL616CL**
  - Fixed BLE throughput rate and average display

- **BL602**
  - Fixed XIP access collision when clearing HBN IRQ pending

- **WiFi**
  - Fixed WEP encryption restore in low-power mode
  - Fixed AP deauth triggered by transient unexpected frames
  - Validated MACSW configuration counts
  - Fixed keyram dump to reflect exact stored content
  - Fixed UF buffer configuration build
  - Set A-MSDU supported bit in ADDBA request
  - Updated WL LP and wl80211 flow
  - Removed deprecated configuration code

- **Bluetooth / BLE**
  - Fixed discovery restart after stop during name resolution (BR/EDR)
  - Fixed connection reference issue in central demo
  - Fixed BLE EBQ issues across multiple chips

- **Display / Graphics**
  - Updated DPI framebuffer case handling
  - Updated OSD and DVP raster driver

- **Low Power**
  - Optimized and refactored low-power receive BCMC feature
  - Fixed WEP encryption key restoration during LP resume

- **Build System**
  - Fixed `CONFIG_GCC_COMPILE_LTO` incorrectly applying to board, flash PSRAM, and startup code
  - Set BL618DG default optimization level to `-O2`
  - Generated VERSION files for WiFi6 and MACSW libraries

- **Peripherals**
  - Fixed DMA burst size limitation
  - Prevented I2S channel misalignment by enabling I2S after DMA start
  - Preserved eFUSE lock word when programming bit 31

- **Thread**
  - Guarded idle check against NULL instance
  - Avoided idle sleep when TX frame is pending

- **Shell**
  - Reserved autolist read only (tab-completion improvement)

- **Examples**
  - Fixed compile error on wl_ble_lp example
  - Updated BL618DG HBN RTC example

### Improvements

- Refactored low-power receive BCMC feature across system drivers for better maintainability
- Updated HToL TX test configuration
- Improved cross-platform build compatibility

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
