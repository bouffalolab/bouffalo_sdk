#!/usr/bin/env python3
"""Check fixed-point echo startup, double talk and reset with ASan.

Run: python3 speexdsp/tests/run_host_tests.py
Fixed-point Speex uses intentional integer overflow and shifts; omit UBSan.
"""
import os
from pathlib import Path
import subprocess
import tempfile

speex = Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(prefix='speex-echo-tests-') as build:
    binary = str(Path(build) / 'test_echo')
    subprocess.run([
        os.environ.get('CC', 'cc'), '-std=c11', '-D_DEFAULT_SOURCE', '-g', '-O2',
        '-fsanitize=address', '-fno-omit-frame-pointer', '-fno-pie', '-no-pie',
        '-DFIXED_POINT', '-DUSE_KISS_FFT', '-DDISABLE_FLOAT_API',
        '-I' + str(speex / 'include'), '-I' + str(speex / 'include/speex'),
        str(speex / 'tests/test_echo.c'),
        *[str(speex / 'src' / (name + '.c')) for name in
          ('mdf', 'preprocess', 'fftwrap', 'filterbank', 'kiss_fft', 'kiss_fftr')],
        '-lm', '-o', binary,
    ], check=True)
    subprocess.run([binary], check=True, timeout=30)
