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



def function_source(source, name):
    signature = re.search(r'^(?:static (?:inline )?)?(?:int|void|uint32_t|uint16_t|uint8_t|bool) ' +
                          re.escape(name) + r'\([^;]*?\)\s*\{', source, re.MULTILINE)
    assert signature is not None, 'Missing production function: ' + name
    return source[signature.start():source.index('\n}\n', signature.end()) + 3]


def audit_compile_time_api(common, header, controls_header):
    declared = set(re.findall(r'^(?:int|void|uint32_t|uint16_t|uint8_t|bool) ((?:wave_player|wave_record)_\w+)\(',
                              header, re.MULTILINE))
    shared = {'wave_player_get_mic_channel_caps', 'wave_player_get_mic_channel_state',
              'wave_player_get_speaker_channel_caps', 'wave_player_get_speaker_channel_state',
              'wave_player_get_mic_mute_mask', 'wave_player_get_speaker_volume',
              'wave_player_get_speaker_mute', 'wave_player_get_mic_mute',
              'wave_player_get_buffered_bytes', 'wave_player_dump_stack', 'wave_player_is_running'}
    legacy = sorted(declared - shared)
    for name in legacy:
        probe = '#include "wave_player.h"\nint main(void) { (void)&' + name + '; return 0; }\n'
        for pcm in (False, True):
            option = ['-DCONFIG_BSP_AUDIO_PCM=1'] if pcm else []
            result = subprocess.run([*common, *option, '-DCONFIG_WAVE_PLAYER_ENABLE_REC=1',
                                     '-x', 'c', '-fsyntax-only', '-'], input=probe,
                                    capture_output=True, text=True)
            assert (result.returncode != 0) == pcm, (name, pcm, result.stderr)
            if pcm:
                assert name in result.stderr and 'undeclared' in result.stderr, result.stderr
    shared_controls = sorted(set(re.findall(
        r'^(?:int|void|uint32_t|uint16_t|uint8_t|bool) (wave_player_\w+)\(', controls_header, re.MULTILINE)))
    for name in shared_controls:
        probe = '#include "wave_player.h"\nint main(void) { (void)&' + name + '; return 0; }\n'
        for pcm in (False, True):
            option = ['-DCONFIG_BSP_AUDIO_PCM=1'] if pcm else []
            subprocess.run([*common, *option, '-DCONFIG_WAVE_PLAYER_ENABLE_REC=1',
                            '-x', 'c', '-fsyntax-only', '-'], input=probe, text=True, check=True)
    return {'legacy_api_declarations_checked': legacy, 'pcm_header_hides_legacy': True,
            'shared_control_declarations_checked': shared_controls}



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
    pcm_source = (player / 'audio_pcm.c').read_text()
    pcm_boundary = (player / 'wave_player_pcm.h').read_text()
    control_source = (player / 'audio_controls.c').read_text()
    assert 'struct audio_pcm_session {' in pcm_source and 'struct audio_pcm_session {' not in source
    assert 'g_pcm' not in source and 'g_audio' not in pcm_source
    assert not re.search(r'\bwave_player_pcm_(?:init|open|close)\s*\(', source + pcm_source + pcm_boundary)
    assert not re.search(r'%[-+ #0]*[0-9]*(?:\.[0-9]+)?[fFeEgGaA]', control_source), \
        'Audio controls must work without printf floating-point support'
    ownership_audit = audit_compile_time_api(
        common, (player / 'wave_player.h').read_text(), (player / 'audio_controls.h').read_text())
    exports = dict((alias, command) for command, alias in re.findall(
        r'SHELL_CMD_EXPORT_ALIAS\(\s*(\w+)\s*,\s*(\w+)\s*,', source + '\n' + control_source))
    expected_controls = {'speaker_vol': 'cmd_speaker_vol',
                         'speaker_mute': 'cmd_speaker_mute',
                         'mic_vol': 'cmd_mic_vol', 'mic_gain_db': 'cmd_mic_gain_db', 'mic_mute': 'cmd_mic_mute',
                         'audio_channel': 'cmd_audio_channel'}
    assert all(exports.get(alias) == command for alias, command in expected_controls.items())
    assert not set(('audio_vol', 'audio_mute', 'rec_vol', 'rec_pga', 'mic_gain')) & exports.keys()
    public_header = (player / 'wave_player.h').read_text() + (player / 'audio_pcm.h').read_text()
    for prefix in ('wave_player', 'audio_pcm'):
        for operation in ('set', 'get'):
            for old_role in ('volume', 'mute', 'record_volume', 'record_pga', 'record_mute'):
                assert not re.search(r'\b' + prefix + '_' + operation + '_' + old_role + r'\s*\(', public_header)
    # An expected UB must stop the executable rather than merely print a warning.
    sanitizer_probe = Path(directory) / 'sanitizer_failfast.c'
    sanitizer_probe.write_text('int main(void)\n{\n    volatile int value = 15;\n'
                               '    volatile int shift = 28;\n    return (value << shift) == 0;\n}\n')
    probe_output = Path(directory) / 'sanitizer_failfast'
    subprocess.run([*common, str(sanitizer_probe), '-o', str(probe_output)], check=True)
    probe = subprocess.run([str(probe_output)], capture_output=True, text=True, timeout=10)
    assert probe.returncode != 0 and 'runtime error' in probe.stderr, 'UBSan did not stop on undefined behavior'
    print('Sanitizer fail-fast diagnostic and compile-time frontend API audit passed')
    gain_source = (codec / 'audio_codec.c').read_text()
    begin = gain_source.index('int audio_codec_get_mic_channel_caps(')
    end = gain_source.index('int audio_codec_set_sample_rate(', begin)
    gain_helpers = "#include <math.h>\n" + gain_source[begin:end]
    (Path(directory) / 'audio_codec_gain_under_test.h').write_text(gain_helpers)
    native_source = (codec / 'audio_codec_internal.h').read_text()
    (Path(directory) / 'audio_codec_native_gain_under_test.h').write_text(
        function_source(native_source, 'audio_codec_gain_step_index'))
    (Path(directory) / 'wave_player_pcm_fill_under_test.h').write_text(
        function_source(source, 'pcm_fill_pcm_block'))
    # Compile the actual controls module. The host shell declaration marks
    # private handlers unused; fixtures exercise the CLI handlers separately.
    (Path(directory) / 'shell.h').write_text(
        '#define SHELL_CMD_EXPORT_ALIAS(function, alias, description) '
        'static int function(int, char **) __attribute__((unused))\n')
    controls_build = ['-DCONFIG_WAVE_PLAYER_REC_DEFAULT_MIC_GAIN_DB=6',
                      '-ffunction-sections', '-fdata-sections', str(player / 'audio_controls.c'),
                      '-Wl,--gc-sections']
    public_controls = {'wave_player_set_mic_channel_volume', 'wave_player_set_mic_volume',
                       'wave_player_set_mic_channel_gain_db', 'wave_player_set_mic_gain_db',
                       'wave_player_set_mic_channel_mute', 'wave_player_set_mic_mute',
                       'wave_player_set_speaker_channel_volume', 'wave_player_set_speaker_volume',
                       'wave_player_set_speaker_channel_mute', 'wave_player_set_speaker_mute'}
    internal_controls = {name + '_internal' for name in public_controls if '_channel_' in name}
    control_symbol_flags = ['-Wl,--undefined=' + name for name in public_controls]
    for pcm in (0, 1):
        output = Path(directory) / f'control_core_{pcm}'
        pcm_option = ['-DCONFIG_BSP_AUDIO_PCM=1'] if pcm else []
        subprocess.run([*common, *pcm_option, '-DCONFIG_WAVE_PLAYER_ENABLE_REC=1',
                        str(tests / 'test_control_core.c'), *controls_build, *control_symbol_flags, '-lm', '-o', str(output)], check=True)
        symbols = {line.split()[-1] for line in subprocess.check_output(
            ['nm', '-g', '--defined-only', str(output)], text=True).splitlines()}
        assert public_controls <= symbols
        assert not internal_controls & symbols
        run_sanitized(output, timeout=10)
    ownership_audit['control_symbols_shared_by_pcm_modes'] = True
    for default_volume in (None, 0, 50, 100):
        suffix = 'fallback' if default_volume is None else str(default_volume)
        output = Path(directory) / ('control_defaults_' + suffix)
        option = [] if default_volume is None else [
            '-DCONFIG_WAVE_PLAYER_DEFAULT_SPEAKER_VOLUME=' + str(default_volume)]
        subprocess.run([*common, *option, str(tests / 'test_audio_controls_defaults.c'),
                        *controls_build, '-lm', '-o', str(output)], check=True)
        run_sanitized(output, timeout=10)
    for default_volume in (-1, 101):
        invalid_default = subprocess.run(
            [*common, '-DCONFIG_WAVE_PLAYER_DEFAULT_SPEAKER_VOLUME=' + str(default_volume),
             '-x', 'c', '-fsyntax-only', '-'], input='#include "audio_controls.h"\n',
            capture_output=True, text=True)
        assert invalid_default.returncode != 0
        assert 'CONFIG_WAVE_PLAYER_DEFAULT_SPEAKER_VOLUME' in invalid_default.stderr
    ownership_audit['default_speaker_volume_bounds_checked'] = True
    codec_source = (codec / 'audio_codec_bl.c').read_text()
    state_begin = codec_source.index('typedef struct {')
    state_end = codec_source.index('\nstatic const audio_codec_bl_caps_t', state_begin)
    playback = codec_source[state_begin:state_end]
    internal = (codec / 'audio_codec_internal.h').read_text()
    for signature in ('static inline int audio_codec_native_caps(',
                      'static inline int audio_codec_gain_step_index('):
        begin = internal.index(signature)
        finish = internal.index('\n}\n', begin) + 3
        playback += internal[begin:finish] + '\n'
    for signature in ('static audio_codec_bl_state_t *audio_codec_bl_get_state(',
                      'static int audio_codec_bl_map_audac_rate(', 'static int audio_codec_bl_apply_volume(',
                      'static int audio_codec_bl_apply_record_volume(', 'static int audio_codec_bl_apply_mic_gain_db(',
                      'static int audio_codec_bl_set_mute(', 'static int audio_codec_bl_set_record_volume(',
                      'static int audio_codec_bl_set_mic_channel_volume(', 'static int audio_codec_bl_set_mic_channel_gain_db(',
                      'static int audio_codec_bl_get_mic_channel_caps(',
                      'static int audio_codec_bl_init_playback('):
        begin = codec_source.index(signature)
        finish = codec_source.index('\n}\n', begin) + 3
        playback += codec_source[begin:finish] + '\n'
    (Path(directory) / 'builtin_playback_under_test.h').write_text(playback)
    lhal = player.parents[2] / 'drivers/lhal'
    (Path(directory) / 'bflb_audac.h').write_text((lhal / 'include/bflb_audac.h').read_text())
    (Path(directory) / 'bflb_auadc.h').write_text((lhal / 'include/bflb_auadc.h').read_text())
    dac_source = (lhal / 'src/bflb_audac.c').read_text()
    begin = dac_source.index('int bflb_audac_init(')
    finish = dac_source.index('\n}\n', begin) + 3
    (Path(directory) / 'audac_init_under_test.h').write_text(dac_source[begin:finish])
    output = Path(directory) / 'builtin_playback_clock'
    subprocess.run([*common, '-DCONFIG_BSP_AUDIO_CODEC_BL_ENABLE=1', '-DBL616=1', '-DAUDIO_CODEC_BL_AUADC_INPUT_MODE=0', '-I' + str(lhal / 'include'),
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
    start = source.index('static void pcm_fill_pcm_block(')
    end = source.index('\n}\n', start) + 3
    (Path(directory) / 'wave_player_pcm_fill_under_test.h').write_text(source[start:end])
    for dsp in (0, 1):
        output = Path(directory) / f'pcm_mute_{dsp}'
        subprocess.run([*common, f'-DCONFIG_I2S_DSP_MODE={dsp}',
                        str(tests / 'test_pcm_mute.c'), '-o', str(output)], check=True)
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
    transport_helpers = ''.join(function_source(source, name) + '\n' for name in (
        'audio_sample_rate_is_44k1_family', 'audio_sample_rate_share_pll_family',
        'wave_player_default_playback_channels', 'wave_player_default_capture_channels',
        'wave_player_i2s_tx_slot_word_bytes'))
    (Path(directory) / 'wave_player_transport_helpers_under_test.h').write_text(transport_helpers)
    tx_reload = function_source(source, 'wave_player_reload_tx_ring')
    (Path(directory) / 'wave_player_transport_config_under_test.h').write_text(
        tx_reload + '\n' + function_source(source, 'wave_player_config_internal'))
    for pcm in (0, 1):
        for reset in (True, False):
            name = 'transport_config' if reset else 'transport_config_no_reset'
            output = Path(directory) / (name if not pcm else name + '_pcm')
            reset_option = [] if reset else ['-DTEST_NO_I2S_RESET=1']
            pcm_option = ['-DCONFIG_BSP_AUDIO_PCM=1'] if pcm else []
            subprocess.run([*common, *reset_option, *pcm_option, '-DCONFIG_WAVE_PLAYER_ENABLE_REC=1',
                            '-DCONFIG_I2S_DSP_MODE=1',
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
    validator = '#if WAVE_PLAYER_ENABLE_REC\n' + function_source(source, 'wave_player_capture_layout_valid') + '\n#endif\n'
    (Path(directory) / 'wave_player_init_under_test.h').write_text(
        validator + function_source(source, 'wave_player_init'))
    start = source.index('#if WAVE_PLAYER_ENABLE_RESAMPLE\n    bool use_resample =')
    end = source.index('\n    if (!audio_pcm_player_wait_reference())', start)
    (Path(directory) / 'wave_player_scratch_under_test.h').write_text(source[start:end])
    for recording in (0, 1):
        output = Path(directory) / ('player_init_' + str(recording))
        subprocess.run([*common, '-DCONFIG_WAVE_PLAYER_ENABLE_REC=' + str(recording),
                        str(tests / 'test_wave_player_init.c'), '-o', str(output)], check=True)
        run_sanitized(output, timeout=10)

    (Path(directory) / 'wave_player_restart_under_test.h').write_text(
        function_source(source, 'wave_player_start') + '\n')
    output = Path(directory) / 'player_restart'
    subprocess.run([*common, str(tests / 'test_wave_player_restart.c'), '-o', str(output)], check=True)
    run_sanitized(output, timeout=10)

    legacy_controls = ''.join(function_source(source, name) + '\n' for name in (
        'wave_record_enable_internal', 'wave_record_enable', 'wave_record_is_enabled',
        'wave_player_rec_stop_internal', 'wave_player_rec_stop', 'wave_player_codec_cli_read'))
    (Path(directory) / 'wave_player_controls_under_test.h').write_text(
        '#if !defined(CONFIG_BSP_AUDIO_PCM)\n' + legacy_controls + '#endif\n')
    (Path(directory) / 'wave_player_rec_start_under_test.h').write_text(
        function_source(source, 'wave_player_rec_start_internal') + '\n' +
        function_source(source, 'wave_player_rec_start'))
    # Exercise the actual standalone CLI module with the fixture's API backends.
    cli_source = control_source[control_source.index('static int parse_unsigned('):]
    cli_controls = '\n'.join(line for line in cli_source.splitlines()
                             if not line.startswith(('#include ', 'SHELL_CMD_EXPORT_ALIAS('))) + '\n'
    (Path(directory) / 'wave_player_cli_controls_under_test.h').write_text(cli_controls)

    diagnostic = ''
    for signature in ('static int parse_unsigned(', 'static void format_gain_db(',
                      'static int parse_gain_db(', 'static int cmd_audio_channel('):
        begin = control_source.index(signature)
        finish = control_source.index('\n}\n', begin) + 3
        diagnostic += control_source[begin:finish] + '\n'
    (Path(directory) / 'audio_channel_cli_under_test.h').write_text(diagnostic)
    for pcm in (0, 1):
        for recording in (0, 1):
            output = Path(directory) / f'audio_channel_{pcm}_{recording}'
            pcm_option = ['-DCONFIG_BSP_AUDIO_PCM=1'] if pcm else []
            subprocess.run([*common, *pcm_option, f'-DCONFIG_WAVE_PLAYER_ENABLE_REC={recording}',
                            str(tests / 'test_audio_channel.c'), '-o', str(output)], check=True)
            run_sanitized(output, timeout=10)

    for recording in (0, 1):
        output = Path(directory) / ('legacy_controls_rec_' + str(recording))
        subprocess.run([*common, '-DCONFIG_WAVE_PLAYER_ENABLE_REC=' + str(recording),
                        '-DCONFIG_WAVE_PLAYER_ENABLE_REC_RB=1',
                        str(tests / 'test_legacy_controls.c'), *controls_build, '-lm', '-o', str(output)], check=True)
        run_sanitized(output, timeout=10)

    engine = tx_reload + '\n' + function_source(source, 'wave_player_stop_timeout_internal') + '\n'
    for name in ('wave_player_pcm_engine_snapshot', 'wave_player_pcm_engine_get_caps',
                 'wave_player_pcm_engine_attach', 'wave_player_pcm_engine_detach',
                 'wave_player_pcm_engine_capture_enable', 'wave_player_pcm_engine_configure',
                 'wave_player_pcm_engine_start_playback', 'wave_player_pcm_engine_resume_tx',
                 'wave_player_pcm_engine_stop', 'wave_player_pcm_engine_stop_io',
                 'wave_player_pcm_engine_write', 'wave_player_pcm_engine_flush', 'wave_player_pcm_engine_trim'):
        engine += function_source(source, name) + '\n'
    (Path(directory) / 'wave_player_pcm_engine_under_test.h').write_text(engine)
    for legacy_ring in (0, 1):
        output = Path(directory) / ('audio_pcm_sessions_' + str(legacy_ring))
        subprocess.run([*common, '-pthread', '-DCONFIG_BSP_AUDIO_PCM=1',
                        '-DCONFIG_WAVE_PLAYER_ENABLE_REC=1', '-DCONFIG_WAVE_PLAYER_REC_CHANNELS=4',
                        '-DCONFIG_WAVE_PLAYER_ENABLE_REC_RB=' + str(legacy_ring),
                        str(tests / 'test_audio_pcm.c'), *controls_build, '-lm', '-o', str(output)], check=True)
        run_sanitized(output, timeout=15)
    output = Path(directory) / 'codec_init_failures'
    subprocess.run([*common, '-include', 'stdio.h', str(tests / 'test_codec_init_failures.c'),
                    str(codec / 'audio_codec_es8388.c'), str(codec / 'audio_codec_es8389.c'),
                    str(codec / 'audio_codec_wm8978.c'), '-o', str(output)], check=True)
    run_sanitized(output, timeout=10)
    output = Path(directory) / 'jy6311_init'
    subprocess.run([*common, '-include', 'stdio.h', '-ffunction-sections', '-fdata-sections',
                    str(tests / 'test_jy6311_init.c'), str(codec / 'audio_codec_jy6311.c'),
                    '-Wl,--gc-sections', '-o', str(output)], check=True)
    run_sanitized(output, timeout=10)
    output = Path(directory) / 'es8311_width'
    subprocess.run([*common, '-include', 'stdio.h', '-ffunction-sections', '-fdata-sections',
                    str(tests / 'test_es8311_width.c'), '-Wl,--gc-sections', '-o', str(output)], check=True)
    run_sanitized(output, timeout=10)
    output = Path(directory) / 'mp3_lifecycle'
    subprocess.run([*common, '-pthread',
                    '-I' + str(player.parents[2] / 'components/multimedia/pvmp3dec/include'),
                    str(tests / 'test_mp3_lifecycle.c'), '-o', str(output)], check=True)
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
        for pcm in (0, 1):
            output = Path(directory) / f'playback_dma_{enabled}_{pcm}'
            pcm_option = ['-DCONFIG_BSP_AUDIO_PCM=1'] if pcm else []
            subprocess.run([*common, *pcm_option, str(tests / 'test_playback_dma.c'), '-o', str(output)], check=True)
            run_sanitized(output, timeout=10)

result = {'status': 'passed', 'positive_sanitizer_executables': len(executables),
          'executables': executables, 'expected_sanitizer_failures': 1,
          'sanitizer_failfast': True, 'ownership_audit': ownership_audit}
if args.report_json:
    args.report_json.write_text(json.dumps(result, indent=2) + '\n')
print(f'ASan/UBSan executables passed: {len(executables)}; expected fail-fast diagnostic: 1')
