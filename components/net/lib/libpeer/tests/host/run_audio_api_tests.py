#!/usr/bin/env python3
"""Link C99/C++11 consumers without SDK macros to optional audio APIs."""
import argparse
import os
from pathlib import Path
import subprocess
import tempfile

host = Path(__file__).resolve().parent
sdk = host.parents[5]
public = sdk / 'components/net/lib/webrtc_peer/include'
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--mbedtls-include', type=Path, required=True)
args = parser.parse_args()
cc, cxx = os.environ.get('CC', 'cc'), os.environ.get('CXX', 'c++')
flags = ['-g', '-O1', '-Wall', '-Wextra', '-Werror', '-fsanitize=address,undefined',
         '-fno-omit-frame-pointer', '-fno-pie', '-no-pie', '-ffunction-sections', '-fdata-sections']
includes = ['-I' + str(d) for d in (public, sdk / 'components/net/lib/libpeer/src',
                                    sdk / 'components/net/lib/srtp/port', args.mbedtls_include)]
fixture = host / 'test_audio_api.c'
with tempfile.TemporaryDirectory(prefix='audio-api-') as directory:
    build = Path(directory)
    for diagnostics, packet_info in ((0,0), (0,1), (1,0), (1,1)):
        enabled = f'{diagnostics}_{packet_info}'
        library = build / f'library_{enabled}.o'
        subprocess.run([cc, '-std=c11', '-D_DEFAULT_SOURCE', *flags, *includes,
                        '-DAUDIO_API_FIXTURE', '-DCONFIG_LIBPEER_RTP_PACKET_INFO=' + str(packet_info),
                        '-DCONFIG_LIBPEER_MEDIA_DIAGNOSTICS=' + str(diagnostics),
                        '-c', str(fixture), '-o', str(library)], check=True)
        for compiler, standard, language in ((cc, 'c99', 'c'), (cxx, 'c++11', 'c++')):
            consumer = build / f'consumer_{enabled}_{language}.o'
            subprocess.run([compiler, '-std=' + standard, '-x', language, *flags,
                            '-I' + str(public), '-c', str(fixture), '-o', str(consumer)], check=True)
            output = build / f'api_{enabled}_{language}'
            subprocess.run([compiler, *flags, str(consumer), str(library),
                            '-Wl,--gc-sections', '-o', str(output)], check=True)
            subprocess.run([str(output)], check=True, timeout=10)
