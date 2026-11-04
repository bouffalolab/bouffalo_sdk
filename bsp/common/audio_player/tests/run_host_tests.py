#!/usr/bin/env python3
"""Player allocation, multichannel capture and codec routing with ASan/UBSan.

Host only; supported environment: Linux, GCC and Python 3. From the SDK root:
CC=gcc python3 bsp/common/audio_player/tests/run_host_tests.py
"""
import argparse
import json
import os
import re
from pathlib import Path
import subprocess
import sys
import tempfile

# Keep UBSan failures fatal even when the caller supplies other options.
os.environ['UBSAN_OPTIONS'] = os.environ.get('UBSAN_OPTIONS', '') + ':halt_on_error=1'

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--report-json', type=Path, help='Write machine-readable results to this output file.')
args = parser.parse_args()
executables = []
if args.report_json:
    args.report_json.parent.mkdir(parents=True, exist_ok=True)
    args.report_json.write_text(json.dumps({'status': 'running'}) + '\n')


def run_sanitized(output, timeout):
    subprocess.run([str(output)], check=True, timeout=timeout)
    executables.append(output.name)


def audit_legacy_ownership(source, header):
    def body(name):
        signature = re.search(r'^(?:static )?(?:int|void|uint32_t|uint8_t|bool) ' + re.escape(name) +
                              r'\([^;]*?\)\n\{', source, re.MULTILINE)
        if signature is None:
            return None
        return source[signature.start():source.index('\n}\n', signature.end()) + 3]

    declared = set(re.findall(r'^(?:int|void|uint32_t|uint8_t|bool) ((?:wave_player|wave_record)_\w+)\(',
                              header, re.MULTILINE))
    read_only = {'wave_player_get_volume', 'wave_player_get_mute', 'wave_player_get_buffered_bytes',
                 'wave_player_rec_get_mono', 'wave_record_is_enabled', 'wave_player_dump_stack'}
    unimplemented = {'wave_player_deinit', 'wave_player_write'}
    for name in unimplemented:
        assert body(name) is None, 'New legacy implementation needs an ownership policy: ' + name
    mutators = declared - read_only - unimplemented - {'wave_player_init'}
    private_mutators = {'wave_player_codec_cli_read', 'wave_player_codec_cli_write', 'wave_player_rec_dump_flash',
                        'cmd_rec_play', 'wave_player_play_wave_buffer'}
    for name in sorted(mutators | private_mutators):
        function = body(name)
        assert function is not None and 'wave_player_legacy_enter()' in function, name
        assert 'wave_player_legacy_leave()' in function, name
    assert 'wave_player_set_record_pga(' in body('cmd_rec_pga')
    assert 'wave_player_apply_rec_mic_pga(' not in body('cmd_rec_pga')
    return {'public_mutators': sorted(mutators), 'private_mutators': sorted(private_mutators)}


tests = Path(__file__).resolve().parent
player = tests.parent
codec = player.parent / 'audio_codec'
subprocess.run([sys.executable, str(tests / 'test_kconfig.py')], check=True)
flags = ['-std=c11', '-g', '-O1', '-Wall', '-Wextra', '-Werror',
         '-fsanitize=address,undefined', '-fno-sanitize-recover=all', '-fno-omit-frame-pointer']
if sys.platform.startswith('linux'):
    flags += ['-fno-pie', '-no-pie']
with tempfile.TemporaryDirectory(prefix='bsp-audio-tests-') as directory:
    common = [os.environ.get('CC', 'cc'), *flags, '-I' + str(tests / 'stubs'),
              '-I' + str(player), '-I' + str(codec), '-I' + directory]
    source = (player / 'wave_player.c').read_text()
    ownership_audit = audit_legacy_ownership(source, (player / 'wave_player.h').read_text())
    # An expected UB must stop the executable rather than merely print a warning.
    sanitizer_probe = Path(directory) / 'sanitizer_failfast.c'
    sanitizer_probe.write_text('int main(void)\n{\n    volatile int value = 15;\n'
                               '    volatile int shift = 28;\n    return (value << shift) == 0;\n}\n')
    probe_output = Path(directory) / 'sanitizer_failfast'
    subprocess.run([*common, str(sanitizer_probe), '-o', str(probe_output)], check=True)
    probe = subprocess.run([str(probe_output)], capture_output=True, text=True, timeout=10)
    assert probe.returncode != 0 and 'runtime error' in probe.stderr, 'UBSan did not stop on undefined behavior'
    print('Sanitizer fail-fast diagnostic and legacy ownership audit passed')
    codec_source = (codec / 'audio_codec_bl.c').read_text()
    state_begin = codec_source.index('typedef struct {')
    state_end = codec_source.index('\nstatic const audio_codec_bl_caps_t', state_begin)
    playback = codec_source[state_begin:state_end]
    for signature in ('static int audio_codec_bl_map_audac_rate(', 'static int audio_codec_bl_apply_volume(',
                      'static int audio_codec_bl_init_playback('):
        begin = codec_source.index(signature)
        finish = codec_source.index('\n}\n', begin) + 3
        playback += codec_source[begin:finish] + '\n'
    (Path(directory) / 'builtin_playback_under_test.h').write_text(playback)
    lhal = player.parents[2] / 'drivers/lhal'
    (Path(directory) / 'bflb_audac.h').write_text((lhal / 'include/bflb_audac.h').read_text())
    dac_source = (lhal / 'src/bflb_audac.c').read_text()
    begin = dac_source.index('int bflb_audac_init(')
    finish = dac_source.index('\n}\n', begin) + 3
    (Path(directory) / 'audac_init_under_test.h').write_text(dac_source[begin:finish])
    output = Path(directory) / 'builtin_playback_clock'
    subprocess.run([*common, '-DCONFIG_BSP_AUDIO_CODEC_BL_ENABLE=1', '-I' + str(lhal / 'include'),
                    str(tests / 'test_builtin_playback_clock.c'),
                    '-o', str(output)], check=True)
    run_sanitized(output, timeout=10)
    boards = player.parents[1] / 'board'
    for board_index, board_path in enumerate((boards / 'bl618dg_mstar/board_gpio_overlay.c',
                                             boards / 'bl618dgdk/board_gpio.c')):
        board_source = board_path.read_text()
        hooks = ''
        for signature in ('void board_audac_gpio_init(', 'void board_auadc_gpio_init('):
            begin = board_source.index(signature)
            finish = board_source.index('\n}\n', begin) + 3
            hooks += board_source[begin:finish] + '\n'
        (Path(directory) / 'board_builtin_clock_under_test.h').write_text(hooks)
        for pdm in (0, 1):
            output = Path(directory) / f'board_clock_{board_index}_{pdm}'
            input_mode = ['-DCONFIG_AUDIO_MIC_PDM_MODE=1'] if pdm else []
            subprocess.run([*common, *input_mode, str(tests / 'test_builtin_board_clock.c'),
                            '-o', str(output)], check=True)
            run_sanitized(output, timeout=10)
    start = source.index('static void audio_cap_proc(')
    end = source.index('\n#endif', source.index('\n}\n', start))
    capture = source[start:end]
    start = source.index('static int wave_player_rec_set_callback_internal(')
    end = source.index('\nstatic int wave_record_enable_internal(', start)
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
        run_sanitized(output, timeout=10)
    slot_helpers = ''
    for signature in ('static inline uint8_t audio_channel_mode_from_count(',
                      'static inline uint8_t audio_channel_count_clamp(',
                      'static uint8_t wave_player_i2s_tx_slot_count(',
                      'static void audio_fill_i2s_cfg('):
        start = source.index(signature)
        end = source.index('\n}\n', start) + 3
        slot_helpers += source[start:end] + '\n'
    (Path(directory) / 'wave_player_slots_under_test.h').write_text(slot_helpers)
    # Keep the real I2S declarations, resolving its core include to the host
    # stub instead of the neighboring RISC-V-only production core header.
    (Path(directory) / 'bflb_i2s.h').write_text(
        (player.parents[2] / 'drivers/lhal/include/bflb_i2s.h').read_text())
    transport_helpers = ''
    for signature in ('static bool audio_sample_rate_is_44k1_family(',
                      'static bool audio_sample_rate_share_pll_family(',
                      'static uint8_t wave_player_default_playback_channels(',
                      'static uint8_t wave_player_default_capture_channels(',
                      'static uint8_t wave_player_i2s_tx_slot_word_bytes('):
        start = source.index(signature)
        end = source.index('\n}\n', start) + 3
        transport_helpers += source[start:end] + '\n'
    (Path(directory) / 'wave_player_transport_helpers_under_test.h').write_text(transport_helpers)
    tx_start = source.index('static int wave_player_reload_tx_ring(')
    tx_end = source.index('\nstatic int wave_player_config_internal(', tx_start)
    tx_reload = source[tx_start:tx_end]
    start = source.index('static int wave_player_config_internal(')
    end = source.index('\nvoid wave_player_config(', start)
    (Path(directory) / 'wave_player_transport_config_under_test.h').write_text(tx_reload + '\n' + source[start:end])
    output = Path(directory) / 'transport_config'
    subprocess.run([*common, '-DCONFIG_WAVE_PLAYER_ENABLE_REC=1',
                    '-DCONFIG_I2S_DSP_MODE=1', '-DCONFIG_WAVE_PLAYER_I2S_DSP_LR_MERGE=1',
                    '-DCONFIG_WAVE_PLAYER_OUT_CHANNELS=1', '-DCONFIG_WAVE_PLAYER_REC_CHANNELS=3',
                    str(tests / 'test_transport_config.c'), '-o', str(output)], check=True)
    run_sanitized(output, timeout=10)
    for dsp, slots in ((0, 0), (1, 0), (1, 1), (1, 2), (1, 3), (1, 4), (1, 6)):
        output = Path(directory) / f'slots_{dsp}_{slots}'
        subprocess.run([*common, '-DCONFIG_AUDIO_CODEC_JY6316_ENABLE=1',
                        f'-DCONFIG_I2S_DSP_MODE={dsp}',
                        f'-DCONFIG_WAVE_PLAYER_I2S_TX_SLOTS={slots}',
                        str(tests / 'test_i2s_slots.c'), '-o', str(output)], check=True)
        run_sanitized(output, timeout=10)
    invalid_slots = subprocess.run([*common, '-DCONFIG_I2S_DSP_MODE=1',
                                   '-DCONFIG_WAVE_PLAYER_I2S_TX_SLOTS=5', '-fsyntax-only',
                                   str(tests / 'test_i2s_slots.c')], capture_output=True, text=True)
    assert invalid_slots.returncode != 0 and 'must be 0, 1, 2, 3, 4 or 6' in invalid_slots.stderr
    start = source.index('static bool wave_player_capture_layout_valid(')
    end = source.index('\nstatic void audio_cap_proc(', start)
    validator = '#if WAVE_PLAYER_ENABLE_REC\n' + source[start:end] + '\n#endif\n'
    start = source.index('int wave_player_init(')
    end = source.index('\nstatic int wave_player_feed_internal(', start)
    (Path(directory) / 'wave_player_init_under_test.h').write_text(validator + source[start:end])
    start = source.index('#if WAVE_PLAYER_ENABLE_RESAMPLE\n    bool use_resample =')
    end = source.index('\n    if (!audio_pcm_player_wait_reference())', start)
    (Path(directory) / 'wave_player_scratch_under_test.h').write_text(source[start:end])
    for recording in (0, 1):
        output = Path(directory) / ('player_init_' + str(recording))
        subprocess.run([*common, '-DCONFIG_WAVE_PLAYER_ENABLE_REC=' + str(recording),
                        str(tests / 'test_wave_player_init.c'), '-o', str(output)], check=True)
        run_sanitized(output, timeout=10)

    controls_start = source.index('static int wave_player_set_mute_internal(')
    controls_end = source.index('\n#if WAVE_PLAYER_ENABLE_REC\nstatic int wave_player_rec_set_callback_internal(', controls_start)
    rec_stop_start = source.index('static int wave_player_rec_stop_internal(')
    rec_stop_end = source.index('\n}\n', source.index('int wave_player_rec_stop(', rec_stop_start)) + 3
    codec_read_start = source.index('static int wave_player_codec_cli_read(')
    codec_read_end = source.index('\n}\n', codec_read_start) + 3
    (Path(directory) / 'wave_player_controls_under_test.h').write_text(
        source[controls_start:controls_end] + '\n' + source[rec_stop_start:rec_stop_end] + '\n' +
        source[codec_read_start:codec_read_end])
    ownership = '#if defined(CONFIG_BSP_AUDIO_PCM)\n'
    for enabled in (True, False):
        for signature in ('static bool wave_player_legacy_enter(void)\n',
                          'static void wave_player_legacy_leave(void)\n'):
            begin = source.index(signature, source.index('struct audio_pcm_session {') if enabled else 0)
            finish = source.index('\n}\n', begin) + 3
            ownership += source[begin:finish] + '\n'
        ownership += '#else\n' if enabled else '#endif\n'
    (Path(directory) / 'wave_player_ownership_under_test.h').write_text(ownership)
    for pcm in (0, 1):
        output = Path(directory) / ('legacy_controls_' + str(pcm))
        pcm_option = ['-DCONFIG_BSP_AUDIO_PCM=1'] if pcm else []
        subprocess.run([*common, *pcm_option, str(tests / 'test_legacy_controls.c'),
                        '-o', str(output)], check=True)
        run_sanitized(output, timeout=10)

    state_start = source.index('struct audio_pcm_session {')
    state_end = source.index('\n#endif\n\n#if WAVE_PLAYER_DIAG', state_start)
    pcm_start = source.index('#if defined(CONFIG_BSP_AUDIO_PCM)\n/* The PCM device')
    stop_start = source.index('static int wave_player_stop_timeout_internal(')
    stop_end = source.index('\nint wave_player_stop(', stop_start)
    (Path(directory) / 'audio_pcm_under_test.h').write_text(
        tx_reload + '\n' + source[state_start:state_end] + '\n' + source[stop_start:stop_end] + '\n' + source[pcm_start:])
    for legacy_ring in (0, 1):
        output = Path(directory) / ('audio_pcm_sessions_' + str(legacy_ring))
        subprocess.run([*common, '-pthread', '-DCONFIG_BSP_AUDIO_PCM=1',
                        '-DCONFIG_WAVE_PLAYER_ENABLE_REC=1', '-DCONFIG_WAVE_PLAYER_REC_CHANNELS=4',
                        '-DCONFIG_WAVE_PLAYER_ENABLE_REC_RB=' + str(legacy_ring),
                        str(tests / 'test_audio_pcm.c'), '-o', str(output)], check=True)
        run_sanitized(output, timeout=15)
    output = Path(directory) / 'es8388_input'
    subprocess.run([*common, '-include', 'stdio.h', str(tests / 'test_es8388_input.c'),
                    str(codec / 'audio_codec_es8388.c'), '-o', str(output)], check=True)
    run_sanitized(output, timeout=10)
    output = Path(directory) / 'playback_scratch'
    subprocess.run([*common, str(tests / 'test_playback_scratch.c'), '-o', str(output)], check=True)
    run_sanitized(output, timeout=10)

    for slots in (0, 4):
        output = Path(directory) / f'jy6316_clock_{slots}'
        subprocess.run([*common, '-DCONFIG_I2S_DSP_MODE=1',
                        f'-DCONFIG_WAVE_PLAYER_I2S_TX_SLOTS={slots}',
                        '-ffunction-sections', '-fdata-sections',
                        str(tests / 'test_jy6316_clock.c'),
                        str(codec / 'audio_codec_jy6316.c'),
                        str(codec / 'audio_codec_jy6311.c'),
                        str(codec / 'audio_codec_jy6166.c'),
                        '-Wl,--gc-sections', '-o', str(output)], check=True)
        run_sanitized(output, timeout=10)

    start = source.index('static void wave_player_task(')
    end = source.index('\n#if WAVE_PLAYER_ENABLE_REC', source.index('\ncleanup:', start))
    # Use the production enabled/disabled macro as well as the production loop.
    # Otherwise a mock macro can hide the exact diagnostic-reset regression.
    counters = [line for line in source.splitlines()
                if line.startswith('#define WAVE_PLAYER_DIAG_GET_TX_ISR()')]
    assert len(counters) == 2
    for enabled in (0, 1):
        (Path(directory) / 'wave_player_loop_under_test.h').write_text(
            counters[0 if enabled else 1] + '\n' + source[start:end])
        output = Path(directory) / f'playback_dma_{enabled}'
        subprocess.run([*common, str(tests / 'test_playback_dma.c'), '-o', str(output)], check=True)
        run_sanitized(output, timeout=10)

result = {'status': 'passed', 'positive_sanitizer_executables': len(executables),
          'executables': executables, 'expected_sanitizer_failures': 1,
          'sanitizer_failfast': True, 'ownership_audit': ownership_audit}
if args.report_json:
    args.report_json.write_text(json.dumps(result, indent=2) + '\n')
print(f'ASan/UBSan executables passed: {len(executables)}; expected fail-fast diagnostic: 1')
