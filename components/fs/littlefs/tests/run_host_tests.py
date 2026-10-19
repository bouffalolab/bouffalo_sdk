#!/usr/bin/env python3
"""Exercise production LittleFS and KV against emulated flash with fault injection."""
import os
from pathlib import Path
import subprocess
import tempfile

tests = Path(__file__).resolve().parent
component = tests.parent
with tempfile.TemporaryDirectory(prefix='lfs-kv-tests-') as build:
    binary = str(Path(build) / 'test_kv')
    subprocess.run([
        os.environ.get('CC', 'cc'), '-std=c11', '-g', '-O1', '-Wall', '-Wextra',
        '-Wno-sign-compare', '-fsanitize=address,undefined', '-fno-omit-frame-pointer',
        '-fno-pie', '-no-pie', '-DLFS_NO_DEBUG', '-DLFS_NO_WARN', '-DLFS_NO_ERROR',
        '-I' + str(tests / 'stubs'), '-I' + str(component / 'littlefs'),
        '-I' + str(component / 'port'), '-I' + str(component / 'lfs_kv'),
        str(tests / 'test_kv.c'), str(component / 'lfs_kv/lfs_kv.c'),
        str(component / 'littlefs/lfs.c'), str(component / 'littlefs/lfs_util.c'),
        '-o', binary,
    ], check=True)
    subprocess.run([binary], check=True, timeout=30)
