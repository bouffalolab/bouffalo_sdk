# RTSP

`CONFIG_RTSP` builds a bounded RTSP/1.0 server with RTP/JPEG and mono PCMA.
The public header is `rtsp_server.h`. The protocol and JPEG packetizer are
independent of FreeRTOS, camera drivers, audio codecs and WebRTC.

## Integration

Provide nonblocking `read_video` and optional `read_audio` callbacks that copy
one available frame into the provided buffer. Video timestamps use a 90 kHz
clock; audio timestamps use the 8 kHz capture sample clock. Dropped microphone
samples must still advance the source clock. Supply a random-number callback
for session identifiers, RTP sequence numbers and SSRCs.

Provide `rtp_clock(ctx, track, now_ms)` when the source has an independent
capture clock. It maps the poll clock to the current 90 kHz/8 kHz sampling
timestamp, including elapsed time since the last capture block. RTCP uses
this clock rather than a queued packet's delivery time. Without the callback,
each track extrapolates from its first packet; sources must then run at the
nominal clock rate. Do not re-anchor a sampling clock on every socket write.

On ports with a shared RX/TX memory pool, supply `video_ready` to pause video
before exhausting the pool. An unfinished JPEG is retained within its 200 ms
deadline; the next frame is read after completion or expiry. Audio/control
remain eligible. Drain stale source buffers while the network is paused.
Reserve room for incoming ACKs; waiting for `send` to fail can be too late
when unacknowledged TCP payloads occupy the same heap needed to receive ACKs.

Call `rtsp_server_open`, then `rtsp_server_poll` from a single owner every
1..5 ms using monotonic unsigned milliseconds. Serialize start, stop and stats
requests through that owner. Call `rtsp_server_close` to release the listener,
client, UDP sockets and frame storage. No background task is created.
Pass `sizeof(config)` when opening and `sizeof(stats)` when reading stats;
undersized structures are rejected before accessing their fields.

The instance allocates its state and configured maximum JPEG buffer at open.
There are no per-packet allocations. One client and one incomplete outgoing
packet are retained; the next JPEG cannot overwrite an incomplete send. TCP
short writes resume at the exact byte offset. A blocked write closes the
client after five seconds without write progress, allowing TCP loss recovery
without confusing a slow, progressing partial write with a dead client.
A JPEG taking more than 200 ms to send may lose its
remaining fragments; a partially written TCP packet is completed first.
Expired video packets and audio packets waiting over 60 ms are discarded
before writing any interleaved header byte. UDP packets remain atomic.
`video_packets_per_poll` limits video bursts (default 8, maximum 32), while
audio/control retain priority. This bounds bursts without a camera FPS cap.
Optional `video_dscp` / `audio_dscp` select Diffserv markings (0..63); zero
keeps best effort. UDP tracks are marked individually; a shared TCP flow uses
the video marking when configured. Socket/network support determines whether
these QoS hints take effect; unsupported markings do not reject a connection.
Incoming requests are limited to 4096 bytes including their body, and receive
fragmentation and pipelining are supported. A partial request times out after
five seconds; client activity times out after sixty seconds.
Transient lwIP buffer exhaustion (`ENOMEM`/`ENOBUFS`) retains the pending
packet for retry. These retries are counted separately from fatal send errors.

## Supported scope

- OPTIONS, DESCRIBE, SETUP, PLAY, PAUSE, GET_PARAMETER and TEARDOWN.
- RTP over unicast UDP or RTSP TCP interleaving, independently per track.
- RFC 2435 baseline JPEG, 4:2:0 or 4:2:2, 8-bit quantization, default Huffman
  tables and dimensions divisible by eight up to 2040. Restart-coded,
  progressive and custom-Huffman JPEG are rejected and counted.
- Optional PCMA/8000/1, up to 480 samples per source packet.
- Initial and periodic compound RTCP Sender Report + SDES. Both tracks use the same
  relative NTP clock; no synchronized wall clock is assumed. Receiver Reports
  are counted separately from local send backpressure.

This version admits one client. Additional clients are closed without taking
over the active connection. It provides no authentication, TLS, multicast,
RTSP publishing, H.264 packetization, ONVIF or remote speaker backchannel.

## Legacy API

`CONFIG_RTSP_LEGACY` defaults on to retain `librtspsrv.h` and its existing
background task. Applications using only the owner-driven API can turn it off.
The new API does not change or invoke the legacy session implementation.

## Host tests

With a C compiler and Pillow available:

```sh
python3 tests/run_host_tests.py --build-dir /tmp/rtsp-host
```

The script runs the production code with ASan/UBSan and real TCP/UDP sockets.
It checks exact JPEG scan reassembly, tail fragments, truncated and malformed
inputs, source audio, RTCP, request fragmentation/pipelining, channel limits,
extra-client handling, reconnects, tick wrap and repeated open/close.
`server_host /path/to/standard.jpg 8554` provides a synthetic source for an
independent client such as FFmpeg. Define `RTSP_SERVER_POSIX` when building
these sources on a POSIX host; normal SDK builds use lwIP.

Protocol references: [RTSP/1.0](https://www.rfc-editor.org/rfc/rfc2326),
[RTP/JPEG](https://www.rfc-editor.org/rfc/rfc2435),
[RTP/RTCP](https://www.rfc-editor.org/rfc/rfc3550).
