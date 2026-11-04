#!/usr/bin/env python3
"""RTSP production server: sanitizers and actual TCP/UDP protocol exchanges.

Requires a C compiler and Pillow (test JPEG generation only). --build-dir
retains server_host for playback with an independent RTSP implementation.
"""
import argparse
import os
from pathlib import Path
import select
import socket
import struct
import subprocess
import tempfile
import time
from PIL import Image

component = Path(__file__).resolve().parents[1]


class Client:
    def __init__(self, port):
        self.sock = socket.create_connection(('127.0.0.1', port), timeout=3)
        self.sock.settimeout(3)
        self.port, self.cseq, self.buf = port, 0, b''
        self.media = []

    def send(self, method, path='/live', headers='', body=b'', split=False):
        self.cseq += 1
        uri = '*' if path == '*' else f'rtsp://127.0.0.1:{self.port}{path}'
        request = (f'{method} {uri} RTSP/1.0\r\nCSeq: {self.cseq}\r\n'
                   f'{headers}Content-Length: {len(body)}\r\n\r\n').encode() + body
        if split:
            for start in range(0, len(request), 7):
                self.sock.sendall(request[start:start + 7])
                time.sleep(.001)
        else:
            self.sock.sendall(request)

    def item(self):
        while True:
            if self.buf.startswith(b'$') and len(self.buf) >= 4:
                n = int.from_bytes(self.buf[2:4], 'big')
                if len(self.buf) >= 4 + n:
                    channel, data = self.buf[1], self.buf[4:4 + n]
                    self.buf = self.buf[4 + n:]
                    return 'rtp', channel, data
            elif b'\r\n\r\n' in self.buf:
                end = self.buf.index(b'\r\n\r\n') + 4
                header = self.buf[:end].decode()
                fields = dict((line.split(':', 1)[0].lower(), line.split(':', 1)[1].strip())
                              for line in header.split('\r\n')[1:] if ':' in line)
                n = int(fields.get('content-length', 0))
                if len(self.buf) >= end + n:
                    body = self.buf[end:end + n]
                    self.buf = self.buf[end + n:]
                    return 'response', int(header.split()[1]), fields, body
            data = self.sock.recv(8192)
            if not data:
                raise EOFError
            self.buf += data

    def response(self, status=200):
        while True:
            item = self.item()
            if item[0] == 'response':
                assert item[1] == status, item
                return item
            self.media.append(item)

    def request(self, *args, status=200, **kwargs):
        self.send(*args, **kwargs)
        return self.response(status)

    def close(self):
        self.sock.close()


def tcp(port, scan):
    c = Client(port)
    c.request('OPTIONS', '*', split=True)
    response = c.request('DESCRIBE', split=True)
    assert b'JPEG/90000' in response[3] and b'PCMA/8000/1' in response[3]
    r = c.request('SETUP', '/live/track0', 'Transport: RTP/AVP/TCP;unicast;interleaved=254-255\r\n')
    session = r[2]['session'].split(';')[0]
    c.request('SETUP', '/live/track1', f'Session: {session}\r\nTransport: RTP/AVP/TCP;unicast;interleaved=0-1\r\n')
    c.request('PLAY', status=454)
    c.request('PLAY', headers=f'Session: {session}\r\n')
    extra = Client(port)
    assert extra.sock.recv(1) == b''
    extra.close()
    frames, audio, reports, offsets = 0, 0, 0, {}
    sequences = {}
    until = time.monotonic() + 5.4
    while time.monotonic() < until:
        item = c.item()
        assert item[0] == 'rtp', item
        channel, p = item[1:]
        if channel in (1, 255):
            assert p[1] == 200 and len(p) >= 48
            assert int.from_bytes(p[20:24], 'big') > 0
            reports += 1
            continue
        assert p[0] == 0x80
        seq, ts, ssrc = struct.unpack('!HII', p[2:12])
        if channel in sequences:
            assert seq == (sequences[channel] + 1) % 65536
        sequences[channel] = seq
        if channel == 0:
            assert p[1] & 127 == 8 and len(p) == 172
            assert set(p[12:]) == {0xd5}
            audio += 1
        else:
            assert channel == 254 and p[1] & 127 == 26
            offset = int.from_bytes(p[13:16], 'big')
            hdr = 8 if offset else 140
            current = offsets.setdefault(ts, bytearray())
            assert offset == len(current)
            current.extend(p[12 + hdr:])
            if p[1] & 128:
                assert bytes(current) == scan
                del offsets[ts]
                frames += 1
    assert frames > 100 and audio > 200 and reports >= 2, (frames, audio, reports)
    # Fragment a receiver report before a pipelined body-bearing keepalive.
    rr = b'$\xff\x00\x08\x80\xc9\x00\x01\x12\x34\x56\x78'
    c.sock.sendall(rr[:6]); time.sleep(.01); c.sock.sendall(rr[6:])
    c.send('GET_PARAMETER', headers=f'Session: {session}\r\n', body=b'ping')
    c.send('OPTIONS', '*')
    c.response(); c.response()
    c.request('PAUSE', headers=f'Session: {session}\r\n')
    c.request('PLAY', headers=f'Session: {session}\r\n')
    c.request('TEARDOWN', headers=f'Session: {session}\r\n')
    c.close()
    print(f'TCP: {frames} exact JPEGs, {audio} continuous audio packets, {reports} sender reports')


def udp(port):
    c = Client(port)
    sockets = [socket.socket(socket.AF_INET, socket.SOCK_DGRAM) for _ in range(4)]
    for s in sockets:
        s.bind(('127.0.0.1', 0))
    session = ''
    for track in range(2):
        a, b = sockets[track * 2].getsockname()[1], sockets[track * 2 + 1].getsockname()[1]
        headers = (f'Session: {session}\r\n' if session else '')
        headers += f'Transport: RTP/AVP;unicast;client_port={a}-{b}\r\n'
        r = c.request('SETUP', f'/live/track{track}', headers)
        session = r[2]['session'].split(';')[0]
    c.request('PLAY', headers=f'Session: {session}\r\n')
    video, audio = 0, 0
    until = time.monotonic() + 1
    while time.monotonic() < until:
        for s in select.select(sockets, [], [], .1)[0]:
            p = s.recv(1500)
            if s is sockets[0]:
                assert p[1] & 127 == 26
                video += bool(p[1] & 128)
            elif s is sockets[2]:
                assert p[1] & 127 == 8 and len(p) == 172
                audio += 1
    assert video > 15 and audio > 30, (video, audio)
    c.request('TEARDOWN', headers=f'Session: {session}\r\n')
    c.close()
    for s in sockets:
        s.close()
    print(f'UDP: {video} video frames, {audio} audio packets')


def negative(port):
    for request in (b'OPTIONS * RTSP/1.0\r\n\r\n',
                    b'OPTIONS * RTSP/1.0\r\nCSeq: 1\r\nContent-Length: +12\r\n\r\n',
                    b'OPTIONS * RTSP/1.0\r\nCSeq: 1\r\nContent-Length: 0\r\nContent-Length: 0\r\n\r\n',
                    b'OPTIONS * RTSP/1.0\r\nX: ' + b'a' * 4200):
        c = Client(port); c.sock.sendall(request); c.response(400); c.close()
    for _ in range(20):
        c = Client(port); c.request('OPTIONS', '*'); c.close(); time.sleep(.005)
    c = Client(port)
    c.request('DESCRIBE', '/missing', status=404)
    c.request('SETUP', '/live/track0', 'Transport: RTP/AVP/TCP;unicast;interleaved=256-257\r\n', status=461)
    c.close()
    print('Malformed lengths/requests, bad transport, extra client and 20 reconnects passed')


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build-dir')
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix='rtsp-tests-') as temporary:
        build = Path(args.build_dir or temporary)
        build.mkdir(parents=True, exist_ok=True)
        # Nontrivial baseline JPEG with the standard Huffman tables; avoid
        # making a serializer test depend on WebRTC's custom-Huffman fixtures.
        image = Image.new('RGB', (320, 240))
        image.putdata([((x * 11) % 256, (y * 7) % 256, (x + y) % 256)
                       for y in range(240) for x in range(320)])
        fixture = build / 'source.jpg'
        image.save(fixture, quality=65, subsampling=2, optimize=False)
        data = fixture.read_bytes()
        pos = data.index(b'\xff\xda')
        scan = data[pos + 2 + int.from_bytes(data[pos + 2:pos + 4], 'big'):-2]
        common = [os.environ.get('CC', 'cc'), '-std=c99', '-D_POSIX_C_SOURCE=200809L',
                  '-DRTSP_SERVER_POSIX=1', '-Wall', '-Wextra', '-Werror', '-O1', '-g',
                  '-fsanitize=address,undefined', '-fno-sanitize-recover=all', '-fno-pie', '-no-pie',
                  '-I' + str(component / 'inc')]
        jpeg = str(component / 'src/rtsp_server_jpeg.c')
        subprocess.run(common + [str(component / 'tests/test_server.c'), jpeg,
                                '-o', str(build / 'test_server')], check=True)
        subprocess.run([str(build / 'test_server'), str(fixture)], check=True)
        subprocess.run(common + [str(component / 'tests/server_host.c'),
                                str(component / 'src/rtsp_server.c'), jpeg,
                                '-o', str(build / 'server_host')], check=True)
        process = subprocess.Popen([str(build / 'server_host'), str(fixture)],
                                   stdout=subprocess.PIPE, text=True)
        try:
            port = int(process.stdout.readline().split()[1])
            tcp(port, scan)
            udp(port)
            negative(port)
        finally:
            process.terminate()
            rest, _ = process.communicate(timeout=5)
            print(rest, end='')
        assert process.returncode == 0, process.returncode
        print('RTSP host tests passed (ASan/UBSan)')


if __name__ == '__main__':
    main()
