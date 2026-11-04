#!/usr/bin/env python3
"""Compare diag=off RTP machine code with a caller-selected Git baseline.

Requires Git history, a native C compiler and objcopy. Does not modify checkout.
"""
import argparse
import hashlib
import io
import json
import os
from pathlib import Path
import subprocess
import tarfile
import tempfile

sdk = Path(__file__).resolve().parents[6]
relative = Path('components/net/lib/libpeer/src')
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--baseline', required=True, help='Commit before media diagnostics')
p.add_argument('--cc', default=os.environ.get('CC', 'cc'))
p.add_argument('--objcopy', default=os.environ.get('OBJCOPY', 'objcopy'))
args = p.parse_args()
archive = subprocess.check_output(['git', 'archive', args.baseline, str(relative)], cwd=sdk)
with tempfile.TemporaryDirectory(prefix='diagnostic-overhead-') as directory:
    build = Path(directory)
    # Read regular source files only; no archive paths or symlinks are trusted.
    with tarfile.open(fileobj=io.BytesIO(archive)) as tar:
        for member in tar.getmembers():
            if member.isfile():
                name = Path(member.name)
                assert not name.is_absolute() and '..' not in name.parts
                target = build / 'baseline' / name
                target.parent.mkdir(parents=True, exist_ok=True)
                target.write_bytes(tar.extractfile(member).read())
    data = {}
    for name, source, definitions in (
        ('baseline', build / 'baseline' / relative, []),
        ('default', sdk / relative, []),
        ('disabled', sdk / relative, ['-DCONFIG_LIBPEER_MEDIA_DIAGNOSTICS=0']),
    ):
        obj, code = build / (name + '.o'), build / (name + '.text')
        command = [args.cc, '-std=c11', '-D_DEFAULT_SOURCE', '-O2',
                   '-I' + str(source), *definitions, '-c', str(source / 'rtp.c'), '-o', str(obj)]
        subprocess.run(command, check=True)
        subprocess.run([args.objcopy, '-O', 'binary', '--only-section=.text', str(obj), str(code)], check=True)
        data[name] = code.read_bytes()
    assert data['baseline'] and data['baseline'] == data['default'] == data['disabled']
    print(json.dumps({'compiler': subprocess.check_output([args.cc, '--version'], text=True).splitlines()[0],
                      'baseline': args.baseline, 'flags': '-std=c11 -D_DEFAULT_SOURCE -O2',
                      'text_bytes': {name: len(code) for name, code in data.items()},
                      'text_sha256': hashlib.sha256(data['baseline']).hexdigest(),
                      'identical': True}, indent=2))
