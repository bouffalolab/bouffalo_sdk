# Xcodec IO Recording and Playback

The `bl618dg_orion` board configuration uses the same hardware wiring as Timebox.
JY6316 runs at 16 kHz with PCM16 over a four-slot DSP/TDM bus. Capture retains all
four channels. Each default read returns 512 bytes of interleaved PCM (64 frames,
4 ms), ordered CH0, CH1, CH2, CH3. The example extracts CH3 for mono playback in
slot 0 and fills the other output slots with silence.

Changing `record_format.channel_mask` in `main.c` automatically adjusts sample
indexing to the selected channels. The capture buffer and each read remain
512 bytes, independent of channel count and DMA block size. `0x08` selects CH3;
`0` selects all channels. For formats such as three channels whose frames do not
divide the buffer size, the driver retains remaining frame bytes across calls.
`process_pcm()` tracks the current channel to extract the playback channel from
the continuous sample stream. Statistics and `xcodec echo <channel>` use physical
channel numbers; channels excluded from capture cannot be selected for echo.
Masks take effect at device open. To change the format, stop IO, close the device
and reopen it.

`channel` is always the physical bus slot count. Bit n of `channel_mask` selects
CHn, starting at CH0. The mask selects application samples while the physical slot
count and DMA format remain as configured by the board. For Orion with `channel=4`:

| channel_mask | Selected physical channels | Application samples per PCM frame |
| --- | --- | --- |
| `0` or `0x0F` | CH0, CH1, CH2, CH3 | CH0, CH1, CH2, CH3 |
| `0x01` (`1 << 0`) | CH0 | CH0 |
| `0x08` (`1 << 3`) | CH3 | CH3 |
| `0x0A` (`(1 << 1) \| (1 << 3)`) | CH1, CH3 | CH1, CH3 |

Capture returns only selected samples. For example, `0x08` returns a continuous
mono CH3 PCM stream. Playback accepts samples in selected-channel order, places
them in the corresponding physical slots and fills unselected slots with silence.
With playback mask `0x0A`, the buffer contains
`CH1[0], CH3[0], CH1[1], CH3[1], ...`.
This example defaults to capture mask `0` and playback mask `0x01`.
`process_pcm()` extracts one capture channel and writes it to playback CH0. When
multiple playback channels are selected, it copies the same echo sample to each.
Mask bits must fit within `channel`: `0x10` is invalid for four slots. Mono boards
use `0` or `0x01`.

BL616 uses built-in AUDAC/AUADC with the standard board audio GPIO and
DMA configuration. For example:

```sh
make CHIP=bl616 BOARD=bl616dk
```

BL616 uses analog ADC capture by default and selects PDM when
`CONFIG_AUDIO_MIC_PDM_MODE=y`. It uses built-in AUDAC playback. Its board adapter
is in `bsp/board/bl616dk/board_audio.c/.h`.
Other boards must provide `board_audio.h` and recording/playback initialization in
their BSP.

Initialization follows these steps:

1. Optionally change the DMA and timeout fields of `xcodec_channel_default_config`
   in main; otherwise, use the driver defaults.
2. Call `xcodec_dev_board_init()` without arguments to initialize GPIO, control
   buses, Codec interfaces and board-selected data interfaces.
3. Use `xcodec_dev_cfg_t` to combine the returned data and Codec interfaces into
   `play_dev` and `record_dev`.
4. Pass the sample format to `xcodec_dev_open()`, set output controls, then read
   and write PCM in a loop.
5. Stop IO, close and delete devices, then call `xcodec_dev_board_deinit()` to
   release the interfaces and Codec resources.

Default macros are defined in driver headers. Board code uses native hardware
configurations and fills in wiring differences:

| Configuration | Default macro and board settings |
| --- | --- |
| DMA block size, period count, timeout | `XCODEC_CHANNEL_DEFAULT_CONFIG()`: 160 frames, 8 periods, 1000 ms |
| Sample format | Set field by field in main: PCM16, 16 kHz, board physical slot count; capture all slots, play CH0 |
| Orion I2S | `XCODEC_I2S_DEFAULT_CONFIG()`: board selects four-slot DSP/TDM, unmerged FIFO, DMA and clock callback |
| AUADC | `XCODEC_AUADC_DEFAULT_CONFIG()`: BL616 selects analog or PDM input |
| AUDAC | `XCODEC_AUDAC_DEFAULT_CONFIG()`: built-in mono PWM output |

Main configures capture and playback formats separately, with each field explained
in the initializer:

```c
static xcodec_dev_sample_info_t record_format = {
    .sample_rate = 16000,             /* Capture at 16 kHz. */
    .bits_per_sample = 16,            /* PCM16 samples. */
    .channel = BOARD_AUDIO_CHANNELS,  /* Physical slots: Orion 4, built-in ADC/DAC 1. */
    .channel_mask = 0,                /* 0: all slots; bit n selects CHn (0x08: CH3 only). */
    .mclk_multiple = 0,               /* Accept the board-selected MCLK. */
};
static xcodec_dev_sample_info_t play_format = {
    .sample_rate = 16000,             /* Match capture rate for echo. */
    .bits_per_sample = 16,            /* PCM16 samples. */
    .channel = BOARD_AUDIO_CHANNELS,  /* Physical slots on the playback bus. */
    .channel_mask = 0x01,             /* Bit 0: output to CH0; other slots carry silence. */
    .mclk_multiple = 0,               /* Accept the board-selected MCLK. */
};
```

Change `sample_rate` in both formats to change the rate. Change
`record_format.channel_mask` to select capture channels. The channel count remains
the physical slot count; use the mask to select a subset of slots.

`xcodec_dev_echo_task()` uses the common board entry point:

```c
xcodec_board_handle_t board = xcodec_dev_board_init();
xcodec_dev_cfg_t dev_cfg = {
    .dev_type = XCODEC_DEV_TYPE_OUT,
    .data_if = board->play_data,
    .codec_if = board->play_codec,
};
xcodec_dev_handle_t play_dev = xcodec_dev_new(&dev_cfg);
xcodec_dev_open(play_dev, &play_format);
```

The driver defines `xcodec_channel_default_config` using
`XCODEC_CHANNEL_DEFAULT_CONFIG()`. Before board initialization, main may change its
fields or assign a custom `xcodec_channel_cfg_t`. Changing it after initialization
does not affect existing DMA resources. Stop IO, delete devices, and deinitialize
and reinitialize the board to apply a new DMA configuration.

Orion shares one I2S data interface between capture and playback. Built-in
converters use separate AUADC and AUDAC interfaces. The board selects backends,
connects interfaces and releases them; main does not branch on backend type.
The board owns data and Codec interfaces, while main owns device handles. Sample
formats are passed only to device `open()`. To replace an external Codec, change
the board control interface; the data driver and PCM processing are independent
of the Codec model.

Built-in AUDAC remains muted after configuration. After opening playback, the
application sets volume to 60 and unmutes it. The first open on shared I2S starts
all configured DMA directions. Playback sends silence when unopened or empty;
unopened capture is discarded. The application does not need to prefill silence
or read an initial capture block. PCM buffers are static, and read/write sizes
are independent of DMA block size.
Changing DMA settings requires deleting and recreating the data interface.
Changing the sample format requires closing and reopening devices. Shared I2S
directions must use the same physical format; a running peer prevents rate changes.
`period_count` must be 2..1023, and DMA blocks must satisfy transfer-size and
cache-line alignment limits. Current data backends support PCM16. Orion has four
physical slots; BL616 has one.
Built-in converter duplex operation uses 8, 16, 24, 32 and 48 kHz from a common
fixed board reference clock. Orion configures the PLL through the board callback
at `open()`, then configures I2S and the Codec. Supported rates depend on hardware.
Both capture and playback masks use `0` to select all channels. Multiple selected
playback slots receive the same echo or test tone.
`read/write` lengths specify the requested byte count, not buffer capacity. A
successful call processes the entire requested length. Requests do not need PCM
frame or DMA block alignment. The driver retains partial frames across calls and
clears them on close.
The example reads `sizeof(record_pcm)` bytes, and `process_pcm()` returns the
number of playback bytes it generated:

```c
while (!xcodec_dev_read(record_dev, record_pcm, sizeof(record_pcm))) {
    xcodec_cli_process(play_dev);
    unsigned play_bytes = process_pcm(record_pcm, sizeof(record_pcm), play_pcm);
    if (xcodec_dev_write(play_dev, play_pcm, play_bytes)) {
        break;
    }
}
```

```sh
make CHIP=bl618dg BOARD=bl618dg_orion CPU_ID=ap
make CHIP=bl618dg flash COMX=/dev/ttyUSB2
```

After flashing, toggle RTS high and low to reset the board. Echo starts
automatically at boot. The serial baud rate is 2000000.

The serial CLI supports debugging on the board. `cli_xcodec.c` parses and executes
commands. Main handles recording/playback initialization, echo channel selection,
test tone generation and PCM statistics, exposing `audio_set_echo_channel()`,
`audio_start_tone()` and `audio_print_status()` to the CLI.
The audio loop calls `xcodec_cli_process(play_dev)` to execute queued commands,
then calls `process_pcm()`. Device control and audio processing run serially in
the same task. Commands:

```text
xcodec status          # Show volume, mute, echo channel and DAC registers
xcodec volume 60       # Set volume, 0..100
xcodec mute 1          # Mute; use mute 0 to unmute
xcodec echo 3          # Select echo channel; omit it to choose CH3 or the first captured channel
xcodec tone            # Play a 2-second 1 kHz tone at current volume, then resume echo
xcodec tone 1024       # Set tone PCM peak amplitude: 0..8192, default 4096
xcodec reg 0x3b        # Read a Codec register
xcodec reg 0x3c 0x10   # Write and read back; direct writes do not update cached volume/mute
```

Recording and playback handles are independent and share one I2S data interface.
They may open in either order. The first open starts all configured DMA
directions: unopened or empty playback sends silence, and unopened capture is
discarded. Opening or closing the peer does not restart DMA. The last close stops
the bus. Recording alone requires only opening the recording handle, without
opening playback or writing data.
External Codec and built-in AUDAC/AUADC controls implement `xcodec_if_t` under
`xcodec/device/`. Playback binds a control interface. Volume and mute calls through
`xcodec_dev_set_out_vol()` and `xcodec_dev_set_out_mute()` reach the BSP
`audio_codec_*` functions for external Codecs, protected by the board shared I2C
lock. Playback is initially muted. After open, the application sets volume and
mute state; close mutes playback without affecting recording.
The recording handle has a separate control interface. On JY6316, direction
control changes microphone ADC digital enable bits while retaining the DAC
reference and shared clocks. JY6316 microphone gain in dB and precise input mute
return unsupported because their hardware mappings have not been verified.
After stopping IO, confirm that both devices close successfully before deleting
them. `xcodec_dev_board_deinit()` releases data interfaces, deleting a shared
interface once, then releases recording and playback control interfaces. Orion
then deinitializes the native Codec owned by the board.
`xcodec/platform/xcodec_data_i2s.c` connects the device interface to I2S/DMA.
`platform/codec_data.c` implements cyclic DMA rings and blocking IO.
Data drivers do not depend on the Codec model or call the legacy `src/bl/codec/`
driver. They convert between application PCM and software queues according to the
board slot configuration. DMA workers process only completed DMA blocks.
The data interface owns I2S exclusively. Creation prepares rings and workers;
`open` configures the hardware. Normal IO and start/stop operations do not allocate
memory. Deletion releases buffers and deinitializes I2S. DMA rings and equally
sized software queues are allocated separately. Orion's default duplex total is
80 KiB, excluding descriptors, worker stacks and synchronization resources.
Playback underruns send silence without recreating devices.
The data interface's `channel.period_count` explicitly sets the DMA period count,
from 2 to 1023. `xcodec_dev_board_init()` copies the driver's
`xcodec_channel_default_config` into the board-selected backend configuration.
Each board provides `board_audio.h`. The SDK selects its BSP through `BOARD`, and
board CMake includes the audio implementation when `CONFIG_XCODEC_IO` is enabled.
The example does not select board source files or header paths itself.
At startup, `audio_set_echo_channel(-1)` selects CH3 if captured, otherwise the
lowest selected channel. Built-in converters default to CH0.
`xcodec echo <channel>` uses the same function to switch channels at runtime.
Main applies initial volume and mute through device APIs; the CLI can change
them at runtime. The application does not query hardware clocks. The I2S data
driver calls the board clock callback and configures the divider for the running
format.

The example plays one selected capture channel at default volume 60.
JY6311's percentage volume curve includes positive gain: 30 is about -58 dB,
60 is about -19.5 dB, and 75 is 0 dB. Volume 100 does not mean 0 dB. Avoid acoustic
feedback when using a speaker.
Every 100 reads, the serial log reports progress and the PCM range of each
captured channel. The interval depends on the sample rate and selected channel
count. PCM ranges and progress confirm digital data flow; audible playback and
echo require listening checks.
