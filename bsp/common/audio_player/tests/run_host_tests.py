#!/usr/bin/env python3
"""Player allocation, multichannel capture and codec routing with ASan/UBSan.

Host only; supported environment: Linux, GCC and Python 3. From the SDK root:
CC=gcc python3 bsp/common/audio_player/tests/run_host_tests.py
"""
import os
from pathlib import Path
import subprocess
import sys
import tempfile

tests = Path(__file__).resolve().parent
player = tests.parent
codec = player.parent / 'audio_codec'
subprocess.run([sys.executable, str(tests / 'test_kconfig.py')], check=True)
flags = ['-std=c11', '-g', '-O1', '-Wall', '-Wextra', '-Werror',
         '-fsanitize=address,undefined', '-fno-omit-frame-pointer']
if sys.platform.startswith('linux'):
    flags += ['-fno-pie', '-no-pie']
with tempfile.TemporaryDirectory(prefix='bsp-audio-tests-') as directory:
    common = [os.environ.get('CC', 'cc'), *flags, '-I' + str(tests / 'stubs'),
              '-I' + str(player), '-I' + str(codec), '-I' + directory]
    source = (player / 'wave_player.c').read_text()
    start = source.index('static void audio_cap_proc(')
    end = source.index('\n#endif', source.index('\n}\n', start))
    capture = source[start:end]
    start = source.index('int wave_player_rec_set_callback(')
    end = source.index('\nint wave_record_enable(', start)
    capture += '\n' + source[start:end]
    (Path(directory) / 'wave_player_capture_under_test.h').write_text(capture)
    for dsp, channels, ring, blocks in (
            (0, 1, 0, 8), (0, 1, 1, 8), (0, 2, 0, 8), (0, 2, 1, 8),
            (1, 4, 0, 8), (1, 4, 1, 8), (0, 2, 1, 256), (0, 2, 1, 257)):
        output = Path(directory) / f'capture_{dsp}_{channels}_{ring}_{blocks}'
        subprocess.run([*common, '-DCONFIG_WAVE_PLAYER_ENABLE_REC=1',
                        f'-DCONFIG_I2S_DSP_MODE={dsp}',
                        f'-DCONFIG_WAVE_PLAYER_REC_CHANNELS={channels}',
                        f'-DCONFIG_WAVE_PLAYER_ENABLE_REC_RB={ring}',
                        f'-DCONFIG_WAVE_PLAYER_REC_DMA_BLOCKS={blocks}',
                        str(tests / 'test_capture.c'), '-o', str(output)], check=True)
        subprocess.run([str(output)], check=True, timeout=10)
    slot_helpers = ''
    for signature in ('static inline uint8_t audio_channel_count_clamp(',
                      'static uint8_t wave_player_i2s_tx_slot_count('):
        start = source.index(signature)
        end = source.index('\n}\n', start) + 3
        slot_helpers += source[start:end] + '\n'
    (Path(directory) / 'wave_player_slots_under_test.h').write_text(slot_helpers)
    for dsp, slots in ((0, 0), (1, 0), (1, 4), (1, 6)):
        output = Path(directory) / f'slots_{dsp}_{slots}'
        subprocess.run([*common, f'-DCONFIG_I2S_DSP_MODE={dsp}',
                        f'-DCONFIG_WAVE_PLAYER_I2S_TX_SLOTS={slots}',
                        str(tests / 'test_i2s_slots.c'), '-o', str(output)], check=True)
        subprocess.run([str(output)], check=True, timeout=10)
    invalid_slots = subprocess.run([*common, '-DCONFIG_I2S_DSP_MODE=1',
                                   '-DCONFIG_WAVE_PLAYER_I2S_TX_SLOTS=5', '-fsyntax-only',
                                   str(tests / 'test_i2s_slots.c')], capture_output=True, text=True)
    assert invalid_slots.returncode != 0 and 'must be 0, 1, 2, 3, 4 or 6' in invalid_slots.stderr
    start = source.index('static bool wave_player_capture_layout_valid(')
    end = source.index('\nstatic void audio_cap_proc(', start)
    validator = '#if WAVE_PLAYER_ENABLE_REC\n' + source[start:end] + '\n#endif\n'
    start = source.index('int wave_player_init(')
    end = source.index('\nint wave_player_feed(', start)
    (Path(directory) / 'wave_player_init_under_test.h').write_text(validator + source[start:end])
    start = source.index('#if WAVE_PLAYER_ENABLE_RESAMPLE\n    bool use_resample =')
    end = source.index('\n    bool ping = true;', start)
    (Path(directory) / 'wave_player_scratch_under_test.h').write_text(source[start:end])
    for recording in (0, 1):
        output = Path(directory) / ('player_init_' + str(recording))
        subprocess.run([*common, '-DCONFIG_WAVE_PLAYER_ENABLE_REC=' + str(recording),
                        str(tests / 'test_wave_player_init.c'), '-o', str(output)], check=True)
        subprocess.run([str(output)], check=True, timeout=10)
    output = Path(directory) / 'es8388_input'
    subprocess.run([*common, '-include', 'stdio.h', str(tests / 'test_es8388_input.c'),
                    str(codec / 'audio_codec_es8388.c'), '-o', str(output)], check=True)
    subprocess.run([str(output)], check=True, timeout=10)
    output = Path(directory) / 'playback_scratch'
    subprocess.run([*common, str(tests / 'test_playback_scratch.c'), '-o', str(output)], check=True)
    subprocess.run([str(output)], check=True, timeout=10)
