# webrtc_peer

`webrtc_peer` is a vendor-neutral application-facing facade for libpeer. It keeps
applications independent of libpeer internals while preserving libpeer as the
ICE, DTLS-SRTP, RTP, and SCTP engine.

Public headers:

- `webrtc_peer.h`: PeerConnection lifecycle, signaling messages, media,
  DataChannel, state, and statistics.
- `webrtc_peer_signaling.h`: pluggable signaling lifecycle. The first backend is
  AppRTC HTTPS/WSS.
- `webrtc_peer_mjpeg.h`: optional MJPEG DataChannel chunking, pacing,
  reassembly, and statistics.
- `webrtc_peer_mbedtls_config.h`: WebRTC prerequisites for the SDK's generic
  mbedTLS configuration, without an application-specific cipher policy.

Enable the core facade with:

```text
CONFIG_FREERTOS =y
CONFIG_LWIP =y
CONFIG_MBEDTLS =y
CONFIG_SRTP =y
CONFIG_LIBPEER =y
CONFIG_WEBRTC_PEER =y
```

Enable AppRTC signaling only for applications that need it:

```text
CONFIG_WEBRTC_PEER_SIGNALING_APPRTC =y
```

Enable the MJPEG adapter only for applications that send JPEG frames over a
DataChannel:

```text
CONFIG_WEBRTC_PEER_MJPEG =y
```

The facade accepts encoded media; the application supplies its own microphone,
camera, codec, and signaling transport. Audio supports Opus, PCMA, and PCMU;
RTP video supports H.264. MJPEG uses the optional DataChannel adapter. HTTP
pages, Wi-Fi credentials, GPIO selection, and capture tasks belong to the
application and are not required by these libraries.

Use the public headers in `components/net/lib/webrtc_peer/include` rather than
libpeer's internal structures. Integrate the owner loop and callbacks below
into the application's task and pass incoming SDP/ICE messages to the facade.
Hardware AES/SHA acceleration is selected by the application's normal mbedTLS
configuration. libpeer selects its mbedTLS 2.x or 3.x API from
`CONFIG_MBEDTLS_V2`; no application-specific API version override is needed.
Size the media queues for the application's memory budget;
`CONFIG_VIDEO_BUFFER_SIZE`, `CONFIG_AUDIO_BUFFER_SIZE`, and
`CONFIG_DATA_BUFFER_SIZE` can be overridden by application build definitions.

`CONFIG_LIBPEER` selects lwIP, mbedTLS, SRTP and the default v3 profile's
DTLS/certificate-writing prerequisites. With mbedTLS 3, libpeer's CMake selects
`libpeer_mbedtls_config.h` through the standard `MBEDTLS_USER_CONFIG_FILE` hook.
This component-owned overlay enables certificate generation and `use_srtp`;
the SDK's `config-tls-generic.h` remains unchanged. The SDK's separate
`CONFIG_MBEDTLS_USER_CONFIG_FILE` hook still loads the application's cipher
and hardware policy. The libpeer overlay does not choose cipher suites.

To replace the standard hook, set the CMake variable `MBEDTLS_USER_CONFIG_FILE`
before `find_package(bouffalo_sdk)` and include `libpeer_mbedtls_config.h` (or
the compatible `webrtc_peer_mbedtls_config.h`) from that user header. Custom
profiles must also enable DTLS/certificate writing. mbedTLS 2 keeps its
application-owned `mbedtls_sample_config.h`; include the overlay there if
needed. This is a minimal overlay;
`components/crypto/mbedtls/kvs_webrtc_mbedtls_config.h` is a complete legacy
KVS profile and does not replace the SDK generic profile.

### Porting to another platform

Keep the `webrtc_peer_*` API, `WEBRTC_PEER_*` constants, public headers and
component name unchanged when moving to another chip vendor. The public
headers use only standard C types and callbacks; they expose no chip or RTOS
structures. Camera, microphone, codecs and task ownership remain application
responsibilities.

Port the libpeer backend's sockets, DNS, interface discovery, millisecond
clock and sleep functions in `libpeer/src/socket.c` and `ports.c`, or use its
existing POSIX/lwIP paths. Provide mbedTLS, libsrtp, cJSON and compatible
logging. Supply a cryptographic entropy source through the platform's mbedTLS
port; optional AES/SHA hardware acceleration also belongs there. AppRTC uses
the connected TLS transport's DRBG for WebSocket keys and masks and calls no
chip TRNG API directly. Replace the SDK CMake/Kconfig integration with the
target build system's source, include and feature selections as needed.

The facade was previously named `bflb_peer`. Existing callers must update
`bflb_peer*.h` to `webrtc_peer*.h`, `bflb_peer_*` to `webrtc_peer_*`,
`BFLB_PEER_*` to `WEBRTC_PEER_*`, and `CONFIG_BFLB_PEER*` to
`CONFIG_WEBRTC_PEER*`, then rebuild. Native libpeer APIs retain their names.

### Memory budget

Default SDK queue capacities are **per connection**, excluding TLS/ICE/SCTP
state, codec working memory, task stacks and capture buffers:

Only queues for enabled media codecs/DataChannel are allocated.

| Queue | Default capacity |
| --- | ---: |
| Video | 332800 bytes (325 KiB) |
| Audio | 8096 bytes |
| DataChannel | 80960 bytes |

Audio-only or small-memory applications should set `CONFIG_VIDEO_BUFFER_SIZE=0`
and size the other queues in application CMake before loading the SDK.
Zero-capacity media queues send synchronously from the owner context. A camera
application can use a 16384-byte data queue and send encoded audio directly.
Check the complete target heap/stack budget; PSRAM does not automatically size
these queues.

## Lifecycle

### ICE updates and connection liveness

Trickled candidates append compatible pairs without resetting an existing
check or the selected/nominated pair. Late candidates do not automatically
migrate an established connection. In-place renegotiation and ICE restart are
not implemented. SDP offers/answers are accepted only in the NEW state; the
facade returns INVALID_STATE for a replacement, leaving the live session intact.
Use webrtc_peer_new_connection() and exchange fresh SDP to replace a session.
Candidates received before the initial remote SDP are retained and deduplicated
with its candidates. A candidate-free SDP enters CHECKING and waits for a
compatible trickled candidate; the application must impose its overall
signaling/negotiation timeout. Candidate-pair check deadlines start when checks
begin, rather than while waiting for signaling to supply candidates.
Native `peer_connection_create_offer()` and `peer_connection_create_answer()`
also reject active/failed negotiations without modifying their transports.
For legacy native signaling, an explicit `peer_connection_close()` followed by
`create_offer()` still starts a new negotiation; it does not renegotiate an
active session. `create_answer()` requires NEW after receiving a remote offer.

Each candidate pair has a dedicated connectivity transaction, separate from
consent's eight slots. Retransmissions retain the same authenticated transaction
ID, with a 500 ms initial RTO, exponential backoff, seven transmissions and a
39.5 s monotonic deadline per candidate pair. Checks across candidate pairs proceed
in parallel, paced to at most one request/50 ms, so an unreachable first pair
does not block later usable pairs. State is bounded by
`AGENT_MAX_CANDIDATE_PAIRS`; reduce that compile-time bound on smaller targets. No dynamic
per-probe allocation is needed. A delayed reply cannot be overwritten by a
faster owner-loop poll or a later candidate. A winner requires a live transaction,
matching source and authenticated response; other pending checks are retired.
Adaptive RTT and complete ICE role-conflict/restart recovery are not provided.
The removed `AGENT_CONNCHECK_MAX` loop counter no longer configures this budget;
the deadline is measured in milliseconds per pair, not owner-loop iterations.

Internal structure sizes depend on ABI, options and candidate limits. With the
default 100-pair limit, example storage requirements are:

| Build | IceCandidatePair | Agent (including inline pair/consent storage) |
| --- | --- | --- |
| BL618DG AP, RISC-V ILP32D | 56 bytes | 10760 bytes |
| Linux x86_64 | 72 bytes | 12544 bytes |

Other 32-bit ABIs can give different alignment and sizes. Include the full
Agent storage in the per-connection budget, plus the separately allocated RNG
context (576 bytes in the measured BL618DG configuration).

Each Agent owns an independent mbedTLS CTR_DRBG and entropy context, seeded once
at creation and released at destruction. ICE credentials and periodic probes
reuse it; automatic DRBG reseeding remains enabled. Different connections do
not share RNG state or require a global platform-specific RNG mutex. Calls on
one connection must still be serialized by its owner, as with all other APIs.

The owner must keep calling `webrtc_peer_main_loop()`, including while idle.
libpeer sends authenticated STUN consent probes every randomized 4..6 seconds.
Only a matching authenticated response from the active peer address renews
consent. After 30 seconds without one, or a matching authenticated STUN 403,
application sends stop, queued media/SCTP data is cleared, and the state callback
reports `WEBRTC_PEER_STATE_DISCONNECTED`. Incoming requests/media do not renew
outbound consent, and late responses cannot revive an expired session.
The application must call `webrtc_peer_new_connection()` and exchange fresh
SDP/ICE credentials to reconnect. This changes the old default, which could
leave a dead path reported as completed indefinitely. Owner scheduling adds
latency to the state notification; the send path also rejects expired consent.
Consent freshness is enabled by default and has no disable switch. This is an
intentional liveness/security behavior change from the former disabled
KEEPALIVE_CONNCHECK option: peers must continue answering authenticated STUN
Binding requests after establishment. A pending initial consent permits only
outbound STUN probes, not application/DTLS sends; it is not immediately reported
as lost. Authenticated inbound media/DTLS records, including cached DTLS records,
continue to be processed within the normal receive budget while waiting.
An invalid established ICE path reports `WEBRTC_PEER_STATE_FAILED`.
Callbacks execute synchronously in the owner context. Schedule reconnection,
reset or destruction after the callback and current owner-loop iteration return.

Reliable receive reassembly retains a bounded contiguous prefix when a peer
uses more DATA fragments than the configured receive slot count. The supported
RX slot range is 2..32, enforced at compile time.
Buffered wire-chunk bytes are capped at the larger of
`CONFIG_SCTP_RX_WINDOW * CONFIG_MTU` and the maximum message plus
one DATA header per receive slot. Wire alignment padding is not retained or
charged to that byte budget.
The original 16-bit DATA header length is retained, while the merged prefix
length is tracked separately as `size_t`, avoiding truncation when non-SDK builds
configure receive limits above 64 KiB. SDK Kconfig currently caps the message
limit at 16 KiB. Set the message limit to fit the target memory budget.
The head prefix reuses geometrically grown capacity, giving amortized linear
copying rather than repeatedly copying the
whole prefix for each fragment. Unused capacity is less than one maximum
message. During growth the old and new allocations coexist; allow up to two
additional maximum-message buffers above the wire-byte budget, plus bounded
chunk/allocator metadata. This allowance also covers the final message-sized
application delivery buffer (not three simultaneous extra buffers).
Allocation failure preserves acknowledged data and the owner loop retries.
This avoids a zero-window stall without increasing the default slot count.
Existing gap/FORWARD_TSN and ordered-delivery rules still apply.

`webrtc_peer_get_stats_ex(peer, &stats, sizeof(stats))` with a zero-initialized
`webrtc_peer_stats_ex_t` adds `data_retransmissions` (reliable DATA retry attempts, excluding
handshake retries) and `data_retry_exhaustions`. They remain available after an
association failure and reset on the next association. The optional usrsctp
backend currently reports zero for these counters. The original
`webrtc_peer_stats_t` layout and `webrtc_peer_get_stats()` writes are unchanged,
including for old binary callers. The extension query accepts the base prefix
and writes at most the smaller of the caller's size and the library's known
size. Unknown future tail bytes remain unchanged (initialize them to zero).
New fields are appended only to the extension.
These are diagnostics, not congestion control.

```c
webrtc_peer_t *peer = NULL;
webrtc_peer_config_t config = {
    .audio_codec = WEBRTC_PEER_CODEC_PCMA,
    .data_channel_type = WEBRTC_PEER_DATA_CHANNEL_STRING,
    .on_local_message = on_local_message,
};

webrtc_peer_open(&config, &peer);
webrtc_peer_new_connection(peer, WEBRTC_PEER_ROLE_OFFERER);

while (running) {
    webrtc_peer_main_loop(peer);
}

webrtc_peer_close(peer);
```

`on_local_message` receives the local SDP offer or answer. Incoming SDP and
ICE messages are applied with `webrtc_peer_send_msg()`. An answerer creates and
emits its answer automatically when it receives an offer.

`webrtc_peer_new_connection()` reports the initial `WEBRTC_PEER_STATE_NEW` state
for both roles. An answerer may remain in that state while signaling delivers
the remote offer asynchronously; `CLOSED` is reserved for an explicitly
closed or failed lifecycle.

`webrtc_peer_open()` deep-copies ICE server strings and reference-counts
`peer_init()/peer_deinit()`. `webrtc_peer_new_connection()` can replace the
native PeerConnection while retaining the facade handle and configuration.

## MJPEG DataChannel Adapter

`webrtc_peer_mjpeg` sits above the generic peer facade; it does not add
MJPEG policy to libpeer or `webrtc_peer.c`. The sender emits the browser-compatible
`JPGC` format: 4-byte magic followed by big-endian frame ID, total JPEG size,
and chunk offset, then the JPEG bytes. The receiver also accepts the older
whole-frame `JPEG` format for compatibility.

The adapter performs no hidden large allocation. The caller supplies the chunk
and reassembly buffers. A submitted JPEG is borrowed until
`webrtc_peer_mjpeg_sender_is_busy()` becomes false or the sender is reset, so the
caller must not reuse or modify that frame while it is active. Each process
call attempts at most one chunk. Default `max_chunk_retries=0` drops the frame
on transport pressure, preserving the realtime policy of existing callers.
For recovery, set `max_chunk_retries=3` and `retry_timeout_ms=20`:
`ERR_WOULD_BLOCK` keeps the frame/offset and retries the identical chunk on a
later millisecond. Exhausting the count/deadline drops the frame; permanent
errors still drop immediately. Zero timeout uses 20 ms, with no frame-rate cap.
The caller retains the borrowed JPEG during retries too. Optional XOR parity
loss does not hold a completed data frame.

The receiver requires contiguous chunks for one frame. A missing, reordered,
size-mismatched, or malformed message is rejected without exposing a partial
JPEG to the application callback.

## Concurrency

A peer handle has one owner task. That owner creates connections, applies
signaling, calls `webrtc_peer_main_loop()`, creates DataChannels, disconnects,
and closes the handle. libpeer's queues are not internally locked, so sending
from another task must use an application-owned mutex or submit work to the
owner task.

Applications using multiple peer handles must also serialize `webrtc_peer_open()`
and `webrtc_peer_close()` across their owner tasks. The process-wide
`peer_init()/peer_deinit()` reference count is intentionally lightweight and
does not contain an OS-specific lock.

Callbacks run synchronously in the owner context. They must not close the
handle that invoked them.

An SCTP association-open callback does not mean a DataChannel is ready.
`webrtc_peer_is_data_channel_open()` becomes true only after the selected DCEP
stream has completed OPEN/ACK. Data sends before that return
`WEBRTC_PEER_ERR_INVALID_STATE`.

## Current Limits

- libpeer's remote-description setter has no parse result. The facade can
  propagate immediate native `FAILED` as `WEBRTC_PEER_ERR_LIBPEER` and counts
  only accepted signaling. Missing mandatory DTLS/ICE fields fail immediately.
  The parser still has limited validation; later negotiation errors are
  visible through state callbacks and logs.
- `webrtc_peer_main_loop()` mirrors libpeer's current coarse loop result.
- Opening or closing independent handles concurrently requires an
  application-level lock around those lifecycle calls.
- TURN relay media is not advertised by the AppRTC backend because libpeer
  does not yet implement the complete TURN permission/channel data path.
- The built-in SCTP implementation has bounded reliable DATA retries
  (by default 16 packet slots, 750 ms intervals, at most 8 retries) and per-stream
  reassembly. `CONFIG_SCTP_MESSAGE_SIZE` defaults to 16384 bytes and is also
  advertised in SDP. Reliable messages retain owned packet copies;
  `maxRetransmits=0` unordered streams retain the real-time drop policy.
  The implementation does not provide full SCTP congestion control.
  At the default message, slot and MTU settings, budget approximately 20 KiB
  for a full reliable TX window. RX storage also needs the payload, prefix
  growth and delivery allowances described above, in addition to application
  queues.
  Both reliable and abandonable sends enforce the message limit and cap a
  single message's fragment count at the TX window size. Abandonable sends do
  not retain retransmission copies or consume the reliable TX window.
  Complete received messages survive FORWARD_TSN processing. The main loop
  retries queued delivery after allocation failure and advertises a reopened
  receive window, even without further incoming traffic. Failed SACK sends
  for DATA, FORWARD_TSN and deferred window updates are retried by the poll loop.
  At most five stream entries are retained per association. DATA for additional
  SIDs and messages exceeding the message limit are discarded without closing
  healthy channels. Bare DATA SIDs also allocate entries; entries are not
  reclaimed until the association closes. At a full RX window, the next
  cumulative TSN can replace the farthest fragment above the cumulative ACK;
  its gap ACK is retracted so the sender can retransmit it. Cumulatively
  acknowledged payload is retained, and the RX slot limit is preserved even
  across consecutive gap repairs. Oversized fragmented
  messages release their accumulated
  payload and retain one discard marker until their end, reopening the window
  without delivering truncated data. This remains a bounded subset of SCTP;
  it does not implement stream reset/reclamation or general partial delivery.
  Incomplete messages have no receive-side expiry. Missing reliable fragments
  require sender retransmission; abandoned fragments require the sender's
  negotiated FORWARD_TSN support. The receiver does not unilaterally discard
  cumulatively acknowledged reliable fragments. A missing first fragment
  produces a SACK gap starting at 2 or higher.

The SDK exposes these transport budgets in Kconfig under `CONFIG_LIBPEER`.
Non-SDK builds can override the same macros before including `config.h`.

| Option | Default | Kconfig range | Unit |
| --- | ---: | --- | --- |
| `CONFIG_SCTP_MESSAGE_SIZE` | 16384 | 1..16384 | message bytes, also advertised in SDP |
| `CONFIG_SCTP_TX_WINDOW` | 16 | 1..64 | retained reliable packets; fragments per send |
| `CONFIG_SCTP_RX_WINDOW` | 16 | 2..32 | received DATA chunks |
| `CONFIG_SCTP_DATA_RETRY_MS` | 750 | 1..60000 | milliseconds between retries |
| `CONFIG_SCTP_DATA_RETRY_LIMIT` | 8 | 1..64 | DATA retransmissions |
| `CONFIG_DTLS_HANDSHAKE_TIMEOUT_MS` | 15000 | 1000..120000 | handshake deadline in milliseconds |

Size the message limit for the target memory budget and transmit fragment window.
Reliable sends that cannot fit the TX window return backpressure. The RX window
limits retained chunk nodes, not the total fragment count of a message: prefix
compaction supports messages spanning more fragments than the 2..32 receive slots.
Budget reassembly storage as described above; reducing the slot count does not
remove the need for a complete message's payload storage. These controls do not
add congestion control.

## Compatibility and diagnostics

The built-in SCTP message limit is now 16 KiB, replacing the previous
256 KiB SDP advertisement. This is a compatibility change: larger sends fail
and oversized received messages are discarded. Applications carrying larger
payloads must split them into application-level messages within the configured
limit; MJPEG chunk sizes must include their protocol headers in that budget.

`webrtc_peer_send_data()` is a single-channel convenience: its implicit SID tracks
the most recent channel creation, label lookup, or received message. Multiple
channels should use `webrtc_peer_send_data_on_channel()` with an explicit SID.
Read-only SCTP statistics diagnose backpressure/loss recovery; they expose no
native structures and should not drive lifecycle or delivery decisions.

- `CONFIG_MBEDTLS_2_X` follows the actual SDK library: default
  `CONFIG_MBEDTLS_V2=n` selects the v3 API; explicit V2 retains the v2 API.
  The old hard-coded v2 flag did not match the default library.
- `peer_connection_create_offer()` returns a borrowed SDP pointer. Existing
  statement calls that ignore the return remain source compatible; the SDP
  callback still runs synchronously. Function pointers returning `void` must
  update their type. Recompile consumers rather than mixing old binary objects
  with new headers.
- DTLS server cookies are enabled. When local SDP advertises `setup:passive`,
  the remote active peer must handle HelloVerifyRequest.
- `dtls_srtp_set_role()` changes the DTLS role only in its initial state.
  `peer_connection_get_local_datachannel_sid()` returns a role-appropriate local
  SID for callers of the explicit-SID APIs. The legacy native create/send
  convenience functions retain SID 0; callers must select the appropriate API.
  Apply SDP and drive the handshake from the same owner context.
- DTLS requires a fresh, well-formed SHA-256 certificate fingerprint in each
  remote SDP and compares the presented leaf certificate during verification
  and after handshake. SDP parsing validates into a temporary pin; rejected
  descriptions in NEW never clear the stored pin, but fail the connection;
  replacements in other states leave the connection intact. A connected
  peer cannot be repinned within that DTLS session. In-place renegotiation is
  unsupported. Create a new PeerConnection for a new session and authenticate
  its remote SDP; pin continuity is not guaranteed across sessions.
  A fingerprint received over unauthenticated signaling does not establish
  peer identity.
- For DTLS only, the pinned identity replaces CA/name, certificate-date and
  legacy certificate self-signature/key policy checks. The verification callback
  clears only `NOT_TRUSTED`, `CN_MISMATCH`, `EXPIRED`, `FUTURE`, `BAD_MD` and
  `BAD_KEY`; revocation, usage, missing-certificate and unknown flags remain
  fatal under `VERIFY_REQUIRED` in both roles. TLS handshake algorithms still
  enforce their own policy. HTTPS CA/hostname verification is unchanged.
- The certificate-bundle adapter uses mbedTLS's versioned DER access macro so
  the same build supports the SDK's mbedTLS 2 and 3 certificate structures.
- `DISABLE_PEER_SIGNALING=1` in the SDK build disables legacy libpeer MQTT/HTTP
  signaling. Native SDP/ICE callbacks and optional AppRTC are available;
  external legacy-signaling callers must migrate deliberately.
- `ports_get_epoch_time()` retains its old name but returns wrapping
  monotonic milliseconds: `sys_now()` uptime on lwIP and `CLOCK_MONOTONIC`
  on POSIX hosts. Use unsigned differences for relative deadlines; this is
  not a portable epoch timestamp.

## AppRTC deployment limits

HTTPS/WSS verifies against the bundled **DigiCert Global Root G2**, not an OS
CA store. A service changing CA needs a reviewed trust-anchor update. URL
limits are 767 bytes overall, 95 bytes for a DNS/IPv4 host, and 511 bytes for
the path. IPv6 literals, userinfo, fragments and control characters are rejected.
Room/client identifiers are excluded from normal join/error logs. The deployment
must provide a certificate chain trusted by the configured anchor.

## Host regressions

`libpeer/tests/host/` compiles the production sources with transport/allocation
faults and tests DTLS with the SDK's mbedTLS 2/3 and libsrtp. These scripts run
separately from firmware CMake; the existing test source glob is not recursive.
Supported environment: Linux, GCC, Python 3, CMake and Git.
Initialize the SDK crypto submodules, including the mbedTLS 3 framework, then
run from the SDK root:

```sh
git submodule update --init --recursive components/crypto/mbedtls/mbedtls components/crypto/mbedtls/mbedtls_v3
CC=gcc python3 components/net/lib/libpeer/tests/host/run_host_tests.py
CC=gcc python3 components/net/lib/libpeer/tests/host/run_dtls_tests.py --build-dir /tmp/libpeer-dtls-v2
CC=gcc python3 components/net/lib/libpeer/tests/host/run_dtls_tests.py --mbedtls-version 3 --build-dir /tmp/libpeer-dtls-v3
```
