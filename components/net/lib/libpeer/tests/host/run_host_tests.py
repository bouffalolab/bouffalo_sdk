#!/usr/bin/env python3
"""Run production SCTP/buffer/ICE regressions without a solution or API keys."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile

host = Path(__file__).resolve().parent
peer = host.parents[1] / 'src'
flags = ['-std=c11', '-D_DEFAULT_SOURCE', '-g', '-O1', '-Wall', '-Wextra',
         '-Werror', '-Wno-unused-parameter', '-Wno-sign-compare',
         '-fsanitize=address,undefined', '-fno-omit-frame-pointer']
if sys.platform.startswith('linux'):
    flags += ['-fno-pie', '-no-pie']
with tempfile.TemporaryDirectory(prefix='libpeer-host-') as directory:
    common = [os.environ.get('CC', 'cc'), *flags, '-I' + str(peer)]
    for name in ('libpeer', 'sctp_review'):
        output = Path(directory) / name
        sources = [str(host / ('test_' + name + '.c'))]
        if name == 'libpeer':
            sources += [str(peer / 'sctp.c'), str(peer / 'buffer.c')]
        subprocess.run([*common, '-include', str(host / 'libpeer_stubs.h'),
                        *sources, '-o', str(output)], check=True)
        subprocess.run([str(output)], check=True, timeout=10)
    output = Path(directory) / 'sctp_small_tx_window'
    subprocess.run([*common, '-Wno-unused-function', '-DCONFIG_SCTP_TX_WINDOW=2',
                    '-DSCTP_SEND_LIMITS_ONLY', '-include', str(host / 'libpeer_stubs.h'),
                    str(host / 'test_sctp_review.c'), '-o', str(output)], check=True)
    subprocess.run([str(output)], check=True, timeout=10)
    compact = Path(directory) / 'sctp_small_rx_window'
    subprocess.run([*common, '-Wno-unused-function', '-DCONFIG_SCTP_RX_WINDOW=2',
                    '-DSCTP_RX_COMPACTION_ONLY', '-include', str(host / 'libpeer_stubs.h'),
                    str(host / 'test_sctp_review.c'), '-o', str(compact)], check=True)
    subprocess.run([str(compact)], check=True, timeout=10)
    # The reassembly prefix uses size_t, independent of a wire chunk's 16 bits.
    for message_size in (32768, 71680):
        output = Path(directory) / ('sctp_rx_message_' + str(message_size))
        subprocess.run([*common, '-Wno-unused-function', '-DCONFIG_SCTP_RX_WINDOW=2',
                        '-DCONFIG_SCTP_MESSAGE_SIZE=' + str(message_size),
                        '-DSCTP_RX_COMPACTION_ONLY', '-include', str(host / 'libpeer_stubs.h'),
                        str(host / 'test_sctp_review.c'), '-o', str(output)], check=True)
        subprocess.run([str(output)], check=True, timeout=10)
    rejected = subprocess.run([os.environ.get('CC', 'cc'), '-I' + str(peer),
                               '-DCONFIG_SCTP_RX_WINDOW=1', '-E', '-x', 'c', '-'],
                              input='#include "config.h"\n', text=True, capture_output=True)
    assert rejected.returncode != 0 and 'RX window in 2..32' in rejected.stderr
    print('Unsupported single-slot receive window rejected at compile time', flush=True)
    output = Path(directory) / 'agent_lifecycle'
    subprocess.run([*common, '-Wno-unused-variable', '-ffunction-sections',
                    '-fdata-sections', str(host / 'test_agent_lifecycle.c'),
                    str(peer / 'socket.c'), '-Wl,--gc-sections', '-o', str(output)], check=True)
    subprocess.run([str(output)], check=True, timeout=10)
