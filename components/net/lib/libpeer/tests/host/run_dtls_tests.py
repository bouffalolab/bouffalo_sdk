#!/usr/bin/env python3
"""Build SDK crypto libraries natively and test the actual libpeer DTLS path."""
import argparse
import os
from pathlib import Path
import shutil
import subprocess

host = Path(__file__).resolve().parent
sdk = host.parents[5]
parser = argparse.ArgumentParser()
parser.add_argument('--build-dir', type=Path, default=host / 'build_dtls_host')
parser.add_argument('--mbedtls-version', choices=('2', '3'), default='2')
args = parser.parse_args()
cc = os.environ.get('CC', 'cc')
build = args.build_dir.resolve()
mbedtls = sdk / 'components/crypto/mbedtls' / ('mbedtls' if args.mbedtls_version == '2' else 'mbedtls_v3')
srtp = sdk / 'components/net/lib/srtp/libsrtp'
subprocess.run([os.environ.get('PYTHON', 'python3'),
                str(host / 'run_diagnostics_api_tests.py'),
                '--mbedtls-include', str(mbedtls / 'include')], check=True)
subprocess.run([os.environ.get('PYTHON', 'python3'),
                str(host / 'run_audio_api_tests.py'),
                '--mbedtls-include', str(mbedtls / 'include')], check=True)

peer = sdk / 'components/net/lib/libpeer/src'
flags = '-g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer'
definitions = ['-DMBEDTLS_SSL_DTLS_SRTP',
               '-DCONFIG_MBEDTLS_2_X=' + ('1' if args.mbedtls_version == '2' else '0')]
shim = build / 'include'
(shim / 'lwip').mkdir(parents=True, exist_ok=True)
(shim / 'lwip/inet.h').write_text('#include <arpa/inet.h>\n#include <netinet/in.h>\n')
subprocess.run(['cmake', '-S', str(mbedtls), '-B', str(build / 'mbedtls'),
                '-DCMAKE_C_COMPILER=' + cc,
                '-DENABLE_PROGRAMS=OFF', '-DENABLE_TESTING=OFF', '-DMBEDTLS_FATAL_WARNINGS=OFF',
                '-DCMAKE_C_FLAGS=-DMBEDTLS_SSL_DTLS_SRTP ' + flags], check=True)
subprocess.run(['cmake', '--build', str(build / 'mbedtls'), '-j8'], check=True)
subprocess.run(['cmake', '-S', str(srtp), '-B', str(build / 'srtp'),
                '-DCMAKE_C_COMPILER=' + cc,
                '-DTEST_APPS=OFF', '-DCMAKE_C_FLAGS=' + flags + ' -I' + str(shim)], check=True)
# The SDK fork includes srtp_config.h; upstream CMake emits config.h.
shutil.copyfile(build / 'srtp/config.h', build / 'srtp/srtp_config.h')
subprocess.run(['cmake', '--build', str(build / 'srtp'), '-j8'], check=True)
output = build / 'dtls_transport'
subprocess.run([cc, '-std=c11', '-D_DEFAULT_SOURCE', *definitions,
                *flags.split(), '-fno-pie', '-no-pie', '-ffunction-sections', '-fdata-sections',
                '-I' + str(shim), '-I' + str(peer), '-I' + str(mbedtls / 'include'),
                '-I' + str(sdk / 'components/net/lib/srtp/port'),
                '-I' + str(srtp / 'include'), '-I' + str(srtp / 'crypto/include'),
                str(host / 'test_dtls_transport.c'), str(peer / 'dtls_srtp.c'),
                str(peer / 'socket.c'), str(peer / 'address.c'),
                str(build / 'mbedtls/library/libmbedtls.a'),
                str(build / 'mbedtls/library/libmbedx509.a'),
                str(build / 'mbedtls/library/libmbedcrypto.a'), str(build / 'srtp/libsrtp2.a'),
                '-Wl,--gc-sections', '-lm', '-o', str(output)], check=True)
subprocess.run([str(output)], check=True, timeout=20)
policy = build / 'dtls_policy'
subprocess.run([cc, '-std=c11', '-D_DEFAULT_SOURCE', *definitions,
                *flags.split(), '-fno-pie', '-no-pie', '-ffunction-sections', '-fdata-sections',
                '-I' + str(shim), '-I' + str(peer), '-I' + str(mbedtls / 'include'),
                '-I' + str(sdk / 'components/net/lib/srtp/port'),
                '-I' + str(srtp / 'include'), '-I' + str(srtp / 'crypto/include'),
                str(host / 'test_dtls_policy.c'), str(peer / 'socket.c'), str(peer / 'address.c'),
                str(build / 'mbedtls/library/libmbedtls.a'),
                str(build / 'mbedtls/library/libmbedx509.a'),
                str(build / 'mbedtls/library/libmbedcrypto.a'), str(build / 'srtp/libsrtp2.a'),
                '-Wl,--gc-sections', '-lm', '-o', str(policy)], check=True)
subprocess.run([str(policy)], check=True, timeout=10)
receive = build / 'peer_receive'
subprocess.run([cc, '-std=c11', '-D_DEFAULT_SOURCE', *definitions,
                '-DCONFIG_AUDIO_BUFFER_SIZE=0',
                *flags.split(), '-fno-pie', '-no-pie', '-ffunction-sections', '-fdata-sections',
                '-I' + str(peer), '-I' + str(mbedtls / 'include'),
                '-I' + str(sdk / 'components/net/lib/srtp/port'),
                str(host / 'test_peer_receive.c'), str(peer / 'rtp.c'), str(peer / 'dtls_srtp.c'),
                '-Wl,--gc-sections', '-o', str(receive)], check=True)
subprocess.run([str(receive)], check=True, timeout=5)
# A data-only consumer keeps its SCTP queue enabled but disables both media
# queues. Compile with the same unused-variable failure policy as strict builds.
subprocess.run([cc, '-std=c11', '-D_DEFAULT_SOURCE', *definitions,
                '-DCONFIG_AUDIO_BUFFER_SIZE=0', '-DCONFIG_VIDEO_BUFFER_SIZE=0',
                '-DCONFIG_DATA_BUFFER_SIZE=65536', '-Werror=unused-variable',
                '-I' + str(peer), '-I' + str(mbedtls / 'include'),
                '-I' + str(sdk / 'components/net/lib/srtp/port'),
                '-c', str(peer / 'peer_connection.c'), '-o', str(build / 'data_only.o')], check=True)
for nonblocking, diagnostics, packet_info in ((0,0,0), (1,0,0), (0,1,0), (1,1,0),
                                             (0,0,1), (1,1,1)):
    burst = build / f'rtp_burst_{nonblocking}_{diagnostics}_{packet_info}'
    subprocess.run([cc, '-std=c11', '-D_DEFAULT_SOURCE', *definitions,
                    '-DCONFIG_PEER_NONBLOCKING_RX=' + str(nonblocking),
                    '-DCONFIG_LIBPEER_RTP_PACKET_INFO=' + str(packet_info),
                    '-DCONFIG_LIBPEER_MEDIA_DIAGNOSTICS=' + str(diagnostics),
                    '-DCONFIG_AUDIO_BUFFER_SIZE=0', '-DCONFIG_VIDEO_BUFFER_SIZE=0',
                    '-DCONFIG_DATA_BUFFER_SIZE=0',
                    *flags.split(), '-fno-pie', '-no-pie', '-ffunction-sections', '-fdata-sections',
                    '-I' + str(peer), '-I' + str(mbedtls / 'include'),
                    '-I' + str(sdk / 'components/net/lib/srtp/port'),
                    str(host / 'test_rtp_burst.c'), str(peer / 'rtp.c'), str(peer / 'rtcp.c'),
                    str(peer / 'media_diagnostics.c'),
                    '-Wl,--gc-sections', '-o', str(burst)], check=True)
    subprocess.run([str(burst)], check=True, timeout=5)
offer = build / 'offer_sdp'
subprocess.run([cc, '-std=c11', '-D_DEFAULT_SOURCE', *definitions,
                *flags.split(), '-fno-pie', '-no-pie', '-ffunction-sections', '-fdata-sections',
                '-I' + str(peer), '-I' + str(mbedtls / 'include'),
                '-I' + str(sdk / 'components/net/lib/srtp/port'),
                str(host / 'test_offer_sdp.c'), str(peer / 'sdp.c'),
                '-Wl,--gc-sections', '-o', str(offer)], check=True)
subprocess.run([str(offer)], check=True, timeout=5)
for name in ('sctp_collision', 'sctp_reorder'):
    output = build / name
    subprocess.run([cc, '-std=c11', '-D_DEFAULT_SOURCE', *definitions,
                    '-DCONFIG_USE_USRSCTP=0', *flags.split(), '-fno-pie', '-no-pie',
                    '-I' + str(peer), '-I' + str(mbedtls / 'include'),
                    '-I' + str(sdk / 'components/net/lib/srtp/port'),
                    str(host / ('test_' + name + '.c')), str(peer / 'sctp.c'),
                    '-o', str(output)], check=True)
    subprocess.run([str(output)], check=True, timeout=5)

output = build / 'ice_transport'
subprocess.run([cc, '-std=c11', '-D_DEFAULT_SOURCE', *definitions,
                '-DCONFIG_USE_USRSCTP=0', *flags.split(), '-fno-pie', '-no-pie',
                '-ffunction-sections', '-fdata-sections',
                '-I' + str(peer), '-I' + str(mbedtls / 'include'),
                '-I' + str(sdk / 'components/net/lib/srtp/port'),
                '-I' + str(sdk / 'components/net/lib/webrtc_peer/include'),
                str(host / 'test_ice_transport.c'),
                *[str(peer / (name + '.c')) for name in
                  ('ice', 'address', 'socket', 'utils', 'stun', 'turn', 'dtls_srtp',
                   'sctp', 'buffer', 'rtp', 'rtcp', 'sdp', 'peer')],
                str(build / 'mbedtls/library/libmbedtls.a'),
                str(build / 'mbedtls/library/libmbedx509.a'),
                str(build / 'mbedtls/library/libmbedcrypto.a'), str(build / 'srtp/libsrtp2.a'),
                '-Wl,--gc-sections', '-lm', '-o', str(output)], check=True)
subprocess.run([str(output)], check=True, timeout=30)

output = build / 'utils_random'
subprocess.run([cc, '-std=c11', '-D_DEFAULT_SOURCE', *definitions,
                *flags.split(), '-Wall', '-Wextra', '-Werror', '-fno-pie', '-no-pie',
                '-I' + str(peer), '-I' + str(mbedtls / 'include'),
                str(host / 'test_utils_random.c'),
                str(build / 'mbedtls/library/libmbedcrypto.a'),
                '-o', str(output)], check=True)
subprocess.run([str(output)], check=True, timeout=10)

output = build / 'dtls_send_pressure'
subprocess.run([cc, '-std=c11', '-D_DEFAULT_SOURCE', *definitions,
                *flags.split(), '-fno-pie', '-no-pie', '-ffunction-sections', '-fdata-sections',
                '-I' + str(peer), '-I' + str(mbedtls / 'include'),
                '-I' + str(sdk / 'components/net/lib/srtp/port'),
                '-I' + str(srtp / 'include'), '-I' + str(srtp / 'crypto/include'),
                str(host / 'test_dtls_send_pressure.c'), str(peer / 'dtls_srtp.c'),
                str(peer / 'socket.c'), str(peer / 'address.c'),
                str(build / 'mbedtls/library/libmbedtls.a'),
                str(build / 'mbedtls/library/libmbedx509.a'),
                str(build / 'mbedtls/library/libmbedcrypto.a'), str(build / 'srtp/libsrtp2.a'),
                '-Wl,--gc-sections', '-lm', '-o', str(output)], check=True)
subprocess.run([str(output)], check=True, timeout=15)
