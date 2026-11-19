# CHANGELOG

## v2.3.39 — since v2.3.38 (2026-11-04 → 2026-11-19)

### Breaking Changes

- **SDK initialization**: Fixed SDK startup now runs through the SYSINIT framework on all supported chips, so console, heap, flash and partition setup, filesystems, RF and Wi-Fi bring-up, Bluetooth storage, TCP/IP and the C++ runtime are all initialized automatically before the application starts. Applications must remove their own calls to the managed initialization entry points and must not start the RTOS scheduler manually; legacy calls are still accepted during the transition but only print a deprecation warning, no longer re-run initialization, and will be removed in SDK v3. Startup settings that applications previously applied from `main()` before bringing up Wi-Fi are now taken from build configuration or board initialization.

- **Wi-Fi 6 / Bluetooth coexistence (BL618DG)**: The board RF topology — combo shared path, standalone dual antenna, or standalone single antenna with an SPDT switch — is now a build-time selection that is prepared during startup. The previous runtime topology commands are retired, and a coexistence build without a topology selection now fails at build time instead of running an unknown RF path.

- **Display**: The legacy BSP LCD framework has been replaced by the unified display framework. Legacy LCD drivers and LVGL v8/v9 examples are still provided as a deprecated compatibility path that emits deprecation warnings and is scheduled for removal in the next major release; existing LCD applications should migrate to the display framework and the display examples.

- **Audio devices**: The legacy codec interface is deprecated in favor of a unified audio-device layer that covers I2S codecs, the built-in audio ADC/DAC and PDM microphones.

- **Wi-Fi 6 MAC profiles**: The MAC software profile is now selected from a fixed set of named profiles instead of a free-form string, so project configurations that set a string value must select the equivalent named profile.

- **Flash tooling**: Generated flash programming configurations now program a backup copy of the partition table as well as the primary copy, and partition layouts must define the backup address; custom partition tables without one are rejected when the programming configuration is generated.

### New Features

- **Wi-Fi**
  - Added a three-port WDS bridge on BL618DG that forwards traffic between Wi-Fi and both Ethernet ports, with independent DMA rings, queues, PHY and link state per port, PSRAM-backed receive buffers, a larger forwarding database and per-port diagnostics
  - Added bridge forwarding-database inspection APIs to look up a port, resolve the destination ports for an address, and iterate learned and static entries with their remaining lifetime
  - Added per-station statistics reporting: last RX/TX PHY rate with format, MCS, bandwidth, spatial streams and guard interval, MPDU counters, per-TID counters and receive-discard events, with reports rejected when collected across station-slot reuse or counter resets
  - SoftAP security is now configurable with management-frame protection (disabled, optional or required), SAE password-element handling, Transition Disable, distinct WPA/WPA2 mixed-mode advertisement and validated WEP hexadecimal keys in non-HT mode; U-APSD is off by default and disabling WMM forces legacy 20 MHz operation
  - Added access-point SSID hiding: beacons carry an empty SSID, wildcard probe requests are not answered and the WPS information element is omitted; configurable from the access-point start API, the CLI and AT firmware
  - Raw 802.11 frame injection now supports 5 GHz channels on dual-band chips, validating the channel against the configured channel plan, rejecting disabled, passive and radar channels, and using the channel power limit and a valid dwell time
  - Wi-Fi Alliance tooling: access-point transaction streams can be limited by frame count and reset, stopping a stream interrupts active senders instead of hanging, the UDP echo server can be stopped and restarted, and shell commands accept more option/value pairs
  - Added control over HE dual-carrier modulation advertisement; it is disabled by default and the chip defaults can be restored before Wi-Fi starts

- **Bluetooth**
  - Added optional BLE Signed Write support, with write callbacks informed whether a write carried a verified signature and signed write commands on encrypted links discarded as the specification requires
  - Added BL616CL controller library variants for up to 4 simultaneous connections with power-down sleep, and for one connection in peripheral role while scanning

- **Audio**
  - Added a unified audio-device layer covering I2S codecs, the built-in audio ADC/DAC and PDM microphones, with channel selection, partial-frame transfers, driver-default DMA buffering and shared I2S full-duplex operation; an echo demo exposes volume, mute, channel, tone and register commands
  - Added board audio adapters for BL616, BL616CL and BL618DG Orion boards, and support for a JY6416 codec with two JY6166 ADCs on a six-slot PCM bus
  - Added multi-slot recording and playback with SD card storage
  - Unified speaker and microphone channel control across the direct and PCM audio frontends, with per-channel gain in dB and independent microphone mute

- **Display / Graphics**
  - Added a unified display framework for GRAM-transfer and VIDEO scanout panels, separating panel IC and module configuration, bus access, scanout backends and board GPIO hooks, with explicit frame lifecycle events and a software bitmap font renderer
  - Added runtime DBI wire-width control (1, 2 or 4 lanes) and DPI input switching with BT.601 full or limited YUV range selection
  - Added a ST7102_YDP395BT008 MIPI-DSI panel and corrected the AXS15231E 258x960 panel configuration
  - The LVGL v8 and v9 display ports now render through the unified display framework, with partial GRAM updates for SPI/DBI panels, buffer reuse on flip for RGB/MIPI-DSI panels and touch ranges that follow the active display
  - Added BL618DG Touch V2 examples for self-capacitance interrupt and continuous scan and 4x4 mutual-capacitance, with shared calibration, thresholds and channel diagnostics

- **AT command interface**
  - Added an optional AT 2.0 command core that can be selected at build time, with AT 1.0 remaining the default
  - AT 2.0 adds sleep modes and timed deep sleep, light-sleep GPIO wakeup, RF calibration, register access, OTA image rollback, system timestamp, regular-expression result filtering, a configurable pass-through interval and UART parameters changeable at runtime
  - AT 2.0 adds a factory-data partition: a parameter file is converted into a factory key-value image during the build and loaded at startup, with commands to read, write and erase the partition
  - AT 2.0 pass-through mode accepts the escape sequence only after a guard time, so payload received together with the escape is processed as commands instead of being dropped

- **WebAssembly**
  - Enabled AOT execution on BL616CL
  - Added a runtime AOT module flow that lists modules, downloads them over HTTP(S), installs them into a flash AOT partition and runs them from flash or RAM, with a Wi-Fi station helper for downloads
  - Added XZ-compressed AOT images that are stored compressed and decompressed on install or run to save flash space and download time
  - Added a CoreMark benchmark comparing native, interpreter, AOT, dynamic AOT and execute-in-place AOT execution

- **USB / Matter**
  - Added Android Debug Bridge device support, with a demo that enumerates as a standard ADB interface and provides an interactive shell and one-shot commands over serialized, error-checked transfers
  - The Matter factory-data module now supports BL618DG, including its flash layout and image offsets

- **Flash / Boot / Boards**
  - BL616CL: boot2 now write-protects the first 56 KB while still allowing partition and application updates
  - Flash initialization now falls back to two-line mode automatically when the configured quad mode is not validated, enabled by default
  - BL618DG: the flash loader answers GET_BOOTINFO with the BootROM-compatible payload without entering the ROM ISP
  - Added the BL618DG Sirius audio-video board and a BL618DG dual-core Wi-Fi/Bluetooth/802.15.4 coexistence test example

- **Build / RTOS / libc**
  - SDK components can be delivered as separately supplied extensions that are enabled without modifying the SDK tree, and Kconfig now tolerates absent optional components
  - Added C++ condition variable support to the FreeRTOS POSIX layer, including pthread cancellation, thread-local storage keys and key destructors
  - Added an alias-safe fast memset implementation, Picolibc system types when SDK POSIX is disabled, and Matter support without Thread for Wi-Fi and Ethernet applications

### Bug Fixes

- **Wi-Fi**
  - After a station restart, the station now notifies the access point of its presence immediately after association, so an access point that kept the previous power-save state no longer stalls the WPA handshake with buffered EAPOL frames
  - Management-frame protection: SA Query responses now follow the protection policy instead of being marked protected without carrying a security header
  - Scanning: the associated access point's beacons are now included while connected, and explicit user-requested scans clear earlier results
  - Rate control: 40 MHz stations now use the matching 40 MHz legacy rate table instead of the 20 MHz table
  - 802.11k radio measurement: beacon measurement requests for 80, 160 and 80+80 MHz are parsed with the correct band, channel and operating class, and malformed subelements are rejected before use
  - 5 GHz station reporting now shows the actual negotiated link mode instead of the configured mode, including for 5 GHz Wi-Fi 6 links; the negotiated mode is also available from a new station link-mode query, and raw frame transmission selects the correct band policy
  - Fixed access-point HT20 capability advertisement, which claimed 40 MHz-only capabilities and an incorrect maximum rate
  - Fixed frame corruption when an access point resends a buffered frame to a power-saving station by building the headers in the private transmit copy
  - Fixed possible memory corruption during Wi-Fi 6 host initialization by allocating the copy transmit pool from the Wi-Fi RAM heap
  - Access-point station information and deletion now validate the station index and output pointer and propagate errors instead of accessing invalid table entries
  - Management frames with a truncated 802.11 header are now rejected before parsing
  - Fixed truncation of maximum-length SSIDs and 64-character hexadecimal passphrases in the string-based station connect API, which now also reports the real result of automatic channel selection
  - Fixed a crash during WPA3-SAE authentication when the access point requests an anti-clogging token
  - Fixed a BL616 WLAN firmware DHCP failure caused by a PHY calibration workspace overflowing into adjacent retained memory; the same latent overlap on BL616CL was also fixed
  - Fixed compilation failures in reduced and strict build configurations, including minimum Wi-Fi feature sets, IPv6 builds and warning-as-error builds
  - Fixed a C++ compilation error when the Wi-Fi MAC public header is included from C++ sources

- **Bluetooth**
  - BL616: fixed corruption of the Bluetooth non-retention heap after low-power wakeup when it is dynamically allocated
  - Receive-clock drift sampling is now enabled only after the low-power firmware has initialized, so applications that do not use it no longer read invalid timing data
  - BL616CL: fixed a manufacturing firmware build failure caused by a character-output macro expansion conflict
  - BLE throughput test: receive throughput is now reported in Kbps with warm-up traffic excluded, corrected averages, a final per-connection summary and correct totals beyond 4 GiB

- **Security / HTTPS**
  - The certificate bundle lifetime is now reference-counted, its CA chain is cleared on detach, and replacement is refused while connections still use it
  - Certificate bundle lookup now matches full issuer names and verifies leaf-plus-intermediate chains against a root-only bundle
  - HTTPS connections now fail instead of silently disabling peer verification when no trust anchor is available
  - HTTP Host headers now include non-default ports

- **RF / Power**
  - BL618DG: fixed Bluetooth-only builds reporting advertising success while nothing was transmitted, by re-applying the standalone RF path calibration and TX power after the path switch
  - BL618DG low-power firmware: fixed late advertising events not being transmitted, deriving the advertising delay and late-event budget from the configured timing precision and interval
  - BL618DG: fixed startup by programming the required DCDC and SOC LDO voltages during early initialization
  - BL618DG: fixed CPU PLL stability after frequency changes by resetting the PLL and allowing it time to lock
  - Fixed the BLE low-power example build on BL616 and its RF re-initialization on BL618DG

- **Audio**
  - Fixed fixed-point math in the bundled audio algorithms so large shifts no longer rely on undefined overflow, extreme small values clamp to zero and buffer reallocation failures no longer lose state
  - Fixed echo cancellation to keep the exact sampling rate above 32 kHz, such as 44.1 and 48 kHz
  - Fixed the Orion six-slot JY6416 topology
  - Reduced audio memory use and fixed the recording flow
  - Removed the unverified BL616CL codec board adapter and example instructions that claimed built-in audio support

- **Peripherals**
  - BL618DG: added ADC low-voltage calibration based on internal ground and 20 mV reference measurements, with each sample corrected by the calibration of the unit that produced it
  - BL616CL/BL618DG: fixed differential ADC offset sign handling so negative readings are no longer clamped to zero
  - BL616CL: fixed low-voltage ADC offset compensation and disabled the internal 20 mV calibration reference after calibration
  - I2C: after a NAK or arbitration loss the driver now waits for the current transaction to end before disabling the controller instead of shutting it down while the bus may still be active

- **Media / Display**
  - Completed AVE and MStar media board defaults, including microphone gain, display DBI hooks and panel profiles
  - Reduced image sensor memory use by storing initialization tables in read-only memory

- **Storage / Matter / C++**
  - Fixed the EasyFlash-compatible key-value API so it can be used from C++ without enum-conversion errors
  - Fixed a Matter factory-data build issue caused by relying on transitive NULL definitions
  - Fixed C++ runtime startup and linker configuration

- **Host drivers**
  - The Linux host driver's SDIO interrupt handler now schedules packet processing on the driver workqueue instead of running it in interrupt context
  - The NuttX host driver now transmits access-point traffic on the access-point interface instead of always using the station interface

- **Manufacturing**
  - BL618DG: fixed in-RAM manufacturing firmware linking and runtime, the 154ANT command and flash two-line fallback
  - BL616CL: fixed the manufacturing firmware linker configuration

### Improvements

- Updated the BL618DG low-power firmware to v2.5.5, refreshed its RF PHY library so saved Wi-Fi channel calibration is restored after wakeup and WLAN clock initialization is skipped in Bluetooth/802.15.4 modes, and reduced power use by gating unused clocks and disabling unused initialization records
- Reduced receive-buffer memory when A-MPDU reception is disabled, cutting the BL616CL receive pool from about 41 KB to 6.4 KB
- Reduced BL616 WLAN firmware OCRAM use by moving transmit descriptors to Wi-Fi RAM and using one fewer transmit buffer
- Removed the dependency on the compiler's short-enum setting from the wireless and RF libraries, so their structures no longer change layout between components built with different flags
- lwIP now builds optional mDNS, TCP-over-TLS and IPv6/DHCPv6 support only when the corresponding options are enabled, and Wi-Fi builds use a larger packet link headroom
- Enabled an alias-safe fast memset option and turned it on in the I2S codec example to reduce repeated PCM buffer clearing cost
- Removed duplicate PCM block clearing by filling valid TDM slots directly and zeroing only the remaining tail
- Increased the default shell argument capacity from 16 to 20
- Increased the transmit descriptors reserved for the video access category from one to four in the Wi-Fi Alliance certification profile
- The station connect CLI now requests management-frame-protection-capable operation by default
- Legacy-only 802.11b/g/a builds now omit disabled HT, HE and VHT capabilities from beacons and association requests and release unsupported receive aggregates instead of forwarding them
- Console USB identity now uses the standard CDC descriptor set with VID/PID read from eFuse and no serial number on BL616CL and BL618DG
- Moved the BL616 low-power firmware sources into a common directory

## v2.3.38 — since v2.3.37 (2026-10-19 → 2026-11-04)

### Breaking Changes

- **Toolchain**: The SDK has switched to GCC 14.3 with Picolibc for all supported chips. The Xuantie GCC and Zephyr GCC toolchains are deprecated. Migrate local development and CI environments to the GCC 14.3 toolchain before upgrading to this release

### New Features

- **WiFi / Networking**
  - Added a four-address WDS Ethernet bridge with station and access-point support
  - Added per-band WiFi rate-control algorithm selection and a CLI to switch algorithms
  - Made the WiFi 4 scan-cache capacity configurable
  - Added configurable RPMsg buffer sizes and optional raw-mode iperf support for dual-core networking; both cores must use matching buffer sizes

- **WebRTC / RTSP**
  - Added authenticated, nonblocking TURN-over-UDP transport for WebRTC relay connections
  - Added an optional direct-first ICE nomination policy and exposed the selected local and remote ICE candidate types
  - Added typed, per-stream DataChannel messaging
  - Added optional timestamped audio playout with packet-loss concealment and RTP media diagnostics
  - Added an RTSP server for JPEG video and G.711 A-law audio

- **Storage / Flash**
  - Added SPI-NAND storage support with Dhara flash translation and FATFS integration
  - Added support for IS25LP/WP01G and XM25QH512D flash devices
  - Added BL618DG support for loading RF calibration data from flash OTP

- **Bluetooth**
  - Added a BL616CL Bluetooth controller library supporting up to 10 connections

- **Audio / Peripherals**
  - Added I2C interrupt-mode transfers and examples
  - Added per-channel microphone PGA gain control and capture sequence numbers for detecting dropped frames
  - Added I2S frame-sync polarity control

- **Graphics / Examples**
  - Added HTTP OTA updates and a resolution-adaptive UI to the LVGL 9 OSD example
  - Added an optimized GIF player for LVGL 9 with display output options
  - Added phone remote-control input through BLE writes for USB keyboard and microphone forwarding on BL616/BL616CL; the BLE/USB router example now uses GATT Write/Write Without Response, so existing clients must update their input flow and rediscover services
  - Added configurable transmit memory thresholds and receiver acknowledgement intervals to the image-transmission solution

- **Build / Debug / Manufacturing**
  - Expanded C++ runtime support, including startup and POSIX compatibility functions
  - Added BL618DG reset and reconnect support to OpenOCD
  - Added RAM-resident BL618DG manufacturing firmware

### Bug Fixes

- **WiFi / P2P**
  - Hardened management-frame and information-element parsing in WiFi 4, WiFi 6, and Linux host drivers against malformed or truncated frames
  - Fixed WiFi 6 capability parsing and rate encoding, and disabled invalid dual-carrier modulation settings for long extended-range frames
  - Fixed access-point MAC initialization and beacon transmission, including duplicate submission of an active beacon
  - Fixed WiFi 4 forwarding between access-point clients
  - Fixed Wi-Fi Direct group-client discovery and remain-on-channel handling
  - Fixed Wi-Fi Direct group-owner rate advertisement and association checks, and used OFDM rates for P2P action frames and discovery
  - Restored ordinary SoftAP idle-client cleanup and disconnect handling in P2P-enabled builds
  - Fixed Linux host-driver receive-buffer lifetime and key-deletion interface handling
  - Fixed Linux host-driver compatibility with the monitor-channel API in kernel 6.12.101 and later
  - Restored WiFi/BLE coexistence initialization in WLAN firmware
  - Fixed BL616 WiFi linking for supported build targets

- **Networking / Dual-Core**
  - Fixed an lwIP assertion when closing sockets during concurrent send and receive operations
  - Fixed bridge port removal and deletion, including stale forwarding entries
  - Fixed DHCP cleanup before network-interface teardown
  - Fixed socket-offload header and build configuration selection

- **Bluetooth / Power Management**
  - Fixed BLE peripheral-latency calculation when the connection event counter wraps around
  - Fixed a BL616CL BLE controller hang
  - Fixed BL618DG BLE low-power entry conditions
  - Preserved BLE channel-map updates and receive-clock drift compensation across low-power firmware handoffs
  - Fixed returns from low-power firmware to XIP code and cleared stale wakeup boot flags after resume
  - Fixed races between Bluetooth AVDTP delayed-work scheduling and release

- **WebRTC**
  - Fixed UDP send backpressure handling and nonblocking socket configuration
  - Fixed SDP answers to preserve offered media identifiers and validated remote SDP independently of local storage limits
  - Fixed concurrent DataChannel open lifecycles and retained all six ICE servers under automatic connection policy
  - Fixed builds for data-only peers

- **Audio**
  - Fixed playback DMA refills and TDM clock configuration
  - Fixed audio-player initialization and allocation failures with the I2S codec configuration
  - Fixed JY6316, JY6166, and JY6311 audio codec clock configuration
  - Fixed AAC decoder out-of-bounds prefetches at the end of transform windows
  - Fixed media-buffer reallocation when a buffer outgrows its original heap and memory remains available in another heap

- **Memory / Drivers**
  - Fixed BL618DG PSRAM clock-divider configuration order
  - Fixed BL618DG SD host transfers with unaligned buffers
  - Fixed BL618DG ADC calibration and temperature-sensor offsets
  - Expanded BL618DG flash memory protection coverage to 64 MiB
  - Fixed N900 cache-maintenance safety and kept cache operations in tightly coupled memory
  - Fixed BL602 cache initialization to preserve disabled cache ways
  - Fixed USB endpoint-zero stall completion
  - Fixed FreeRTOS machine-timer interrupt detection
  - Rejected heaps too small for TLSF allocator metadata

- **Diagnostics / Manufacturing**
  - Corrected CPU usage reporting in the image-transmission solution
  - Fixed RF initialization for boards with an SPDT RF switch

- **Build / Packaging**
  - Fixed custom firmware post-processing with quoted arguments and multiple packaging options

### Improvements

- Improved WDS bridge scheduling and network buffer budgets
- Reduced BL602 WiFi/BLE coexistence memory usage and adjusted BL602 and BL616 lwIP heap budgets
- Made the TLSF allocator's second-level list count configurable to tune memory overhead
- Reduced BL616CL WLAN firmware power use by lowering the MCU clock and gating unused peripherals
- Updated BL618DG low-power firmware to v2.5.4 and refreshed its RF PHY library
- Updated BL616CL and BL618DG Boot2 images and added wildcard selection for Boot2 packaging
- Added optional AES-GCM backends for mbedTLS with configurable speed and memory trade-offs
- Configured the BL618DG MStar board's TF card interface for single-bit SD host operation
- Reduced WebRTC recovery-log overhead and improved audio DMA diagnostics across counter resets

## v2.3.37 — since v2.3.36 (2026-10-03 → 2026-10-19)

### New Features

- **WiFi**
  - Added a runtime Wi-Fi diagnostic verbosity setting for automatic MAC diagnostics, while preserving explicit log-level overrides
  - Added a per-channel regulatory power dump
  - Made the scan cache size and per-channel scan dwell time configurable, and kept the best RSSI per scan
  - Added associated-peer MPDU and confirmation counters, and retained station statistics in the Wi-Fi Ethernet bridge example
  - Exposed connected-access-point TWT responder capability and passed the station authentication timeout to the MAC layer
  - Exposed credentials provisioned through WPS PBC

- **Audio**
  - Added realtime audio streaming support, including a borrowed playback PCM callback for software echo cancellation, queued-audio flush, optional codec matching, and configurable task priorities
  - Exposed capture-channel layout and board audio defaults
  - Added a BL618DG MStar audio board profile with a single microphone in slot 3 and the captured reference in slot 0
  - Generalized the sound driver implementation and configuration across chips, and refined the audio examples

- **WebRTC**
  - Added reusable WebRTC peer libraries with optional MJPEG and AppRTC adapters
  - Added reliable realtime transport with bounded SCTP retransmission and reassembly, gap SACK and FORWARD_TSN recovery, incremental DTLS handshakes with a configurable deadline, and SRTP role selection
  - Added mandatory RFC 7675 consent freshness with randomized probes, authenticated revocation, and a 30-second deadline
  - Bounded RTP burst reception so receive bursts catch up without starving uplink

- **Networking**
  - Added dual-core socket offload and iperf support for BL618DG
  - Exposed RPC frame lengths and configurable capacity in OpenAMP

- **Thread / Zigbee / 802.15.4**
  - Added CSL timing support across chips, including timed CSL receive and delayed transmit in the 802.15.4 MAC, receive-window scheduling, and OpenThread CLI integration
  - Zigbee now supports both EasyFlash and LittleFS for persistent storage
  - Improved Zigbee runtime role changes

- **Board Support / Examples**
  - Added BL616CL AVE v10 board support
  - Added an LED matrix driver and example
  - Added WAMR (WebAssembly Micro Runtime) support with an application example
  - Added a software reset sequence to the AXS15231E JX371 MIPI-DSI panel

- **Storage / Flash**
  - Added IS25LP256H Information Row definitions

### Bug Fixes

- **WiFi**
  - Fixed authentication and association retries continuing past the connection timeout or after a transmit failure
  - Fixed access-point startup to wait for the start confirmation, and stopped management-frame transmit failures from leaking frames
  - Fixed NuttX builds by removing the unused mbedTLS hardware poll hook
  - Fixed SDIO probing on older BL616 boards that report SDIO ID 0x0606
  - Fixed low-power entry by suspending the NetHub SDIO task before the hardware is stopped

- **Bluetooth**
  - Hardened the Bluetooth host and stack against multiple published vulnerabilities (CVE-2023-0396, CVE-2023-0397, CVE-2023-2234, CVE-2024-1638, CVE-2024-5754, CVE-2025-10456, CVE-2025-10457, CVE-2025-10458, CVE-2026-5068)
  - Fixed BR/EDR scan flags not being preserved after a disconnect
  - Fixed early AVDTP responses arriving before the request was registered
  - Fixed BR/EDR being disabled by default in the configuration

- **Power Management**
  - Fixed BL618DG low-power firmware placement in the first 64 KB of OCRAM
  - Fixed BL618DG flash AES state not being restored after PDS
  - Fixed BL618DG BLE low-power RC32K trigger configuration
  - Fixed BL618DG wireless clock switching before BLE clock gating
  - Fixed the BL618DG AON LDO level and TZC access configuration

- **Security / Crypto**
  - Fixed hardware SHA-1/256/512 finish errors being reported as success by the mbedTLS hardware acceleration layer
  - Fixed an out-of-bounds read when comparing certificate bundle issuers

- **Flash / Drivers**
  - Fixed flash security register operations and address-mode error propagation
  - Fixed W25Q32FW boot with 0xFF read mode for shared flash IDs
  - Fixed BL616 AHB clock IDs to match the ROM API

- **Zigbee**
  - Fixed a compile error when Zigbee debug output is enabled
  - Fixed flash size and manufacturer configuration in the Zigbee examples

- **Audio**
  - Fixed SpeexDSP echo cancellation startup and history reset

- **Storage**
  - Fixed LittleFS key-value commit and delete errors not being reported

- **Manufacturing**
  - Fixed BL618DG PDS current and SPDT GPIO configuration, and removed the unsupported V command from the manufacturing firmware
  - Updated the BL618DG HTOL firmware

- **Dual-Core**
  - Fixed the dual-core Wi-Fi example to use the default flash and partition layout, restoring OTA image generation

- **Build / Tools**
  - Fixed SDK configuration when the AT component is not checked out
  - Fixed the Bluetooth entry in the SBOM table

### Improvements

- Removed unused code across the SDK to reduce code size
- Moved the AT examples into the AT component
- Updated the BL618DG RF PHY library
- Centralized WiFi6 chip capability handling and removed the obsolete PHY diagnostic command
- Renamed the ACL enable parameter to `acl_mode` for clarity
- Exported `compile_commands.json` by default

## v2.3.36 — since v2.3.35 (2026-09-22 → 2026-10-03)

### New Features

- **WiFi**
  - Added survey-based automatic channel selection when starting a SoftAP, choosing a legal, measured non-DFS channel for the requested band and bandwidth
  - Added NuttX host support to the wl80211 driver and WPA supplicant, including a PBKDF2 backend and bounded in-flight station transmit frames

- **Memory / Boot**
  - Added PSRAM execute-in-place (XIP) support for BL616, BL616CL, and BL618DG, including multi-image firmware descriptors, a startup image loader, and configurable secondary-core images
  - Added multi-image support to the low-power firmware adapter

- **Bluetooth**
  - Added an API to configure the Bluetooth public device address
  - Added Bluetooth controller support for BL702 and BL702L, including the BLE CLI example
  - Added BLE link support to the BL618DG low-power firmware, with Bluetooth power-down controls and a configurable Bluetooth path in the WiFi/BLE low-power example
  - Released a UART HCI transport variant of the BL616CL Bluetooth controller library

- **Power Management**
  - Added tickless PDS1 low-power support for BL616
  - Added a PSRAM retention option for BL616 low-power applications

- **OTA**
  - Added an HTTPS OTA server that hosts a browser page for uploading a firmware image to the device, with a demonstration and a selectable OTA erase mode

- **Display**
  - Added the ST7701S HD40007C30 (480x480) and FL7707N HD395003C30 (720x720) square MIPI-DSI panels
  - Improved RGB565/YUV base-layer and OSD0 format configuration for DSI v2 displays

- **Thread / 802.15.4**
  - Added TREL (Thread Radio Encapsulation Link) support to the legacy Thread border router, including mDNS peer discovery

- **Storage**
  - Added a filesystem key-value storage API on LittleFS and migrated internal users from EasyFlash; stored key-values are preserved across the upgrade, and the legacy interface remains available as a deprecated compatibility layer

### Bug Fixes

- **WiFi**
  - Fixed WPA 4-way handshake failures being reported with a misleading status instead of the actual failure reason
  - Fixed raw transmit callbacks being invoked after a transmit error in the lwIP port
  - Fixed interrupt-safety and receive-ring race conditions in the WiFi MAC driver

- **Bluetooth**
  - Fixed an advertising data buffer overflow
  - Fixed BLE resume after sleep in the BL618DG low-power firmware
  - Fixed the BL618DG BLE power configuration for PDS15

- **Thread / Zigbee / 802.15.4**
  - Fixed receive restart after frames that do not request an acknowledgement when receive-on-idle is enabled

- **Power Management**
  - Fixed stale wireless wake scheduling flags that could persist across low-power transitions
  - Fixed BL618DG PSRAM clock reporting when the CPU PLL is used

- **AT Commands**
  - Fixed BL616 flash and GPIO handling, including GPIO pin range validation
  - Fixed BL616 TLS memory allocation in AT commands by using the system heap

- **Display**
  - Fixed DSI v2 direct-mode frame swapping, where a stale frame interrupt could be reported as a completed buffer swap

- **Tools**
  - Fixed relative path handling in the flashing tool configuration files

### Improvements

- Updated the BL616CL and BL618DG RF PHY libraries
- Improved 5 GHz calibration timing for station connections and channel switches
- Updated MXIC QPI flash command handling
- Added a BL616CL WiFi MAC configuration profile for NuttX hosts that prevents receive-FIFO overflow under high-throughput TCP receive
- Updated LittleFS to v2.11.3
- Completed BL702/BL702L PSRAM support with the PSRAM controller driver and chip-level integration
- Updated FlashCube to v1.4.4

## v2.3.35 — since v2.3.34 (2026-09-15 → 2026-09-22)

### New Features

- **WiFi / P2P**
  - Added Wi-Fi Direct (P2P) support for BL616 and BL618DG, including group-owner and group-client operation, persistent groups, GO-intent configuration, and coexistence with station mode
  - Added a P2P demonstration with supplicant-compatible management commands and lifecycle events

- **WiFi**
  - Added access-point PMF (802.11w) mode to scan results
  - Added channel-switch-announcement (CSA) completion events for access-point and station operation, and reported them from the WiFi examples

- **Power Management**
  - Added a unified BL616CL power-management demo with board-level low-power GPIO wakeup configuration
  - Added keyscan wakeup support, including an AON watchdog option in the BL618DG power-management demo

- **Manufacturing / Flashing**
  - Added a RAM-resident BL618DG eFlash loader that is downloaded over JTAG and uses the existing host programming protocol
  - Added BL618DG flash support for XM25EH512 and W25Q02RV devices

- **BL702 / BL702L**
  - Added PSRAM support, including memory layout, Boot2 flash configuration, and multi-image descriptors

- **Audio**
  - Added the ACM8852 audio codec driver

### Bug Fixes

- **WiFi / Coexistence**
  - Fixed single-antenna coexistence so the configured antenna plan is restored after scans and remains consistent across connection, scan, and access-point events
  - Fixed RF initialization ordering for combo, dual-antenna, and single-GPIO SPDT boards, and rejected Wi-Fi/Bluetooth startup before RF preparation is complete
  - Fixed a WiFi task hang when the final authentication frame of an aborted connection could not be transmitted, as happened after a failed WPA3/SAE handshake; pending frames are now retried without blocking and released when they can no longer be delivered
  - Fixed AT+CWLAP reporting incorrect RSSI and authentication values on BL602
  - Fixed a crash in the TCP client test command caused by reading shell arguments after the command had returned
  - Fixed iperf result parsing for different iperf versions
  - Fixed P2P DHCP pool address formatting
  - Restored hidden SSIDs and their metadata in scan results reported by the WiFi TCP example

- **Bluetooth**
  - Fixed A2DP media-channel setup and SBC codec configuration
  - Fixed HFP codec negotiation by no longer advertising mSBC and always answering +BCS with CVSD
  - Fixed BR/EDR connections with devices that do not report an encryption key size
  - Fixed null-pointer crashes in AVDTP stream and local-endpoint handling

- **Networking / Ethernet**
  - Fixed BL618DG UWB/WiFi low-power wakeup over NetHub
  - Fixed Ethernet ping failures after reboot caused by EMAC v2 receive-DMA initialization order

- **Power Management**
  - Fixed spurious keyscan interrupts while idle

- **Manufacturing / Boot**
  - Fixed BL616CL manufacturing tests applying RF temperature optimization when temperature calibration is disabled
  - Removed the unsupported individual APP AES-128 XTS encryption option from BL618DG Boot2 ISP

- **Build / Tools**
  - Fixed menuconfig generation of the autoconfig.menuconfig file

### Improvements

- Increased the AT command task stack size
- Made RF parameter loading overridable by board implementations
- Updated the BL616CL PHY RF library and the keyscan driver
- Tuned TCP buffering in the BL618DG WiFi/Bluetooth coexistence demo to balance throughput and memory use
- Updated coexistence handling in AT commands

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
  - Added missing config symbols (`CONFIG_MULTIMEDIA_VEDIO`, `CONFIG_DVP_RASTER`)

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
