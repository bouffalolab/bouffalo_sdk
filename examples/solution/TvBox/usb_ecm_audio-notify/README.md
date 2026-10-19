# USB ECM Audio Adapter (Notify Transport)

This example is a variant of `usb_ecm_audio-master` that uses **BLE
notifications (GATT Notify) bidirectionally** instead of the master variant's
write-without-response + notify echo design.

Everything else is identical:

- CDC ECM network interface bridged to the BL616CL EMAC/PHY.
- USB HID boot keyboard interface for remote-control key passthrough/conversion.
- USB Audio Class 1.0 at 16 kHz, 16 bit, mono.
- BLE custom GATT channel for remote key/audio ingress validation.
- HID, speaker, and microphone streaming are enabled by default and can be controlled by compile-time macros.

The project directly reuses `examples/cherryusb/cherryusb_cli/device_demo/eth_emac.c` and `eth_emac.h`.

## What changed vs. master

| Aspect | `usb_ecm_audio-master` | `usb_ecm_audio-notify` (this) |
|--------|------------------------|-------------------------------|
| Client → Server | `bt_gatt_write_without_response` on peer Write characteristic | `bt_gatt_notify` on local Notify characteristic |
| Server → Client | `bt_gatt_notify` on local Notify characteristic | `bt_gatt_notify` on local Notify characteristic |
| Discovery | Central discovers Write + Notify; Peripheral only advertises | **Both** roles discover and subscribe to the peer Notify characteristic |
| Echo | Server notifies back any non-voice write it receives | Only the peripheral echoes non-voice notifications back to avoid loops |
| Reassembly | Two directions (write / notify) | Single direction (notify only) |

Both sides register the same GATT service (Write + Notify + CCC).  The Write
characteristic is kept for backward compatibility with nRF Connect / manual
bring-up, but the firmware transport always sends via Notify.

## Build

```sh
make CHIP=bl616cl BOARD=bl616cldk
```

## Feature Macros

The default macros are set in `CMakeLists.txt`:

- `USB_ETHERNET_BLE_ENABLE=1`
- `USB_ETHERNET_HID_ENABLE=1`
- `USB_ETHERNET_UAC_ENABLE=1`
- `USB_ETHERNET_UAC_SPEAKER_ENABLE=1`
- `USB_ETHERNET_UAC_MIC_ENABLE=1`

Set HID, speaker, or mic to `0` if a product profile does not need that function.
