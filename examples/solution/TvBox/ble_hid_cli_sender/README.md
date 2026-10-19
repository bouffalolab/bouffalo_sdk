# BLE HID CLI Sender

This example is the lightweight second-board peer for `usb_ecm_audio-master`.

It starts only:

- BLE custom GATT transport
- shell CLI

It does not start:

- USB device
- ECM/EMAC
- UAC

## Build

```sh
make CHIP=bl616cl BOARD=bl616cldk
```

## Intended Pairing

- Board A runs `examples/cherryusb/TvBox/ble/usb_ecm_audio-master`
- Board B runs this `ble_hid_cli_sender`

Board B scans for `USB-ECM-AUD`, connects, then sends framed packets over the
existing private BLE protocol.

## Commands

```sh
ble_hid hello
ble_voice_bin
```

`ble_voice_bin` sends the full embedded PCM blob:

- PCM format: `16 kHz`, `mono`, `s16le`
- PCM source: `output.pcm` embedded into the firmware image with `INCBIN`
- Playback length: automatically derived from the embedded file size

The implementation style matches the SDK audio examples that use `INCBIN`, for example:

- `bsp/common/audio_player/wave_player.c`
- `bsp/common/audio_player/mp3_player.c`

Prepare `output.pcm` on Linux:

```sh
ffmpeg -i input.wav -f s16le -acodec pcm_s16le -ac 1 -ar 16000 output.pcm
```

Place `output.pcm` beside `main.c`, rebuild the firmware, reflash Board B, and run:

```sh
ble_voice_bin
```

You do not need to separately burn `output.pcm` to a fixed flash address.

For the current Board A firmware:

- `ble_hid <text>` is the most useful path
- Board A receives channel `KEY`
- Board A converts ASCII to USB HID keyboard reports
- The USB host attached to Board A should see the text typed out

Incoming notify echoes from Board A are printed on the sender shell for bring-up.
