# minialsa

English | [简体中文](README_CN.md)

minialsa provides embedded ALSA-style PCM, sound card, and mixer interfaces.
Use it to play or capture raw PCM without the full media player. For compressed
files, network streams, or arbitration between audio sources, use
[smart_audio](../smart_audio/README.md) and the media framework above minialsa.

## Layout and Backend

| Path | Purpose |
| --- | --- |
| [include/alsa/pcm.h](include/alsa/pcm.h) | PCM types, parameters, playback, and capture APIs |
| [include/alsa/mixer.h](include/alsa/mixer.h) | Mixer elements and volume APIs |
| [include/alsa/snd.h](include/alsa/snd.h) | Sound card and backend driver interfaces |
| `src/pcm.c` | PCM handles, state, parameters, and I/O dispatch |
| `src/mixer.c`, `src/snd.c`, `src/core.c` | Mixer, sound card registration, and shared support |
| [../drv_snd](../drv_snd/) | Existing minialsa backend using xcodec |

The output path is `application → minialsa → drv_snd → xcodec → board audio`.
minialsa does not configure board pins, DMA, clocks, or an external codec by itself.
API names follow ALSA conventions, but availability and behavior are defined by
these headers and the selected backend; this is a subset of desktop ALSA.

## SDK Configuration

Start from [examples/audio/minialsa](../../../examples/audio/minialsa/) and enable:

```makefile
CONFIG_FREERTOS=y
CONFIG_MULTIMEDIA=y
CONFIG_MSP_USE_CODEC_ONLY=y
CONFIG_MINIALSA=y
CONFIG_DRV_SND=y
CONFIG_XCODEC=y
CONFIG_XPORT=y
CONFIG_XUTILS=y
```

The reduced `CONFIG_MSP_USE_CODEC_ONLY` path suits direct PCM applications.
Disable it when integrating a full media player. Configure the target audio
hardware separately; see the [multimedia guide](../README.md) and
[board audio port](../../../examples/audio/common/multimedia_port_common.c).

Initialize board resources, then register the sound card using
`snd_card_register()` from [devices/drv_snd.h](../drv_snd/include/devices/drv_snd.h).
Use board-appropriate `snd_config_t` gain settings. Close all PCM and mixer handles
before calling `snd_card_unregister()`.
The existing backend exposes `pcmP0` for playback and `pcmC0` for capture.

## PCM Workflow

1. Open the required stream with `msp_pcm_open()` and `MSP_PCM_STREAM_PLAYBACK` or `MSP_PCM_STREAM_CAPTURE`.
2. Configure access, format, channels, and sample rate. `msp_pcm_set_params()` provides a basic setup; use `msp_pcm_hw_params_*` and `msp_pcm_hw_params()` for explicit period/buffer sizes.
3. Apply optional software thresholds with `msp_pcm_sw_params_*` and `msp_pcm_sw_params()`.
4. Write interleaved PCM with `msp_pcm_writei()`, or capture with `msp_pcm_readi()` / `msp_pcm_readn()`. Handle partial transfers and check negative returns.
5. For playback, call `msp_pcm_drain()` to finish queued data or `msp_pcm_drop()` to discard it. Close with `msp_pcm_close()`.

A **PCM frame** contains one sample for every channel. For stereo 16-bit PCM,
one frame is 4 bytes. I/O lengths and successful return values, `period_size`,
and `buffer_size` use frames. Convert bytes with `msp_pcm_bytes_to_frames()` and
`msp_pcm_frames_to_bytes()`; packed and padded formats can have different storage widths.

`msp_pcm_set_params()` takes `latency` in **microseconds** (for example, `200000`
is 200 ms). Its current implementation ignores `soft_resample`. Supported
formats, rates, channel counts, and DMA alignment depend on the backend.

### Playback Example

This function assumes board audio and the sound card are already initialized.
It accepts signed 16-bit mono PCM at 16 kHz; `frames` counts samples for this mono format.

```c
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <alsa/pcm.h>

int play_pcm16_mono(const int16_t *samples, size_t frames)
{
    msp_pcm_t *pcm = NULL;
    int ret = msp_pcm_open(&pcm, "pcmP0", MSP_PCM_STREAM_PLAYBACK, 0);
    if (ret < 0) {
        return ret;
    }

    ret = msp_pcm_set_params(pcm, SND_PCM_FORMAT_S16_LE,
                            MSP_PCM_ACCESS_RW_INTERLEAVED,
                            1, 16000, 0, 200000);
    if (ret < 0) {
        goto done;
    }

    while (frames > 0) {
        msp_pcm_sframes_t written = msp_pcm_writei(pcm, samples, frames);
        if (written <= 0) {
            ret = written < 0 ? (int)written : -EIO;
            goto done;
        }
        samples += written;
        frames -= written;
    }
    ret = msp_pcm_drain(pcm);

done:
    {
        int close_ret = msp_pcm_close(pcm);
        return ret < 0 ? ret : close_ret;
    }
}
```

For capture, open `pcmC0` with `MSP_PCM_STREAM_CAPTURE`, apply supported input
parameters, and loop over `msp_pcm_readi()` into a writable buffer. For
`msp_pcm_readn()`, pass one buffer per channel as required by the selected access mode.
Capture also needs a configured ADC/microphone or I2S input.

## Mixer and Error Handling

Open a mixer with `msp_mixer_open()`, attach it to the backend sound card using
`msp_mixer_attach()`, then load controls with `msp_mixer_load()`. Enumerate elements
and use `msp_mixer_selem_*` to query ranges and adjust playback/capture volume.
Names, ranges, and dB mappings are backend-specific. Close with `msp_mixer_close()`.

For `-EPIPE` (playback underrun or capture overrun), call
`msp_pcm_recover(pcm, error, 0)` and check the result before retrying. The current
recovery path reopens the device and reapplies hardware parameters; it can
interrupt queued audio. Other errors are returned to the caller. Use
`msp_pcm_avail()` / `msp_pcm_wait()` for pacing as appropriate for the backend.

## Build and Hardware Verification

From the SDK root:

```bash
make -C examples/audio/minialsa CHIP=bl616 BOARD=bl616dk
```

After flashing and starting the example, run commands in the serial shell:

```text
minialsa_play wa
minialsa_play sin
minialsa_api_test all wa
minialsa_api_test playback sin
minialsa_api_test capture
minialsa_output_clock_test 48000
```

`wa` and `sin` are built-in 16 kHz, 16-bit, mono samples. Capture requires a valid
input. The clock test writes silence for scope measurement and requires a rate
divisible by 8000 Hz. See [app_minialsa.c](../../../examples/audio/minialsa/app/player/app_minialsa.c)
for command arguments and checks. Compilation verifies integration; physical
playback, capture, and clock accuracy require board testing.

If opening fails, check registration and device names. If output is silent,
check PA, mute, volume, codec initialization, and I2S pins/clocks. If audio
stutters, check data supply, task scheduling, period/buffer sizes, and frame units.
