#!/usr/bin/env python3
"""Resolve shared audio options and compile their generated C configuration."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile

sdk = Path(__file__).resolve().parents[4]
sys.path.insert(0, str(sdk / 'tools/kconfig'))
from config_seed import (config_fragment, load_kconfig, normalize_defconfig,
                         resolved_values, set_selector_environment, write_header)

os.environ.update(BL_SDK_BASE=str(sdk), CHIP='bl616cl',
                  BOARD='bl616cl_ave_v10', CPU_ID='', CPU_MODEL='')
base = {'CONFIG_BSP_AUDIO_CODEC': 'y', 'CONFIG_BSP_AUDIO_PLAYER': 'y',
        'CONFIG_MULTIMEDIA': 'y', 'CONFIG_WAVE_PLAYER_ENABLE_REC': 'y'}


def resolve(entry, values):
    os.environ.update(CHIP='bl616cl', BOARD='bl616cl_ave_v10',
                      CPU_ID='', CPU_MODEL='')
    set_selector_environment(values)
    config = load_kconfig(str(sdk / entry))
    config.warn_assign_undef = True
    fragment = config_fragment(config, values)
    try:
        config.load_config(fragment)
        config.write_config(os.devnull, save_old=False)
    finally:
        os.unlink(fragment)
    return config


profiles = (
    ({'CONFIG_CHIP': 'bl618dg', 'CONFIG_BOARD': 'bl618dg_mstar',
      'CONFIG_CPU_ID': 'ap', 'CONFIG_CPU_MODEL': 'b0'},
     (0x08, 1, 1, 4, 1, 2, 4)),
    ({}, (2, 1, 0, 0, 0, 2, 2)),
    ({'CONFIG_WAVE_PLAYER_OUT_CHANNELS': '1', 'CONFIG_WAVE_PLAYER_REC_CHANNELS': '1'},
     (1, 0, 0, 0, 0, 1, 1)),
    ({'CONFIG_BOARD': 'audio_test_no_defaults'}, (1, 0, 0, 0, 0, 2, 2)),
    ({'CONFIG_BOARD': '', 'CONFIG_WAVE_PLAYER_REC_CHANNELS': '2'},
     (1, 0, 0, 0, 0, 2, 2)),
    ({'CONFIG_WAVE_PLAYER_REC_CHANNELS': '2',
      'CONFIG_BOARD_AUDIO_CAPTURE_MIC_MASK': '0x01',
      'CONFIG_BOARD_AUDIO_CAPTURE_REF_MASK': '0x02'}, (1, 2, 0, 0, 0, 2, 2)),
    ({'CONFIG_AUDIO_CODEC_JY6316_ENABLE': 'y'}, (0x0c, 1, 1, 4, 1, 2, 4)),
    ({'CONFIG_AUDIO_CODEC_JY6316_ENABLE': 'y', 'CONFIG_WAVE_PLAYER_ENABLE_REC': 'n',
      'CONFIG_WAVE_PLAYER_OUT_CHANNELS': '1'}, (0x0c, 1, 1, 4, 1, 1, 4)),
    ({'CONFIG_AUDIO_CODEC_JY6316_ENABLE': 'y',
      'CONFIG_WAVE_PLAYER_I2S_DSP_LR_MERGE': 'n'}, (0x0c, 1, 1, 4, 0, 2, 4)),
    ({'CONFIG_I2S_DSP_MODE': 'y', 'CONFIG_WAVE_PLAYER_I2S_TX_SLOTS': '6',
      'CONFIG_WAVE_PLAYER_REC_CHANNELS': '4',
      'CONFIG_BOARD_AUDIO_CAPTURE_MIC_MASK': '0x03',
      'CONFIG_BOARD_AUDIO_CAPTURE_REF_MASK': '0x08'}, (3, 8, 1, 6, 1, 2, 4)),
)
names = ('CONFIG_BOARD_AUDIO_CAPTURE_MIC_MASK', 'CONFIG_BOARD_AUDIO_CAPTURE_REF_MASK',
         'WAVE_PLAYER_I2S_DSP_MODE', 'WAVE_PLAYER_I2S_TX_SLOTS',
         'WAVE_PLAYER_I2S_DSP_LR_MERGE', 'WAVE_PLAYER_OUT_CHANNELS',
         'CONFIG_WAVE_PLAYER_REC_CHANNELS')
with tempfile.TemporaryDirectory(prefix='audio-kconfig-') as directory:
    header = Path(directory) / 'config.h'
    for entry in ('Kconfig', 'examples/peripherals/i2s/i2s_codec/Kconfig'):
        for overrides, expected in profiles:
            config = resolve(entry, {**base, **overrides})
            assert not config.warnings, config.warnings
            for name in ('I2S_DSP_MODE', 'WAVE_PLAYER_I2S_TX_SLOTS',
                         'WAVE_PLAYER_I2S_DSP_LR_MERGE'):
                assert len(config.syms[name].nodes) == 1, name
            write_header(str(header), resolved_values(config))
            source = '#include "wave_player.h"\n'
            for name, value in zip(names, expected):
                source += f'_Static_assert({name} == {value}, "{name}");\n'
            subprocess.run([os.environ.get('CC', 'cc'), '-std=c11', '-Werror',
                            '-include', str(header),
                            '-I' + str(sdk / 'bsp/common/audio_player'),
                            '-I' + str(sdk / 'bsp/common/audio_codec'),
                            '-x', 'c', '-fsyntax-only', '-'], input=source,
                           text=True, check=True)
    invalid = resolve('Kconfig', {**base, 'CONFIG_I2S_DSP_MODE': 'y',
                                 'CONFIG_WAVE_PLAYER_I2S_TX_SLOTS': '8'})
    assert invalid.warnings and invalid.syms['WAVE_PLAYER_I2S_TX_SLOTS'].str_value != '8'
    example = sdk / 'examples/peripherals/i2s/i2s_codec'
    example_values = normalize_defconfig(example / 'defconfig')
    example_profiles = [
        ({}, (2, 2, 2, 1)),
        ({'CONFIG_AUDIO_CODEC_JY6316_ENABLE': 'y'}, (2, 4, 0x0c, 1)),
        ({'CONFIG_WAVE_PLAYER_OUT_CHANNELS': '1',
          'CONFIG_WAVE_PLAYER_REC_CHANNELS': '1'}, (1, 1, 1, 0)),
    ]
    for chip, board in (('bl616', 'bl616dk'), ('bl618dg', 'bl618dgdk'),
                        ('bl616cl', 'bl616cldk')):
        for overrides, expected in (
                ({}, (1, 1, 1, 0)),
                ({'CONFIG_AUDIO_CODEC_JY6316_ENABLE': 'y'}, (2, 4, 0x0c, 1)),
                ({'CONFIG_WAVE_PLAYER_OUT_CHANNELS': '2',
                  'CONFIG_WAVE_PLAYER_REC_CHANNELS': '2'}, (2, 2, 1, 0))):
            example_profiles.append(({'CONFIG_CHIP': chip, 'CONFIG_BOARD': board,
                                      **overrides}, expected))
        common = resolve('Kconfig', {**base, 'CONFIG_CHIP': chip, 'CONFIG_BOARD': board})
        assert not common.warnings, common.warnings
        assert common.syms['WAVE_PLAYER_OUT_CHANNELS'].str_value == '2'
        assert common.syms['WAVE_PLAYER_REC_CHANNELS'].str_value == '2'
    for overrides, expected in example_profiles:
        config = resolve(str(example / 'Kconfig'), {**example_values, **overrides})
        assert not config.warnings, config.warnings
        for name, value in zip(('WAVE_PLAYER_OUT_CHANNELS', 'WAVE_PLAYER_REC_CHANNELS',
                                'BOARD_AUDIO_CAPTURE_MIC_MASK',
                                'BOARD_AUDIO_CAPTURE_REF_MASK'), expected):
            assert int(config.syms[name].str_value, 0) == value, name
        assert config.syms['WAVE_PLAYER_OUT_SAMPLES'].str_value == '480'
    unavailable = resolve('Kconfig', {**base, 'CONFIG_MULTIMEDIA': 'n',
                                     'CONFIG_AUDIO_CODEC_JY6316_ENABLE': 'y'})
    assert not unavailable.warnings, unavailable.warnings
    assert unavailable.syms['WAVE_PLAYER_REC_CHANNELS'].str_value == ''
print('Shared audio Kconfig profiles, generated headers and LR merge disable passed')
