# BSP audio device controls

The application, GMF device, PCM owner and legacy player use the same direction
names and units. `volume` is a normalized 0-100 percent device level, microphone
`gain_db` is physical analog gain, and `mute` preserves the selected volume and gain.
The PGA is the hardware implementation of input analog gain, not a second
independent setting. Software PCM gain remains a separate GMF processing element.

| Shell command | Public API suffix | Meaning |
| --- | --- | --- |
| `speaker_vol <0-100> [slot_mask]` | `set_speaker_volume`, `set_speaker_channel_volume` | Playback volume |
| `speaker_mute <0\|1> [slot_mask]` | `set_speaker_mute`, `set_speaker_channel_mute` | Playback mute |
| `mic_vol <0-100> [slot_mask]` | `set_mic_volume`, `set_mic_channel_volume` | Input digital volume |
| `mic_gain_db <dB> [slot_mask]` | `set_mic_gain_db`, `set_mic_channel_gain_db` | Analog microphone gain in dB |
| `mic_mute <0\|1> [slot_mask]` | `set_mic_mute`, `set_mic_channel_mute` | Capture mute |

Prefixes are `solution_audio_`, `bflb_gmf_audio_device_`, `audio_pcm_` and
`wave_player_` at their respective layers. The codec layer exposes explicit
channel operations; it does not infer application microphone/reference roles.
Global device setters expand the role mask and call the corresponding channel setter.
The firmware selects one frontend at build time. With `CONFIG_BSP_AUDIO_PCM`,
applications and shell controls use PCM; otherwise they use the direct player
APIs. Shared control functions remain available in both builds; direct transport APIs are excluded from PCM builds.

`wave_player.c` retains codec device instances, DMA buffers, the ring buffer and
hardware tasks. `audio_controls.c` owns microphone/speaker control state and the
shared `wave_player_set/get_*` control APIs declared in `audio_controls.h`.
`audio_pcm.c` implements the PCM device APIs and owns logical sessions, locks,
callback reservations and the reference-audio timeline. `wave_player_pcm.h`
defines a small runtime snapshot and typed engine operations; neither module
exposes its internal context. Device capabilities are cached at initialization.
Shell handlers stay in `audio_controls.c`. PCM builds use the session adapter
for serialization; direct builds call the shared control APIs. The shared core
does not call PCM, start tasks or reconfigure DMA.
Native drivers retain volume and gain across rate changes; the player does not
repeat their restore operations.

## Channels and capabilities

Channel setters require a nonzero mask. Bit N selects physical interleaved PCM
slot N in that direction; slot zero is bit `0x01`, not mask zero. Omitting the
shell mask invokes the global setter. Microphone masks must be subsets of the
configured `mic_mask`; reference and unknown slots are rejected. Playback masks
come from actual codec outputs, optionally narrowed by `speaker_mask` in the
hardware configuration. They are not inferred from mono/stereo source samples.

For a declared array with microphone slots 2 and 3 (`mic_mask=12`),
`mic_gain_db 18 4` selects only slot 2 and `mic_gain_db 24 8` selects only slot 3.
`mic_vol 80` sets both microphones. A slot identifies wiring, not a spatial
position; the board integration supplies the array's physical placement mapping.

`audio_channel <mic|speaker> <slot>` prints capabilities and confirmed state.

`get_mic_channel_caps/state` and `get_speaker_channel_caps/state` query one slot
number. Capabilities describe available controls, native dB steps and linked
hardware channels. A linked operation requires the complete linked group and
never silently changes an unselected peer. Software playback mute is independent
only where the actual PCM/DMA packing can address the selected output slots.

Gain requests must be finite and within the device's range. They use the closest
native step, with ties selecting the lower gain. A single min/max percentage
formula cannot represent codecs with missing or nonuniform steps. The startup
`CONFIG_WAVE_PLAYER_REC_DEFAULT_MIC_GAIN_DB` is bounded to the available range;
explicit API requests outside the range fail. Minimum gain or volume is not the
mute state. Unmuting at volume zero retains volume zero.

State describes confirmed settings, not register readback. Inspect its `valid`
bits. A failed native batch invalidates the affected control for every requested
slot because part of the request may have changed hardware. Unsupported requests
leave state unchanged; multi-channel operations do not promise rollback. The speaker
volume getter uses an output argument and returns an error for mixed or unknown
channel values. All-mute boolean getters mean every declared channel is muted;
use the microphone mute mask or channel states to distinguish partial mute.

## Capture and playback mute

Capture mute preserves reference/auxiliary slots, sample clocks and analog gain.
Raw capture callbacks receive read-only ADC samples and `muted_mic_mask`; they
must honor that mask when copying into existing writable storage. The recording
ring, I2S recorder and GMF queues follow this contract. Consumers with additional
queues recheck the current microphone mute mask before final mixing or sending.
The player allocates no extra mute frame and never modifies live DMA capture PCM.
Previously delivered or transmitted media cannot be recalled.

Playback mute is applied to existing writable PCM/DMA blocks, including physical
slot packing. It does not allocate another frame or stop shared capture clocks.

## Ownership and errors

Frontend selection is fixed at build time. PCM and GMF applications and shell
commands use `audio_pcm_*` so controls share the session mutex. The shared
`wave_player_set/get_*` functions implement that adapter; they are not an
alternative frontend for bypassing PCM serialization. Without PCM, direct
callers must serialize their own control operations. There is no runtime owner
handoff or fallback to another frontend after an error.

AVRCP volume callbacks select the configured control frontend. This routing does
not add PCM media transport support to the direct A2DP example.

Device setters validate all targets before starting a bus
write and propagate hardware failures. Invalid values, mask zero and undeclared
slots are rejected; unsupported controls do not fall back to global hardware
changes or software amplification. The old ambiguous control names and PGA
percentage APIs are not compatibility aliases.

`audio_info` reports explicit speaker/microphone mute states and shows mixed
speaker volume when channel levels differ. In PCM builds, direct player lifecycle, recording, register access and built-in
WAV/MP3 playback commands are not exported. The MP3 decoder library remains
available to GMF. In non-PCM builds, those commands retain their transport meanings.

PCM initializes the physical device once. Further `audio_pcm_init()` calls return
`-EALREADY` without applying a different hardware configuration. GMF provider
recreation reuses that initialized device; opening and closing logical capture or
playback sessions does not reinitialize the hardware.

The declared speaker mask selects application outputs. The physical playback
mask still describes all codec outputs: muting a narrowed role must not silence
unselected outputs. The data path uses `audio_controls_playback_is_muted()` only
for its whole-block silence optimization and applies the slot mute mask otherwise.

Direct `wave_player_start()` requires an initial format configuration. After
stop, it restores that format and the current recording-enable setting before
starting the task. PCM sessions retain their own explicit configuration lifecycle.
