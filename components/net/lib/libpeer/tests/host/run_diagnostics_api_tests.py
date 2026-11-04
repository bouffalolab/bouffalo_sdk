#!/usr/bin/env python3
"""Compile a consumer without SDK config, then link it to debug on/off libraries."""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

host = Path(__file__).resolve().parent
sdk = host.parents[5]
peer = sdk / 'components/net/lib/libpeer/src'
public = sdk / 'components/net/lib/webrtc_peer/include'
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--mbedtls-include', type=Path, required=True)
args = p.parse_args()
cc, cxx = os.environ.get('CC', 'cc'), os.environ.get('CXX', 'c++')
flags = ['-g', '-O1', '-Wall', '-Wextra', '-Werror', '-fsanitize=address,undefined',
         '-fno-omit-frame-pointer', '-fno-pie', '-no-pie', '-ffunction-sections', '-fdata-sections']
includes = ['-I' + str(d) for d in (peer, public, args.mbedtls_include,
                                    sdk / 'components/net/lib/srtp/port')]
fixture = host / 'test_diagnostics_api.c'
with tempfile.TemporaryDirectory(prefix='diagnostics-api-') as directory:
    build = Path(directory)
    for enabled in (0, 1):
        library = build / f'library_{enabled}.o'
        subprocess.run([cc, '-std=c11', '-D_DEFAULT_SOURCE', *flags, *includes,
                        '-DDIAGNOSTICS_API_FIXTURE',
                        '-DCONFIG_LIBPEER_MEDIA_DIAGNOSTICS=' + str(enabled),
                        '-c', str(fixture), '-o', str(library)], check=True)
        for compiler, standard, language in ((cc, 'c99', 'c'), (cxx, 'c++11', 'c++')):
            # No -D arguments, no config header or forced includes on this TU.
            consumer = build / f'consumer_{enabled}_{language}.o'
            subprocess.run([compiler, '-std=' + standard, '-x', language, *flags,
                            '-I' + str(public), '-c', str(fixture), '-o', str(consumer)], check=True)
            output = build / f'api_{enabled}_{language}'
            subprocess.run([compiler, *flags, str(consumer), str(library),
                            '-Wl,--gc-sections', '-o', str(output)], check=True)
            result = subprocess.run([str(output)], check=True, timeout=10,
                                    text=True, stdout=subprocess.PIPE)
            print(result.stdout, end='', flush=True)
            expected = 'reads all fields' if enabled else 'disabled library'
            assert expected in result.stdout, 'Library capability differs from build configuration'

    # Fault-inject layout drift. Use copied headers to leave production sources
    # untouched. Even adding a field to BOTH structs must require a mapper edit.
    headers = build / 'headers'
    headers.mkdir()
    for source in list(peer.glob('*.h')) + list(public.glob('*.h')):
        shutil.copyfile(source, headers / source.name)
    native_original = (headers / 'media_diagnostics.h').read_text()
    public_original = (headers / 'webrtc_peer.h').read_text()
    for mode in ('native_extra', 'both_extra', 'public_reorder', 'native_width'):
        native, exported = native_original, public_original
        if mode in ('native_extra', 'both_extra'):
            native = native.replace('} RtpDiagnosticReport;', '  uint32_t extra;\n} RtpDiagnosticReport;')
        if mode == 'both_extra':
            exported = exported.replace('} webrtc_peer_rtp_diagnostics_t;',
                                         '    uint32_t extra;\n} webrtc_peer_rtp_diagnostics_t;')
        if mode == 'public_reorder':
            exported = exported.replace('uint32_t ssrc, clock_rate,', 'uint32_t clock_rate, ssrc,')
        if mode == 'native_width':
            native = native.replace('uint32_t jitter_us, max_arrival_gap_ms;',
                                     'uint64_t jitter_us; uint32_t max_arrival_gap_ms;')
        (headers / 'media_diagnostics.h').write_text(native)
        (headers / 'webrtc_peer.h').write_text(exported)
        result = subprocess.run([cc, '-std=c11', '-D_DEFAULT_SOURCE', *flags,
                                 '-I' + str(headers), *includes,
                                 '-DDIAGNOSTICS_API_FIXTURE', '-DCONFIG_LIBPEER_MEDIA_DIAGNOSTICS=1',
                                 '-c', str(fixture), '-o', str(build / 'rejected.o')],
                                text=True, capture_output=True)
        assert result.returncode and 'static assertion' in result.stderr, (mode, result.stderr)
        print('Diagnostic layout drift rejected: ' + mode, flush=True)
