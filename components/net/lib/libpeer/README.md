# libpeer - Portable WebRTC Library for IoT/Embedded Device

![build](https://github.com/sepfy/pear/actions/workflows/build.yml/badge.svg)

libpeer is a WebRTC implementation written in C, developed with BSD socket. The library aims to integrate IoT/Embedded device video/audio streaming with WebRTC, such as ESP32 and Raspberry Pi

### Features

- Vdieo/Audio Codec
  - H264
  - G.711 PCM (A-law)
  - G.711 PCM (µ-law)
  - OPUS
- DataChannel
- STUN/TURN
- IPV4/IPV6
- Signaling
  - [WHIP](https://www.ietf.org/archive/id/draft-ietf-wish-whip-01.html)
  - MQTT

### Dependencies

* [mbedtls](https://github.com/Mbed-TLS/mbedtls)
* [libsrtp](https://github.com/cisco/libsrtp)
* [usrsctp](https://github.com/sctplab/usrsctp)
* [cJSON](https://github.com/DaveGamble/cJSON.git)
* [coreHTTP](https://github.com/FreeRTOS/coreHTTP)
* [coreMQTT](https://github.com/FreeRTOS/coreMQTT)

### Getting Started with Generic Example
- Copy URL from the test [website](https://sepfy.github.io/libpeer)
- Build and run the example
```bash
$ sudo apt -y install git cmake
$ git clone --recursive https://github.com/sepfy/libpeer
$ cd libpeer
$ cmake -S . -B build && cmake --build build
$ wget http://www.live555.com/liveMedia/public/264/test.264 # Download test video file
$ wget https://mauvecloud.net/sounds/alaw08m.wav # Download test audio file
$ ./examples/generic/sample -u <URL>
```
- Click Connect button on the website

### Examples for Platforms
- [ESP32](https://github.com/sepfy/libpeer/tree/main/examples/esp32): MJPEG over datachannel
- [PICO](https://github.com/sepfy/libpeer/tree/main/examples/pico): Ping pong with datachannel
- [Raspberry Pi](https://github.com/sepfy/libpeer/tree/main/examples/raspberrypi): Video and two-way audio stream
### Optional media diagnostics

`CONFIG_LIBPEER_MEDIA_DIAGNOSTICS` is a debug-only Kconfig option, disabled
by default. Enable it in menuconfig or pass
`CONFIG_LIBPEER_MEDIA_DIAGNOSTICS=y` to the SDK make command. Keep it disabled
in production builds. Disabled builds have no added RTP diagnostic state,
clock reads, packet processing or observer object in the library.

When enabled, `peer_connection_get_rtp_diagnostics()` provides an owner-thread
snapshot for audio or RTP video. The facade exposes the size-checked
`webrtc_peer_get_rtp_diagnostics()` equivalent without changing existing stats
layouts. There are no per-packet allocations or logs.

- Sequence validation and jitter follow RFC 3550 Appendices A.1/A.8. A new
  source needs two consecutive packets. Loss/expected counts cover the current
  validated sequence epoch and reset after a confirmed restart. Late arrivals
  correct loss; duplicate arrivals may make RFC cumulative loss negative.
- `received_packets` counts all observed authenticated RTP packets, including
  probation/rejected sequence observations. Duplicate identification covers
  the last 64 sequence numbers; older arrivals have a separate counter.
  Packets rejected by SRTP authentication/replay protection are not observed.
- For mono PCMA/PCMU, timestamp gaps between consecutive sequence numbers are
  counted separately from missing RTP packets. DTX or intentional source
  discontinuities can produce such gaps; they do not prove a gateway fault.
- Existing RTCP SR sender counters and RR report blocks are observed with
  bounds checks. `remote_*` describes the remote receiver's report about our
  outgoing stream, not this endpoint's incoming loss. Compare SR counter deltas
  over aligned intervals rather than subtracting lifetime counters directly.
- The observer does not send SR/RR, reorder packets, change playback, or
  implement a jitter buffer/congestion controller. No signaling or server
  protocol change is required. DataChannel MJPEG uses SCTP statistics.
