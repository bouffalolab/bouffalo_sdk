# BLE HID CLI Sender

This example is the lightweight second-board peer for `../ble_usb_router`.

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

- Board A runs `examples/solution/TvBox/ble_usb_router`
- Board B runs this `ble_hid_cli_sender`

Build Board A from its own directory with
`make CHIP=bl616cl BOARD=bl616cldk`, then build Board B with the command above.
Board A always advertises `USB-ECM-AUD`. Board B does not advertise: it scans
for Board A and initiates the connection. Wait for the `peer value ready` log
before sending commands. Board B sends existing framed channel packets with
GATT Write Without Response to the discovered value handle, without CCC subscription.

The build directly reuses `ble_common.c` and `ble_notify.c`
from `../ble_usb_router/ble`, with no duplicate protocol state.
Its configuration enables scanning and Write TX, and disables advertising,
and Write/WNR ingress on Board B. Address-order selection and all Notify/CCC
logic are removed. Board B has no local business GATT service or RX reassembly buffers.
Board B only sends application data and does not connect to phones.
Product audio is one-way: Board B BLE -> Board A MCU -> USB microphone -> host.
Board A has no USB speaker interface or USB-host-to-BLE audio forwarding;
Board B's audio sending remains available.

Phones use the same Board A firmware and write the same framed packets to the
same characteristic. Only one peer is supported at a time: stop or power off
Board B before connecting a phone, because Board B automatically reconnects.
Board A provides no Notify/CCC or reverse application data path. Phones should
rediscover its services after upgrading from the older Notify/CCC firmware.

## Commands

```sh
ble_hid hello
ble_voice_bin
```

`ble_voice_bin` sends the full embedded PCM blob:

- PCM format: `16 kHz`, `mono`, `s16le`
- PCM source: `audio.pcm` embedded into the firmware image with `INCBIN`
- Playback length: automatically derived from the embedded file size

PCM packets reference the embedded const source directly; the shared BLE API
copies each fragment into an ATT buffer before returning. Preview samples are
decoded bytewise, without depending on source alignment. Logs distinguish
`attempted_pkt`, `queued_pkt`, `failed_pkt` and `queued_bytes`. Queued counts
mean complete packets accepted by the local BLE stack, not confirmed peer
delivery; bytes from partially queued failed packets are not included.

The implementation style matches the SDK audio examples that use `INCBIN`, for example:

- `bsp/common/audio_player/wave_player.c`
- `bsp/common/audio_player/mp3_player.c`

Prepare `output.pcm` on Linux:

```sh
ffmpeg -i input.wav -f s16le -acodec pcm_s16le -ac 1 -ar 16000 audio.pcm
```

Place `audio.pcm` beside `main.c`, rebuild the firmware, reflash Board B, and run:

```sh
ble_voice_bin
```

You do not need to separately burn `audio.pcm` to a fixed flash address.

For the current Board A firmware:

- `ble_hid <text>` is the most useful path
- Board A receives channel `KEY`
- Board A converts ASCII to USB HID keyboard reports
- The USB host attached to Board A should see the text typed out

Board B does not receive or print application channel packets from Board A.
