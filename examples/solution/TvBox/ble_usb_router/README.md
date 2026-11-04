# BLE USB Router

The router always advertises `USB-ECM-AUD` as a BLE peripheral/GATT server.
Either a phone or the second-board sender connects as a central and writes
channel frames through Write / Write Without Response. No BLE mode selection
or firmware switch is needed. The product audio path is only
phone/sender BLE -> MCU -> USB microphone -> host. There is no USB speaker
interface or USB-host-to-BLE audio path. BLE Notify and CCC are not provided.

Other functions remain:

- CDC ECM network interface bridged to the BL616CL EMAC/PHY.
- USB HID boot keyboard interface for remote-control key passthrough/conversion.
- USB Audio Class 1.0 at 16 kHz, 16 bit, mono.
- BLE custom GATT channel for remote key/audio ingress validation.
- ECM, HID and the UAC microphone are always enabled, with no per-function switches.

The project directly reuses `examples/cherryusb/cherryusb_cli/device_demo/eth_emac.c` and `eth_emac.h`.

On BL616, controller initialization runs synchronously before USB startup
(its PDS reset affects USB PHY); startup failure leaves USB stopped until reboot. BL616 uses
ECM IN/OUT 0x81/0x02, HID 0x83 and microphone 0x84. ECM notification IN 0x85
remains in the descriptor; BL616 skips opening EP5 and disables notifications.
This configuration has been confirmed for the target use case. BL616CL
uses the same data endpoints and ECM notification endpoint 0x85 with notifications enabled.

## Scope and migration from usb_ecm_audio-notify

BS-1635 requests phone remote-control input while retaining USB ECM/EMAC bridging.
This revision also incorporates the subsequently confirmed one-way product scope:
phone/sender -> BLE -> USB keyboard/microphone. These additional scope decisions
must be recorded in the issue before merging; they are not implied by its original text.

- Rename `usb_ecm_audio-notify` to `ble_usb_router` and split shared BLE code from USB.
	Update build/automation paths to the new example directory.
- Replace Notify/CCC transport with Write/WNR using fixed router/server and sender/client
	roles. This is not wire-compatible with old Notify clients: update sender and phone
	software together, remove subscriptions and rediscover the writable characteristic.
- Remove USB speaker and host-to-BLE audio, reverse `ble_hid`, receiver `ble_pcm`,
	and the product USER test receiver. Generic USER framing/registration APIs remain.
	Applications requiring these old demonstration paths must retain the old firmware.
- Remove the bundled `audio.wav` demonstration asset; sender audio input remains available.

The shared FreeRTOS configuration retains runtime statistics for both router and
sender, using the existing microsecond timer source.

## BLE connection model

The receiver registers one GATT service with one value characteristic supporting
**Write + Write Without Response**, without Notify or CCC. The service
UUID remains `07af27a5-9c22-11ea-9afe-02fcdc4e7412`; the value UUID remains
`07af27a9-9c22-11ea-9afe-02fcdc4e7412`. Both clients write the same 12-byte
channel header plus payload, not bare text or PCM. Clients with a cached GATT
database must rediscover services after upgrading from the Notify/CCC firmware.

The router does not scan or subscribe to peer Notify. The sender scans for
`USB-ECM-AUD`, discovers its writable value, and sends WNR frames; it does not
advertise or connect to phones. Only one peer is supported at a time. Stop or
power off the auto-reconnecting sender before connecting a phone.

## Modules

- `ble/ble_common.c/h`: public `ble_common_*` API, BLE initialization, advertising,
	scanning, connection/reconnection and MTU management, the only GATT table,
	and shared channel framing/reassembly/callback dispatch. The single public
	header `ble/ble_common.h` also groups cross-module connection and
	receive-frame interfaces in a separate section; no duplicate connection or
	protocol state.
- `ble/ble_remote.c/h`: phone/sender Write/WNR ingress validation, including rejection of
	prepared and offset writes; forwards frames to the common reassembler.
- `ble/ble_notify.c/h`: sender-only peer discovery, WNR egress and TX-buffer
	backpressure (historical filename; no Notify implementation). Voice keeps
	its 20 ms per-fragment `-ENOMEM` retry budget; KEY/USER keep their 1000 ms
	retry budget. These outer budgets do not bound stack-internal `K_FOREVER`
	allocations in the GATT API and are not total send-time limits.
- `usb/usb_hid_bridge.c/h`: unchanged KEY-to-HID conversion and queued reports,
	using `usb_composite_hid_*`.
- `usb/usb_composite.c/h`: `usb_composite_start/stop` and HID report APIs; retains
	ECM/EMAC, UAC microphone and HID implementations together for now.

The router compiles common + remote; `../ble_hid_cli_sender` compiles common +
notify without USB dependencies. The sender has no local business GATT service,
RX reassembly buffers or receive callbacks. Address-order connection selection
has been removed; the sender scans by name and reconnects to its remembered peer.

The product entry no longer registers the USER bandwidth-test receiver. USER
framing, sending and callback registration remain available for applications;
the HID-test and CPU/task diagnostic CLI commands remain. The receiver's
reverse BLE KEY-debug command `ble_hid` has been removed; the sender's
`ble_hid` command and the receiver's BLE-to-USB HID path remain unchanged.
The receiver's `ble_pcm` command and USB speaker forwarding have been removed;
the sender's audio send path remains available. Received VOICE packets
with odd byte lengths are rejected because they contain incomplete 16-bit samples.
EMAC/MDIO and USB pins use the selected board's BSP configuration. UAC uses
the USB/BLE PCM path, not an external I2S codec, I2C or audio-PLL setup.

HID readiness is derived from the USB core's configured and suspend states.
Suspend blocks new reports but preserves in-flight busy state; host resume
restores readiness. The unchanged descriptor does not advertise remote wakeup,
so HID sends do not attempt it.

## Build

```sh
make CHIP=bl616cl BOARD=bl616cldk
```

`BLE_MODE` is no longer used: the router always advertises and receives writes.
The AVE board is also supported, for example:
`make CHIP=bl616cl BOARD=bl616cl_ave_v10`.

CHIP/BOARD remain overridable defaults; both DK and AVE builds use their
respective BSP GPIO configurations.

## Feature Configuration

BLE, ECM, HID and the UAC microphone are always enabled. There are no Kconfig
switches to omit these USB functions. The five fixed interfaces are
ECM 0/1, AudioControl 2, microphone AudioStreaming 3,
and HID 4. Both chips use ECM IN/OUT `0x81`/`0x02`, HID IN `0x83`, and microphone
IN `0x84`. BL616 declares ECM notification IN `0x85` without sending notifications;
BL616CL uses notification IN `0x85` and sends link/speed notifications.
